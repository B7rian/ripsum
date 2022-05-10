
#include <cstdint>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <mutex>

#include <openssl/evp.h>
#include "taskflow/taskflow.hpp"

//const std::filesystem::path ROOT { "." };
//const std::filesystem::path ROOT { "/home/bwh/Pictures" };
const std::filesystem::path ROOT = u8"/run/media/bwh/Samsung_T5";
const int FILE_BLOCK_SIZE { 16*1024 };

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

struct SharedState {
	std::filesystem::path path;		// Path to file

	std::ifstream sin;				// Input stream
	uint8_t buf[FILE_BLOCK_SIZE];	// File buffer data
	uint32_t buf_count;				// Bytes in file buffer
	uint32_t bytes_read;

	EVP_MD_CTX *ctx;
	const EVP_MD *md;
    unsigned char outdigest[EVP_MAX_MD_SIZE];
	unsigned int digest_len;

	SharedState(const std::filesystem::path& p): path(p) { }
};


//
// main
// 


int main(int argc, char **argv) {
	tf::Executor executor;
	tf::Taskflow taskflow;
	std::vector<SharedState> state;
	uint32_t file_count = 0;
	std::mutex mtx;

    tf::Task find_file_task = taskflow.emplace([&]() {
		findFiles(ROOT,
			[&](const std::filesystem::path s) { 
				state.emplace_back(s);
				file_count++;
				if((file_count % 1000) == 0) {
				    std::cerr << "Found " << file_count 
					          << " files" << std::endl;
				}
			}
		);
	}).name("find_files");

	tf::Task checksum_file_task = taskflow.emplace([&](tf::Subflow subflow) {
		for(auto& s: state) { 
			tf::Task a = subflow.emplace([&]() {
				s.bytes_read = 0;
				s.sin.open(s.path, std::ios::binary);

				{
					std::lock_guard<std::mutex> lock(mtx);
					s.ctx = EVP_MD_CTX_new();
					s.md = EVP_sha256();
					EVP_DigestInit_ex(s.ctx, s.md, NULL);
				}

			}).name("A");

			tf::Task b = subflow.emplace([&]() {
				s.buf_count = 0;
				if(s.sin.good() && !s.sin.eof()) {
					s.sin.read((char *)s.buf, FILE_BLOCK_SIZE);
					s.buf_count = s.sin.gcount();
					s.bytes_read += s.sin.gcount();
				}
			}).name("B");

			tf::Task c = subflow.emplace([&]() {
				if(s.buf_count == 0) {
				    return 1;
				}
				return 0;
			}).name("C");

			tf::Task d = subflow.emplace([&]() {
				EVP_DigestUpdate(s.ctx, s.buf, s.buf_count);
			}).name("D");

			tf::Task d2 = subflow.emplace([&]() {
				return 0;
			}).name("D2");

			tf::Task e = subflow.emplace([&]() {
				EVP_DigestFinal_ex(s.ctx, s.outdigest, &s.digest_len);

				std::stringstream str;
				str << std::hex << std::setfill('0') << std::setw(2);
				for(int i = 0; i < s.digest_len; i++) {
					str << std::setfill('0') << std::setw(2) 
						<< (unsigned int)s.outdigest[i];
				}

				std::string checksum = str.str();

				{
					std::lock_guard<std::mutex> lock(mtx);
					EVP_MD_CTX_free(s.ctx);
					std::cout << checksum << " " << s.path.native()
					          << " " << s.bytes_read;
					if(s.sin.eof()) {
					    std::cout << " EOF  ";
					}
					if(s.sin.good()) {
					    std::cout << " GOOD ";
					}
					if(s.sin.fail()) {
					    std::cout << " FAIL ";
					}
					if(s.sin.bad()) {
					    std::cout << " BAD  ";
					}
					std::cout << std::endl;

					s.sin.close();
			    }
				 
			}).name("E");

			a.precede(b);
			b.precede(c);
			c.precede(d, e);
			d.precede(d2);
			d2.precede(b);
		}
	}).name("checksum");

	checksum_file_task.succeed(find_file_task);
	executor.run(taskflow).wait();
	//taskflow.dump(std::cout);
}


