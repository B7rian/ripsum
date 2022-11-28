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
#include <string>
#include <iostream>
#include <openssl/evp.h>

// Checksum has all the state required to compute a checksum

class Checksum {
public:
    Checksum(void);

    // Include th egiven bytes in the checksum being computed.
    void AddBytesToChecksum(uint8_t *aBytes, uint32_t aCount);

	// Gets the checksum in string form
    const std::string& GetChecksum(void);

	// SetExpectedChecksum is a setter for the expected checksum
    void SetExpectedChecksum(const std::string& aChecksum) {
        mExpectedChecksum = aChecksum;
    }

    // ChecksumIsOk returns true of the computed checksum is what
    // was expected.  Undefined behavior if the expected checksum isn't
    // set
    bool ChecksumIsOk(void) {
        return GetChecksum() == mExpectedChecksum;
    }

    // BytesChecksummed returns the total number of bytes checksummed
    uint32_t BytesChecksummed(void) {
        return mBytesChecksummed;
    }

private:
    EVP_MD_CTX *mCtx;				// Checksum contact
    const EVP_MD *mMd;				// Message Digest object
    std::mutex mDigestMutex;		// Mutex for access to mMd
    unsigned char mOutDigest[EVP_MAX_MD_SIZE];
    unsigned int mDigestLen;		// Length of output digest

    std::string mChecksum;			// Checksum/Digest in string form
    std::string mExpectedChecksum;	// Expected CS/Digest for check
    uint32_t mBytesChecksummed;     // Number of bytes checksummed
};

