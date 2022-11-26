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
#include <list>
#include <thread>
#include <atomic>

#include "RecipeState.h"
#include "UserInput.h"
#include "RipsumOutput.h"
#include "TaskList.h"

class Executor {
public:
    Executor(void);

    // ComputeChecksums computes checksums for files at/under the
    // given path and calls into the output object with the result
    void ComputeChecksums(const std::filesystem::path& aP,
                          UserInput& aConfig,
                          RipsumOutput *apOut);

    // CheckChecksums checkes the checksums in the given file
    // and calls into the output object with the result
    void CheckChecksums(const std::filesystem::path& aChecksumFile,
                        UserInput& aConfig,
                        RipsumOutput *apOut);

    // RecipeStarted logs when a new recipe starts running
    void RecipeStarted(void) {
        mtActiveRecipes++;
    }

    // AddTask adds a task to the task list that anyone can take from
    void AddTask(const Task& aT) {
        mAnyoneTasks.AddTask(aT);
    }

    // AddTask whith a thread parameter adds a task to the task list
    // for the thread that the parameter specifies
    void AddTask(const uint32_t aThreadNum, const Task& aT) {
        mvThreadTasks[aThreadNum]->AddTask(aT);
    }

    // RecipeDone logs when a recipe has completed
    void RecipeDone(void) {
        mtActiveRecipes--;
    }

    // Wait waits for all queues to empty and Worker threads to finish
    // their current tasks (via join)
    void Wait(void);

private:
    void Worker(uint32_t aThreadNum);     // Worker thread function
    TaskList mAnyoneTasks;                // Lists of tasks any thread can run
    std::vector<TaskList*> mvThreadTasks; // Tasks for each thread
    std::list<std::thread> mlThreads;     // The worker threads
    std::atomic<uint32_t> mtActiveRecipes;// Number of running recipes
};

