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

#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>
#include <mutex>

#include <openssl/evp.h>
#include "Hash.h"

static std::mutex sgEVPMutex;

void Hash::InitHash(void) {
    std::lock_guard<std::mutex> lock(sgEVPMutex);
    mCtx = EVP_MD_CTX_new();
    mMd = EVP_sha256();
    EVP_DigestInit_ex(mCtx, mMd, NULL);
}

void Hash::AddBytesToHash2(uint8_t *aBytes, uint32_t aCount) {
    std::lock_guard<std::mutex> lock(mDigestMutex);
    EVP_DigestUpdate(mCtx, aBytes, aCount);
}

void Hash::FinishHash(void) {
    {
        std::lock_guard<std::mutex> lock(mDigestMutex);
        EVP_DigestFinal_ex(mCtx, mOutDigest, &mDigestLen);
    }

    {
        std::lock_guard<std::mutex> lock(sgEVPMutex);
        EVP_MD_CTX_free(mCtx);
    }

    std::stringstream str;
    str << std::hex << std::setfill('0') << std::setw(2);
    for(int i = 0; i < mDigestLen; i++) {
        str << std::setfill('0') << std::setw(2)
            << (unsigned int)mOutDigest[i];
    }

    mChecksum = str.str();
}

