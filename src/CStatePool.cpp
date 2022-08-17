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

#include <filesystem>
#include <vector>
#include <mutex>

#include "CTaskState.h"
#include "CStatePool.h"


//
// GetNewState: Return a new state.  Right now it is actually a new state
// but it doesn't have to stay that way
//
CTaskState *CStatePool::GetNewState(std::filesystem::path aP) {
	CTaskState *pNewState = new CTaskState(aP);
	std::lock_guard<std::mutex> lock(smStateMtx);
	mvpStatePointers.push_back(pNewState);
	return pNewState;
}


//
// DeleteStates: Delete all the states.  To be called when they aren't needed
// anymore.
//
void CStatePool::DeleteStates(void) {
	for(auto s: mvpStatePointers) {
		if(s) delete s;
	}
}

