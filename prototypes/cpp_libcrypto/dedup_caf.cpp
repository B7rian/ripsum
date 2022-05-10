
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <openssl/evp.h>
#include "caf/all.hpp"

struct PathMsg {
	std::filesystem::path path;
};

CAF_BEGIN_TYPE_ID_BLOCK(dedup_types, caf::first_custom_type_id)
	CAF_ADD_TYPE_ID(dedup_types, (PathMsg))
CAF_END_TYPE_ID_BLOCK(dedup_types)

const int NUM_ACTORS { 64 };
const std::filesystem::path ROOT { "/home/bwh/Pictures" };
//const std::filesystem::path ROOT = u8"/run/media/bwh/Samsung_T5";
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
// Wrap above classes in actor stuff
//

caf::behavior sha256sum_actor(caf::event_based_actor *self) {
	return {
		[=](const PathMsg path_msg) { 
			auto sum = computeSha256Sum(path_msg.path); 
			caf::aout(self) << sum << " " << path_msg.path.native() 
				            << std::endl;
		}
	};
}


//
// main
// 

void main_actor(caf::event_based_actor* self,
		        const std::vector<caf::actor>& actors) 
{
	findFiles(ROOT,
		  [=](std::filesystem::path s) { 
			  int actor_num = 0;
			  PathMsg m{s};
			  self->send(actors[actor_num], m);
			  actor_num = (actor_num + 1) % actors.size();
		  }
	);
}


void caf_main(caf::actor_system& sys) {
	std::vector<caf::actor> actors;

	for(int i = 0; i < NUM_ACTORS; i++) {
		actors.push_back(sys.spawn<caf::detached>(sha256sum_actor));
	}

	sys.spawn(main_actor, actors);
}

CAF_MAIN()

