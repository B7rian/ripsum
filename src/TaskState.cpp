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
#include <chrono>
#include <thread>
#include "TaskState.h"

void TaskState::Init(void) {
    InitHash();
}

void TaskState::Finish(void) {
    using namespace std::chrono_literals;

    while(BytesRead() != BytesHashed()) {
        //std::cerr << "Waiting for other tasks to complete" << std::endl;
        std::this_thread::sleep_for(25ms);
    }

    FinishHash();
}

void TaskState::AddBytesToHash(void) {
    uint8_t *pBuf;
    uint32_t n;

    std::lock_guard<std::mutex> lock(mGetAndHashMutex);
    n = GetBytes(pBuf);
    //std::cerr << "*" << n << std::endl;
    if(n > 0) {
        AddBytesToHash2(pBuf, n);
        CleanupBytes(pBuf);
    }
}

