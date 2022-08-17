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
#include <mutex>

#include "CTaskState.h"

//
// CStatePool was created to get the details of TaskState allocation out
// of the CScheduler class.
// 

class CStatePool {
public:
	CTaskState *GetNewState(std::filesystem::path aP);
	void DeleteStates(void);

private:
	std::mutex smStateMtx;
	std::vector<CTaskState*> mvpStatePointers;
};
