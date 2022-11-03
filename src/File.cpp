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
    : mPath{aP}, mBlockSize{aBlockSize}
{
    mSin.open(mPath, std::ios::binary);
}


uint32_t File::ReadBytes(void) {
    if(!mSin.good() || mSin.eof()) {
        mOk = false;
        return 0;
    }

    Buffer b;
    b.pBytes = new uint8_t[mBlockSize];
    mSin.read((char *)b.pBytes, mBlockSize);
    b.mCount = mSin.gcount();

    if(b.mCount > 0) {
        mOk = true;
        mlBuffers.push_back(b); // Makes a copy
    }
    else {
        mOk = false;
        delete[] b.pBytes;
    }

    return b.mCount;
}


uint32_t File::GetBytes(uint8_t* &apBytes) {
    Buffer b = mlBuffers.front();
    apBytes = b.pBytes;
    return b.mCount;
}


void File::CleanupBytes(void) {
    Buffer b = mlBuffers.front();
    delete[] b.pBytes;
    mlBuffers.pop_front();
}

