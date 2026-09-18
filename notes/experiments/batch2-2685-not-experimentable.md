# NOT-EXPERIMENTABLE leftover blocks (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- scope: all 81 `Leftover:` blocks in `git diff origin/main...HEAD --name-only -- 'src/*'`.
  17 blocks pinned by 19 artifacts (see TESTED); the remainder below are not
  compiler-testable as TU-local spelling variants, one line why each
  (64 full blocks plus untested sub-claims of the 7 partially-pinned blocks).

## TESTED (19 artifacts, 17 blocks)

- Exp1 pgmthr-setanim SUPPORTED, Exp2 pgmthr-dropshadow SUPPORTED,
  Extra1 pgmthr-init-1ac SUPPORTED (all three pin `d_a_pg_mthr.cpp:23` sub-claims).
- Extra2 pgmthr-matrix SUPPORTED (`d_a_pg_mthr.cpp:32`).
- Exp3 bakubaku-klass-ice DISPROVED (`d_a_bakubaku.cpp:21`).
- Exp6 bakubaku-matrix SUPPORTED (`d_a_bakubaku.cpp:30`).
- Extra12 bakubaku-deadstore SUPPORTED (`d_a_bakubaku.cpp:39` dead-store representative).
- Exp4 dossunbar-resdesc SUPPORTED, Exp5 dossunbar-setfile SUPPORTED
  (`d_a_obj_bk_dossunbar.cpp:21` and `:16` SetFile part).
- Extra3 waterfall-levelid, Extra4 waterfall-vector3, Extra5 waterfall-particle
  all SUPPORTED (`d_a_obj_waterfall.cpp:21`, `:23`, `:15`).
- Extra6 wcobj01-getclsn, Extra7 wcobj06-getclsn SUPPORTED
  (`d_a_obj_wc_obj01.cpp:18`, `d_a_obj_wc_obj06.cpp:18`).
- Extra8 wanwan-factory SUPPORTED (`d_a_wanwan.cpp:39`).
- Extra9 wanwan-matrix SUPPORTED (`d_a_wanwan.cpp:42`).
- Extra13 wanwan-stump DISPROVED (`d_a_wanwan.cpp:31` Stump part).
- Extra10 damky-matrix DISPROVED (`daMky_c.cpp:22` matrix part).
- Extra11 damip-matrix DISPROVED (`daMip_c.cpp:75` matrix part).

## NOT-EXPERIMENTABLE

### Naming belongs in ov002/ov098/base (16)

- `src/game/actors/d_a_obj_bk_fall_block.cpp:17` — ov098 FallBlock Init/Cleanup linker names, base leaves slots pure virtual.
- `src/game/actors/d_a_obj_bk_ukisima.cpp:15` — ov002 Kaitendai Init/Cleanup linker names.
- `src/game/actors/d_a_obj_fl_gura.cpp:15` — ov002 linker names.
- `src/game/actors/d_a_obj_ki_ita.cpp:13` — ov002 float-board setup linker name, slot pure virtual on base.
- `src/game/actors/d_a_obj_km1_kuruma.cpp:15` — ov002 Kuruma Init/Cleanup linker names.
- `src/game/actors/d_a_obj_km1_kurumajiku.cpp:18` — ov002 Kurumajiku Init/Cleanup linker names.
- `src/game/actors/d_a_obj_km2_fall_block.cpp:14` — ov098 FallBlock linker names.
- `src/game/actors/d_a_obj_km2_gura.cpp:19` — ov002 Guragura Init/Cleanup linker names.
- `src/game/actors/d_a_obj_km2_ukishima.cpp:18` — ov002 Ukiyuka Init/Cleanup linker names.
- `src/game/actors/d_a_obj_km3_kaitendai.cpp:21` — ov002 linker names.
- `src/game/actors/d_a_obj_km3_kuruma.cpp:18` — ov002 Kuruma linker names.
- `src/game/actors/d_a_obj_km3_kurumajiku.cpp:21` — ov002 setup/teardown helper linker names.
- `src/game/actors/d_a_obj_rc_kaitendai.cpp:15` — ov002 Kaitendai Init/Cleanup linker names.
- `src/game/actors/d_a_obj_wc_obj01.cpp:13` — ov002 float-board helper linker name, not this class's Init.
- `src/game/actors/d_a_obj_wc_obj06.cpp:14` — ov002 FloatBoard Init linker name.
- `src/game/actors/d_a_obj_wc_obj07.cpp:15` — ov002 Kaitendai Init/Cleanup linker names.

### Overlay .data/.rodata/file-table/halfwords not owned (17)

- `src/game/actors/d_a_obj_bk_fall_block.cpp:20` — 02114880 overlay .data + data_ov015_* handles not owned here.
- `src/game/actors/d_a_obj_bk_ukisima.cpp:18` — start-angle halfword 02114794 not owned here.
- `src/game/actors/d_a_obj_fl_gura.cpp:17` — 0211adb0 overlay .rodata + data_ov064_* not owned here.
- `src/game/actors/d_a_obj_ki_ita.cpp:18` — 02114b8c file table between RTTI/type-name not owned here.
- `src/game/actors/d_a_obj_km1_dorifu.cpp:21` — data_ov043_* handles not owned here.
- `src/game/actors/d_a_obj_km1_kuruma.cpp:18` — 02112418 overlay data + data_ov043_* not owned here.
- `src/game/actors/d_a_obj_km1_kurumajiku.cpp:21` — 02112344 overlay data + data_ov043_* not owned here.
- `src/game/actors/d_a_obj_km2_fall_block.cpp:19` — data_ov045_* + S14 profile not owned here.
- `src/game/actors/d_a_obj_km2_gura.cpp:22` — data_ov045_* not owned here.
- `src/game/actors/d_a_obj_km2_ukishima.cpp:21` — file table 02112f08 not owned here.
- `src/game/actors/d_a_obj_km3_dorifu.cpp:21` — 02112508 + g_profile + data_ov047_* overlay data not owned here.
- `src/game/actors/d_a_obj_km3_kaitendai.cpp:23` — data_ov047_* + halfwords before .data not owned here.
- `src/game/actors/d_a_obj_km3_kuruma.cpp:21` — data_ov047_* not owned here.
- `src/game/actors/d_a_obj_km3_kurumajiku.cpp:24` — data_ov047_* not owned here.
- `src/game/actors/d_a_obj_rc_dorifu.cpp:21` — data_ov036_* (CLPS before .data, SharedFilePtrs BSS) not owned here.
- `src/game/actors/d_a_obj_rc_kaitendai.cpp:18` — halfwords 02113b18/1c before typeinfo not owned here.
- `src/game/actors/d_a_obj_wc_obj07.cpp:18` — data_ov029_* + halfword before typeinfo not owned here.

### S14 profile / text-only TU (9)

- `src/game/actors/d_a_obj_bk_fall_block.cpp:23` — g_profile_BK_DOWN_B outside TU (S14).
- `src/game/actors/d_a_obj_bk_ukisima.cpp:19` — g_profile_BK_UKISIMA outside TU (S14).
- `src/game/actors/d_a_obj_km1_kuruma.cpp:21` — g_profile overlay data, text-only deadstripped duplicate.
- `src/game/actors/d_a_obj_km1_kurumajiku.cpp:24` — g_profile overlay data, text-only deadstripped duplicate.
- `src/game/actors/d_a_obj_km2_ukishima.cpp:22` — g_profile overlay data, text-only deadstripped duplicate.
- `src/game/actors/d_a_obj_rc_kaitendai.cpp:20` — g_profile_RC_KAITEN outside TU (S14).
- `src/game/actors/d_a_obj_wc_obj01.cpp:22` — g_profile_WC_OBJ01 ov029 data outside TU (S14).
- `src/game/actors/d_a_obj_wc_obj06.cpp:22` — g_profile_WC_OBJ06 ov029 data outside TU (S14).
- `src/game/actors/d_a_obj_wc_obj07.cpp:21` — g_profile_WC_UKISIMA outside TU (S14).

### Fields/names belong elsewhere, cross-TU callers (10, incl. partials)

- `src/actors/daBombking_c.cpp:37` (Player/Camera/data part) — Player+8/+0x6ce/+0xc8 on Player, Camera+0x114/154 on Camera, data_ov078_*/func_02035550 ownership.
- `src/actors/daMip_c.cpp:75` (Player/data/S14 part) — Player+8/+0x6d9/+0x6ce on Player, data_ov085_* handles, S14 (matrix part DISPROVED via Extra11).
- `src/actors/daMky_c.cpp:22` (Player/data/factory part) — Player+8/cap+0xc8 on those classes, data_ov030_* handles, factories in other files (matrix part DISPROVED via Extra10).
- `src/actors/daSanbo_c.cpp:65` (Player/data/S14/veneer part) — Player+8 on Player, data_ov096_*/state records, S14, func_02038414 veneer naming.
- `src/game/actors/d_a_obj_bk_dossunbar.cpp:19` — unk_0a4 is dActor_c X speed, not this class's field.
- `src/game/actors/d_a_pg_mthr.cpp:37` — talk/message/func_0201267c names with callees, held-actor slots on Player.
- `src/game/actors/d_a_wanwan.cpp:31` (Player part) — +0x6fb byte name belongs on Player (Stump part DISPROVED via Extra13).
- `src/game/actors/d_a_wanwan.cpp:34` — Pos() not on this branch's dActor_c, needs shared-header decl (issue #2566).
- `src/game/actors/d_a_s_cre.cpp:13` — Pos() leaf overlay, the shared dActor_c::Pos() boundary is issue #2566, not this branch.
- `src/game/actors/d_a_bakubaku.cpp:37` — water height + particle/sound helper names belong with callees.
- `src/game/actors/d_a_obj_bk_dossunbar.cpp:26` — ITCM flag/vec + callback/store names belong with those callees.

### Symbol-label ownership func_ov/extern-C + sinit-filled tables (5)

- `src/actors/daMky_c.cpp:12` — func_ov030_* are address-derived repository labels, not preserved
  original names; a migration would rename source and config together, so this is
  reconstruction scope, not a compiler barrier. 0211124c is owned by another TU.
- `src/game/actors/d_a_bakubaku.cpp:9` — func_ov032_* are address-derived repository labels,
  not preserved original names. 15 defined here, 14 take `daBakubaku_c *self`; 02111ff4's
  `(void *, void *)` shape leaves its owner open. Migration scope: this file + ov032 symbols.txt.
- `src/game/actors/d_a_obj_bk_dossunbar.cpp:12` — extern-C state bodies + unowned PMF records, tables filled by sinit not this TU.
- `src/game/actors/d_a_pg_mthr.cpp:10` — func_ov018_* free spelling is a reconstruction choice;
  the labels are address-derived, original ownership unknown. 15 defined here, no outside
  caller. Migration scope: this file + 2 decl_common.h decls + 1 SurfaceInfo.h decl + ov018 symbols.txt.
- `src/game/actors/d_a_wanwan.cpp:11` — func_ov014_* are address-derived repository labels,
  not preserved original names. 22 defined here, each taking the object first but spelled
  char*/void*/u8*, so the receiver type is assumed. Migration scope: this file + daWanwan_c.h
  (02111fb8) + decl_common.h (02111ebc) + ov014 symbols.txt.

### Needs shared-header decl / sinit / layout recovery (4, incl. partials)

- `src/game/actors/d_a_bakubaku.cpp:32` — decl_common.h extern-int handles require shared-header change to type cleanly.
- `src/game/actors/d_a_wanwan.cpp:27` — SharedFilePtr has no recovered layout, decl_common char view required.
- `src/actors/daObjLava_c.cpp:13` — wrapper plants default callback; direct New requires callback wiring, not a spelling.
- `src/game/actors/d_a_pg_mthr.cpp:34` — data_ov018_* handles + decl_common int[] + S14 text-only (packaging + shared-header).

### Generic wall 6az / S8/S15, no per-TU size predicted (5 partials + 2 full)

- `src/actors/daBombking_c.cpp:37` (wall part) — BlendModelAnim/SetAnim/Spawn wall ref, no sizes; wall pinned by Exp1/2/5.
- `src/actors/daMip_c.cpp:75` (wall part) — SetAnim wall ref, no sizes (see also Player/data/S14 above).
- `src/actors/daMky_c.cpp:19` — SetAnim wall ref, no sizes.
- `src/actors/daSanbo_c.cpp:65` (wall part) — Init/DropShadow/Spawn wall ref, no sizes.
- `src/game/actors/d_a_bakubaku.cpp:24` — SetAnim wall ref + dBgCh Undefined link claim (link needs full link, not match.py bytes).
- `src/game/actors/d_a_wanwan.cpp:25` — Init/DropShadow wall ref, no sizes.
- `src/game/actors/d_a_obj_bk_dossunbar.cpp:16` (IsClsn/NewSimple part) — wall ref, no sizes (SetFile pinned via Exp5).
- `src/game/actors/d_a_pg_mthr.cpp:23` (TextureSequence/dBgCh part) — no sizes for TextureSequence; dBgCh Undefined needs full link (SetAnim/Init/DropShadow pinned).
- `src/game/actors/d_a_bakubaku.cpp:39` (gotos/register part) — no func/size predicted; dead-store representative pinned via Extra12.
- `src/game/actors/d_a_obj_bk_dossunbar.cpp:21` (sinit/S14 part) — sinit IDs + g_profile S14 packaging (ResourceDescriptor pinned via Exp4).
- `src/game/actors/d_a_bakubaku.cpp:35` — data_ov032_* handles + sinit IDs + state-table symbols packaging/sinit.
- `src/game/actors/d_a_wanwan.cpp:36` — data_ov014_* handles + sinit IDs + S14 packaging.
