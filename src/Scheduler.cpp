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

#include "Scheduler.h"
#include "TaskState.h"
#include "FileSystem.h"

namespace {

Work MakeChecksumFinishLambda(TaskState *apState) {
    return [apState](void) {
        apState->Finish();
    };
}

Work MakeReadAndHashLambda(TaskState *apState, WorkList *aplWork) {
    return [apState, aplWork](void) {
        apState->ReadBytes();
        if(apState->FileOk() == true) {
            aplWork->push_front(MakeReadAndHashLambda(apState, aplWork));
            apState->AddBytesToHash();
        }
        else {
            aplWork->push_front(MakeChecksumFinishLambda(apState));
        }
    };
}

Work MakeGenerateChecksumLambda(TaskState *apState, WorkList *aplWork) {
    return [apState, aplWork](void) {
        apState->Init();
        aplWork->push_front(MakeReadAndHashLambda(apState, aplWork));
    };
}

}


//
// ComputeChecksums
//

void Scheduler::ComputeChecksums(const std::filesystem::path& aPath,
                                 RipsumOutput *apOut) 
{
    FileSystem::FindFiles(aPath,
        [&](std::filesystem::path aP) {
            TaskState *apState = new TaskState(aP);
            mlWork.push_front(MakeGenerateChecksumLambda(apState, &mlWork));

            while(!mlWork.empty()) {
                auto f = mlWork.front();
                mlWork.pop_front();
                f();
            }

            apOut->NotifyChecksumReady(aP, apState->GetChecksum());
            delete apState;
        });
}

