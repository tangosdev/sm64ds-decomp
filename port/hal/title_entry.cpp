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
 *   f0     state 11   THE TITLE SCREEN, and it WAITS. See the correction
 *                     below: it does not time out, it is waiting to be
 *                     touched, and the tap at f200 is what starts it leaving.
 *   f504   req 10     state 11's own exit: `unk8->unk2 = 0xA`. Exactly 304
 *                     frames after the tap, and the 4000-frame run agrees to
 *                     the frame -- its tap is at f1700 and its exit at f2004.
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
 * ---- THE CORRECTION THAT SHAPED THIS, AND IT IS WORTH READING ------------
 *
 * A first cut of this file said top-state 11 was an attract sequence that
 * "times out on its own", and built the skip on that. IT IS NOT TRUE. Top-
 * state 11 IS THE TITLE SCREEN AND IT WAITS FOREVER. Measured directly rather
 * than inferred: out/boot-title/P0_attract_noinput.log is a default boot with
 * no input at all, 3000 frames, and its state trace has one line at f1 and
 * then silence -- the machine never leaves 11. The reading that produced "it
 * times out" came from two runs that BOTH tapped, and the 504 and 2004 they
 * exited at are both exactly 304 frames after their own first tap.
 *
 * That is correct behaviour and it is what a title screen does. It also means
 * there is no state the port can ASK for to get past it: src/func_ov007_-
 * 020b0a20.c's only ways out are its animation reaching its end and an INPUT
 * EDGE. So step 1 below supplies the edge -- through the same poll a real
 * stylus goes through -- and leaves every one of the ROM's own conditions in
 * place to decide what to do with it, including its own reading of its own
 * save slots through func_ov007_020aebac. If the ROM refuses, the title stays
 * up and the player taps it himself.
 *
 * ---- WHAT IT DELIBERATELY DOES NOT DO ------------------------------------
 *
 * It does not pick a file. That is the player's, and it is the whole point of
 * the toggle. */
extern "C" {
extern char *data_ov007_0210342c;      /* dScDSMT_c's scene global */
void func_ov007_020aec94(int choice);  /* src/func_ov007_020aec94.c */
/* hal/sub_screen.cpp: a press consumed by poll_touch in the same place the
   scripted probe and the comms injection are, so from that line on it is
   indistinguishable from a stylus. */
void port_touch_force_press(int x, int y, int frames);
}

enum { TITLE_CHOICE_FILE_SELECT = 9 };

/* src/func_ov007_020b1224.c's own opening test, READ AND REPORTED RATHER THAN
   GATED ON. It was a gate in the first cut of this file and that was wrong:
   measured over a whole SKIP_MENU run (out/boot-title/P2_skipmenu_elems.log,
   first attempt) the word is never zero while the menu is up, so gating on it
   meant the pick never fired at all. It gates the ROUTER -- the path a stylus
   hit takes -- and this file does not use the router. Kept as a printed
   number because the next reader will find the same three lines in the ROM
   and wonder, and -1 for a chain that is not built yet. */
static int title_router_word(char *g)
{
    char *p1 = *(char **)(g + 0x134);
    char *p2 = p1 ? *(char **)(p1) : 0;
    char *p3 = p2 ? *(char **)(p2 + 4) : 0;
    return p3 ? (int)*(short *)p3 : -1;
}

/* ---- SM64DS_TITLE_ELEMS=<frame>: WHERE THE MENU'S BOXES ARE --------------
 *
 * READ-ONLY, DEFAULT OFF, one dump on the named frame. It exists because the
 * A/B/C entry proofs have to tap the three save files at real DS pixels, and
 * the alternative to this was guessing coordinates and reading the resulting
 * save-slot number back -- which is a search, not a measurement, and which
 * cannot tell "I missed the box" from "the box is not there".
 *
 * The arithmetic is src/func_ov007_020b46b0.c's own, copied from its armed
 * branch rather than reinvented: the element record's [1] is the placement,
 * whose 20.12 x/y at +4/+8 shift down into screen pixels, and the object's
 * +0x28 points at a box whose +4/+8/+0xc/+0x10 are min x, min y, max x, max y
 * RELATIVE to that placement. The centre printed beside each box is the point
 * a tap should aim at, and it is the arithmetic mean of the ROM's own edges
 * rather than a number anybody chose.
 *
 * The element list is the same one func_ov007_020aed98 walks: 24 records at
 * ctx+0x114, four bytes apart. Every dereference is guarded, because this can
 * be asked of a frame on which the page is half-built. */
extern "C" void port_title_elems_dump(int frame)
{
    static int at = -2;
    if (at == -2) {
        const char *e = std::getenv("SM64DS_TITLE_ELEMS");
        at = e ? std::atoi(e) : -1;
    }
    if (at < 0 || frame != at)
        return;
    char *g = data_ov007_0210342c;
    if (!g) {
        std::printf("[elems] f%d: no scene global\n", frame);
        std::fflush(stdout);
        return;
    }
    char *sp = *(char **)(g + 8);
    char *ep = *(char **)(g + 4);
    std::printf("[elems] f%d: top-state %d, element-machine %d, choice %d\n",
                frame, sp ? (int)*(short *)sp : -1,
                ep ? (int)*(short *)ep : -1, *(int *)(g + 0x10));
    for (int i = 0; i < 24; ++i) {
        char **rec = *(char ***)(g + 0x114 + i * 4);
        if (!rec)
            continue;
        char *obj = rec[0];
        char *place = rec[1];
        if (!obj || !place)
            continue;
        char *box = *(char **)(obj + 0x28);
        if (!box)
            continue;
        const int px = *(int *)(place + 4) >> 12;
        const int py = *(int *)(place + 8) >> 12;
        const int x0 = *(int *)(box + 4) + px, y0 = *(int *)(box + 8) + py;
        const int x1 = *(int *)(box + 0xc) + px, y1 = *(int *)(box + 0x10) + py;
        std::printf("[elems]   slot %2d  id %3u  box x %4d..%-4d y %4d..%-4d"
                    "  centre (%d,%d)\n",
                    i, (unsigned)*(unsigned short *)obj, x0, x1, y0, y1,
                    (x0 + x1) / 2, (y0 + y1) / 2);
    }
    std::fflush(stdout);
}

/* Called once per frame from hal/scene_boot.cpp's port_scene_tick, beside the
   state trace and for the same reason: after the actor phases, so it reads the
   state the frame ended in. Inert unless SM64DS_SKIP_MENU is set. */
extern "C" void port_title_skip_tick(int frame)
{
    port_title_elems_dump(frame);
    static int tapped;      /* the title screen has been touched */
    static int chosen;      /* the menu row has been picked */
    if (chosen || !port_boot_skip_menu())
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
    const int state = *(short *)(sp + 0);
    const int req   = *(short *)(sp + 2);

    /* ---- STEP 1: THE TITLE SCREEN WANTS A TOUCH, SO GIVE IT ONE ----------
     *
     * Top-state 11 is the title screen and it waits. MEASURED, and it is the
     * measurement that shaped this whole function: a default boot with no
     * input sits there for 3000 frames and never leaves --
     * out/boot-title/P0_attract_noinput.log has one state line at f1 and then
     * nothing. There is no state to ask for here; src/func_ov007_020b0a20.c's
     * only exits are its own animation ending and an INPUT EDGE, so the port
     * supplies the edge and the ROM's own gate decides what to do with it.
     *
     * The press goes in through hal/sub_screen.cpp's poll, beside the scripted
     * probe and the comms injection, which is early enough in the frame for
     * func_ov007_020c1db0 to have built the panel record the title reads. The
     * centre of the touch screen because the title takes a touch ANYWHERE --
     * its gate reads the down edge and not a rectangle -- and because that is
     * the point every proven chain in this tree taps.
     *
     * The state's own frame counter must be past 4 before the ROM will look
     * (`mode >= 4` in its gate), so this waits for it rather than tapping into
     * a screen that is still arriving and then wondering why nothing moved. */
    if (state == 11) {
        if (!tapped && *(int *)(sp + 0xc) >= 8) {
            tapped = 1;
            port_touch_force_press(128, 96, 2);
            std::printf("[skip-menu] f%d: the title screen is waiting for a "
                        "touch (top-state 11, its own frame %d). Supplying "
                        "one at (128,96) through the same poll a real stylus "
                        "goes through; src/func_ov007_020b0a20.c's own gate "
                        "decides whether to take it.\n",
                        frame, *(int *)(sp + 0xc));
            std::fflush(stdout);
        }
        return;
    }
    if (state != 0)
        return;                       /* still in transit: 10, 1 */
    if (req != -1)
        return;                       /* a transition is already requested */
    if (*(int *)(g + 0x10) != 0)
        return;                       /* something has already been chosen */
    /* THE TWO POINTERS src/func_ov007_020aec94.c WILL DEREFERENCE. It reads
       two save bytes through g+0x28 and writes the request through g+8, and it
       carries no null check of its own because on the DS it is only ever
       reached from a live menu. A host can tick this scene before the chain is
       built, so the fault would be the port's to answer for. */
    if (!*(char **)(g + 0x28))
        return;
    /* AND THE STATE'S OWN FRAME COUNTER, which is the ROM's own idea of "this
       state has settled": src/func_ov007_020b0a20.c gates its input on
       `mode >= 4` off exactly this word, and src/func_ov007_020b10dc.c and
       src/func_ov007_020b0834.c both run their entry work on its zero frame.
       Eight rather than four for the reason the title tap uses eight: it costs
       an eighth of a second and it puts the choice after every handler in the
       chain has had its zero frame. */
    if (*(int *)(sp + 0xc) < 8)
        return;

    /* ---- STEP 2: THE MENU ROW, THROUGH THE ROM'S OWN CHOICE -------------- */
    chosen = 1;
    const int router = title_router_word(g);
    func_ov007_020aec94(TITLE_CHOICE_FILE_SELECT);
    std::printf("[skip-menu] f%d: chose %d through the ROM's own "
                "func_ov007_020aec94, which asked its own top-state machine "
                "for %d. The file select comes up on the chain's own "
                "transition; A, B and C are the player's. (router word %d, "
                "reported not gated on -- see title_router_word)\n",
                frame, TITLE_CHOICE_FILE_SELECT,
                (int)*(short *)(sp + 2), router);
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
