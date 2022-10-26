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
#include <vector>

#include "ConsoleOutput.h"

//
// Windows NTFS paths are UTF-16.
// Linux paths are UTF-8
// I don't know what encoding is used when reading NTFS on Linux.
//
// C++ seems to think the encoding is OS-specific but it is filesystem-specific
// and can be different on 1 OS reading mixed filesystems.
//
// Conversion from UTF-16 is not defined (well?) in C++.  Come up with a better
// solution
//

void ConsoleOutput::NotifyGoodChecksum(const std::filesystem::path& aPath) {
    std::cout << aPath.generic_u8string();
    std::cout << ": OK";
    std::cout << '\n';
}

void ConsoleOutput::NotifyBadChecksum(const std::filesystem::path& aPath) {
    std::cout << aPath.generic_u8string();
    std::cout << ": FAILED";
    std::cout << '\n';
    mBadSums++;
}

void ConsoleOutput::NotifyChecksumReady(const std::filesystem::path& aPath,
                                        const std::string& aChecksum)
{
    std::cout << aChecksum
#if defined(__MINGW64__ ) || defined(__MINGW32__)
              << " *"
#else
              << "  "
#endif
              << aPath.generic_u8string()
              << '\n';
}

void ConsoleOutput::NotifyBadFileFormat(void) {
    mBadLines++;
}

void ConsoleOutput::Done(void) {
    if(mBadSums) {
        std::cerr << "sha256sum: WARNING: " << mBadSums << " computed ";
        std::cerr << ((mBadSums == 1) ? "checksum" : "checksums");
        std::cerr << " did NOT match" << '\n';
    }

    if(mBadLines > 0) {
        std::cerr << "sha256sum: WARNING: " << mBadLines;
        std::cerr << ((mBadLines == 1) ? " line is" : " lines are");
        std::cerr << " improperly formatted" << '\n';
    }
}

void ConsoleOutput::UserNeedsHelp(void) {
    std::vector<std::string> vHelp= {
        "Usage: sha256sum [OPTIONS] [FILE | DIRECTORY]...",
        "       sha256sum [OPTIONS] -c [FILE]...",
        "Print of check SHA256 (256-bit) checksums.",
        "",
        "-c, --check\tRead SHA256 sums from the FILEs and check them",
        "-s <size>\tRead <size> bytes at a time from the input file(s)",
        "         \tDefault is 8192",
        "--help\tdisplay this help and exit",
        "",
        "Sums are checked and computed using OpenSSL or whatever libcrypt",
        "is installed on your system. The generated output is compatible with",
        "sha256sum from GNU coreutils and can be used interchangably with",
        "sha256sum on GNU systems to verify results.",
        "",
        "Only binary mode is supported.",
    };

    for(auto& l: vHelp) {
        std::cout << l << '\n';
    }
}



