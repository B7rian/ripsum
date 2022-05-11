# ripsum - Better checksums
ripsum is meant to be a drop-in replacement for

```
find <dirs> -type f -exec sha256sum {} +
```
and

```
sha256sum -c <file>
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
ripsum will make full use of your CPU(s) and will likely saturate the I/O bandwidth on your
system. You can infer that your I/O bandwidth is saturated if ripsum is running a large
job and CPU utilization is less than 100%.

Actual speedup vs. find-sha256sum depends on your system. On systems with slower cores 
and/or fast I/O, you might see 4 or more cores heavily utilized and a speedup approaching 
4.  On systems with faster cores and/or slow I/O you might saturate I/O with just 2 cores
and get a speedup less than 2. 

# Known issues
- Not all sha256sum command line options implemented
- Only sha256sum is supported
- Return codes not implemented
- Error handling not implemented
	- May not handle missing files when -c is used
- Performance: Big files scheduled late extend the total runtime unnecessarily
- Performance: Initial directory traversal to build file list runs in single core and can peg that core for a few seconds unnecessarily
- Linux only?
- OpenSSL 1.1 API



