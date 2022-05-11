# ripsum - Better checksums
ripsum is meant to be a drop-in replacement for
```
find <dirs...> -type f -exec sha256sum {} +
```
(or equivalent) and

```
sha256sum -c <checksum file>
```

The name and features are inspired by ripgrep, my initial reaction to ripgrep being 
something like
- "Wow, that was a lot faster than I expected!"
- "Hey the output is much more readable than what I'm used to"

and after some time it was just really nice not to have to type so much.  I hope ripsum
is as pleasent to use as ripgrep someday.

# Usage
To generate checksums: `ripsum <dirs...>`

The check previously generated checksums in a file:: `ripsum -c <checksum file>`

checksum files are interchangable with sha256sum

ripsum ignores symlinks.

# Performance

## CPU Utilization
ripsum utilize as much of your CPU(s) as it can and will likely saturate the I/ O 
bandwidth on your system. You can infer that your I/O bandwidth is saturated if 
ripsum is running a large job and CPU utilization is less than 100% - in this case
ripsum can't get the data off of the filesystem fast enough to keep the CPU(s) busy.

Actual speedup vs. find+sha256sum depends on your system. On systems with slower cores 
and/or fast I/O, you might see 4 or more cores heavily utilized and a speedup approaching 
4.  On systems with faster cores and/or slow I/O you might saturate I/O with just 2 cores
and get a speedup less than 2. 

I am aware of jobflow and GNU parallel but have not benchmarked them. 

## Read block size

I did some experiments to find the optimal file read block size on my system and
the results are in the NOTES files. 256k seemed to work best but uses a lot of RAM on
large jobs. I would like to make this command-line controllable and maybe even dynamic 
in the future.

# Known issues / To do
- Uses too much memory
- Not all sha256sum and md5sum and other *sum command line options fully implemented
- Only sha256 is supported
- Return codes not implemented
- Error handling not implemented
	- May not handle missing files when -c is used
- Performance: Big files scheduled late extend the total runtime unnecessarily
- Performance: Initial directory traversal to build file list runs in single core and can peg that core for a few seconds unnecessarily
- Probably Linux only.  I haven't tried anything else.
- OpenSSL 1.1 API
- Read block size optimized form my development system




