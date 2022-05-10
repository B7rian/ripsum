
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <openssl/evp.h>
#include "taskflow/taskflow.hpp"

//const std::filesystem::path ROOT { "/home/bwh/Pictures" };
const std::filesystem::path ROOT = u8"/run/media/bwh/Samsung_T5";
const int FILE_BLOCK_SIZE { 256*1024 };

std::string computeSha256Sum(const std::filesystem::path path) {
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	const EVP_MD *md = EVP_sha256();
	unsigned char buffer[FILE_BLOCK_SIZE];
    unsigned char outdigest[EVP_MAX_MD_SIZE];
	unsigned int len;

	EVP_DigestInit_ex(ctx, md, NULL);

	std::ifstream f(path, std::ios::binary);
	while(f.good()) {
	    f.read((char *)buffer, FILE_BLOCK_SIZE);
		EVP_DigestUpdate(ctx, buffer, f.gcount());
	}

	EVP_DigestFinal_ex(ctx, outdigest, &len);

	std::stringstream s;
	s << std::hex << std::setfill('0') << std::setw(2);
	for(int i = 0; i < len; i++) {
		s << std::setfill('0') << std::setw(2) << (unsigned int)outdigest[i];
	}

	std::string checksum = s.str();
	EVP_MD_CTX_free(ctx);
	return checksum;
}


void findFiles(const std::filesystem::path rootDir, 
		       std::function<void(std::filesystem::path)> fileCb) 
{
	std::list<std::filesystem::path> dirs;

	dirs.push_front(rootDir);

	while(!dirs.empty()) {
		std::filesystem::path dir = dirs.front();
		dirs.pop_front();
		for(auto& f: std::filesystem::directory_iterator(dir)) {
			std::filesystem::path p{f.path()};
			if(std::filesystem::is_regular_file(f)) {
				fileCb(p);
			}
			else if(std::filesystem::is_directory(f)) {
				dirs.push_front(p);
			}
		}
	}
}


//
// main
// 

union stuff {
	struct {
		std::filesystem::path path;
		std::string checksum;
	} s;
	char foo[64];

	stuff(std::filesystem::path p): s{p, ""} {}
	stuff(const stuff& in): s{in.s} {}
	~stuff(void) {}
};

int main(int argc, char **argv) {
	tf::Executor executor;
	tf::Taskflow taskflow;
	std::list<stuff> files;
	int done_count = 0;

	findFiles(ROOT,
		  [&](std::filesystem::path s) { 
		      stuff n { s };
			  files.push_front(n);
		  }
	);

	taskflow.for_each(files.begin(), files.end(), 
		[&](stuff& f) { 
			f.s.checksum = computeSha256Sum(f.s.path);
			done_count++;
			if((done_count % 1000) == 0) {
			    std::cerr << done_count << std::endl;
			}
		}
	);	

	executor.run(taskflow).wait();

	while(!files.empty()) {
		auto f = files.front();
		files.pop_front();
		std::cout << f.s.checksum << " " << f.s.path << std::endl;
	}
}
