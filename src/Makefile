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

#
# Test/check stuff
#
sha256sum-expected.out:
	bash -c 'time find taskflow -type f -exec sha256sum {} + | sort -k 2 > sha256sum-expected.out'

sha256sum-check-expected.out: sha256sum-expected.out
	bash -c 'time sha256sum -c sha256sum-expected.out | sort > sha256sum-check-expected.out'

sha256sum-badline-check.out: ripsum-with-badline.out
	sha256sum -c ripsum-with-badline.out 2> sha256sum-badline-check.err | sort > sha256sum-badline-check.out 

sha256sum-badline-check.err: sha256sum-badline-check.out


ripsum.out: ripsum
	bash -c 'time ./ripsum taskflow | sort -k 2 > ripsum.out'

ripsum-check.out: ripsum ripsum.out
	bash -c 'time ./ripsum -c ripsum.out | sort > ripsum-check.out'

ripsum-with-badline.out: ripsum.out
	cp ripsum.out ripsum-with-badline.out
	echo bad line bad line bad line bad line >> ripsum-with-badline.out

ripsum-badline-check.out: ripsum-with-badline.out
	./ripsum -c ripsum-with-badline.out  2> ripsum-badline-check.err | sort > ripsum-badline-check.out
	
ripsum-badline-check.err: ripsum-badline-check.out

check: sha256sum-expected.out ripsum.out sha256sum-check-expected.out ripsum-check.out ripsum-badline-check.out sha256sum-badline-check.out ripsum-badline-check.err sha256sum-badline-check.err
	diff sha256sum-expected.out ripsum.out
	diff sha256sum-check-expected.out ripsum-check.out
	#diff sha256sum-badline-check.out ripsum-badline-check.out
	#diff sha256sum-badline-check.err ripsum-badline-check.err

clean:
	rm -f sha256sum-expected.out \
		ripsum.out \
		sha256sum-check-expected.out \
		ripsum-check.out \
		ripsum-with-badline.out \
		sha256sum-badline-check.out \
		sha256sum-badline-check.err \
		ripsum-badline-check.out \
		ripsum-badline-check.err \
		ripsum \
		*.o


