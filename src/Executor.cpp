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


namespace {

Task MakeChecksumFinishLambda(TaskState *apState, 
                              Executor *apEx,
                              RipsumOutput *apOut) 
{
    return [apState, apEx, apOut](void) {
        apState->Finish();
        std::cerr << "Finish " << apState->GetPath() << std::endl;
        apOut->NotifyChecksumReady(apState->GetPath(), apState->GetChecksum());
        delete apState;
        apEx->ActivityDone();
    };
}

Task MakeReadAndHashLambda(TaskState *apState, 
                           Executor *apEx,
                           RipsumOutput *apOut) 
{
    return [apState, apEx, apOut](void) {
        apState->ReadBytes();
        if(apState->FileOk() == true) {
            apEx->AddTask(MakeReadAndHashLambda(apState, apEx, apOut));
            apState->AddBytesToHash();
        }
        else {
            apEx->AddTask(MakeChecksumFinishLambda(apState, apEx, apOut));
        }
    };
}

Task MakeComputeChecksumLambda(std::filesystem::path aP,
                               uint32_t aBlockSize,
                               Executor *apEx,
                               RipsumOutput *apOut) 
{
    return [aP, aBlockSize, apEx, apOut](void) {
        std::cerr << "Init " << aP << std::endl;
        TaskState *pState = new TaskState(aP, aBlockSize);
        pState->Init();
        apEx->AddTask(MakeReadAndHashLambda(pState, apEx, apOut));
    };
}

}


//
// ComputeChecksums
//

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
    mlThreads.push_front(std::thread(&Executor::Worker, this));
    //mlThreads.push_front(std::thread(&Executor::Worker, this));
}


// Wait for all the work to complete.  If the main thread has called Wait()
// we assume it is also done doing stuff and call ActivityDone() to signal
// the worker threads that it's done.
void Executor::Wait(void) {
    std::cerr << "** Wait" << std::endl;
    ActivityDone();
    while(mtRunning > 0 || !mTasks.Empty()) {
        while(!mTasks.Empty()) {}
        std::cerr << "** Tasks empty" << std::endl;
        while(mtRunning > 0) {}
        std::cerr << "** No runners" << std::endl;
    }

    std::cerr << "** Joining threads" << std::endl;
    while(!mlThreads.empty()) {
        mlThreads.front().join();
        mlThreads.pop_front();
    }
    std::cerr << "** All joined" << std::endl;
}


Executor::~Executor(void) {
}


void Executor::Worker(void) {
    Task newTask;
    bool haveTask;

    haveTask = mTasks.GetTask(newTask);
    while(haveTask || mtRunning) {
        if(haveTask) {
            newTask();
        }
        haveTask = mTasks.GetTask(newTask);
    }
    std::cerr << "** Worker done" << std::endl;
}

