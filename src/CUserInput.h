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


// 
// CUserInput was added so I could capture all the error checking of user 
// input in 1 spot instead of having error checking sprinkled all over the 
// rest of the code.  Errors caught here should be handled relatively 
// gracefully, whereas errors encountered in other parts of the code can 
// do whatever
//
// It winds up being a good spot to put the command line parser, checksum file
// reader, and to check that the files in the directories that the user
// gives us are actually readable.
//

class CUserInput {
public:
	// Reads checksums from the given file and calls the given callback
	// for each checksum read
	static void ReadChecksumsFromFile(std::filesystem::path aP,
		   std::function<void(std::filesystem::path, std::string)> aFileCb);

	static void Done(void);

	// Parses the command line (pass in argc and argv) and stores the found
	// options and stuff in member variables
    void ParseCommandline(int argc, char **argv);

	// 1 member per command line flag, not valid until after ParseCommandLine
	// is called correctly
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

	// A list of (supposed) paths from the command line.  Not valid until 
	// after ParseCommandLine is called correctly
	std::vector<std::filesystem::path> mPaths;

private:
	static int smBadLines;

};

