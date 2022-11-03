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

#include <functional>
#include <mutex>
#include <vector>

// Shorthand for a Task.  Tasks have to capture everything they need to run
using Task = std::function<void(void)>;


// A TaskList is a collection of tasks that are ready to be run
class TaskList {
public:
    // AddTask adds a task to the list
    void AddTask(const Task& aT);

    // GetTask copies a tast to aT and returns true if there is a task
    // to be gotten, otherwise returns false
    bool GetTask(Task& aT);

    // Empty returns true if there are no tasks in the list
    bool Empty(void);

private:
    std::vector<Task> mvTasks;  // Tasks to be run
    std::mutex mTaskListMutex;  // Mutex for task vector
};


