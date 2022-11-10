//
// Copyright 2022 Brian W. Hughes
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>
#include <functional>

#include "Executor.h"
#include "TaskState.h"
#include "FileSystem.h"


//
// ComputeChecksums
//

namespace {

Task MakeChecksumFinishLambda(TaskState *apState,
                              Executor *apEx,
                              RipsumOutput *apOut)
{
    return [apState, apEx, apOut](uint32_t aThreadNum) {
        apState->Finish();
        apOut->NotifyChecksumReady(apState->GetPath(), apState->GetChecksum());
        delete apState;
        apEx->ActivityDone();
    };
}

Task MakeReadAndHashLambda(TaskState *apState,
                           Executor *apEx,
                           RipsumOutput *apOut)
{
    return [apState, apEx, apOut](uint32_t aThreadNum) {
        apState->ReadBytes();
        if(apState->FileOk() == true) {
            apEx->AddTask(aThreadNum,
                          MakeReadAndHashLambda(apState, apEx, apOut));
            apState->AddBytesToHash();
        }
        else {
            apEx->AddTask(aThreadNum,
                          MakeChecksumFinishLambda(apState, apEx, apOut));
        }
    };
}

Task MakeComputeChecksumLambda(std::filesystem::path aP,
                               uint32_t aBlockSize,
                               Executor *apEx,
                               RipsumOutput *apOut)
{
    return [aP, aBlockSize, apEx, apOut](uint32_t aThreadNum) {
        TaskState *pState = new TaskState(aP, aBlockSize);
        pState->Init();
        apEx->AddTask(aThreadNum, MakeReadAndHashLambda(pState, apEx, apOut));
    };
}

}

void Executor::ComputeChecksums(const std::filesystem::path& aPath,
                                UserInput& aConfig,
                                RipsumOutput *apOut)
{
    FileSystem::FindFiles(aPath,
    [&](std::filesystem::path aP) {
        ActivityStarted();  // Matching ActivityDone() is in the final Task
        AddTask(MakeComputeChecksumLambda(aP, aConfig.mBlockSize,
                                          this, apOut));
    });
}


// ActivityStarted is called below to account for the main thread.  When
// the main thread calls Wait() we'll decrement it to signal worker threads
// that we don't have anything else to do
Executor::Executor(void): mtRunning{0} {
    ActivityStarted();
    // Create per-thread TaskLists 1st so vector can reallocate internally
    // without messing up running threads
    for(int i = 0; i < 4; i++) {
        mvThreadTasks.push_back(new TaskList);
    }
    for(int i = 0; i < 4; i++) {
        mlThreads.push_front(std::thread(&Executor::Worker, this, i));
    }
}


// Wait for all the work to complete.  If the main thread has called Wait()
// we assume it is also done doing stuff and call ActivityDone() to signal
// the worker threads that it's done.
void Executor::Wait(void) {
    using namespace std::chrono_literals;

    ActivityDone();
    while(mtRunning > 0 || !mAnyoneTasks.Empty()) {
        // Make tasklists first b/c vector will realocate and move
        // stuff and cause segfaults
        while(!mAnyoneTasks.Empty()) {
            std::this_thread::sleep_for(25ms);
        }
        while(mtRunning > 0) {
            std::this_thread::sleep_for(25ms);
        }
    }

    while(!mlThreads.empty()) {
        mlThreads.front().join();
        mlThreads.pop_front();
    }
}


Executor::~Executor(void) {
}


void Executor::Worker(uint32_t aThreadNum) {
    Task newTask;
    bool haveTask;

    haveTask = mvThreadTasks[aThreadNum]->GetTask(newTask);
    if(!haveTask) {
        haveTask = mAnyoneTasks.GetTask(newTask);
    }

    while(haveTask || mtRunning) {
        if(haveTask) {
            newTask(aThreadNum);
        }

        haveTask = mvThreadTasks[aThreadNum]->GetTask(newTask);
        if(!haveTask) {
            haveTask = mAnyoneTasks.GetTask(newTask);
        }
    }
}

