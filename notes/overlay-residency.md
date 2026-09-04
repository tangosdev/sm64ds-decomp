# Which overlays can be resident together

**Status:** answered. The map is code, in `tools/overlay_residency.py`, and
`resolve_placeholders.py` uses it. This note is the evidence behind it.

`notes/overlay-ambiguous-references.md` said 169 (later 171) references could not be
resolved because two overlays both define the target address and nothing said which
was loaded. Something does say: the game's own loader. **127 of the 137 that were
genuinely overlay-ambiguous are now settled.**

---

## 1. The premise the whole thing rests on

`LoadOverlay` (`src/LoadOverlay.c`) keeps a 12-entry table of resident overlays and
refuses to create an overlap:

```c
for (r = data_0209d3c4, i = 0; i < 12; r++, i++)
  if (r->valid == 0xffffffff) { if (slot == 0) slot = r; }
  else if (((r->start + r->size) > start) && (end > r->start)) Crash();
```

`size` is code + bss (`buf+8` plus `buf+0xc`). So: **two overlays whose
[base, base+code+bss) ranges intersect are never simultaneously resident** -- not as
a guess about the linker, but because the game panics if you try. Ranges come from
`extracted/dsd/arm9_overlays/overlays.yaml`.

That is rule **E2**, and everything else narrows it further.

## 2. The layout

`dsd lcf` reconstructs the NitroSDK stacking tree into `build/arm9.lcf`, and it
matches the level tables in §5 exactly:

```sh
OV000 : AFTER(ARM9)                    OV001 : AFTER(ARM9)
OV002 : AFTER(OV001)   OV003, OV004, OV007 : AFTER(OV001)
OV005, OV006 : AFTER(OV003, OV004)
OV008..OV059 : AFTER(OV002, OV003, OV004, OV007)     <- one level slot
OV060 : AFTER(OV011, OV028, OV037, ... OV057)
OV061 : AFTER(OV008..OV059)   OV062..OV067 : AFTER(OV061)
OV068 : AFTER(OV062..OV067)   OV069..OV074 : AFTER(OV068)
OV076 -> OV077..OV082 -> OV083 -> OV084..OV087 -> OV088 -> OV089..OV092
      -> OV093 -> OV094..OV097 -> OV098 -> OV099 -> OV100, OV101 -> OV102
```

Each `AFTER(...)` group is a slot whose members are mutually exclusive. [ov061](../config/arm9/overlays/ov061/symbols.txt), [ov068](../config/arm9/overlays/ov068/symbols.txt), [ov076](../config/arm9/overlays/ov076/symbols.txt), [ov083](../config/arm9/overlays/ov083/symbols.txt), [ov088](../config/arm9/overlays/ov088/symbols.txt), [ov093](../config/arm9/overlays/ov093/symbols.txt), [ov099](../config/arm9/overlays/ov099/symbols.txt) and [ov101](../config/arm9/overlays/ov101/symbols.txt) are 32-byte spacers that pin the
next slot's address.

Note what this does **not** say: [ov000](../config/arm9/overlays/ov000/symbols.txt) ends at `0x020BF4E0` and [ov006](../config/arm9/overlays/ov006/symbols.txt) starts at
`0x020BFEC0`, so geometry alone permits them together. §3 is what rules it out.

## 3. [ov000](../config/arm9/overlays/ov000/symbols.txt) is a boot overlay (**E1**)

`src/func_0201a2f8.c` -- loaded, entered at its own base address, unloaded, all
before anything else exists:

```c
LoadOverlay((int)&overlay_0);     /* line 41 */
func_020aa420();                  /* == ov000's base_address */
UnloadOverlay((int)&overlay_0);   /* line 43 */
...
LoadOverlay((int)&overlay_1);     /* line 49 */
```

This is exhaustive, not a sample: the ROM reaches `LoadOverlay` (`0x02018028`) and
`UnloadOverlay` (`0x02017f34`) through exactly **15 calls and 2 function-pointer
loads** (`config/**/relocs.txt`), every one of those functions is decompiled in
`src/`, and only this one names overlay 0. There is no `UnloadOverlay(1)` anywhere,
so [ov001](../config/arm9/overlays/ov001/symbols.txt) is resident from boot onward.

**So [ov000](../config/arm9/overlays/ov000/symbols.txt) is never resident alongside another overlay, and the only arm9 code that
can see it is `func_0201a2f8` itself.** Every `overlays(0,N)` shortlist from an
overlay means N. That alone answers the note's biggest case -- 22 references to
`func_020beb68`, which is [ov004](../config/arm9/overlays/ov004/symbols.txt)'s bss, referenced 68 more times from inside [ov004](../config/arm9/overlays/ov004/symbols.txt).

The window matters. `func_0201a2f8`'s own call to `func_020aa420` is the one place
where [ov000](../config/arm9/overlays/ov000/symbols.txt) *is* the answer; a module-granular version of this rule got that
backwards, which is why `possible()` takes the referring function.

## 4. The scene slot (**E3**, **E4**)

`GetSceneOverlayID` (`src/GetSceneOverlayID.c`) is the scene -> overlay map:

| scene | overlay |
|---|---|
| 1 | [ov007](../config/arm9/overlays/ov007/symbols.txt) |
| 2, 4, 8 | [ov003](../config/arm9/overlays/ov003/symbols.txt) |
| 3, 6, 7 | [ov002](../config/arm9/overlays/ov002/symbols.txt) |
| 5 | [ov005](../config/arm9/overlays/ov005/symbols.txt) |
| 0x169..0x186 (minigames, `IsMinigameActorID`) | [ov006](../config/arm9/overlays/ov006/symbols.txt) |

`func_0201a694` unloads the previous one before loading the next and tracks it in a
single word `data_0208ee4c`, so **exactly one of {[ov002](../config/arm9/overlays/ov002/symbols.txt), [ov003](../config/arm9/overlays/ov003/symbols.txt), [ov005](../config/arm9/overlays/ov005/symbols.txt), [ov006](../config/arm9/overlays/ov006/symbols.txt), [ov007](../config/arm9/overlays/ov007/symbols.txt)}
is resident**. Scene 6 additionally loads [ov075](../config/arm9/overlays/ov075/symbols.txt) once (file select).

And `src/func_0201a798.c:8`:

```c
if (id == (int)&overlay_6) LoadOverlay((int)&overlay_4);
```

**[ov006](../config/arm9/overlays/ov006/symbols.txt) always drags [ov004](../config/arm9/overlays/ov004/symbols.txt) in with it** (`func_0201a754` mirrors the unload). Since
[ov004](../config/arm9/overlays/ov004/symbols.txt) overlaps [ov000](../config/arm9/overlays/ov000/symbols.txt), that is a second, independent proof for the [ov006](../config/arm9/overlays/ov006/symbols.txt) -> [ov004](../config/arm9/overlays/ov004/symbols.txt)
family.

## 5. The level tables (**E6**)

`_Z17LoadLevelOverlaysi` and `LoadOrUnloadObjectOverlays` read three tables in
arm9 `.data`. Decoded from `extracted/dsd/arm9/arm9.bin`:

- `data_020758c8[52]` -- level -> overlay, and it is exactly `level + 8`, i.e. levels
  0..51 map to ov008..ov059. One level overlay at a time.
- `data_02075804[7][7]` -- object-overlay groups. **Each group is exactly one slot of
  the layout in §2**, which is the strongest single confirmation that the model is
  right:

  | group | ids | slot base |
  |---|---|---|
  | 0 | 62..67 | 0x02115EE0 |
  | 1 | 69..74 | 0x0211F000 |
  | 2 | 77..82 | 0x02123740 |
  | 3 | 84..87 | 0x02129020 |
  | 4 | 89..92 | 0x02130F00 |
  | 5 | 94..97 | 0x02135700 |
  | 6 | 100, 101 | 0x02140D80 |

- `data_02075998[52][7]` -- per level, a 1-based index into each group (0 = none).
  At most one member of a group per level.


Plus a hard-coded tail: levels 0x24/0x26/0x28 load [ov060](../config/arm9/overlays/ov060/symbols.txt) instead; every other level
loads [ov098](../config/arm9/overlays/ov098/symbols.txt) (conditionally) and [ov102](../config/arm9/overlays/ov102/symbols.txt).

So the resident set for level L is
`{ov001, ov002, ov(L+8)} u {group[i][idx[L][i]]} u ({ov060} | {ov098, ov102})`,
and a module in the level system can only reference overlays reachable from some
level that loads it.

Worked example: `src/_ZN15TtcRotatingGear13InitResourcesEv.cpp` is in [ov065](../config/arm9/overlays/ov065/symbols.txt), which
is `group[0][4]`, loaded by levels 13, 27 and 33 -- level overlays [ov021](../config/arm9/overlays/ov021/symbols.txt), [ov035](../config/arm9/overlays/ov035/symbols.txt),
[ov041](../config/arm9/overlays/ov041/symbols.txt). Its ambiguous target `0x021121b8` listed eleven candidate level overlays;
exactly one, **[ov035](../config/arm9/overlays/ov035/symbols.txt)**, is in that set.

## 6. Calls prove residency (**E5**)

Where dsd resolved a relocation to a single overlay *and* classified it as a call,
that BL transferred control, so the target was in memory. Loads do not count: a
pointer-shaped word can name an address nothing dereferences.

This is what settles the [ov002](../config/arm9/overlays/ov002/symbols.txt) vs [ov007](../config/arm9/overlays/ov007/symbols.txt) family (`_ZN5EnemyC2Ev` at `0x020aed98`,
`_ZN8Platform4KillEv` at `0x020ee55c`, 159 sites between them). [ov002](../config/arm9/overlays/ov002/symbols.txt) and [ov007](../config/arm9/overlays/ov007/symbols.txt)
share a base so they are exclusive, and the ~30 referring object overlays each have
an unambiguous *call* into [ov002](../config/arm9/overlays/ov002/symbols.txt).

A referrer whose own proven set is self-contradictory is loaded in more than one
configuration and is excluded from this rule -- [arm9](../config/arm9/symbols.txt) is the case, being always
resident and calling both [ov002](../config/arm9/overlays/ov002/symbols.txt) and [ov004](../config/arm9/overlays/ov004/symbols.txt).

## 7. Falsification

The model is one-sided: it rules candidates out, never in, and a caller only has an
answer when exactly one survives. It was checked against every verdict dsd reached
without help:

```sh
$ python tools/overlay_residency.py --check
cross-module unambiguous verdicts: {'call': 2322, 'load': 926}
forbidden by this model: 10
calls contradicted: 0
```

**Zero of 2322 cross-module calls contradict it.** The 10 exceptions are all
`kind:load` -- pointer-shaped words in [ov007](../config/arm9/overlays/ov007/symbols.txt) and [ov009](../config/arm9/overlays/ov009/symbols.txt) naming addresses in overlays they cannot coexist with, i.e. constants dsd read as pointers, no control flow.

## 8. What it settled

Of the 171 `resolve_placeholders.py` could not resolve, **137 were actually overlay
ambiguity** (the other 34 are 26 immediates that are not relocations at all, 6
`module:none`, and 2 config gaps). After the rules:

| outcome | before | after |
|---|---|---|
| unresolved -- overlay-ambiguous | 137 | **10** |
| resolvable (rename ready) | 56 | 72 |
| target unnamed in config | 22 | 133 |

**127 settled.** 16 of them are ready to rename now; the other 111 landed on
`target unnamed in config`, meaning the module is known and `symbols.txt` has only
an address-shaped name there. That is a naming job with a known answer, not an
ambiguity -- and it is now the biggest single block of work left.

## 9. The 10 that remain, and what would settle them

| file | referrer | why residency cannot decide |
|---|---|---|
| `src/func_ov002_020ec670.c` | [ov002](../config/arm9/overlays/ov002/symbols.txt) | `0x02123804` -- all four of [ov077](../config/arm9/overlays/ov077/symbols.txt)/[ov078](../config/arm9/overlays/ov078/symbols.txt)/[ov079](../config/arm9/overlays/ov079/symbols.txt)/[ov080](../config/arm9/overlays/ov080/symbols.txt) hold a real, differently-sized function there. Engine code reaching into a slot; the callee genuinely depends on the level. |
| `src/_ZN14CutsceneObject13InitResourcesEv.cpp` | [ov002](../config/arm9/overlays/ov002/symbols.txt) | `0x02113c20` in the level slot; [ov002](../config/arm9/overlays/ov002/symbols.txt) is resident for every level. |
| `src/_ZN16BowserShockwaves13InitResourcesEv.cpp` | [ov060](../config/arm9/overlays/ov060/symbols.txt) | narrowed to the three Bowser levels ([ov044](../config/arm9/overlays/ov044/symbols.txt)/[ov046](../config/arm9/overlays/ov046/symbols.txt)/[ov048](../config/arm9/overlays/ov048/symbols.txt)); all 19 candidate symbols are dsd placeholders. |
| `src/func_ov089_0213162c.c` | [ov089](../config/arm9/overlays/ov089/symbols.txt) | [ov089](../config/arm9/overlays/ov089/symbols.txt) is loaded by many levels. |
| `src/_ZN6Bullet13InitResourcesEv.cpp` | [ov002](../config/arm9/overlays/ov002/symbols.txt) | [ov065](../config/arm9/overlays/ov065/symbols.txt) vs [ov075](../config/arm9/overlays/ov075/symbols.txt) (see below). |
| `src/_ZN8CapEnemy6AddCapEj.c` | [arm9](../config/arm9/symbols.txt) | [ov002](../config/arm9/overlays/ov002/symbols.txt) vs [ov007](../config/arm9/overlays/ov007/symbols.txt); arm9 spans both. |
| `src/func_02008b4c.c` | [arm9](../config/arm9/symbols.txt) | [ov002](../config/arm9/overlays/ov002/symbols.txt) vs [ov006](../config/arm9/overlays/ov006/symbols.txt); both hold a real function. |
| `src/func_02029408.c` | [arm9](../config/arm9/symbols.txt) | [ov002](../config/arm9/overlays/ov002/symbols.txt) `_ZN6Player8CanPauseEv` vs [ov004](../config/arm9/overlays/ov004/symbols.txt) (see below). |
| `src/func_0201a458.c` | [arm9](../config/arm9/symbols.txt) | [ov062](../config/arm9/overlays/ov062/symbols.txt) vs [ov065](../config/arm9/overlays/ov065/symbols.txt), and [ov006](../config/arm9/overlays/ov006/symbols.txt) vs [ov100](../config/arm9/overlays/ov100/symbols.txt) (see below). |
| `src/func_0201a2f8.c` | [arm9](../config/arm9/symbols.txt) | the one place [ov000](../config/arm9/overlays/ov000/symbols.txt) *is* the answer. The tool refuses instead of answering ov001, which is the point of passing the function; the source above settles it by inspection. |

Four of these have a second, weaker line of evidence available. **dsd flags a symbol
`ambiguous` in `symbols.txt` when it invented it only to have a name for a possible
target of an ambiguous relocation.** Measured across the config: 3137 flagged
symbols, **all** are targets of some ambiguous relocation, **none** has unambiguous
evidence, and none is a function -- only `data(any)` or `bss`. So a flagged
candidate is a placeholder and an unflagged one is a symbol dsd derived
independently. Where exactly one candidate is unflagged:

- `_ZN6Bullet13InitResourcesEv` `0x0211d610` -> **[ov065](../config/arm9/overlays/ov065/symbols.txt)** ([ov075](../config/arm9/overlays/ov075/symbols.txt)'s is flagged)
- `func_0201a458` `0x0211d9c0` -> **[ov065](../config/arm9/overlays/ov065/symbols.txt)**; `0x02140d80` -> **[ov100](../config/arm9/overlays/ov100/symbols.txt)** (which is what
  `func_02034fbc` loads immediately before calling it)
- `func_02029408` `0x020bd828` -> **[ov002](../config/arm9/overlays/ov002/symbols.txt)** `_ZN6Player8CanPauseEv`**

That is weaker than residency and is deliberately **not** wired into the tool. It
would need someone to decide the flag is trustworthy as evidence rather than as a
hint.

The remaining six need an emulator trace (`notes/emu-trace-plan.md`) or a reading of
the call site, and three of them may have no single answer at all -- an engine
module reaching into a slot means a different callee per level, which is a source
structure problem, not a lookup.

## 10. A misnaming found on the way

`overlay_64` and `overlay_66` in `src/func_02034fbc.c`, [src/func_ov007_020cc2cc.c](../config/arm9/overlays/ov007/symbols.txt) and [src/func_ov075_02117bc4.c](../config/arm9/overlays/ov075/symbols.txt) are **overlay ids 100 and 102**: the literals in the ROM are `0x64` and `0x66` and whoever named them wrote the hex digits as decimal.
`overlay_75` next to them is genuine decimal 75 (`0x4b`), so the convention is not
even consistent. Taken literally, [ov064](../config/arm9/overlays/ov064/symbols.txt) and [ov066](../config/arm9/overlays/ov066/symbols.txt) overlap and `LoadOverlay` would `Crash()`; [ov100](../config/arm9/overlays/ov100/symbols.txt) and [ov102](../config/arm9/overlays/ov102/symbols.txt) do not. Renaming them is a separate, easy change.

## Related

- `tools/overlay_residency.py` -- the model, with `--check`
- `notes/overlay-ambiguous-references.md` -- the problem this closes
- `notes/runbook-reference-repair.md` -- the loop that consumes it
