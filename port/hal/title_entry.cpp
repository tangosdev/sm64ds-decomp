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
 *
 * ^^ THAT LAST PARAGRAPH IS RETIRED. It was true when it was written and the
 * boot-flow seam at the bottom of this file is what retires it: the shipped
 * default now DOES reach this file, through the title, and SM64DS_TITLE_ENTRY
 * has become the developer opt-OUT rather than the opt-in. The paragraph is
 * kept rather than deleted because the next reader will find it quoted in the
 * lane records and in port/tools/*.py comments, and a quote whose source has
 * silently changed is worse than one that says so. See THE BOOT FLOW below.
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

/* ---- THE BOOT FLOW -------------------------------------------------------
 *
 * THE OWNER'S RULING, verbatim: "Boot to title with option in settings to
 * toggle skip main menu and boot to file and then let them select a file A b
 * or c and also an option to skip opening cutscene."
 *
 * So the shipped default stops being a level and becomes the ROM's own first
 * screen. Everything that makes that work was already here and already
 * matched; what was missing was one answer to one question, and this is it.
 *
 * ---- THE ROM'S ORDER, MEASURED, AND IT IS NOT WHAT THE BRIEF ASSUMED -----
 *
 * The opening cutscene does NOT come before the title. It comes LAST:
 *
 *     title -> menu -> file select -> a slot is picked -> CUTSCENE -> adventure
 *
 * That is the ROM's own order and it is readable in two independent places.
 * src/_ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider.cpp:76-98
 * computes the opening's gate from game mode 0, flags2 bit 7 clear and
 * ContinueKuppaScriptIfNecessary()==0 and then calls StartIntroCutscene() --
 * and Stage::LoadClsnAndObjects runs during the LEVEL boot, which on this path
 * is the boot StartFile asked for after the file was picked. The landed proof
 * log agrees: runs/rel0215/out/gatefix/after/intro.log has
 * "[title-entry] ENTERING THE ADVENTURE" at line 60604 and "[intro] the
 * opening is ARMED for this entry" at 122066, in that order, in one process.
 *
 * IT MATTERS FOR THE KNOBS. SM64DS_SKIP_MENU alone therefore still plays the
 * cutscene, because the cutscene is downstream of the file pick, not upstream
 * of the title. A reader expecting "skip the menu, get the cutscene first"
 * would be reading the wrong machine.
 *
 * ---- WHAT NAMES A DESTINATION -------------------------------------------
 *
 * The default is only the default when NOTHING else has said where to boot.
 * Four things say it, and all four keep working exactly as they did:
 *
 *   SM64DS_SCENE=<id>   the scene harness. Read before this seam is consulted
 *                       at all (hal/scene_boot.cpp's port_scene_env_want), so
 *                       every battery scene row and every lane's scene run is
 *                       untouched.
 *   SM64DS_LEVEL=<n>    the level harness. Every battery level row, every
 *                       selftest and every proof that names a level.
 *   SM64DS_VS_MAP=<n>   AND THIS ONE IS THE TRAP. tests/walk_window.cpp's
 *                       port_menu_relaunch_vs clears BOTH SM64DS_SCENE and
 *                       SM64DS_LEVEL and sets only this, so its child is bare
 *                       in the two names above and would have booted the title
 *                       instead of the VS match. It is a destination and it is
 *                       treated as one.
 *   SM64DS_BOOT_CLASSIC=1  the opt-out: the direct level boot, as before.
 *
 * SM64DS_TITLE_ENTRY=0 is accepted as a second spelling of BOOT_CLASSIC,
 * because that is the name the tree already documents for "no title bridge"
 * and a reader who sets it means the old boot.
 *
 * RESOLVED ONCE AND CACHED, like every other knob on this path, and it must
 * not call port_scene_env_want: that function calls THIS one, and the two
 * would chase each other. It reads SM64DS_SCENE itself instead, which is the
 * same question asked one layer down. */
static int g_boot_default = -1;    /* -1 = not yet resolved */

static int bf_flag(const char *name, int missing)
{
    const char *e = std::getenv(name);
    return e ? (std::atoi(e) != 0) : missing;
}

/* The destination a bare boot takes: SCENE_TITLE, or -1 for "the level path,
   as before". Called by hal/scene_boot.cpp's port_scene_env_want when and only
   when SM64DS_SCENE is unset. */
extern "C" int port_boot_default_scene(void)
{
    if (g_boot_default < 0) {
        g_boot_default =
            (std::getenv("SM64DS_LEVEL")  == 0 &&
             std::getenv("SM64DS_VS_MAP") == 0 &&
             !bf_flag("SM64DS_BOOT_CLASSIC", 0) &&
             bf_flag("SM64DS_TITLE_ENTRY", 1)) ? 1 : 0;
    }
    return g_boot_default ? SCENE_TITLE : -1;
}

/* Did THIS process boot the title because nothing named a destination? The
   distinction is load-bearing and it is what keeps the battery's scene-1 row
   unchanged: an explicit SM64DS_SCENE=1 is a MEASUREMENT of the title scene
   and still needs SM64DS_TITLE_ENTRY=1 to grow a bridge into the adventure,
   while the shipped default IS the adventure's front door and arms the bridge
   by itself. */
extern "C" int port_boot_is_default_title(void)
{
    return std::getenv("SM64DS_SCENE") == 0 &&
           port_boot_default_scene() == SCENE_TITLE;
}

/* ---- THE LAUNCHER CONTRACT, AND IT BINDS BOTH SIDES ----------------------
 *
 * The launcher expresses a toggle that is OFF as ABSENCE. It sets
 * SM64DS_SKIP_MENU=1 / SM64DS_SKIP_INTRO=1 when the box is ticked and REMOVES
 * the name from the child's environment when it is not; it never writes "0".
 *
 * So AN ABSENT NAME MUST READ AS OFF, and a reader that needed a
 * present-but-zero value to mean off would break the pairing silently -- the
 * player unticks the box, the launcher deletes the variable, and the game
 * carries on doing the thing he just turned off. bf_flag's second argument is
 * that rule made explicit: it is the answer for a name that is not there, and
 * for both skip knobs it is 0. A present "0" is also honoured, because a
 * developer typing it means off and there is no reason to surprise him.
 *
 * ONE KNOWN, ACCEPTED HOLE. The already-shipped launcher neither sets nor
 * removes these names, so a player who has exported SM64DS_SKIP_MENU=1 in his
 * own shell has it inherited straight through that launcher into the game.
 * That is opt-in shaped -- he has to have typed it -- and the new launcher
 * closes it by removing the name rather than leaving it alone. Recorded here
 * rather than defended against: scrubbing the environment on the game side
 * would also scrub the launcher's own legitimate set of it, and the two are
 * indistinguishable from inside this process.
 *
 * SM64DS_SKIP_MENU=1 -- "boot to file". The player lands on the file select
 * with A, B and C in front of him instead of walking the attract, the
 * press-start and the menu. HOW it gets there is hal/title_skip.cpp's problem
 * and the honesty rule lives in that file's banner; this is only the knob. */
extern "C" int port_boot_skip_menu(void)
{
    static int v = -1;
    if (v < 0) v = bf_flag("SM64DS_SKIP_MENU", 0);
    return v;
}

/* SM64DS_SKIP_INTRO=1 -- "skip opening cutscene". Read by
   hal/level_boot.cpp's port_intro_suppressed, which is the ONE suppression
   seam and stays the one. SM64DS_INTRO=0 is accepted as the second spelling
   for the reason SM64DS_TITLE_ENTRY=0 is above: it is the name already in the
   tree for "not this time". */
extern "C" int port_boot_skip_intro(void)
{
    static int v = -1;
    if (v < 0)
        v = (bf_flag("SM64DS_SKIP_INTRO", 0) || !bf_flag("SM64DS_INTRO", 1));
    return v;
}

/* SM64DS_TITLE_ENTRY=1, and only on the title -- OR the shipped default boot,
   which arms itself. Resolved once and cached, the way every other knob on
   this path is. */
extern "C" int port_title_entry_armed(void)
{
    if (g_armed < 0) {
        if (port_scene_env_want() != SCENE_TITLE) {
            g_armed = 0;
        } else if (port_boot_is_default_title()) {
            /* The default boot's whole point is that picking a file plays the
               game. Requiring a flag here would ship a title screen that
               cannot be left. */
            g_armed = 1;
        } else {
            /* An explicit SM64DS_SCENE=1 is somebody measuring the title
               scene. It behaves exactly as it did before this lane. */
            g_armed = bf_flag("SM64DS_TITLE_ENTRY", 0);
        }
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

/* ---- SM64DS_SKIP_MENU: BOOT TO FILE, THROUGH THE ROM'S OWN CHOICE ---------
 *
 * The owner asked for a toggle that skips the main menu and drops the player
 * on the file select, where he picks A, B or C normally. This is that, and it
 * is ONE CALL TO ONE MATCHED ROM FUNCTION with the value the ROM's own machine
 * passes when the player taps that row. No state is written by the port.
 *
 * ---- WHAT THE CHAIN ACTUALLY DOES, MEASURED FRAME BY FRAME ----------------
 *
 * From a traced title run of the shipped binary (SM64DS_TITLE_TRACE=1, the
 * five-tap chain, out/boot-title/A1_probe_short.log). Top-state on the left,
 * the ROM's own words on the right:
 *
 *   f0     state 11   the title's opening animation. NOTHING SKIPS IT HERE --
 *                     the taps at f200 and f400 changed nothing, because the
 *                     ROM's own skip in src/func_ov007_020b0a20.c needs
 *                     func_ov007_020aebac() == 0 and on this save state it is
 *                     not. The state times out on its own.
 *   f504   req 10     state 11's own exit: `unk8->unk2 = 0xA`.
 *   f505   state 10   times out at its 0x3c frame into a screen fade.
 *   f634   state 1    then asks for 0.
 *   f666   state 0    THE TITLE'S FRONT SCREEN. Live, waiting.
 *   f808   verdict 9, req 6      <- the tap at (160,168). This is the menu.
 *   f809   state 6    the page change.
 *   f878   state 0, ELEM 5       <- THE FILE SELECT IS UP.
 *   f1008  verdict 3, req 11     <- the tap at (44,68): file A.
 *   f1108  pending scene 3, and the bridge carries it into the adventure.
 *
 * So the whole of "the main menu" between the title coming up and the file
 * select appearing is ONE PICK, and the pick's entire effect is a call to
 *
 *     func_ov007_020aec94(9)
 *
 * ---- WHY THAT FUNCTION AND NOT A STATE WRITE ------------------------------
 *
 * src/func_ov007_020aec94.c is the title's own "the player chose X". It is
 * three statements and both of them matter:
 *
 *     g[4] = r4;                                 // g+0x10, the choice
 *     ...
 *     if ((unsigned)(r4 - 3) <= 2)               // 3,4,5 = files A, B, C
 *         *(u16 *)(g[2] + 2) = 0xb;              // ask for top-state 11
 *     else
 *         *(u16 *)(g[2] + 2) = 6;                // ask for top-state 6
 *
 * g[2] IS THE TOP-STATE RECORD and +2 is its REQUESTED field -- the same word
 * state 11's own exit writes, the same word the router func_ov007_020b1224
 * writes, the same word hal/scene_boot.cpp's trace reads as `req`. So this
 * function does not move the machine; it ASKS the machine, in the machine's
 * own vocabulary, and src/func_ov007_020b0548.c's dispatcher performs the
 * transition on its own next frame. The fade, the sound, the page build and
 * the element rebuild are all the ROM's and all of them run.
 *
 * IT ALSO BRANCHES ON THE ROM'S OWN DATA AND THE PORT DOES NOT PRE-EMPT THAT.
 * The same function's first branch is `if (r4 == 9) if (r5 || r6) ... ask for
 * state 9 instead` -- the confirmation prompt -- where r5 and r6 are two save
 * bytes at the title's own +0x37/+0x38. Whichever way that falls is the ROM's
 * reading of its own save state, unchanged by this file.
 *
 * 9 IS MEASURED, NOT CHOSEN. It is what the player's own tap produced at f808
 * above, and the page it opened is the page the f1008 tap picked file A on.
 * The port supplies the choice; it does not invent the destination.
 *
 * ---- THE GUARD IS THE ROM'S TOO ------------------------------------------
 *
 * Fired on the first frame the front screen is live AND the menu is idle, and
 * "idle" is not this file's opinion: it is the test src/func_ov007_020b1224.c
 * opens with, the chain g+0x134 -> [0] -> [+4], zero when nothing is playing.
 * Calling into a menu mid-animation is how a page gets built twice.
 *
 * ONCE PER PROCESS. The chain comes BACK to top-state 0 when the file select
 * is up (f878 above, with the element machine at 5), so a condition of "state
 * 0 and nothing chosen" is true twice and the second time is the screen the
 * player is supposed to be looking at. Firing again there would take the file
 * select away from him.
 *
 * ---- WHAT IT DELIBERATELY DOES NOT DO ------------------------------------
 *
 * It does not touch the title's opening animation. The owner's ruling is "boot
 * to title", and states 11, 10 and 1 above ARE the title arriving -- roughly
 * eleven seconds of the ROM's own timing that no input in the measured run
 * shortened, because the ROM's own skip gate refused it. Forcing that gate
 * would be the port overruling the game about its own save state, which is
 * exactly the class of fix port/tools/linkage.py's north star refuses.
 *
 * And it does not pick a file. That is the player's, which is the whole point
 * of the toggle. */
extern "C" {
extern char *data_ov007_0210342c;      /* dScDSMT_c's scene global */
void func_ov007_020aec94(int choice);  /* src/func_ov007_020aec94.c */
}

enum { TITLE_CHOICE_FILE_SELECT = 9 };

/* Is the title's own menu idle? src/func_ov007_020b1224.c's own opening test,
   with the null checks a host has to add because the port can be ticking this
   scene before the chain is built and a fault here would be the port's. */
static int title_menu_idle(char *g)
{
    char *p1 = *(char **)(g + 0x134);
    char *p2 = p1 ? *(char **)(p1) : 0;
    char *p3 = p2 ? *(char **)(p2 + 4) : 0;
    return p3 && *(short *)p3 == 0;
}

/* Called once per frame from hal/scene_boot.cpp's port_scene_tick, beside the
   state trace and for the same reason: after the actor phases, so it reads the
   state the frame ended in. Inert unless SM64DS_SKIP_MENU is set. */
extern "C" void port_title_skip_tick(int frame)
{
    static int done;
    if (done || !port_boot_skip_menu())
        return;
    /* Only on the title. port_scene_env_want is the same test the bridge
       makes, and it is cheap and cached. */
    if (port_scene_env_want() != SCENE_TITLE)
        return;
    char *g = data_ov007_0210342c;
    if (!g)
        return;                       /* the scene has not been built yet */
    char *sp = *(char **)(g + 8);
    if (!sp)
        return;
    if (*(short *)(sp + 0) != 0)
        return;                       /* not the front screen yet */
    if (*(short *)(sp + 2) != -1)
        return;                       /* a transition is already requested */
    if (*(int *)(g + 0x10) != 0)
        return;                       /* something has already been chosen */
    if (!title_menu_idle(g))
        return;                       /* the ROM's own idle test says wait */

    done = 1;
    func_ov007_020aec94(TITLE_CHOICE_FILE_SELECT);
    std::printf("[skip-menu] f%d: chose %d through the ROM's own "
                "func_ov007_020aec94, which asked its own top-state machine "
                "for %d. The file select comes up on the chain's own "
                "transition; A, B and C are the player's.\n",
                frame, TITLE_CHOICE_FILE_SELECT,
                (int)*(short *)(sp + 2));
    std::fflush(stdout);
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
