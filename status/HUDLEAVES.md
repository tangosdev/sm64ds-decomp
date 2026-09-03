# Lane HUD-LEAVES -- HUD::Render and its seven leaf routines

Worktree `C:\tmp\hud-leaves`, branch `port/hud-leaves` off the cons tip
`6ca1975c6`. Scope: `HUD::Render` (ov002) and the seven functions it calls
directly that are themselves leaves (call nothing further): `RenderCoinCount`,
`RenderCameraButtons`, `RenderRedCoins`, `RenderSilverStars`, `RenderLifeCount`
(ov002), `Event::GetBit`, `Stage::RenderBouncingArrows` (arm9 main). Nothing
under `port/hal` or `port/unmatched` was touched; no slice manifest entry was
added. This is a pure decomp contribution: matched `src/` files already
existed for all eight bodies at this tip, and `config/**/delinks.txt` now
marks all eight `complete`.

## 1. Identification, by address

`HUD::Render` and its overlay-resident siblings were located in
`config/arm9/overlays/ov002/symbols.txt` (module base `0x020ad660`, from
`extracted/dsd/arm9_overlays/overlays.yaml`, `id: 2`); the two arm9-main
siblings (`Event::GetBit`, `Stage::RenderBouncingArrows`) were located in
`config/arm9/symbols.txt` (base `0x02004000`). `HUD::Render`'s body was
disassembled and its eleven direct callees enumerated from the extern
declarations of the already-checked-in `src/_ZN3HUD6RenderEv.cpp`; the four
that call further into other named routines (`RenderVsTimer` -> `OAM::Render`,
`RenderStarCount` -> `CalculateDigits`, `RenderHealthMeter` ->
`Player::IsInsideOfCannon`, `RenderTimeTimer` -> `Timer::GetTime`) were
excluded as non-leaf, leaving exactly seven: five HUD members plus
`Event::GetBit` and `Stage::RenderBouncingArrows`, matching the eight-body
count asked for (Render + seven leaves).

## 2. The name-shift trap, and what it actually was here

This lane's brief warned that config addresses are not always reliable and to
verify by address/RTTI rather than trust the mangled filename. That warning
proved justified, but the actual fault was upstream of `symbols.txt`: the
worktree's wired `extracted/dsd/arm9_overlays/ov002.bin` (junctioned in by
the `decomp-worktree` skill from the cons main checkout) is **389344 bytes**,
4704 bytes short of the **394048 bytes** `config/arm9/overlays/ov002/delinks.txt`
requires (`.data end 0x0210d9a0` minus `base_address 0x020ad660`). Diagnosis:

* `tools/match.py` against that file failed for `HUD::Render`,
  `HUD::InitResources` and all five ov002-resident leaf candidates -- not
  with a clean byte diff, but with the ROM side of the comparison decoding
  to code that made no sense at the claimed addresses (mid-state-machine
  bytes where a function prologue should start).
* A control group of ten other, unrelated, already-matched ov002 functions
  spanning the whole module was bisected against the same file: everything
  up to `func_ov002_020b14d8` matched; every candidate tried at or after
  `func_ov002_020b1674` (file offset `0x2014`) failed, consistent with a
  decompression stream that desyncs partway through and produces wrong bytes
  from that point to EOF (and is short by exactly the missing tail).
* `extracted/overlays/overlay_0002.bin` (the non-`dsd`-reprocessed
  extraction, sitting right next to it in the same `extracted/` tree) is the
  correct 394048-byte decompression. Every one of the eight target bodies,
  and the ten-function control group, matches byte-for-byte, strict-relocs
  clean, against that file at the config's own unmodified addresses. No
  symbols.txt or delinks.txt address needed changing.

This is worth flagging outside this lane: any other worktree wired against
the same cons checkout's `extracted/dsd/arm9_overlays/ov002.bin` will see the
same false non-matches for ov002 work from roughly file offset `0x2014`
onward. `cons` itself was not touched (per the lane's standing rule).

## 3. Per-body report

All eight bodies below are pre-existing `src/` files at this tip (none were
newly written by this lane); this lane's contribution is the verification
against the correct binary and the three `complete` markers in section 4.

| body | address | module | size | match | wall |
|---|---|---|---|---|---|
| `HUD::Render` (`_ZN3HUD6RenderEv`) | 0x020fd5e0 | ov002 | 0x1c4 | MATCH (2004/b56), strict-relocs clean | none |
| `HUD::RenderLifeCount` (`_ZN3HUD15RenderLifeCountEv`) | 0x020fbe38 | ov002 | 0x214 | MATCH (2004/b56), strict-relocs clean | none |
| `HUD::RenderCameraButtons` (`_ZN3HUD19RenderCameraButtonsEv`) | 0x020fc04c | ov002 | 0x378 | MATCH (2004/b56), strict-relocs clean | none |
| `HUD::RenderSilverStars` (`_ZN3HUD17RenderSilverStarsEv`) | 0x020fc3c4 | ov002 | 0x94 | MATCH (2004/b56), strict-relocs clean | none |
| `HUD::RenderRedCoins` (`_ZN3HUD14RenderRedCoinsEv`) | 0x020fc77c | ov002 | 0xa0 | MATCH (2004/b56), strict-relocs clean | none |
| `HUD::RenderCoinCount` (`_ZN3HUD15RenderCoinCountEv`) | 0x020fc81c | ov002 | 0x1fc | MATCH (2004/b56), strict-relocs clean | none |
| `Event::GetBit` (`_ZN5Event6GetBitEj`) | 0x02029ee0 | arm9 (main) | 0x18 | MATCH (2004/b56), strict-relocs clean | none |
| `Stage::RenderBouncingArrows` (`_ZN5Stage20RenderBouncingArrowsEv`) | 0x02023be0 | arm9 (main) | 0x1d8 | MATCH (2004/b56), strict-relocs clean | none |

Match commands (ov002 bodies use the corrected binary; arm9-main bodies need
no `--bin` override):

    python tools/match.py --c src/_ZN3HUD6RenderEv.cpp --func _ZN3HUD6RenderEv \
        --addr 0x020fd5e0 --size 0x1c4 \
        --bin extracted/overlays/overlay_0002.bin --base 0x020ad660 \
        --module ov002 --strict-relocs --cpp-check
    # ... (RenderLifeCount / RenderCameraButtons / RenderSilverStars /
    #      RenderRedCoins / RenderCoinCount, same --bin/--base/--module)

    python tools/match.py --c src/_ZN5Event6GetBitEj.c --func _ZN5Event6GetBitEj \
        --addr 0x02029ee0 --size 0x18 --strict-relocs
    python tools/match.py --c src/_ZN5Stage20RenderBouncingArrowsEv.cpp \
        --func _ZN5Stage20RenderBouncingArrowsEv --addr 0x02023be0 --size 0x1d8 \
        --strict-relocs

All eight: `MATCHING VERSIONS: 2004/b56`, zero mismatched bytes, zero
strict-reloc destination mismatches.

`tools/prepush_linkcheck.py` (which resolves its own binary and is not
affected by the ov002.bin extraction issue above) verified all eight
independently:

    python tools/prepush_linkcheck.py --files \
        src/_ZN3HUD6RenderEv.cpp src/_ZN3HUD15RenderLifeCountEv.cpp \
        src/_ZN3HUD19RenderCameraButtonsEv.cpp src/_ZN3HUD17RenderSilverStarsEv.cpp \
        src/_ZN3HUD14RenderRedCoinsEv.cpp src/_ZN3HUD15RenderCoinCountEv.cpp \
        src/_ZN5Event6GetBitEj.c src/_ZN5Stage20RenderBouncingArrowsEv.cpp
    prepush-linkcheck: 8 checked - 8 verified, 0 warning(s), 0 blocking

## 4. The port/config change

Three of the eight already-matched files were carrying matched source but
had never been promoted to `complete` in their module's `delinks.txt` (so
`dsd delink`/`rombuild` would still have supplied ROM-byte gap objects for
their address ranges rather than compiling the checked-in source). This lane
adds exactly the missing marker, by hand (not via `tools/enroll.py --all`,
whose dry run touched 13 delinks.txt files project-wide -- out of scope for
this lane):

| file | change |
|---|---|
| `config/arm9/overlays/ov002/delinks.txt` | `complete` added under `src/_ZN3HUD15RenderLifeCountEv.cpp:` and `src/_ZN3HUD15RenderCoinCountEv.cpp:` |
| `config/arm9/delinks.txt` | `complete` added under `src/_ZN5Stage20RenderBouncingArrowsEv.cpp:` |

The other five bodies (`HUD::Render`, `RenderCameraButtons`,
`RenderSilverStars`, `RenderRedCoins`, `Event::GetBit`) were already marked
`complete`. No `src/`, `include/`, `port/hal/` or `port/unmatched/` file was
touched, and no slice manifest was edited.

## 5. Build and battery

`port/build-port.cmd` (all guards) and `python port/tools/battery.py
--skip-build` were run in this worktree, TMP/TEMP under
`C:/tmp/hud-leaves/build/tmp`. See the tip-line report for the exact
outcome; no linkage change was expected (the three newly-`complete` bodies
were already reachable/verified source, not new content, and none of the
eight are seated in `port/hal` or a slice manifest in this lane).

## 6. What is NOT in this lane

* `HUD::InitResources` was spot-checked against the corrected binary as a
  sanity cross-check (it also matches cleanly) but is not one of the eight
  target bodies and was not touched.
* The ov002.bin extraction issue in section 2 is a wiring/extraction defect
  in the shared `extracted/` tree, not a decomp defect; it is reported here,
  not fixed there (cons was not touched).
