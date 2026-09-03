# VS-CHARS -- game side of VS character selection (SM64DS_VS_CHARS)

Foundation for per-char colours, Luigi Infection and the lobby character picker.
Builds on the vs-nonyoshi spike (status/VSNONYOSHI.md: non-Yoshi chars load,
render and play in a VS arena via port_player_set_character -> SetRealCharacter).
STAGED, do NOT push. src/ byte-frozen (host-layer only).

Worktree: C:\tmp\sm64ds-vschars   branch game/vs-chars  off cons
  4b54888e2 (port-mount-noseat-cluster, the live cons tip).
Build: C:\tmp\sm64ds-vschars\build\port   Build TEMP: C:\tmp\vschars-tmp
Evidence: C:\tmp\vschars-out   Proof: C:\tmp\vschars-out\vschar_proof.py

## WHAT IT DOES
SM64DS_VS_CHARS: a per-slot character list, comma-separated, SLOT order, the
SAME grammar as SM64DS_VS_NAMES / SM64DS_VS_COLORS -- EXACTLY 3 commas (four
fields) OR 15 (sixteen). Each field:
    empty or '0' -> that slot keeps its Yoshi default (no swap)
    '1' Mario     '2' Luigi     '3' Wario
So "1,2,3,0" = Mario / Luigi / Wario / Yoshi in slots 0..3. The env value is NOT
the engine index (0 is Yoshi so "leave the VS default" is the natural zero);
1..3 map to the engine's 0..2 at apply. IGNORED WHOLESALE on any violation.
ABSENT = silent no-op = the all-Yoshi arena, byte for byte.

## WHERE IT HOOKS
- Reader + apply: port/hal/player_bridges.cpp, port_vs_apply_chars(int frame),
  right after port_player_set_character. Self-gates: VS mode only
  (data_0209f2d8==1), ONCE at frame 90 (the proven SM64DS_SWITCH timing, past
  the level-entry no-control), guarded by a done flag. Per slot i, if the pick
  differs from Yoshi and data_0209f394[i] is live, swaps via
  port_player_set_character(data_0209f394[i], chr) -- the door path the spike
  proved. Yoshi/empty slots untouched.
- Call site: port/tests/walk_window.cpp, in the render loop beside the
  SM64DS_SWITCH probe (search "SM64DS_VS_CHARS: the game side"), one
  port_vs_apply_chars(frame) per frame (inert except the one shot).

## VERIFY (all green, 2026-09-03)
- Build EXIT=0. Guards: dsstate OK, alternatename/gxband/tailjump/closestplayer/
  inferred_stub OK. battery.py: ALL GREEN (every smoke, every level + scene
  selftest, default boot, shipcfg). src/ diff vs 4b54888e2 EMPTY.
- FUNCTIONAL (C:\tmp\vschars-out\vschar_proof.py, SUMMARY: ALL PASS), single-
  process VS arena SM64DS_VS_MAP=0 SM64DS_VS_PLAYERS=4 SM64DS_FAULTS_FATAL=1:
    RENDER  four DISTINCT walk_window_selftest.bmp shas, one per character
            (mario 96ff7359, luigi 10424338, wario 1879a551, yoshi 982bd686) --
            a different body drew, not a shared shadow.
    PLAY    each swapped body's position moves over the run (spans 15.7M-18.7M).
    ARENA   SM64DS_VS_CHARS="1,2,3,0": all four slots Yoshi(3) before f90, flip
            to 0/1/2/3 after, slot 3 (Yoshi) unchanged throughout, rc=0.
    DEFAULT no SM64DS_VS_CHARS: every slot stays Yoshi(3), no apply log, and the
            BMP sha (982bd686...) is IDENTICAL to a BASE build (my two files
            reverted, rebuilt) no-env arena -> byte-identical to current all-Yoshi.

## NEXT: the lobby picker plugs in here
lobby assigns a per-seat character -> plan -> SM64DS_VS_CHARS env (this grammar)
-> port_vs_apply_chars applies it post-boot. Same shape as names/colours; this
lane is only the game side, the picker UI is a separate lane.
