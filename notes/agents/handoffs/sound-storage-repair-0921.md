# Sound component storage repair for PR #2881

This candidate preserves the original PR head `1a953cca4b1e04cbc349745294c03325f88688d3`
and composes it with main `a7b2c0d02c8943730cc1fa5e93ced2af011844a9`,
including the landed Memory2 shared-header change. The initial storage repair
was `e3816fa24d4e8923e87977925ab5d7c33318000c`, measured against pre-repair
composition `9a711b25af7bfa0d78a2bd49ed39b80ca1e861a7` on main
`2749832c6a462cdf22dd5cc9df3ade8bf0e24152`. Only the Sound header, production
TU and this handoff differ from the current main base. No source move, manifest edit, symbol rename or credit change occurs.

## Source correction

**SOUND2881-01 is fixed.** The old `mTable[0x1a8]` excluded a live four-byte
word which `src/func_ov006_020c33dc.c` initializes at component offset +0x1a8.
The animation functions at ov006 020c2300, 020c2440, 020c24e4, 020c2594 and
020c271c test that word before requesting sound. `func_02012174` wraps sound
playback; the word is not a resource-ready result.

`dMgSoundComponentStorage` contains the opaque prefix and the descriptive
`mSuppressSound` member. Calls pass the address of that storage, including its
last word, and scene accesses use `mTable.mSuppressSound`. Its size is 0x1ac,
its scene address remains 0x4f38, and the flag remains at scene offset 0x50e0.
The class still has size 0x562c. The type and field names are reconstructed.
The header and TU no longer claim that the first scene access proves a boundary,
or that later scene fields lie within the shortened mTable.

**SOUND2881-02 is fixed.** Independent review found two nonexistent sprite
draw-function names in the header comment. The references now identify the
actual visible/x/y draw bodies, `func_ov006_02119aa8` for SpriteA and
`func_ov006_021199c0` for SpriteB. This corrects the evidence citation without
changing the sprite layout or code.

The existing sprite and scalar readability improvements remain intact.
**SOUND-RECON01 remains partial**, tracked by issue #2492 for the next Sound
class producer: original component extent, internal component objects, and the
subsequent conflicting array views remain unresolved. Raw scene countdowns,
legacy ABI helper spellings and their existing compiler constraints are not
claimed complete by this narrow storage repair.

## Producer proof under 2004/b56

- `affected_src.py include/dScMgSound_c.h`: one production consumer,
  `src/actors/dScMgSound_c.cpp`.
- Complete compiled TU object is identical to the pre-repair composition:
  36,680 bytes, SHA256 `daf34c005a640ca2e029e624704332961317ed1cf5049e1da9f64cbd6e271192`.
  This comparison includes emitted metadata and relocations.
- `prepush_linkcheck.py --files src/actors/dScMgSound_c.cpp`: all 82 functions
  VERIFIED, no differences, zero blind words, no warnings or blocking results.
- `romdata_check.py --files src/actors/dScMgSound_c.cpp`: 13 records,
  7 VERIFIED / 212 bytes, 6 PARTIAL / 68 compared bytes, zero DIFFERS.
  Partial ancestor metadata is unchanged; it is not relabeled as full coverage.
- `rombuild.py -j16 --no-rom`: 11,208/11,208 functions reproduce,
  106/106 modules exact. This is the module-byte comparison, not ROM packaging.
  Global metadata reporting retains its inherited 3 differing records; the
  Sound-specific metadata check has zero differences and its object is unchanged.
- `check_decl_agreement.py --changed origin/main`: no new disagreements.
- `port_refcheck.py`: 418 references resolve.
- `langmode_audit.py --check langmode-baseline.json`: PASS.
- `git -c core.whitespace=cr-at-eol diff --check`: PASS; preserves the header's
  existing CRLF convention.

Private logs and control objects are in the ignored producer build/sound-repair
folder. They contain local toolchain output and are not published as source.
Independent review of this exact candidate and final PR checks are separate
acceptance gates; producer evidence alone is not formal Source review.

## Current-main composition recheck

Main's shared `dScMgSingle3DBase_c.h` now declares the existing SysTracker
constructor, and `func_ov004_020b1e34.c` records its existing three additional
scalar arguments. The Sound object remains entirely identical to the original
control after composition. Fresh header-expanded strict checks still verify all
82 functions with zero blind words or differences, and the complete ROM comparison
still reproduces all 11,208 functions and all 106 modules on this base.
The final comment-reference correction also preserves that complete object.
The previous rejected review and both finding IDs remain in queue history;
independent acceptance must name this revised candidate and current tested base.
