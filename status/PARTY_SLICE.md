# PARTY_SLICE - the first 2-player party co-op slice

Lane: ADVENTURE co-op, party tier (A leads co-op; C owns rollback). This is the
proof slice COOP_PARTY.md's Status names: the shared, synced world for an opt-in
group, built as C's SHIPPED rollback infra applied through the ADVENTURE invite
path. It is on its own branch, gated by C, and SHIPS NOTHING. The sim is the
shipped rollback machinery UNCHANGED; this slice is host-layer plumbing +
rendering only.

Base: cons tip `f1b05e8fe` (contains M2 adventure/invite AND the shipped
rollback d1ed6ed88). Worktree `C:\tmp\party-build`, branch `port-party-slice`.

## The frozen seam (C's words), driven, never modified

- The rollback NetMode entry, the snapshot ring, and the exchange()/peer_block()
  confirm-rollback hook are FROZEN. This slice only DRIVES them: it hands a party
  session `SM64DS_NETMODE=rollback`, and the shipped handshake does the rest.
- `port_actor_render_replay` (hal/actor_registry.cpp): C's re-sim only changes
  WHICH actors are in its tick-only list. This slice builds the party render
  treatment AGAINST that function -- entirely in the SEPARATE player-body render
  pass (hal/player_bridges.cpp `hal_render_player_world`) that coexists with it,
  never reaching into its actor-id list, never touching its signature or call
  site.
- NO rollback code was edited: `git diff f1b05e8fe HEAD --stat` touches neither
  `port/hal/rollback.cpp`, `port/hal/actor_registry.cpp`, nor
  `port/hal/comms_loopback.cpp`. The frozen files were read only.

## The four steps

1. PARTY SESSION + FLAG. The lobby gains `session_type "party"` beside "vs" and
   "adventure" (port/tools/lobby/app/server.py), reusing the M2 room-code invite
   path. The per-slot party flag (the COOP_PARTY.md first-slice detail "which
   slot field carries is-party-member") is resolved as a dedicated per-slot host
   array `g_peer_party[]` in comms_sync.cpp's presence block, beside the existing
   per-slot level/liveness state, read through `port_party_member(slot)`. First
   slice drives membership from the invite/proof knobs (`SM64DS_PARTY`,
   `SM64DS_PARTY_MEMBERS`); `port_party_set_member()` is the seam the wire writes.

2. SEAT AS ROLLBACK SESSION. A party plan carries `netmode:"rollback"`, so the
   launcher exports `SM64DS_NETMODE=rollback`. That DRIVES the frozen NetMode
   entry: `announce_roster()` carries the accept-rollback bit (bit 17,
   kAcceptRollbackBit 0x00020000), and a joiner that asked for lockstep ADOPTS
   the parent's rollback (comms_loopback.cpp ~2374-2383, the shipped handshake).

3. SHARED WORLD ENTRY. The party go-plan carries `host_level` (= the host's
   current level) for EVERY member, so a joiner boots the HOST's level rather
   than its own (M2 already puts level-id on the wire). Leaving the party drops
   back to solo (ghost tier): STUBBED this slice (see below).

4. GHOST-VS-PARTY RENDER BOUNDARY. The per-slot party flag selects treatment.
   A party member renders SOLID (the ghost-opacity pass is skipped) and COLLIDES
   (the adventure no-collision hold skips it, and the ghost follower leaves it to
   the sim); a non-party remote stays a translucent, pass-through adventure ghost
   (`port_player_render_hidden` + the adventure translucency/hold gate,
   unchanged). Built in the player render pass against `port_actor_render_replay`
   the function, per the frozen seam. `g_party_render_ghost[]` records the actual
   per-slot render treatment so the proof reads an observation, not a re-derived
   decision. Party members are always drawn (a party rides the sim, not the ghost
   broadcast presence gate).

## Proof (headless, 2-instance loopback; port/tools/party_proof.py) -- ALL GREEN

Both runs are quiet/minimized/muted through mp2_proof.env_base; each run's TEMP
is its own dir under the worktree, so the %TEMP%/sm64ds-crashes sink is isolated
(agent test crashes never masquerade as live-player reports).

SINGLE INSTANCE (the render-boundary authority), exe 591DA47A0935B3C0:

    [partyprobe] partyslot=1 ghostslot=2 have_pair=1 not_vs=1 adv_on=1
      party_member=1 party_solid=1 party_collides=1 ghost=1 ghost_translucent=1
      ghost_passthrough=1 local_solid=1 local_interactive=1
      party_live=1(anim 159744->118784 posd=0) => ALL PASS

  Both treatments coexist in ONE run: slot 1 party body SOLID + COLLIDES; slot 2
  non-party GHOST translucent + pass-through; local body solid + interactive;
  the party body TICKS BEHAVIOR (anim advances -> liveness, not a frozen census).

LIVE LOOPBACK (the session authority), two consoles seat as a party:

    parent: NetMode ROLLBACK took
    child : the parent runs NetMode rollback and this end had lockstep; ADOPTING
            the parent's        (bit 17 adoption)
    parent/child: session live mask full (peak 0x3, both seated)
    parent/child: booted the HOST's level 2 (shared world entry)
    parent/child: rb-local same-frame OK
    parent/child: party body TICKS BEHAVIOR (anim advances)
    the shipped sim MOVES a party body through the world (real traversal)
    parent/child: every rewind honoured (unrecoverable=0)

  The child ASKED lockstep and ADOPTED the parent's rollback off bit 17; both
  booted the host's level; the shipped rollback sim re-ticks every Behavior and
  moves the bodies under injected input.

## Regression + seam integrity

- `port/tools/battery.py`: battery ALL GREEN -- every smoke, all 50+ levels
  under FAULTS_FATAL (the level 27 TTC_MOVING_BEAM and 45 GOOMBOSS skips are
  PRE-EXISTING, on other lanes), all 34 scenes, default boot (title), shipcfg
  build+selftest, linkage 9479 (83.7%), ptr_audit 0. No regression to solo, VS,
  ghost/adventure, or default boot.
- `port/tools/lobby/test_units.py`: 530 passed, 0 failed (adds the party session
  tests: party room, host_level handed identically to host AND joiner,
  netmode=rollback, vs/adventure plans carry neither field).
- `git diff f1b05e8fe HEAD -- src/ include/`: EMPTY. Host-layer only. Five files
  changed: comms_sync.cpp, player_bridges.cpp, walk_window.cpp (wiring),
  lobby/app/server.py, lobby/test_units.py.

## Stubbed / deferred to the next slice

- LEAVE -> SOLO (COOP_PARTY.md step 2 tail). `port_party_set_member(slot, 0)`
  clears membership, which drops a body back to the ghost treatment, but the
  full leave path (tear the rollback seat, hand the leaver back to its own solo
  adventure/ghost session, reconcile progress) is not built. Stubbed.
- WIRE-DRIVEN MEMBERSHIP. First slice drives the party flag from the invite/proof
  knobs; wiring `port_party_set_member` off the lobby accept (so membership rides
  the roster rather than an env) is the next seam.
- SHARED-SIM SEATING OF BOTH BODIES IN ONE LIVE RUN. The live run proves the
  session plumbing (seat, adopt, level) and movement via the shipped sim; the
  two-body SOLID-vs-GHOST coexistence is proven deterministically in the single
  instance. Rendering both a solid party body and a translucent ghost together
  in a LIVE loopback session (adventure ghosts + rollback party members sharing
  one level) is the COOP_PARTY.md "distance hybrid / mixed" open question and is
  scoped to the co-design with C.
- The open questions in COOP_PARTY.md (wide cost cap, area-change flush, progress
  ownership) are untouched by this slice and stay for the co-design.

## Status

- 2026-09-03: first 2-player party slice built and proven on `port-party-slice`
  off cons `f1b05e8fe`. battery ALL GREEN, party_proof.py ALL GREEN, lobby tests
  530/0, src diff empty, frozen seam untouched. Ready for C's gate. Not merged,
  not pushed to cons.
