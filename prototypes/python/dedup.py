#!/usr/bin/env python3

# Not async:
# real	2m44.867s
# user	1m52.145s
# sys	0m15.622s
#
# 1st attempt at async:
# real	2m45.753s
# user	1m52.760s
# sys	0m15.691s
#
# Threads
# real	2m34.377s
# user	2m8.096s
# sys	0m18.934s
#


import asyncio
import hashlib
import os
import sys

MAX_FILES = 2000000
BLOCK_SIZE = 256*1024


class FileMetadata:
    """Information about a file"""

    def __init__(self, path):
        """Save the path for later, initialize cache for checksum"""
        self.path = path
        self.checksum = None

    def get_path(self):
        """Return the path to this file (including filename)"""
        return self.path

    def compute_sha256sum(self):
        """Compute the sha256sum of the file and return the hexdigest string"""
        # https://www.quickprogrammingtips.com/python/how-to-calculate-sha256-hash-of-a-file-in-python.html
        sha256_hash = hashlib.sha256()
        with open(self.path, "rb") as f:
            for byte_block in iter(lambda: f.read(BLOCK_SIZE), b""):
                sha256_hash.update(byte_block)
        self.checksum = sha256_hash.hexdigest()
    
    async def compute_all_sha256sums(file_list):
        await asyncio.gather(
            *[ asyncio.to_thread(FileMetadata.compute_sha256sum, f) 
                for f in file_list ])

    def get_sha256sum(self):
        return self.checksum


class DirTree:
    """List of stuff in a directory tree"""
    
    def __init__(self, root_dir):
        """Save the root_dir for later"""
        self.root_dir = root_dir

    def get_file_list(self):
        """Read the list of files from the given path (and below)"""
        # https://docs.python.org/3/library/os.html#os.scandir
        # https://docs.python.org/3/library/os.html#os.DirEntry
        files = []
        dirs = [ self.root_dir ]
        while(dirs and len(files) < MAX_FILES):
            with os.scandir(dirs[0]) as it:
                for entry in it:
                    name = entry.path
                    if(entry.is_dir()):
                        dirs.append(name)
                    elif(entry.is_file()):
                        files.append(FileMetadata(name))
                        if(len(files) % 1000 == 0):
                            print("Found {} files".format(len(files)),
                                    file=sys.stderr)
                    # Ignore everthing else
                dirs.pop(0)
        return files


class FileSet:
    """A set of files that all have the same checksum"""

    self_checks = 0

    def __init__(self, tag):
        """Save the tag for later"""
        self.tag = tag
        self.files = list()

    def get_tag(self):
        return self.tag

    def add_file(self, file):
        """Add a file to the set"""
        self.files.append(file)

    def get_files(self):
        """Get a list of files in the set"""
        return self.files

    def is_ok(self):
        """Make sure all files in the set have the same checksum"""
        for f in self.files:
            FileSet.self_checks += 1
            c = f.get_sha256sum()
            if(c != self.tag):
                print("Checksum for {} {} doesn't match tag {}".format(
                    f.get_path(), f.get_sha256sum(), self.tag))
                return False
        return True

    def get_best_file(self):
        """Get the file to save in the set"""
        return self.files[-1]

    def get_redundant_files(self):
        """Get the files we don't need"""
        return self.files[:-1]

    def print_summary(self):
        print(self.tag)
        for f in self.files:
            print("    rm {}".format(f.get_path()))


class FileSets:
    """All the sets of files with the same checksums"""

    self_checks = 0

    def __init__(self):
        """Initialize the object with an empty dict"""
        self.sets = dict()

    def add_file(self, file):
        """Add a FileMetadata object to the correct set"""
        s = file.get_sha256sum()
        if(s not in self.sets):
            self.sets[s] = FileSet(s) 
        self.sets[s].add_file(file)

    def checksums(self):
        """Return a list of checksums that we have files for"""
        return self.sets.keys()

    def files_with_checksum(self, checksum):
        """Return a list of files that have the given checksum"""
        if(checksum in self.sets):
            return self.sets[checksum].get_files()
        else:
            return []

    def is_ok(self):
        """Make sure all the files in each set have the same checksum"""
        for k,v in self.sets.items():
            FileSets.self_checks += 1
            if(k != v.get_tag()):
                print("Dict key {} doesn't match FileSet tag {}".format(
                    k, v.get_tag()))
                return False
            if(not v.is_ok()):
                # FileSet prints its own error message
                return False
        return True

    def get_best_files(self):
        """Get the file to save in the set"""
        return [ s.get_best_file() for s in self.sets.values() ]

    def get_redundant_files(self):
        """Get the paths to the files we don't need"""
        f = list()
        for s in self.sets.values():
            f.extend(s.get_redundant_files())
        return f

    def print_summary(self):
        for s in self.sets.values():
            s.print_summary()


async def main():
    #dir_tree = DirTree("/run/media/bwh/Samsung_T5")
    dir_tree = DirTree("/home/bwh/Pictures")
    file_sets = FileSets()
    file_list = dir_tree.get_file_list()
    await FileMetadata.compute_all_sha256sums(file_list);

    for f in file_list:
        print("{} {}".format(f.checksum, f.path))

#    for f in file_list:
#        file_sets.add_file(f)

#    if(file_sets.is_ok()):
#        print("FileSet self checks: {}".format(FileSet.self_checks), 
#                file=sys.stderr)
#        print("FileSets self checks: {}".format(FileSets.self_checks), 
#                file=sys.stderr)
#        print("Everything is great!", file=sys.stderr)
#    else:
#        print("Something is wrong!", file=sys.stderr)

#    file_sets.print_summary()

asyncio.run(main())

