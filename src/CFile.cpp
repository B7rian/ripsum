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

#include "CFile.h"

void CFile::InitFile(void) {
	mBytesRead = 0;
	mSin.open(mPath, std::ios::binary);
}

void CFile::ReadBytes(void) {
	mBufCount = 0;
	if(mSin.good() && !mSin.eof()) {
		mSin.read((char *)mBuf, FILE_BLOCK_SIZE);
		mBufCount = mSin.gcount();
		mBytesRead += mSin.gcount();
	}
}

void CFile::FinishFile(void) {
	mSin.close();
}

