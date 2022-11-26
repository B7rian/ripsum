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
#include <filesystem>

#include "File.h"
#include "Checksum.h"

class RecipeState: public File, public Checksum {
public:
    RecipeState(const std::filesystem::path& aP, uint32_t aBlockSize)
        : File(aP, aBlockSize) { }

    void Finish(void);
    void AddBytesToChecksum(void);
private:
    std::mutex mGetAndChecksumMutex;
};

