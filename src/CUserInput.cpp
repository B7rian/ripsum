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
#include <getopt.h>
#include <regex>

#include "CUserInput.h"
#include "CChecksumLine.h"

void CUserInput::ReadChecksumsFromFile(std::filesystem::path aP,
			   std::function<void(std::filesystem::path, std::string)> aFileCb)
{
	std::ifstream sin;
	std::string line;

	sin.open(aP);

	while(std::getline(sin, line)) {
		CChecksumLine parser(line);
		if(parser.IsOk()) {
			aFileCb(parser.GetPath(), parser.GetChecksum());
		}
		else {
			mOut.NotifyBadChecksumLine();
		}
	}
}


// This code is based on the example given in the documentation
// for GNU getopt
void CUserInput::ParseCommandline(int argc, char **argv) {
	int option_index = 0;
	int c;

	// Options stolen from sha256sum and md5sum man pages
	static struct option long_options[] = {
//		{"binary", no_argument, &mBinaryFlag, 1},
		{"check", no_argument, &mCheckFlag, 1},
//		{"tag", no_argument, &mTagFlag, 1},
//		{"text", no_argument, &mTextFlag, 1},
//		{"zero", no_argument, &mZeroFlag, 1},
//		{"ignore-missing", no_argument, &mIgnoreMissingFlag, 1},
//		{"quiet", no_argument, &mQuietFlag, 1},
//		{"status", no_argument, &mStatusFlag, 1},
//		{"strict", no_argument, &mStrictFlag, 1},
//		{"warn", no_argument, &mWarnFlag, 1},
		{"help", no_argument, &mHelpFlag, 1},
		{"version", no_argument, &mVersionFlag, 1},
		{0, 0, 0, 0}
	};

	do {
		c = getopt_long(argc, argv, "c", long_options, &option_index);
		switch(c) {
			case -1: break; // -1 means no option found
			case 0: break;  // 0 means long option detected and handled
//			case 'b': mBinaryFlag = 1; break;
			case 'c': mCheckFlag = 1; break;
//			case 't': mTextFlag = 1; break;
//			case 'z': mZeroFlag = 1; break;
//			case 'w': mWarnFlag = 1; break;
			default: mOut.UserNeedsHelp();
		}
	} while(c != -1);

	if(mHelpFlag) {
		mOut.UserNeedsHelp();
	}
	else {
		while(optind < argc) {
			mPaths.push_back(argv[optind++]);
		}
	}
}

