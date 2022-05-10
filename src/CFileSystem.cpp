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

#include <list>
#include <filesystem>
#include <functional>
#include "CFileSystem.h"

void CFileSystem::FindFiles(const std::filesystem::path aRootDir, 
		                    std::function<void(std::filesystem::path)> aFileCb) 
{
	std::list<std::filesystem::path> dirs;

	dirs.push_front(aRootDir);

	while(!dirs.empty()) {
		std::filesystem::path dir = dirs.front();
		dirs.pop_front();
		for(auto& f: std::filesystem::directory_iterator(dir)) {
			std::filesystem::path p{f.path()};
			if(std::filesystem::is_regular_file(f)) {
				aFileCb(p);
			}
			else if(std::filesystem::is_directory(f)) {
				dirs.push_front(p);
			}
		}
	}
}

