
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

extern "C" {
#include "mhash.h"
}

const uint32_t MAX_FILES = 2000000000;
const uint32_t FILE_BLOCK_SIZE = 32768;

class FileMetadata {
public:
	FileMetadata(std::string& path) : mPath{path}, mChecksum{""} { }
	std::string getPath(void) { return mPath; }
	void computeSha256Sum(void);
	std::string getSha256Sum(void) { return mChecksum; }

private:
	std::string mPath {};
	std::string mChecksum {};
};


void FileMetadata::computeSha256Sum(void) {
	MHASH hash_handle;
	unsigned char *hash;
	unsigned char buffer[FILE_BLOCK_SIZE];
	size_t count;

	int hashBlockSize = mhash_get_block_size(MHASH_SHA256);

	hash_handle = mhash_init(MHASH_SHA256);

	std::ifstream f(mPath, std::ios::binary);
	while(f.good()) {
	    f.read((char *)buffer, FILE_BLOCK_SIZE);
		mhash(hash_handle, buffer, f.gcount());
	}

	hash = (unsigned char *)mhash_end(hash_handle);

	std::stringstream s;
	s << std::hex << std::setfill('0') << std::setw(2);
	for(int i = 0; i < hashBlockSize; i++) {
		s << std::setfill('0') << std::setw(2) << (unsigned int)hash[i];
	}
	mChecksum = s.str();

	delete hash;
}


class DirTree {
public:
	DirTree(std::string rootDir): mRootDir{rootDir} {}
	// list has O(1) performance for push_front
	// https://baptiste-wicht.com/posts/2012/12/cpp-benchmark-vector-list-deque.html
	void getFileList(std::list<FileMetadata>& files);

private:
	std::string mRootDir {};
};

void DirTree::getFileList(std::list<FileMetadata>& files) {
	std::list<std::string> dirs;

	dirs.push_front(mRootDir);

	while(!dirs.empty()) {
		std::string dir = dirs.front();
		dirs.pop_front();
		for(auto& f: std::filesystem::directory_iterator(dir)) {
			std::string f_string{f.path().string()};
			if(std::filesystem::is_regular_file(f)) {
				files.push_front(FileMetadata(f_string));
			}
			else if(std::filesystem::is_directory(f)) {
				dirs.push_front(f_string);
			}
		}
	}
}


int main(int argc, char **argv) {
	std::list<FileMetadata> files;
	DirTree dt("/run/media/bwh/Samsung_T5/Acer/Videos");
	//DirTree dt(".");

	dt.getFileList(files);

	for(auto& f: files) {
		f.computeSha256Sum();
		std::cout << f.getSha256Sum() << ' ' << f.getPath() << std::endl;
	}

	return 0;
}

