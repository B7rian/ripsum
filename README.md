# ripsum - A faster sha256sum
ripsum is meant to be a fast drop-in replacement for
```
find <dirs...> -type f -exec sha256sum {} +
```
(or equivalent) and

```
sha256sum -c <checksum file>
```

The name and features are inspired by
[ripgrep](https://github.com/BurntSushi/ripgrep) which has, overall, a much
better user experience than the find-grep commandlines it replaces (nothing
against find or grep of course). I hope ripsum is as nice to use as ripgrep
someday. 

# Usage
To generate checksums: `ripsum <dirs...>`

The check previously generated checksums in a file:: `ripsum -c <checksum
file>`

Checksum files are interchangable with sha256sum but don't support all features
yet. In particular, the mode character is ignored and everything is done in
binary mode. 

ripsum ignores symlinks.

# Build

Ripsum can be built on Linux with basic cmake commands. 
```
mkdir build && cd build
cmake ..
cmake --build 
```

You may need to install the libssl development package for your Linux distro.
On void, Ubuntu, and Ubuntu derivitives like Zorin it's called libssl-dev. 

# Testing

`make test` in the cmake build directory will run both sha256sum and ripsum and
compare the output. The tests are run on the build subdirectory by default
but you can change run_tests.sh script to use your own dataset.

# Performance

ripsum used to use a lot of threads, but the performance gains weren't 
there and I'm currently working on benchmarks to determine the best path
forward.

## CPU Utilization

One of the goals of ripsum is to use as many CPU core/threads as are needed to
take advantage of the I/O bandwidth on your system. You can infer that your I/O
bandwidth is saturated if ripsum is running a large job and CPU utilization is
less than 100% - in this case ripsum can't get the data off of the filesystem
fast enough to keep the CPU busy. 

Actual speedup vs. find+sha256sum depends on your system. For example, on
systems with slower cores and/or fast I/O, you might see 4 or more cores
heavily utilized and a speedup approaching 4.  On systems with faster cores
and/or slow I/O you might have alread saturated I/O with find+sha256sum and
ripsum won't actually be any faster. This is the case in the VirtualBox VM I'm
currently working in.

## Read block size

I did some experiments to find the optimal file read block size on my system
and the results are in the NOTES files. 256k seemed to work best but uses a lot
of RAM on large jobs. I would like to make this command-line controllable and
maybe even dynamic in the future.




