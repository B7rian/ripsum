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

#include <mutex>

#include "File.h"
#include "TaskState.h"
#include "RipsumOutput.h"

class ConsoleOutput: public RipsumOutput {
public:
	void NotifyGoodChecksum(File *apFile);
	void NotifyBadChecksum(File *apFile);
	void NotifyGenerateDone(TaskState *apState);
	void NotifyBadFileFormat(void);
	void UserNeedsHelp(void);
	void Done(void);

private:
	std::mutex outputMtx;
	int mBadSums = 0;
	int mBadLines = 0;
};

