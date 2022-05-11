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

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

class CUserInput {
public:
	static void ReadChecksumsFromFile(std::filesystem::path aP,
		   std::function<void(std::filesystem::path, std::string)> aFileCb);

    void ParseCommandline(int argc, char **argv);

	int mBinaryFlag {0};
	int mCheckFlag {0};
	int mTagFlag {0};
	int mTextFlag {0};
	int mZeroFlag {0};
	int mIgnoreMissingFlag {0};
	int mQuietFlag {0};
	int mStatusFlag {0};
	int mStrictFlag {0};
	int mWarnFlag {0};
	int mHelpFlag {0};
	int mVersionFlag {0};

	std::vector<std::filesystem::path> mPaths;
};
