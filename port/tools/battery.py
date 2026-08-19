#!/usr/bin/env python3
"""The port's full verification battery, one command.

Everything the merge gate runs, in order, stopping at the first failure:

  1. build            port/build-port.cmd (32-bit MSVC, ninja)
  2. smoke suite      every smoke_*.exe in build/port, exit 0 each
  3. level selftests  walk_window.exe, SM64DS_WINDOW_SELFTEST=300 and
                      SM64DS_FAULTS_FATAL=1, on every mounted level -- the ids
                      are read out of port_level_table[] in hal/level_boot.cpp
                      at run time, so a new mount is covered the moment it
                      lands and no list here can go stale. A level whose
                      blocker belongs to another lane runs with that lane's
                      class skipped, named in LEVEL_SKIPS below and re-probed
                      bare on every run so the skip cannot outlive the bug
  4. scene selftests  walk_window.exe, SM64DS_SCENE=<id> and
                      SM64DS_FAULTS_FATAL=1, on every hosted NON-LEVEL scene --
                      the ids are read out of port_scene_classes[] in
                      hal/scene_boot.cpp at run time, the same rule the level
                      list follows and for the same reason. A scene whose
                      blocker belongs to another lane runs with SCENE_SKIPS
                      naming it and is re-probed bare on every run
  5. linkage          port/tools/linkage.py -- the linked count is printed and
                      compared against --linked-floor if given (a merge must
                      never lower it)
  6. ptr_audit        port/tools/ptr_audit.py -- unhosted code pointers must
                      stay at zero

    python port/tools/battery.py [repo-root] [--linked-floor N] [--skip-build]

Exit 0 all green, 1 first red, with a one-line verdict per step so a log tail
reads as a checklist.

THE SELFTEST BMP TRACKS THE HOSTED-GLOBAL LAYOUT, NOT ONLY THE .dsstate BASE.

Read this before treating a walk_window_selftest.bmp diff as a rendering
regression, and before reading a clean diff as proof that nothing moved.

Some hosted DS data reaches the geometry stream as a pointer VALUE, so the
rendered frame can depend on the ABSOLUTE ADDRESS of a hosted global. Two
separate things move those addresses and both change the frame:

  * the .dsstate section base, which shifts whenever a change grows the
    preceding sections past a 4 KB page boundary, however unrelated that
    change is to rendering;
  * the layout INSIDE .dsstate, which shifts every hosted global past the
    insertion point while leaving the section base exactly where it was.

An equal .dsstate base is therefore necessary for a byte-exact comparison and
it is NOT sufficient. The comparison with meaning holds the whole hosted
layout constant, base and span both.

Measured 2026-08-16 at 7f994ccdb with the ov009 sizing line named below
REVERTED, so the table shows the defect that line fixes. walk_window.exe,
level 1, the .dsstate span as printed by tools/dsstate_guard.py at link time.
Every run ended at the same player position as the unpadded build:

  perturbation                   .dsstate base   span     296 frames   300
  none                           0xbb0000        959200   --           --
  +4 KB inert .data, base moves  0xbb1000        959200   1 px, ch 2   same
  +64 B interior to .dsstate     0xbb0000        959272   1354 px,     same
                                                          ch 16

300 IS A FRAME COUNT ON WHICH BOTH PERTURBATIONS HAPPEN TO AGREE, and 300 is
the count this battery runs. A clean 300-frame BMP compare across a layout
change is not evidence that the layout stayed out of the frame. A sweep of the
same two binaries over counts 290-310, recorded in the same session and not
re-measured line by line for this note, found six counts where the frames
differ -- 291, 292, 296, 298, 302, 303 -- and 300 is not one of them; 292 and
299 were re-measured here and behave as that sweep says. Whether the last
rendered frame is layout dependent is a property of where the animation sits
when the run stops.

The two perturbations also differ by three orders of magnitude, and that gap
is a fingerprint of the cause below rather than a difference in how hard they
push: a 4 KB shift is a whole number of periods of the 32x32 water texture and
very nearly cancels, so it leaves one pixel where 64 bytes leaves 1354.

"ADD A LITTLE INERT .bss" IS NOT A PERTURBATION. Two earlier measurements used
one -- a "base + 16 bytes of inert .bss, no movement" row in the 2026-08-14
table this block used to carry, and a 32-byte version a later lane built an
argument on. Neither moves anything. Measured 2026-08-16: `volatile char
x[32];` in an anonymous namespace is DROPPED, absent from the recompiled .obj
and not merely from the map, because volatile constrains accesses and does not
force emission (`extern "C" volatile char x[32];` is worse: [dcl.link]/7 makes
it a declaration and it emits nothing at all). A spelling that does survive,
`__declspec(dllexport) volatile char x[32];`, present in the map, still leaves
the .dsstate base and span untouched and the frame byte-identical at 296 and
at 300. A null result from a .bss pad is a null result about nothing. Check
any pad for PRESENCE in the .obj or the map before believing what it measures.

The two recipes that do move something, both measured above:

  move the base, at file scope in any hal TU outside a DSSTATE bracket --

    extern "C" __declspec(dllexport) unsigned char rev_pad_data[4096] = {1};

  shift the interior without moving the base, in hal/dsstate_seg.cpp --

    #pragma section(".dsstate$aab", read, write)
    extern "C" __declspec(allocate(".dsstate$aab"))
        __declspec(dllexport) unsigned char pad64[64] = {1};

Read the resulting base and span off the dsstate_guard line the link prints.

So: a PR whose selftest positions match to the digit but whose BMP differs,
and whose dsstate_guard line reports a different base or a different span than
the baseline, is not by itself a render regression. Rebuild the baseline with
inert padding that lands .dsstate at the same base and the same span, and
compare there. The general method: hold everything but the change under test
constant and see whether the BMP follows the change or follows the footprint.
A worked case -- a touch-hosting PR's 318-pixel, max-channel-13 delta was
reproduced exactly by keeping the PR's probe code and REMOVING the change
under test, which proved the delta belonged to the probe's own footprint.

THE CAUSE IS KNOWN AND THE FIX IS IN THIS TREE.

It was never the render path reading an uninitialised field. The castle moat
water's S/T translation is a 91-frame BTA track at ov009 DS 0x021122ec, one
contiguous ramp of 0 to 0x1000 ending exactly where __sinit_ov009_02112458
begins. Nothing named it, so the only thing hosting it was the synthetic gap
block port_ov009_gap_0211222c, which dsd sized at 0xf4 bytes against the first
of eleven `ambiguous` boundaries it guessed inside the track's span -- thirteen
of the ninety-one words. From animation frame 13 the read ran off the block
into whatever the linker placed next, which is port_ov009_gap_02112b7c, laid
down immediately after it with no padding between: the ASCII "water_mat" and
two words the patch pass overwrites with REBASED HOST POINTERS. The overrun
starts on the string and reaches the first pointer three words later, and
func_02044b30 folds the track value into the water's texture-matrix
translation as -(value << 9), so a host address decided pixels.

port/ov009_syms.txt now names the array with its ROM extent
(`data_ov009_021122ec:0x16c`), which pins all 91 frames on hosted storage. With
the line in place the interior shift is byte-identical to the unpadded build at
291, 292, 296, 298, 300, 302 and 303 -- every count on which it moved the frame
before, plus the one this battery runs -- and the base move is byte-identical at
296 and 300. The frames themselves change, because the water now animates off
its own ROM data instead of off a host address: 2962 pixels of the level 1
moat at 300 frames, same player position to the digit.

The fix is one line and a rebase can drop it silently, so check the generated
source rather than trusting this paragraph:

    cd build/port/host-src
    grep -cF "data_ov009_021122ec[364]" ov009_syms.c      -> 1
    grep -cF "port_ov009_gap_0211222c[244]" ov009_syms.c  -> 0

The gap block does not disappear when the line is present, it shrinks 244 to
40 bytes, so its mere presence in the map proves nothing either way.

Five more truncations of the same class are known and NOT fixed here: two
ov009 path tables, ov016 CLPS, an ov021 class-name string, and an ov070 curve
cut at its apex. port/tools/gapaudit.py is the detector. Until those land a
layout change can still reach the frame through one of them, so the comparison
rule above stands whether or not the ov009 line is in the tree. The long form,
with the gap audit and the leftovers, is notes/port-selftest-bmp-gate.md on
main.

A SELFTEST DOES NOT ALWAYS END ON THE LEVEL IT STARTED.

Every selftest log carries TWO [census] blocks -- one after boot and one at the
end of the run -- and the census reports the live actor set at print time.
Often the two match: level 1 prints "82 spawned (22 classes), 0 skipped" twice,
and so do levels 0, 32 and 46.

TWO MATCHING BLOCKS ARE NOT A PROOF THAT THE LEVEL STAYED PUT, and the
converse is not a proof that it warped. The census counts what is ALIVE, and a
level can spawn more of something over 300 frames without going anywhere:
level 30 stays on suisou the whole run, carries no "[lvl] change:" line, and
still goes from 71 spawned to 106 in nine classes. Reading two matching blocks
as "did not warp" happens to be right on most levels, which is what makes it
dangerous.

Levels 19, 20, 26, 34, 35, 39 and 49 warp within the 300 frames. Their logs carry a
"[lvl] change: level N -> M" line, and the second census is then M's, not N's:
level 26 warps to 1 and its second block is level 1's 82/22 exactly, level 39
warps to 5 and drops from 96 spawned to 64. (26 and 39 confirmed on this tree
2026-08-15; the rest are as reported by the mount lanes.)

Level 34 joined that list the day it was mounted, and it is the clearest case
of why the rule is the rule rather than a curiosity: rainbow_mario is Wing
Mario Over the Rainbow, a stage made of platforms over nothing, so the
selftest's idle player walks off and the game sends him to the castle grounds
-- "[lvl] change: level 34 -> 1, entrance 10, reason 0" -- exactly as it
should. Its second census is level 1's.

The battery only reads exit codes and does not care. Anything that reads a
census OFF one of these logs must take the FIRST [census] block, before the
first "[lvl] change:" line -- taking the last one silently files the
destination level's actors under the level that was asked for. Two blocks by
itself is not the warp signal; a "[lvl] change:" line is.
"""

import os
import re
import subprocess
import sys

SELFTEST_FRAMES = "300"
STEP_TIMEOUT = 600

TABLE_OPEN = "static const PortLevelDesc port_level_table[] = {"
SCENE_TABLE_OPEN = "static const PortSceneClass port_scene_classes[] = {"

# A HOSTED SCENE WHOSE BLOCKER IS NOT THE SCENE BOOT, AND WHAT BLOCKS IT.
# Same contract as LEVEL_SKIPS above, one row down: scene id -> (env to set,
# who owns the fix, what it looks like). The env is applied on top of the scene
# selftest's own, and the bare run is re-probed on every pass so a skip cannot
# outlive its bug. Empty is the goal.
SCENE_SKIPS = {
    # SCENE 390 (0x186), dScMgFlower_c, run mg5 lane FLW. The class BOOTS and
    # TICKS: 300 frames, exit 0, slot 0 InitResources once, slot 6 Behavior
    # 253 times, its own state variable at +0x5fe8 advancing 0 -> 1, and its
    # sub-object member-pointer dispatch routing 253 calls with 0 unhandled
    # addresses. What it cannot yet do is RENDER, and that path was
    # unreachable until this seat existed, so this is a frontier the seat
    # opened rather than a regression it caused.
    #
    #   flw_render (slot 9) -> func_ov006_0212aacc -> func_ov006_020c3bf4
    #     -> ModelAnim::Virtual18 -> ModelAnim::Virtual10
    #   FAULT c0000005 accessing 00000000, eax=0
    #
    # src/func_ov006_020c3bf4.cpp's last statement before the petal loop is a
    # VIRTUAL call on the ModelAnim at sub-object +0xd18
    # (`((Obj*)(c + 0xd18))->f5(0)` in its own spelling), and the null is
    # inside arm9's ModelAnim, not in ov006.
    #
    # WHAT IS MEASURED AND WHAT IS NOT, kept apart on purpose. MEASURED: the
    # fault is UPSTREAM of the 0x16-iteration petal loop, so it is NOT
    # explained by this class's one undecompiled body (func_ov006_0212a764,
    # the petal layout, which the run reports entering once and which leaves
    # every petal record zeroed). NOT MEASURED: whether the ModelAnim at +0xd18
    # ever received a file. func_ov006_020c3d88 loads one through the
    # SharedFilePtr at +0xd7c that the factory constructs with id 0x222, and
    # nobody has checked what that resolves to on the host.
    #
    # OWNED BY: whoever takes the ov006 minigame RENDER path next -- it is a
    # model/animation seam question, not a vtable or dispatch one, and this
    # lane's brief scopes it out. The bare run is re-probed every pass, so the
    # day it stops faulting this row prints SKIP RETIRED.
    390: ("SM64DS_SCENE_SLOT9=0",
          "run mg5 lane FLW opened it; owned by the ov006 render seam",
          "null ModelAnim virtual inside func_ov006_020c3bf4, upstream of the "
          "petal loop"),
    # Scene 4's SM64DS_SCENE_SLOT9=0 entry retired 2026-08-15 (run
    # link60, lane L1): the blocker was never the model loader the entry named.
    # func_0205a358's spin-wait on GXSTAT bit 25 could not fall through because
    # ntr modelled GXSTAT as a plain latch and func_0205583c's store of 0 wiped
    # the read-only FIFO-status bits. Fixed in port/ntr/io.cpp; the bare probe
    # went 300 frames clean with render-slot hits 300 and the gate printed
    # SKIP RETIRED on its first run over the fixed tree. The entry shape is
    # documented in the skip section above; new debts go here with the scene,
    # the owning lane, and the evidence.
}

# A HOSTED SCENE THAT CANNOT COMPLETE A SELFTEST AT ALL, AND WHY.
#
# SCENE_SKIPS above is for a scene that PASSES once one slot is switched off.
# This is the row below that: a scene whose blocker is upstream of any frame,
# so there is no env that makes 300 frames happen and a skip would be a lie.
# The scene is still HOSTED -- its ACTOR_SPAWN_TABLE row is the ROM's own edge
# and it is what /OPT:REF follows to keep the class's TUs in the binary -- so
# leaving it out of port_scene_classes to dodge the battery would delete real
# linkage to make a number look better.
#
# scene id -> (who owns the fix, a STRING THAT MUST APPEAR in the run's output,
#              what it looks like)
#
# THIS IS NOT A SKIP AND IT IS NOT WEAKER THAN ONE, in the direction that
# matters. A skip asserts "it passes with this env". This asserts the OPPOSITE
# and checks it: the battery still runs the scene bare, and it FAILS the
# battery unless the run reproduces THE NAMED BLOCKER. A different crash is a
# regression and reads as one, and a run that unexpectedly SUCCEEDS prints
# BLOCK RETIRED, exactly like a retired skip. The marker string is what pins
# it: without one, any new fault would read as the known one.
SCENE_BLOCKED = {
    # SCENE 390 (0x186), dScMgFlower_c, WAS HERE FOR ONE RUN AND IS RETIRED,
    # run mg5 lane FLW. The row named an ov004 ARM ARGUMENT RIDE-THROUGH two
    # calls into the class's own InitResources -- the third instance of the
    # family port/mg_fanout_costs.txt section 6 records for slots 5 and 7, and
    # the first of the three found by running instead of by reading:
    #
    #   flw_init -> func_ov006_0212b480 (slot 0) +0xa
    #            -> func_ov004_020ad8b8 +0x10 -> func_ov004_020adc3c +0x6
    #   FAULT c0000005 accessing 0x00000009
    #
    # The ROM at 0x020ad8c4 does `ldr r0,[r0]` off data_ov004_020beb68 and then
    # `bl 0x20adc3c` without touching r0, so one register does the null test
    # and carries the argument. src/func_ov004_020ad8b8.c declares the callee
    # (void) and calls it with none; src/func_ov004_020adc3c.c takes void* and
    # reads c+8 first statement, so on the host it read [esp+4], which held 1.
    #
    # RETIRED BY THE FIX SECTION 6 PRESCRIBES, not by a workaround and not by a
    # skip: port/unmatched/MgFlower_InitScore.cpp is a host copy of
    # func_ov004_020ad8b8 that places the argument, src/ untouched, and it cost
    # the one linked function that made it the coordinator's ruling to grant.
    # Scene 390 now runs 300 frames bare under SM64DS_FAULTS_FATAL=1 with
    # nothing switched off and its REAL slot-0 InitResources, so it is an
    # ordinary scene selftest row above and this dict is empty again.
    #
    # THE ROW'S REPLACEMENT IS THAT ORDINARY ROW. If the host copy is ever
    # dropped, scene 390 faults in InitResources again and goes red on the next
    # battery, which is a stronger signal than a marker string.
    # SCENE 1 WAS HERE AND IS RETIRED, run link60 Stage 5 lane MR1. The row
    # had been CONVERTED FOUR TIMES and every conversion was a real blocker
    # retired by a real seat: func_ov007_020c9688 (lane L2, host transcription
    # by lane SC1), func_ov007_020b2998 (SC1, MATCHED by CK1, brought across by
    # PC2), the ARM register ride-through at func_ov007_020be980 (PC2, seated
    # by RT1) and the implicit r0 argument at func_ov007_020ae558 (RT1, seated
    # by AE1). The fifth blocker was the FIRST of the four that was not an
    # argument at all: src/func_ov007_020bbff0.c reached
    # ModelComponents::Render through an /alternatename onto a __thiscall
    # member, so the receiver was never delivered and all three arguments
    # landed one slot along.
    #
    # MR1 STOOD A RECEIVER-BRIDGING FACE in hal/scene_boot.cpp, in the shape
    # Scene::SetFaders twelve lines above it uses, deleted the directive, and
    # the scene ran. THE COUNTERS ARE THE EVIDENCE AND THE RENDER SLOT IS THE
    # ONE THAT MOVED: init 1, behavior 299, RENDER 300, cleanup 0,
    # pending-destroy 0, over a bare 300-frame run with SM64DS_FAULTS_FATAL=1
    # and NOTHING switched off. Every earlier clean scene-1 run in this file's
    # history was taken with the Render slot no-op'd; this one is not, and the
    # difference is the whole point.
    #
    # SO THIS IS A RETIREMENT, NOT A FIFTH CONVERSION: the scene has no blocker
    # left to name, and it runs as an ordinary scene selftest row above.
    # The marker and its predicate STAY in the tree on purpose. The predicate
    # in port/CMakeLists.txt is now anchored to the pragma line (MR1 closed the
    # unanchored-comment hole the AE1 review demonstrated, in the same commit
    # that created the comment that would have fooled it), so restoring the
    # receiver-dropping alias arms the marker again and the scene's own row
    # goes red at the same time. Two independent signals instead of one.
    #
    # AND THE FRAME IS NOT A TITLE SCREEN. The Render slot RUNS, which is the
    # milestone and is what retires this row; what it draws is three colors.
    # Decomposed in seat section 5f: 93.52% flat clear color, one solid 128x96
    # magenta rectangle (the sub panel) and its 452-pixel outline, nothing
    # else. Nobody should read this retirement as the title screen appearing.
    #
    # Full write-up, with the disassembled face and the five proof configures:
    # port/ov007_seat.txt sections 5f, 7 and 8.
    # SCENE 374 (0x176) WAS HERE AND IS RETIRED, run link60 lane FDR2. The
    # row had been CONVERTED TWICE and each conversion was a seat that landed:
    # lane MG2 recorded the arm9 fader (data_0209f61c's vptr was zero because
    # nothing ran its constructor), lane FDR retired that by running the ROM's
    # own src/__sinit_02074f80.c and converted the row to the NitroSDK
    # open-by-name file system, and lane NFS retired THAT by running the ROM's
    # own archive registration behind a host read face. The third blocker was
    # the fader again and it was not a missing body: Scene::BeforeBehavior
    # pushes two arguments into vtable slot 0x0c and cleans nothing, because
    # MSVC's __thiscall is callee-cleans, and the fader seat's trap stub was
    # declared `int __fastcall(void *, void *)` -- both parameters in
    # registers, cleaning nothing -- so eight bytes leaked and the caller's
    # epilogue read them back as esi, ebp and a return address.
    #
    # FDR2 re-declared the two stubs for the shape their call sites emit and
    # audited all twelve (port/fader_boot_map.txt section 9). Scene 374 then
    # ran 300 frames under SM64DS_FAULTS_FATAL=1 with exit 0, no fault, no
    # quarantine line in the playlog, and the state machine dispatching 32,557
    # calls with 0 unhandled addresses. So this is a RETIREMENT and not a
    # fourth conversion: the scene has no blocker left to name.
    #
    # WHAT WAS STILL MISSING WAS NOT A BLOCKER EITHER, and it is no longer
    # missing. The paragraph this replaces said the three dWipe_c motion slots
    # were still named traps and that hal/scene_mg.cpp printed a FADE MOTION
    # MISSING advisory keyed on port_fdr_motion_slots_unseated(), "which goes
    # quiet by itself when the ROM bodies are seated". Run link60 Stage 5 lane
    # SEAT8 seated the last of them (slot 0x08, func_0202f428) and wired the
    # ROM's own driver for it, so the predicate and the advisory are both
    # retired. The rule they were written under stands unchanged: an advisory
    # is not a battery row and must not become one again.

    # SCENE 376 (0x178), dScMgSmartball_c, run mg5 lane SMB. THE FIRST ROW IN
    # THIS TABLE WHOSE BLOCKER IS A DECOMP GAP RATHER THAN A PORT SEAT, which
    # is why it is worth reading before it is worth fixing: every blocker this
    # table has ever carried was something the port had not built yet, and this
    # one is something the DECOMP has not built yet.
    #
    # TWO OF THE CLASS'S NINE VTABLE OVERRIDES HAVE NO SOURCE AT ALL. No block
    # in config/arm9/overlays/ov006/delinks.txt covers 0x02118b70 (slot 0,
    # InitResources) or 0x021173c8 (slot 9, Render), and no src file defines
    # either; both are named, self-reporting traps in
    # port/unmatched/MgSmartball_Traps.cpp on the func_ov006_020e1854
    # precedent. Slot 0 is the one that blocks: it is where the class builds
    # every sub-object its Behavior ticks, so with it trapped the object's
    # count fields stay zero and its sub-object pointers stay null.
    #
    # THE FAULT, from a bare 300-frame run with SM64DS_FAULTS_FATAL=1:
    #
    #     FAULT code c0000005 at +0x0005049b accessing 00000000
    #       walker node 307F9D8C actor 307F9D64 id 0x178
    #       eax=00000000 ecx=307f9d64
    #
    #     port/tools/faultmap.py: _func_ov006_02118488 +0x11b into it
    #
    # which is src/func_ov006_02118488.c's case 0 opening statement,
    # `o = *(char**)(c + 0x4684); (**(VFunc**)o)(o);`, with no null guard --
    # correct on the ROM, where slot 0 ran.
    #
    # THE MARKER IS THE FIELD, NOT THE ADDRESS, and hal/scene_mg.cpp's slot 6
    # thunk prints it one frame before the fault, the way the fader pre-flight
    # printed scene 374's. A link offset moves on every build and cannot pin a
    # blocker; self+0x4684 being null while the state at self+0x4660 is zero
    # can, and it is a statement about the class rather than about the binary.
    # A different crash does not reproduce it and reads as a regression, which
    # is what this table is for.
    #
    # WHAT IS ALREADY PROVEN, so a reader knows what the row does NOT cover:
    # the class registers (the SpawnInfo +4 cross-check passes), both 36-slot
    # tables fill with ZERO raw DS words left, the factory spawns the object
    # with a host vptr, and slots 0, 1, 2, 6, 7, 26, 31, 32 and 33 all
    # dispatch. The seat is complete; the body behind slot 0 is not.
    #
    # RETIRING IT IS A BYTE-GATED-TREE JOB and it is routed rather than taken:
    # 0x02118b70 is 0x8dc bytes and 0x021173c8 is 0x10c0, both inside delink
    # holes that have to be split before either can be decompiled. Derivation
    # in port/slice_smb.txt.
    #
    # RUN mg5 LANE INTEG MOVED THIS BLOCKER ONE FLOOR DEEPER, which is progress
    # and is why the marker below changed. InitResources (0x02118b70) is now
    # seated -- the plain-C _ZN16dScMgSmartball_c13InitResourcesEv aliased onto
    # the ROM address name -- so slot 6 no longer dereferences a null
    # self+0x4684 (that marker is gone). The class now BUILDS its sub-objects,
    # and the first sub-object's method call faults because that sub-object's
    # ov006 vtable at 0x0213eca0 still holds raw DS addresses (its slot resolves
    # to func_ov006_02114458, not seated), so the call jumps into DS space.
    # Seating that whole sub-object class and relocating its vtable is a separate
    # lane. hal/scene_mg.cpp's 376 SEAT line prints the new marker before the
    # fault, the way the slot-6 thunk printed the old one. (Baseline check: scene
    # 390 faults at HEAD 19dc07ef2 too, in an unrelated flower-render floor this
    # lane did not touch, so it is a pre-existing red rather than this lane's.)
    376: ("a separate sub-object-class lane (decomp body + port vtable relocation)",
          "vtable at 0x0213eca0 holds raw DS addresses",
          "run mg5 lane INTEG seated slot 0 InitResources, clearing the old null "
          "self+0x4684 deref; the class now builds its sub-objects and the "
          "blocker moved one floor deeper to a sub-object whose ov006 vtable at "
          "0x0213eca0 holds raw DS addresses (func_ov006_02114458, not seated), "
          "so its first method call jumps into DS space. Slot 9 Render stays "
          "trapped behind the thiscall wall"),
}

# A MOUNTED LEVEL WHOSE BLOCKER IS NOT THE MOUNT, AND THE CLASS THAT BLOCKS IT.
#
# level id -> (SM64DS_SKIP_CLASS value, who owns the fix, what it looks like)
#
# The alternative to this table is worse in both directions. Leave the level
# out of the table in level_boot.cpp and a proven mount goes unshipped because
# somebody else's actor is broken; leave it in and run it bare and the battery
# is red on every lane until that actor is fixed. So the mount lands, the
# battery covers it, and the one class that is not the mount's responsibility
# is named here in the open.
#
# A SKIP HERE IS A DEBT, AND THE BATTERY COLLECTS IT. Every entry is re-probed
# BARE on every run (retire_probe below). The moment the owning lane's fix
# lands, the bare run goes green and the battery says so in capitals, so the
# skip cannot quietly become permanent -- which is the only way a mechanism
# like this stays honest. Deleting a retired entry is the whole maintenance
# burden.
LEVEL_SKIPS = {
    # Empty since wave C closed. Level 33's SNUFIT entry retired 2026-08-15:
    # the w19 slot-5 fix and the level-33 mount met at the wave's final
    # merge, the bare run went 300 frames clean, and the battery printed
    # SKIP RETIRED on its first run over the combined tree. The entry shape
    # is documented in this file's skip section; new debts go here with the
    # class, the owning lane, and the evidence, never a raw fault offset.
}
# The bare re-probe is expected to FAULT while the debt stands, and a fault
# under FAULTS_FATAL exits fast. A probe that instead hangs is not evidence of
# anything, so it gets a short leash and is read as "still needed".
RETIRE_PROBE_TIMEOUT = 120


def mounted_levels(root):
    """Every mounted level id, read out of port_level_table[] at run time.

    This list used to be a literal here, and a literal is how the battery
    silently under-tests: run linkw wave 8 mounted sixteen levels and the tuple
    sat at nineteen ids for the rest of the wave, so the shipped battery
    covered 19 of 35 mounts and every lane that ran it read a green it had not
    earned. Deriving it is the only form that cannot go stale.

    A parse failure is fatal on purpose. Falling back to a built-in list would
    reintroduce exactly the bug this replaced -- a battery that keeps printing
    greens while testing a set nobody has checked in months.
    """
    path = os.path.join(root, "port", "hal", "level_boot.cpp")
    with open(path, encoding="utf-8", errors="replace") as f:
        text = f.read()

    i = text.find(TABLE_OPEN)
    if i < 0:
        raise SystemExit(f"battery: no port_level_table[] in {path}")
    i += len(TABLE_OPEN)
    j = text.find("\n};", i)
    if j < 0:
        raise SystemExit(f"battery: unterminated port_level_table[] in {path}")

    # The table carries per-wave comment blocks that quote ROM tables and row
    # fields, so strip comments before matching or a quoted id becomes a level.
    body = re.sub(r"/\*.*?\*/", " ", text[i:j], flags=re.S)
    body = re.sub(r"//[^\n]*", " ", body)

    # A row is {<id>, "<name>", "ov0NN", 0x0......., ...}; the name string is
    # what keeps the pattern off any other brace in the span.
    ids = [int(n) for n in re.findall(r"\{\s*(\d+)\s*,\s*\"", body)]
    if not ids:
        raise SystemExit(f"battery: port_level_table[] parsed empty in {path}")

    dupes = sorted({n for n in ids if ids.count(n) > 1})
    if dupes:
        # port_level_desc_for() returns the first match, so a duplicate id is a
        # dead row, not a harmless one.
        raise SystemExit(f"battery: duplicate level ids in port_level_table[]:"
                         f" {dupes}")

    return tuple(sorted(ids))


def hosted_scenes(root):
    """Every hosted NON-LEVEL scene id, read out of port_scene_classes[].

    Derived rather than listed, for the reason mounted_levels() is derived: a
    literal here goes stale the first time a lane seats a scene, and a battery
    that keeps printing green over a set nobody has looked at is the exact bug
    that list replaced. A parse failure is fatal for the same reason; an ABSENT
    table is not, because a tree with no scene boot at all is a legitimate
    state and there is nothing to under-test in it.
    """
    path = os.path.join(root, "port", "hal", "scene_boot.cpp")
    if not os.path.exists(path):
        return ()
    with open(path, encoding="utf-8", errors="replace") as f:
        text = f.read()

    i = text.find(SCENE_TABLE_OPEN)
    if i < 0:
        raise SystemExit(f"battery: no port_scene_classes[] in {path}")
    i += len(SCENE_TABLE_OPEN)
    j = text.find("\n};", i)
    if j < 0:
        raise SystemExit(f"battery: unterminated port_scene_classes[] in {path}")

    body = re.sub(r"/\*.*?\*/", " ", text[i:j], flags=re.S)
    body = re.sub(r"//[^\n]*", " ", body)
    # a row is {<id>, "<NAME>", <info>, <factory>, <fill>}; the terminator
    # {0, 0, 0, 0, 0} has no string and does not match.
    # HEX IS ACCEPTED, run link60 lane MG2. This used to read \d+ only, and a
    # scene written {0x176, "..."} -- which is the natural spelling, because an
    # actor id is hex everywhere else in the tree -- simply did not match. The
    # failure is SILENT UNDER-TESTING, the exact shape this function's own
    # docstring says it exists to prevent: the scene is hosted, the battery
    # does not know it exists, and the run stays green over it. It surfaced
    # here only because SCENE_BLOCKED named an id the parse had dropped and the
    # orphan check caught the disagreement.
    ids = [int(n, 0) for n in
           re.findall(r"\{\s*(0[xX][0-9a-fA-F]+|\d+)\s*,\s*\"", body)]
    if not ids:
        raise SystemExit(f"battery: port_scene_classes[] parsed empty in {path}")

    dupes = sorted({n for n in ids if ids.count(n) > 1})
    if dupes:
        raise SystemExit(f"battery: duplicate scene ids in port_scene_classes[]:"
                         f" {dupes}")
    return tuple(sorted(ids))


def run(cmd, cwd, env=None, timeout=STEP_TIMEOUT):
    return subprocess.run(cmd, cwd=cwd, env=env, timeout=timeout,
                          capture_output=True, text=True)


def selftest_env(lvl, skip=None):
    env = dict(os.environ,
               SM64DS_LEVEL=str(lvl),
               SM64DS_FAULTS_FATAL="1",
               SM64DS_WINDOW_SELFTEST=SELFTEST_FRAMES)
    if skip:
        env["SM64DS_SKIP_CLASS"] = skip
    else:
        # The battery's own environment must not decide what a level runs. An
        # SM64DS_SKIP_CLASS inherited from whoever invoked it would let a lane
        # skip its way to a green over levels this table says need nothing.
        env.pop("SM64DS_SKIP_CLASS", None)
    # SM64DS_DUAL_SCREEN forces the stacked layout on or off (hal/sub_screen.cpp).
    # A level is inset by default and the selftest BMP this step compares is the
    # 512x384 framebuffer in either layout, but an inherited force-on would still
    # change what the run presents and what its window is shaped like, and the
    # comparator must measure the default rather than the caller's preference.
    env.pop("SM64DS_DUAL_SCREEN", None)
    # SM64DS_CLICK_TEST (lane TCH2) is dropped here as well as in scene_env. The
    # driver already refuses to arm under a selftest, so this is the second of
    # two locks rather than the only one -- and it is the cheap one to keep.
    env.pop("SM64DS_CLICK_TEST", None)
    # SM64DS_TOUCH_PROBE is the third member of the injected-input class and
    # the one with NO selftest gate of its own: it injects synthetic stylus
    # presses outside the g_headless guard. Review TCR1 measured an inherited
    # one changing the level-1 selftest BMP (524 probe lines, d2cec869 instead
    # of the baseline). It is dropped in both env builders for that reason.
    env.pop("SM64DS_TOUCH_PROBE", None)
    return env


def retire_probe(build, lvl):
    """Does level `lvl` still need its skip? (True still needed, False retired).

    Runs the level BARE. While the debt stands this faults under FAULTS_FATAL
    and returns quickly; when the owning lane's fix lands it returns 0 and the
    entry in LEVEL_SKIPS is dead weight.
    """
    try:
        r = run([os.path.join(build, "walk_window.exe")], build,
                env=selftest_env(lvl), timeout=RETIRE_PROBE_TIMEOUT)
    except subprocess.TimeoutExpired:
        return True, "the bare run did not finish inside %ds" % \
            RETIRE_PROBE_TIMEOUT
    if r.returncode:
        return True, "bare rc=%d" % r.returncode
    return False, "bare rc=0"


def scene_env(scene, extra=None):
    """The scene selftest's environment. SM64DS_SCENE takes the whole run
    (walk_window hands over to hal/scene_boot.cpp's port_scene_run before the
    first level-shaped statement), so SM64DS_LEVEL and the level knobs are not
    just unnecessary here, they are inapplicable -- and SM64DS_LEVEL is dropped
    so an inherited one cannot make a scene run read as a level run.
    Same frame count as the level selftests, for the same reason: a number that
    two steps disagree on is a number a reader has to look up."""
    env = dict(os.environ,
               SM64DS_SCENE=str(scene),
               SM64DS_SCENE_FRAMES=SELFTEST_FRAMES,
               SM64DS_FAULTS_FATAL="1")
    # EVERY scene knob is dropped before the table's own is applied, not just
    # the level ones. The battery's own environment must not decide what a
    # scene runs: an inherited SM64DS_SCENE_SLOT9=0 would let a lane skip its
    # way to a green over a scene this table says needs nothing, which is the
    # identical hole SM64DS_SKIP_CLASS is popped for one line up.
    # SM64DS_DUAL_SCREEN is in the list for the same reason the rest are. A
    # minigame scene defaults to the stacked layout out of the ROM's own
    # IsMinigameActorID and a non-minigame scene defaults to the inset panel;
    # an inherited force would have the caller decide that instead of the code
    # under test, in both directions.
    # SM64DS_SCENE_WINDOW AND SM64DS_CLICK_TEST ARE IN THE LIST BECAUSE A
    # HEADLESS ROW MUST STAY HEADLESS. Measured by run link60 lane SWR1: with
    # SM64DS_SCENE_WINDOW inherited from the shell a battery scene row opens a
    # REAL window, which puts a live mouse into poll_touch and a machine-global
    # TAB into the panel latch, and both of those move the frame this step is
    # comparing. The lane that added the variable wrote "nothing in the battery
    # sets it", which was true and is not the same claim as "the battery cannot
    # receive it". SM64DS_CLICK_TEST (lane TCH2's scripted stylus) is dropped
    # in the same breath and for the identical reason: a click script inherited
    # from a shell would drive synthetic presses into a comparison run.
    # SM64DS_PAD_TEST joins them, and its absence was a real hole rather than a
    # deliberate omission: a scene row does NOT set SM64DS_WINDOW_SELFTEST, so
    # g_selftest is false on this path and the scripted pad is live. An
    # inherited one would press buttons into a scene the battery is measuring,
    # and the click and the pad are the same class of input with the same
    # exposure.
    for k in ("SM64DS_LEVEL", "SM64DS_SKIP_CLASS", "SM64DS_SCENE_NO_RENDER",
              "SM64DS_SCENE_BMP", "SM64DS_SCENE_BMP_STACKED",
              "SM64DS_SCENE_TRACE", "SM64DS_SCENE_SLOT9",
              "SM64DS_SCENE_SUBLEVEL", "SM64DS_DUAL_SCREEN", "PORT_WATCHDOG",
              "SM64DS_SCENE_WINDOW", "SM64DS_CLICK_TEST", "SM64DS_PAD_TEST",
              "SM64DS_TOUCH_PROBE"):
        env.pop(k, None)
    if extra:
        for kv in extra.split(","):
            k, _, v = kv.partition("=")
            env[k] = v or "1"
    return env


def scene_retire_probe(build, scene):
    """Does scene `scene` still need its skip? Same contract as retire_probe,
    with one difference that matters: scene 4's debt is a HANG, not a fault, so
    the bare probe does not exit fast the way a FAULTS_FATAL crash does. It
    runs out the leash instead, and the timeout IS the evidence. Read
    "did not finish inside Ns" as "still needed", the same as a nonzero rc."""
    try:
        r = run([os.path.join(build, "walk_window.exe")], build,
                env=scene_env(scene), timeout=RETIRE_PROBE_TIMEOUT)
    except subprocess.TimeoutExpired:
        return True, "the bare run did not finish inside %ds" % \
            RETIRE_PROBE_TIMEOUT
    if r.returncode:
        return True, "bare rc=%d" % r.returncode
    return False, "bare rc=0"


def main():
    args = [a for a in sys.argv[1:]]
    floor = 0
    if "--linked-floor" in args:
        i = args.index("--linked-floor")
        floor = int(args[i + 1])
        del args[i:i + 2]
    skip_build = "--skip-build" in args
    if skip_build:
        args.remove("--skip-build")
    root = os.path.abspath(args[0] if args else ".")
    build = os.path.join(root, "build", "port")

    if not skip_build:
        r = run(["cmd", "/c", os.path.join(root, "port", "build-port.cmd")],
                root)
        if r.returncode:
            print("build: FAIL")
            print(r.stdout[-2000:])
            print(r.stderr[-2000:])
            return 1
        print("build: ok")

    smokes = sorted(f for f in os.listdir(build)
                    if f.startswith("smoke") and f.endswith(".exe"))
    for exe in smokes:
        r = run([os.path.join(build, exe)], build)
        tail = (r.stdout.strip().splitlines() or [""])[-1][:90]
        if r.returncode:
            print(f"{exe}: FAIL rc={r.returncode} {tail}")
            return 1
        print(f"{exe}: ok  {tail}")

    # FAULTS_FATAL is not optional here. Without it a level can take an access
    # violation inside a quarantined actor, freeze that actor, keep ticking and
    # exit 0 -- so the battery passes a level that is visibly broken. Every
    # lane's own census runs set it; the battery did not, which was the same
    # class of unearned green as the hand-maintained level list it used to run.
    levels = mounted_levels(root)
    print(f"levels: {len(levels)} mounted, from hal/level_boot.cpp")

    # A skip for a level that is not mounted is the staleness bug that killed
    # the hand-maintained level list, in miniature: the entry reads as covered
    # and tests nothing. Refuse it rather than skip past it.
    orphans = sorted(set(LEVEL_SKIPS) - set(levels))
    if orphans:
        print(f"levels: FAIL, LEVEL_SKIPS names unmounted level(s) {orphans}")
        return 1

    retired = []
    for lvl in levels:
        skip = LEVEL_SKIPS.get(lvl)
        env = selftest_env(lvl, skip[0] if skip else None)
        r = run([os.path.join(build, "walk_window.exe")], build, env=env)
        if r.returncode:
            print(f"selftest level {lvl}: FAIL rc={r.returncode}"
                  + (f" (SM64DS_SKIP_CLASS={skip[0]})" if skip else ""))
            print(r.stdout[-1500:])
            return 1
        if not skip:
            print(f"selftest level {lvl}: ok")
            continue
        still, how = retire_probe(build, lvl)
        print(f"selftest level {lvl}: ok with SM64DS_SKIP_CLASS={skip[0]}"
              f", owned by {skip[1]} ({how})")
        if not still:
            retired.append(lvl)

    for lvl in retired:
        skip = LEVEL_SKIPS[lvl]
        print(f"SKIP RETIRED: level {lvl} now runs 300 frames clean BARE. "
              f"{skip[0]} is fixed, so delete level {lvl} from LEVEL_SKIPS in "
              f"port/tools/battery.py -- the level is being tested with a "
              f"class switched off for no reason.")

    # THE SCENE SELFTESTS. Same shape as the level ones a few lines up, over a
    # different mode of the game: SM64DS_SCENE=<id> hands walk_window's whole
    # run to hal/scene_boot.cpp's port_scene_run, which boots the scene through
    # the ROM's own Scene::SetSceneToSpawn -> Scene::SpawnIfNecessary chain and
    # runs the same five actor phases for the same 300 frames. FAULTS_FATAL for
    # the same reason: without it a quarantined fault reads as a pass.
    scenes = hosted_scenes(root)
    print(f"scenes: {len(scenes)} hosted, from hal/scene_boot.cpp")
    # A skip for a scene that is not hosted reads as covered and tests nothing,
    # the same staleness bug the level orphan check refuses. It also makes the
    # final "skips:" line load-bearing: a non-empty SCENE_SKIPS can only reach
    # that print if every one of its ids was hosted AND its selftest passed, so
    # an ALL GREEN carrying a scene skip is proof the scene step really ran.
    scene_orphans = sorted(set(SCENE_SKIPS) - set(scenes))
    if scene_orphans:
        print(f"scenes: FAIL, SCENE_SKIPS names unhosted scene(s) "
              f"{scene_orphans}")
        return 1
    scene_block_orphans = sorted(set(SCENE_BLOCKED) - set(scenes))
    if scene_block_orphans:
        print(f"scenes: FAIL, SCENE_BLOCKED names unhosted scene(s) "
              f"{scene_block_orphans}")
        return 1
    both = sorted(set(SCENE_BLOCKED) & set(SCENE_SKIPS))
    if both:
        print(f"scenes: FAIL, scene(s) {both} are in BOTH SCENE_SKIPS and "
              f"SCENE_BLOCKED -- a scene cannot both pass with an env and be "
              f"unable to run.")
        return 1

    scene_retired = []
    scene_unblocked = []
    for sc in scenes:
        block = SCENE_BLOCKED.get(sc)
        if block:
            r = run([os.path.join(build, "walk_window.exe")], build,
                    env=scene_env(sc))
            # BOTH streams: the unmatched-body trap that names the blocker
            # writes to stderr (unbuffered, so a fault cannot swallow it) and
            # the scene's own progress lines go to stdout.
            out = (r.stdout or "") + (r.stderr or "")
            if not r.returncode:
                print(f"selftest scene {sc}: BLOCK RETIRED -- the bare run "
                      f"now completes. Delete scene {sc} from SCENE_BLOCKED "
                      f"in port/tools/battery.py.")
                scene_unblocked.append(sc)
                continue
            if block[1] not in out:
                print(f"selftest scene {sc}: FAIL rc={r.returncode} -- it "
                      f"failed, but NOT with its recorded blocker. "
                      f"SCENE_BLOCKED expects {block[1]!r} in the output and "
                      f"it is not there, so this is a different failure.")
                print(out[-1500:])
                return 1
            print(f"selftest scene {sc}: BLOCKED as recorded, owned by "
                  f"{block[0]} (rc={r.returncode}, blocker reproduced)")
            continue
        skip = SCENE_SKIPS.get(sc)
        r = run([os.path.join(build, "walk_window.exe")], build,
                env=scene_env(sc, skip[0] if skip else None))
        if r.returncode:
            print(f"selftest scene {sc}: FAIL rc={r.returncode}"
                  + (f" ({skip[0]})" if skip else ""))
            print(r.stdout[-1500:])
            return 1
        if not skip:
            print(f"selftest scene {sc}: ok")
            continue
        still, how = scene_retire_probe(build, sc)
        print(f"selftest scene {sc}: ok with {skip[0]}, owned by {skip[1]}"
              f" ({how})")
        if not still:
            scene_retired.append(sc)

    for sc in scene_retired:
        skip = SCENE_SKIPS[sc]
        print(f"SKIP RETIRED: scene {sc} now runs {SELFTEST_FRAMES} frames "
              f"clean BARE. {skip[0]} is no longer needed, so delete scene "
              f"{sc} from SCENE_SKIPS in port/tools/battery.py.")

    r = run([sys.executable, os.path.join(root, "port", "tools", "linkage.py"),
             root], root)
    m = re.search(r"linked into walk_window\s*:\s*(\d+)\s*\(([\d.]+)%\)",
                  r.stdout)
    if not m:
        print("linkage: FAIL (no linked count in output)")
        return 1
    linked = int(m.group(1))
    print(f"linkage: {linked} ({m.group(2)}%)")
    if floor and linked < floor:
        print(f"linkage: FAIL, below the floor of {floor}")
        return 1

    r = run([sys.executable, os.path.join(root, "port", "tools", "ptr_audit.py")],
            root)
    m = re.search(r"^(\d+) carry code pointers AND no host TU names them",
                  r.stdout, re.M)
    if not m:
        print("ptr_audit: FAIL (no verdict line)")
        return 1
    if int(m.group(1)) != 0:
        print(f"ptr_audit: FAIL, {m.group(1)} unhosted code pointers")
        return 1
    print("ptr_audit: 0 unhosted code pointers")

    # gate.py tails this output, so the debt is restated where a tail will see
    # it rather than only next to the level it belongs to.
    if LEVEL_SKIPS:
        print("skips: " + ", ".join(
            f"level {lvl} without {LEVEL_SKIPS[lvl][0]} ({LEVEL_SKIPS[lvl][1]})"
            for lvl in sorted(LEVEL_SKIPS)))
    if SCENE_SKIPS:
        print("skips: " + ", ".join(
            f"scene {sc} with {SCENE_SKIPS[sc][0]} ({SCENE_SKIPS[sc][1]})"
            for sc in sorted(SCENE_SKIPS)))
    if retired:
        print("skips: RETIRED and removable -- " +
              ", ".join(f"level {lvl}" for lvl in retired))
    if scene_retired:
        print("skips: RETIRED and removable -- " +
              ", ".join(f"scene {sc}" for sc in scene_retired))
    # A blocked scene is debt too, and louder debt than a skip, so it is
    # restated on the same line gate.py tails rather than only next to the
    # scene it belongs to.
    if SCENE_BLOCKED:
        print("skips: " + ", ".join(
            f"scene {sc} BLOCKED, cannot run at all ({SCENE_BLOCKED[sc][0]})"
            for sc in sorted(SCENE_BLOCKED)))
    if scene_unblocked:
        print("skips: BLOCK RETIRED and removable -- " +
              ", ".join(f"scene {sc}" for sc in scene_unblocked))

    print("battery: ALL GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
