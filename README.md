# ripsum - Better checksums
ripsum is meant to be a drop-in replacement for

```bash
find <dirs> -type f -exec sha256sum {} +
```

and


```
sha256sum -c <file>
```

The name and features are inspired by ripgrep, my reaction being something like
- "Wow, that was a lot faster than I expected!"
- "Hey the output is much more readable than what I'm used to"

and after some time it was just really nice not to have to type so much.


