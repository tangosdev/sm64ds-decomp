// ---------------------------------------------------------------------------
// rom_frame.cpp -- THE ROM'S FRAME STATE, HOSTED, AND THE ONE SEAM THE TEST
// HARNESS READS ITS FRAME NUMBER THROUGH.
//
// Run link100, boot plan rung R3a (lane R3A). Worth ZERO linked TUs by design.
// Its whole value is that after it, the harness no longer depends on the host
// frame loop's own local counter, so the frame can later be handed to the ROM's
// own loop (rungs R3b to R3d, worth +72) by changing ONE function here instead
// of twenty-one readers and two hundred traces in tests/walk_window.cpp.
//
// THE ROM'S FRAME STATE IS TWO WORDS, and src/func_020197b8.c -- the ROM's game
// loop, arm9 0x020197b8 -- is what writes both:
//
//     data_0209d50c = 5; func_02019404();     phase 5
//     data_0209d50c = 6;                      <- PHASE 6, first statement
//     data_020a0db0 = data_020a0db0 + 1;      <- PHASE 6, second statement
//
//   data_0209d50c  0x0209d50c, .bss (config/arm9/symbols.txt:4628). THE PHASE
//                  ID: which of the loop's twelve phases the frame is in. Until
//                  this file NOTHING IN THE PORT HOSTED IT and nothing read it:
//                  its only ROM writer is func_020197b8, which is not linked
//                  (port/CMakeLists.txt renames it to port_rom_loop_seam for
//                  src/main.c's sake). It is defined here, and the host loop
//                  writes the one phase it actually runs at the point it runs
//                  it. R3b adds the other eleven as it moves the loop's duties
//                  into the ROM's own seams.
//
//   data_020a0db0  0x020a0db0, .bss (config/arm9/symbols.txt:4924). THE FRAME
//                  CLOCK, hosted as int[8] in hal/auto_bss.cpp:251 and stepped
//                  by port_frame_clock_tick() in hal/fader_wipes.cpp. NOT
//                  TOUCHED HERE, and read only to measure. Read that file's
//                  banner before assuming this one could have used it as the
//                  harness's frame number: thirteen linked readers hang their
//                  blink off it, this port steps it ONE PHASE EARLY and GATED
//                  ON THE GAME TICK, and hal/scene_boot.cpp's port_scene_tick
//                  steps the same word, so on a title-entry fall-through the
//                  scene has already stepped it a few thousand times before
//                  main's level loop starts its own frame at 0.
//
// SO THE HARNESS'S FRAME NUMBER IS THE PHASE-6 STEP COUNT, kept here, stepped
// once per frame at the host loop's frame boundary by port_rom_frame_phase6(),
// which is the ROM's phase-6 body run where the ROM's loop runs it: after the
// frame's work, before the frame ends. Under rung R3d that step becomes
// func_020197b8's own line and this file stops writing it. Nothing else about
// the frame moves in this rung: no pacing, no input, no present, and the blink
// clock's own call site is exactly where it was.
//
// THE CROSS-CHECK IS THE POINT OF THE RUNG, NOT A DEBUG AID. Every converted
// reader passes the host loop's own `frame` in beside its request, and this
// file refuses to return a number that disagrees with it: it prints the frame,
// both counters and the reader's name, and leaves with code 90. It is ON unless
// SM64DS_FRAME_CROSSCHECK=0, so a battery row that goes green has PROVED the
// two counters agreed on every frame of it, rather than merely not crashing.
//
// AND THE ONE DEVIATION IS MEASURED RATHER THAN ASSUMED. Every frame this file
// reconciles its own step count against data_020a0db0's, and the report line at
// the end of a run says how many phase-6 steps this port's pause suppressed
// from the blink clock (the frame a level change re-seats, and every frame the
// debug menu holds the world still -- walk_window.cpp:10631 and :11084). The
// ROM's loop has no pause, so that number is exactly the gap R3d closes.
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"

/* THE ROM'S PHASE ID, hosted for the first time, INSIDE the .dsstate bracket.
   It is a hosted DS BSS global at 0x0209d50c and tools/dsstate_guard.py refuses
   the link if one of those lands outside the span a save state captures -- it
   refused this file's first draft, which put the word in plain .bss on the
   argument that a within-frame phase id is a transient. The guard is right and
   the argument was wrong: a save state taken mid-frame has a phase, and a
   restore that comes back with a different one has changed the ROM's state. */
DSSTATE_BEGIN
extern "C" {
int data_0209d50c;
}
DSSTATE_END

extern "C" {

/* the blink clock, read here only to measure the gap. hal/auto_bss.cpp owns it
   and hal/fader_wipes.cpp is the only writer. */
extern int data_020a0db0[8];

static int         g_frame;          /* phase-6 steps since this loop began */
static int         g_blink_base;     /* data_020a0db0[0] when it began */
static int         g_blink_missed;   /* steps this port's pause suppressed */
static int         g_checks;         /* cross-check calls */
static int         g_rewinds;        /* rollback re-anchors */
static const char *g_loop = "none";
static int         g_on = -1;        /* the cross-check gate, resolved once */

static int crosscheck_on(void)
{
    if (g_on < 0) {
        const char *e = std::getenv("SM64DS_FRAME_CROSSCHECK");
        g_on = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
    }
    return g_on;
}

/* ---- THE ROM-LOOP KNOB, AND WHY IT LIVES HERE (run link100, lane R3G) -----

   SM64DS_ROM_LOOP is rung E1's knob: with it on, phase 7 of the frame is the
   ROM's own sleep and IRQ::VBlankHandler's wake is what ends the frame. Lane
   R3E wrote the reader into tests/walk_window.cpp, beside the level loop's
   phase-7 point, because that loop was the only reader there could be.

   RUNG G1 GIVES hal/scene_boot.cpp's port_scene_run THE SAME PHASE 7, and that
   file is compiled into smoke_player, which does not compile
   tests/walk_window.cpp at all -- so a reader over there would have answered
   with an unresolved external at link time. This is the same move rung R3b step
   B0 made with the frame state itself, for the identical reason and at the
   identical cost: rom_frame.cpp is host code with no matched TU behind it and
   it is already enrolled in all three targets that compile its callers, so
   moving the accessor here changes no count.

   NOTHING ABOUT THE KNOB CHANGES WITH THE FILE IT LIVES IN. The spelling is
   still the BOOTR1 pattern, the banner is still printed once on the first read
   -- which is what makes a run's log say which loop is running -- and
   tests/walk_window.cpp's five readers still read exactly this function.

   RUNG H2, THE FLIP (run link100, lane R3H). The default is now ON, and
   SM64DS_ROM_LOOP=0 selects the host loop. Nothing else about this function
   moves: same name, same spelling of the variable, same single read, same
   banner point. The condition is inverted and that is the whole change, which
   is what makes the flip revertable on its own.

   WHAT EARNED IT. Every level row has run on the ROM's phase 7 since rung E1
   went green (lane R3F: levels 1, 5, 9 and 13 at 300 frames, the shipped path's
   own end position to the unit), all 37 scene rows since rung G1 (lane R3G),
   and the last scene that faulted -- scene 6, the VS menu -- since rung H1
   above. A whole battery with the knob exported came back ALL GREEN on all 51
   level rows and all 37 scene rows before this line changed.

   THE HOST LOOP IS KEPT WHOLE FOR ONE RELEASE. It is not deleted, not
   #ifdef'd out and not left to rot untested: SM64DS_ROM_LOOP=0 runs it, and
   the lane that made this change ran a second whole battery with exactly that
   exported. If the release finds nothing wrong with the ROM's loop, the lane
   after this one is the one that removes the host loop; until then the
   fallback is one environment variable away.

   THE CROSS-CHECK IS WHAT MAKES THE CLAIM CHECKABLE. It stays ON: every
   converted reader still passes the host loop's own counter in beside its
   request and this file still refuses to answer with a number that disagrees.
   A battery row that goes green under the flipped default has proved the two
   frame accounts agreed on every frame of it. */
int port_rom_loop_enabled(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_ROM_LOOP");
        v = (e && e[0] == '0' && e[1] == '\0') ? 0 : 1;
        if (v)
            std::fprintf(stderr, "[r3e] the ROM's own frame loop is the default "
                    "(SM64DS_ROM_LOOP=0 selects the host loop, kept whole for "
                    "one release): phase 7 is the ROM's own "
                    "sleep on every frame (func_0201a4bc -> OS_SleepThread"
                    "(data_0209d500) -> the idle thread -> the wait), so "
                    "IRQ::VBlankHandler's wake branch is what ends the frame "
                    "and func_02019144 is what commits the display\n");
        else
            std::fprintf(stderr, "[r3e] SM64DS_ROM_LOOP=0: the HOST frame loop, "
                    "which is the fallback rung H2 kept whole for one release. "
                    "The ROM's own loop is the default and this run is not "
                    "using it\n");
    }
    return v;
}

/* A FRAME LOOP IS STARTING. Both of tests/walk_window.cpp's loops call this
   with their own name, so a run that plays a scene and then falls through into
   a level reports two frame accounts rather than one blurred one. */
void port_rom_frame_begin(const char *loop)
{
    g_loop = loop ? loop : "loop";
    g_frame = 0;
    g_blink_base = data_020a0db0[0];
    g_blink_missed = 0;
    g_checks = 0;
    g_rewinds = 0;
    std::fprintf(stderr, "[rom-frame] %s: the harness reads the ROM's frame "
                         "state from here (phase id data_0209d50c, phase-6 step "
                         "count); cross-check against the host counter is %s\n",
                 g_loop, crosscheck_on() ? "ON" : "OFF");
}

/* THE ROM'S PHASE-6 BODY, at the host loop's frame boundary.
   func_020197b8.c:49-50 is `data_0209d50c = 6; data_020a0db0 += 1;`. The first
   statement is this port's for the first time. The second is already this
   port's, at its own point and under its own tick gate in hal/fader_wipes.cpp,
   so it is NOT repeated here -- it is reconciled instead: `blink` is how many
   times the clock stepped since this loop began, and the difference is what the
   pause suppressed. */
void port_rom_frame_phase6(void)
{
    data_0209d50c = 6;
    ++g_frame;
    {
        const int blink = data_020a0db0[0] - g_blink_base;
        g_blink_missed = g_frame - blink;
    }
}

int port_rom_frame(void)
{
    return g_frame;
}

/* ROLLBACK RE-ANCHOR. hal/rollback.cpp's rb_frame_end / rb_probe_frame_end take
   `&frame` and may move it BACKWARDS to re-run a contradicted round. The ROM's
   phase-6 step counts frames EXECUTED and cannot go back, so the one place the
   host counter legitimately moves on its own says so here rather than tripping
   the cross-check. Inert unless port::comms_rb_mode() is on. */
void port_rom_frame_rewind(int to)
{
    std::fprintf(stderr, "[rom-frame] %s: rollback moved the host counter %d "
                         "-> %d; re-anchoring the ROM frame number with it\n",
                 g_loop, g_frame, to);
    ++g_rewinds;
    g_frame = to;
    /* g_blink_base is deliberately left where it was: the blink clock keeps
       counting frames EXECUTED, so after a re-anchor the reconciliation below
       reports a NEGATIVE suppression, which is the honest reading -- the clock
       stepped more times than the run has logical frames. */
}

/* THE ONE ACCESSOR EVERY CONVERTED READER GOES THROUGH. Returns the ROM's frame
   number; refuses to return one that disagrees with the host loop's counter. */
int port_rom_frame_checked(int host, const char *reader)
{
    if (crosscheck_on()) {
        ++g_checks;
        if (g_frame != host) {
            std::fprintf(stderr,
                         "[frame-crosscheck] MISMATCH in %s at reader '%s': the "
                         "ROM's frame state says %d, the host loop's counter "
                         "says %d (blink clock %d, suppressed %d, rewinds %d). "
                         "The frame boundary moved; this is the finding.\n",
                         g_loop, reader ? reader : "?", g_frame, host,
                         data_020a0db0[0] - g_blink_base, g_blink_missed,
                         g_rewinds);
            std::fflush(stderr);
            std::fflush(stdout);
            std::exit(90);
        }
    }
    return g_frame;
}

/* ONE LINE PER RUN, so a battery row's log can be grepped for it and counted.
   Prints whether the cross-check was on, how many reads it verified, and the
   measured gap between the ROM's phase-6 step count and the blink clock. */
void port_rom_frame_report(void)
{
    std::fprintf(stderr,
                 "[frame-crosscheck] %s: %s -- %d frames, %d reads verified, 0 "
                 "disagreements; blink clock stepped %d of them, %d suppressed "
                 "by the port's pause, %d rollback re-anchor(s)\n",
                 g_loop, crosscheck_on() ? "ON" : "OFF", g_frame, g_checks,
                 data_020a0db0[0] - g_blink_base, g_blink_missed, g_rewinds);
    std::fflush(stderr);
}

}  /* extern "C" */
