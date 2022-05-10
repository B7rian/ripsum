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

#include <cstdint>
#include <fstream>
#include <filesystem>

class CFile {
	public:
		CFile(const std::filesystem::path& aP): mPath(aP) { }
		
		void InitFile(void);
		void ReadBytes(void);
		void FinishFile(void);

		auto GetPath(void) { return mPath; }
		uint8_t *GetBuf(void) { return mBuf; }
		uint32_t GetBufCount(void) { return mBufCount; }
		uint32_t GetBytesRead(void) { return mBytesRead; }

	private:
		static const int FILE_BLOCK_SIZE { 16*1024 };
										// Number of bytes to read each time

		std::filesystem::path mPath;	// Path to file
		std::ifstream mSin;				// Input stream
		uint8_t mBuf[FILE_BLOCK_SIZE];	// File buffer data
		uint32_t mBufCount;				// Bytes in file buffer
		uint32_t mBytesRead;			// Total bytes read from file
};
