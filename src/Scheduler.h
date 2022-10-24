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

#pragma once

#include <filesystem>
#include <vector>

#include "TaskState.h"
#include "UserInput.h"

class Scheduler {
public:
    Scheduler(void) { }
    void AddPath(std::filesystem::path aP);
    void Run(UserInput& input,
             std::function<void(TaskState*)> aDoneCb);
    ~Scheduler(void) {}

private:
    std::vector<std::filesystem::path> mvPaths;

    void HashFile(std::filesystem::path aP,
                  std::function<void(TaskState*)> aDoneCb);

};

