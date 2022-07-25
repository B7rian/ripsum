# ripsum - Parallel/Multicore/Threaded sha256sum
ripsum is meant to be a fast drop-in replacement for
```
find <dirs...> -type f -exec sha256sum {} +
```
(or equivalent) and

```
sha256sum -c <checksum file>
```

The name and features are inspired by 
[ripgrep](https://github.com/BurntSushi/ripgrep)
which has, overall, a much better user experience
than the find-grep commandlines it replaces (nothing
against find or grep of course). I hope ripsum
is as pleasent to use as ripgrep someday. 

# Usage
To generate checksums: `ripsum <dirs...>`

The check previously generated checksums in a file:: `ripsum -c <checksum file>`

checksum files are interchangable with sha256sum but don't support all features yet. In 
particular, the mode character is ignored and everything is done in binary mode. 

ripsum ignores symlinks.

# Build

Ripsum can be built on Linux with a simple `make` and tested with
`make check`. 

You may need to install the libssl development package for your Linux 
distro. On void, Ubuntu, and Ubuntu derivitives like Zorin
it's called libssl-dev. 

ripsum uses [taskflow](https://taskflow.github.io) to run tasks on multiple cores and the needed
headers from taskflow are included in the ripsum git repo. 

# Testing

`make check` will run both sha256sum and ripsum and compare the output. 
The tests are run on the taskflow subdirectory by default but you can
change run_tests.sh script to use your own dataset. For example,
before release I run the tests on about 167000 files on 
a 1TB external drive and make sure the results match. 

# Performance

## CPU Utilization
ripsum utilize as much of your CPU as it can and will likely saturate the I/ O 
bandwidth on your system. You can infer that your I/O bandwidth is saturated if 
ripsum is running a large job and CPU utilization is less than 100% - in this case
ripsum can't get the data off of the filesystem fast enough to keep the CPU busy.

Actual speedup vs. find+sha256sum depends on your system. For example, on systems with slower cores 
and/or fast I/O, you might see 4 or more cores heavily utilized and a speedup approaching 
4.  On systems with faster cores and/or slow I/O you might saturate I/O with just 2 cores
and get a speedup less than 2. 

I am aware of jobflow and GNU parallel but have not benchmarked them. 

## Read block size

I did some experiments to find the optimal file read block size on my system and
the results are in the NOTES files. 256k seemed to work best but uses a lot of RAM on
large jobs. I would like to make this command-line controllable and maybe even dynamic 
in the future.




