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
void CScheduler::Run(CUserInput& aInput,
	                 std::function<void(CTaskState*)> aDoneCb)
{
	if(aInput.mCheckFlag) {
		for(auto& target: mvPaths) {
			mTaskflow.emplace([&, target](tf::Subflow& aSubflow) {
				aInput.ReadChecksumsFromFile(target,
					[&](std::filesystem::path aP, std::string aChecksum) { 
						MakeTasksToHashFile(aSubflow, aP, 
							[&, aChecksum](CTaskState *apState) {
								apState->SetExpectedChecksum(aChecksum);
								aDoneCb(apState);
						});
					});
			});
		}
	}
	else {
		for(auto& target: mvPaths) {
			mTaskflow.emplace([&, target](tf::Subflow& aSubflow) {
				CFileSystem::FindFiles(target,
					[&](std::filesystem::path aP) { 
						MakeTasksToHashFile(aSubflow, aP, aDoneCb);
					});
			});
		}
	}

	mExecutor.run(mTaskflow).wait(); 
}

//
// MakeTasksToHashFile creates tasks to compute the has for a file.  Note that
// taskflow requires a dummy task (called loopBack below) to avoid hanging.
// This is in the taskflow docs somewhere.
//
// Pick single-buffer or double-buffered file reads below

#if 0

//
// This is the single-file-buffer version of the flow.
//
// xxx      is a strong dependency
// --- or | is a week dependency
//
//      Init
//        x
//        x
//  +-->Read 
//  |     x      
//  |     x    0
//  |   Check?---+
//  |     |      |
//  |     | 1    |
//  |     v      |
//  |   Hash     |
//  |     x      |
//  |     x      |
//  +--Loopback  |
//               |
//     Finish <--+
//

void CScheduler::MakeTasksToHashFile(tf::Subflow& aSubflow,
		std::filesystem::path aP, std::function<void(CTaskState*)> aDoneCb) {
	CTaskState *pState = new CTaskState(aP);

	tf::Task a = aSubflow.emplace([=]() { 
		pState->Init(); 
	}).name("init");

	tf::Task b = aSubflow.emplace([=]() { 
		pState->ReadBytes(); 
	}).name("read");

	tf::Task c = aSubflow.emplace([=]() {
		if(pState->FileOk() == false) {
			return 1;
		}
		return 0;
	}).name("check");

	tf::Task d = aSubflow.emplace([=]() { 
		pState->AddBytesToHash(); 
	}).name("hash");

	tf::Task d2 = aSubflow.emplace([=]() { 
		return 0; 
	}).name("loopBack");

	tf::Task e = aSubflow.emplace([=]() {
		pState->Finish();
		aDoneCb(pState);
		delete pState;
	}).name("finish");

	a.precede(b);
	b.precede(c);
	c.precede(d, e);
	d.precede(d2);
	d2.precede(b);
}

#else

//
// This is the double-file-buffer version of the flow.  
//
// xxx      is a strong dependency
// --- or | is a week dependency
//
//           Init
//            x
//    +----->Do
//    |       x
//    |   xxxxxxxxx
//    |   x       x
//    | Read0    Hash1
//    |   x       x
//    |   xxxxxxxxx
//    |       x
//    |      Sync
//    |       x
//    |   xxxxxxxxx
//    |   x       x
//    | Hash0    Read1
//    |   x       x
//    |   xxxxxxxxx
//    |       x
//    |     While----->Finish
//    |       |
//    +-------+
//           
// This is more like a do-while loop and doesn't need a special loopback node
// to avoid deadlock.  The "Do" node is effectively a fork to kick off 2 tasks
// in parallel (a read and a hash) which the "While" conditional node can't do.
// The Sync node keeps things coordinated e.g. so the hash nodes don't get to
// far ahead of the reads and vice versa. 
//


void CScheduler::MakeTasksToHashFile(tf::Subflow& aSubflow,
		std::filesystem::path aP, std::function<void(CTaskState*)> aDoneCb) {

	CTaskState *pState = new CTaskState(aP);

	tf::Task init = aSubflow.emplace([=]() { 
		pState->Init(); 
		pState->ReadBytes(); // Prime the loop
	}).name("init");

	init.acquire(mMemLimiter);

	tf::Task do_task = aSubflow.emplace([=]() {
		//std::cerr << "--do--" << std::endl;
	}).name("do_task");

	init.precede(do_task);

	tf::Task read0 = aSubflow.emplace([=]() { 
		pState->ReadBytes(); 
	}).name("read0");

	tf::Task hash1 = aSubflow.emplace([=]() { 
		pState->AddBytesToHash(); 
	}).name("hash1");

	read0.succeed(do_task);
	hash1.succeed(do_task);

	tf::Task sync = aSubflow.emplace([=]() {
		//std::cerr << "--sync--" << std::endl;
	}).name("sync");

	read0.precede(sync);
	hash1.precede(sync);

	tf::Task hash0 = aSubflow.emplace([=]() { 
		pState->AddBytesToHash(); 
	}).name("hash0");

	tf::Task read1 = aSubflow.emplace([=]() { 
		pState->ReadBytes(); 
	}).name("read1");

	hash0.succeed(sync);
	read1.succeed(sync);

	tf::Task while_task = aSubflow.emplace([=]() {
		//std::cerr << "--while--" << std::endl;
		if(pState->FileOk() == false) {
			return 1;
		}
		return 0;
	}).name("while_task");

	hash0.precede(while_task);
	read1.precede(while_task);

	tf::Task finish = aSubflow.emplace([=]() {
		//std::cerr << "--finish--" << std::endl;
		pState->Finish();
		aDoneCb(pState);
		delete pState;
	}).name("finish");

	while_task.precede(do_task, finish);
	finish.release(mMemLimiter);
}

#endif
