# Retire the obsolete ov023 initializer

Candidate: this commit. Base: `67bf07bbb364e06fe50de726ee2b2e99c51d1696`. Producer/integration owner:
`codex-overlay-root1016`. Independent source acceptance is recorded separately.

Remove only the old `__sinit_ov023_02111aa8.c` shard in overlay 023. The existing complete
`d_a_obj_fm_battan.cpp` entry owns text 021111a0..02111760, init
02111aa8..02111b14, ctor 02111b14..02111b18, data 02111fa0..02112064 and BSS
02112080..021120a8. Its manifest records the generated initializer and all ten
relocation sites/destinations. No enrollment or actor source change is needed.
Remaining occurrences of the old name are historical manifest/ratchet records,
not source, header or port callers. The active actor owner's source and ranges
were not changed by this deletion.

Deletion-only proof: `python tools/rombuild.py -j16 --no-rom` exited zero:
106/106 exact modules, 11334/11334 reproducing functions, 338/338 source-data
claims, 245 BSS claims. The scratch ROM-gap baseline retained its nine known
symbol diagnostics; no new diagnostics were introduced. The global data report
still has existing partial/differing rows; this deletion claims no repair of them.
`python tools/port_refcheck.py`: all 247 references resolve.
Declaration check correctly reported deletion-only scope; no declarations added.

This is removal of an unenrolled duplicate, not new reconstruction or a new TU
ownership claim. Attribution remains in function-authors.json unchanged.
Next action: independent review of this commit, then scoped publication; no merge.

Publication bookkeeping: retire only this obsolete source identity from the
converted baseline, with the tool-generated reason in the exception ledger.
The existing C++ owner and its compiler-generated initializer remain unchanged.
