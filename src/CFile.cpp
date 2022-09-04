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
#include "CFile.h"

// InitBuffer initializes internal state and saves the pointer to the 
// next buffer for later
void CBuffer::InitBuffer(CBuffer *apNextBuffer) {
	mDataCount = 0;
	mBytesRead = 0;
	mpNext = apNextBuffer;
}

// ReadBytes pulls some data from the file into the buffer.  
bool CBuffer::ReadBytes(std::ifstream& aSin) {
	mDataCount = 0;
	if(aSin.good() && !aSin.eof()) {
		aSin.read((char *)mData, FILE_BLOCK_SIZE);
		mDataCount = aSin.gcount();
		mBytesRead += aSin.gcount();
		//std::cerr << "<" << mDataCount << std::endl;
	}
	return (mDataCount > 0);
}


// InitFile opens the file and sets up the double buffering
void CFile::InitFile(void) {
	mSin.open(mPath, std::ios::binary);
	mPing.InitBuffer(&mPong);
	mPong.InitBuffer(&mPing);
	mpReadBuffer = &mPing;
	mpFullBuffer = &mPing;
}

// FinishFile: Just close the file when we're done
void CFile::FinishFile(void) {
	mSin.close();
}


// ReadBytes reads some bytes from the file into the current read buffer, then
// swaps read buffers in preperation for the next read
void CFile::ReadBytes(void) {
	mOk = mpReadBuffer->ReadBytes(mSin);
	mpReadBuffer = mpReadBuffer->GetNextBuffer();
}

// GetBytes provides the "full" buffer and bytes count, then swaps buffers
// for the next GetBytes call
uint32_t CFile::GetBytes(uint8_t* &aBytes) {
	aBytes = mpFullBuffer->GetData();
	uint32_t count = mpFullBuffer->GetDataCount();
	mpFullBuffer = mpFullBuffer->GetNextBuffer();
	return count;
}

uint32_t CFile::GetTotalBytesRead(void) {
	// There's a possible race here with data reading functions
	return mPing.GetTotalBytesRead() + mPong.GetTotalBytesRead();
}


