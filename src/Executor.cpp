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

Task MakeReadAndHashLambda(TaskState *apState, TaskList *aplTasks) {
    return [apState, aplTasks](void) {
        apState->ReadBytes();
        if(apState->FileOk() == true) {
            aplTasks->push_front(MakeReadAndHashLambda(apState, aplTasks));
            apState->AddBytesToHash();
        }
        else {
            aplTasks->push_front(MakeChecksumFinishLambda(apState));
        }
    };
}

Task MakeComputeChecksumLambda(TaskState *apState, TaskList *aplTasks) {
    return [apState, aplTasks](void) {
        apState->Init();
        aplTasks->push_front(MakeReadAndHashLambda(apState, aplTasks));
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
            mlTasks.push_front(MakeComputeChecksumLambda(apState, &mlTasks));

            while(!mlTasks.empty()) {
                auto f = mlTasks.front();
                mlTasks.pop_front();
                f();
            }

            apOut->NotifyChecksumReady(aP, apState->GetChecksum());
            delete apState;
        });
}


