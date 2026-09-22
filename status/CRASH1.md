# CRASH1 (respawn) heartbeat

- 20:00 EDT 2026-09-14 - spawned (respawn #2; first CRASH1 died at 19:42 with empty out/).
  ROOT C:/tmp/l7e on port/l7-crash1 @ 3b72845d4, clean apart from untracked port/build-port-k0.cmd.
  Identity Tango / 248217834+tangosdev@users.noreply.github.com. Starting: verify binary+map,
  resolve +0x272156, then static read of walk_window.cpp main.
- 20:12 EDT - STEP 1 DONE. Fault instruction found statically, no build needed.
  RVA 0x272156 = _main+0xdb6 (map: _main 0001:002703a0 = VA 006713a0, walk_window.cpp.obj).
  Disasm: 0067214a push edi / 0067214b push [0xa66f48 = g_mc] / 00672151 call 0x419070
  = _port_stage_a_boot (level_boot.cpp.obj) / 00672156 movzx eax, word ptr [eax+8] <-- FAULT.
  Source: port/tests/walk_window.cpp:8334-8335
    void *lvl = port_stage_a_boot(g_mc, boot_spawns);
    level_bmd = *(unsigned short *)((char *)lvl + 8);
  NULL = the return value of port_stage_a_boot. Now: step 2, why it returns 0.
- 20:30 EDT - STEP 2 narrowing. port_stage_a_boot returns g_boot_result, filled only if
  st_init (stage_bridges.cpp:350, _ZTV5Stage slot 0) runs. LINK21's FULL run.log survived at
  tmp/int4cap/A_level1_300_crosscheck/run.log: NO output from inside the boot at all, and
  regs edx=017a4a64 (= _data_020a4b88) / ecx=3003a6d8 (= g_stage+0x28) say func_020433b8
  ran to its LAST call func_0203b244(data_020a4b88, self+0x28) without taking an early return.
  func_020433b8 does NOT dispatch slot 0 itself: it delegates to func_0204335c =
  fBase_c::Process(self, data_02099ebc, data_02099ec4, data_02099e94), three mwcc
  pointer-to-member PAIRS. ALL THREE ARE ZERO IN THE SHIPPED IMAGE (.dsstate is fully
  initialized data; dumped at VA 017ab388/398/3a8 = 00000000 x4 each).
- 20:48 EDT - CHAIN PROVEN BY DISASSEMBLY (no build). The built func_0204335c (RVA 1efab0)
  is the port's fBase_c::Process and it dispatches through the OBJECT'S OWN VPTR at fixed
  indexes: ebx=*(void**)self; call [ebx+4] (slot 1); if eax==0 -> jmp 1efb0a which calls
  [ebx+8] (slot 2) and RETURNS WITHOUT EVER CALLING [ebx+0] (slot 0 = st_init = the boot).
  So the boot is skipped whenever slot 1 returns 0.
  Slot 1 as seated = st_binit -> face __ZN5Stage19BeforeInitResourcesEv (005c82c0,
  faces_sync_gen.cpp.obj) -> jmp ?BeforeInitResources@Stage@@UAE_NXZ (006bade0) ->
  jmp ?ResetFadersAndSound@dScene_c@@QAEHXZ (005010b0), which returns 1 (its gate 004b2f90
  is fBase_c::BeforeInitResources, ICF-folded with 12 CRT names, body `mov al,1; ret`).
  So the SEATED slot 1 is healthy. Next suspect: the vptr address point (+0 vs +8), i.e.
  whether [ebx+4] is really _ZTV5Stage[1].
- 21:02 EDT - vptr theory FALSIFIED statically: the only .text site storing a _ZTV5Stage
  address point into an object is _dScStage_c_classInit+0x24 storing table+0 (the +4/+8
  hits are hal_fill_stage_vtable's own seat writes inlined into port_stage_create). So
  [ebx+4] really is _ZTV5Stage[1]. Running ONE quiet arm-A run of the EXISTING artifact
  (no rebuild) with SM64DS_STAGE_SEAT_PROBE=1 + SM64DS_MM_STALE=1 to decide whether the
  boot body ran at all. Taking the slot lock now.
- 21:45 EDT - Static chain verified END TO END and all of it is HEALTHY:
  port_stage_a_boot(019070) -> port_stage_lifecycle_boot(2b3bd0) -> func_020433b8(0bda50)
  -> func_0204335c(1efab0, the Process) -> slot1 st_binit(2b34c0) -> face(1c82c0) ->
  Stage::BeforeInitResources(2bade0) -> dScene_c::ResetFadersAndSound(1010b0) which RETURNS 1.
  Seat writes s0=st_init s1=st_binit s2=st_ainit; dScStage_c_classInit stores vptr=_ZTV5Stage+0;
  g_stage has exactly ONE writer so port_stage_create did not early-return; st_init's targets
  are the real port_stage_boot_arg_mc/_spawn/_body/_set_result. Runtime says the body never ran
  ([mm-stale] is stderr+fflush and is absent; [char] spawning Luigi PROVES env delivery and that
  execution reached the statement right before the boot). Seat-probe silence is NOT evidence:
  it uses printf and the fault handler only fflushes stderr, so stdout dies buffered.
  => taking the brief's step 3: ONE rebuild of port/tests/walk_window.cpp ALONE (Release flags
  kept -- /Od would break ~50 ov007 tail-jump forwarder rows per tools/tailjump_guard.py), with
  a stderr dump of g_stage, its vptr, +0x0e/0f/10 and _ZTV5Stage[0..2]. BUILD RUNNING NOW.
- 22:05 EDT - DIAGNOSTIC RUN 1 (tmp/crash1probe/C/run.log:45-47):
    [crash1] g_stage=3003A6B0 vptr=010FBE80 alive+0x0e=0 +0x0f=0 +0x10=0
    [crash1] &_ZTV5Stage=010FBE80 s0=006B36E0 s1=006B3560 s2=0069CBA0
    [crash1] port_stage_a_boot returned 00000000
  vptr == &_ZTV5Stage EXACTLY (address point +0 confirmed at runtime, vptr theory dead),
  alive byte 0 so the FIRST-ENTRY func_020433b8 path is taken, table seated. Adding one
  more line: call port_stage_boot_body DIRECTLY when the boot returns null, to separate
  "the Process skipped slot 0" from "the body itself returns null".
- 22:30 EDT - MEASURED: [crash1] slot1 direct -> 0, and the body RUNS when called directly
  ([mm-stale] boot entry printed from the DIRECT port_stage_boot_body call). So the boot is
  skipped because _ZTV5Stage[1] returns 0. But the disassembled chain st_binit -> face
  __ZN5Stage19BeforeInitResourcesEv -> ?BeforeInitResources@Stage@@UAE_NXZ ->
  ?ResetFadersAndSound@dScene_c@@QAEHXZ should return 1 (its gate 004b2f90 is `mov al,1; ret`).
  Measuring each link BY SYMBOL to find which one diverges. OVER THE 2h BUDGET; continuing,
  one measurement from the answer.

## CRASH1 CONCLUSIONS (final)

FAULTING LINE  port/tests/walk_window.cpp:8335
    void *lvl = port_stage_a_boot(g_mc, boot_spawns);      // 8334
    level_bmd = *(unsigned short *)((char *)lvl + 8);      // 8335  <-- FAULT
  +0x272156 = _main+0xdb6 = `movzx eax, word ptr [eax+8]` right after
  `call _port_stage_a_boot`; fault regs eax=00000000. Unchecked return, no null guard.

THE NULL  port_stage_a_boot's return = the g_boot_result stash (level_boot.cpp:3113),
  never filled because _ZTV5Stage SLOT 0 (st_init -> port_stage_boot_body) is never
  dispatched. The built fBase_c::Process (func_0204335c, RVA 1efab0) calls SLOT 1 first
  and, when it returns 0, jumps to 1efb0a which runs slot 2 and returns WITHOUT slot 0.
  Measured: [crash1] slot1 direct -> 0. Body is HEALTHY (runs when called directly).
  NOT the vptr: runtime vptr == &_ZTV5Stage exactly (010FBE80), alive+0x0e=0, table seated.

ATTRIBUTION  70e533d3d (2026-08-15, run link60 lane L4, port-only, NOT on origin/main)
  made the boot conditional on slot 1 for the first time. SEAT2's b302c87fe (2026-09-12)
  changed which body slot 1 runs, but is EXONERATED BY TEST: reverting that one hop
  (st_binit -> _ZN8dScene_c19ResetFadersAndSoundEv direct) rebuilt + re-ran arm A and the
  fault is UNCHANGED. Not this campaign's rows, not the main->port sync.

OPEN QUESTION FOR THE NEXT LANE  dScene_c::ResetFadersAndSound measurably returns 0, so
  `if (!fBase_c::BeforeInitResources()) return 0;` takes the false branch -- but that gate
  is ICF-folded to `b0 01 c3` = mov al,1; ret at 004B2F90, verified in the FILE and read
  back from LIVE process memory. Static vs runtime disagree; that is the whole remaining
  bug. Leads: (1) the 13-symbol ICF fold at 004B2F90 (six CRT fns + VIntact + OnHeapCreated)
  -- try one link with /OPT:NOICF; (2) Stage::BeforeInitResources has NO return statement
  (src/_ZN5Stage19BeforeInitResourcesEv.cpp documents it as deliberate for ARM byte-
  exactness; UB on MSVC) and its own comment names the clean fix (declare
  dScene_c::ResetFadersAndSound bool). Both are src/, so NOT applied here.

TREE  artifact restored byte-identical (sha256 bd69278...b4d55), sources reverted+touched,
  git clean apart from the pre-existing untracked port/build-port-k0.cmd. 6 quiet arm-A
  runs under the slot lock. Budget 2h, used ~4h (step-2 stayed static too long).
