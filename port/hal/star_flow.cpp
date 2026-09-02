// Gate 35: the course loop -- what makes a level a GAME rather than a walk.
//
// Everything here is a SEAM onto matched src, not a reimplementation. The
// game already owns damage, death, respawn, the coin counter and the star
// bookkeeping; all of it is in the link and has been since gate 10. What was
// missing is the state those functions read before they mean anything, and a
// caller for the two or three entry points nothing on the port reaches yet.
//
// Four pieces:
//
//   port_course_seat()      the boot: player globals, the sound group/bank,
//                           and the course's own music, all through the
//                           game's own functions.
//   Actor::GivePlayerCoins  the COIN SEAM. Matched src, added to the link
//                           here. See the header over port_give_player_coins.
//   port_star_collect()     the star's bookkeeping and the course-clear
//                           handoff, which is the level-independent half of
//                           func_ov002_020e8ef0 plus func_ov002_020c94a4's
//                           ExitLevel tail.
//   HitDeathPlane           an ARM argument ride-through, hosted here for the
//                           same reason as the five in hal/sdat/sound_abi.cpp.
//
// plus a probe surface (port_course_probe_*) the harnesses drive so every
// one of those paths can be shown moving with a log rather than asserted.
#include <cstdio>
#include "vs_width.h"   /* run vs16: the port's player width */
#include <cstdlib>
#include <cstring>

#include "sdat/sdat.h"

extern "C" {

// ---- the game's own functions this file calls ------------------------------
void SetPlayerGlobals(void);
int  GetSoundGroupID(int level, int sub);
void _ZN5Sound19LoadGroupAndSetBankEii(int group, int bank);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int seqId);
unsigned int _ZN5Sound6Play2DEjj(unsigned int bank, unsigned int id);
unsigned int func_02012790(unsigned int id);
void _ZN5Event6SetBitEj(unsigned int bit);
int  _ZN5Event6GetBitEj(unsigned int bit);
void _ZN5Event8ClearBitEj(unsigned int bit);
void CollectStarInCurLevel(int starId);
int  IsStarCollectedInCurLevel(int starId);
void SetNextLevel(int reason);
void StartExitCharacterWipe(void);
void StartExitFaderWipe(int wipeType);
void ExitLevel(void);
void KillPlayer(void);
void GiveCoins(int playerNo, int amount);
short NumCoins(void);
unsigned char NumStars(void);
int  _ZN6Player9GetHealthEv(void *player);
int  _ZN6Player4HealEi(void *player, int amt);
int  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *src,
                                             unsigned kind, int knockback,
                                             unsigned char scaleByChar,
                                             unsigned char special,
                                             unsigned char particles);
void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *actor, void *player,
                                           unsigned char count,
                                           unsigned int kind);
int  func_ov002_020d82f0(void *player);   /* Player::Hurt's own entry gate */
void sdat_host_tick(void);

// ---- the globals it reads and seats ----------------------------------------
extern signed char  data_0209f2f8;    /* current sublevel */
extern signed char  data_0209f2f4[];  /* lives */
extern short        data_02092144[];  /* per-player health: HP<<8 | fraction */
extern signed char  data_02092114;    /* QUEUED character swap; -1 = none */
extern signed char  data_0208e428;    /* the SFX bank Player_PlaySoundEffect uses */
extern unsigned char data_0209b47c;   /* the sound group currently loaded */
extern unsigned char data_0209b478;
extern unsigned char data_0209b480;   /* master "sound effects on" */
extern unsigned char data_0209f220;   /* star filter */
extern short        data_0209f358[];  /* per-player coin counter */
extern unsigned char data_0209f228;   /* the star just collected */
extern unsigned char data_0209f2ac;   /* 1 = it was NEW */
extern unsigned char data_0209f250;   /* local player index */
extern unsigned char data_02092110;   /* next sublevel  (SetNextLevel) */
extern unsigned char data_0209f268;   /* next entrance  (SetNextLevel) */
extern unsigned char data_0209f26c;   /* why we are leaving */
extern void        *data_0209f394[];  /* per-player Actor* */
/* Sound::PlayLong, the game's own "start or refresh a looping sound", and its
   0x40-entry handle table. The loop probe drives PlayLong at the player's own
   camera-space position (which the listener is centred on, so it clears the 3D
   distance cull) and reads the table to prove the level-change reap emptied it. */
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int j1,
    unsigned int j2, unsigned int j3, void *v, unsigned int j5);
extern int data_0209b53c[];   /* PlayLong's handle table: 8-byte header, 0x40
                                 entries of 0x14; live when entry+0 != 0 */
extern unsigned char data_0209caa0[];
/* {group, bank, bgm} x 0x34, from romdata. config names the three columns
   separately (data_02075768/69/6a) because the ROM walks them with a stride
   of three; the port emits the run once and does the same arithmetic. */
extern unsigned char data_02075768[];
/* The VS half of the stage's state. Spellings are the ones the two ROM
   functions this file stands in for use: Stage::InitResources declares
   data_0209f2bc s8 and data_0209f304/f308 s16, Stage::Behavior declares
   data_0209f2bc u8 and data_0209f304 u16; the host storage is four bytes wide
   either way (hal/w8a_stage_storage.cpp, hal/auto_bss.cpp). */
extern unsigned char data_0209f2d8;   /* game mode: 0 adventure, 1 VS, 2 script */
extern unsigned char data_0209f2bc;   /* VS start countdown: 3, 2, 1, 0 */
extern unsigned short data_0209f304;  /* frames left in the current count */
extern short         data_0209f308;
extern unsigned char data_0209f2c4;   /* pause-screen state; 0 = playing */
extern unsigned char data_0209fc50;   /* players in the match (SetNumPlayers) */
extern int           data_0208ee44;   /* vblanks per game tick */
extern unsigned char data_0209d45c;   /* engine A's software layer mask; the
                                         frame's DISPCNT publish copies it */

}  // extern "C"

// =============================================================================
// HitDeathPlane: an ARM argument ride-through
// =============================================================================
//
// src/HitDeathPlane.c declares `extern void SetNextLevel(void)` and calls it
// with no arguments, while src/SetNextLevel.c defines `SetNextLevel(int arg)`.
// That is the same shape as the five in hal/sdat/sound_abi.cpp: on ARM the
// argument is already in r0 from HitDeathPlane's own frame and the ROM's
// `bl` never touches it, so mwccarm reproduces the bytes with the callee
// unnamed. On x86 cdecl nothing is pushed and SetNextLevel's `arg` reads
// whatever the caller's stack happened to hold -- and `arg` is what lands in
// data_0209f26c, the reason-for-leaving code the next scene reads.
//
// `arg` is HitDeathPlane's only live value at the call, and it is the same
// quantity SetNextLevel's parameter already carries elsewhere: ExitLevel
// passes 1 (course cleared) and KillPlayer passes 2 (died). So the rider is
// named here and src is left alone. src/HitDeathPlane.c is filtered out of
// SLICE10_CAM_SOURCES in port/CMakeLists.txt.
extern "C" {
void _ZN5Scene14StartSceneFadeEjjt(unsigned a, unsigned b, unsigned short c);

// PORT_HOST_ABI: ARM r0 ride-through -- src calls SetNextLevel(void) with arg in r0.
void HitDeathPlane(int arg)
{
    if (data_0209f2f4[0] != 0 || arg == 0)
        SetNextLevel(arg);
    else
        _ZN5Scene14StartSceneFadeEjjt(8, 0, 0);
    StartExitFaderWipe(6);
}
}  // extern "C"

// =============================================================================
// The boot: the state the whole loop reads before any of it means anything
// =============================================================================

namespace {

int g_seated;
int g_course_music = -1;

/* THE QUEUED SWAP TRAP. data_02092114 is the in-level character swap request
   and -1 means "nothing queued"; SetPlayerGlobals is the only thing that ever
   writes that -1. The port left the symbol zero-initialised, which reads as a
   pending swap to character 0. It is inert while the local player IS Mario
   (Player::InitResources takes the same branch either way), which is why the
   port has walked around for eleven gates without noticing -- and it stops
   being inert the moment anything queues a real swap or checks `>= 0`.
   Running the game's own function is the fix, and it seats the health words
   and the life count in the same call. */
void seat_player_globals(void)
{
    SetPlayerGlobals();
    fprintf(stderr, "[course] player globals: hp=%d/%d lives=%d "
            "queued-swap=%d\n", data_02092144[0] >> 8, 8,
            (int)data_0209f2f4[0], (int)data_02092114);
}

/* The course's sound row, exactly as Stage::InitResources reads it:

       soundGroup = GetSoundGroupID(level)      -- data_02075768[level*3]
       bank       = data_02075769[level*3]      -- ...[level*3 + 1]
       Sound::LoadGroupAndSetBank(soundGroup, bank)
       Sound::LoadAndSetMusic_Layer1(data_0207576a[level*3])

   THE BANK IS WRITTEN HERE AND THAT IS DELIBERATE. LoadGroupAndSetBank is
   matched src and in the link, but its first line is func_0203d974, whose
   read at 0x027ffc40 the port answers for. (This note used to say that
   address was outside every region ntr maps. It is not: SHARED_BASE
   0x027ff000 is a fatal region in ntr/io.cpp's kRegions, and the port writes
   a cartridge-boot 0 there. The reason the face stays is the sound heap, not
   the mapping -- see func_0203d974 below.) The port answers 1 and the ROM's `1`
   branch returns early for any group but 0x2f without recording anything.
   Both branches END by storing the bank and the group, and those two words
   are the whole observable effect once the load itself is a no-op (the SDAT
   consumer pre-seats every FAT residency slot, so there is nothing to load).
   So the port writes what the branch it did not take would have left behind,
   and says so, rather than editing matched src or faking the load.

   data_0208e428 matters concretely: Player_PlaySoundEffect passes it as the
   BANK for every kind-3 sound, and a zero there forces bank 0 on sounds whose
   own SEQARC entry names a different one. */
/* THE VS ARENAS TAKE THE OTHER BRANCH, AND THE PORT WAS TAKING NEITHER.
 *
 * The block above transcribes only the ADVENTURE half of the ROM's sound row.
 * The ROM's own half it left out is the first thing InitResources does:
 *
 *     int soundGroup = 0;
 *     u8  bank = 0x36;
 *     if (data_0209f220 == 2) {                       // a VS match
 *         if (level == 1 || level == 0x33) soundGroup = 0x2B;
 *         else if (level == 0x1D)          soundGroup = 0x2E;
 *         else if (level == 0x2A)          soundGroup = 0x2C;
 *         else if (level == 0x2B)          soundGroup = 0x2D;
 *     }
 *     if (soundGroup == 0) {                          // adventure: the table
 *         soundGroup = GetSoundGroupID(level);
 *         bank       = data_02075769[level * 3];
 *     }
 *     Sound::LoadGroupAndSetBank(soundGroup, bank);
 *
 * -- src/_ZN5Stage13InitResourcesEv.cpp:314-327, and those four ids are the
 * four VS arenas (51, 29, 42, 43). data_0209f220 is 2 in a match because the
 * VS start stages it that way: func_ov075_02116c8c calls
 * LoadLevelNoReturn(map, 0, 2, 0), LoadLevel puts that starID in data_0209f1f0
 * and InitResources copies it to data_0209f220.
 *
 * WHY IT IS NOT COSMETIC. `bank` is data_0208e428, which Player_PlaySoundEffect
 * hands to func_02051e60 as the SBNK every kind-3 sound is looked up in, and
 * func_02051a98 then resolves each note's PROGRAM in that bank. A program the
 * bank does not carry is not a wrong instrument, it is NO NOTE -- the note is
 * dropped where hal/sdat/sseq.cpp's start_note says "program %d has no note
 * there", without a sound and without an error.
 *
 * MEASURED, this branch off versus on: same binary, same 600-frame run, same
 * staged star, bank the only variable. Arena map 0 (level 51) under the
 * adventure bank 0x20: sound 0x39 lost 15 notes and sound 0x3a lost 3, all of
 * them to programs 22 and 23, which bank 0x36 carries and 0x20 does not --
 * under 0x36 both sounds come back whole. Arena map 3 (level 42) under
 * adventure bank 0x34: sounds 0x173, 0x40, 0x41 and 0x06 lost 15 notes between
 * them to programs 22 and 25, and all four come back. That is the reported
 * "some sounds do not play in the arena", and it is per-NOTE rather than
 * per-sound, which is why it reads as intermittent rather than as a sound
 * being missing outright.
 *
 * It is not a one-way trade and this comment will not pretend it is. Bank 0x36
 * has no program 6, so on levels 51 and 29 the coin VARIANT 0x12 loses its
 * program-6 track where the adventure bank happened to have one. That is what
 * the DS does with the bank the DS loads. The chime a normal pickup plays is
 * 0x11, whose program 19 bank 0x36 carries.
 *
 * Nothing here is invented: the branch, the four constants, the 0x36 default
 * and the fall-through to the table are the ROM's, and the adventure path
 * below is bit-for-bit what it was. */
static int vs_sound_group(int level)
{
    if (data_0209f220 != 2)
        return 0;
    if (level == 1 || level == 0x33) return 0x2B;
    if (level == 0x1D)               return 0x2E;
    if (level == 0x2A)               return 0x2C;
    if (level == 0x2B)               return 0x2D;
    return 0;
}

void seat_course_sound(int level)
{
    const unsigned char *row = data_02075768 + level * 3;
    int group = vs_sound_group(level);
    int bank = 0x36;
    const int vs = (group != 0);
    if (!vs) {
        group = GetSoundGroupID(level, 0);
        bank = row[1];
    }
    /* SIGNED, and that is the whole meaning of the column: Stage::InitResources
       declares it `extern s8 data_0207576a[]`, and LoadAndSetMusic_Layer1's
       first branch is `if (j < 0) stop`. 0xff is "this sublevel has no layer-1
       track", not "sequence 255" -- reading it unsigned asks the sequencer for
       an entry 172 past the end of an 83-entry SSEQ table. */
    int bgm = (signed char)row[2];

    /* The SDAT root has to be seated before any of this: LoadAndSetMusic_Layer1
       walks data_020a5bb8 + 0x84 unconditionally, and unlike Sound::Play it
       does not self-initialise. One tick with nothing queued does it. */
    sdat_host_tick();

    _ZN5Sound19LoadGroupAndSetBankEii(group, bank);
    data_0208e428 = (signed char)bank;
    data_0209b47c = (unsigned char)group;
    data_0209b478 = 0;
    data_0209b480 = 1;                 /* master SFX flag; the consumer's
                                          init sets it too, but the boot may
                                          run before the first sound call */

    /* SM64DS_COURSE_MUSIC=<seq> overrides the row. The castle grounds is
       sublevel 1 and its row really does say -1 -- arriving there STOPS
       layer 1, and what the player hears outside is the AMBIENT_SOUND_EFFECTS
       actors, not a BGM. So the one level the port can boot today is also the
       one that proves nothing about a course theme playing; this is how the
       real thing (Bob-omb Battlefield is sublevel 6, group 6, bank 0x21,
       sequence 58) gets exercised before that level mounts. */
    const char *ov = std::getenv("SM64DS_COURSE_MUSIC");
    if (ov) {
        bgm = std::atoi(ov);
        fprintf(stderr, "[course] SM64DS_COURSE_MUSIC=%d overrides the row\n",
                bgm);
    }

    /* THE LAYER-1 CALL IS NOT PART OF THE VS ROW, AND THE PORT WAS MAKING IT.
     *
     * The block above is InitResources' sound-GROUP statement, which the ROM
     * guards on `data_0209f2d8 != 2`; VS is mode 1, so it runs and the VS
     * branch above is right to be here. The MUSIC statement forty lines later
     * is a different statement with a different guard
     * (src/_ZN5Stage13InitResourcesEv.cpp:365-381):
     *
     *     int v0_8 = (data_0209f2d8 == 1) ? 1 : 0;
     *     if (v0_8 == 0) {
     *         ... four more preconditions ...
     *         Sound::LoadAndSetMusic_Layer1(data_0207576a[level * 3]);
     *
     * -- so a VS match reaches the table row NEVER. The port called it anyway,
     * and the arena's row is what came out:
     *
     *   level 51 row[2] = -1  ->  LoadAndSetMusic_Layer1's own `if (j < 0)`
     *                             STOP branch. Silence, and this is Tango's
     *                             "no music at all" on SM64DS_VS_MAP=0.
     *   level 42 row[2] = 65  ->  the ADVENTURE track for the Battle Fort,
     *                             which the DS never plays in a match.
     *
     * The arena's real music is Stage::Behavior's, at the end of the 3-2-1 --
     * see port_vs_countdown_tick below. Only the ROM's VS guard is added here:
     * the other four preconditions (the cutscene flag, the two same-area
     * transitions and the level-2 event bit) are separate port gaps and
     * changing them would move adventure levels, which this does not. */
    const int vs_mode = (data_0209f2d8 == 1);

    /* g_course_music is only read by port_course_sound_probe, and it means
       "what this file last asked layer 1 for". In a match nothing is asked for
       here, and func_ov075_02116c8c's StopLoadedMusic_Layer1 has already run,
       so -1 is the true state until the countdown starts 0x4d. */
    g_course_music = vs_mode ? -1 : bgm;
    fprintf(stderr, "[course] sublevel %d sound row: group=%d bank=0x%02x "
            "bgm=%d (%s) [%s branch, star=%d]\n", level, group, bank, bgm,
            vs_mode ? "VS: the ROM does not read this column"
                    : bgm < 0 ? "no layer-1 track" : "start",
            vs ? "VS arena" : "adventure table", (int)data_0209f220);
    if (!vs_mode)
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(bgm);
    /* Push the START the music call just queued at the ARM9 half through the
       hosted ARM7, so the first frame already has voices allocated rather
       than a batch sitting in the ring. */
    sdat_host_tick();
}

/* Stage::InitResources' VS COUNTDOWN SEAT, its lines 270-279:
 *
 *     if (data_0209f2d8 == 1) { data_0209f2bc = 3;      // 3, 2, 1
 *                               data_0209f304 = 0x28;   // frames per count
 *                               data_0209f308 = 0; }
 *     else                    { data_0209f2bc = 0;
 *                               data_0209f304 = 0; }
 *
 * port/stage_lifecycle_map.txt section 2d lists all three as ABSENT from the
 * port, with data_0209f2bc flagged as already having a live reader. They are
 * seated now because a second reader turned up and it is the one Tango
 * reported: with all three left at their bss zero, Stage::Behavior's countdown
 * cannot start, and the arena's music is the last thing that countdown does.
 * Nothing else in this file reads them; the tick below does. */
void seat_vs_countdown(void)
{
    if (data_0209f2d8 == 1) {
        data_0209f2bc = 3;
        data_0209f304 = 0x28;
        data_0209f308 = 0;
        fprintf(stderr, "[vsgo] VS countdown armed: %d counts of %d frames, "
                "step %d\n", (int)data_0209f2bc, (int)data_0209f304,
                data_0208ee44);
    } else {
        data_0209f2bc = 0;
        data_0209f304 = 0;
    }
}

/* ---- WHY THERE IS NO VS SCOREBOARD AND NO MATCH CLOCK ON SCREEN -----------
 *
 * Reported from real play: no star count and no countdown anywhere, on either
 * screen, for the whole match. The HUD is NOT missing and this is worth saying
 * first, because "the HUD actor never spawns" was the reasonable guess and it
 * is wrong. Measured, SM64DS_VS_HUD=1 (hal/input_probe.cpp) on a live arena:
 *
 *     [vshud] f3 DISPCNT_A 00000000 OBJ OFF | mask d45c=00 (engine A, OBJ NOT
 *             asked for) d454=18 | engine A OAM: 0 all-zero, 122 parked,
 *             6 PLACED
 *
 * Six sprites PLACED in engine A's OAM on frame 3 and every frame after. The
 * HUD is alive, HUD::Behavior runs (it is what counts the match clock down --
 * that is the same object), HUD::Render takes its VS arm, and RenderVsTimer
 * and RenderStarCount put their digits exactly where the ROM puts them: engine
 * A, the TOP screen, sub=0 on every OAM::Render call in both.
 *
 * WHAT IS MISSING IS ONE ASSIGNMENT. On the DS nothing writes DISPCNT's layer
 * enables directly. Code sets a software mask -- data_0209d45c for engine A --
 * and func_02019144 copies it into bits 8-12 once a frame. The port does that
 * copy (hal/message_compositor.cpp's engine-A publish, every frame, and it
 * works). What the port never does is SET the mask, because the line that sets
 * it is Stage::InitResources line 402:
 *
 *     data_0209d45c = 0x11;          BG0 and OBJ
 *
 * and Stage::InitResources is not run by the port. port/stage_lifecycle_map.txt
 * section 2d lists that exact statement with "none" in its seat column and
 * notes hal/auto_bss.cpp leaves the word zero. So the mask is 0, the publish
 * copies 0, and the engine-A compositor's `!any_bg && !obj_on` exit returns
 * before it draws -- with the HUD's sprites sitting in OAM the whole time.
 *
 * ---- WHY THIS SEAT IS GATED ON VS, WHEN THE ROM'S IS NOT -------------------
 *
 * The ROM's line has no condition and neither should the port's, eventually.
 *
 * THE REASON THIS FILE FIRST GAVE FOR THE GATE IS RETRACTED. It said the same
 * assignment "turns on the ADVENTURE HUD" and would retake every BMP baseline.
 * That was measured false: with the seat forced on, adventure levels 1 and 5
 * are BYTE-IDENTICAL, because engine A's OAM holds ZERO placed sprites there --
 * the adventure HUD reaches the player through engine B, not this register.
 * Nothing wakes up, so there was never a baseline to spend.
 *
 * THE REAL REASON IS CONSERVATISM, and it is smaller and honest. The seat is
 * PROVEN NEEDED in VS: it is what puts the match clock, the star count and the
 * moved coin counter on screen, each measured to the pixel. Outside VS it is
 * proven to buy NOTHING on the levels measured. A register write that changes
 * nothing observable is still a register write on every adventure boot in the
 * game, on a path this lane did not investigate and cannot speak for, so it
 * stays behind the mode test until somebody has a reason to want it there.
 * Un-gating is a one-word change for the lane that finds one.
 *
 * SM64DS_HUD_LAYER_SEAT=always is that unconditional form on this same binary;
 * =off restores the pre-seat behaviour the same way.
 *
 * THE NAME IS THE SECOND ONE THIS KNOB HAS HAD, and the first was a bug I
 * shipped. It was SM64DS_ENGINE_A_LAYERS, which is ALREADY a live variable:
 * hal/message_compositor.cpp reads it as a HEX LAYER SUBSET
 * (strtoul(e, 0, 16) & 0x1F, default 0x1f), port/tools/headroom.py sets it, and
 * port/ppu_gap_audit.txt documents it. Under that grammar the documented value
 * "always" parses as 0x0a -- BG1 and BG3, with OBJ DROPPED, which is the layer
 * the entire HUD lives on -- and "off" parses as 0x00. Review measured both
 * rendering byte-identical while this seat's own log line said the mask had
 * been seated: the mask was seated and then thrown away downstream. Renamed,
 * and the compositor's variable is untouched.
 *
 * AND THE PREVIEW =always PROMISES IS EMPTY ON THE LEVELS MEASURED, which is a
 * correction to this lane's own round-1 open question rather than a defect in
 * the knob. Re-measured after the rename, adventure selftests on levels 1 and
 * 5, SM64DS_HUD_LAYER_SEAT=always against =off: the seat fires (its log line
 * prints, DISPCNT_A goes 0x00000008 -> 0x00001108, OBJ ON) and the frame is
 * still BYTE-IDENTICAL -- because engine A's OAM holds 0 PLACED sprites on both
 * levels. The adventure HUD draws to the SUB screen: HUD::Render's adventure
 * arm runs (measured, sub OAM 237 nonzero bytes) and every leaf it reaches
 * there goes to engine B. So "the adventure HUD is one statement away" was
 * WRONG. This seat is necessary for it and nowhere near sufficient, and the
 * lane that takes it inherits finding out what actually submits to engine A on
 * an adventure screen. */
void seat_engine_a_layers(void)
{
    static int mode = -1;   /* 0 off, 1 VS only (default), 2 always */
    if (mode < 0) {
        const char *e = std::getenv("SM64DS_HUD_LAYER_SEAT");
        mode = 1;
        if (e && std::strcmp(e, "off") == 0) mode = 0;
        else if (e && std::strcmp(e, "always") == 0) mode = 2;
    }
    if (mode == 0)
        return;
    if (mode == 1 && data_0209f2d8 != 1)
        return;
    data_0209d45c = 0x11;
    /* AND BG0 IS THE 3D LAYER, WHICH HAS TO BE SAID OR THE MASK MAKES IT WORSE.
     *
     * DISPCNT bit 3 selects whether engine A's BG0 shows the 3D engine's output
     * or an ordinary tiled background.
     *
     * AND THE ROM SETS IT IN THIS VERY FUNCTION, 151 lines above the mask
     * statement and in the same unconditional block (found in review; an
     * earlier draft argued this from hardware behaviour, which was true and
     * weaker):
     *
     *     src/_ZN5Stage13InitResourcesEv.cpp:251
     *         int one = 1;
     *         GX::SetGraphicsMode(one, 0, one);      // c = 1
     *
     * and GX::SetGraphicsMode's body is `reg = (c << 3) | reg`
     * (src/_ZN2GX15SetGraphicsModeEiii.c), so c = 1 IS bit 3. The two lines are
     * one statement of the ROM's, split across the function: turn BG0 into the
     * 3D layer, then name BG0 in the layer mask. Seating the mask without the
     * bit takes half of it.
     *
     * The port never calls SetGraphicsMode: it renders 3D through its own path
     * and leaves engine A's DISPCNT alone, so the register's low byte is zero
     * for the whole run.
     *
     * hal/message_compositor.cpp keys on that bit by name: BG0 with bit 3 set
     * is "the 3D LAYER, not composited here (the 3D frame is already in the
     * framebuffer)", and without it an ordinary 2D text background. So seating
     * the mask ALONE would enable a BG0 the DS never composites as 2D, and the
     * compositor would draw whatever happens to be in that tilemap over the
     * whole top screen. Measured, before this line was added: 34638 changed
     * pixels spread over every band of the top screen instead of the HUD's own
     * rows. With it, the compositor skips BG0 exactly as the hardware does and
     * only OBJ reaches the picture. (An earlier revision of this line said
     * "the timer and the star count". It was the timer alone, and the next
     * block is why.)
     *
     * A raw register write in a host file is not the shape this tree likes, and
     * calling the ROM's own SetGraphicsMode instead is worse: its `a` and `b`
     * arguments also set the display mode and the BG mode, two fields the port
     * deliberately leaves at zero because it does not drive engine A. One bit,
     * which is the one bit of that call the port can honour. */
    *(volatile unsigned int *)0x04000000 |= 8;
    /* AND THE 3D LAYER'S PRIORITY, WHICH IS WHY HALF THE VS HUD WAS INVISIBLE.
     *
     * Found chasing the coin move (round 2, order 1) and it is a bigger bug
     * than the thing that found it: THE VS STAR COUNT HAS NEVER RENDERED in
     * this port. Measured with the OAM census, engine A, a live arena:
     *
     *   OAM[0..2]  the timer      prio 0   -> drawn
     *   OAM[3..5]  the star count prio 1   -> placed, real tiles, NOT drawn
     *
     * hal/message_compositor.cpp's layer_behind_3d is the ROM's own rule --
     * a sprite loses to the 3D layer only when `prio > p3d` -- and p3d is read
     * straight out of BG0CNT (0x04000008 bits 0-1). The port never writes that
     * register, so it reads 0, every priority-1 sprite is strictly worse than
     * the 3D layer, and the compositor drops it. The rule is right; the input
     * to it was never seated.
     *
     * THE VALUE IS 2, FROM THE LEVEL PATH'S OWN Stage::InitResources:330:
     *
     *     *(vu16 *)0x04000008 = (*(vu16 *)0x04000008 & ~3) | 2;
     *
     * in the same unconditional block as line 251's GX::SetGraphicsMode(1, 0, 1)
     * and line 402's layer mask -- the two statements this seat already hosts.
     * Nothing on the level path writes engine A's BG0CNT after it:
     * Stage::LoadGraphics2D, which runs two lines later, touches BG0CNT_SUB
     * (0x04001008) only. So all three lines are one statement of the ROM's,
     * split across the function, and this seats the third.
     *
     * AN EARLIER REVISION WROTE 1 AND CITED src/func_02005a58.c FOR IT, calling
     * that "not a guess". Both halves were wrong and review caught them.
     * func_02005a58 is dScBoot_c::InitResources -- a BOOT scene, not the level
     * path -- and its header carries "recovered from vtable slot identity",
     * which is the GUESSED-BODY marker. This lane dropped a body in round 1 for
     * carrying exactly that marker and then quoted another one as authority in
     * the same breath. It cost no pixels (a sprite clears `prio > p3d` at either
     * value, so every measurement taken under the wrong one still stands) and it
     * was still a seated hardware statement with a wrong value and a false
     * provenance: it diverges for BACKGROUNDS, whose rule is `prio >= p3d`, and
     * it printed into every VS playlog.
     *
     * Seating the two priority bits, and nothing else in that register, is the
     * smallest statement of the ROM's own fact. It is what makes the star count
     * appear, and it is the precondition for the coin move in
     * hal/sub_actors.cpp -- the coins are priority 1 too. */
    *(volatile unsigned short *)0x04000008 =
        (unsigned short)((*(volatile unsigned short *)0x04000008 & ~3) | 2);
    fprintf(stderr, "[vshud] engine A layer mask seated: data_0209d45c = 0x11 "
            "(BG0 + OBJ), DISPCNT bit 3 (BG0 is the 3D layer) and BG0CNT "
            "priority 2 (Stage::InitResources:330's own value, which is what "
            "lets a priority-1 HUD sprite draw over the arena). %s\n",
            mode == 2 ? "SM64DS_HUD_LAYER_SEAT=always: every mode, unconditional"
                      : "VS only by default (see the gate note above)");
}

}  // namespace

extern "C" {

/* func_0203d974, hosted. Removed from slice_gate14's source list.
 *
 * The ROM reads the BOOT INDICATOR the firmware leaves at 0x027ffc40 -- "was
 * this console download-played" -- and ORs it with "is my comms slot not the
 * parent's". (An earlier draft of this note called it a console-type word and
 * said nothing mapped it. Both halves are wrong now: ntr/io.cpp maps
 * SHARED_BASE 0x027ff000 as a fatal region, and the port writes 0 there, a
 * cartridge boot. See the boot-indicator note at that write.) So the read is
 * fine and the DS-faithful ANSWER is 0 -- which is exactly the branch this
 * face exists to avoid, for the reason below.
 *
 * The port answers 1. Not because the host is a DSi, but because the 0 branch
 * is the one that walks the SOUND HEAP -- func_020134d8 -> func_02051918 over
 * data_0209b498, and func_0205117c's SolidHeapAllocator save/restore. That
 * heap is built by func_02050f34, which hal/sdat/consumer.cpp SKIPs at init
 * (hal/sdat/sdat.cpp seats an equivalent root instead), so data_0209b498 is
 * null and the DS-faithful answer is a null dereference three calls down. The
 * 1 branch is a clean early return for every group the game asks for.
 *
 * What that costs: nothing on the port, because there is nothing to load.
 * What it hides: the bank/group store at the end of both branches, which
 * seat_course_sound writes instead. When the sound heap is built for real
 * (see the report), this returns to the src version and that store goes away.
 * PORT_HOST_ABI: src reads the unmapped console-type halfword at 0x027ffc40.
 */
int func_0203d974(void)
{
    return 1;
}

/* The sound row, run FROM THE BOOT at the point Stage::InitResources runs it.
   InitResources seats the course's sound group/bank and layer-1 music between
   its archive load and LoadClsnAndObjects (the block around lines 311-328 and
   365-381 of src/_ZN5Stage13InitResourcesEv.cpp), driven by GetSoundGroupID,
   Sound::LoadGroupAndSetBank and Sound::LoadAndSetMusic_Layer1 -- the same
   matched functions seat_course_sound calls. Hosting it here, called from
   port_stage_a_boot, is what lets the sound row ride EVERY boot the way the ROM
   does, the warp included, instead of only the one gate-35 seat that ran once
   per process. Per-boot guard, reset by the level change, so a warp re-seats.
   The one non-matched write is the bank/group store the hosted func_0203d974==1
   early return skips (see seat_course_sound's own note). */
void port_boot_course_sound(int level)
{
    seat_course_sound(level);
    seat_vs_countdown();
    /* the third statement of Stage::InitResources this file seats, and the one
       that puts the VS scoreboard and the match clock on screen; see its own
       block above for why it is the mask and not the HUD that was missing */
    seat_engine_a_layers();
}

/* ---- THE ARENA'S OWN MUSIC ------------------------------------------------
 *
 * Stage::Behavior's VS countdown statement, run every frame the way
 * Stage::Behavior runs it -- the treatment hal/message_pump.cpp gives
 * Stage::UpdateMessage, and for the same reason: the whole function is not in
 * the port's link (no slice lists src/_ZN5Stage8BehaviorEv.cpp) and pulling it
 * in would bring PS_Update, VE_Init, Scene::SetSceneToSpawn and the rest of the
 * pause/level-change machinery the port does not host. This is ONE statement of
 * it, calling the real matched functions, and nothing else.
 *
 * The body is src/_ZN5Stage8BehaviorEv.cpp:141-170, inside that function's own
 * `data_0209f2d8 == 1` branch:
 *
 *     int cnt = 0;
 *     for (i = 0; i < 4; i++) {
 *         char *p = data_0209f394[i];
 *         if (p && *(u8 *)(p + 0x711)) cnt++;
 *     }
 *     if (cnt >= data_0209fc50) {
 *         if (data_0209f304 == 0x28 && data_0209f2bc == 3) func_02012790(0x2b);
 *         if (data_0209f2c4 == 0) {
 *             if (data_0209f304 != 0 || data_0209f2bc != 0) {
 *                 data_0209f304 -= data_0208ee44;
 *                 if (data_0209f304 == 0 && data_0209f2bc != 0) {
 *                     data_0209f2bc -= 1;
 *                     data_0209f304 = 0x28;
 *                     if (data_0209f2bc != 0) func_02012790(0x2b);
 *                     else { func_02012790(0x2a);
 *                            Sound::LoadAndSetMusic_Layer1(0x4d); }
 *                 }
 *             }
 *         }
 *     }
 *
 * WHY THIS IS THE ARENA'S MUSIC AND NOT A GUESS. Every BL to
 * Sound::LoadAndSetMusic_Layer1 (0x0201320c) in the image was enumerated over
 * arm9_dec.bin and all of extracted/overlays -- 19 call sites. Exactly three
 * are in arm9: ProcessKuppaScript (0x0200eac8), this one (0x0202be08) and
 * InitResources' (0x0202d36c, guarded off in VS). The other sixteen are in
 * ov002/3/4/5/6/7 and ov075, and ov075's four are the LOBBY's own tracks
 * (0x4c/0x51/0x52) -- func_ov075_02118378 and func_ov075_0211a410, both scene
 * code, neither reachable once the arena is up. The four arena overlays
 * (ov059/ov051/ov037/ov050, levels 51/43/29/42) contain not one call. So seq
 * 0x4d, from here, is the only music a VS arena can have.
 *
 * The SDAT agrees from the other end. Group 0x2b, NCS_GRP_VS_CASTLE -- the
 * group Stage::InitResources loads for arena levels 1 and 0x33 -- has FOURTEEN
 * members, and seq 0x4d is among them: WAVEARC 51/40/44, BANK 0/1/54/9, and
 * SEQ 76/77/78/79/80/81/82. (An earlier draft of this record said the group
 * "ships exactly SEQ 77 and BANK 54". It does not; those are two of its
 * fourteen, and the other five SEQs are the rest of the VS set.) BANK 54 is
 * 0x36, the bank seat above, and SEQ 77 is 0x4d, NCS_BGM_VSATHRETIC. The
 * arena's own group carries the arena's own music.
 *
 * cnt >= data_0209fc50 is the ROM's own readiness gate: SetNumPlayers writes
 * data_0209fc50 (SetNumPlayers -> func_020308d0), and +0x711 is set by the
 * player's own level-enter step func_ov002_020c71e0, which is in the link and
 * dispatched through hal/player_states.inc:299. That same step then holds the
 * player where he lands while data_0209f2bc != 0 -- the DS's 3-2-1 freeze --
 * so seating the counter without ticking it down would stop the match dead.
 * The two land together for that reason.
 *
 * THE FREEZE IS NOT THE SAME LENGTH ON EVERY MAP, AND THE LONG ONE IS NOT A
 * HITCH. The counter only runs once cnt has caught up with data_0209fc50, so
 * the hold is 60 frames of countdown plus however long the players take to
 * reach their level-enter step. Measured at 600 frames on all four maps: three
 * of them end the countdown around frame 60, and the Battle Fort (map 4, level
 * 42) around frame 109. That gap is the ROM's own readiness gate answering a
 * heavier level, not the port stalling.
 *
 * func_02012790 is Sound::Play2D(2, id): 0x2b three times for the counts,
 * 0x2a for the GO. Both are the ROM's ids at the ROM's frames.
 *
 * TWO ROM PRECONDITIONS THIS HOSTED STATEMENT DOES NOT CARRY, recorded rather
 * than faked (and logged as one debt line in port/stage_lifecycle_map.txt).
 * Stage::Behavior's VS branch has two early returns AHEAD of the countdown
 * that this function does not reproduce -- the data_0209fc9c arm (the pause
 * state, which returns after func_02032f54) and the data_0209fc68 == 6 arm
 * (match end). The port hosts neither piece of state, so both read zero and
 * the difference is currently unobservable; the day either goes live, this
 * ticks where the ROM would suppress. Related, and the same debt: the ROM
 * picks between UpdateMessage and this block on one data_0209f2d8 test, but
 * port_message_pump does not self-guard on the mode while this arm does, so
 * the pairing is half-implemented -- in a match the port runs both arms where
 * the DS runs one. */
void port_vs_countdown_tick(void)
{
    if (data_0209f2d8 != 1)
        return;
    int cnt = 0;
    /* run vs16: over data_0209fc50 -- what SetNumPlayers seated -- rather than
       four. The gate below already compares against that same number, so a
       census that stopped at four could never satisfy it above four players
       and the countdown would have hung forever at the fifth seat. */
    const int np_ready = (int)data_0209fc50 < kPortNarrowPlayers
                         ? kPortNarrowPlayers : (int)data_0209fc50;
    for (int i = 0; i < np_ready && i < kPortMaxPlayers; i++) {
        const char *p = (const char *)data_0209f394[i];
        if (p != 0 && *(const unsigned char *)(p + 0x711) != 0)
            cnt++;
    }
    if (cnt < data_0209fc50)
        return;
    if (data_0209f304 == 0x28 && data_0209f2bc == 3)
        func_02012790(0x2b);
    if (data_0209f2c4 != 0)
        return;
    if (data_0209f304 == 0 && data_0209f2bc == 0)
        return;
    data_0209f304 = (unsigned short)(data_0209f304 - data_0208ee44);
    if (data_0209f304 != 0 || data_0209f2bc == 0)
        return;
    data_0209f2bc -= 1;
    data_0209f304 = 0x28;
    if (data_0209f2bc != 0) {
        func_02012790(0x2b);
    } else {
        func_02012790(0x2a);
        fprintf(stderr, "[vsgo] countdown finished: Sound::Play2D(2, 0x2a) and "
                "Sound::LoadAndSetMusic_Layer1(0x4d), the arena's own track\n");
        g_course_music = 0x4d;
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x4d);
    }
}

// =============================================================================
// THE OTHER END OF THE SAME STATEMENT: what happens when the VS clock runs out
// =============================================================================
//
// port_vs_countdown_tick above is the HEAD of Stage::Behavior's VS block. This
// is its TAIL, hosted here for the identical reason and in the identical shape:
// the whole function is not in the port's link, and this is one statement of
// it calling the ROM's own functions.
//
// The statement, src/_ZN5Stage8BehaviorEv.cpp:173-186, verbatim:
//
//     if (data_ov002_02111188 == 0 && data_0209f204 != 0) {
//         if (data_0209fc68 == 0) {
//             ... the single-console VE_Init/VE_Update arm ...
//         } else {
//             Scene::StartSceneFade(7, 0, 0);
//             data_02092778 = 1;
//             data_0209d4b0 = 0;
//         }
//         Sound::StopLoadedMusic_Layer1(0x3c);
//     }
//
// data_0209f204 is the match clock's own "time is up" flag and HUD::UpdateVsTimer
// is what raises it -- the port hosts the HUD as a sub-screen actor
// (hal/sub_actors.cpp) and its Behavior slot dispatches UpdateVsTimer every
// frame, so the clock the port runs is the ROM's clock. data_ov002_02111188 is
// the same function's sub-counter, which UpdateVsTimer reloads to 0x3c at the
// moment it raises the flag: the ROM therefore waits SIXTY FRAMES after time-up
// before it asks for the results, and the wait is part of the statement, not
// padding this could skip.
//
// data_0209fc68 is the wireless session state. It is nonzero in every VS run
// this port can produce -- VS is a wireless mode and there is no offline VS on
// the cartridge -- so the else arm is the live one and VE_Init is not reachable
// from here. Scene 7 is the VS results screen: the same dScEntry_c class as the
// scene 6 lobby, told apart by its own id (src/func_ov075_0211a410.cpp branches
// on self->unk_00c == 6 and hands everything else to func_ov075_02116818, the
// results screen with the winner calculation behind it).
//
// ---- WHAT THE PORT CAN AND CANNOT DO WITH THAT REQUEST ----------------------
//
// Scene::StartSceneFade is matched src and the port links it: it parks the
// pending scene id in data_02092664 and writes the fade colour. What consumes a
// pending id is Scene::SpawnIfNecessary, and the port runs that ONLY on the
// scene path (hal/scene_boot.cpp's carrier inside port_scene_tick). On the LEVEL
// path -- which is where a VS arena runs -- there is no scene spawner, because
// crossing from a live level into a scene means tearing the Stage down first and
// port/stage_lifecycle_map.txt section 5 measures that teardown blocked on seven
// pieces. So the request is real, the ROM makes it, the port records it, and
// nothing consumes it. That is the honest state and this function reports it
// rather than papering over it.
//
// ---- AND IT IS WORSE THAN "NOTHING CONSUMES IT". MEASURED. ------------------
//
// Run R2 of this lane, both windows, from this tip with the request armed:
//
//     [vs] f2791 TIME UP: ... scores 3,0,0,0, stars taken 3
//       [vs] Scene::StartSceneFade(7, 0, 0): the VS RESULTS screen requested,
//            pending scene id now 7
//       [vs] f2792 pending scene id is now 7
//     [fx] f2820 ...
//     FATAL: Stage vtable slot 3 (CleanupResources) is not hosted
//
// Both processes, ~28 frames after the request, exit 127. Something DOES
// consume it, and what consumes it is the ROM's own Scene::BeforeBehavior,
// correctly seated in _ZTV5Stage slot 7 and dispatched on the Stage every
// frame. Its body -- quoted in full in hal/level_change.cpp's pending-scene
// block -- is: a pending id that is not the 0x187 sentinel means run the fader,
// and once the fader is AT THE END, ActorBase::MarkForDestruction(self). `self`
// is the Stage. So the ROM asks for the Stage to be torn down before the scene
// comes up, which is exactly right on the DS and is the one thing this port
// cannot do: port/stage_lifecycle_map.txt section 5 measures the teardown
// blocked on seven pieces, and slot 3 is the first of them.
//
// THAT IS THE SAME ABORT hal/level_change.cpp's port_scene_request_release was
// written to prevent -- port/exitlevel_seat.txt reproduces it, word for word
// and exit code for exit code, from a latched pending id on the level path.
// A ROM-faithful match-end request is simply another way to latch one.
//
// SO THE SCENE REQUEST IS OFF BY DEFAULT and the reason is not timidity. A
// default that turns "the match ends and both windows stand in the arena" into
// "the match ends and both windows abort" is a regression, whatever the ROM
// does. The switch stays because the next lane -- the one that hosts the Stage
// teardown and puts a scene carrier on the level path -- needs to be able to
// arm the request and watch where it stops, and this is that instrument.
//
// ---- THE ENVIRONMENT --------------------------------------------------------
//
//   SM64DS_VS_MATCH_END=0   the whole watcher off. Default ON: the time-up
//                           report and the ROM's own music stop are the ROM's
//                           behaviour at the ROM's own moment, and the marker
//                           costs a line in the flight recorder.
//   SM64DS_VS_END_SCENE=1   ALSO make the ROM's Scene::StartSceneFade(7,0,0)
//                           request. Default OFF. IT ABORTS THE PROCESS about
//                           thirty frames later, by the measurement above; it
//                           is a diagnostic for the lane that will fix that,
//                           not something to leave on.
//   SM64DS_VS_EXIT_ON_END=1 quit the process once the end state is reached.
//                           Default OFF -- see the note over the exit.
//   SM64DS_VS_END_GRACE=<n> frames between the time-up statement and the
//                           marker. Default 240 (four seconds at 60fps), which
//                           is eight times the ROM's own 0x3c wait and long
//                           enough for a fade plus a scene bring-up to be
//                           visible if one ever happens.
//   SM64DS_VS_STAR_TARGET=<n>  the SECOND win condition: first player to n
//                           stars ends the match through this same path.
//                           Unset or 0 is off and off is the pure cartridge
//                           behaviour. The launcher lobby sets it, so the
//                           variable is the interface; see the trigger's own
//                           block below.
//
// OUTSIDE VS THIS IS ONE COMPARE, A DISARM AND A RETURN. data_0209f2d8 != 1
// leaves before anything else is read and before any getenv; the disarm clears
// the end latch so a second match in one process would arm again, and its own
// branch is taken at most once per match. Measured: an adventure selftest is
// byte-identical to the pre-change build at a matched .dsstate base.
extern "C" {
extern unsigned char data_0209f204;      /* VS "time is up"                   */
extern unsigned short data_ov002_02111188; /* UpdateVsTimer's sub-counter     */
extern int           data_0209fc68;      /* wireless session state            */
extern unsigned char data_02092778;      /* Stage::Behavior sets this with the
                                            results request                   */
extern int           data_0209d4b0;      /* and clears this                   */
extern unsigned short data_02092664;     /* Scene::SetSceneToSpawn's pending id,
                                            0x187 = nothing pending           */
extern signed char   data_0209f310[];    /* the four players' VS star counts  */
signed char NumVsStarsObtained(void);
}
/* src/_ZN5Sound22StopLoadedMusic_Layer1Ej.cpp spells the C++ method out rather
   than an extern "C" face, so the MSVC-mangled name is what is in the link and
   no reverse bridge is needed -- hal/reverse_bridges.cpp's sound block names
   this function as its one exception for exactly that reason. Declaring the
   class here and calling the method is therefore how this TU reaches it. A
   member function's linkage is not affected by the enclosing extern "C". */
struct Sound { static void StopLoadedMusic_Layer1(unsigned int frames); };

/* Does the port ask for scene 7 by default? NO, and the measurement that says
   so is quoted in the block above: with it on, both windows abort on
   "Stage vtable slot 3 (CleanupResources) is not hosted" about thirty frames
   after the request. */
enum { PORT_VS_END_SCENE_DEFAULT = 0 };

/* The latch: the frame the end statement ran on, whether the marker has gone
   out, and which trigger fired. At file scope rather than inside the function
   so the mode test below can clear them, which is what re-arms this for a
   SECOND match in one process. Nothing produces one today -- going from a
   finished match back to the lobby is not something the port can do yet -- but
   a latch that silently swallows the second match is exactly the bug the lane
   that adds the lobby return would spend a day chasing.

   ---- AND THE RE-ARM IS A TRAP UNTIL SOMEBODY CLEARS THE SCORES -------------

   READ THIS BEFORE MAKING AN IN-PROCESS SECOND MATCH WORK. This latch re-arms,
   but data_0209f310 -- the ROM's per-player carried-star array, the thing every
   trigger and both markers read -- IS NEVER CLEARED IN THIS PORT. A second
   match in the same process would therefore start with the first match's
   scores: the star target would fire on frame one against stale values, and the
   marker would report a winner nobody played for.

   IT IS DORMANT TODAY AND THAT IS THE ONLY REASON IT IS A NOTE. The lobby's
   rematch spawns a FRESH PROCESS (C:	mp\lobbyd-out\SPEC.md section 5: the
   launcher waits for exit, then the room re-arms and the next match is a new
   launch), so nothing in the shipped flow reaches a second match in one
   process. The day something does, this is where it breaks.

   THE ROM DOES CLEAR IT, and the loop is already written -- in
   src/_ZN5Stage13InitResourcesEv.cpp:181-197, which this port does not run
   (port/stage_lifecycle_map.txt section 2d lists data_0209f310 among the
   per-player clears with seat "none"):

       if ((s32)data_0209f21c > 0) {
           ... for idx in 0 .. data_0209f21c-1:
                   if (sl || (v1 == 0 && f26c != 1)) data_0209f358[idx] = 0;
                   data_0209f30c[idx] = 0;
                   data_0209f310[idx] = 0;
       }

   NOT SEATED HERE, deliberately. It sits inside a guard three conditions deep
   (`b1 || data_0209f26c == 2 || v1 || temp_r4 == 0x1D || temp_r4 != temp_r0`,
   line 169) over state this file does not carry, and it clears two more arrays
   besides -- the coin counts and data_0209f30c. Hosting a partial copy of a
   guarded ROM block on a path nobody can exercise is how a wrong clear gets
   shipped and stays invisible; the honest move is to name the block, the file
   and the line so the lane that needs it seats the whole thing with its guard.
   That lane also owns deciding whether the port's f21c is the count the ROM
   means here. */
static int g_end_fired = -1;
static int g_end_announced;
static int g_end_by_target;
/* THE SCORES AS THEY WERE AT THE MOMENT THE MATCH WAS WON, latched at the
   trigger and reported by the marker. Without this the marker reports whatever
   the array holds `grace` frames LATER, which on a first-to-2 match measured
   3,0,0,0 -- the third star landed inside the grace window. The launcher reads
   the marker, so the marker has to carry the scores the match was decided on. */
/* run vs16: sixteen. g_vs_players below is how many of them a given match
   actually has, so nothing here has to guess. */
static int g_end_scores[kPortMaxPlayers];

/* HOW MANY PEOPLE ARE IN THIS MATCH, read once from the same knob that decides
   the wire and the names grammar (hal/comms_loopback.cpp, THE TWO WIRES). One
   number, one source: if this disagreed with the wire the scoreboard would
   describe a different match than the one being played. Absent or out of
   range is FOUR, which is what every existing deployment and every existing
   proof gets. */
static int g_vs_players_cache;
static int vs_players(void)
{
    if (g_vs_players_cache) return g_vs_players_cache;
    g_vs_players_cache = kPortNarrowPlayers;
    if (const char *e = std::getenv("SM64DS_VS_PLAYERS")) {
        const int v = std::atoi(e);
        /* FLOORED AT FOUR, and the floor is the whole reason this lane can
           claim a two- or four-player match is unchanged.

           Every loop below that used to be a literal `< 4` now runs to this
           number. Without the floor, a two-player session would run those loops
           to TWO -- which happens to produce the same answers today (the slots
           it skips have no actor and a zero score) but is a different amount of
           work over different memory, and "it comes out the same" is not the
           same claim as "it does the same thing". With the floor, four or fewer
           players is arithmetically identical to the code before this lane, and
           only a fifth player changes anything. */
        if (v > kPortNarrowPlayers && v <= kPortMaxPlayers)
            g_vs_players_cache = v;
    }
    return g_vs_players_cache;
}
static int g_end_total;

/* THE PLAY HOLD. Nonzero once the match is over, which is what
   port_vs_match_end_hold below reads to stop the pads and what the harness's
   own collect trigger reads to stop scoring. */
extern "C" int port_vs_match_end_frozen(void) { return g_end_fired >= 0; }

/* ---- THE FREEZE ------------------------------------------------------------
 *
 * Tango: first-to-N must stop play AT the target. Two halves, and only the
 * first is load-bearing for the marker:
 *
 *   THE SCORES ARE LATCHED at the trigger (above), so the marker reports the
 *   moment the match was decided whatever happens afterwards. That alone kills
 *   the 3,0,0,0 quirk and it cannot fail.
 *
 *   THE PADS ARE HELD from the frame after, so nothing can be played. All four
 *   slots, every button and stick word the ROM's readers use -- the same seven
 *   fields walk_window's own comms fan-out writes. With no input no player can
 *   swing at a ball, so no star can be released and none collected: "no
 *   scoring" falls out of "no play" rather than being a second mechanism.
 *
 * WHY NOT SKIP THE TICK, which is the debug menu's freeze and the obvious move.
 * The menu's `game_ticked = 0` skips the whole else-branch of the level loop,
 * and the comms exchange lives inside it. Two windows that stop exchanging --
 * even in perfect step -- are two windows whose transport is no longer being
 * pumped, and this lane is not going to find out what that does to a live
 * session at the one moment the match is trying to end cleanly. Holding the
 * pads leaves every frame doing exactly what it did, minus the input.
 *
 * LOCKSTEP-SAFE BY CONSTRUCTION: the hold is derived from the end latch, the
 * end latch is set by a test on state both consoles share, and both measured
 * setting it on the same frame. Neither window is told anything by the other.
 *
 * SM64DS_VS_END_FREEZE=0 leaves the pads live and keeps only the latch. */
extern "C" {
extern char data_0209f49c[];   /* held        */
extern char data_0209f49e[];   /* pressed     */
extern char data_0209f4a0[];   /* stick / etc */
extern char data_0209f4a2[];
extern char data_0209f4a4[];
extern char data_0209f4a6[];
extern unsigned char data_0209f4ac[];  /* touching */
}
/* ---- THE WINNER, SHOWN ------------------------------------------------------
 *
 * Tango: "when the match is over and it says who wins and would normally end it
 * closes and goes to the lobby." The launcher is the lobby until Stage 1 builds
 * a real one, so the port's half is: say who won, then close.
 *
 * THIS IS THE FALLBACK AND IT SAYS SO. The real answer is the ROM's own results
 * screen -- scene 7, which boots and renders and whose own exit asks for the
 * lobby -- and this lane measured exactly why it cannot be reached from inside a
 * match (the block over port_vs_match_end_poll). Rather than half-fake it with
 * ROM sprites, the port says who won in the port's own voice, through the host
 * overlay, where nobody can mistake it for the cartridge's results screen.
 *
 * THE WINNER IS COMPUTED FROM THE LATCHED SCORES, not from the ROM's own
 * func_ov075_021165b0. That function is linked and it is the right one, and it
 * takes the results SCENE as its receiver -- there is no scene here to give it.
 * Most stars wins; equal top scores are reported as a draw rather than broken
 * by an invented rule.
 *
 * Text only, one line, and the caller draws it: hal/ has no framebuffer. */
/* ---- SM64DS_VS_NAMES: the lobby's nicknames, for the winner banner ---------
 *
 * Tango: "make the winning thing at the end say the nickname of who won." The
 * lobby server builds the string; this reads it. The contract is
 * C:	mp\lobbyd-out\SPEC.md section 4.7 and every rule below is theirs, not
 * this file's invention:
 *
 *     four fields, comma separated, EXACTLY three commas, slot order 0..3
 *     whole variable 1..67 bytes (4 x 16 + 3)
 *     each field 0..16 bytes
 *     bytes 0x20..0x7E only; comma is the separator and barred from a name
 *     an empty field means that slot has no name
 *
 * IGNORED WHOLESALE ON ANY VIOLATION, which is the spec's word and the rule
 * that protects every existing proof: a malformed variable is not partially
 * salvaged, it is dropped, and the banner falls back to PLAYER n+1. show.sh and
 * every proof tool never set it, so they keep producing exactly the banner they
 * produce today.
 *
 * THE GAME DOES NOT TRUST THE ENVIRONMENT even though the server sanitizes and
 * the launcher re-validates. This is a string arriving from another process,
 * built from text a stranger typed into a lobby; it is checked here as if
 * neither of those steps existed. Read once, cached, never written back.
 *
 * The 3-4 player caveat is the spec's and is not this reader's to solve: field
 * n is slot n, and above two players the parent assigns child slots from JOIN
 * arrival order, which need not match lobby seat order. GAME_MAX_PLAYERS is 2
 * today and the mapping is a fact; the lane that raises it inherits 4.7's
 * paragraph on it. */
/* RUN vs16: TWO LEGAL SHAPES, AND THE SAME NUMBER DECIDES BOTH THE SHAPE AND
 * THE WIRE.
 *
 * The coordinator's cross-lane ruling, carried in
 * port/tools/lobby/README.md section 3:
 *
 *     NAMES + COLORS exactly-4 today, with the same per-field grammar
 *     (exactly 15 commas); future = 16 fields together in one coordinated
 *     version change when the wire moves; never independently.
 *
 * The wire has now moved (hal/comms_loopback.cpp, THE TWO WIRES), so this IS
 * that coordinated change, and it lands in one commit set with the colours
 * reader in hal/fs_mods.cpp, the lobby's build_names/build_colors and the
 * launcher's two validators. Not one of the five moved on its own.
 *
 * THE GRAMMAR: exactly 3 commas (four fields, a narrow session) OR exactly 15
 * commas (sixteen fields, a wide one). Nothing between, nothing else. That
 * keeps the property the ruling exists to protect -- one variant cannot be
 * mis-parsed as the other, because the comma count settles it in one pass with
 * no count prefix for two readers to disagree about.
 *
 * A FOUR-FIELD STRING IS READ EXACTLY AS IT ALWAYS WAS, down to the log line,
 * so every existing proof and every 2P/4P match is untouched. */
static char g_vs_names[kPortMaxPlayers][17];
static int  g_vs_names_read;
static int  g_vs_names_fields;   /* 4 or 16, whichever the string carried */

static void vs_names_load(void)
{
    if (g_vs_names_read)
        return;
    g_vs_names_read = 1;
    g_vs_names_fields = kPortNarrowPlayers;
    for (int i = 0; i < kPortMaxPlayers; ++i) g_vs_names[i][0] = 0;

    const char *e = std::getenv("SM64DS_VS_NAMES");
    if (!e)
        return;                      /* absent: the common case, and silent */

    const size_t len = std::strlen(e);
    /* 16 x 16 + 15 = 271 at the wide shape, 67 at the narrow one. The cap is
       the wide one because the COMMA COUNT below is what decides the shape,
       and a length check that rejected a legal wide string before reaching it
       would be a second opinion about the grammar. */
    if (len < 1 || len > 271) {
        fprintf(stderr, "[vs] SM64DS_VS_NAMES ignored: %u bytes, the contract "
                "allows 1..67 (four fields) or 1..271 (sixteen)\n",
                (unsigned)len);
        return;
    }
    int commas = 0;
    for (size_t i = 0; i < len; ++i) {
        const unsigned char c = (unsigned char)e[i];
        if (c == ',') { ++commas; continue; }
        if (c < 0x20 || c > 0x7E) {
            fprintf(stderr, "[vs] SM64DS_VS_NAMES ignored: byte %02x at %u is "
                    "outside 0x20..0x7E\n", (unsigned)c, (unsigned)i);
            return;
        }
    }
    if (commas == kPortNarrowPlayers - 1) {
        g_vs_names_fields = kPortNarrowPlayers;
    } else if (commas == kPortMaxPlayers - 1) {
        g_vs_names_fields = kPortMaxPlayers;
    } else {
        fprintf(stderr, "[vs] SM64DS_VS_NAMES ignored: %d comma(s), the "
                "contract requires exactly 3 (four fields) or exactly 15 "
                "(sixteen)\n", commas);
        return;
    }
    const int nf = g_vs_names_fields;
    /* Split. Every field is already known to be printable and comma-free. */
    char tmp[kPortMaxPlayers][17];
    int slot = 0, w = 0;
    for (size_t i = 0; i <= len; ++i) {
        const char c = (i < len) ? e[i] : ',';
        if (c == ',') {
            if (slot < nf) { tmp[slot][w] = 0; }
            ++slot; w = 0;
            if (slot > nf) break;
            continue;
        }
        if (w >= 16) {
            fprintf(stderr, "[vs] SM64DS_VS_NAMES ignored: field %d is longer "
                    "than 16 bytes\n", slot);
            return;
        }
        if (slot < nf) tmp[slot][w] = c;
        ++w;
    }
    for (int i = 0; i < nf; ++i) {
        /* the spec says upstream already stripped edge spaces and says the
           reader may trim again harmlessly; harmless is worth having */
        char *b = tmp[i];
        size_t a = 0, z = std::strlen(b);
        while (a < z && b[a] == ' ') ++a;
        while (z > a && b[z - 1] == ' ') --z;
        const size_t m = z - a;
        std::memcpy(g_vs_names[i], b + a, m);
        g_vs_names[i][m] = 0;
    }
    if (nf == kPortNarrowPlayers) {
        /* the four-field line, unchanged to the byte, because every existing
           proof greps for exactly this shape */
        fprintf(stderr, "[vs] SM64DS_VS_NAMES accepted: [%s] [%s] [%s] [%s]\n",
                g_vs_names[0], g_vs_names[1], g_vs_names[2], g_vs_names[3]);
    } else {
        fprintf(stderr, "[vs] SM64DS_VS_NAMES accepted, %d fields:", nf);
        for (int i = 0; i < nf; ++i) fprintf(stderr, " [%s]", g_vs_names[i]);
        fprintf(stderr, "\n");
    }
}

/* The name for a slot, or 0 when that slot has none (empty field, or the whole
   variable was rejected). Callers fall back to PLAYER n+1. */
static const char *vs_name_for(int slot)
{
    if (slot < 0 || slot >= kPortMaxPlayers)
        return 0;
    vs_names_load();
    /* A slot past what the string carried has no name, the same answer an
       empty field gives -- the caller falls back to PLAYER n+1. */
    if (slot >= g_vs_names_fields)
        return 0;
    return g_vs_names[slot][0] ? g_vs_names[slot] : 0;
}

/* ---- WHAT THE NAME TAGS READ ----------------------------------------------
 *
 * tests/nametag.h draws a tag over every REMOTE player's head, and the two
 * things it puts in the tag are this file's already: the lobby nickname for a
 * seat and that seat's carried-star count. Rather than let the window grow a
 * second reader of SM64DS_VS_NAMES and a second opinion about the star band,
 * the two facts leave here through one face each.
 *
 * THE SLOT IS THE FIELD INDEX, and that is the SAME rule the end banner
 * already follows (port_vs_match_end_banner calls vs_name_for(best) with a
 * data_0209f310 index). It carries the same caveat vs_names_load's banner
 * states in the lobby spec's words: above two players the parent assigns child
 * slots from JOIN arrival order, which need not match lobby seat order. That
 * is a property of the lobby contract, not of this reader, and a tag that
 * disagreed with the winner banner would be worse than one that shares its
 * caveat -- so it shares it, deliberately, and both move on the day the lobby
 * sends a seat map.
 *
 * NEITHER FACE WRITES ANYTHING. */
extern "C" const char *port_vs_slot_name(int slot)
{
    return vs_name_for(slot);
}

/* That slot's carried VS stars. The read is data_0209f310[slot] -- the same
 * expression the end latch above uses, over the same guarded 32-byte band
 * (hal/actor_classes_star.cpp's .dsstate$hvsstar pair, checked by
 * port/tools/gxband_guard.py), so sixteen slots are inside the storage the
 * port already has. Out of range answers 0 rather than reading past it. */
extern "C" int port_vs_slot_stars(int slot)
{
    if (slot < 0 || slot >= kPortMaxPlayers)
        return 0;
    return (int)data_0209f310[slot];
}


/* ---- THE SCOREBOARD LINE, at four players and at sixteen -------------------
 *
 * THE MECHANICAL PART IS EXACT AND THE DISPLAY IS DELIBERATELY BASIC, which is
 * the owner's standing rule for UI a lane was not asked to design. The winner,
 * the draw test and the marker below all run over every player in the match.
 * What the BANNER shows is a layout question, and a layout question belongs to
 * the owner's eyes, not to this file's judgement.
 *
 * AT FOUR OR FEWER: exactly "%d - %d - %d - %d", four numbers, always, even at
 * two players. Byte-identical to what this banner has printed since VSEND, so
 * every capture, every proof and the launcher's own marker parser see what
 * they have always seen.
 *
 * ABOVE FOUR: sixteen scores in one line at this font is a wall of digits, and
 * the honest simple thing is to show the two that a player actually wants --
 * the leader's and his own -- plus how many people are playing. So the default
 * is:
 *
 *     MATCH OVER  -  tango WINS  9   you 4   (16 players)
 *
 * TWO ALTERNATIVES ARE BUILT AND SELECTABLE so the owner can look at all three
 * rather than at a description of them. SM64DS_VS_BANNER picks:
 *
 *     basic  (default)  leader + you + the count, as above
 *     top4              the four highest scores, "9 - 7 - 4 - 2  (16 players)"
 *     all               every score in slot order, comma separated
 *
 * None of the three is chosen here as the right one. Captures of all three are
 * banked for the owner in C:\tmp\vs16-out; the default is `basic` only because
 * it is the least.
 * ------------------------------------------------------------------------- */
static int vs_banner_mode(void)
{
    static int m = -1;
    if (m >= 0) return m;
    m = 0;                                   /* basic */
    if (const char *e = std::getenv("SM64DS_VS_BANNER")) {
        if (std::strcmp(e, "top4") == 0)     m = 1;
        else if (std::strcmp(e, "all") == 0) m = 2;
    }
    return m;
}

static int vs_score_list(char *out, int cap, int np, int best)
{
    if (cap <= 1) return 0;
    if (np <= kPortNarrowPlayers) {
        return std::snprintf(out, (size_t)cap, "%d - %d - %d - %d",
                             g_end_scores[0], g_end_scores[1],
                             g_end_scores[2], g_end_scores[3]);
    }
    const int me = (int)data_0209f250;
    switch (vs_banner_mode()) {
    case 2: {                                /* all */
        /* snprintf RETURNS THE LENGTH IT WANTED, not the length it wrote, so
           an accumulating `w += snprintf(...)` walks PAST the buffer the moment
           one call truncates -- and the next iteration then writes at
           out + w, outside it. The loop guard alone does not save it: it is
           checked before the call that overruns, not after. So clamp on every
           step. Sixteen scores at up to two digits plus commas is 47 bytes
           against a caller that guarantees at least 64, so this cannot fire
           today; it is here because "cannot fire today" is a property of the
           caller and this function does not get to assume it. */
        int w = 0;
        for (int i = 0; i < np && w < cap - 1; ++i) {
            const int k = std::snprintf(out + w, (size_t)(cap - w), "%s%d",
                                        i ? "," : "", g_end_scores[i]);
            if (k < 0) break;
            w += k;
            if (w > cap - 1) { w = cap - 1; break; }
        }
        return w;
    }
    case 1: {                                /* top4 */
        /* the four highest, by value, without sorting the array itself --
           selection over a copy of the indices, sixteen elements, four passes */
        int pick[kPortNarrowPlayers];
        int used = 0;
        for (int k = 0; k < kPortNarrowPlayers; ++k) {
            int bi = -1;
            for (int i = 0; i < np; ++i) {
                if (used & (1 << i)) continue;
                if (bi < 0 || g_end_scores[i] > g_end_scores[bi]) bi = i;
            }
            if (bi < 0) { pick[k] = 0; continue; }
            used |= 1 << bi;
            pick[k] = g_end_scores[bi];
        }
        return std::snprintf(out, (size_t)cap, "%d - %d - %d - %d  (%d players)",
                             pick[0], pick[1], pick[2], pick[3], np);
    }
    default: {                               /* basic */
        const int mine = (me >= 0 && me < np) ? g_end_scores[me] : 0;
        if (me == best)
            return std::snprintf(out, (size_t)cap, "%d   (%d players)",
                                 g_end_scores[best], np);
        return std::snprintf(out, (size_t)cap, "%d   you %d   (%d players)",
                             g_end_scores[best], mine, np);
    }
    }
}

extern "C" int port_vs_match_end_banner(char *out, int n)
{
    /* SM64DS_VS_END_BANNER=0 keeps the end flow and drops the presentation, so
       a capture pair can isolate exactly this line's pixels with the freeze,
       the latch and the close all still running. */
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_VS_END_BANNER");
        on = (e && e[0] == '0') ? 0 : 1;
    }
    if (!on || g_end_fired < 0 || data_0209f2d8 != 1 || n < 64)
        return 0;
    /* THE MECHANICAL HALF IS EXACT AT EVERY N. The winner, the draw test and
       the score list all run over the players who are actually in the match,
       not over four. What the DISPLAY does with sixteen scores is a separate
       question and is answered below. */
    const int np = vs_players();
    int best = 0, ties = 0;
    for (int i = 1; i < np; ++i)
        if (g_end_scores[i] > g_end_scores[best]) best = i;
    for (int i = 0; i < np; ++i)
        if (i != best && g_end_scores[i] == g_end_scores[best]) ++ties;
    if (ties) {
        int w = std::snprintf(out, (size_t)n, "MATCH OVER  -  DRAW  ");
        if (w < 0) w = 0;
        if (w > n - 1) w = n - 1;
        vs_score_list(out + w, n - w, np, best);
    } else {
        /* the nickname if the lobby gave us one for THIS slot, the ROM-shaped
           PLAYER n+1 otherwise -- per slot, because a room can have one named
           seat and one unnamed */
        const char *nick = vs_name_for(best);
        char who[24];
        if (nick)
            std::snprintf(who, sizeof who, "%s", nick);
        else
            std::snprintf(who, sizeof who, "PLAYER %d", best + 1);
        int w = std::snprintf(out, (size_t)n, "MATCH OVER  -  %s WINS  ", who);
        if (w < 0) w = 0;
        if (w > n - 1) w = n - 1;
        vs_score_list(out + w, n - w, np, best);
    }
    return 1;
}

extern "C" void port_vs_match_end_hold(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_VS_END_FREEZE");
        on = (e && e[0] == '0') ? 0 : 1;
    }
    if (!on || g_end_fired < 0 || data_0209f2d8 != 1)
        return;
    for (int i = 0; i < vs_players(); ++i) {
        const int o = i * 0x18;
        *(short *)(data_0209f49c + o) = 0;
        *(short *)(data_0209f49e + o) = 0;
        *(short *)(data_0209f4a0 + o) = 0;
        *(short *)(data_0209f4a2 + o) = 0;
        *(short *)(data_0209f4a4 + o) = 0;
        *(short *)(data_0209f4a6 + o) = 0;
        data_0209f4ac[o] = 0;
    }
}

extern "C" int port_vs_match_end_poll(int frame)
{
    /* Outside VS: nothing but the disarm, and the disarm is a branch taken at
       most once per match. No getenv on this path, ever. */
    if (data_0209f2d8 != 1) {
        if (g_end_fired >= 0) {
            g_end_fired = -1;
            g_end_announced = 0;
            g_end_by_target = 0;
        }
        return 0;
    }

    static int on = -1, want_scene = -1, want_exit = -1, grace = -1,
               star_target = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_VS_MATCH_END");
        on = (e && e[0] == '0') ? 0 : 1;
        e = std::getenv("SM64DS_VS_END_SCENE");
        want_scene = e ? (e[0] != '0') : (int)PORT_VS_END_SCENE_DEFAULT;
        e = std::getenv("SM64DS_VS_EXIT_ON_END");
        want_exit = (e && e[0] == '0') ? 0 : 1;
        e = std::getenv("SM64DS_VS_END_GRACE");
        grace = e ? std::atoi(e) : 240;
        if (grace < 0) grace = 0;
        e = std::getenv("SM64DS_VS_STAR_TARGET");
        star_target = e ? std::atoi(e) : 0;
        if (star_target < 0) star_target = 0;
        if (star_target > 0)
            fprintf(stderr, "[vs] win condition: FIRST TO %d STAR(S). The ROM's "
                    "own condition -- most stars when the clock runs out -- "
                    "still stands underneath and whichever happens first ends "
                    "the match (SM64DS_VS_STAR_TARGET)\n", star_target);
    }
    if (!on)
        return 0;

    if (g_end_fired < 0) {
        /* ---- TRIGGER ONE: the ROM's own. Stage::Behavior's guard, both
           halves, in its own order. */
        const int timeup = (data_ov002_02111188 == 0 && data_0209f204 != 0);
        /* ---- TRIGGER TWO: FIRST TO N STARS, the host-layer alternative.
         *
         * The owner's ruling: online VS is a host-layer opt-in and already a
         * mod, so the win condition is selectable -- the cartridge's "most
         * stars when time runs out", or "first to X stars". This is the second
         * one, and it is a WATCHER, never an editor. It reads data_0209f310,
         * the ROM's own per-player carried-star array that GiveVsStars writes
         * and NumVsStarsObtained sums, and it writes nothing anywhere: the
         * ROM's five-star final-star behaviour underneath is untouched and so
         * is the clock.
         *
         * UNSET OR 0 IS OFF, and off means the pure ROM behaviour -- one
         * compare against a constant per frame and nothing else. The launcher
         * lobby will set the variable, so the env IS the interface.
         *
         * BOTH TRIGGERS FEED ONE END PATH on purpose. Whichever fires first
         * runs the same statement, prints the same marker and takes the same
         * exit, so there is one match-over mechanism to reason about and not
         * two that drift. */
        int star_winner = -1;
        const int np_end = vs_players();
        if (star_target > 0) {
            for (int i = 0; i < np_end; ++i)
                if ((int)data_0209f310[i] >= star_target) { star_winner = i; break; }
        }
        if (!timeup && star_winner < 0)
            return 0;
        g_end_fired = frame;
        g_end_by_target = (star_winner >= 0);
        /* run vs16: latch every player's score. data_0209f310 is hosted 32
           bytes wide as a guarded contiguous band (hal/actor_classes_star.cpp),
           so sixteen is inside the storage the port already had -- this loop
           was the only thing stopping at four. */
        for (int i = 0; i < np_end; ++i) g_end_scores[i] = (int)data_0209f310[i];
        g_end_total = (int)NumVsStarsObtained();
        if (star_winner >= 0)
            fprintf(stderr, "[vs] f%d TARGET REACHED: player %d has %d star(s), "
                    "the target is %d (wireless state=%d, scores %d,%d,%d,%d, "
                    "stars taken %d)\n", frame, star_winner,
                    (int)data_0209f310[star_winner], star_target, data_0209fc68,
                    (int)data_0209f310[0], (int)data_0209f310[1],
                    (int)data_0209f310[2], (int)data_0209f310[3],
                    (int)NumVsStarsObtained());
        else
            fprintf(stderr, "[vs] f%d TIME UP: the match clock reached zero and "
                    "Stage::Behavior's own end-of-match guard is open "
                    "(data_0209f204=%u, sub-counter=%u, wireless state=%d, "
                    "scores %d,%d,%d,%d, stars taken %d)\n",
                    frame, (unsigned)data_0209f204,
                    (unsigned)data_ov002_02111188, data_0209fc68,
                    (int)data_0209f310[0], (int)data_0209f310[1],
                    (int)data_0209f310[2], (int)data_0209f310[3],
                    (int)NumVsStarsObtained());

        if (data_0209fc68 == 0) {
            /* The single-console arm. Not reachable from a VS arena on this
               port -- VS is a wireless mode and the session state is nonzero
               whenever one is up -- so it is REPORTED rather than hosted, the
               way port_vs_countdown_tick records the two preconditions it does
               not carry. Hosting VE_Init/VE_Update off a path that cannot be
               reached would be untested code with no way to test it. */
            fprintf(stderr, "  [vs] wireless session state is 0, so the ROM "
                    "would take the VE_Init arm here, not the results screen. "
                    "That arm is not hosted; nothing requested.\n");
        } else if (want_scene) {
            /* THE ROM'S OWN THREE STATEMENTS, in the ROM's own order. */
            _ZN5Scene14StartSceneFadeEjjt(7, 0, 0);
            data_02092778 = 1;
            data_0209d4b0 = 0;
            fprintf(stderr, "  [vs] Scene::StartSceneFade(7, 0, 0): the VS "
                    "RESULTS screen requested, pending scene id now %u\n",
                    (unsigned)data_02092664);
        } else {
            fprintf(stderr, "  [vs] the results-screen request is NOT made "
                    "(SM64DS_VS_END_SCENE is off, which is the default): the "
                    "port cannot tear the Stage down, so asking would abort "
                    "the process. Pending scene id left at %u.\n",
                    (unsigned)data_02092664);
        }
        /* Outside the if/else on the ROM too -- both arms stop the music. */
        Sound::StopLoadedMusic_Layer1(0x3c);
        return 0;
    }

    if (g_end_announced)
        return 0;
    if (frame - g_end_fired < grace) {
        /* Report what the request is doing while the grace runs, on change
           only, so a scene that DOES come up leaves a trail and one that never
           does leaves one line. */
        static unsigned last_pending = 0xffff;
        if (data_02092664 != last_pending) {
            last_pending = data_02092664;
            fprintf(stderr, "  [vs] f%d pending scene id is now %u%s\n", frame,
                    (unsigned)data_02092664,
                    data_02092664 == 0x187 ? " (the nothing-pending sentinel)"
                                           : "");
        }
        return 0;
    }

    g_end_announced = 1;
    /* THE MARKER. One line, fixed shape, in the flight recorder, so a launcher
       can watch for it without parsing the rest of a playlog. The scores are
       the ROM's own per-player array and the total is the ROM's own sum, and
       `win=` says which of the two conditions ended it -- the launcher that
       chose the condition is the thing that wants to know it was honoured. */
    /* ---- v2 OF THIS LINE, AND THE APPEND IS THE CONTRACT --------------------
     *
     * The lobby spec (section 5) says the launcher parses ONLY `win=` and
     * `scores=` and bounds them, so new fields are safe -- but only while they
     * are APPENDED. Nothing before the end moves, nothing is renamed, nothing
     * changes shape, so a reader written against v1 keeps working untouched.
     * That is what makes this additive rather than a version the launcher has
     * to chase.
     *
     * v2 appends, in this order:
     *   winner=<0..3|-1>    the winning slot, -1 on a draw. Same numbering as
     *                       scores= and as SM64DS_VS_NAMES field order.
     *   winner_name=<s>     ONLY when the lobby named that slot; omitted
     *                       entirely otherwise, so an unnamed match's marker is
     *                       v1 plus winner= and nothing else.
     *
     * The name is printed raw because it has already passed the 4.7 grammar
     * check on read: printable ASCII, no comma, at most 16 bytes. It therefore
     * cannot break the field grammar of this line. */
    const int np_marker = vs_players();
    int win_slot = -1;
    {
        int b = 0, t = 0;
        for (int i = 1; i < np_marker; ++i)
            if (g_end_scores[i] > g_end_scores[b]) b = i;
        for (int i = 0; i < np_marker; ++i)
            if (i != b && g_end_scores[i] == g_end_scores[b]) ++t;
        if (!t) win_slot = b;
    }
    /* THE MARKER'S SCORE FIELD CARRIES EVERY PLAYER, always, whatever the
       banner chose to draw -- it is the machine-readable record the launcher
       and the crash pipeline parse, and it must not be a summary. At four or
       fewer it is the exact four-number field it has always been, so the
       shipped launcher's parser is untouched; above four it is np numbers in
       the same comma-separated shape, which is the one extension a parser
       written for "split on comma" reads for free. */
    char scores_field[kPortMaxPlayers * 6];
    {
        int w = 0;
        const int cap = (int)sizeof scores_field;
        const int nprint = np_marker < kPortNarrowPlayers
                           ? kPortNarrowPlayers : np_marker;
        for (int i = 0; i < nprint && w < cap - 1; ++i) {
            const int k = std::snprintf(scores_field + w, (size_t)(cap - w),
                                        "%s%d", i ? "," : "",
                                        g_end_scores[i]);
            if (k < 0) break;
            w += k;
            if (w > cap - 1) { w = cap - 1; break; }
        }
    }
    const char *win_name = (win_slot >= 0) ? vs_name_for(win_slot) : 0;
    fprintf(stderr, "[vs] MATCH OVER f%d win=%s scores=%s players=%d total=%d "
            "pending_scene=%u results_screen=%s winner=%d%s%s\n", frame,
            g_end_by_target ? "star-target" : "time-up",
            scores_field, np_marker,
            g_end_total, (unsigned)data_02092664,
            data_02092664 == 7 ? "REQUESTED-BUT-UNSERVICED"
                               : (data_02092664 == 0x187 ? "none" : "other"),
            win_slot, win_name ? " winner_name=" : "", win_name ? win_name : "");
    fflush(stderr);

    /* THE EXIT IS ON BY DEFAULT NOW, and round 1 had it the other way round.
       That was this lane's choice and the owner has overruled it: "when the
       match is over and it says who wins and would normally end it closes and
       goes to the lobby." The launcher is the lobby until there is a real one,
       so closing IS going to the lobby, and the objection round 1 raised -- a
       window vanishing the instant the clock runs out -- is answered by the
       thing that changed with it: the winner is on screen for the whole grace
       window before this runs. Nobody's window disappears without being told
       who won. SM64DS_VS_EXIT_ON_END=0 still opts out. */
    if (!want_exit)
        return 0;
    fprintf(stderr, "  [vs] closing: the winner has been on screen for the "
            "whole grace window, so the match ends the way the owner asked -- "
            "out through the window's own WM_QUIT path, exit code 0. "
            "SM64DS_VS_EXIT_ON_END=0 opts out.\n");
    fflush(stderr);
    return 1;
}

/* Seat what the course loop reads that the boot does NOT: the player globals
   (SetPlayerGlobals, which InitResources does not call -- the handoff and the
   title path do). The sound row moved into the boot (port_boot_course_sound),
   so this no longer seats it. Called once from the harness after the boot.

   ONCE PER SESSION IS CORRECT, AND THE ONE-SHOT IS NOT A RESPAWN BUG. Read the
   relocs before "fixing" this: SetPlayerGlobals (0x0202acfc) has three callers
   in the ROM -- StartFile (0x0202ae88), PrepareVsMode and the ov003
   title-confirm path func_ov003_020ad814 -- plus two the port adds, this one
   and hal/level_change.cpp:1415 (the port's copy of that ov003 path). It is
   the NEW FILE seat: lives to 4, health to 0x880 for all four players. This
   call stands in for StartFile's, so it belongs exactly where it is.

   Two of the ROM's three DO enter a level -- StartFile and func_ov003_020ad814
   both call LoadLevelNoReturn immediately before it, so do not repeat the
   earlier claim here that "none is a level entry". The distinction that
   matters is narrower and is the whole reason this stays a one-shot: none of
   them is on the PER-ENTRY OR RESPAWN path. They run when a file is started,
   never when a running file re-enters a level.

   Re-running it per entry would be WORSE than the bug it looks like it causes:
   it would restore lives to 4 on every respawn, so HitDeathPlane's
   `data_0209f2f4[0] != 0` game-over branch below could never be taken.

   The ROM's PER-ENTRY health restore is a different mechanism entirely and does
   not pass through here: the Player's own level-enter step heals to full when
   the latched entry reason says fresh-or-death (src/func_ov002_020c75f0.c:29),
   and the boot latches that reason -- see the data_0209f2fc seat in
   hal/level_boot.cpp. TRIAGE14 read this one-shot as the cause of the infinite
   death loop; the cause was the missing latch, and this note is here so the
   next reader does not walk the same path. */
void port_course_seat(void)
{
    if (g_seated) return;
    g_seated = 1;
    seat_player_globals();
}

// =============================================================================
// COINS -- and the seam with the coin actor
// =============================================================================
//
// THE SEAM IS Actor::GivePlayerCoins, which is Nintendo's own and is now in
// the link (slice_gate35.txt). It is the single point where a coin actor
// stops being a coin actor and becomes a number:
//
//     void Actor::GivePlayerCoins(Player &p, u8 count, u32 kind)
//       -> Sound::PlayBank3(0x11 or 0x12, this->pos)     the chime
//       -> GiveCoins(p.mPlayerNo, count * data_02075238[kind])
//       -> Player::Heal(&p, amount << 8)                 coins heal
//       -> Actor::Spawn(data_02075230[kind], ...)        the count popup
//
// A coin class calls it from its own Behavior on the frame its collider
// reports the player, with `kind` 0 for yellow, 1 for red and 2 for blue.
// Nothing else is needed on the coin side and nothing here reaches into the
// coin: the ROM function is the whole contract.
//
// port_give_player_coins is the same call with a C name, for callers that
// have not got an Actor in hand (the probe below, and any host-side collect).

void port_give_player_coins(void *actor, void *player, int count, int kind)
{
    _ZN5Actor15GivePlayerCoinsER6Playerhj(actor, player,
                                          (unsigned char)count,
                                          (unsigned int)kind);
}

// =============================================================================
// THE STAR
// =============================================================================
//
// port_star_collect is the level-independent half of func_ov002_020e8ef0
// (PowerStar's own collect handler) followed by func_ov002_020c94a4's tail.
// Both are matched src; neither can be CALLED here, because the first wants a
// live PowerStar actor and the second a Player already in the star-get state,
// and the PowerStar class is not registered yet -- its InitResources is 0x820
// bytes of star-model, silver-star and red-coin-star loading, and the castle
// grounds names no star to hang it on.
//
// So this is the bookkeeping and the handoff, written out, and it is the same
// sequence in the same order:
//
//   Event 0x1e   "a star has been got this session". func_ov002_020e930c
//                returns early on it, which is what stops a second star from
//                starting a second cutscene.
//   Event 0x1d   the HUD's own bit: HUD::UpdateHealthMeter forces the health
//                meter into state 5 (retract) while it is set.
//   data_0209f228 / data_0209f2ac   which star, and whether it was NEW -- the
//                two words the star-select screen reads back on the way out.
//   CollectStarInCurLevel           the save bit.
//   func_02012790(0x2d)             the fanfare, Sound::Play2D(2, 0x2d).
//   ExitLevel()                     SetNextLevel(1) + StartExitCharacterWipe,
//                                   which is the course-clear handoff itself:
//                                   next level 1 (the hub), and the wipe the
//                                   current character's row of data_020755bc
//                                   selects.
//
// What it is NOT: the star's spin, Mario's hands-up animation, and the
// "COURSE CLEAR" message. Those are the cutscene, they live in the PowerStar
// actor's state table and in Message::, and they are honestly missing rather
// than faked. The handoff after them is here and fires.

int port_star_collect(int starId)
{
    if (_ZN5Event6GetBitEj(0x1e)) {
        fprintf(stderr, "[star] already collected this session -- ignored\n");
        return 0;
    }
    const int wasNew = IsStarCollectedInCurLevel(starId) ? 0 : 1;

    _ZN5Event6SetBitEj(0x1e);
    _ZN5Event6SetBitEj(0x1d);
    data_0209f228 = (unsigned char)starId;
    data_0209f2ac = (unsigned char)wasNew;
    CollectStarInCurLevel(starId);
    func_02012790(0x2d);

    fprintf(stderr, "[star] collected star %d in sublevel %d (%s), "
            "total now %d\n", starId, (int)data_0209f2f8,
            wasNew ? "NEW" : "already had it", (int)NumStars());

    ExitLevel();
    fprintf(stderr, "[star] course clear: ExitLevel -> next sublevel %d "
            "entrance %d, reason %d, wipe for character %d\n",
            (int)data_02092110, (int)data_0209f268, (int)data_0209f26c,
            (int)data_0209caa0[0x41]);
    return 1;
}

// =============================================================================
// The probe surface
// =============================================================================
//
// One entry point per thing the gate has to be able to SHOW moving. The
// harness drives them from a frame number; nothing here runs on its own.

int port_course_health(void)
{
    return data_02092144[data_0209f250] >> 8;
}

/* Sound, measured rather than asserted. Voices and sequencer players are the
   two things that either exist or do not: sd_mix_active is the mixer's own
   "is this hardware channel sounding", and sd_seq_active is "is this player
   still walking its sequence". A submitted command that allocated nothing
   shows up here as zeroes, which is the failure the [snd] log alone cannot
   tell apart from silence in the source material. */
extern int func_02049018(const int *camSpacePos);
extern int data_02099fac;   /* the 3D distance limit, romdata */
extern int data_02099fb0;   /* the type-9 voice-pool count, romdata */

int port_course_sound_probe(const char *when)
{
    int voices = 0, players = 0;
    for (int i = 0; i < 16; ++i) if (sd_mix_active(i)) ++voices;
    for (int i = 0; i < 32; ++i) if (sd_seq_active(i)) ++players;
    fprintf(stderr, "[snd-probe] %s: %d/16 voices sounding, %d/32 sequence "
            "players running, bank=0x%02x group=%d music=%d\n",
            when, voices, players, (unsigned char)data_0208e428,
            (int)data_0209b47c, g_course_music);
    /* the two numbers that decide whether a POSITIONAL sound is allowed to
       exist at all, plus where the listener thinks the player is */
    const char *p = (const char *)data_0209f394[data_0209f250];
    if (p) {
        const int *cs = (const int *)(p + 0x74);
        fprintf(stderr, "[snd-probe]   listener-relative player = "
                "(%d, %d, %d) scene, distance %d, limit %d, "
                "positional voices %d\n",
                cs[0] >> 12, cs[1] >> 12, cs[2] >> 12,
                func_02049018(cs), data_02099fac, data_02099fb0);
    }
    return voices;
}

/* ---- the looping-sound reap, observed rather than asserted ----------------
 *
 * The per-frame reaper (func_020119c8) and the level-change reaper (func_02011974)
 * both walk data_0209b53c, PlayLong's 0x40-entry handle table. An entry is LIVE
 * exactly when its first word -- the handle its owner is holding -- is non-zero.
 * This is the same count sd_vtrace_loop_census prints, exposed as a number the
 * harness can read without the voice trace on. */
int port_course_loop_live(void)
{
    int live = 0;
    for (int i = 0; i < 0x40; i++)
        if (data_0209b53c[2 + i * 5])   /* +8 header, then 0x14 (5 words) each */
            live++;
    return live;
}

/* Start a looping sound through the game's OWN Sound::PlayLong, at the player's
 * camera-space position. That position (Actor+0x74, the same field the coin
 * probe borrows) is what the listener is centred on, so func_02048a1c's distance
 * cull passes and the loop actually starts -- the walk/slide states that would
 * start one in play depend on collision geometry the straight-line selftest does
 * not reach, so the table is otherwise empty. Sound id 3 is the ambient-loop
 * kind AmbientSoundEffects::Behavior uses. Returns the handle PlayLong hands
 * back (0 if it was culled), which is what mLoopingSoundHandle would hold. Pass
 * the previous handle back in `prev` to REFRESH the same loop (PlayLong finds it
 * and marks it, so exactly one handle stays live) the way an actor holding a
 * loop does; pass 0 to start a fresh one. */
unsigned int port_course_loop_start(unsigned int prev, unsigned int soundId)
{
    void *player = data_0209f394[data_0209f250];
    if (!player)
        return 0;
    void *camSpacePos = (char *)player + 0x74;
    return _ZN5Sound8PlayLongEjjjRK7Vector3j(prev, 3, soundId, camSpacePos, 0);
}

int port_course_coins(void)
{
    return NumCoins();
}

/* The handoff, OBSERVED rather than caused.
 *
 * SetNextLevel is the only thing that writes data_02092110 / data_0209f268 /
 * data_0209f26c, and it has exactly three callers: ExitLevel (course
 * cleared), KillPlayer (died) and HitDeathPlane (fell out of the world). So a
 * change in that triple means the course has ended and nothing on the port
 * had to say so.
 *
 * A CHANGE, not a non-zero value. data_02092110 is a NAMED romdata entry, so
 * it boots with the ROM's own byte in it (0xff), and "is it non-zero" reads
 * as fired before anything has happened -- which is what the first version of
 * this probe reported. */
int g_arm_watch[3];

void port_course_arm_watch(void)
{
    g_arm_watch[0] = data_02092110;
    g_arm_watch[1] = data_0209f268;
    g_arm_watch[2] = data_0209f26c;
}

int port_course_handoff_fired(void)
{
    return data_02092110 != (unsigned char)g_arm_watch[0] ||
           data_0209f268 != (unsigned char)g_arm_watch[1] ||
           data_0209f26c != (unsigned char)g_arm_watch[2];
}

int port_course_next_sublevel(void)
{
    return (int)data_02092110;
}

/* Is he in a state that ends the life? data_ov002_0211010c is the one
   Player::Hurt changes to when func_ov002_020d91e0 answers "dead", and
   data_ov002_02110124 is its water twin -- the two the ROM itself compares
   against in Player::Heal to refuse healing a corpse. Comparing the state
   POINTER is exact; comparing a printed address is not. */
extern int data_ov002_0211010c[];
extern int data_ov002_02110124[];

int port_course_in_dead_state(void *player)
{
    const void *st = *(void **)((char *)player + 0x370);
    if (st == (void *)data_ov002_0211010c) return 1;
    if (st == (void *)data_ov002_02110124) return 2;
    return 0;
}

/* The HUD's own displayed health. St_DeadHit_Main will not call KillPlayer
   until this has ticked down to 0, which is why the death is not instant: the
   meter empties one wedge at a time and the state machine waits for it. */
int port_course_hud_health(void)
{
    extern unsigned char data_ov002_0211117c;
    return (int)data_ov002_0211117c;
}

/* Damage, through the game's own Player::Hurt.
 *
 * `kind` is the ROM's damage class: 1 is a normal hit (one wedge), 2 and up
 * are the heavy ones that also knock Mario onto his back. Hurt derives the
 * knockback angle from `src`, so the probe passes a point a body-length in
 * front of him -- that is what an enemy standing there would be.
 *
 * Hurt itself does not subtract the health. It calls func_ov002_020d91e0,
 * which is the piece that reads the cap state, applies the 1.5x no-cap
 * penalty, calls Player::Heal(-damage) and answers "is he dead now" -- and
 * on 1 it changes the state to the DEAD one rather than the hurt one. So the
 * whole death decision is the ROM's; this only supplies the hit. */
/* The ROM's own "may he be hurt right now" gate: the invincibility timer, the
   mega/no-control flags and the already-taking-damage flag. An enemy's
   collider hits every frame it overlaps and Player::Hurt refuses on all but
   the first, so a probe that wants to land a SECOND hit has to ask this the
   same way rather than count frames. */
int port_course_can_hurt(void *player)
{
    return func_ov002_020d82f0(player);
}

int port_course_hurt(void *player, int kind)
{
    char *p = (char *)player;
    int src[3];
    const int hp_before = port_course_health();
    src[0] = *(int *)(p + 0x5c) + 0x8000;   /* 8 units in +X */
    src[1] = *(int *)(p + 0x60);
    src[2] = *(int *)(p + 0x64);
    const int r = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
        player, src, (unsigned)kind, 0x8000, 0, 0, 1);
    const int dead = port_course_in_dead_state(player);
    fprintf(stderr, "[hurt] kind=%d -> Hurt returned %d, hp %d -> %d, "
            "step=%u, state=%s\n", kind, r, hp_before, port_course_health(),
            *(unsigned char *)(p + 0x6e3),
            dead == 1 ? "DEAD (data_ov002_0211010c)"
                      : dead == 2 ? "DEAD-WATER (data_ov002_02110124)"
                                  : "alive");
    return r;
}

/* The drowning tick. Player::St_HurtWater is the underwater damage state and
   the breath meter is the same health word, so drowning is Heal(-n) applied
   while mIsUnderwater is set -- which is what the ROM's own water damage
   does. Spelled here so a probe can drive it without a water volume. */
int port_course_drown_tick(void *player, int amount)
{
    _ZN6Player4HealEi(player, -amount);
    return port_course_health();
}

/* The death handoff, as the ROM fires it. St_DeadHit_Main calls KillPlayer
   once the HUD's displayed health has ticked to zero; this is the same call
   for a harness that wants it now. */
void port_course_kill(void)
{
    const int lives = data_0209f2f4[0];
    KillPlayer();
    /* The handoff, read back as data: SetNextLevel fills the next sublevel
       and its entrance out of data_02075638, and StartExitFaderWipe(4) arms
       the wipe. Printing the two words is the difference between "the call
       returned" and "the call did something". */
    fprintf(stderr, "[death] KillPlayer: lives %d -> next sublevel %d "
            "entrance %d, reason %d, exit wipe 4 armed\n",
            lives, (int)data_02092110, (int)data_0209f268,
            (int)data_0209f26c);
}

/* Respawn: put the health back the way the game does between lives and hand
   the player to the respawn state. data_02092144 is what SetPlayerGlobals
   fills at 0x880 and what every life starts from. */
void port_course_respawn(void *player)
{
    data_02092144[data_0209f250] = 0x880;
    _ZN5Event8ClearBitEj(0x1d);
    fprintf(stderr, "[respawn] health restored to %d, health meter released\n",
            port_course_health());
    (void)player;
}

}  // extern "C"

// =============================================================================
// func_0200ee8c: the star-get camera-script launcher -- a u8-return ride-through
// =============================================================================
//
// src/func_0200ee8c.c is the star-get cutscene's script selector:
//
//     void func_0200ee8c(s32 arg0) {
//         if (arg0 < 0) arg0 = GetStarCameraSetting(data_0209f224);
//         RunKuppaScript(data_020876e4[arg0]);   // 6-entry table
//     }
//
// It declares `extern s32 GetStarCameraSetting(s32)` and uses the FULL return as
// the index into data_020876e4[6]. But GetStarCameraSetting is defined
// `unsigned char GetStarCameraSetting(int)` -- `(data_02092134 >> (idx*4)) & 0xf`
// -- so it only ever sets AL (verified in the obj: `and al,0Fh; ret`). On ARM the
// s8/s32 declaration mismatch is harmless: the ROM's `bl` leaves the byte in r0
// and the AND already cleared the top bits, so `data_020876e4[r0]` indexes 0..5.
//
// On x86 MSVC the __cdecl caller reads the WHOLE of EAX, whose high bytes
// GetStarCameraSetting never wrote, so a call that should return 1 came back as
// 0x00001001 = 4097. `data_020876e4[4097]` reads 0x4000 bytes past a 24-byte
// table, hands RunKuppaScript a garbage pointer (0ffaff1e), and ProcessKuppaScript
// faults c0000005 the first frame of the star-get cutscene. MEASURED on the
// natural king-defeat + touch path (SM64DS_LEVEL=6 SM64DS_KING_FORCE_DEFEAT=90,
// star state 4 -> collect handler -> St_NoControl_Main -> func_ov002_020c7ff8 ->
// func_0200d4b0 -> here).
//
// The fix is the sound_abi.cpp / HitDeathPlane pattern: host the one function and
// spell the return the width the ROM means. GetStarCameraSetting yields a 4-bit
// value, so masking its result with 0xf reproduces exactly the ARM behaviour (the
// only bits the ROM's own `& 0xf` and single-byte return could carry) and keeps
// the index inside the 6-entry table. src/func_0200ee8c.c is filtered out of
// slice_gate10.txt in exchange.
// PORT_HOST_ABI: GetStarCameraSetting returns u8 (AL only); the src reads it as
// s32 and indexes with it, so mask to the ROM's own 0xf here.
extern "C" {
extern void *data_020876e4[];
extern signed char data_0209f224;
extern int GetStarCameraSetting(int star);
extern void RunKuppaScript(void *script);

/* PORT_HOST_ABI: GetStarCameraSetting returns u8 (AL only); cdecl reads all
   of EAX, so mask to the ROM's own 0xf here (see the header above). */
void func_0200ee8c(int arg0)
{
    if (arg0 < 0)
        arg0 = GetStarCameraSetting((int)data_0209f224) & 0xf;
    RunKuppaScript(data_020876e4[arg0]);
}
}  // extern "C"
