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
#include "taskflow/taskflow.hpp"

#include "CTaskState.h"

class CScheduler {
public:
	void AddPath(std::filesystem::path aP);
	void Run(bool aCheckNotCompute);
	~CScheduler(void);

private:
	static std::mutex smOutputMtx;

	tf::Executor mExecutor;
	tf::Taskflow mTaskflow;

	std::vector<std::filesystem::path> mvPaths;
	std::vector<CTaskState*> mvpStatePointers;

	void MakeTasksToHashFile(tf::Subflow& aSubflow, CTaskState *apState,
							 std::function<void(CTaskState*)> aDoneCb);
	tf::Task MakeTasksToFindAndHashFiles(tf::Taskflow& aTf, 
		std::filesystem::path aTarget,
		std::function<CTaskState* (std::filesystem::path)> aStateAllocatorCb);
	tf::Task MakeTasksToReadAndCheckFiles(tf::Taskflow& aTf, 
		std::filesystem::path aTarget,
		std::function<CTaskState* (std::filesystem::path)> aStateAllocatorCb);

};

