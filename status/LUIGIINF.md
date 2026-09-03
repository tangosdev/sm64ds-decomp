# LUIGIINF -- game side of the Luigi Infection VS mode (SM64DS_VS_LUIGI_INFECTION)

Built on the char-apply seam (status/VSCHARS.md: port_player_set_character ->
SetRealCharacter swaps a slot's character mid-match). STAGED, do NOT push. src/
byte-frozen (host-layer only).

Worktree: C:\tmp\sm64ds-luigiinf   branch game/vs-luigi-infection off game/vs-chars
Build: C:\tmp\sm64ds-luigiinf\build\port   Build TEMP: C:\tmp\luigiinf-tmp
Proof: port/tools/luigi_proof.py   Evidence: C:\tmp\luigiinf-out

## THE MODE
One player starts as Luigi (the tagger), HIDDEN from every survivor's minimap
while survivors show. Luigi tags survivors with the moves that normally drop a
star (punch / slide-kick / ground-pound); a tagged survivor CONVERTS to Luigi
and joins his team. Timed match: the LUIGIS win if everyone is infected before
the clock, the SURVIVORS win if any are alive when it runs out. Survivor-vs-
survivor combat is exactly ROM-normal; Luigi is immune to being hit/knocked and
cannot be eaten, and can only tag.

## ENV WIRING (what the lobby picker feeds -- same shape as SM64DS_VS_CHARS)
- SM64DS_VS_LUIGI_INFECTION  unset / "" / "0" -> OFF (byte-identical normal VS).
                             any other value -> ON.
- SM64DS_VS_LUIGI_SEED       0..15, which slot starts as the tagger (default 0).
                             LOCKSTEP: the HOST picks the one random tagger and
                             passes the SAME value to every peer -- there is no
                             local rand(), which would desync a real session.
                             The lobby's job is exactly this one number.
- SM64DS_VS_LUIGI_TIME       optional test override, seconds (0/unset = the
                             ROM's own match clock, the faithful default).
Diagnostics (never set in production): SM64DS_VS_LUIGI_PROBE (minimap band
snapshot), SM64DS_VS_LUIGI_HITTEST / _TAGALL (drive staged hits for the proof).

## WHERE IT HOOKS
- hal/luigi_infection.cpp -- the whole mode. g_li_team is the single source of
  truth (seed sets one bit, a tag sets another); minimap/win/hit all read it.
  Functions: port_luigi_enabled/seed/minimap_hide/survivors_alive/seeded/
  tag_player/is_infected_player/match_reset (+ the hittest harness).
- SEED: port_luigi_seed(frame), walk_window.cpp render loop beside
  port_vs_apply_chars, one shot at frame 90 (the char-apply timing).
- MINIMAP HIDE: port_luigi_minimap_hide() at the tail of
  unmatched/Minimap_Behavior.cpp (after every blip is reseated, before Render);
  blanks each Luigi-team slot's mapID to -1 so the frozen Render `== current
  map` test skips it. Survivors keep a live mapID and still draw.
- DUAL WIN: hal/star_flow.cpp port_vs_match_end_poll, a fourth trigger feeding
  the existing latched end path/banner/marker. all-infected -> win=
  luigi-all-infected; clock-up-with-survivors -> win=survivor-timeout. Banner
  says THE LUIGIS WIN / THE SURVIVORS WIN.
- TAG + IMMUNITY: unmatched/LuigiInfection_Hit.cpp host-copies the VS player-on-
  player hit resolver func_ov002_020d869c (src excluded from slice_gate10.txt).
  It is a VERBATIM transcription of the src body plus ONE guard reached only when
  armed: c is the victim, r6 the attacker; a Luigi victim returns early
  (IMMUNITY, and Luigi-vs-Luigi is a no-op), a survivor struck by a Luigi is
  converted (TAG, replacing the star-drop), neither-infected falls through to the
  exact ROM resolution. Inert when off -> survivor-vs-survivor is ROM-normal.
- YOSHI-EAT: nothing to gate. Player::OnYoshiTryEat() returns 1 (uneatable) for
  every player, so a Yoshi can never eat any player -- the eat path is already
  closed ROM-side; the hit resolver is the only player-on-player vector.

## VERIFY (2026-09-03, all green)
- Build EXIT=0, every guard green (dsstate/alternatename/gxband/tailjump/
  closestplayer/inferred_stub). battery.py ALL GREEN (every smoke, every level,
  scene selftests 383-390, default boot, shipcfg build+selftest; linkage 9402,
  ptr_audit 0) -- with the mode OFF, so the collision smokes exercise the exact
  ROM path of the host-copied resolver and pass. src/ diff vs game/vs-chars EMPTY.
- FUNCTIONAL (port/tools/luigi_proof.py, single-process 4P VS arena, quiet-spawn):
    seed   ALL PASS -- slot 2 -> Luigi(char 1) at f90, slots 0/1/3 stay Yoshi(3);
           minimap s2[LUIGI mapID=-1] while s0/s1/s3[surv mapID=0==curmap]; the
           tagger stays live after the swap.
    inert  ALL PASS -- no SM64DS_VS_LUIGI_INFECTION: zero [luigi] output, every
           slot Yoshi(3).
    hit    ALL PASS -- through the real host resolver on real actors:
           C survivor-vs-survivor rc=1, no convert, and BYTE-IDENTICAL mode ON vs
             OFF (the critical regression check);
           A Luigi->survivor tags (char 3->1, team set); B survivor->Luigi immune
             (rc=0, no change).
    win    ALL PASS -- tag every survivor -> survivors_alive 0 -> win=
           luigi-all-infected; and SM64DS_VS_LUIGI_TIME -> win=survivor-timeout.
- Screenshot: C:\tmp\luigiinf-out\luigi_arena_on.png (mode-on arena; look not
  judged here -- for the owner's eyes).

## NEXT: the picker plugs in here
The lobby assigns the mode + one random tagger slot -> plan -> the three env
vars above -> this game side applies them post-boot. Same seam shape as
SM64DS_VS_CHARS. Survivors must not be assigned Luigi as a character (the mode
owns character assignment); team logic keys off g_li_team, not mCharacter, so a
mis-assignment would not corrupt the logic, only the look.
