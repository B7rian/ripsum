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
#include <mutex>
#include <list>

//
// File is the file-related state that gets passed between the tasks inside
// the scheduler.  
//

class File {
public:
    File(const std::filesystem::path& aP, uint32_t aBlockSize);
    ~File(void);
 
    // GetPath is a simple accessor to get the file path
    auto GetPath(void) { return mPath; }
 
    // FileOk returns true if we should keep reading
    bool FileOk(void) { return mOk; }

    // ReadBytes reads some bytes from the file into an internal buffer
    // Returns the number of bytes read
    uint32_t ReadBytes(void);

    // GetBytes provides a pointer to the latest read buffer and
    // returns the number of bytes in the buffer.  This is a bit
    // C-ish and can be made safer.
    uint32_t GetBytes(uint8_t* &apBytes);

    // CleanupBytes cleans up the buffer given buffer
    void CleanupBytes(uint8_t* apBytes);

    // BytesRead returns the number of bytes read
    uint32_t BytesRead(void) { return mBytesRead; }

private:
    struct Buffer {                 // An entry in the buffer queue
        uint8_t *pBytes;
        uint32_t mCount;
    };

    std::filesystem::path mPath;	// Path to file
    uint32_t mBlockSize;            // Size of a single block of data
    bool mOk;						// No errors and not EOF
    uint32_t mBytesRead;            // Number of bytes read

    std::ifstream mSin;				// Input stream
    std::mutex mStreamMutex;        // Mutex for stream access

    std::list<Buffer> mlBuffers;    // A collection of buffers
    std::mutex mBuffersMutex;       // Mutex for buffer collection access
};



