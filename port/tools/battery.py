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
  4c. default boot    walk_window.exe with NO SM64DS_* environment at all
                      beyond the harness minimum, which since the owner's
                      boot-to-title ruling is a title run rather than a level
                      one. Asserts rc 0, the title chain's own two probe lines
                      and a written frame. It replaces no level row: this is
                      the question "where does a launch with no arguments go",
                      which nothing here could ask while the answer was a level
  5. linkage          port/tools/linkage.py -- the linked count is printed and
                      compared against --linked-floor if given (a merge must
                      never lower it)
  6. ptr_audit        port/tools/ptr_audit.py -- unhosted code pointers must
                      stay at zero
  7. shipping config  THE OTHER CONFIGURATION. Steps 1-6 build and run exactly
                      one of the two configurations this tree has -- build/port,
                      the developer build -- and the binary that actually goes
                      out is PORT_ROM_CLEAN against the static CRT, which is a
                      different compile and a different link and breaks on its
                      own. This arm configures and builds it into its own
                      build/port-kit and fails the battery if configure, build
                      or link fails, then runs that exe's headless selftest ONCE
                      as a liveness check. --no-shipcfg opts out, loudly. The
                      long form is at THE SHIPPING CONFIGURATION below

    python port/tools/battery.py [repo-root] [--linked-floor N] [--skip-build]
                                 [--no-shipcfg]

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

THE SHIPPING CONFIGURATION IS A SECOND CONFIGURATION, AND IT BREAKS ALONE.

Steps 1 to 6 build and run build/port, the developer build. The binary that
actually goes out is not that one. It is PORT_ROM_CLEAN against the static CRT,
configured by tools/portable_kit/package_kit.ps1 into its own build/port-kit,
and for four days in August 2026 it did not compile at all while this battery
kept printing ALL GREEN.

port/release_hardening.txt is the write-up. b91d34ed7 added the camera's
optional mouse capture; three of its helpers call ClientToScreen_, which sat
inside an #ifndef PORT_ROM_CLEAN fence; the shipping build stopped compiling
with five C2039s in walk_window.cpp and nothing noticed. That note also names
the reason nothing noticed, and the reason was this file:

    nobody saw it for four days because nothing builds PORT_ROM_CLEAN
    routinely (the battery cannot: under that flag the emitters zero every ROM
    table and only walk_window/_hires link the loader, so every other smoke
    goes red).

THAT PARENTHESIS IS ABOUT RUNNING AND NOT ABOUT COMPILING, which is the whole
reason this arm can exist. Under PORT_ROM_CLEAN the smoke exes still BUILD
perfectly well; they fail when RUN, because their ROM tables are zeroed and
they do not link the loader that would fill them back in. So the arm builds the
shipping configuration and runs exactly one thing out of it: walk_window, which
is the target that ships. Nothing here ever runs a smoke exe out of
build/port-kit, and adding one would reintroduce the exact red that kept this
arm out of the battery for so long.

WHICH LEAVES ONE GAP, AND IT IS NAMED RATHER THAN GLOSSED. The quote above says
walk_window AND walk_window_hires link the loader, so walk_window is not the
only target that could run out of this directory. It is the only one this arm
BUILDS, because tools/portable_kit/package_kit.ps1 builds only walk_window and
that is the exe the kit ships; the ninja line here is `ninja -C <dir>
walk_window` and matching the packaging script was the deliberate choice. The
consequence is that a ROM_CLEAN break confined to walk_window_hires -- its own
target_compile_definitions block in port/CMakeLists.txt, or anything it links
that walk_window does not -- is caught by NOTHING: not by this arm, not by
package_kit.ps1, and not by the developer steps above, which never define the
flag at all. That is a smaller hole than the one this arm closes, and it is
still a hole. Adding walk_window_hires to the ninja line would close it at the
cost of a second link; whoever wants that should know they are also making this
arm stop matching what the kit actually builds.

ITS OWN BINARY DIRECTORY, NOT A RECONFIGURE OF build/port. The two
configurations disagree about a preprocessor symbol that reaches most of the
tree, so sharing one directory would throw away every object file on each
alternation and make the battery unusable. package_kit.ps1 already keeps them
apart for that reason and this arm uses the same directory it does, so the two
share a cache instead of fighting over one.

THE BMP HERE IS A LIVENESS CHECK, NOT A RASTER COMPARISON, and it is not
compared against the developer build's BMP either. THE SELFTEST BMP TRACKS THE
HOSTED-GLOBAL LAYOUT above is the long form: a comparison with meaning holds
the whole hosted layout constant, base and span both, and a different
configuration is a different compile and a different link, so it holds neither.
port/tools/kit_smoke.py states the same rule for the same reason over the
packaged exe. What is being asked of this run is only "did it reach the
renderer and write a frame". Anyone who later turns this into a picture compare
will be comparing two binaries that were never comparable.

DEFAULT ON, BECAUSE AN OFF-BY-DEFAULT ARM CANNOT CATCH A SILENT BREAK. That is
not a preference, it is the entire finding of the four days: the check that
exists and is not run is worth what the check that does not exist is worth.
--no-shipcfg opts a fast iteration loop out and prints a warning saying the
shipping configuration was not built, so a green carrying that line cannot be
read as the same green as one without it.

LAST, AND THAT ORDERING IS LOAD-BEARING. A tree-wide breakage -- a crashing
actor, a bad seat -- breaks both configurations, and the developer steps above
diagnose it far better than this one can, with the skip and blocked machinery
and a named level or scene. Running those first means every red this arm ever
prints arrives on a tree whose developer configuration is already green, which
makes it a CONFIGURATION-SPECIFIC red by construction. Moving this arm earlier
would buy a few minutes of latency and spend the only thing that makes its
failures easy to read.
"""

import os
import re
import subprocess
import sys
import time

SELFTEST_FRAMES = "300"
STEP_TIMEOUT = 600

# THE SHIPPING CONFIGURATION. Spelled exactly as
# tools/portable_kit/package_kit.ps1 spells it, because two checks that
# disagree about what the shipping build IS are worse than one -- the same
# rule kit_smoke.py's ROM_CLEAN_MARK is kept spelled the same way for.
SHIPCFG_BUILD = os.path.join("build", "port-kit")
# A FROM-SCRATCH configure and build of a second whole configuration, so this
# is not STEP_TIMEOUT. Once the directory exists ninja is incremental and the
# arm costs a link; the leash is sized for the first run in a fresh worktree.
SHIPCFG_BUILD_TIMEOUT = 5400
SHIPCFG_RUN_TIMEOUT = 600
SHIPCFG_BMP = "walk_window_selftest.bmp"

TABLE_OPEN = "static const PortLevelDesc port_level_table[] = {"
SCENE_TABLE_OPEN = "static const PortSceneClass port_scene_classes[] = {"

# A HOSTED SCENE WHOSE BLOCKER IS NOT THE SCENE BOOT, AND WHAT BLOCKS IT.
# Same contract as LEVEL_SKIPS above, one row down: scene id -> (env to set,
# who owns the fix, what it looks like). The env is applied on top of the scene
# selftest's own, and the bare run is re-probed on every pass so a skip cannot
# outlive its bug. Empty is the goal.
SCENE_SKIPS = {
    # SCENE 390 (0x186), dScMgFlower_c, RETIRED by run mg5 lane Y3D. The row
    # stood from lane FLW, which seated the class and found that it booted and
    # ticked but could not RENDER:
    #
    #   flw_render (slot 9) -> func_ov006_0212aacc -> func_ov006_020c3bf4
    #     -> ModelAnim::Virtual18 -> ModelAnim::Virtual10
    #   FAULT c0000005 accessing 00000000, eax=0
    #
    # The row named it a model/animation seam question and left NOT MEASURED
    # whether the ModelAnim at +0xd18 ever received a file. It had one; the
    # null was never the object. src/func_ov006_020c3bf4.cpp calls that virtual
    # through a LOCAL SHADOW CLASS, so its `f5(0)` is the ROM's vtable byte
    # +0x14 -- the sixth Itanium word, which on Model and ModelAnim alike is
    # Render(scale). MSVC folds the D1/D0 pair into one slot, so on the host
    # that byte is Virtual18(mat, scale), the one-argument call handed it
    # mat = 0, and Model::Virtual10 read address zero. The MATRIX was null, not
    # the model. Same defect port/unmatched/Door_Render.cpp records on the same
    # two classes; the seat is
    # port/unmatched/MgFlower_ModelRender_020c3bf4.cpp.
    #
    # Scene 390 now runs 300 frames bare under SM64DS_FAULTS_FATAL=1 with
    # nothing switched off, slot hits init 1, behavior 253, RENDER 300, and
    # SM64DS_PPU_AUDIT reports POLYGONS 878 on all 300 samples where it read 0
    # before. If the host copy is ever dropped, the scene's ordinary selftest
    # row goes red, which is a stronger signal than this entry was.
    #
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

    # SCENE 376 (0x178) WAS HERE AND IS RETIRED, run mg5 lane SMBSEAT. The row
    # had been CONVERTED ONCE and the conversion was a real seat: lane SMB
    # recorded that two of the class's nine vtable overrides had no source at
    # all, lane INTEG retired slot 0 InitResources and the aux ball-table
    # seeder, and the blocker moved one floor deeper to "a sub-object whose
    # ov006 vtable at 0x0213eca0 holds raw DS addresses (func_ov006_02114458,
    # not seated), so its first method call jumps into DS space".
    #
    # THAT LAST BLOCKER WAS NOT A DECOMP GAP AND THE ADDRESS IN IT WAS OFF BY
    # ONE TABLE, which is the part worth keeping. func_ov006_02114458 was
    # already in src/ here and always had been -- it was simply in no slice,
    # which reads identically to "no body" from a symbol search and is not the
    # same thing. And 0x0213eca0 is not a vtable: it is the WORD that holds
    # 0x02114458, the third slot of a table whose vptr is 0x0213ec98. An
    # Itanium record walk of overlay_0006.bin finds TWELVE such tables between
    # 0x0213ec90 and 0x0213eda0, one per cMgSmartball_ class, THREE slots each
    # -- not the five the config symbol span reads, because config names each
    # table's vptr rather than its head and the span therefore runs into the
    # next class's own offset-to-top and typeinfo. Filling five would have
    # overwritten a neighbour's RTTI with a host code address.
    #
    # SMBSEAT RELOCATED ALL TWELVE with the same address-keyed fill every scene
    # class in hal/scene_mg.cpp already used, seated the thirty-six bodies they
    # dispatch (thirty-five already in src/, two of those under wrong recovered
    # names; one, cMgSmartball_board_c::SaveSnapshot at 0x0210f564, brought
    # across from origin/main by address and re-verified with tools/match.py at
    # 2004/b56 against overlay_0006.bin in this worktree), and stood slot 9
    # Render on main's matched __thiscall member behind a cdecl forwarder --
    # the MgPachinko_Faces section 3 shape, because an alias cannot cross a
    # calling convention.
    #
    # THE COUNTERS ARE THE EVIDENCE AND THE RENDER SLOT IS ONE OF THEM. A bare
    # 300-frame run with SM64DS_FAULTS_FATAL=1 and nothing switched off:
    # rc 0, "300 frames of scene 376 (SCENE_MG_SMARTBALL), clean", ov006 slot
    # hits init 1 / behavior 253 / RENDER 300, 16941 slot entries across the
    # 36-slot table, dScMgSmartball_c traps entered 0 total, and 8310
    # dispatches through the twelve relocated three-slot tables. That last
    # number is the one that cannot be faked by a fill: a table can be
    # relocated and never entered, and those two look identical from outside.
    # Neither FILL INCOMPLETE line printed, so zero raw DS words are left in
    # the class table or in any of the twelve.
    #
    # SO THIS IS A RETIREMENT, NOT A SECOND CONVERSION: the scene has no
    # blocker left to name, and it runs as an ordinary scene selftest row
    # above. ONE new trap did arrive with the enlarged closure and it is NOT a
    # blocker -- func_ov006_02115248, named in symbols.txt at size 0x238 with
    # no delink block here and the identical gap on origin/main, reached from
    # cMgSmartball_kinoko_c's Update closure. It was not entered on any run in
    # this lane. It is a named, self-reporting trap in
    # port/unmatched/MgSmartball_Traps.cpp on the func_ov006_020e1854
    # precedent, its counter is in the atexit report, and an advisory is not a
    # battery row and must not become one.
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
    # Level 33's SNUFIT entry retired 2026-08-15: the w19 slot-5 fix and the
    # level-33 mount met at the wave's final merge, the bare run went 300
    # frames clean, and the battery printed SKIP RETIRED on its first run over
    # the combined tree. The entry shape is documented in this file's skip
    # section; new debts go here with the class, the owning lane, and the
    # evidence, never a raw fault offset.
    #
    # THIS ROW IS NOT A PORT DEFECT AND NO PORT LANE CAN CLOSE IT.
    # run rel0215 wave 2, lane w2-ov074 seated ov074's whole cast and took
    # level 45 to zero skipped -- census 5 spawned (5 classes), 0 skipped --
    # and GOOMBOSS then quarantines on its FIRST behaviour frame because the
    # DECOMP does not have the body its first state ticks:
    #
    #   func_ov074_021201f0 (0x1f0 bytes, ROM 0x021201f0) is Goomboss's state
    #   0 TICK. It has no delink block in config/arm9/overlays/ov074/
    #   delinks.txt and no src file anywhere in the tree -- one of three ov074
    #   function symbols in that state, and the only one the boot walk reaches.
    #   Its own relocations name Camera::SetPos, Camera::SetLookAt,
    #   Camera::SetFlag_3, Actor::ClosestPlayer, Sound::LoadAndSetMusic_Layer3,
    #   Message::PrepareTalk, Player::ShowMessage, Player::StartTalk and
    #   func_ov074_021203e4: the fight's opening cutscene, which ends by
    #   changing state. Nothing in ov074 sets an initial state, so the bss zero
    #   IS state 0 and the very first Goomboss::Behavior dispatches into it.
    #
    # THE EVIDENCE, both directions. The run logs live in the lane's
    # orchestration directory, NOT in this repo -- nothing under any
    # runs/rel0215 path is checked in, and an earlier revision of this comment
    # cited one as though it were repo-relative. Each run below is reproducible
    # from the command given, which is the part that has to survive:
    #   BARE, no FAULTS_FATAL, SM64DS_LEVEL=45, 300 and 600 frames: rc 0,
    #       census 5 spawned (5 classes) 0 skipped, and EXACTLY ONE quarantined
    #       actor, named -- "[quarantine] actor id 198 (GOOMBOSS) FROZEN, frame
    #       continues". Spawn, InitResources, the 31-slot fill, Render and the
    #       first Behavior up to the state dispatch all run clean; the shipped
    #       configuration degrades exactly as it is designed to.
    #   WITH THIS SKIP, FAULTS_FATAL=1, level 45, 300 and 600 frames: rc 0.
    #   THE CLASS ITSELF is proven on its OTHER half, and on a different level
    #       for the reason hal/actor_classes_ov074.cpp gives at length:
    #       SM64DS_LEVEL=13 SM64DS_SPAWN_ACTOR=198:0x1111 FAULTS_FATAL=1, 600
    #       frames -> rc 0, zero faults, zero quarantine lines. Level 45 cannot
    #       host that probe under FAULTS_FATAL and the skip would make it
    #       vacuous (SM64DS_SKIP_CLASS matches by substring).
    # The class stays REGISTERED, so the day func_ov074_021201f0 is matched the
    # bare re-probe below goes green and this row retires itself with no
    # further port work.
    45: ("GOOMBOSS", "the decomp (func_ov074_021201f0 has no matched body)",
         "quarantines on frame 0 of Goomboss::Behavior, in the state-0 tick "
         "the loud face in hal/actor_classes_ov074.cpp names"),
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


# CREATE_NO_WINDOW. Every exe this battery runs is a CONSOLE-subsystem binary
# (measured: walk_window.exe's PE subsystem is 3), so when the battery is
# launched by something that has no console of its own -- a lane agent, a
# scheduled task, an editor's run button -- Windows makes a NEW CONSOLE WINDOW
# for each child, and each of those windows appears on the desk and competes for
# the foreground. Roughly a hundred of them over one battery run.
#
# capture_output=True already pipes all three handles, so no child here has any
# use for a console. This flag is therefore a pure removal of windows nobody was
# reading. It is ignored on non-Windows, where the attribute does not exist.
#
# This is the CONSOLE half of the focus problem. The GAME WINDOW half is
# SM64DS_NO_FOCUS, set in selftest_env below.
NO_CONSOLE = getattr(subprocess, "CREATE_NO_WINDOW", 0)

# SW_SHOWMINNOACTIVE. CREATE_NO_WINDOW silences the console and SM64DS_NO_FOCUS
# stops the game window taking the keyboard, but the game window still APPEARED,
# and a battery paints roughly a hundred of them over the desk in seven minutes.
# walk_window honours the launcher's STARTUPINFO show request (host_show_mode(),
# the other half of the run mg12 no-focus work), so asking for
# SW_SHOWMINNOACTIVE here starts every game window minimized and unactivated:
# nothing appears, nothing flashes, and the selftest BMP is written from the
# port's own framebuffer so the picture cannot depend on the window being
# visible -- proven by md5 A/B before this shipped (port/no_focus.txt section 9).
if hasattr(subprocess, "STARTUPINFO"):
    SI_MIN = subprocess.STARTUPINFO()
    SI_MIN.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    SI_MIN.wShowWindow = 7  # SW_SHOWMINNOACTIVE
else:
    SI_MIN = None


def run(cmd, cwd, env=None, timeout=STEP_TIMEOUT):
    return subprocess.run(cmd, cwd=cwd, env=env, timeout=timeout,
                          capture_output=True, text=True,
                          creationflags=NO_CONSOLE, startupinfo=SI_MIN)


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
    # run mg15 lane MP1: the four-slot input route and its per-frame readout,
    # dropped in both env builders for the reason the four above it are.
    env.pop("SM64DS_COMMS_FANOUT", None)
    env.pop("SM64DS_COMMS_REPORT", None)
    # run mg16 lane MP2: the loopback carrier's four knobs, dropped for the
    # reason the two above are and then some. SM64DS_COMMS_ROLE makes the run
    # open a UDP socket and hunt for a peer; on a battery box that is a bind
    # that may fail, a wait bound that may expire, and either way a run whose
    # input path came off a wire instead of off the harness. The other three
    # are its arguments.
    env.pop("SM64DS_COMMS_ROLE", None)
    env.pop("SM64DS_COMMS_PORT", None)
    env.pop("SM64DS_COMMS_SLOT", None)
    env.pop("SM64DS_COMMS_INJECT", None)
    # SM64DS_RNG_MENU_FRAMES (run mg5, lane RNGSEED) pins the minigame RNG's
    # menu dwell and forces the seed, which moves data_0209d4b8 off the .bss
    # zero every draw in this tree is measured from. It is DETERMINISTIC -- the
    # same n is the same state -- so an inherited one could not make a row
    # flaky, only wrong in a stable way, which is the harder kind to notice.
    # Dropped in both env builders for the reason the four above it are.
    env.pop("SM64DS_RNG_MENU_FRAMES", None)
    # SM64DS_NO_FOCUS: the level selftests are the only step in this battery
    # that opens a game window, and a battery run opens one per mounted level.
    # Each of those took the foreground off whoever was using the machine, which
    # on the shared box is the owner, mid-sentence, once per level.
    #
    # The battery DECIDES this rather than inheriting it, the same way it decides
    # the four knobs popped above -- and for the stronger reason that this one
    # must be provably invisible to the artifact this step compares. The flag is
    # presentation OF THE WINDOW: WS_EX_NOACTIVATE plus SW_SHOWNOACTIVATE, no
    # pixel and no frame touched. Measured before it was wired here, level 1 at
    # SELFTEST_FRAMES, flag off vs on: walk_window_selftest.bmp byte-identical.
    #
    # A lane that needs the real foreground -- anything driving SM64DS_CLICK_TEST
    # -- does not go through this battery, and walk_window overrides the flag by
    # itself when that driver is armed.
    env["SM64DS_NO_FOCUS"] = "1"
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
    # SM64DS_MG_SCORE_TRACE JOINS THEM FOR A REASON THAT IS NOT PRINTING. Run
    # mg5 lane HISCORE added it, and most of what it does is read-only, but one
    # arm of it -- hal/scene_mg.cpp's pch_award_abi_check -- CALLS GAME CODE
    # (func_ov006_020fb7e0 and the award routine behind it) on a scratch object
    # during the pachinko fill. It parks data_ov004_020beb68 so it cannot reach
    # the live score, and it is off unless the variable is set, but an inherited
    # value would still run two ov006 bodies inside a row this step is
    # measuring. Same class as the pad and the click: not input, but not
    # nothing either.
    for k in ("SM64DS_LEVEL", "SM64DS_SKIP_CLASS", "SM64DS_SCENE_NO_RENDER",
              "SM64DS_SCENE_BMP", "SM64DS_SCENE_BMP_STACKED",
              "SM64DS_SCENE_TRACE", "SM64DS_SCENE_SLOT9",
              "SM64DS_SCENE_SUBLEVEL", "SM64DS_DUAL_SCREEN", "PORT_WATCHDOG",
              "SM64DS_SCENE_WINDOW", "SM64DS_CLICK_TEST", "SM64DS_PAD_TEST",
              "SM64DS_TOUCH_PROBE", "SM64DS_MG_SCORE_TRACE",
              # run mg15 lane MP1: SM64DS_COMMS_FANOUT routes TouchInfo[4] and
              # PadData[4] through the ROM's four comms records instead of the
              # port's direct writes, and SM64DS_COMMS_REPORT prints four lines
              # a frame. An inherited fanout would change the whole input path
              # of a row this step is measuring -- the same class as the five
              # above, and the more consequential one.
              "SM64DS_COMMS_FANOUT", "SM64DS_COMMS_REPORT",
              # run mg16 lane MP2: the loopback carrier's four. An inherited
              # SM64DS_COMMS_ROLE opens a socket and hunts for a peer on a box
              # whose whole job is to answer one question about one scene.
              "SM64DS_COMMS_ROLE", "SM64DS_COMMS_PORT",
              "SM64DS_COMMS_SLOT", "SM64DS_COMMS_INJECT",
              "SM64DS_RNG_MENU_FRAMES"):
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


def shipcfg_script(root, build):
    """Write the cmd that configures and builds the shipping configuration.

    Toolchain located the way port/build-port.cmd locates it, switched the way
    tools/portable_kit/package_kit.ps1 switches it, and the target is
    walk_window ALONE for the reason the doctrine block gives. A file rather
    than a command line because vcvars32.bat has to leave its environment
    behind for cmake in the same shell, which a single subprocess call cannot
    arrange.

    Returns (script path, None), or (None, the vcvars path that is not there).
    """
    pf = os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")
    vs = os.path.join(pf, "Microsoft Visual Studio", "2022", "BuildTools")
    vcvars = os.path.join(vs, "VC", "Auxiliary", "Build", "vcvars32.bat")
    cm = os.path.join(vs, "Common7", "IDE", "CommonExtensions", "Microsoft",
                      "CMake")
    if not os.path.isfile(vcvars):
        return None, vcvars
    # Under build/, which is gitignored, so the arm leaves nothing in the tree.
    # A FIXED name rather than a temporary one: it is overwritten every run,
    # and it is the honest answer to "what exactly did the arm build" for
    # anyone reading a red.
    path = os.path.join(root, "build", "shipcfg_build.cmd")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="ascii", newline="") as f:
        f.write(
            "@echo off\r\n"
            'call "%s" >nul || exit /b 1\r\n'
            'set "PATH=%s\\CMake\\bin;%s\\Ninja;%%PATH%%"\r\n'
            'cmake -S "%s" -B "%s" -G Ninja -DCMAKE_BUILD_TYPE=Release'
            ' -DPORT_ROM_CLEAN=ON -DCMAKE_MAKE_PROGRAM="%s\\Ninja\\ninja.exe"'
            ' -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded || exit /b 1\r\n'
            'ninja -C "%s" walk_window\r\n'
            % (vcvars, cm, cm, os.path.join(root, "port"), build, cm, build))
    return path, None


def shipcfg_missing_inputs(root):
    """Which of port/kit_assets.txt's required paths this tree does not have.

    THE LIST IS READ, NOT RESTATED HERE. port/kit_assets.txt is the tree's one
    answer to "what does a run of the shipping exe need", and it is the list
    kit_smoke.py already checks the packaged exe and the player's extractor
    against in both directions. A copy of it in this file would be a second
    answer, and it would go stale the first time the first one moved -- which
    is the failure port/kit_assets.txt itself was created to end.

    TWO of the eight paths on it the arm's own BUILD produces: romdata.bin and
    romdata.manifest are outputs of the romblob_ready_ww target that
    walk_window depends on under PORT_ROM_CLEAN (port/CMakeLists.txt). That
    target also writes romdata.recipe.tsv, but the recipe is NOT on this list
    and must not be counted as one of them: it is the shippable how -- offsets
    and hashes for the player's own extractor -- and the exe never opens it, so
    kit_assets.txt does not name it. Counting it here would report the tree one
    input better off than it is.

    Five more are the NitroFS catalog -- files.tsv, handles.tsv, nitrofs.tsv
    and the card's own FNT and FAT images -- and that is SETUP rather than a
    build product: it comes from `python tools/asset_catalog.py generate <rom>`
    and a fresh worktree has none of it. gate.py already names that same state
    for the developer selftests. The eighth is the unpacked filesystem's
    sentinel, extracted/dsd/files/data/sound_data.sdat.

    WHICH OF THOSE CAN ACTUALLY REACH THE NOT-RUN BRANCH IN A FULL BATTERY IS
    NARROWER THAN THE LIST LOOKS, and it is worth knowing before anyone reads
    a missing-input partial as the common case. hal/fs_names.cpp's asset_root()
    resolves the catalog in BOTH configurations, so the developer selftests
    need those same five files: measured with nitrofs.tsv moved aside, level 1
    exits 2 with "FATAL: NitroFS table index missing" and the battery is red at
    step 3, hundreds of lines before this arm. The two romdata files cannot
    reach it either, because the arm's own build regenerates them before this
    function is called. So in practice the branch that fires under a full
    battery is the kit_assets.txt one above, and this branch is a guard for a
    tree assembled some other way rather than a routine outcome.
    """
    path = os.path.join(root, "port", "kit_assets.txt")
    if not os.path.isfile(path):
        # No list is not "nothing is required", so do not answer as if it were.
        return None
    missing = []
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if not os.path.exists(os.path.join(root, *line.split("/"))):
                missing.append(line)
    return missing


def default_boot_env():
    """THE ENVIRONMENT A PLAYER GETS, plus the harness minimum.

    An ALLOWLIST, and it has to be. Every other env builder in this file starts
    from os.environ and pops the knobs it knows about, which is right when the
    row names its own destination -- an inherited SM64DS_SKIP_CLASS cannot make
    a level row pass, because the row still says SM64DS_LEVEL=<n>. This row
    names NOTHING, and that is the whole point of it: what it measures is what
    the game does when nobody has said anything. A pop-list would leave exactly
    the two names that decide this row's answer -- an inherited SM64DS_LEVEL or
    SM64DS_SCENE turns "prove the default boots the title" into "prove a level
    boots", silently and greenly. So this builds the environment from nothing
    and adds four names.
    """
    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    # SM64DS_ASSET_ROOT IS PUT BACK, and it is the one exception. It is a
    # LOCATION, not a behaviour knob: it says where the game's data is, the
    # launcher passes it on every real launch, and the other arms in this file
    # inherit it because they build from os.environ without popping it. Dropping
    # it here would make this the only row that resolves assets differently from
    # the forty-six above it, which is a divergence that would surface as a
    # confusing red rather than as the thing this row is asking about. Unset in
    # the caller's environment it stays unset, which is the developer build's
    # normal state -- hal/fs_names.cpp then resolves against the tree.
    if "SM64DS_ASSET_ROOT" in os.environ:
        env["SM64DS_ASSET_ROOT"] = os.environ["SM64DS_ASSET_ROOT"]
    # The rest are what the harness owes every run in this file: a bounded
    # headless run (WINDOW_SELFTEST also forces the scene path headless -- see
    # port_scene_want_window), faults that end the process instead of being
    # quarantined and counted, and the quiet rule.
    env["SM64DS_WINDOW_SELFTEST"] = SELFTEST_FRAMES
    env["SM64DS_FAULTS_FATAL"] = "1"
    env["SM64DS_NO_FOCUS"] = "1"
    env["SM64DS_VOLUME"] = "0"
    return env


def default_boot_arm(build):
    """Arm 4c: THE BARE LAUNCH REACHES THE TITLE.

    The owner ruled that the game boots to its own first screen, so the run
    with no environment at all stopped being a level boot and became the title
    chain. Nothing in this battery covered that before, because before this
    lane there was nothing to cover: a bare run was a level run and the level
    rows measured it forty-six times over.

    WHAT IT ASSERTS, and every line of it is the chain's own rather than this
    file's opinion of it:

      rc 0 under SM64DS_FAULTS_FATAL=1   the run finished and nothing faulted.
                                         Read the caveat in section 9 of
                                         port/ov007_seat.txt before trusting rc
                                         alone -- which is why the two probe
                                         lines below are also required.
      "[scene] 1 = "                     hal/scene_boot.cpp's own boot line,
                                         printed by port_scene_boot after the
                                         ROM's spawn spine returned a scene
                                         object. It is how the run says the
                                         TITLE is what came up rather than
                                         something that merely did not crash.
      "frames of scene 1"                port_scene_finish's census, which the
                                         same section 9 names as the line a run
                                         must END with before any reading of it
                                         counts. A truncated run and a clean
                                         run that never got there otherwise
                                         look identical.
      walk_window_selftest.bmp           the run reached the renderer. Liveness
                                         only, deliberately -- the pixels are
                                         not compared, for the reason the
                                         shipcfg arm gives at length. Deleted
                                         first so a file an earlier row left
                                         behind cannot answer for this one.

    THIS ARM IS NOT A WEAKENED LEVEL ROW. It replaces nothing: all forty-six
    level rows still run, still name their level and still compare their BMP.
    This is a new question -- "where does a launch with no arguments go" --
    that the tree could not previously ask.
    """
    exe = os.path.join(build, "walk_window.exe")
    bmp = os.path.join(build, "walk_window_selftest.bmp")
    if os.path.exists(bmp):
        os.remove(bmp)
    try:
        r = run([exe], build, env=default_boot_env())
    except subprocess.TimeoutExpired:
        print(f"default boot: FAIL, a bare launch did not finish "
              f"{SELFTEST_FRAMES} frames inside {STEP_TIMEOUT}s")
        return False
    out = (r.stdout or "") + (r.stderr or "")
    if r.returncode:
        print(f"default boot: FAIL rc={r.returncode} -- a launch with no "
              f"SM64DS_* environment at all did not complete "
              f"{SELFTEST_FRAMES} frames.")
        print(out[-1500:])
        return False
    for probe, why in (("[scene] 1 = ",
                        "the title scene never came up (no boot line from "
                        "port_scene_boot)"),
                       ("frames of scene 1",
                        "the run never reached port_scene_finish's census, so "
                        "it did not finish the title run it started")):
        if probe not in out:
            print(f"default boot: FAIL rc=0 but {why}. Expected {probe!r} in "
                  f"the output.")
            print(out[-1500:])
            return False
    if not os.path.isfile(bmp):
        print("default boot: FAIL, rc=0 and the title ran but no "
              "walk_window_selftest.bmp was written -- the run never reached "
              "the renderer.")
        return False
    print(f"default boot: ok -- a bare launch reaches the TITLE, "
          f"{SELFTEST_FRAMES} frames clean, and writes its frame "
          f"({os.path.getsize(bmp):,} bytes, liveness only)")
    return True


def shipcfg_env(root):
    """The shipping exe's environment: an ALLOWLIST, not a pop-list.

    selftest_env and scene_env above drop knobs one at a time, and their own
    comments record what that costs -- a knob per lane, each one added after
    somebody noticed an inherited value moving a row that was supposed to be
    measuring something else. kit_smoke.py's launch_env answers the same
    question the other way round for the packaged exe: keep nothing that starts
    with SM64DS_, then set exactly what the run needs. A NEW arm can start from
    that shape at no cost, and unlike a pop-list it cannot go stale.
    """
    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    # Not optional the way it is for the developer build: this configuration
    # has NO PORT_REPO_ROOT fallback (hal/asset_root_refuse.cpp), so without
    # this the exe refuses to start, on purpose.
    env["SM64DS_ASSET_ROOT"] = root
    env["SM64DS_WINDOW_SELFTEST"] = SELFTEST_FRAMES
    # For the reason the level selftests set it: without it a quarantined
    # access violation freezes one actor, the run keeps ticking and exits 0. A
    # liveness check that accepts that is not a liveness check.
    env["SM64DS_FAULTS_FATAL"] = "1"
    # A modal box would hang the battery until somebody clicked it.
    # SM64DS_WINDOW_SELFTEST already implies this (tests/walk_window.cpp), so
    # it is the second of two locks rather than the only one.
    env["SM64DS_NO_DIALOG"] = "1"
    # The window half of the quiet-desk plumbing, as everywhere else in this
    # file. The console half (CREATE_NO_WINDOW) and the show-mode half
    # (SW_SHOWMINNOACTIVE) are applied by run() itself, so this child is as
    # silent as every other one the battery spawns.
    env["SM64DS_NO_FOCUS"] = "1"
    return env


def shipcfg_arm(root):
    """Build the shipping configuration, and prove the exe it makes still runs.

    Returns (ok, carry): ok is False if the battery must stop, and carry is
    None or the text of a `skips:` line main() must restate at the end.

    THE CARRY IS NOT DECORATION. This arm has a PARTIAL outcome -- the build
    half ran and the run half did not, because the inputs for it are setup
    rather than build products -- and a partial that returns green with no
    standing line is indistinguishable in a gate tail from a full green.
    gate.py keeps lines matching ^skips: whatever the return code was, and
    that is the only channel a partial has. Every NOT RUN path below therefore
    hands one back, the same way --no-shipcfg does.

    The long form -- why it exists, why its own directory, why walk_window
    only and what that leaves uncovered, and why the BMP is not compared
    against anything -- is THE SHIPPING CONFIGURATION in this file's docstring.
    """
    build = os.path.join(root, SHIPCFG_BUILD)
    script, no_vcvars = shipcfg_script(root, build)
    if script is None:
        print(f"shipcfg build: FAIL, no vcvars32.bat at {no_vcvars}")
        return False, None

    t0 = time.time()
    try:
        r = run(["cmd", "/c", script], root, timeout=SHIPCFG_BUILD_TIMEOUT)
    except subprocess.TimeoutExpired:
        print(f"shipcfg build: FAIL, configure and build did not finish "
              f"inside {SHIPCFG_BUILD_TIMEOUT}s")
        return False, None
    secs = time.time() - t0
    if r.returncode:
        print(f"shipcfg build: FAIL rc={r.returncode} after {secs:.0f}s -- the "
              f"SHIPPING configuration (PORT_ROM_CLEAN, static CRT) does not "
              f"build. Every step above this line is green, so this is a break "
              f"in the configuration that ships and in no other.")
        print((r.stdout or "")[-3000:])
        print((r.stderr or "")[-3000:])
        return False, None
    exe = os.path.join(build, "walk_window.exe")
    if not os.path.isfile(exe):
        # ninja can report success over a target that produced nothing if the
        # link line is wrong in the right way, so ask the filesystem.
        print(f"shipcfg build: FAIL, rc=0 but there is no walk_window.exe at "
              f"{exe}")
        return False, None
    print(f"shipcfg build: ok, walk_window.exe linked in {SHIPCFG_BUILD} "
          f"(PORT_ROM_CLEAN, static CRT, {secs:.0f}s)")

    missing = shipcfg_missing_inputs(root)
    if missing is None:
        print("shipcfg selftest: NOT RUN, port/kit_assets.txt is not in this "
              "tree, so what the run half needs cannot be established. The "
              "BUILD half above ran.")
        return True, ("the shipping exe was BUILT but NOT RUN "
                      "(port/kit_assets.txt is not in this tree)")
    if missing:
        shown = ", ".join(missing[:3]) + (", ..." if len(missing) > 3 else "")
        print(f"shipcfg selftest: NOT RUN, this tree is missing {len(missing)} "
              f"of the inputs port/kit_assets.txt requires ({shown}). The "
              f"BUILD half above ran, and it is the half that catches a "
              f"shipping-config compile break; `python "
              f"tools/asset_catalog.py generate <rom>` gets the run half too.")
        return True, (f"the shipping exe was BUILT but NOT RUN ({len(missing)} "
                      f"of port/kit_assets.txt's inputs are missing: {shown})")

    bmp = os.path.join(build, SHIPCFG_BMP)
    if os.path.exists(bmp):
        # Otherwise "a BMP is there" is satisfied by one an earlier run left,
        # and the check passes over an exe that wrote nothing at all.
        os.remove(bmp)
    t0 = time.time()
    try:
        r = run([exe], build, env=shipcfg_env(root),
                timeout=SHIPCFG_RUN_TIMEOUT)
    except subprocess.TimeoutExpired:
        print(f"shipcfg selftest: FAIL, the shipping exe did not finish "
              f"{SELFTEST_FRAMES} frames inside {SHIPCFG_RUN_TIMEOUT}s")
        return False, None
    secs = time.time() - t0
    if r.returncode:
        print(f"shipcfg selftest: FAIL rc={r.returncode} -- the shipping "
              f"configuration builds, but the exe it makes cannot complete a "
              f"headless selftest.")
        print(((r.stdout or "") + (r.stderr or ""))[-1500:])
        return False, None
    if not os.path.isfile(bmp):
        print(f"shipcfg selftest: FAIL, rc=0 but no {SHIPCFG_BMP} was written "
              f"-- the run exited cleanly without ever reaching the renderer.")
        return False, None
    print(f"shipcfg selftest: ok, rc=0 and {SHIPCFG_BMP} written "
          f"({os.path.getsize(bmp):,} bytes, {secs:.0f}s) -- LIVENESS ONLY, "
          f"not a raster comparison and not compared against the developer "
          f"build's BMP (see THE SHIPPING CONFIGURATION above)")
    return True, None


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
    # SEPARATE FROM --skip-build ON PURPOSE. --skip-build means "reuse what is
    # in build/port"; it says nothing about the other configuration, which
    # lives in another directory and has its own ninja cache. A lane that wants
    # both fast paths asks for both.
    skip_shipcfg = "--no-shipcfg" in args
    if skip_shipcfg:
        args.remove("--no-shipcfg")
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

    if not default_boot_arm(build):
        return 1

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

    # THE SHIPPING CONFIGURATION, LAST. Everything above tests build/port, the
    # developer build; this is the other one, the one that goes out. It is last
    # so that a tree-wide breakage is diagnosed by the steps above -- which name
    # a level or a scene and carry the skip machinery -- and every red this arm
    # prints therefore lands on a tree whose developer configuration is already
    # green. That makes its failures configuration-specific by construction.
    shipcfg_carry = None
    if skip_shipcfg:
        print("shipcfg: SKIPPED by --no-shipcfg -- the SHIPPING configuration "
              "(PORT_ROM_CLEAN, static CRT) was NOT built or run by this "
              "battery, so a break in it is not covered by this green.")
        shipcfg_carry = "the SHIPPING configuration was NOT built (--no-shipcfg)"
    else:
        ok, shipcfg_carry = shipcfg_arm(root)
        if not ok:
            return 1

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
    # An opted-out arm is debt like any other skip, and so is a PARTIAL one --
    # built but not run, because the run half's inputs are setup rather than
    # build products. Both are restated on the line gate.py's BATTERY_CARRY
    # tails. A green carrying one of these lines and a green without one are
    # not the same green, and a reader of a tail should not have to know the
    # arm's internals to see the difference.
    if shipcfg_carry:
        print("skips: " + shipcfg_carry)

    print("battery: ALL GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
