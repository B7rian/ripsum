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

//
// The classes below implement double-buffered file reading.
//
// Implementation Notes:
//   We use a pointer to the current filling and full buffer to
//   avoid having confusing branching logic in the methods that have
//   to pick a buffer.  Each buffer also knows the "Next" buffer (which
//   is just the other buffer), again avoiding potentially buggy
//   conditionals.  Instead, we have potentially buggy pointers but
//   they're relatively simple and therefore easy to manage.
//

//
// Buffer is just a single buffer with some metadata around it and the
// ability to read from a file.  This is here so we don't have 2 copies
// of every variable in File to manage double-buffering.
//

class Buffer {
public:
    Buffer(uint32_t aBlockSize): mBlockSize(aBlockSize) {}
    // InitBuffer sets up the buffer for future reads and stuff
    void InitBuffer(Buffer *apNextBuffer);
    // ReadBytes reads bytes from the stream into the buffer and
    // returns true if it gets anything
    bool ReadBytes(std::ifstream& aSin);
    void FinishBuffer(void) {
        delete[] mpData;
    }

    // GetData returns a pointer to the last-read data
    uint8_t *GetData(void) {
        return mpData;
    }
    // GetDataCount returns the number of bytes from the last read
    uint32_t GetDataCount(void) {
        return mDataCount;
    }
    // GetTotalBytes read returns the number of bytes read across
    // all reads so far
    uint32_t GetTotalBytesRead(void) {
        return mBytesRead;
    }
    // GetNextBuffer provides a pointer to the next buffer to use
    Buffer *GetNextBuffer(void) {
        return mpNext;
    }

private:
    uint32_t mBlockSize;            // Number of bytes to read each time
    Buffer *mpNext;                 // Pointer to next buffer in ring
    uint8_t *mpData;				// File data
    uint32_t mDataCount = 0;		// Bytes in file buffer
    uint32_t mBytesRead = 0;		// Total bytes read from file
};


//
// File is the file-related state that gets passed between the tasks inside
// the scheduler.  File implements double-buffering so we can read the file
// in parallel with processing in another class that we don't care about;
// We just provide the data and ping pong between our buffers for file
// reads and providing the data
//

class File {
public:
    File(const std::filesystem::path& aP, uint32_t aBlockSize)
        : mPath(aP), mPing(aBlockSize), mPong(aBlockSize) { }

    // InitFile sets up the file class to read the file
    void InitFile(void);
    // Readbytes reads some bytes from the file into an internal buffer
    void ReadBytes(void);
    // FileOk returns true if we should keep reading
    bool FileOk(void) {
        return mOk;
    }
    // Finish file cleans everything up
    void FinishFile(void);

    // GetPath is a simple accessor to get the file path
    auto GetPath(void) {
        return mPath;
    }
    // GetBytes provides a pointer to the latest read buffer and
    // returns the number of bytes in the buffer.  This is a bit
    // C-ish and can be made safer.
    uint32_t GetBytes(uint8_t* &aBytes);
    // GetTotalBytes read gives us the file size when Awe're all done
    uint32_t GetTotalBytesRead(void);

private:
    std::filesystem::path mPath;	// Path to file
    std::ifstream mSin;				// Input stream
    std::mutex mStreamMutex;        // Mutex for stream access
    Buffer mPing;					// 1st buffer
    Buffer mPong;					// 2nd buffer
    Buffer *mpReadBuffer;			// Buffer that we're reading into
    Buffer *mpFullBuffer;			// Buffer with data to process
    bool mOk;						// No errors and not EOF
};

