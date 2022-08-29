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

#include "CFile.h"
#include "CTaskState.h"
#include "CRipsumOutput.h"

class CConsoleOutput: public CRipsumOutput {
public:
	void NotifyGoodChecksum(CFile *apFile);
	void NotifyBadChecksum(CFile *apFile);
	void NotifyGenerateDone(CTaskState *apState);
	void NotifyBadChecksumLine(void);
	void Done(void);

private:
	std::mutex outputMtx;
	int mBadSums = 0;
	int mBadLines = 0;
};
