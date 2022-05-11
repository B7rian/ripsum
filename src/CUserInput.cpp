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
#include <functional>
#include <fstream>
#include <sstream>

#include "CUserInput.h"

void CUserInput::ReadChecksumsFromFile(std::filesystem::path aP,
			   std::function<void(std::filesystem::path, std::string)> aFileCb)
{
	std::ifstream sin;
	std::string line;

	sin.open(aP);
	while(std::getline(sin, line)) {
		std::string checksum;
		char c;
		std::filesystem::path p;
		std::stringstream ss(line);

		ss >> checksum;
		ss.get(c); // Discard space
		ss.get(c); // Will be * for binary, space for text mode
		ss >> p;

		aFileCb(p, checksum);
	}
}
