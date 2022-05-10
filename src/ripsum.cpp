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

#include <getopt.h>
#include "CScheduler.h"



static int sgBinaryFlag {0};
static int sgCheckFlag {0};
static int sgTagFlag {0};
static int sgTextFlag {0};
static int sgZeroFlag {0};
static int sgIgnoreMissingFlag {0};
static int sgQuietFlag {0};
static int sgStatusFlag {0};
static int sgStrictFlag {0};
static int sgWarnFlag {0};
static int sgHelpFlag {0};
static int sgVersionFlag {0};

int main(int argc, char **argv) {
	CScheduler s;

	static struct option long_options[] = {
		{"binary", no_argument, &sgBinaryFlag, 1},
		{"check", no_argument, &sgCheckFlag, 1},
		{"tag", no_argument, &sgTagFlag, 1},
		{"text", no_argument, &sgTextFlag, 1},
		{"zero", no_argument, &sgZeroFlag, 1},
		{"ignore-missing", no_argument, &sgIgnoreMissingFlag, 1},
		{"quiet", no_argument, &sgQuietFlag, 1},
		{"status", no_argument, &sgStatusFlag, 1},
		{"strict", no_argument, &sgStrictFlag, 1},
		{"warn", no_argument, &sgWarnFlag, 1},
		{"help", no_argument, &sgHelpFlag, 1},
		{"version", no_argument, &sgVersionFlag, 1},
		{0, 0, 0, 0}
	};

	int option_index = 0;
	int c;

	do {
		c = getopt_long(argc, argv, "bctzw", long_options, &option_index);
		switch(c) {
			case -1: break; // -1 means no option found
			case 0: break; // 0 means long option detected and already handled
			case 'b': sgBinaryFlag = 1; break;
			case 'c': sgCheckFlag = 1; break;
			case 't': sgTextFlag = 1; break;
			case 'z': sgZeroFlag = 1; break;
			case 'w': sgWarnFlag = 1; break;
			default: abort();
		}
	} while(c != -1);

	while(optind < argc) {
		s.AddPath(argv[optind++]);
	}

	s.Run(sgCheckFlag);
	
	return 0;
}

