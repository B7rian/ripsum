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
#include "FileSystem.h"


void FileSystem::FindFiles(const std::filesystem::path aRoot, 
		                    std::function<void(std::filesystem::path)> aFileCb) 
{
	std::list<std::filesystem::path> paths;

	paths.push_front(aRoot);

	while(!paths.empty()) {
		std::filesystem::path p = paths.front();
		paths.pop_front();

		if(std::filesystem::is_regular_file(p)) {
			aFileCb(p);
		}
		else if(std::filesystem::is_directory(p)) {
			for(auto& entry: std::filesystem::directory_iterator(p)) {
				if(entry.is_regular_file()) {
					aFileCb(entry.path());
				}
				else if(entry.is_directory()) {
					paths.push_front(entry.path());
				}
			}
		}
	}
}

