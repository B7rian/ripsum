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

#include <cstdlib>
#include "TaskList.h"

// AddTask is thread safe.  Put stuff at the back of the vector rather
// than the front so it doesn't have to move everything done.  This will
// trigger allocations when the vector needs to expand but they'll be
// less often than a std:list or something like it would need to allocate.
// This is actually a little faster than using a deque to keep things
// in order, but the tasks need to be aware that they might run out of 
// order and handle races - this is managable for this particular application
void TaskList::AddTask(const Task& aT) {
    std::lock_guard<std::mutex> lock(mTaskListMutex);
    mvTasks.push_back(aT);
}

// GetTask is also thread safe.
bool TaskList::GetTask(Task& aT) {
    std::lock_guard<std::mutex> lock(mTaskListMutex);
    if(mvTasks.empty()) {
        aT = [](uint32_t unused) {
            abort();
        };
        return false;
    }

    aT = mvTasks.front();
    mvTasks.pop_front();
    return true;
}

// Empty is thread safe too, but remember that a Worker might have a task
// to run and be working on it even if the list is empty, and also that
// Worker can add more tasks to the list before it's done.
bool TaskList::Empty(void) {
    std::lock_guard<std::mutex> lock(mTaskListMutex);
    return mvTasks.empty();
}


