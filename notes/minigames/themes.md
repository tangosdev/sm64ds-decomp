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
   - Exception: when the singleton `gMgScene` is declared `int`, mwcc loads a `+0x46xx` offset from the literal pool instead of splitting it as `add #0x4000`. A `char *` or `dScMgBase_c *` base splits like the cartridge.
   - Proven through the singleton cast to `dScMgBase_c *`: the score and record helpers `Mg_GetSessionBest`, `020adbc0`, `020adbe0`, `020ada40`, `020ad940` and `020b0b1c` in `src/minigames/d_s_mg_base.cpp`. `+0x4650` is now `mTopRecord`, read by name in `Mg_SetTopRecord` and `Mg_GetTopRecord`.
   - Access to the same block through `this` or a `dScMgBase_c *` matches: `dScMgBase_c::BeforeInitResources`, `BeforeBehavior` and `OpenMenu` all use the names.
   - Proven through an unreassigned `char *c = (char *)this` alias, 27 reads across Bomroom, Panel, Teresa, Trampoline2 and `dMg3DEspAnimSet_c` (pass 2, 2026-10-15). The `char *` exception above was disproved for a `dScMgBase_c *` base: `Mg_SetTopRecord` emits the same split.
2. **`char *c = (char *)this` inside a real member.**
   - Readable form: plain member access.
   - Proven: in `dScMgBase_c`, `dScMgPachinko2_c`'s `Behavior` and `InitResources`, and `dScMg3DEsp_c`'s `InitResources`, `StartRound` and `Render`.
   - Caution, [GUESS]: mixing `c` and `this` in one function may move register allocation, so convert a function all at once.
3. **Mangled `extern "C"` calls to real C++ methods.**
   - Readable form: a normal call through the header.
   - Proven for plain arguments: `Sound::StopLoadedMusic_Layer1` in `dScMgBase_c::AfterCleanupResources`, and in `dScMg3DEsp_c` for `dExtAnmTexSRT_c::Prepare`, `Animation::Advance` and the qualified destructor calls in D0 and D1.
   - Measured negative for native `u16` SDK arguments (`dScMgD3DBase_c`).
   - Measured negative for `Fix12<int>` by value: `dExtAnmTexSRT_c::SetFile` costs at least 12 bytes in `dScMg3DEsp_c::InitResources`. The Fix12-by-value `OAM::Render` overloads in `d_s_mg_base.cpp` stay mangled for the same reason.
   - Proven in the ov004 runtime: `ApproachLinear` and `ApproachLinear2` (`Mg_UpdateMenu`, the MgFx pool, `unit020b2c84`), `cstd::fdiv`, `Sound::LoadAndSetMusic_Layer1` (`Mg_RestoreMusic`), `OAM::Render` with a `Matrix2x2 *` and `OAM::RenderSub` (the sprite wrappers), and `Sound::PlayBank2_2D` in `dMgState_c` (through a local declaration; including `Sound.h` renumbers a pinned static, see theme 16).
   - Measured negative: `Sound::PlaySub` stays mangled in `d_s_mg_base.cpp`.
   - Blocked, not measured, for `cstd::atan2`: it keeps its mangled name because no readable spelling links (`src/minigames/d_s_mg_hanachan_methods.cpp`).
   - No header declares the SDK namespaces `GX`, `GXS` and `G2S`. A local namespace declaration compiles identically (the `InitResources` of `dScMgPachinko2_c` and `dScMg3DEsp_c`), but was not applied: a shared header is the readable form.
4. **Hand-built factories** (`fBase_c::operator new`, the base `C2`, then vptr stores).
   - Readable form: `return new dScMgX_c;`.
   - Proven: `dScMgMemory2_c`'s factory, and the factories of `dScMgBomroom_c`, `dScMgCurling_c`, `dScMgPachinko_c`, `dScMgPanel_c` and `dScMgTeresa_c`, once one-function `d_s_mg_*.cpp` files and now each folded into its class's TU under `src/game/actors/`, and the one-function factory in `src/minigames/d_s_mg_curling2.cpp`. The form is C++, so the file is a `.cpp` that starts with `//cpp`. rombuild picks the language from `//cpp`, so the rename from `.c` changed no byte.
   - Measured negative where the factory constructs a member that the header keeps as raw bytes, so the implicit constructor never calls it. Sizes are the `new` form against the ROM:
     - Coin: 0x34 against 0x48, without `func_0203b9b4(this + 0x51c4, 1)`. An inline class constructor making that call, tried in a scratch copy of the header, matches.
     - 3DEsp: 0x50 against 0x90, without the two `dExtSimpleModel_c`, the `dMg3DEspModel_c` and the `dExtAnmTexSRT_c` constructors.
     - Flower: 0x50 against 0x94, without the `__cxa_vec_ctor` for `mArray` and without `FlowerFace_Construct`.
     - Slot3: 0x50 against 0x60, without `Mascot_Construct`.
     - Roulette: 0x4c against 0xac, without the table, racer-array and two `dExtSimpleModel_c` constructors.
     - Slot1: 0x58 against 0x88, without `SlotBlink_Clear` (twice) and `SlotLamp_Clear`.
   - Resolved for Snowball: 0x60 against 0x34 with the constructor inlined, because the ROM calls an out-of-line one. Declaring `dScMgSnowball_c();` out of line matches the bytes, and the address is now named as the C1 (`_ZN15dScMgSnowball_cC1Ev`, with `SnowSpray` and `Vector2` typing the arrays it builds).
   - Unresolved for `dScMgSlot1_c`: its [class header](../../include/dScMgSlot1_c.h) records allocation/code-generation constraints. The absence of a standalone C1 alone does not rule out an inlined constructor.
5. **Pointer-to-member state tables.**
   - Readable form: a typedef over named state members.
   - Proven with the complete class in `dScMgCoin_c` and `dScMgMemory_c`.
   - `dScMg3DEsp_c::Behavior` matches with either receiver.
   - Measured to need an incomplete receiver in Curling and Curling2. Teresa keeps its receiver incomplete too.
   - Jump needs a non-polymorphic receiver instead.
   - Proven as typed member-pointer records assigned directly: `dMgState_c` (its 29 behavior/render records and the enter table) and `unit020b4aa4`'s `SpriteFn` records, including the `!=` compare in `Coin_Launch`.
   - Measured negative in `unit020b2c84`: a plain record copy in `Banner_Init` changes 12 words, so its `b ? a : a` copies stay. Typed copies in `Banner_StartSpin`, `020b39a4` and `020b422c` give identical text but move the name counter (theme 16), so the `W2` copies stay.
   - Test the receiver per file.
6. **Shared ov004 functions called by address**, such as `Mg_SubmitScore` (submit score) and `Mg_SetSharedState` (enter a shared state).
   - Readable form: a coined name, renamed in place in ov004 `symbols.txt`.
   - A rename is byte-neutral by construction, but the declaration gate re-checks every caller.
   - Status: untested. Batch 1 (2026-10-11) collected evidence-backed proposals for the ov004 helpers; they are applied as their own pass because every caller must be re-checked.
7. **The scene singleton `gMgScene`, retyped in each file** (`int`, `char *[]`, `struct Obj *`).
   - Readable form: the one `extern` in `dScMgBase_c.h`, plus member access.
   - Proven (pass 2, 2026-10-15): the header declares `extern "C" dScMgBase_c *gMgScene`. 25 `((dScMgBase_c *)gMgScene)->` casts are gone, and so are the three `char *` local copies in `d_s_mg_base.cpp` and `unit020b4aa4.cpp`. `char *g = (char *)gMgScene` aliases became `dScMgBase_c *` in Coin, Jump, Memory2 and Sound.
   - Measured negative (theme 16): the same alias retype in the `StartRound` of Luigi and Panel renumbers their pinned `@N` PMF records (Luigi `@2188` to `@2234`, Panel `@2234` to `@2288`), so those two keep `char *p`.
   - Still local: `d_mg_state.cpp` (`Base *`), `unit020b2c84.cpp` (`Obj *`) and `unit020bfec0.cpp` (`V *`), each through a TU-local view.
8. **Local redeclarations that disagree with a header**, including the manifests' `CONFLICT` notes.
   - Readable form: one prototype in one header.
   - Every change here is matching work: rebuild and compare.
   - Proven: deleting a local copy that agrees with `decl_common.h` (`dScMgSingle3DBase_c`, `dScMgPachinko2_c::Render`, `dScMg3DEsp_c::InitResources`, and every agreeing copy in `d_s_mg_base.cpp`). The ten local declarations of `sMgSpriteTables` there, in five types, are now one file-level `int *[]`.
   - Blocked where the copies disagree: including `decl_common.h` in `dScMgPachinko2_c::Behavior` fails with "illegal function overloading" (`void *` against `char *` or `int`). `LoadFile` returns `int` at its definition and `void *` in Card and Memory.
9. **`add #0x4000` and `add #0x5000` split cursors.** A walk pointer is padded so that a field lands past 0x4000 or 0x5000.
   - Measured negative: `mCards[i]` did not match in `dScMgMemory_c`'s `DrawCards`, `RoundWaitDeal` and `ShuffleCards`. It does match in the class's other members.
   - Measured negative in `dScMgSingle3DBase_c::InitEngines`: both plainer spellings of its two `DecompressLZ16` destinations differ by 44 words.
   - Where it does not match, keep the cursor, with a one-line comment.
10. **The touch sample**, `{down, changed, x, y}` in `gTouchHeld`, is spelled as `u8[]`, as scalars, or as local structs.
    - Some local views name the bytes `{pressed, held}`, which is wrong: byte 1 flips on both press and release.
    - Readable form: one shared type.
    - Status: untested. It would change every consumer.
11. **Hand-rolled vtable views.**
    - Readable form: a real virtual call, such as `StartRound(-1)`.
    - Proven in `dScMgTrampoline2_c`, `dScMgCup_c` and `dScMg3DEsp_c` (`StartRound(-1)` in `InitResources`, `dExtSimpleModel_c::Render(0)` in `Render`).
    - Proven in the ov004 runtime: `GraphCallback0`-`3`, `OnGraphCallback2`, the menu functions, the OAM sprite wrappers, `Mg_DrawTally` and `Mg_CallStartRound` in `d_s_mg_base.cpp` (the 27- and 29-slot stand-in views are gone, and so is the private header that held the 27-slot one), and `dMgState_c`'s scene slots 18-20 and `dFader_c` slots.
12. **The sine table `data_02082214`.** It holds interleaved fx16 sine and cosine pairs [ROM].
    - Readable form: a named table or accessor. [GUESS] It is NitroSDK's `FX_SinCosTable_`.
    - Status: untested.
13. **Resource loading** (`LoadFile`, `DecompressLZ16`, `FreeGfxSlotsById`, language tables).
    - Readable form: typed calls.
    - Proven: `dScMgCard_c`.
    - Measured negative: narrowing the file ID to `u16` grows `InitSubScreenBg` by 16 bytes (`dScMgD3DBase_c`).
14. **Fix12 arithmetic.**
    - Spelling a value `Fix12i` (a plain `s32`) is safe.
    - Passing the `Fix12<int>` template by value is measured negative at the call site; see theme 3.
15. **Hardware registers by raw address.**
    - Readable form: the names in `nitro/hw/registers.h`.
    - Proven: `REG_DISPCNT` and `REG_DISPCNT_SUB` in `dScMgSingle3DBase_c::InitEngines`.
    - Registers the header does not name yet (the light and sub-BG3 registers there) stay raw.
16. **The compiler's name counter.** mwccarm numbers generated names (`$N`, `@N`) from one counter per TU. Every named parameter (prototypes included), local, temporary and included declaration advances it, and several of these names are pinned in `symbols.txt` and the manifests.
    - Measured: including `dScMgBase_c.h` in `dMgState_c` renumbers the pinned `sEnterTable$184` to `$208` (`Sound.h` gives `$199`). Including `dScMgD3DBase_c.h` in `d_s_mg_base.cpp` renumbers its `@329` temps to `@529`. In `unit020b2c84`, named prototype parameters balance the count that keeps `__arraydtor$954`.
    - Measured (pass 2): an inline `dScMgBase_c::AddHudPoint` plus free `Mg_AddHudPoint`/`Mg_GetHudScore` in `dScMgBase_c.h`, with no call site using them yet, renumbers `__arraydtor$328` in `d_s_mg_base.cpp` to `$345` and the pinned temps of Jump, Jump2, Luigi and `dMg3DEspAnimSet_c`. The 13 copies of the capped score bump ("one point, capped at 9999, carried into the best") therefore stay spelled out.
    - A pass that adds or removes a name must re-check the pins. A plain object compare that matches the `@N`/`$N` names exactly is the check; normalizing them away hides exactly this failure. A header that gains declarations shifts every consumer's temps (#3725's `types.h` constructors moved two minigame TUs' PMF records by 14).
