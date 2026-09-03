# VS-CHAR-COLORS -- game side of per-character bro colour (SM64DS_VS_CHAR_COLORS)

Builds on the char-apply seam (status/VSCHARS.md, SM64DS_VS_CHARS puts a chosen
character in each VS slot). This recolours the bros (Mario, Luigi, Wario) by
CATEGORY in VS. Yoshi keeps his existing 2-colour body/shoes path untouched.
STAGED, do NOT push. src/ byte-frozen (host-layer only). The launcher colour
picker is a SEPARATE lane; this is the game side it feeds.

Worktree: C:\tmp\sm64ds-vscharcol   branch game/vs-char-colors off game/vs-chars
  (a9d061a92). Build: build\port   TEMP: C:\tmp\vscharcol-tmp
Evidence: C:\tmp\vscharcol-out   Proof: C:\tmp\vscharcol-out\vscharcol_proof.py

## TANGO'S FIVE CATEGORIES (non-Yoshi)
1 hat + undershirt (TIED as one colour)   2 overalls   3 gloves
4 buttons   5 shoes

## GRAMMAR
SM64DS_VS_CHAR_COLORS mirrors SM64DS_VS_COLORS / SM64DS_VS_CHARS: comma-separated
fields in SLOT order, EXACTLY 3 commas (four fields) OR 15 (sixteen). Each field
is empty (that slot's character keeps its ROM colours) or FIVE colon-separated
categories -- exactly four colons -- in Tango's order:

    hat_undershirt : overalls : gloves : buttons : shoes

Each category is empty (leave on ROM) or six hex digits. Example: a Mario slot
recolouring only overalls green and gloves red -> ":00ff00:ff0000::". Whole field
":" (all empty) is legal and recolours nothing. IGNORED WHOLESALE on any grammar
violation (length, non-hex/colon/comma byte, wrong comma count, malformed field)
-- the sibling contracts' rule. ABSENT = one getenv, byte-inert.

Which slot owns which character comes from SM64DS_VS_CHARS. With no
SM64DS_VS_CHARS no bro is seated, so this whole path is inert.

## HOW THE INDEX-SETS ARE SHIPPED
port/tools/gen_vs_char_groups.py reads the Studio's groups.json overrides[]
{family,file,index,pal} and emits port/hal/vs_char_groups.h -- a flat table of
{char, category, file, palette, indices} Group records. Nothing is invented: an
index appears only if it is in groups.json. The hat+undershirt tie maps BOTH the
"Hat" and "Undershirt" families to category 0 (they live in different files, so
one picked colour drives a family transform in each, each shaded from its own ROM
anchor). Regenerate with:
    python port/tools/gen_vs_char_groups.py --groups <path>/groups.json

COVERAGE (measured from groups.json, gaps left on the ROM, NOT invented):
  bro    hat+undershirt      overalls  gloves  buttons  shoes
  Mario  Undershirt(1)       12        7       GAP      GAP
  Luigi  Hat(16)+Undershirt(2) 2       14      GAP      GAP
  Wario  Hat(18)             2         12      GAP      Shoes(2)

THE BUTTONS GAP + THE WARIO SET: groups.json has NO "Buttons" OVERRIDE rows for
ANY bro (the auto-clustered "torso yellow" family the editor renamed "Buttons"
was never hand-overridden, so it is not in overrides[]). So category 4 (buttons)
ships EMPTY for all three and is a silent no-op today. Mario has no "Hat" family
(only Undershirt), Wario has no "Undershirt" and a stray "jh4" family (auto
"shoes yellow") that is not one of the five and contributes nothing. Shoes has
data only for Wario. All flagged; none filled by guesswork.

## WHERE IT HOOKS (file:line, at the committed tip)
- Generator (arithmetic): port/hal/vs_char_palette_gen.h,
  vspal::char_recolor_palette -- wraps the VERIFIED family_edit/apply_transform
  in vs_palette_gen.h (the byte-verified 512-vector oracle; UNTOUCHED) to recolor
  an arbitrary index set in an arbitrary-sized palette IN PLACE, with a bounds
  check that refuses (writing nothing) on an out-of-range index.
- Baked data: port/hal/vs_char_groups.h (generated).
- Apply seam: port/hal/fs_mods.cpp, vs_char_colors_filter, wired into mod_filter
  right after character_palette_filter (so a room's colours win over a local
  PaletteMario). Reads SM64DS_VS_CHAR_COLORS + SM64DS_VS_CHARS, resolves each
  bro's winning slot, and for each served bro BMD recolours every picked category
  via vcc_recolor_group -> char_recolor_palette (palette found by name, words
  written back in place, no resize -- palette_try's shape).

## THE ONE HONEST LIMIT: keyed per CHARACTER, not per SLOT
The cartridge tells VS Yoshis apart by palette ROW, so two Yoshis differ. The
bros have ONE palette per model file and no per-seat row, so two slots both
playing Mario share one mario_model.bmd and cannot wear different colours. The
LOWEST slot that picked a character wins that character's file; a second slot
picking the same character with colours is logged as a conflict. In the common
VS arena (Mario/Luigi/Wario/Yoshi, each once) there is no conflict.

## VERIFY (all green, 2026-09-03)
- Build EXIT=0 (C:\tmp\vscharcol-out\build1.log). Guards: closestplayer,
  inferred_stub, dsstate (OK, 1066951 bytes), gxband, tailjump, alternatename OK.
- Unit test vs_char_palette_test.cpp: 12/12 green (bounds refuses + writes
  nothing, category recolor is disjoint, identity recolor byte-identical,
  determinism, group table well formed with the documented coverage).
- battery.py: ALL GREEN, linkage 9403 (83.0%), ptr_audit 0, shipcfg build +
  selftest ok. No FAIL (the two "fail" log hits are "0 load-fail").
  C:\tmp\vscharcol-out\battery.log
- src/ diff vs game/vs-chars: EMPTY.
- FUNCTIONAL (vscharcol_proof.py, SUMMARY: ALL PASS), single-process VS arena
  SM64DS_VS_MAP=0 PLAYERS=4, each bro seated as slot 0:
    INERT   each bro's ROM-colour pass (chars only, no CHAR_COLORS) BMP is
            BYTE-IDENTICAL to the base build (game/vs-chars tip, zero colour
            code): mario 96ff7359, luigi 10424338, wario 1879a551. Also a
            present-but-empty ",,," field is byte-identical to base (parse inert).
    RECOLOR each bro's coloured pass logs each available category recoloured
            (buttons silently skipped) and CHANGES pixels vs the ROM-colour pass,
            localized to the character: mario 63px bbox 47x31, luigi 67px 39x42,
            wario 132px 83x43 (wario includes shoes).
    DISTINCT Mario one category at a time changes DIFFERENT pixel sets:
            hat+undershirt 5px, overalls 11px, gloves 47px -- disjoint regions.
    PLAY    positions move over the run with colours set (spans 15.7M-18.7M).
  Screenshots per bro: C:\tmp\vscharcol-out\{mario,luigi,wario}_{rom,colored}\
  walk_window_selftest.bmp (both DS screens stacked, frame 360, quiet+muted).
  NOT JUDGED HERE -- whether it looks right is Tango's call.

## NEXT: the lobby colour picker plugs in here
lobby assigns a per-seat, per-category colour -> plan -> SM64DS_VS_CHAR_COLORS env
(this grammar) + SM64DS_VS_CHARS -> vs_char_colors_filter recolours the served
bro BMDs. Same shape as names/chars/Yoshi-colours; this lane is only the game
side, the picker UI is a separate lane (blocked on Tango's colour-WIP decision).
