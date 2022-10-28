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

#include "TaskList.h"

// AddTask is thread safe.  Put stuff at the back of the vector rather
// than the front so it doesn't have to move everything done.  This will
// trigger allocations when the vector needs to expand but they'll be 
// less often than a std:list or something like it would need to allocate.
void TaskList::AddTask(const Task& aT) {
    std::lock_guard<std::mutex> lock(mTaskListMutex);
    mvTasks.push_back(aT);
}


// GetTask is also thread safe. 
bool TaskList::GetTask(Task& aT) {
    std::lock_guard<std::mutex> lock(mTaskListMutex);
    if(mvTasks.empty()) {
        aT = [=]() { };
        return false;
    }

    aT = mvTasks.back();
    mvTasks.pop_back();
    return true;
}



