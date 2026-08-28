/* THE TITLE-TO-ADVENTURE BRIDGE (run lvled, lane title-adventure).
 *
 * Picking a save file on the title screen starts the game. In the port it did
 * not: the title ran, the file select came up, a slot could be picked, the
 * title tore itself down -- and then nothing happened, for as many frames as
 * the run had left. That is the fade-to-white-and-stop the owner reported.
 *
 * ---- WHAT WAS ALREADY TRUE, AND IT IS NEARLY ALL OF IT --------------------
 *
 * Every step of the handoff is the ROM's own matched code and every step of it
 * already runs. src/func_ov007_020cc2cc.c is dScDSMT_c::Behavior, and its
 * save-file branch is four statements:
 *
 *     if ((unsigned)(result - 3) <= 2) {          // 3, 4, 5 = files A, B, C
 *         int idx = 0;
 *         if (result == 4) idx = 1; else if (result == 5) idx = 2;
 *         func_02013c84(idx, data_0209b33c + idx * 0x44, -1, &data_0209caa0);
 *         StartFile(1, 0);
 *     }
 *
 * func_02013c84 moves the chosen file's 0x44-byte record and leaves the slot
 * index in data_0209caa0[0x328]. src/StartFile.c then does the rest:
 *
 *     LoadLevelNoReturn(1, 0, 1, 0);      -> data_02092110 = 1  (castle grounds)
 *                                            data_0209f268 = 0  (entrance 0)
 *                                            data_0209f1f0 = 1  (star filter)
 *     SetPlayerGlobals();
 *     data_02092128[0] = data_0209caa0[0x41];   the save's character
 *     SetNumPlayers(1);
 *     Scene::StartSceneFade(3, 0, 0x7fff);      -> data_02092664 = 3
 *
 * So by the time the title is gone the game has ALREADY said, in its own
 * words, which level to boot, at which entrance, with which character. Scene 3
 * is the Stage -- on the DS the level boot IS Stage::InitResources -- and the
 * port boots a Stage through port_stage_a_boot on the LEVEL path, which is why
 * the scene path's spawner declines the id honestly: hal/actor_registry.cpp's
 * pre-spawn hook returns 3 for an unregistered id (progress byte 2) and
 * data_020a4bb8[3] is null. Both refusals are correct and neither is a bug.
 *
 * THIS FILE ADDS NO POLICY AND FABRICATES NO STATE. It watches for the ROM's
 * own handoff to complete, stops the scene loop, hands the ROM's own level
 * request to the level boot, and gets out of the way. Every value that reaches
 * the adventure was written by a matched TU.
 *
 * ---- WHY IN-PROCESS AND NOT A RELAUNCH ------------------------------------
 *
 * The port's other scene-to-level crossing, the debug menu's MENU_LEVEL row,
 * is port_menu_relaunch: a new process with SM64DS_LEVEL set. That was the
 * obvious candidate and it is the wrong one HERE, for a reason that is about
 * fidelity rather than convenience.
 *
 * A relaunch would carry the destination and LOSE THE SAVE. data_0209caa0 is
 * process memory; a child process re-runs the title's own default-fill and the
 * picked file's record, its character byte and its slot index are gone. To put
 * them back the port would have to carry a slot number through the environment
 * and re-apply it by hand -- port code fabricating save state the ROM already
 * built, which is exactly what port/tools/linkage.py's north star exists to
 * refuse. Staying in the process keeps the ROM's own writes live and lets the
 * level boot read them, which is what the DS does.
 *
 * The seam is also already where it needs to be. tests/walk_window.cpp's own
 * comment at the SM64DS_SCENE handover says everything above that line is
 * bring-up both modes need and everything below reads the Player the entrance
 * spawned. The scene run returns; the level bring-up below is self-contained.
 * Falling through is the shape that file already describes.
 *
 * ---- THE STOP CONDITION IS THE ROM'S, NOT A FRAME COUNT --------------------
 *
 * Two words, both the ROM's, and no magic numbers:
 *
 *     data_02092664 == 3     the Stage is what was asked for
 *     data_02092660 == 0     the outgoing scene has finished tearing down
 *
 * The second is Scene::AfterCleanupResources' own clear, and it is the same
 * pair the arc-3 carrier in hal/scene_boot.cpp gates on -- so this stops on
 * exactly the frame the ROM's spawner would have been asked, and would have
 * declined. A run therefore carries at most a decline line or two before the
 * bridge takes over, and those lines are left in on purpose: they are the
 * record that the ROM refused and the port did not pretend otherwise.
 *
 * A LEVEL MUST BE PENDING TOO. If scene 3 is asked for with data_02092110 < 0
 * the bridge REFUSES in words rather than booting whatever SM64DS_LEVEL
 * happens to say, because a default that looks like a successful entry is the
 * worst answer available. A refusal ends the run cleanly with its reason
 * printed; it never leaves a torn-down title ticking, which is the half-state
 * this lane was told not to ship.
 *
 * ---- WHAT IS OPT-IN, AND WHAT IS UNCHANGED --------------------------------
 *
 * SM64DS_TITLE_ENTRY=1 arms all of it, and only for SM64DS_SCENE=1. With the
 * flag unset every function here answers 0 before reading anything else, the
 * scene path is the path it always was, and the default boot -- no
 * environment at all -- still goes straight to the level, never reaching this
 * file. Nothing here is on a path the shipped default takes.
 */

#include <cstdio>
#include <cstdlib>

extern "C" {

/* hal/scene_boot.cpp: the scene run, as its three composable halves. The
   headless port_scene_run is begin/tick/finish and nothing else, and
   tests/walk_window.cpp's scene_window_run drives the same three with a
   window around them. This file is a third composition of the same contract;
   it copies none of the bring-up, the capture or the census. */
int  port_scene_begin(void *hwnd, int zoom);
void port_scene_tick(int frame, int tick_game);
int  port_scene_finish(int frames_run);
int  port_scene_frames_wanted(void);
int  port_scene_env_want(void);

/* hal/level_change.cpp: the ROM's own four-line latch plus the boot target,
   and the release of a scene request that is not going to be spawned. */
int  port_level_entry_latch(void);
int  port_scene_request_release(const char *why);

/* The ROM's request words. Types match hal/scene_boot.cpp:278/282 and
   hal/level_change.cpp:114 exactly -- a widened redeclaration here would be a
   silent read of the wrong bytes. */
extern unsigned short data_02092664;   /* pending scene id, 0x187 = none */
extern unsigned char  data_02092660;   /* "a scene has spawned" latch */
extern signed char    data_02092110;   /* next level, -1 = nothing pending */

/* The save block. data_0209caa0 is the ACTIVE FileSaveData, 0x44 bytes, and
   the two bytes read here are the ROM's own record of the pick:
     +0x41  currentCharacter, which StartFile copies to data_02092128
     +0x328 the slot index func_02013c84 was handed (0, 1, 2)
   hal/level_boot.cpp hosts the whole 0x32c run contiguously; +0x328 lives in
   its fifth block and reading it short of that is the bug that file's
   SAVEBLK group exists to prevent. */
extern unsigned char data_0209caa0[];
extern unsigned char data_02092128[];

}  /* extern "C" */

enum {
    SCENE_NONE  = 0x187,   /* the ROM's "nothing pending" sentinel */
    SCENE_STAGE = 3,       /* scene 3 IS the Stage, i.e. the level */
    SCENE_TITLE = 1
};

static int g_armed = -1;   /* -1 = not yet resolved */
static int g_taken;        /* did a run end by entering the adventure */

/* SM64DS_TITLE_ENTRY=1, and only on the title. Resolved once and cached, the
   way every other knob on this path is. */
extern "C" int port_title_entry_armed(void)
{
    if (g_armed < 0) {
        const char *e = std::getenv("SM64DS_TITLE_ENTRY");
        g_armed = (e && std::atoi(e) != 0 &&
                   port_scene_env_want() == SCENE_TITLE) ? 1 : 0;
    }
    return g_armed;
}

/* Did the scene run end because the game asked to start? Read by
   tests/walk_window.cpp to decide whether to return the scene's exit code or
   fall through to the level boot. */
extern "C" int port_title_entry_taken(void)
{
    return g_taken;
}

/* THE HANDOFF TEST, called after a tick by every loop that can host the
   bridge. 0 = keep ticking. Non-zero = the title is done and the run should
   stop; whether that stop is an ENTRY or a REFUSAL is settled by
   port_title_entry_commit, which prints either way. */
extern "C" int port_title_entry_should_stop(void)
{
    if (!port_title_entry_armed())
        return 0;
    if (data_02092664 != SCENE_STAGE)
        return 0;                    /* not the Stage: 2, 6 and 7 are not ours */
    return data_02092660 == 0;       /* the title has finished tearing down */
}

/* COMMIT, once, after the scene's own census has been written. Returns 1 if
   the caller should fall through to the level path, 0 if it should return the
   scene's exit code. Prints its reasoning either way, because a bridge that
   declines silently is indistinguishable from one that never armed. */
extern "C" int port_title_entry_commit(void)
{
    if (!port_title_entry_armed())
        return 0;
    if (data_02092664 != SCENE_STAGE)
        return 0;

    const int slot = (int)data_0209caa0[0x328];
    const int chr  = (int)data_0209caa0[0x41];

    const int level = port_level_entry_latch();
    if (level < 0) {
        /* The Stage was asked for and no level was staged with it. That is a
           state this bridge does not understand, so it refuses rather than
           booting SM64DS_LEVEL's default and calling it an entry. */
        std::printf("[title-entry] REFUSED: scene 3 was requested but no level "
                    "is pending (data_02092110 < 0). StartFile did not run, or "
                    "something consumed its request. Not entering the "
                    "adventure; the run ends here rather than leaving a torn "
                    "-down title ticking.\n");
        std::fflush(stdout);
        return 0;
    }

    /* The request is being served by the level boot, not by a scene spawn, so
       the pending id is released rather than left standing. Same call the
       level path already makes for the same reason. */
    port_scene_request_release("the title handed off and the adventure is "
                               "booting on the level path");

    std::printf("[title-entry] ENTERING THE ADVENTURE: level %d, entrance %d, "
                "save slot %d, character %d\n",
                level, 0, slot, chr);
    std::printf("[title-entry]   level and entrance are StartFile's own "
                "LoadLevelNoReturn(1, 0, 1, 0); slot is data_0209caa0[0x328] "
                "and character is data_0209caa0[0x41], both written by the "
                "ROM's own func_02013c84/StartFile. The port chose none of "
                "them.\n");
    std::fflush(stdout);

    g_taken = 1;
    return 1;
}

/* THE HEADLESS TITLE RUN WITH THE BRIDGE IN IT. The same composition
   port_scene_run makes, with the stop test after the tick -- which is where it
   belongs, because the title writes the request from inside port_actor_tick
   and the words are only settled once that tick has returned.

   Returns the process exit code. On an entry the census has already been
   written and the caller falls through to the level boot. */
extern "C" int port_title_entry_run(void)
{
    const int budget = port_scene_frames_wanted();
    const int rc = port_scene_begin(0, 1);
    if (rc)
        return rc;

    int frame = 0;
    for (; frame < budget; ++frame) {
        port_scene_tick(frame, 1);
        if (port_title_entry_should_stop()) {
            ++frame;             /* this frame ran; count it */
            break;
        }
    }

    /* THE CENSUS IS WRITTEN OVER THE FRAMES THAT ACTUALLY RAN, not over the
       budget. A short run that reports the budget is how a capture ends up
       attributed to frames nobody ticked. */
    const int scene_rc = port_scene_finish(frame);
    if (port_title_entry_commit())
        return 0;                /* the caller falls through; rc is the level's */
    return scene_rc;
}
