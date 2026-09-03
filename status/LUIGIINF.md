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

## START COUNTDOWN (a full 5s before the first Luigi)
Before the tagger is chosen the mode runs a five-second pre-round countdown, so
the first Luigi cannot snowball at spawn. It does NOT invent a countdown: it
drives the ROM's OWN VS start countdown -- Stage::RenderVsModeCountdown
(0x0202a168), which draws the READY? banner, the big red 3/2/1 numeral and START.
The port hosted that render NOWHERE before this (only the countdown sounds,
port_vs_countdown_tick); it is wired now, gated on SM64DS_VS_LUIGI_INFECTION.

- DRIVER: hal/star_flow.cpp port_luigi_countdown_drive(frame), called beside
  port_vs_countdown_tick in the frame loop. Host-frame deterministic (walk_window
  ticks the 3D logic at ~30Hz), anchored at frame 90:
      [90,150)  count 3  READY?+red 3   ~2.0s     [195,240) count 1  READY?+red 1
      [150,195) count 2  READY?+red 2   ~1.5s      240      count 0  START + PICK
  data_0209f2bc (the ROM counter) is the single source of truth for the sound
  beeps, the numerals and the freeze. port_vs_countdown_tick STEPS ASIDE when the
  mode is armed (one guard) so a normal VS round keeps its byte-identical ~2s
  sound countdown. The native beeps (Sound::Play2D 0x2b per count, 0x2a + arena
  music 0x4d at START) ride the stretched cadence.
- RENDER: hal/star_flow.cpp port_luigi_countdown_render(), called in the render
  phase after the actors so the sprites join the VS timer's engine-A OAM batch.
  The banner/START sprite templates (data_ov002_0210d008..0210d420, five
  languages each) are mounted in port/ov002_syms.txt; the render TU rides in
  slice_gate27 with RenderVsTimer (same OAM overloads, same GetOwnerLanguage).
- THE FREEZE is the DS's own: data_0209f2bc != 0 holds the players still
  (func_ov002_020c71e0) for the whole countdown, so the five seconds is a
  get-ready beat, not a run-around; the freeze LIFTS at the pick (proven: the
  local player is frozen f90-240, moves after). LOCKSTEP: the countdown frame
  count and the pick frame are pure functions of the host frame + the env seed,
  identical on every peer -- no local clock, no rand.

## WHERE IT HOOKS
- hal/luigi_infection.cpp -- the whole mode. g_li_team is the single source of
  truth (seed sets one bit, a tag sets another); minimap/win/hit all read it.
  Functions: port_luigi_enabled/seed/minimap_hide/survivors_alive/seeded/
  tag_player/is_infected_player/match_reset (+ the hittest harness).
- SEED: port_luigi_seed(frame) now fires at COUNTDOWN END (frame 240), driven by
  port_luigi_countdown_drive above -- moved off the old one-shot at frame 90 so
  the tagger is chosen only after the READY?/3-2-1/START has played out. The
  SM64DS_VS_LUIGI_TIME match clock is measured from that pick frame, so it starts
  AFTER the countdown, not during it.
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

## VERIFY (2026-09-03, all green; countdown added same day)
- Build EXIT=0, every guard green (dsstate/alternatename/gxband/tailjump/
  closestplayer/inferred_stub). battery.py ALL GREEN (every smoke, every level,
  scene selftests 361-390, default boot, shipcfg build+selftest; linkage 9403,
  ptr_audit 0) -- with the mode OFF, so a normal VS round and the collision
  smokes take the exact byte-unchanged ROM path and pass. src/ diff vs
  04f54ca2b EMPTY (host-layer only: hal/star_flow.cpp, hal/luigi_infection.cpp,
  tests/walk_window.cpp, ov002_syms.txt, slice_gate27.txt, tools/luigi_proof.py).
- COUNTDOWN (port/tools/luigi_proof.py --case seed / det):
    the [luigi] COUNTDOWN log renders READY?+3 @f90, +2 @f150, +1 @f195, START @f240,
    and ONLY THEN SEED @f240 -- the tagger is chosen at countdown end, never before.
    The OAM census (SM64DS_VS_HUD) shows the extra banner+numeral sprites placed
    through the countdown, dropping at f241 (numeral gone) and f271 (START gone).
    det: two instances run the identical countdown and pick on the identical
    frame (f240 -> slot 2). Liveness by movement: the local player is FROZEN
    (1 distinct pos) f90-240 and MOVES (75 pos) after -- the freeze holds and
    lifts at the pick; the tagger stays live with char=1 after.
- FUNCTIONAL (port/tools/luigi_proof.py, single-process 4P VS arena, quiet-spawn):
    seed   ALL PASS -- slot 2 -> Luigi(char 1) at f240 (countdown end), slots
           0/1/3 stay Yoshi(3); minimap s2[LUIGI mapID=-1] while
           s0/s1/s3[surv mapID=0==curmap]; the tagger stays live after the swap.
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
