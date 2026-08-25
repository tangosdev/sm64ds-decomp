# Assessment: reusing `cybervisi0n/pokeplatinum@pc_port` + the libntr suite for an SM64DS PC port

*Status: research + plan, not yet actioned. Slots into [`roadmap.md`](roadmap.md)
Phases 2–3 (LIFT / PORT). Investigated 2026-08-01; revised after adversarial review.*

---

## 1. What the pokeplatinum PC port actually is

The headline is not the pokeplatinum fork. It is **libntr**, and the fork is mostly a
demonstration that libntr works.

| Repo | What it is | Fork of | License |
|---|---|---|---|
| [`cybervisi0n/libntr`](https://github.com/cybervisi0n/libntr) | "Drop-in replacement for NitroSDK that targets Nintendo DS, PC, and other platforms" | `ntrtwl/NitroSDK` | **none** |
| `cybervisi0n/libntrsystem` | Same for NitroSystem (NNS) | `ntrtwl/NitroSystem` | **none** |
| `cybervisi0n/libntrwifi` / `libntrdwc` / `libvct` | WiFi / DWC / VoiceChat | `ntrtwl/*` | **none** |
| `cybervisi0n/pokeplatinum@pc_port` | The pret decomp wired to the above | `pret/pokeplatinum` | none |

**The fork's diff against `pret/pokeplatinum@main` is 61 commits / 318 files.** (The
GitHub compare API caps its `files` array at 300, so the round "300" you get from
`gh api .../compare` is a truncation artifact — the real count comes from the `.diff`.)
That number is small on purpose: libntr absorbs the platform work, so the game tree only
needs targeted edits. Everything port-specific *in the game tree* lives under a new
`src/port/` directory — 12 files, all ImGui debug panels plus a charcode converter. No
engine work.

### libntr's architecture

libntr reimplements the NitroSDK source tree (`libraries/os`, `mi`, `gx`, `fs`, `fx`,
`card`, `snd`, `spi`, `std`, `wm`, `mb`, `math`, `cp`, …) for ARM, x86_64 Linux, x86_64
Windows and Switch, plus a host simulator (`libraries/sim`, `libraries/sim7`). Under
`SDK_PORT` it swaps in X86 hardware headers that turn every MMIO register into a host
global and every DS memory region into a host array:

```c
/* include/nitro/hw/X86/ioreg_G3.h */
extern REGType32v s_reg_G3_MTX_MODE;
#define reg_G3_MTX_MODE (*(REGType32v *)(&s_reg_G3_MTX_MODE))

/* include/nitro/hw/X86/mmap_global.h */
extern u8 s_HW_MAIN_MEM[0x800000];        // PC Port: doubled from 0x400000
#define HW_MAIN_MEM (u64)(s_HW_MAIN_MEM)
```

The simulator then consumes that state: **DS 3D → OpenGL 4.2 with upscaling**, DS 2D in
software, Dear ImGui debug UI, local wireless over LAN. This is Phase 3 of our roadmap,
already built and shipping a playable game.

---

## 2. How much of it transfers to SM64DS

### 2a. libntr has *two different* interception levels — and that is the whole story

The 2D and 3D paths work completely differently, and conflating them mis-sizes the port.

**2D is register-level. A header swap genuinely works.** `libraries/sim/src/sim_g2.c`
rescans the register globals every frame and rasterises from them:

```c
dispcnt = s_reg_GX_DISPCNT;                        /* :291, :532 */
u32 dispSel = s_reg_GX_POWCNT >> 15;               /* :412, :454 */
xoff += (s_reg_G2_BG0OFS & 0x1ff);  bgcnt = s_reg_G2_BG0CNT;   /* :494-496 */
u16 win0H = isSub ? s_reg_G2S_DB_WIN0H : s_reg_G2_WIN0H;       /* :668 */
```

Plus `s_HW_OAM` and `s_HW_BG_VRAM`. So for the 2D engine — **our largest register
surface by reference count** — raw stores to `0x04000000`/`0x04001000` really do render,
provided the address resolves to libntr's global instead of a literal.

**3D is API-level. Raw register writes are dead.** In `include/nitro/gx/g3imm.h` every
immediate-mode inline is `SDK_PORT`-switched:

```c
static inline void G3_MtxMode (GXMtxMode mode)
{
#ifdef SDK_PORT
    draw_msg_t *msg = (draw_msg_t *)malloc(sizeof(draw_msg_t));
    msg->type = DRAW_CMD_G3_MTXMODE;
    msg->data.numU8 = mode;
    SIM_HandleG3Command(msg);
    free(msg);
#else
    reg_G3_MTX_MODE = GX_PACK_MTXMODE_PARAM(mode);
#endif
}
```

`s_reg_G3X_GXFIFO` has no readers; there is no drain, poll, or fault handler. Generic
`MI_DmaSend32` under `SDK_PORT` degenerates to a `memcpy` into a dead 4-byte global
(`libraries/mi/src/mi_dma.c:177`). Several 3D entry points have **no port branch at all**
— `G3_Normal`, `G3_LightVector`, `G3_LightColor`, `G3_End`, `G3_MultMtx44/33`,
`G3_Shininess`, `MI_SendGXCommandFast` — and `GX_SendFifo48B/64B/128B` are explicit
no-ops (`libraries/gx/src/gxasm.c:4`).

**But there is a third seam the first pass missed, and it is the one that fits us.**
libntr has a packed **display-list decoder**: `DecodeG3Op` (`sim_main.c:369-454`) behind
`DRAW_CMD_G3_CMD_LIST` (:951), fed by `MI_SendGXCommand` (:54). SM64DS drives the
geometry engine exactly this way — command word then packed params to the FIFO, and DMA
straight to it:

```c
/* src/Geometry_MatrixMultiply3x3.c:5 */   *(volatile u32 *)0x4000400 = 0x1a;
                                           Copy36Bytes(m, (int *)0x4000400);
/* src/func_0205a290.c:35 */               DMAStartTransfer(ch, base, 0x4000400, 0x84400000 | ...);
```

So the 3D seam for us is the **command stream**, not per-call immediate inlines.

**Failure mode correction:** dead registers do not produce a blank screen, they produce a
**hang**. The game spin-waits on hardware — `while (*(volatile unsigned *)(0x40000b0 + (ch*3+2)*4) & 0x80000000);`
(`src/func_02059fa8.c:2`), `while (IPCSend(7,0,0) < 0);` (`src/func_0205ae30.c`). Any
register the host layer leaves inert is a deadlock in the boot path.

### 2b. Our tree is *not* pre-wrapped in SDK-shaped inlines

**This corrects the first draft's central claim, which was wrong.** I generalised from
two sample files. Measured over the whole tree:

- **317 files** contain real MMIO accesses — **1,959 references** (measured by
  `port/tools/mmio_inventory.py` on the unlanded branch `port/salvage-port-repo-tools`;
an earlier "~670" was a regex artifact, see the appendix).
- **3 files** — `func_0204af3c.c`, `func_ov007_020c1448.c`, `func_ov007_020ca86c.c` —
  define *any* named `G3_`/`GX_`/`G2_` inline. Not "overwhelmingly." Three.
- The norm is a bare store, sometimes not even volatile:
  `*(volatile int *)0x4000440 = 2;` (`src/func_ov006_020d09e0.c:9`),
  `src/func_ov080_02125fd0.c:16`.

So "the decomp has incidentally done most of the API-recovery a port needs" was the
opposite of true, and Phase 2 cannot be sized as "swap a header."

**Register surface, measured:**

| Block | Addresses | Refs | Seam under libntr |
|---|---|---|---|
| 2D engines A/B | `0x04000000`/`0x04001000` DISPCNT, `…08/0a/0c/0e` BGxCNT, `…50` BLDCNT | ~1,400 | **register-level — header swap works** |
| 3D geometry | `0x0440` MTX_MODE, `0x046c` SCALE, `0x04a4` POLYGON_ATTR, `0x04a8` TEXIMAGE_PARAM, `0x0400` GXFIFO, `0x0600` GXSTAT | ~250 | **command-stream level** |
| CP divider/sqrt | `0x0280`–`0x02b8` | ~60 | trivial; libntr `libraries/cp` |
| IRQ / power / VRAM bank | `0x0208` IME, `0x0304` POWCNT, `0x0240`–`46` VRAMCNT | ~130 | boot plumbing; **spin-wait hazard** |

*(The ~1,400 2D total is a family sum I have not individually audited; DISPCNT alone is
~258 raw references.)*

### 2c. NitroSystem: G2d/G3d unused, but the heap layer is NNS-shaped

The first draft said "no NNS at all," based on `grep -rl "NNS_\|nnsys" src` returning 0.
That grep proves nothing in a tree that names unknowns `func_XXXX`. Re-examined:

- **G3d/G2d: genuinely absent.** The BMD loader checks no magic anywhere, `BMD_File.h`
  is a flat count/offset header, and name lookup is linear `strcmp`
  (`src/func_020471ac.c`) where NNS G3d uses radix dictionaries. Hand-rolled EAD readers.
- **FND heaps: NNS under EAD names.** The signatures are literal NNS FND tags —
  `0x46524d48` = `'FRMH'` (`src/_ZN18SolidHeapAllocatorC1EPvj.c:14`) and `0x45585048` =
  `'EXPH'` (`src/_ZN22ExpandingHeapAllocatorC1EPvj.c:24`).

Correct statement: *SM64DS does not use NNS G2d/G3d; its heap and sound layers are
NNS/SDK-shaped under EAD names.* And write "BMD/BTP/BCA," not "BMD0/BTP0/BCA0" — the
game never checks a stamp, and the `0` spelling invites the NSBMD confusion.

### 2d. Audio is *far* better scoped than the first draft claimed

The first draft called audio "a separate, unscoped subproject." That was wrong. The ARM9
side is the **stock NitroSDK sound stack over SDAT**:

- `src/Snd_SendCommand.c` builds command records from a pool and dispatches them —
  NitroSDK's `SND` command-queue shape.
- `src/func_0205b070.c` is `SND_FlushCommand` (DC-flush of 0x1800 = 256 × 0x18, then IPC
  on **PXI tag 7** = sound). `src/IPCSend.c:34` is `PXI_SendWordByFifo` on `0x4000188`.
- The data is SDAT: `symbols/verified.tsv` names `0x020a5bb8` as
  `_ZN5Sound8SDAT_RAM3PTRE`, and `Sound::InfoSequenceEntry::GetWithID` walks the SDAT
  INFO block. `roadmap.md:42` already says "SDAT / SSEQ playback."

And libntr's `sim7` is **not** an emulator — it is a native C reimplementation of the SDK
ARM7 sound component, including a ~1,679-line SSEQ sequencer
(`libraries/sim7/src/snd/snd_seq.c`) over an SDL2-semaphore PXI layer.

→ Audio becomes: *validate libntr's `sim7`/`snd` against SM64DS's command-ID table.*
Not "decompile the ARM7" and not "ship silent forever." (Still true: `config/` is
arm9-only, the ARM7 passes through untouched in `rombuild`, and SM64DS's ARM7 binary is
not ours. But the *protocol* between them is SDK-standard, which is what matters.)

### 2e. The 32-bit pointer problem — and why the arena alone doesn't solve it

The largest visible change class in the pc_port diff is `u32` → `u64` cast patching:

```c
#ifdef SDK_PORT
#define VRAM_TRANSFER_DST(vram, ...) ((void *)((u64)vram + ...))
#else
#define VRAM_TRANSFER_DST(vram, ...) ((void *)((u32)vram + ...))
#endif
```

*(Though only ~34 files actually add `(u64)` casts; much of the 318-file churn is
callback-signature widening, staticness toggles, and `NitroStaticInit` shims.)*

We have the same problem plus in-place 32-bit fixup written back into loaded file images:

```c
/* src/_ZN5Model17UpdateFileOffsetsER8BMD_File.cpp */
int base = (int)&file;
if (g->lists) g->lists = (BMD_DisplayList *)((int)g->lists + base);
```

**The first draft proposed a low-2GB arena so truncating casts round-trip. That is right
in instinct and insufficient as specified.** The truncating casts in this tree mostly
target things that would *never* live in a DS-memory arena — **179 files** cast addresses
via `(u32)&` / `(int)&`, and they point at globals, function pointers, and overlay
descriptors:

```c
if (g == (int)&data_02089608)                     /* src/ContinueKuppaScriptIfNecessary.c:17 */
(int)&func_02034fbc                                /* src/func_02034da4.c:48 — function ptr */
LoadOverlay((int)&overlay_0)                       /* src/func_0201a2f8.c:41 */
```

Moving `s_HW_MAIN_MEM` low fixes none of those; host `.data`/`.text` sit at the image
base. Making the invariant total would additionally require linking non-PIE at a low
base and auditing stack-address truncation.

**Revised recommendation: target 32-bit (i686) for the first host build.** Every pointer
is 4 bytes by construction — statics, stack, heap, function pointers all round-trip, and
pointer-field struct layouts stay identical to the ARM ones, which also keeps the file
loaders honest. The low-2GB arena becomes the *later* 64-bit refinement, not the
foundation. (The OS-level piece does work: `VirtualAlloc` at literal `0x02000000` and
`0x10000000` both succeed inside an ASLR'd 64-bit process, so DS addresses can even be
mirrored 1:1. And use `-fno-strict-aliasing` regardless.)

### 2f. Licensing is not a caveat — it is the gating fact

**All five `cybervisi0n` repos are GitHub forks of `ntrtwl/*`**, verified via the API:

```sh
libntr        fork=true  parent=ntrtwl/NitroSDK     license=none
libntrsystem  fork=true  parent=ntrtwl/NitroSystem  license=none
libntrwifi    fork=true  parent=ntrtwl/NitroWiFi    license=none
libntrdwc     fork=true  parent=ntrtwl/NitroDWC     license=none
libvct        fork=true  parent=ntrtwl/libvct       license=none
```

`ntrtwl/NitroSDK` is described as "Nintendo DS Software Development Kit," carries no
license, and has stripped copyright headers. So "this reads like derived Nintendo SDK
material" is not an inference from code style — it is the **git ancestry**. Prebuilt
binaries are in-tree too (`components/ichneumon_sub.sbin`, ~162 KB of ARM code;
`libcht.a`, `libdgt.a`, `libe3s.a`).

**But the conservative conclusion I first drew — "don't even read it" — is stricter than
both DS-decomp practice and this project's own written policy.** `pret/pokeplatinum` on
**`main`** — the matching decomp, not the port — declares these as build dependencies:

```ini
# pret/pokeplatinum@main : subprojects/NitroSDK.wrap
[wrap-git]
url = https://github.com/ntrtwl/NitroSDK.git
revision = e6fbeba60332e660639ad664eda2dc85c0e14649
directory = NitroSDK-4.2.30001
```

…plus `NitroSystem`, `NitroWiFi`, `NitroDWC`, all pointing at the same `ntrtwl` org, all
pinned to SDK versions (`mwrap_sdk: '2.0/sp1p2'`, `'2.0/sp2'`). So the flagship pret DS
decomp already links the exact unlicensed trees flagged above, **for matching**, and
integrates them as `.wrap` fetches rather than vendored source.

That reframes the question. Three distinct bodies of material, three answers:

| Material | What it is | Our position |
|---|---|---|
| **1. SDK/NNS API surface** — names, signatures, register offsets, struct layouts | Facts / interface | **Already permitted.** `CREDITS.md`: *"import knowledge, write code."* Same category as the leaked licensee docs `CREDITS.md` already credits. |
| **2. SDK/NNS implementations** (`ntrtwl/*`) | Nintendo's code, unlicensed | **Never vendor into `src/`.** A build-time `.wrap` is defensible — it is what pret does, and it mirrors our own "supply your own ROM locally" model. |
| **3. cybervisi0n's port layer** — `libraries/sim/*`, the GL translator, the `SDK_PORT` branches, X86 ioreg generation | **Their** original work, unlicensed | **The real restriction lives here.** Plain copyright, reachable rights holder. Don't copy; ask them to license it. |

Category 3 is both the part we would most want and the only part with a clean, solvable
answer: **ask cybervisi0n to put a license on their own port layer.** They cannot license
Nintendo's SDK, but they can license the translator they wrote.

Two hard rules regardless:

- **No prebuilt binaries, ever.** `components/ichneumon_sub.sbin` (~162 KB of ARM code),
  `libcht.a`, `libdgt.a`, `libe3s.a` are compiled Nintendo code. Different category from
  headers, and there is no version of this where they belong in our tree.
- **Mind the emulator trap.** "Just look at how an emulator does it" is a *worse*
  contamination risk than the SDK headers for an MIT repo: melonDS is GPLv3, DeSmuME
  GPLv2. **GBATEK is the safe spec source** for register semantics — it is documentation,
  not code.

---

### 2g. This is not port-only work — most of it pays back into matching

Worth stating plainly, because the framing above makes it look like PC-port scaffolding.
It isn't. `pret/pokeplatinum` depends on the SDK trees on its **matching** branch, and
the same four levers apply to us today, years before any `.exe`:

1. **SDK functions become compile-and-compare instead of guess-and-check.** SM64DS
   statically links NitroSDK; those functions are inside our 11,347 and are currently
   named `func_XXXXXXXX`. Where real SDK source exists, matching an SDK function is
   potentially "compile the actual source with the right mwccarm" rather than
   reconstructing it from disassembly. **Caveat: version skew is real** — `ntrtwl`'s tree
   is `NitroSDK-4.2.30001` / `NitroSystem-071126.1` (late 2007); SM64DS shipped Nov 2004
   on roughly NitroSDK 1.x–2.x. Expect names and structure to transfer and byte-identity
   often not. Worth checking whether the `ntrtwl` repos carry older tags.
2. **Free naming and type recovery.** We rediscovered NNS FND by hand — `'EXPH'`/`'FRMH'`
   under our own `ExpandingHeapAllocator` / `SolidHeapAllocator` names. The real headers
   would have handed that over immediately, and the same holds across `OS_`, `MI_`,
   `FS_`, `CARD_`, `SPI_`. This is exactly the "import knowledge" lane `CREDITS.md`
   already sanctions.
3. **The ioreg consolidation is decomp hygiene first.** 317 files each carrying a local
   `#define reg_G3_TEXIMAGE_PARAM (*(volatile u32 *)0x040004a8)` are precisely the
   magic-address / un-recovered-constant pattern our own match-review bar flags.
   Replacing them with properly-named shared headers raises match admissibility whether
   or not a port happens. **Phase 2a–2b is ~60% matching work wearing a port's clothes.**
4. **Struct layouts pinned by the SDK ABI.** Game structs handed to SDK calls have field
   offsets fixed by the SDK's own headers — free evidence for the naming effort.

So the sequencing question is not "port or matching." Phase 2 is worth doing on matching
grounds alone; the port is what that work unlocks later.

## 3. Verdict

**The design transfers; the code should not be vendored, and probably should not be read
closely either.** The reusable ideas, all statable without libntr's source:

1. **Per-arch ioreg headers** with an X86 variant turning registers into globals — the
   right shape for our 317 MMIO files, and sufficient on its own for the 2D engine.
2. **`SDK_PORT` as the single porting switch**, ARM path byte-identical. Our
   `rombuild.py` makes that provable; pokeplatinum could not prove it.
3. **A `src/port/` quarantine** so host code never contaminates the matched tree.
4. **Two seams, not one:** register-scan for 2D, display-list decode for 3D.
5. **Meson + wraps** for a multi-target build. We currently have no general build system.

What does *not* transfer: NNS G2d/G3d (unused), and pokeplatinum's cost profile (it is an
NNS-based, mostly-2D game; we are neither).

---

## 4. Plan

### Phase 0 — Decisions (blocking, no code)
- **0a.** Maintainer decision on libntr given §2f: reference-at-arms-length /
  clean-room-from-GBATEK / vendor anyway. Everything below assumes the middle option.
- **0b.** **Host target: i686 first** (§2e). Prototype `rombuild`-adjacent compile of a
  handful of `src/` files under 32-bit clang. Small, and it decides the shape of
  thousands of lines.
- **0c.** **Decide the tree strategy** — this is the real fork in the road:
  - *(i) single tree with `#ifdef SDK_PORT`* — what pokeplatinum did; collides with our
    header policy (below).
  - *(ii) host-build-only source transform* — a libclang/regex pass that rewrites
    `*(volatile u32 *)0x04xxxxxx` into API calls **during the host build only**, leaving
    `src/` and the matching gate completely untouched. **This is probably the answer.**
    It dissolves the Phase 2 tension instead of paying it.
  - *(iii) hard fork, matching frozen* — cheapest per-step, but rots against an active
    decomp with 217 functions still landing.
- **0d.** Refresh `roadmap.md` (says "Phase 1, 65.9%"; README says 98.1% / 92.3%).

**Why 0c matters:** `AGENTS.md:82-84` states that *"a header edit touching more than ~200
sources is refused for human review rather than auto-validated."* Phase 2 as originally
written — one shared header consumed by 317 migrated files — is by the project's own
policy escalated out of the mechanical fan-out path. Option (ii) sidesteps this entirely.

### Phase 1 — Enrollment (useful regardless)
`src/` has 11,061 files; **9,116** are enrolled `complete` in `config/**/delinks.txt`.
Note the correction: enrollment gates **ARM ROM assembly**, not host coverage — every
`src/` file is already individually byte-verified, so a host build can compile all of
them. Enrollment is still worth closing; it is just not the port's ceiling.

### Phase 2 — The hardware layer *(shape depends entirely on 0c)*
- **2a.** Inventory: classify every MMIO site by block (2D / 3D-FIFO /
  3D-immediate / CP / IRQ) and by form (bare store, volatile deref, DMA target,
  spin-wait). This sizes everything downstream and is pure analysis.
- **2b.** Build the host-side register file + 2D scan-out, GBATEK-sourced. The 2D engine
  is the largest surface and the *cheapest* seam — do it first for the earliest visible
  result.
- **2c.** Build the display-list decoder for the 3D command stream, including the
  DMA-to-`0x04000400` path.
- **2d.** Port the SWI/BIOS wrappers already named in `config/arm9/symbols.txt`
  (`Div`, `CpuSet`, `CpuFastSet`, `Sqrt`, `LZ77UnComp*`, `HuffUnComp*`, `BitUnPack`,
  `VBlankIntrWait`) — small, self-contained, portable near-verbatim.
- **2e.** Neutralise every spin-wait (§2a) — otherwise the boot path deadlocks.

### Phase 3 — Host build
Meson alongside `rombuild.py` (not replacing it): `arm9` target unchanged, `host` target
i686. Get the tree to **link**, not run. Expect mwccarm-idiom fallout here beyond pointer
width (see risks).

### Phase 4 — Minimum viable frame
Boot path and main loop → **threading/PXI stand-in** → filesystem from an extracted asset
dir → 2D scan-out (first visible pixels) → 3D display-list → input → audio via `sim7`-shaped
SSEQ.

### Phase 5 — Beyond
Save data, dual-screen/touch presentation, then `roadmap.md` Phase 4 polish.

---

## 5. Risks and gaps the first draft missed

1. **Threads and the PXI/IRQ model.** `OS_SleepThread` in 23 files, `OS_WakeupThread` in
   14; sound, FS and DMA paths block on hardware or ARM7 progress. "VBlank-driven tick on
   a host timer" under-scopes this — without a scheduler and PXI stand-in the port
   deadlocks during boot.
2. **Overlays.** 103 of them, loaded at runtime by address-truncated descriptor. A
   statically-linked host build needs an overlay lifecycle shim (ctors, per-load bss
   re-init).
3. **The 217 unmatched functions are holes in the port.** On ARM they are filled with
   baserom byte slices via delinks; a host build has no equivalent. Needs a
   stub-and-prioritise story — this, not enrollment, is the real coverage gap.
4. **mwccarm dialect and C++ ABI.** The tree is written to coax exact bytes from mwccarm
   1.2sp2p3 (`launder.h`, `(long long)(int)` laundering, `//cpp` dummy-vtable dispatch per
   `notes/mwccarm-codegen.md`). 3,079 `.cpp` files re-ABI'd from mwcc to Itanium is fine
   until something does raw vtable or member-pointer arithmetic. Needs a sample-compile
   spike early.
5. **Dual-screen output, touch and mic.** SM64DS is touch-driven with real second-screen
   content. Mapping two 256×192 screens plus a touch surface onto a host window is a
   product decision, not a detail. The DS pad is the easy third of input.
6. **Save data.** EEPROM over ARM7/SPI. Unaddressed.
7. **Frame pacing.** The game assumes 60 Hz VBlank pacing and reads VCOUNT. Needs an
   explicit policy.
8. **Performance.** libntr's `malloc`/`free` per draw command (g3imm.h:118) is ugly at
   DS command rates but not a wall — worth noting, not worth designing around, and moot
   if we write our own decoder.

---

## 6. What I would do next, in order

1. **Settle 0c (tree strategy).** The host-build-only source transform is the strongest
   candidate and it changes every cost estimate below it.
2. **Spike i686 compilation** of a few dozen `src/` files (0b) — cheap, and it surfaces
   the mwccarm-idiom and C++ ABI risks immediately.
3. **Run the 2a inventory.** Pure analysis, no risk, converts "unknown
   difficulty" into a real estimate.
4. **Put §2f in front of the maintainer.** Fork-of-`ntrtwl` changes this from a
   preference to a policy question.
5. **Refresh `roadmap.md`** (0d) — trivial, and the stale 65.9% will mis-price everything.

Items 2, 3 and 5 are independent and safe to run in parallel; 1 and 4 gate the rest.

---

## Appendix: corrections from adversarial review

Verified corrections applied above, with what changed:

| First draft said | Actually | Impact |
|---|---|---|
| MMIO files "overwhelmingly wrap registers in named `G3_*` inlines" | **3 files** do; the norm is bare stores | Phase 2's premise was wrong |
| libntr interception is API-level, registers are "a decoy / dead writes" | True for 3D; **false for 2D**, which is register-scanned each frame | 2D — the biggest surface — is much cheaper |
| No display-list path | `DecodeG3Op` / `DRAW_CMD_G3_CMD_LIST` exists and fits our FIFO usage | Correct 3D seam identified |
| "SM64DS uses no NNS at all" | G2d/G3d yes; **heaps are NNS FND** (`'EXPH'`/`'FRMH'`) | Hedge required |
| Audio "separate, unscoped subproject" | Stock SDK `SND` over **SDAT**; `sim7` is a native SSEQ reimpl | Whole section rescoped |
| Low-2GB arena kills the `u32`→`u64` class | Most truncations target globals/function pointers **outside** any arena | **i686 first** instead |
| libntr "no license file" | **Fork of `ntrtwl/NitroSDK`**, headers stripped | "Zero legal risk" retracted |
| 208 pointer-address casts / 300 changed files | **179** / **318** (GitHub caps compare at 300) | Numbers corrected |
| Dead registers → renders nothing | → **spin-wait deadlock** | Failure mode is worse |

**A correction I initially rejected and should not have.** The review put the MMIO file
count at 350; I claimed it did not reproduce and used ~670. The review was right. My
pattern `0x0?4[0-9a-f]{6}` lacked a word boundary, so it counted `0x40000000` (a bit-30
flag, 482 occurrences), `0x84400000` (a DMA control word), and even `0x46524d48` —
which is `'FRMH'`, the NNS heap tag from section 2c — as hardware addresses. With ``
the figure is 361 files; restricted to real MMIO ranges, **317 files / 1,959 references**,
now measured properly by `port/tools/mmio_inventory.py` (unlanded; see above).
