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
#include "File.h"

File::File(const std::filesystem::path& aP, uint32_t aBlockSize)
    : mPath{aP}, mBlockSize{aBlockSize}, mBytesRead{0}
{
    mSin.open(mPath, std::ios::binary);
}


uint32_t File::ReadBytes(void) {
    Buffer b;
    b.pBytes = new uint8_t[mBlockSize];
    b.mCount = 0;

    {
        std::lock_guard<std::mutex> lock(mStreamMutex);
        if(!mSin.good() || mSin.eof()) {
            mOk = false;
            delete[] b.pBytes;
            return 0;
        }

        mSin.read((char *)b.pBytes, mBlockSize);
        b.mCount = mSin.gcount();
        mBytesRead += b.mCount;
    }

    if(b.mCount == 0) {
        mOk = false;
        delete[] b.pBytes;
        return 0;
    }

    mOk = true;
    std::lock_guard<std::mutex> lock(mBuffersMutex);
    mlBuffers.push_back(b); // Makes a copy
    return b.mCount;
}


uint32_t File::GetBytes(uint8_t* &apBytes) {
    std::lock_guard<std::mutex> lock(mBuffersMutex);
    if(mlBuffers.empty()) {
        return 0;
    }
    Buffer b = mlBuffers.front();
    mlBuffers.pop_front();
    apBytes = b.pBytes;
    return b.mCount;
}


void File::CleanupBytes(uint8_t *pBytes) {
    delete[] pBytes;
}


File::~File(void) {
    mSin.close();

    uint8_t *pBytes;
    while(GetBytes(pBytes)) {
        CleanupBytes(pBytes);
    }
}

