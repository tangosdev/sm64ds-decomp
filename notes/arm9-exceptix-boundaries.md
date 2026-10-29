# arm9 `.exceptix`: where C++ exceptions were on

arm9's `.exceptix` (0x020737a8..0x02073a24) holds 53 entries of 12 bytes each.
Only code built with C++ exceptions on emits an entry. So where entries start
and stop shows a change of build flags, and a change of build flags is a TU
boundary. This note maps those boundaries over 0x0206a000..0x020736f4. It folds
nothing; the last section says why.

## Entry format

Each entry is `{u32 start, u32 size_and_flag, u32 info}`, and `start` is
always the address of a function in `config/arm9/symbols.txt` (53 of 53).

- **Odd size** (bit 0 set): `info` is an inline unwind descriptor
  (`0x00100100`, `0x00200300`, ...). This is a function with a frame but no
  cleanup actions.
- **Even size**: `info` points into the `.exception` action tables at
  0x020736f4..0x020737a8. This is a function with cleanups or a try block:
  func_0206a928, func_02071d3c, func_02071f1c, func_02072014, func_02072168,
  func_020729f4, `__cxa_vec_cleanup`, func_02073300, `__cxa_vec_ctor`,
  func_02073470 and func_02073534.

`.text` ends at 0x020736f4, which is where `.exception` begins.

## The rule inside an exceptions-on TU

A function gets an entry exactly when it makes a call. Leaf functions get no
entry even if they push registers (func_0206ca7c, func_02071910,
`ReadSignedVarInt`, func_02072fcc). The two 0xc-byte entries (func_02071784
and func_02073220) are tail calls through `ldr ip, =target; bx ip`. So a function that
pushes `lr` and calls something but has **no** entry was built with exceptions
off. That is the signal used below. A leaf between two blocks cannot be
assigned to either one.

To rerun it: read the triples, then mark every function with a `bl`/`blx`
and no entry. The scratch script matched `bl`/`blx` words and
`stmdb sp!,{..lr}` pushes.

## The map

| Range | Exceptions | What is there | Edges |
|---|---|---|---|
| ..0x0206a928 | off | func_0206a29c..func_0206a88c all call, none has an entry | hard: func_0206a88c (calls, no entry) ends at 0x0206a928 |
| **0x0206a928..~0x0206d9cc** | **on** | the debug-print cluster: func_0206a928 (printf core, with an action table), its soft-double helpers func_0206bc8c/func_0206bdb4/func_0206c244/func_0206c51c, the stream callbacks, `src/cstd3.cpp` (`cstd::__assert` plus the card-wait helpers), func_0206ce90..func_0206d940 | top hard at 0x0206a928. Bottom: func_0206d940 is the last caller with an entry, and the leaves func_0206d9cc/func_0206da18 are undecidable, so the edge is at 0x0206d9cc, 0x0206da18 or 0x0206da64 |
| **~0x0206da64..~0x02071644** | **off** | `src/engine/cp15/CP154.cpp`, `src/cstd4.cpp` (including `__pformatter` func_0206e4a4, which the 2.0 compiler built, see `notes/arm9-msl-walls.md`), func_0206ece0..func_0206fd6c (`float2hex` among them), `src/cstd6.cpp` (str*), func_020707f4..func_020715e0 | 32 functions call with no entry. The last is func_020715e0 (ends at 0x02071644) |
| **~0x02071644..0x020736f4** | **on** | the C++ runtime: func_02071784, `__rethrow`, func_0207180c, func_02071864, `__end__catch`, func_02071be8..func_02072dac (func_02072168 is 0x88c with an action table, probably the unwinder), func_02072f3c..func_02073244, `__cxa_vec_cleanup`, func_02073300, `__cxa_vec_ctor`, func_02073470, func_02073534, `src/typeinfo.cpp` | bottom: the leaves func_02071644/func_02071698/func_02071708 are undecidable, so the edge is at 0x02071644, 0x02071698, 0x02071708 or 0x02071784. Top hard at 0x020736f4 (`.exception` begins) |

## What it does not tell you

- **TU edges inside a block.** Every function in a block was built with the
  same flags, so `.exceptix` is silent inside it. Inline descriptors and table
  pointers differ per function, not per TU.
- **Names.** The exceptions-on runtime block is clearly the Metrowerks C++
  runtime (exception handling, `__cxa_vec_*`, `typeinfo`). But the ROM keeps no
  file names, and we don't have the matching MSL source drop. Splitting the
  block into its original `.cp` files would be guesswork.

## Why nothing was folded

A fold needs two things: a boundary at both ends of a run, and a name for the
run. The only runs with both hard edges are whole blocks. Each block spans
several of the original files (the runtime block alone mixes `__rethrow`,
`__end__catch`, the unwinder, `__cxa_vec_*` and `typeinfo`), and no name source
splits them. An exceptions-on TU would also emit `.exceptix`/`.exception`
entries, and its manifest would have to claim them, which no arm9 manifest does
today. Revisit when an MSL source layout for this toolchain turns up. The
`__cxa_vec_*` neighbours (func_02073300, func_02073470, func_02073534) are the
first candidates, because they sit between two named runtime functions with
action tables of the same shape.
