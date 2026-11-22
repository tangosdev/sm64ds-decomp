# Matching the retail ROM

> **Status: historical snapshot.** The count below is frozen at the commit where
> this note was written. Use `README.md` and
> `python tools/progress.py --bar --from-src` for current matching progress.

Rebuild the retail `.nds` byte-for-byte from hand-written C compiled with mwccarm.
Matching is what makes the C the original logic.

**Done when:** `dsd rom build` turns the source back into the exact retail ROM,
every function, zero diffs.

When this note was written the tree stood at 7,504 / 11,390 functions.
