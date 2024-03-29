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

#include <filesystem>
#include <functional>
#include <regex>

#include "ChecksumLine.h"

#include <iostream>

//
// CCheckSumLine
//
// This class was added to hold all the parsing logic for the checksum
// file.
//
// Implementation Discussion: The allowed characters in a filename are
// dependent on the filesystem.  The current implementation below is
// very relaxed so it'll accept paths on multiple filesystem types
// and may allow more filenames than sha256sum does
//

ChecksumLine::ChecksumLine(std::string aLine)
{
    std::regex r("\\s*([0-9a-f]{64}) [ *](.*\\S)\\s*");
    std::smatch captures;

    if(!std::regex_match(aLine, captures, r)) {
        mOk = false;
        return;
    }

    // captures[0] is the whole match
    mChecksum = captures[1];
    //Used to be mPath = mUTFConverter.from_bytes(captures[2]);
	//but this was throwing an exception on Linux 
    mPath = captures[2];

    mOk = true;

#if 0
    std::cout << '\n';
    std::cout << "Line: " << aLine << '\n';
    std::cout << "  CS: " << aChecksum << '\n';
    std::cout << "   P: " << aPath << '\n';
#endif
}

