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

#include "CConsoleOutput.h"

void CConsoleOutput::NotifyGoodChecksum(CFile *apFile) {
	std::lock_guard<std::mutex> lock(outputMtx);
	std::cout << apFile->GetPath().native();
	std::cout << ": OK";
	std::cout << std::endl;
}

void CConsoleOutput::NotifyBadChecksum(CFile *apFile) {
	std::lock_guard<std::mutex> lock(outputMtx);
	std::cout << apFile->GetPath().native();
	std::cout << ": FAILED";
	std::cout << std::endl;
	mBadSums++;
}

void CConsoleOutput::NotifyGenerateDone(CTaskState *apState) {
	std::lock_guard<std::mutex> lock(outputMtx);
	std::cout << apState->GetChecksum() << "  " 
			  << apState->GetPath().native()
			  << std::endl;
}

void CConsoleOutput::NotifyBadChecksumLine(void) {
	mBadLines++;
}

void CConsoleOutput::Done(void) {
	if(mBadSums) {
		std::cerr << "sha256sum: WARNING: " << mBadSums << " computed ";
		std::cerr << ((mBadSums == 1) ? "checksum" : "checksums");
		std::cerr << " did NOT match" << std::endl;
	}

	if(mBadLines > 0) {
		std::cerr << "sha256sum: WARNING: " << mBadLines;
		std::cerr << ((mBadLines == 1) ? " line is" : " lines are");
		std::cerr << " improperly formatted" << std::endl;
	}
}

