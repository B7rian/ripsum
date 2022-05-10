
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

const std::filesystem::path ROOT { "." };
//const std::filesystem::path ROOT { "/home/bwh/Pictures" };
//const std::filesystem::path ROOT = u8"/run/media/bwh/Samsung_T5";
const int FILE_BLOCK_SIZE { 16*1024 };

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


struct state {
	std::filesystem::path path;
	std::ifstream in_stream;
	uint8_t buf[FILE_BLOCK_SIZE];
	uint32_t buf_count;

	EVP_MD_CTX *ctx;
	const EVP_MD *md;
    unsigned char outdigest[EVP_MAX_MD_SIZE];
	unsigned int digest_len;

	tf::Task file_init_task;
	tf::Task stream_good_task;
	tf::Task read_buffer_task;
	tf::Task sha256sum_task;
	tf::Task file_done_task;

	state(const std::filesystem::path& p): path{p} { }
};


//
// main
// 

int main(int argc, char **argv) {
	tf::Executor executor;
	tf::Taskflow taskflow;
	std::vector<state> files;

    tf::Task find_file_task = taskflow.emplace([&]() {
		findFiles(ROOT,
			[&](const std::filesystem::path& s) { 
				files.emplace_back(s);
			}
		);
	}).name("find_files");

	tf::Task checksum_file_task = taskflow.emplace([&](tf::Subflow& sf) {
		for(auto& f: files) {

			f.file_init_task = sf.emplace([&](){
				std::cout << "Init " << f.path << std::endl;
				f.in_stream.open(f.path, std::ios::binary);
				//f.ctx = EVP_MD_CTX_new();
				//f.md = EVP_sha256();
				//EVP_DigestInit_ex(f.ctx, f.md, NULL);
			}).name("init_" + f.path.string());

			f.stream_good_task = sf.emplace([&]() {
				if(f.in_stream.good()) {
					std::cout << "Good " << f.path << std::endl;
					return 0;
				}
				std::cout << "BAD " << f.path << std::endl;
				return 1;
			}).name("stream_good_" + f.path.string());

			f.read_buffer_task = sf.emplace([&]() {
				std::cout << "Read " << f.path << std::endl;
				f.in_stream.read((char *)f.buf, FILE_BLOCK_SIZE);
				f.buf_count = f.in_stream.gcount();
			}).name("read_" + f.path.string());

			f.sha256sum_task = sf.emplace([&]() {
				std::cout << "Sum " << f.path << std::endl;
				//EVP_DigestUpdate(f.ctx, f.buf, f.buf_count);
			}).name("sum_" + f.path.string());

			f.file_done_task = sf.emplace([&]() {
				std::cout << "Done " << f.path << std::endl;
			}).name("done_" + f.path.string());

			f.stream_good_task.precede(f.read_buffer_task, f.file_done_task)
				              .succeed(f.file_init_task);
			f.read_buffer_task.precede(f.sha256sum_task);
			f.sha256sum_task.precede(f.stream_good_task);
		}
	}).name("checksum");

	tf::Task all_done_task = taskflow.emplace([]() { 
		std::cout << "All Done" << std::endl;
	}).name("all_done");

	checksum_file_task.succeed(find_file_task)
		              .precede(all_done_task);
	    
	executor.run(taskflow).wait();
	//taskflow.dump(std::cout);
}


