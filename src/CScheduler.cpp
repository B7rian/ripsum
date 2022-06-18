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
std::mutex CScheduler::smStateMtx;
static int sgBadSums = 0;

//
// AddPath: Save the given path for later.  Depending on which options
// are given on the commandline, Run will decide what to do with them
//
void CScheduler::AddPath(std::filesystem::path aP) {
	mvPaths.push_back(aP);
}


// 
// Run: Set up taskflow tasks to perform the operations requested by 
// the user - either check or compute checksums
//
void CScheduler::Run(bool aCheckNotCompute) { 
	if(aCheckNotCompute) {
		for(auto& target: mvPaths) {
			MakeTasksToReadAndCheckFiles(mTaskflow, target,
				[&](std::filesystem::path aP) -> CTaskState* { 
					CTaskState *pNewState = new CTaskState(aP);
					std::lock_guard<std::mutex> lock(smStateMtx);
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
					std::lock_guard<std::mutex> lock(smStateMtx);
					mvpStatePointers.push_back(pNewState);
					return pNewState;
				});
		}
	}

	mExecutor.run(mTaskflow).wait(); 

	// This should be in a UserOutput class or something
	if(sgBadSums) {
		std::cerr << "sha256sum: WARNING: " << sgBadSums << " computed ";
		std::cerr << ((sgBadSums == 1) ? "checksum" : "checksums");
		std::cerr << " did NOT match" << std::endl;
	}

	CUserInput::Done();
}


// 
// Destructor: Clean up state pointers when done
//
CScheduler::~CScheduler(void) {
	for(auto s: mvpStatePointers) {
		if(s) delete s;
	}
}


//
// MakeTasksToHashFile: Like it's named.  Note that taskflow requires
// a dummy state (called loopBack below) to avoid hanging.  This is in
// the taskflow docs somewhere.
//
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
						    std::cout << ": FAILED";
							sgBadSums++;
						}
						std::cout << std::endl;
					});
			});
	});
}


