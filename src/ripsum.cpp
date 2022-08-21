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

#include "CScheduler.h"
#include "CUserInput.h"

int main(int argc, char **argv) {
	CUserInput config;
	CScheduler s;
	std::mutex outputMtx;
	int badSums = 0;

	config.ParseCommandline(argc, argv);

	for(auto& p: config.mPaths) {
		s.AddPath(p);
	}

	if(config.mCheckFlag) {
		s.Run(config.mCheckFlag,
					[&](CTaskState *apState) {
						std::lock_guard<std::mutex> lock(outputMtx);
						std::cout << apState->GetPath().native();
						if(apState->ChecksumIsOk()) {
						    std::cout << ": OK";
						} else {
						    std::cout << ": FAILED";
							badSums++;
						}
						std::cout << std::endl;
					});
	}
	else {
		s.Run(config.mCheckFlag,
			  [&](CTaskState *apState) {
				  std::lock_guard<std::mutex> lock(outputMtx);
				  std::cout << apState->GetChecksum() << "  " 
							<< apState->GetPath().native()
							<< std::endl;
			  });
	}

	if(badSums) {
		std::cerr << "sha256sum: WARNING: " << badSums << " computed ";
		std::cerr << ((badSums == 1) ? "checksum" : "checksums");
		std::cerr << " did NOT match" << std::endl;
	}

	CUserInput::Done();

	return 0;
}

