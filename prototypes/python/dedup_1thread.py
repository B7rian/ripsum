#!/usr/bin/env python3

import hashlib
import os
import sys

max_files = 2000000000


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
            # Read and update hash string value in blocks of 4K
            for byte_block in iter(lambda: f.read(4096), b""):
                sha256_hash.update(byte_block)
            return sha256_hash.hexdigest()

    def get_sha256sum(self):
        if(self.checksum == None):
            self.checksum = self.compute_sha256sum()
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
        while(dirs and len(files) < max_files):
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
            if(f.get_sha256sum() != self.tag):
                print("Checksum for {} {} doesn't match tag {}".format(
                    f.get_path(), g.get_sha256sum(), self.tag))
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


dir_tree = DirTree("/run/media/bwh/Samsung_T5/Acer/Videos")
#dir_tree = DirTree(".")
file_sets = FileSets()
for f in dir_tree.get_file_list():
    c = f.get_sha256sum()
    print(c, f.get_path(), file=sys.stderr)

