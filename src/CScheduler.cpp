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
#include <filesystem>
#include <vector>
#include <functional>
#include <mutex>
#include <fstream>
#include <sstream>

#include "CScheduler.h"
#include "CTaskState.h"
#include "CFileSystem.h"
#include "CUserInput.h"


//
// AddPath: Save the given path for later.  Depending on which options
// are given on the commandline, Run will decide what to do with them
//
void CScheduler::AddPath(std::filesystem::path aP) {
	mvPaths.push_back(aP);
}


// 
// Run: Compute or check checksums depeding on user options.
//
void CScheduler::Run(CUserInput& aInput,
	                 std::function<void(CTaskState*)> aDoneCb)
{
	if(aInput.mCheckFlag) {
		for(auto& target: mvPaths) {
			aInput.ReadChecksumsFromFile(target,
				[&](std::filesystem::path aP, std::string aChecksum) { 
					HashFile(aP, 
						[&, aChecksum](CTaskState *apState) {
							apState->SetExpectedChecksum(aChecksum);
							aDoneCb(apState);
						});
					});
		}
	}
	else {
		for(auto& target: mvPaths) {
			CFileSystem::FindFiles(target,
				[&](std::filesystem::path aP) { 
					HashFile(aP, aDoneCb);
				});
		}
	}
}

void CScheduler::HashFile(std::filesystem::path aP, 
		                  std::function<void(CTaskState*)> aDoneCb) 
{
	CTaskState *pState = new CTaskState(aP);

	pState->Init(); 

	pState->ReadBytes(); 
	while(pState->FileOk() == true) {
		pState->AddBytesToHash(); 
		pState->ReadBytes(); 
	}
	pState->AddBytesToHash(); 

	pState->Finish();
	aDoneCb(pState);
	delete pState;
}

