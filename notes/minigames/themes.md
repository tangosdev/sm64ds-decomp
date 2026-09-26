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
   - Proven: `dScMgCard_c`'s prompt handling in `src/minigames/d_s_mg_card.cpp`, `dScMgPachinko2_c`'s `Behavior` and `InitResources`, and `dScMg3DEsp_c::InitResources`.
   - Exception: when the singleton `data_ov004_020beb68` is declared `int`, mwcc loads a `+0x46xx` offset from the literal pool instead of splitting it as `add #0x4000` (see `func_ov004_020adafc`). So the accessor functions keep a `char *` base.
   - Access to the same block through `this` or a `dScMgBase_c *` matches: `dScMgBase_c::BeforeInitResources`, `BeforeBehavior` and `OnHitByMegaChar` all use the names.
2. **`char *c = (char *)this` inside a real member.**
   - Readable form: plain member access.
   - Proven: in `dScMgBase_c`, `dScMgPachinko2_c`'s `Behavior` and `InitResources`, and `dScMg3DEsp_c`'s `InitResources`, `OnYoshiTryEat` and `Render`.
   - Caution, [GUESS]: mixing `c` and `this` in one function may move register allocation, so convert a function all at once.
3. **Mangled `extern "C"` calls to real C++ methods.**
   - Readable form: a normal call through the header.
   - Proven for plain arguments: `Sound::StopLoadedMusic_Layer1` in `dScMgBase_c::AfterCleanupResources`, and in `dScMg3DEsp_c` for `TextureTransformer::Prepare`, `Animation::Advance` and the qualified destructor calls in D0 and D1.
   - Measured negative for native `u16` SDK arguments (`dScMgD3DBase_c`).
   - Measured negative for `Fix12<int>` by value: `TextureTransformer::SetFile` costs at least 12 bytes in `dScMg3DEsp_c::InitResources`.
   - Blocked, not measured, for `cstd::atan2`: it keeps its mangled name because no readable spelling links (`src/actors/dScMgHanachan_c.cpp`).
   - No header declares the SDK namespaces `GX`, `GXS` and `G2S`. A local namespace declaration compiles identically (the `InitResources` of `dScMgPachinko2_c` and `dScMg3DEsp_c`), but was not applied: a shared header is the readable form.
4. **Hand-built factories** (`fBase_c::operator new`, the base `C2`, then vptr stores).
   - Readable form: `return new dScMgX_c;`.
   - Proven: `dScMgMemory2_c`'s factory, and the one-function factories in `src/d_s_mg_bomroom.cpp`, `src/d_s_mg_curling.cpp`, `src/d_s_mg_curling2.cpp`, `src/d_s_mg_pachinko.cpp`, `src/d_s_mg_panel.cpp` and `src/d_s_mg_teresa.cpp`. The form is C++, so the file is a `.cpp` that starts with `//cpp`. rombuild picks the language from `//cpp`, so the rename from `.c` changed no byte.
   - Measured negative where the factory constructs a member that the header keeps as raw bytes, so the implicit constructor never calls it. Sizes are the `new` form against the ROM:
     - Coin: 0x34 against 0x48, without `func_0203b9b4(this + 0x51c4, 1)`. An inline class constructor making that call, tried in a scratch copy of the header, matches.
     - 3DEsp: 0x50 against 0x90, without the two `Model`, the `dMg3DEspModel_c` and the `TextureTransformer` constructors.
     - Flower: 0x50 against 0x94, without the `__cxa_vec_ctor` for `mArray` and without `func_ov006_020c3f54`.
     - Slot3: 0x50 against 0x60, without `func_ov006_020c221c`.
     - Roulette: 0x4c against 0xac, without the table, racer-array and two `Model` constructors.
     - Slot1: 0x58 against 0x88, without `func_ov006_0210c2b0` (twice) and `func_ov006_0210c208`.
   - Measured negative for Snowball: 0x60 against 0x34. The ROM calls the out-of-line constructor `func_ov006_021295ac`; with none declared, `new` inlines one. Declaring `dScMgSnowball_c();` out of line matches the bytes, but the link then needs that address named as the C1.
   - Unresolved for `dScMgSlot1_c`: its [class header](../../include/dScMgSlot1_c.h) records allocation/code-generation constraints. The absence of a standalone C1 alone does not rule out an inlined constructor.
5. **Pointer-to-member state tables.**
   - Readable form: a typedef over named state members.
   - Proven with the complete class in `dScMgCoin_c` and `dScMgMemory_c`.
   - `dScMg3DEsp_c::Behavior` matches with either receiver.
   - Measured to need an incomplete receiver in Curling and Curling2. Teresa keeps its receiver incomplete too.
   - Jump needs a non-polymorphic receiver instead.
   - Test the receiver per file.
6. **Shared ov004 functions called by address**, such as `func_ov004_020adb1c` (submit score) and `func_ov004_020b0a54` (enter a shared state).
   - Readable form: a coined name, renamed in place in ov004 `symbols.txt`.
   - A rename is byte-neutral by construction, but the declaration gate re-checks every caller.
   - Status: untested.
7. **The scene singleton `data_ov004_020beb68`, retyped in each file** (`int`, `char *[]`, `struct Obj *`).
   - Readable form: the one `extern` in `dScMgBase_c.h`, plus member access. Theme 1's `char *` exception applies.
   - Status: partly proven (`func_ov004_020b0a54`, and through `dScMgBase_c *` in `dScMg3DEsp_c::OnYoshiTryEat`).
8. **Local redeclarations that disagree with a header**, including the manifests' `CONFLICT` notes.
   - Readable form: one prototype in one header.
   - Every change here is matching work: rebuild and compare.
   - Proven: deleting a local copy that agrees with `decl_common.h` (`dScMgSingle3DBase_c`, `dScMgPachinko2_c::Render`, `dScMg3DEsp_c::InitResources`).
   - Blocked where the copies disagree: including `decl_common.h` in `dScMgPachinko2_c::Behavior` fails with "illegal function overloading" (`void *` against `char *` or `int`). `LoadFile` returns `int` at its definition and `void *` in Card and Memory.
9. **`add #0x4000` and `add #0x5000` split cursors.** A walk pointer is padded so that a field lands past 0x4000 or 0x5000.
   - Measured negative: `mCards[i]` did not match in `dScMgMemory_c`'s `DrawCards`, `RoundWaitDeal` and `ShuffleCards`. It does match in the class's other members.
   - Measured negative in `dScMgSingle3DBase_c::Virtual84`: both plainer spellings of its two `DecompressLZ16` destinations differ by 44 words.
   - Where it does not match, keep the cursor, with a one-line comment.
10. **The touch sample**, `{down, changed, x, y}` in `data_020a0de8`, is spelled as `u8[]`, as scalars, or as local structs.
    - Some local views name the bytes `{pressed, held}`, which is wrong: byte 1 flips on both press and release.
    - Readable form: one shared type.
    - Status: untested. It would change every consumer.
11. **Hand-rolled vtable views.**
    - Readable form: a real virtual call, such as `OnYoshiTryEat(-1)`.
    - Proven in `dScMgTrampoline2_c`, `dScMgCup_c` and `dScMg3DEsp_c` (`OnYoshiTryEat(-1)` in `InitResources`, `Model::Render(0)` in `Render`).
12. **The sine table `data_02082214`.** It holds interleaved fx16 sine and cosine pairs [ROM].
    - Readable form: a named table or accessor. [GUESS] It is NitroSDK's `FX_SinCosTable_`.
    - Status: untested.
13. **Resource loading** (`LoadFile`, `DecompressLZ16`, `FreeGfxSlotsById`, language tables).
    - Readable form: typed calls.
    - Proven: `dScMgCard_c`.
    - Measured negative: narrowing the file ID to `u16` grows `Virtual7C` by 16 bytes (`dScMgD3DBase_c`).
14. **Fix12 arithmetic.**
    - Spelling a value `Fix12i` (a plain `s32`) is safe.
    - Passing the `Fix12<int>` template by value is measured negative at the call site; see theme 3.
15. **Hardware registers by raw address.**
    - Readable form: the names in `nitro/hw/registers.h`.
    - Proven: `REG_DISPCNT` and `REG_DISPCNT_SUB` in `dScMgSingle3DBase_c::Virtual84`.
    - Registers the header does not name yet (the light and sub-BG3 registers there) stay raw.
