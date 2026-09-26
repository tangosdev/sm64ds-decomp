# Minigame readability themes

These rewrites recur across the minigame suite. Each theme gives the machine-shaped
pattern, its readable form, and its **status**:

- **proven**: some file in the tree already uses the readable form and still matches the ROM. The theme cites it.
- **measured negative**: the readable form was compiled and changed bytes. Keep the current form with a one-line reason.
- **untested**: try it, and record the result here.

## How a pass is checked

A pass must not change a single emitted byte or relocation target.
- **Object comparison.** Compare the TU object built before and after the pass: every section, every relocation (offset, type, symbol), and the defined and undefined symbol sets.
- **Renames.** A symbol rename is allowed only as an in-place `symbols.txt` rename at the same address.
- **Gates.** Also run `tubuild verify`, `rombuild`, `prepush_linkcheck` and `check_decl_agreement --changed`.
- **Updating this file.** When a pass proves or disproves a theme, update the entry, citing the function that shows it, not a line number.

## The themes

1. **Inherited base fields by name.**
   - Before: `*(u8 *)(c + 0xc3) = 1`.
   - After: `mPromptEnabled = 1`.
   - Proven: `dScMgCard_c`'s prompt handling in `src/minigames/d_s_mg_card.cpp`.
   - Exception: the score block at `+0x46xx` must be reached through a `char *` base, or the compiler loads the offset from the literal pool instead of splitting it as `add #0x4000` (see `func_ov004_020adafc`).
2. **`char *c = (char *)this` inside a real member.**
   - Readable form: plain member access.
   - Proven: in `dScMgBase_c`. Untested in each game.
   - Caution: mixing `c` and `this` in one function moves register allocation, so convert a function all at once.
3. **Mangled `extern "C"` calls to real C++ methods.**
   - Readable form: a normal call through the header.
   - Proven for plain arguments: `Sound::StopLoadedMusic_Layer1` in `dScMgBase_c::AfterCleanupResources`.
   - Measured negative for `Fix12<int>` passed by value (`cstd::atan2` in `src/minigames/d_s_mg_memory.cpp`) and for native `u16` SDK arguments (`dScMgD3DBase_c`).
4. **Hand-built factories** (`fBase_c::operator new`, the base `C2`, then vptr stores).
   - Readable form: `return new dScMgX_c;`.
   - Proven: `dScMgMemory2_c`'s factory.
   - Not possible for `dScMgSlot1_c`: the ROM has no C1 for it to link.
5. **Pointer-to-member state tables.**
   - Readable form: a typedef over named state members.
   - Proven with the complete class in `dScMgCoin_c` and `dScMgMemory_c`.
   - Measured to need an incomplete receiver in Curling, Curling2, Jump and Teresa. Test the receiver per file.
6. **Shared ov004 functions called by address**, such as `func_ov004_020adb1c` (submit score) and `func_ov004_020b0a54` (enter a shared state).
   - Readable form: a coined name, renamed in place in ov004 `symbols.txt`.
   - A rename is byte-neutral by construction, but the declaration gate re-checks every caller.
   - Status: untested.
7. **The scene singleton `data_ov004_020beb68`, retyped in each file** (`int`, `char *[]`, `struct Obj *`).
   - Readable form: the one `extern` in `dScMgBase_c.h`, plus member access. Theme 1's `char *` exception applies.
   - Status: partly proven (`func_ov004_020b0a54`).
8. **Local redeclarations that disagree with a header**, including the manifests' `CONFLICT` notes.
   - Readable form: one prototype in one header.
   - Every change here is matching work: rebuild and compare.
   - Status: untested.
9. **`add #0x4000` and `add #0x5000` split cursors.** A walk pointer is padded so that a field lands past 0x4000 or 0x5000.
   - Measured negative: `mCards[i]` did not match in `dScMgMemory_c`.
   - Keep the cursor, with a one-line comment.
10. **The touch sample**, `{pressed, held, x, y}` in `data_020a0de8`, is spelled as `u8[]`, as scalars, or as local structs.
    - Readable form: one shared type.
    - Status: untested. It would change every consumer.
11. **Hand-rolled vtable views.**
    - Readable form: a real virtual call, such as `OnYoshiTryEat(-1)`.
    - Proven in `dScMgTrampoline2_c` and `dScMgCup_c`.
12. **The sine table `data_02082214`.** It holds interleaved fx16 sine and cosine pairs [ROM].
    - Readable form: a named table or accessor. [GUESS] It is NitroSDK's `FX_SinCosTable_`.
    - Status: untested.
13. **Resource loading** (`LoadFile`, `DecompressLZ16`, `FreeGfxSlotsById`, language tables).
    - Readable form: typed calls.
    - Proven: `dScMgCard_c`.
    - Measured negative: widening a file ID to `u16` (`dScMgD3DBase_c`).
14. **Fix12 arithmetic.**
    - Spelling a value `Fix12i` (a plain `s32`) is safe.
    - Passing the `Fix12<int>` template by value is not; see theme 3.
