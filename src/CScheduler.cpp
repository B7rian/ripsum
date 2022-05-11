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
#include "taskflow/taskflow.hpp"

#include "CScheduler.h"
#include "CTaskState.h"
#include "CFileSystem.h"
#include "CUserInput.h"

std::mutex CScheduler::smOutputMtx;

void CScheduler::AddPath(std::filesystem::path aP) {
	mvPaths.push_back(aP);
}

void CScheduler::Run(bool aCheckNotCompute) { 
	if(aCheckNotCompute) {
		for(auto& target: mvPaths) {
			MakeTasksToReadAndCheckFiles(mTaskflow, target,
				[&](std::filesystem::path aP) -> CTaskState* { 
					CTaskState *pNewState = new CTaskState(aP);
					mvpStatePointers.push_back(pNewState);
					return pNewState;
				});
		}
	}
	else {
		for(auto& target: mvPaths) {
			MakeTasksToFindAndHashFiles(mTaskflow, target,
				[&](std::filesystem::path aP) -> CTaskState* { 
					CTaskState *pNewState = new CTaskState(aP);
					mvpStatePointers.push_back(pNewState);
					return pNewState;
				});
		}
	}

	mExecutor.run(mTaskflow).wait(); 
}

CScheduler::~CScheduler(void) {
	for(auto s: mvpStatePointers) {
		delete s;
	}
}

void CScheduler::MakeTasksToHashFile(tf::Subflow& aSubflow, 
		                             CTaskState *apState,
			                         std::function<void(CTaskState*)> aDoneCb)
{
	tf::Task a = aSubflow.emplace([=]() mutable { 
		apState->Init(); 
	}).name("init");

	tf::Task b = aSubflow.emplace([=]() { 
		apState->ReadBytes(); 
	}).name("read");

	tf::Task c = aSubflow.emplace([=]() {
		if(apState->GetBufCount() == 0) {
			return 1;
		}
		return 0;
	}).name("check");

	tf::Task d = aSubflow.emplace([=]() { 
		apState->AddBytesToHash(); 
	}).name("hash");

	tf::Task d2 = aSubflow.emplace([=]() { 
		return 0; 
	}).name("loopBack");

	tf::Task e = aSubflow.emplace([=]() {
		apState->Finish();
		aDoneCb(apState);
	}).name("finish");

	a.precede(b);
	b.precede(c);
	c.precede(d, e);
	d.precede(d2);
	d2.precede(b);
}

tf::Task CScheduler::MakeTasksToFindAndHashFiles(tf::Taskflow& aTf, 
		std::filesystem::path aTarget,
		std::function<CTaskState* (std::filesystem::path)> aStateAllocatorCb)
{
    return aTf.emplace([&, aTarget](tf::Subflow& aSubflow) {
		CFileSystem::FindFiles(aTarget,
			[&](std::filesystem::path aP) { 
				CTaskState *pNewState = aStateAllocatorCb(aP);
				MakeTasksToHashFile(aSubflow, pNewState,
					[&](CTaskState *apState) {
						std::lock_guard<std::mutex> lock(smOutputMtx);
						std::cout << apState->GetChecksum() << "  " 
						          << apState->GetPath().native()
						          << std::endl;
					});
			});
	});
}

tf::Task CScheduler::MakeTasksToReadAndCheckFiles(tf::Taskflow& aTf, 
		std::filesystem::path aTarget,
		std::function<CTaskState* (std::filesystem::path)> aStateAllocatorCb)
{
    return aTf.emplace([&, aTarget](tf::Subflow& aSubflow) {
		CUserInput::ReadChecksumsFromFile(aTarget,
			[&](std::filesystem::path aP, std::string aChecksum) { 
				CTaskState *pNewState = aStateAllocatorCb(aP);
				pNewState->SetExpectedChecksum(aChecksum);
				MakeTasksToHashFile(aSubflow, pNewState,
					[&](CTaskState *apState) {
						std::lock_guard<std::mutex> lock(smOutputMtx);
						std::cout << apState->GetPath().native();
						if(apState->ChecksumIsOk()) {
						    std::cout << ": OK";
						} else {
						    std::cout << ": ERROR";
						}
						std::cout << std::endl;
					});
			});
	});
}


