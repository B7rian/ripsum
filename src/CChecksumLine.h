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
#include <string>


// 
// CChecksumLine and derived classes are able to parse input lined for
// the various size digests that we may eventually support.
//


class CChecksumLine {
public:
	CChecksumLine(std::string aLine);

	bool IsOk(void) { return mOk; }
	std::filesystem::path GetPath(void) { return mPath; }
	std::string GetChecksum(void) { return mChecksum; }

private:
	bool mOk;
	std::filesystem::path mPath;
	std::string mChecksum;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> mUTFConverter {};
};

