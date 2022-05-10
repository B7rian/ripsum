
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <filesystem>

#include "picosha2.h"

const uint32_t MAX_FILES = 2000000000;
const uint32_t BLOCK_SIZE = 32768;

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
	std::ifstream f(mPath, std::ios::binary);
	std::vector<unsigned char> s(picosha2::k_digest_size);
	picosha2::hash256(f, s.begin(), s.end());
	picosha2::hash256_hex_string(s, mChecksum);
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

	dt.getFileList(files);

	for(auto& f: files) {
		f.computeSha256Sum();
		std::cout << f.getSha256Sum() << ' ' << f.getPath() << std::endl;
	}

	return 0;
}

