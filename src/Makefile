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

ripsum: CTaskState.o CFile.o CHash.o CFileSystem.o CScheduler.o CUserInput.o CChecksumLine.o ripsum.cpp
	$(CPP) $(CPPFLAGS) -o ripsum $(INCLUDES) ripsum.cpp CTaskState.o CFile.o CHash.o CFileSystem.o CScheduler.o CUserInput.o CChecksumLine.o -lcrypto -lpthread

CFile.o: CFile.cpp CFile.h
	$(CPP) $(CPPFLAGS) -c -o CFile.o CFile.cpp

CHash.o: CHash.cpp CHash.h
	$(CPP) $(CPPFLAGS) -c -o CHash.o CHash.cpp

CFileSystem.o: CFileSystem.cpp CFileSystem.h
	$(CPP) $(CPPFLAGS) -c -o CFileSystem.o CFileSystem.cpp

CTaskState.o: CTaskState.cpp CTaskState.h
	$(CPP) $(CPPFLAGS) -c -o CTaskState.o CTaskState.cpp

CScheduler.o: CScheduler.cpp CScheduler.h
	$(CPP) $(CPPFLAGS) -c $(INCLUDES) -o CScheduler.o CScheduler.cpp

CUserInput.o: CUserInput.cpp CUserInput.h
	$(CPP) $(CPPFLAGS) -c -o CUserInput.o CUserInput.cpp

CChecksumLine.o: CChecksumLine.cpp CChecksumLine.h
	$(CPP) $(CPPFLAGS) -c -o CChecksumLine.o CChecksumLine.cpp

clean:
	rm -f ripsum *.o


