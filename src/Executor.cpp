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

Task MakeChecksumFinishLambda(TaskState *apState) {
    return [apState](void) {
        apState->Finish();
    };
}

Task MakeReadAndHashLambda(TaskState *apState, TaskList *apTasks) {
    return [apState, apTasks](void) {
        apState->ReadBytes();
        if(apState->FileOk() == true) {
            apTasks->AddTask(MakeReadAndHashLambda(apState, apTasks));
            apState->AddBytesToHash();
        }
        else {
            apTasks->AddTask(MakeChecksumFinishLambda(apState));
        }
    };
}

Task MakeComputeChecksumLambda(TaskState *apState, TaskList *apTasks) {
    return [apState, apTasks](void) {
        apState->Init();
        apTasks->AddTask(MakeReadAndHashLambda(apState, apTasks));
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
            TaskState *apState = new TaskState(aP, aConfig.mBlockSize);
            mTasks.AddTask(MakeComputeChecksumLambda(apState, &mTasks));
            Task newTask;

            while(mTasks.GetTask(newTask)) {
                newTask();
            }

            apOut->NotifyChecksumReady(aP, apState->GetChecksum());
            delete apState;
        });
}


