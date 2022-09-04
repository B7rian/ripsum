#
# Copyright 2022 Brian W. Hughes
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CPP=g++
CPPFLAGS=-Ofast --std=c++17 
#CPPFLAGS=-g --std=c++17 
INCLUDES=-I taskflow

ripsum: CTaskState.o CFile.o CHash.o CFileSystem.o CScheduler.o CUserInput.o CChecksumLine.o CConsoleOutput.o ripsum.cpp
	$(CPP) $(CPPFLAGS) -o ripsum $(INCLUDES) ripsum.cpp CTaskState.o CFile.o CHash.o CFileSystem.o CScheduler.o CUserInput.o CChecksumLine.o CConsoleOutput.o -lcrypto -lpthread

CScheduler.o: CScheduler.cpp CScheduler.h
	$(CPP) $(CPPFLAGS) -c $(INCLUDES) -o CScheduler.o CScheduler.cpp

CConsoleOutput.o: CConsoleOutput.cpp CConsoleOutput.h
CFile.o: CFile.cpp CFile.h
CHash.o: CHash.cpp CHash.h
CFileSystem.o: CFileSystem.cpp CFileSystem.h
CTaskState.o: CTaskState.cpp CTaskState.h
CUserInput.o: CUserInput.cpp CUserInput.h
CChecksumLine.o: CChecksumLine.cpp CChecksumLine.h

clean:
	rm -f ripsum *.o


