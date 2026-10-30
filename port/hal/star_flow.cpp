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
//   HitDeathPlane           RETIRED at run link100 wave 15: main fixed the
//                           argument the host copy used to name, and the
//                           matched TU runs. See the block below.
//
// plus a probe surface (port_course_probe_*) the harnesses drive so every
// one of those paths can be shown moving with a log rather than asserted.
#include <cstdio>
#include "vs_width.h"   /* run vs16: the port's player width */
#include <cstdlib>
#include <cstring>

#include "sdat/sdat.h"

/* run mp1 lane STARREF1: the arena half of this file (the referee decides
   stars, hits, the clock and the winner); every use below is gated on the
   arena being installed with a referee. */
#include "vs_arena.h"
#include "vs_arena_ref.h"

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
void _ZN8dActor_c15GivePlayerCoinsER6Playerhj(void *actor, void *player,
                                           unsigned char count,
                                           unsigned int kind);
int  func_ov002_020d82f0(void *player);   /* Player::Hurt's own entry gate */
void sdat_host_tick(void);
int  IsLevelInsideCastle(int level);           /* seat_course_sound's two   */
int  IsLevelTinyHugeIslandOutside(int level);  /* same-area music guards    */
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int frames);
void _ZN5Sound8SetMusicEjj(unsigned int player, unsigned int seqId);
void _ZN5Sound10PauseMusicEv(void);            /* Stage::PS_Init's own call */

// ---- the globals it reads and seats ----------------------------------------
extern int          data_0209fc48;    /* the running cutscene script, 0 = none */
extern int          data_0209b4ac;    /* layer 1's current song (in .dsstate)   */
extern int          data_0208e43c;    /* layer 1's loaded base song (romdata,
                                         outside .dsstate)                      */
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
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int j1,
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
// HitDeathPlane: RETIRED, the matched TU runs (run link100 wave 15, SEAT15F)
// =============================================================================
//
// This file used to carry a host copy of HitDeathPlane, because src/ declared
// `extern void SetNextLevel(void)` and called it with no arguments while
// src/SetNextLevel.c defined `SetNextLevel(int arg)`: right on ARM, where the
// value is already in r0 and the ROM's `bl` never touches it, and wrong on
// cdecl, where `arg` read whatever the caller's stack happened to hold -- and
// `arg` is what lands in data_0209f26c, the reason-for-leaving code the next
// scene reads.
//
// MAIN HAS SINCE FIXED IT. src/HitDeathPlane.c at 8ddff3187 declares
// `extern void SetNextLevel(int arg);` and calls `SetNextLevel(arg)`, and its
// other two callees (StartExitFaderWipe, dScene_c::StartSceneFade) match this
// file's own declarations of them argument for argument. So the host copy was
// a stand-in for a defect that no longer exists, and it is gone: the matched TU
// is back in SLICE10_CAM_SOURCES (the filter line in port/CMakeLists.txt came
// out with the body) and the ROM's own text runs on the death-plane path.
//
// The one declaration the body left behind is kept: the VS fade below still
// calls dScene_c::StartSceneFade.
extern "C" {
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned a, unsigned b, unsigned short c);
}  // extern "C"

/* Luigi Infection (hal/luigi_infection.cpp). The pre-round countdown driver
 * below needs these, and port_vs_countdown_tick guards on the first so the mode
 * owns the whole countdown when it is armed. Both are a cached compare when
 * SM64DS_VS_LUIGI_INFECTION is unset. */
extern "C" int  port_luigi_enabled(void);
extern "C" void port_luigi_seed(int frame);
/* Nonzero while a rewind is being re-run, so the effects the countdown drives
   (the beeps, the music, the trace) stand down the way every other outside-world
   touch in the frame loop already does. DEFINED in hal/luigi_infection.cpp and
   written by hal/rollback.cpp -- this file is linked into targets rollback.cpp
   is not in, so calling its rb_replaying() directly is unresolved in those. */
extern "C" int g_port_rb_replaying;

// =============================================================================
// The boot: the state the whole loop reads before any of it means anything
// =============================================================================

namespace {

int g_seated;
int g_course_music = -1;
/* seat_course_sound's prevLevel: data_0209f2f8 as it stood when the port's
   latch replaced it (hal/level_change.cpp port_level_latch, the ROM's :226).
   -1 until the first latch; the cartridge's bss zero is not a castle level
   either. */
int g_music_prev_level = -1;

/* What seat_course_sound worked out for the level music, kept for
   seat_course_music: the ROM makes the music call later in
   Stage::InitResources than the sound group / bank statements, so the two
   halves run at two points of the boot. */
int g_boot_group, g_boot_bank, g_boot_vs, g_boot_bgm;

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

    g_boot_group = group;
    g_boot_bank = bank;
    g_boot_vs = vs;
    g_boot_bgm = bgm;
}

/* THE LAYER-1 MUSIC CALL, WHERE STAGE::INITRESOURCES MAKES IT.
 *
 * src/_ZN5Stage13InitResourcesEv.cpp:380-391: the music statement comes AFTER
 * Stage::LoadClsnAndObjects (the call that builds the level's actors, the
 * Player among them), and the sound group / bank statements that
 * seat_course_sound still makes up front come well before it. The port ran
 * both together, before the Player existed. That matters for the songs whose
 * variant follows the Player: LoadAndSetMusic_Layer1 ends in func_02011dcc,
 * which asks func_02013078 for the variant and applies it at once with
 * func_0204921c, and func_02013078 reads the Player (underwater flag, height)
 * and answers -1 when there is none. So the cartridge starts Jolly Roger Bay
 * and Dire Dire Docks (NCS_BGM_WATER, layer A = tracks 0-1, layer B = tracks
 * 2-13) with the variant for where Mario is already in place, and the port
 * started with every track at full volume (the upbeat mix) and only then let
 * func_020132d8 ramp the unwanted layers down over about a hundred frames.
 * Measured on both: the cartridge sends the INITMODE pair (volume 0 and a
 * mute on each layer) in the frame of the START; the port sent none. */
void seat_course_music(int level)
{
    const int group = g_boot_group, bank = g_boot_bank, vs = g_boot_vs;
    const int bgm = g_boot_bgm;

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
     * see port_vs_countdown_tick below.
     *
     * THE OTHER FOUR PRECONDITIONS are what keeps the castle theme playing
     * from room to room (src/_ZN5Stage13InitResourcesEv.cpp:386-391):
     *
     *     if (data_0209fc48 == 0)                               no cutscene
     *     if (!IsLevelInsideCastle(level) || !IsLevelInsideCastle(prevLevel))
     *     if (!IsLevelTinyHugeIslandOutside(level) ||
     *         !IsLevelTinyHugeIslandOutside(prevLevel))
     *     if (level != 2 || (data_0209caa0[2] & 0x200))         castle 1F seen
     *         Sound::LoadAndSetMusic_Layer1(data_0207576a[level * 3]);
     *
     * So walking between two castle rooms (sublevels 2, 4, 5 and 0x32, all
     * row 57) or between the two Tiny-Huge Island outsides makes NO music
     * call and the song already playing carries on. Without them the port
     * restarted the castle theme at every door. Level 2 before event bit
     * 0x200 is the first castle entry: Player::St_Talk_Cleanup starts 0x39
     * itself once the welcome message closes. prevLevel is data_0209f2f8
     * before InitResources re-latches it (:226); the port's latch runs in
     * hal/level_change.cpp before this boot and hands the old value to
     * port_music_note_prev_level below. It is the global, not the last level
     * booted: ExitMinigameMenu writes 6 into it on the way back to the Rec
     * Room, so that return is not castle to castle and the castle song, which
     * the minigame menu stopped, starts again. */
    const int vs_mode = (data_0209f2d8 == 1);
    const int prev = g_music_prev_level;
    const char *skip = 0;
    if (vs_mode)
        skip = "VS: the ROM does not read this column";
    else if (data_0209fc48 != 0)
        skip = "a cutscene script is running: no call";
    else if (IsLevelInsideCastle(level) && IsLevelInsideCastle(prev))
        skip = "castle room to castle room: no call, the song carries on";
    else if (IsLevelTinyHugeIslandOutside(level) &&
             IsLevelTinyHugeIslandOutside(prev))
        skip = "Tiny-Huge Island outside to outside: no call, the song "
               "carries on";
    else if (level == 2 && !(((const int *)data_0209caa0)[2] & 0x200))
        skip = "first castle entry: no call, the welcome message starts it";

    /* g_course_music is only read by port_course_sound_probe, and it means
       "what this file last asked layer 1 for". In a match nothing is asked for
       here, and func_ov075_02116c8c's StopLoadedMusic_Layer1 has already run,
       so -1 is the true state until the countdown starts 0x4d. A skipped
       adventure call asks for nothing, so the value stays as it was. */
    if (vs_mode)
        g_course_music = -1;
    else if (!skip)
        g_course_music = bgm;
    fprintf(stderr, "[course] sublevel %d sound row: group=%d bank=0x%02x "
            "bgm=%d (%s) [%s branch, star=%d, from sublevel %d]\n", level,
            group, bank, bgm,
            skip ? skip : bgm < 0 ? "no layer-1 track" : "start",
            vs ? "VS arena" : "adventure table", (int)data_0209f220, prev);
    if (!skip) {
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(bgm);
        /* The ROM's next statement (src/_ZN5Stage13InitResourcesEv.cpp:
           391-395): the local player index, and when that Player's byte
           0x6ff is set (the cap-start sublevels) SetMusic(index, 0x33), the
           cap song over the row song. Sublevel 31 ends on it. */
        const unsigned char sndIdx = data_0209f250;
        if (data_0209f394[sndIdx] &&
            ((const unsigned char *)data_0209f394[sndIdx])[0x6ff] != 0)
            _ZN5Sound8SetMusicEjj(sndIdx, 0x33);
    }
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
 * ---- THE ADVENTURE POWER METER NEEDS THIS SEAT, AND SO IT IS UNGATED --------
 *
 * The ROM's line has no condition and neither does the port's now. The gate was
 * VS-only for a while on a measurement that was right and a conclusion that was
 * wrong, and this is the lane the old note invited ("un-gating is a one-word
 * change for the lane that finds one").
 *
 * THE MEASUREMENT: with the seat forced on, adventure levels 1 and 5 render
 * BYTE-IDENTICAL to the un-seated frame, because engine A's OAM holds ZERO
 * placed sprites on those frames. THE WRONG CONCLUSION drawn from it was that
 * the adventure HUD "reaches the player through engine B, not this register"
 * and that the seat "is necessary for it and nowhere near sufficient". Both are
 * false. Engine A's OAM was empty because the POWER METER is the one adventure
 * HUD leaf that draws to engine A (HUD::RenderHealthMeter calls OAM::Render with
 * sub=0, main engine, and GX::LoadOBJPltt on the main palette), and the power
 * meter is RETRACTED off-screen until Mario takes damage -- mHealthMeterY sits
 * negative, so OAM::Render culls every one of its nine sprites and places
 * nothing. A boot selftest that never gets hurt is measuring a HUD element that
 * is deliberately hidden.
 *
 * DEPLOY IT AND THE SEAT IS EXACTLY SUFFICIENT. Drive one Player::Hurt on level
 * 1 (SM64DS_COURSE_PROBE=hurt) and the meter slides in at x=0x80, top centre:
 * with the seat its nine sprites are PLACED in engine A's OAM and the engine-A
 * compositor draws the "POWER" word and the segmented pie over the 3D frame,
 * exactly where the DS puts it and one wedge down; without the seat the same
 * hit still drops HP (the game logic is untouched by this register) but the
 * meter never reaches the picture, because the OBJ-layer bit the compositor
 * gates on is never set. Coins, stars and the map still reach engine B and the
 * bottom-screen inset as before -- this changes only the one leaf the ROM
 * itself routes to the main engine.
 *
 * SO THE SEAT IS UNCONDITIONAL BY DEFAULT. It is the ROM's own statement and it
 * is what makes the health meter visible in ordinary play. The env knob keeps
 * the older behaviours for A/B: SM64DS_HUD_LAYER_SEAT=off restores the pre-seat
 * frame, =vs restores the VS-only gate, =always is the default spelled out.
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
    static int mode = -1;   /* 0 off, 1 VS only, 2 unconditional (default) */
    if (mode < 0) {
        const char *e = std::getenv("SM64DS_HUD_LAYER_SEAT");
        mode = 2;
        if (e && std::strcmp(e, "off") == 0) mode = 0;
        else if (e && std::strcmp(e, "vs") == 0) mode = 1;
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
     * (src/_ZN2GX15SetGraphicsModeEiii.cpp), so c = 1 IS bit 3. The two lines are
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
     * which is the one bit of that call the port can honour.
     *
     * "THE PORT NEVER CALLS SetGraphicsMode" IS HISTORY ON THE LEVEL PATH (run
     * linkfull, lane HUDQUIT1). hal/level_boot.cpp's port_stage_boot_body now
     * makes Stage::InitResources' own first call, dScene_c::Initialise3dGraphics,
     * on every Stage build, and that function calls GX::SetGraphicsMode(1, 0,
     * 1) after its register reset: the ROM sets bit 3 itself, and the
     * display-mode and BG-mode fields it writes are its own values. The store below therefore finds the bit set
     * and changes nothing on a level boot; it is kept because it is harmless,
     * and retiring it is a separate change. The layer mask and the BG0CNT
     * priority are still this seat's alone. */
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
     * AN EARLIER REVISION WROTE 1 AND CITED src/_ZN9BootScene13InitResourcesEv.cpp FOR IT, calling
     * that "not a guess". Both halves were wrong and review caught them.
     * _ZN9BootScene13InitResourcesEv is dScBoot_c::InitResources -- a BOOT scene, not the level
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
            mode == 2 ? "unconditional: every mode (default; SM64DS_HUD_LAYER_SEAT=off/vs to change)"
                      : "VS only (SM64DS_HUD_LAYER_SEAT=vs)");
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

/* The music half of the sound row, run after Stage::LoadClsnAndObjects (see
   seat_course_music). level_boot.cpp calls it at that point of the boot. */
void port_boot_course_music(int level)
{
    seat_course_music(level);
}

/* Stage::InitResources' prevLevel (:226), handed over by the port's latch
   (hal/level_change.cpp port_level_latch) at the moment it replaces
   data_0209f2f8. seat_course_sound's same-area music guard reads it. */
void port_music_note_prev_level(int level)
{
    g_music_prev_level = level;
}

/* THE MUSIC AFTER A SAVE-STATE RESTORE (a port feature, run hunt3 lane MUSIC1).
 *
 * lk6_savestate_load silences the sequencer and the mixer and re-seeds the
 * hosted ARM7's command pool, because a live sequencer cannot be copied back
 * a few frames out of phase. Sound effects come back on their own: the game
 * asks for them again. The level's song does not: the cartridge asks for it
 * once, at the stage boot (seat_course_sound above), so after an F9 the world
 * ran on in silence until something else started a song.
 *
 * So the restore asks for it here, through the same ROM calls the boot and
 * the game use, and it asks for the song the SNAPSHOT was playing:
 *
 *   base     the restored sublevel's layer-1 row, the song a fresh boot of
 *            that level picks (every castle room is 57, so this is also what
 *            a carried castle song was)
 *   current  data_0209b4ac, layer 1's current song, which lives in .dsstate
 *            and so came back with the snapshot. Sound::SetMusic moves it
 *            off the base for a temporary song and Sound::EndMusic puts the
 *            base back; StopLoadedMusic_Layer1 sets it to -1 (silence).
 *
 * LoadAndSetMusic_Layer1(base) seats the base and starts it. If the snapshot
 * was on a different song, SetMusic(player, current) puts that one on over
 * the base, as the game did, so its own EndMusic later returns to the base;
 * if the snapshot had layer 1 stopped, StopLoadedMusic_Layer1 stops it again.
 * Songs start from their beginning.
 *
 * data_0208e43c (layer 1's loaded base) is romdata, outside .dsstate, so it
 * still describes the world before the restore, and LoadAndSetMusic_Layer1
 * can skip its start when that value equals the song asked for. The
 * sequencer that value described has just been reset, so nothing is loaded:
 * -1 is written first so the call starts the song. Layer 2, layer 3 and the
 * sub-song player are not captured either; LoadAndSetMusic_Layer1 clears them
 * itself (func_02011b7c), the state a fresh boot leaves them in.
 *
 * VS mode is left alone: the ROM does not read the row there and the arena's
 * song belongs to its countdown. */
void port_course_music_after_restore(void)
{
    const int level = data_0209f2f8;
    if (level < 0 || level >= 0x34 || data_0209f2d8 == 1)
        return;
    const int base = (signed char)data_02075768[level * 3 + 2];
    const int current = data_0209b4ac;
    data_0208e43c = -1;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(base);
    if (current != base) {
        if (current < 0)
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0);
        else
            _ZN5Sound8SetMusicEjj(data_0209f250, (unsigned)current);
    }
    /* A snapshot taken on the pause screen comes back on it (data_0209f2c4 is
       in .dsstate), and the song just started has not been through the pause
       the game put the old one through. Stage::PS_Init sends Sound::PauseMusic
       on the way in and PS_Cleanup sends UnpauseMusic (or stops layer 1) on
       the way out, so the song is paused here the same way, and the pause
       screen's own exit lets it go. */
    if (data_0209f2c4 != 0) {
        _ZN5Sound10PauseMusicEv();
        fprintf(stderr, "[course] restore: the snapshot is on the pause "
                "screen (state %d): the song is paused with it\n",
                data_0209f2c4);
    }
    g_course_music = current;
    fprintf(stderr, "[course] restore: sublevel %d, base song %d, song %d "
            "(%s)\n", level, base, current,
            current == base ? (current < 0 ? "no layer-1 track" : "restarted")
            : current < 0 ? "layer 1 was stopped in the snapshot"
                          : "the snapshot's own song over the base");
    sdat_host_tick();
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
 * (0x4c/0x51/0x52) -- func_ov075_02118378 and _ZN10dScEntry_c13InitResourcesEv, both scene
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
    /* LUIGI INFECTION owns the whole countdown when it is armed -- the state,
       the stretched ~5s cadence, the beeps, the arena music and the tagger pick
       (port_luigi_countdown_drive below). So the native ~2s sound countdown
       steps aside here. Gated on the mode: a normal VS round never takes this
       early return and everything below is byte-unchanged. */
    if (port_luigi_enabled())
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

/* ---- SM64DS_VS_LUIGI_INFECTION: the pre-round START COUNTDOWN ---------------
 *
 * Luigi Infection asks for a full FIVE-SECOND wait before the first Luigi, so
 * players can read the arena and the tagger cannot snowball off the spawn. This
 * does not invent a countdown: it drives the ROM's OWN VS start countdown -- the
 * one Stage::InitResources seats (data_0209f2bc = 3) and the matched
 * Stage::RenderVsModeCountdown draws as the READY? banner, the big red 3/2/1
 * numeral, then START. The ROM's countdown is ~2s; this stretches it to 5s off
 * the HOST FRAME COUNTER, so every peer runs the identical cadence and seeds the
 * tagger on the identical frame (lockstep VS -- no local clock, no rand). The
 * port hosts RenderVsModeCountdown NOWHERE for a normal VS round, and
 * port_vs_countdown_tick above steps aside when the mode is armed, so a normal
 * round is byte-unchanged.
 *
 * The schedule, in host ticks (walk_window runs the 3D logic at ~30Hz, so 30
 * ticks ~= one second), anchored at frame 90 -- the frame the tagger used to be
 * seeded, past the level-entry no-control freeze:
 *     [0,90)     count 3   held, silent (the level-entry freeze; see the drive)
 *     [90,150)   count 3   READY? + red 3    ~2.0s (the get-ready / spread beat)
 *     [150,195)  count 2   READY? + red 2    ~1.5s
 *     [195,240)  count 1   READY? + red 1    ~1.5s
 *      240       count 0   START  + port_luigi_seed picks the tagger (a full 5s)
 *     [240,270)  count 0   START held up (f304 non-zero so the glyph stays)
 * The native render pairs the READY? banner with the leading numeral, so the "3"
 * doubles as the get-ready beat held two seconds; 2 and 1 tick once a second.
 * data_0209f2bc != 0 across the window freezes the players exactly as the DS's
 * own 3-2-1 does (func_ov002_020c71e0), and it drops to 0 at the pick. */
extern void _ZN10dScStage_c21RenderVsModeCountdownEv(void);
extern void *data_0209f5bc;   /* the installed fader; the render derefs it, unguarded */

enum {
    kLiCdStart = 90,
    kLiCd3End  = 150,
    kLiCd2End  = 195,
    kLiCdPick  = 240,   /* count reaches 0: START + the tagger pick, a full 5s in */
    kLiCdEnd   = 270    /* START glyph held to here */
};

/* UPDATE half, called from the frame loop's behavior phase beside
   port_vs_countdown_tick: set the ROM countdown counter for this host frame,
   ring the native beeps / GO on the stretched cadence, and pick the tagger when
   the count reaches 0. Inert unless the mode is armed. */
void port_luigi_countdown_drive(int frame)
{
    if (data_0209f2d8 != 1) return;         /* VS only */
    /* ARENA MODE (run mp1, lane STARREF1): the referee's T0 owns the whole
       READY / 3 / 2 / 1 / GO. Every console holds the count at 3 until the
       referee's START arrives, then counts down to T0 on the referee's clock, so
       the match clock and the stars start together everywhere. The same four
       writes and the same sounds this function makes for the Luigi mode. */
    {
        int a_count, a_f304, a_beep, a_music;
        if (port::vs_ref_countdown(&a_count, &a_f304, &a_beep, &a_music)) {
            data_0209f2bc = (unsigned char)a_count;
            data_0209f304 = (unsigned short)a_f304;
            if (a_beep) func_02012790((unsigned int)a_beep);
            if (a_music) {
                g_course_music = a_music;
                _ZN5Sound22LoadAndSetMusic_Layer1Ei(a_music);
            }
            return;
        }
    }
    if (!port_luigi_enabled()) return;      /* a normal VS round never enters */

    /* THE HOLD STARTS AT FRAME 0, not at the first beep. The ROM's own
       Stage::Behavior countdown block runs every VS frame ahead of this call
       (the frame loop's copy retired; see walk_window's Stage comment) and on
       its own ~2s cadence reaches 0 at f61, which lets every player out of the
       level-entry step (func_ov002_020c71e0) at f62. Writing the count back to
       3 at f90 does not put anyone back in that step, so a drive that started
       at f90 left the players running for ~6s under READY? 3-2-1. Held from
       frame 0, the count is never 0 before the pick, so the players stay held
       until START, the way the DS's own 3-2-1 holds them. Before kLiCdStart
       the count is simply held at 3 (what seat_vs_countdown seated), silently:
       the beeps and the trace keep their schedule below. */
    int count;
    if      (frame < kLiCd3End) count = 3;
    else if (frame < kLiCd2End) count = 2;
    else if (frame < kLiCdPick) count = 1;
    else                        count = 0;

    /* THE TIMER WORD WHILE A COUNT IS SHOWING: 0x2a, not 0x28. The ROM block
       that runs first next frame opens with "if (f304 == 0x28 && f2bc == 3)
       Play2D(0x2b)", then steps f304 down by data_0208ee44 (2) and drops the
       count when it lands on 0. 0x2a is not 0x28, so its entry beep never
       fires (a held 0x28 beeps every frame of the 3), and 0x2a - 2 is 0x28,
       not 0, so it never drops the count, rings its own beep or GO, or starts
       the music: this function stays the only source of all four. The render
       only asks whether f304 is non-zero (the START glyph), so the value is
       free. After the pick the count is 0 and the ROM's entry test cannot
       match, so START keeps the 0x28 it always had. */
    data_0209f2bc = (unsigned char)count;
    data_0209f304 = (unsigned short)(count != 0 ? 0x2a
                                     : frame < kLiCdEnd ? 0x28 : 0);

    /* the native countdown audio on the stretched schedule: Sound::Play2D(2,..)
       -- a beep entering each count (0x2b), the GO chord (0x2a) and the arena's
       own music (0x4d) at START -- the exact calls port_vs_countdown_tick makes.

       A REPLAYED FRAME SPLITS THESE TWO WAYS, and the split is the point. A
       rewind re-runs these frames to catch the world up, and the pad, the mouse,
       the camera rig and the editor channel all stand down for it. An EFFECT
       nothing reads back (a beep) must not fire twice. STATE the restore wiped
       (the music selection) must be re-issued, or the replay ends somewhere the
       straight run never was. See each block below.

       THE COUNTER WRITES ABOVE ARE NOT GUARDED and must not be: they are the
       world, they are pure functions of the frame, and reproducing them is what
       the replay is for. */
    /* THE BEEPS RE-TRIGGER ON A REPLAYED FRAME, deliberately, because that is
       what this port's rollback does with in-game sound: hal/rollback.cpp says
       so in as many words -- the restore re-seeds the command queue, "the replay
       re-triggers sounds into the reset queue", the output stage is muted
       (sd_host_mute), and the DET rung NAMES the queue's bytes rather than
       counting them as a divergence. Standing these down was tried and is
       wrong twice over: it makes the replay do less than the straight run did,
       which is the opposite of reproducing it, and it measured no better
       (4 bytes outside the queue either way). The stand-down rule the frame loop
       applies to the pad, the mouse, the camera rig, the editor channel and
       port::voice_tick is for effects the port does NOT mute and does NOT
       excuse -- an input read, or a packet on a wire. */
    if (frame == kLiCdStart || frame == kLiCd3End || frame == kLiCd2End)
        func_02012790(0x2b);
    else if (frame == kLiCdPick)
        func_02012790(0x2a);

    /* THE ARENA MUSIC IS STATE, NOT AN EFFECT, so it is re-issued on a replayed
       frame rather than suppressed -- the one place this mode does NOT stand
       down, and the distinction is the whole point. A restore re-seeds the sound
       driver (sd_consumer_reset / sd_sdat_reseat in hal/rollback.cpp), which
       clears the sequence player the straight run had playing; if the replay
       does not put the selection back, the re-run reaches the same frame with no
       music where the straight run had 0x4d. That is both a divergence in the
       sequence-player block and a match that falls silent after a rewind. The
       replay is muted (sd_host_mute), so re-issuing costs nothing audible. */
    if (frame == kLiCdPick) {
        g_course_music = 0x4d;
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x4d);
    }

    /* The trace is an effect too, and its own latch is a host static a rewind
       cannot put back, so a replay would both double-print and leave the latch
       reading a count the world has since backed out of. */
    static int last_shown = -99;
    if (frame >= kLiCdStart && count != last_shown && !g_port_rb_replaying) {
        last_shown = count;
        fprintf(stderr, "[luigi] COUNTDOWN f%d shows %s\n", frame,
                count == 3 ? "READY? + 3" : count == 2 ? "READY? + 2" :
                count == 1 ? "READY? + 1" : "START (0)");
    }

    /* THE PICK, at countdown end -- moved here from the old frame-90 seed.
       port_luigi_seed self-latches on g_li_seeded, so calling it every frame
       from the pick on seeds exactly once, on frame 240. */
    if (frame >= kLiCdPick)
        port_luigi_seed(frame);
}

/* RENDER half, called from the frame loop's render phase after the actors so the
   countdown sprites join the same engine-A OAM batch the VS timer fills: the
   ROM's own Stage::RenderVsModeCountdown, which reads data_0209f2bc and draws
   READY? + the red numeral + START. Null-guarded on the fader that render
   dereferences without a check. Inert unless the mode is armed.

   THE SAME FOUR-WIDE CENSUS AS THE START. The render opens with the ROM's own
   "count the landed players in slots 0..3, draw nothing unless that reaches
   data_0209fc50", so above four players it drew nothing at all: no READY?, no
   numeral, no START, while the round itself still ran on the drive above. The
   call takes the view hal/stage_frame.cpp gives Stage::Behavior's matching
   census (the seated count reads n minus the landed seats 4..n-1 for the
   length of the call, put back straight after), so the sprites draw once every
   seat has landed at every width. Four players or fewer never take the view. */
extern "C" int  port_vs_census_view_enter(void);
extern "C" void port_vs_census_view_leave(int saved);

void port_luigi_countdown_render(void)
{
    if (data_0209f2d8 != 1) return;
    /* ARENA: the READY? / 3 / 2 / 1 / START banner is drawn while the referee's
       countdown runs (the same draw the Luigi mode uses). */
    if (!port_luigi_enabled() && !(port::vs_ref_active() &&
                                   (data_0209f2bc != 0 || data_0209f304 != 0)))
        return;
    if (data_0209f5bc == 0) return;
    const int census = port_vs_census_view_enter();
    _ZN10dScStage_c21RenderVsModeCountdownEv();
    port_vs_census_view_leave(census);
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
// scene 6 lobby, told apart by its own id (src/_ZN10dScEntry_c13InitResourcesEv.cpp branches
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
// ---- AND THEN THE ROM MADE THE REQUEST ITSELF (run mp1, lane VSTIMEUP1) -----
//
// Everything above was written while Stage::Behavior was not in the link. It is
// now (_ZTV5Stage slot 6 holds the ROM's body, port/slice_gate220.txt), so the
// statement quoted at the top of this block runs on its own and asks for scene
// 7 every frame the guard is open, with this switch off. That turned every VS
// match that ran to the clock into the abort measured above, on every console,
// in the shipped kit and in a player's own match. port_vs_match_end_poll now
// answers a pending 7 in VS itself: this port's results screen is this end
// path (banner, MATCH OVER marker, the launcher room), so the sentinel goes back
// before Scene::BeforeBehavior reads the id and the Stage is never marked. The
// marker says results_screen=port when that happened. See the block at the top
// of port_vs_match_end_poll.
//
// ---- THE ENVIRONMENT --------------------------------------------------------
//
//   SM64DS_VS_MATCH_END=0   the whole watcher off. Default ON: the time-up
//                           report and the ROM's own music stop are the ROM's
//                           behaviour at the ROM's own moment, and the marker
//                           costs a line in the flight recorder.
//   SM64DS_VS_END_SCENE=1   ALSO make the ROM's Scene::StartSceneFade(7,0,0)
//                           request, and leave the ROM's own request standing
//                           (no answer). Default OFF. IT ABORTS THE PROCESS
//                           about thirty frames later, by the measurement
//                           above; it is the instrument that reproduces the
//                           abort from a fixed binary, not something to leave
//                           on.
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
static int g_end_by_king;
/* Luigi Infection: 0 = not this mode, 1 = the Luigis won (all infected), 2 =
 * the survivors won (clock ran out with one alive). Set in the end poll, read
 * by the banner and the marker so the winner line names the team, not a slot. */
static int g_end_by_luigi;
/* lane DROPOUT1: 1 + the slot that won because everybody else left, 0 when
   the match ended any other way. */
static int g_end_by_last;
/* lane VSTIMEUP1: how many times this match the ROM's own Stage::Behavior asked
   for scene 7 and port_vs_match_end_poll answered it (0 = never asked). */
static int g_scene7_answered;
/* lane RESULTS1: arena only. 0 = this match did not end through the referee
   (every lockstep end), 1 = the referee ended it on the clock (the end the ROM
   answers with its own scene-7 request), 2 = the referee ended it any other
   way. The marker's results_screen field reads this in an arena instead of
   g_scene7_answered, which is a per-console race there (see the marker). */
static int g_arena_results;
/* lane DROPOUT1: the slots that have been in play this match, and those of
   them that have left it (vs_track_seats, below, keeps both). A slot that left
   has no place in the result: the winner, the draw test and the scores run
   over the players still in the match. With no carrier session nobody ever
   leaves, and every result below is the one it always was. */
static unsigned g_vs_seen;
static unsigned g_vs_left;

static int vs_popcount(unsigned m)
{
    int n = 0;
    for (; m; m &= m - 1) ++n;
    return n;
}

/* 1 when slot i is in the match: it has been in play (a seat the carrier
   never carried -- slots 2 and 3 of a two-player match, whose floor is four --
   is nobody) and has not left. With no carrier session every slot is. */
static int vs_in_play(int i)
{
    if (g_vs_seen == 0)
        return 1;
    return (g_vs_seen >> i & 1u) != 0 && (g_vs_left >> i & 1u) == 0;
}

/* THE WINNER OVER THE PLAYERS STILL IN THE MATCH: the lowest in-play slot with
   the top latched score, and how many other in-play slots share it (a draw).
   With nobody gone this is exactly the old scan over 0..np-1. */
static int vs_end_best(int np, int *ties)
{
    int best = -1;
    for (int i = 0; i < np; ++i)
        if (vs_in_play(i) && (best < 0 || g_end_scores[i] > g_end_scores[best]))
            best = i;
    if (best < 0)
        best = 0;
    int t = 0;
    for (int i = 0; i < np; ++i)
        if (i != best && vs_in_play(i) && g_end_scores[i] == g_end_scores[best])
            ++t;
    *ties = t;
    return best;
}

/* SM64DS_VS_LUIGI_INFECTION, defined in hal/luigi_infection.cpp. All inert (a
 * cached compare) when the mode is off. */
extern "C" int  port_luigi_enabled(void);
extern "C" int  port_luigi_seeded(void);
extern "C" int  port_luigi_survivors_alive(void);
extern "C" int  port_luigi_match_slots(void);
extern "C" int  port_luigi_time_frames(void);
extern "C" void port_luigi_match_reset(void);

/* ===================== KING OF THE STAR (host win mode) =====================
 *
 * SM64DS_VS_KING_TARGET=<n>: exactly one star for the whole match, whoever
 * holds it scores +1 point per real second, first to <n> points wins. 0 or
 * unset is OFF, and off means one compare against a cached constant per frame
 * and nothing else -- time and stars VS are byte-for-byte unchanged.
 *
 * The points are a SEPARATE host array, NEVER data_0209f310. That array is the
 * ROM's per-player carried-star count the HUD and NumVsStarsObtained read, and
 * it is not cleared between in-process matches (the re-arm trap documented
 * above). g_king_points is ours and is zeroed when a king match arms.
 *
 * Holder detection still READS data_0209f310, which is legitimate because king
 * mode holds a ONE-STAR invariant: level_boot seats a king star-order so only
 * the first marker ever wakes (every later rotation slot names a star-id no
 * marker carries), and the two dupe guards keep a collect worth exactly one.
 * So at most one slot carries the star and its nonzero count IS the holder.
 * Everything here is shared lockstep state, so every console agrees frame for
 * frame on holder and points. */
static int g_king_target = -1;      /* <0 unread, 0 off, >0 the point target  */
static int g_king_points[kPortMaxPlayers];  /* seconds of possession, ours     */
static int g_king_tick;             /* 0..59, the deterministic per-second gate */
static int g_king_armed;            /* zeroed on disarm; arms the match reset   */
static int g_king_holder = -1;      /* current holder slot, for the live HUD    */

/* ---- THE ROLLBACK SNAPSHOT'S COPY OF THE MATCH FLOW -----------------------
 *
 * Everything latched above is SIMULATION STATE that lives in host statics, so
 * it sits outside all three regions the rollback snapshot copies (the DS arena,
 * .dsstate and the hardware content stores) and until run link100 a rewind left
 * it exactly where the frame it rewound over had put it. hal/luigi_infection.cpp
 * names this class and carries its own two words through the slot; this is the
 * same carriage for the match flow. .dsstate is no more the place for these
 * than it was for those: hal/dsstate_seg.h reserves that bracket for hosted DS
 * globals and tools/dsstate_guard.py says host-only symbols must NOT be in it.
 *
 * WHAT GOES WRONG WITHOUT THIS, precisely, and it is player-facing.
 * g_end_fired latches the frame the match was won and never unlatches:
 * port_vs_match_end_frozen reads it to hold every pad from the frame after, the
 * trigger block below is skipped for the rest of the session once it is set,
 * and g_end_scores is the scoreline the marker reports. A console that rewinds
 * over the winning frame comes back with the arena restored -- the star is
 * uncollected again, the match is live again -- but with the match still over
 * in its own head: its pads still held, its old scoreline still latched, its
 * trigger permanently skipped. A console that did NOT rewind over that frame is
 * playing a live match. The two worlds are different from that frame on and
 * nothing brings them back, because the disagreement is not in any byte either
 * side snapshots. Measured at run link100 (lane VS4BISECT): across twelve runs
 * the diverging window is always the one that simulated the match-end frame
 * exactly once while the others re-simulated it, and with the win condition
 * moved out of reach (--star-target 3, no match end) the same six pairings are
 * green every time.
 *
 * THE KING WORDS RIDE ALONG for the same reason: g_king_points is the score a
 * king match is decided on, g_king_tick is the deterministic per-second gate,
 * and g_king_armed is a latch whose arming zeroes both, so a rewind that loses
 * them credits a second twice or not at all and two consoles disagree about the
 * score. g_king_holder is the holder the live HUD and the next tick read.
 *
 * NOT CARRIED, deliberately, and this is luigi_infection.cpp's seed reasoning:
 * the ENV-DERIVED CONFIGURATION -- g_vs_players_cache, g_king_target, and the
 * on/want_scene/want_exit/grace/star_target caches inside the poll -- is parsed
 * once out of an environment every peer shares and never moves after, so
 * restoring it would copy bytes that cannot have changed. Neither are this
 * file's PRINT THROTTLES (last_shown in the countdown trace, last_stars /
 * last_holder / last_pts in the king probe, last_pending in the grace report):
 * they gate a line of the flight recorder, nothing reads them back, and the
 * worst a rewind can do to one is repeat or drop a diagnostic line. The
 * countdown trace already says so itself and checks g_port_rb_replaying.
 *
 * The block crosses as a flat word array with the layout in the getter rather
 * than as eleven arguments, because the slot carries it as bytes and
 * hal/rollback.cpp should not have to learn eleven names to do that.
 * port_vs_end_state_words is the size the two sides agree on, so a change to
 * kPortMaxPlayers is loud in the ring rather than silent past the end of a
 * slot. */
extern "C" int port_vs_end_state_words(void)
{
    return 9 + 2 * kPortMaxPlayers;
}

extern "C" void port_vs_end_state_get(int *w)
{
    int n = 0;
    w[n++] = g_end_fired;
    w[n++] = g_end_announced;
    w[n++] = g_end_by_target;
    w[n++] = g_end_by_king;
    w[n++] = g_end_by_luigi;
    w[n++] = g_end_total;
    for (int i = 0; i < kPortMaxPlayers; ++i) w[n++] = g_end_scores[i];
    w[n++] = g_king_tick;
    w[n++] = g_king_armed;
    w[n++] = g_king_holder;
    for (int i = 0; i < kPortMaxPlayers; ++i) w[n++] = g_king_points[i];
}

extern "C" void port_vs_end_state_set(const int *w)
{
    int n = 0;
    g_end_fired = w[n++];
    g_end_announced = w[n++];
    g_end_by_target = w[n++];
    g_end_by_king = w[n++];
    g_end_by_luigi = w[n++];
    g_end_total = w[n++];
    for (int i = 0; i < kPortMaxPlayers; ++i) g_end_scores[i] = w[n++];
    g_king_tick = w[n++];
    g_king_armed = w[n++];
    g_king_holder = w[n++];
    for (int i = 0; i < kPortMaxPlayers; ++i) g_king_points[i] = w[n++];
}

/* The king target, read once from the launcher-exported env and cached. This
   is the single source of truth every king-mode path (this file, level_boot's
   star-order seat, the CylinderClsn tiebreak and the PowerStar dupe guards)
   asks, so "is this a king match" is one answer, not several that can drift. */
extern "C" int port_vs_king_target(void)
{
    if (g_king_target < 0) {
        const char *e = std::getenv("SM64DS_VS_KING_TARGET");
        g_king_target = e ? std::atoi(e) : 0;
        if (g_king_target < 0) g_king_target = 0;
    }
    return g_king_target;
}

/* The live behaviour list, walked to count the stars IN PLAY for the king probe:
   node[1] is next, node[2] is the actor, its type id is at +0xc (0xb2 STAR /
   0xb3 SILVER_STAR) and its state at +0x440. A VS arena always carries the four
   table stars parked DORMANT in state 9 (the caged markers) -- those are not
   "a star in play", so they are excluded. What is counted is the awake star:
   collectable, held, tossed or dropped -- anything but state 9. The king
   one-star invariant is that this is never more than 1. */
extern int data_020a4b78[8];
static int king_star_count(void)
{
    int n = 0, guard = 0;
    for (int *node = (int *)(size_t)data_020a4b78[0];
         node && guard < 4096; node = (int *)(size_t)node[1], ++guard) {
        char *a = (char *)(size_t)node[2];
        if (!a) continue;
        const unsigned short id = *(unsigned short *)(a + 0xc);
        if ((id == 0xb2 || id == 0xb3) && *(int *)(a + 0x440) != 9) ++n;
    }
    return n;
}

/* The current holder of the one star: the LOWEST slot carrying a star. With the
   one-star invariant only one slot can be nonzero, but scanning low-to-high is
   an explicit deterministic tiebreak, so even a stray double-count can never
   make two consoles pick different holders. */
static int king_holder(int np)
{
    for (int i = 0; i < np && i < kPortMaxPlayers; ++i)
        if ((int)data_0209f310[i] >= 1) return i;
    return -1;
}

/* The live points line for the host overlay. Returns 0 (draws nothing) outside
   a king match. Marks the current holder with a '*'. */
extern "C" int port_vs_king_hud(char *out, int cap)
{
    if (port_vs_king_target() <= 0 || data_0209f2d8 != 1 || cap < 16)
        return 0;
    const int np = vs_players();
    int w = std::snprintf(out, (size_t)cap, "KING/%d", g_king_target);
    if (w < 0) w = 0;
    for (int i = 0; i < np && w < cap - 1; ++i) {
        const int k = std::snprintf(out + w, (size_t)(cap - w), " P%d:%d%s",
                                    i + 1, g_king_points[i],
                                    i == g_king_holder ? "*" : "");
        if (k < 0) break;
        w += k;
    }
    return 1;
}

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
    /* the count shown is the players still in the match (lane DROPOUT1) */
    int shown = 0;
    for (int i = 0; i < np; ++i)
        shown += vs_in_play(i);
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
                             pick[0], pick[1], pick[2], pick[3], shown);
    }
    default: {                               /* basic */
        const int mine = (me >= 0 && me < np) ? g_end_scores[me] : 0;
        if (me == best)
            return std::snprintf(out, (size_t)cap, "%d   (%d players)",
                                 g_end_scores[best], shown);
        return std::snprintf(out, (size_t)cap, "%d   you %d   (%d players)",
                             g_end_scores[best], mine, shown);
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
    /* LUIGI INFECTION names a TEAM, not a scoreline: the whole match is a race
       between the taggers and the survivors, so the banner says which side won
       and how many survivors were left rather than picking a slot by carried
       stars. */
    if (g_end_by_luigi != 0) {
        const int surv = port_luigi_survivors_alive();
        const int np_li = port_luigi_match_slots();
        if (g_end_by_luigi == 1)
            std::snprintf(out, (size_t)n,
                    "MATCH OVER  -  THE LUIGIS WIN  (all %d infected)", np_li);
        else
            std::snprintf(out, (size_t)n,
                    "MATCH OVER  -  THE SURVIVORS WIN  (%d of %d left)",
                    surv, np_li);
        return 1;
    }
    /* THE MECHANICAL HALF IS EXACT AT EVERY N. The winner, the draw test and
       the score list all run over the players who are actually in the match,
       not over four. What the DISPLAY does with sixteen scores is a separate
       question and is answered below. */
    const int np = vs_players();
    int ties = 0;
    const int best = vs_end_best(np, &ties);
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

/* ---- WHO IS STILL IN THE MATCH (multiplayer 1.0, lane DROPOUT1) ------------
 *
 * The carrier's seat mask (hal/comms_loopback.cpp, port_vs_seat_live_mask) is
 * the set of slots the round the game is simulating carried: the same on
 * every console at the same round, so anything keyed on it here is lockstep
 * state, never a local opinion. A slot that was in play and is no longer
 * (it quit with a Bye, was retired after 3 s of silence, or its console died)
 * has LEFT, from that round on, for good.
 *
 * Read once a frame from port_vs_match_end_hold, before the actor walk, so the
 * round the walk is about to run is the one it acts on. 0 from the carrier
 * means no session (solo, the VS debug row, a test with no transport) or no
 * round yet: nothing is tracked and every path below is the old one. Not under
 * rollback: a rewind restores the world to before a departure and this latch
 * would not run again on the replay (rollback returns after 1.0). Nothing
 * changes once the match is decided (g_end_fired): the scores are latched and
 * the consoles are about to close.
 * ------------------------------------------------------------------------- */
extern "C" unsigned port_vs_seat_live_mask(void);     /* hal/comms_loopback.cpp */
extern "C" unsigned port_vs_seat_mask_round(void);    /* ... and its round      */
extern "C++" { namespace port { bool comms_rb_mode(); } }   /* this block is extern "C" */

/* THE LEAVER'S BODY GOES, THE WAY THE CARTRIDGE TAKES A PLAYER OUT (plan G1).
   The DS drops a lost child mid-match in src/func_02030b58.c, the in-match
   wireless state's per-frame check: for each seat whose radio link is gone it
   sets that seat's bit in data_0209fc58 and clears data_0209fc5c[i], the "this
   seat is live" byte. From then on Player::Behavior returns at its first test
   for that seat in VS (src/_ZN6Player8BehaviorEv.cpp:73), Player::Render draws
   nothing (src/_ZN6Player6RenderEv.cpp:46), and the name tag, which reads the
   same byte (tests/nametag.h), is gone too. The port does not run that
   function (its closure is the WM stack; hal/arm9_tables_link100.cpp says
   why), so the same two writes are made here, from the seat mask, at the same
   round on every console. The ROM's loop covers its four seats, and
   data_0209fc58's upper bits mean other things (src/func_02030e4c.c), so seats
   4..15 clear only their live byte.
   PARKED as well, which the cartridge does not do. Its body would stay where
   it stood, invisible but still the nearest player for an enemy's
   dActor_c::ClosestPlayer, and still an icon on the bottom-screen map
   (Minimap::Behavior places every seated body whatever its live byte says).
   Moved far out of the level and stopped (no Behavior runs to move it back:
   no fall, no pit), it is nobody's nearest player and off the map. The spot is
   the body's own position plus fixed offsets -- lockstep state only, so the
   same spot on every console -- at the height the adventure ghosts park at
   (hal/comms_sync.cpp kGwParkUp, proven against ClosestPlayer). */
extern "C" unsigned char data_0209fc5c[];   /* per-seat live byte, 16 wide */
extern "C" unsigned char data_0209fc58;     /* the ROM's "seats lost" bits  */
enum : int {
    kVsParkUp   = 200000 << 12,              /* Fix12 world units */
    kVsParkSide = 100000 << 12,
};

static void vs_remove_body(int k, char *what, int cap)
{
    if (k < 4)
        data_0209fc58 = (unsigned char)(data_0209fc58 | (1u << k));
    data_0209fc5c[k] = 0;
    char *c = (char *)data_0209f394[k];
    if (!c) {
        std::snprintf(what, (size_t)cap, "no body to remove");
        return;
    }
    int *pos = (int *)(c + 0x5c);
    const int x = pos[0], y = pos[1], z = pos[2];
    pos[0] = x + kVsParkSide;
    pos[1] = y + kVsParkUp;
    pos[2] = z + kVsParkSide;
    *(int *)(c + 0x98) = 0;                  /* horizontal speed */
    *(int *)(c + 0xa8) = 0;                  /* vertical speed   */
    std::snprintf(what, (size_t)cap, "body removed where it stood (%d,%d,%d), "
                  "parked at (%d,%d,%d)", x >> 12, y >> 12, z >> 12,
                  pos[0] >> 12, pos[1] >> 12, pos[2] >> 12);
}

/* A LEAVER'S STARS SPILL WHERE IT STOOD (plan G1, Tango's call: spill them if
   the game's own drop does it cleanly). The drop is the ROM's own, the one a
   hit runs (Player::St_Hurt_Main -> _ZN6Player19func_ov002_020d94ccEv): one carried star off
   data_0209f310 and one VS star actor (0xb2) spawned at the body, flying the
   way the body faces. It is called once per carried star, turning the body a
   fraction of a circle between calls so the stars fan out instead of landing
   in one pile; the body is parked right after, so the turn is never seen. A
   star the ROM could not spawn (actor table full) is already off the count --
   the drop decrements first -- and so leaves with the player. The same calls
   at the same round on every console: the stars are lockstep state like any
   other drop. King of the Star's one star goes back on the field this way. */
extern "C" int _ZN6Player19func_ov002_020d94ccEv(char *self);
extern "C" char *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned id, char *prev);
extern "C" char *_ZN8dActor_c10FindWithIDEj(unsigned id);

/* THE DROP NEEDS A FREE STAR MARKER. The dropped star is tied to a marker
   (0xb4) whose own star is gone, picked at random among them
   (src/func_ov002_020e7554.c) -- `% count`, and with no free marker that is a
   division by zero, which the DS's software divide shrugs off and an x86
   divide does not. Every carried star freed the marker it came from, so in
   play there is always one per carried star; this counts them the ROM's way
   first anyway, and a star with no marker to go to leaves with the player
   instead of taking every console down with it. */
static int vs_free_star_markers(void)
{
    int n = 0;
    char *m = 0;
    while (n < 5 && (m = _ZN8dActor_c15FindWithActorIDEjPS_(0xb4, m)) != 0) {
        const unsigned char st = *(unsigned char *)(m + 0x1d8);
        const int gone = _ZN8dActor_c10FindWithIDEj(*(unsigned *)(m + 0x1cc)) == 0;
        if ((st == 3 && gone) ||
            (st != 0 && (*(unsigned char *)(m + 0x1db) & 1) && gone))
            ++n;
    }
    return n;
}

static int vs_spill_stars(int k)
{
    char *c = (char *)data_0209f394[k];
    const int n = data_0209f310[k];
    if (!c || n <= 0)
        return 0;
    short *yaw = (short *)(c + 0x94);
    const short yaw0 = *yaw;
    int spilled = 0;
    for (int i = 0; i < n && data_0209f310[k] > 0; ++i) {
        if (vs_free_star_markers() == 0)
            break;
        *yaw = (short)(yaw0 + i * (0x10000 / n));
        spilled += _ZN6Player19func_ov002_020d94ccEv(c) ? 1 : 0;
    }
    *yaw = yaw0;
    return spilled;
}

/* SM64DS_VS_LEAVE_PROBE=1 (a proof instrument; reads only): for 150 frames
   after a seat leaves, every 15 frames, one line with the removed body (its
   live byte, where it is), where the bottom-screen map would put its icon and
   whether Minimap::Render's own on-screen and same-map tests would draw it
   (the sixteen-wide host copy, port/unmatched/Minimap_Render.cpp), and every
   VS star (0xb2) on the field. */
extern "C" int g_mm_iconX[16];                /* the minimap's per-seat blips,  */
extern "C" int g_mm_iconY[16];                /* sixteen wide (port/unmatched/  */
extern "C" signed char g_mm_mapID[16];        /* Minimap_wide.h)                */
extern "C" signed char data_ov002_02111148;   /* the map area being shown */
extern "C" void *data_0209b468;               /* the live-actor list head */
static int g_leave_probe = -1;
static int g_leave_probe_left;                /* frames of probing left */
static int g_leave_probe_tick;
static unsigned g_leave_probe_slots;

static void vs_leave_probe(void)
{
    if (g_leave_probe_left <= 0)
        return;
    --g_leave_probe_left;
    if (g_leave_probe_tick++ % 15)
        return;
    char buf[1400];
    int w = 0;
    buf[0] = 0;
    for (int k = 0; k < kPortMaxPlayers && w < 1000; ++k) {
        if (!(g_leave_probe_slots >> k & 1u))
            continue;
        const char *c = (const char *)data_0209f394[k];
        const int *p = c ? (const int *)(c + 0x5c) : 0;
        w += std::snprintf(buf + w, sizeof buf - (size_t)w,
                           " slot%d live=%u pos=(%d,%d,%d)", k,
                           (unsigned)data_0209fc5c[k], p ? p[0] >> 12 : 0,
                           p ? p[1] >> 12 : 0, p ? p[2] >> 12 : 0);
        {
            const int ix = g_mm_iconX[k], iy = g_mm_iconY[k];
            const int id = g_mm_mapID[k];
            const int shown = (unsigned short)(iy + 0x10) < 0xe0 &&
                              (unsigned short)(ix + 0x10) < 0x120 &&
                              id == data_ov002_02111148;
            w += std::snprintf(buf + w, sizeof buf - (size_t)w,
                               " map_icon=(%d,%d) map_id=%d/%d drawn=%d",
                               ix, iy, id, (int)data_ov002_02111148, shown);
        }
    }
    struct Node { void *x0; Node *next; char **x8; };
    int stars = 0;
    for (Node *nd = *(Node **)&data_0209b468; nd && w < 1300; nd = nd->next) {
        const char *a = (const char *)nd->x8;
        if (!a || *(const unsigned short *)(a + 0xc) != 0xb2)
            continue;
        const int *p = (const int *)(a + 0x5c);
        ++stars;
        w += std::snprintf(buf + w, sizeof buf - (size_t)w, " star(%d,%d,%d)",
                           p[0] >> 12, p[1] >> 12, p[2] >> 12);
    }
    fprintf(stderr, "[vsleave] t%d%s | %d VS star(s) on the field\n",
            g_leave_probe_tick - 1, buf, stars);
}

/* TEMPORARY proof fixture, SM64DS_VS_GIVE_STARS=<slot>:<frame>[,<frame>...]:
   on each frame, before the walk, arm the touch of the first reachable VS star
   (PowerStar state 4 or 8, the ones _ZN8daStar_c19func_ov002_020e930cEv reads) with <slot>'s
   body, the way SM64DS_STAR_TRIGGER (hal/input_probe.cpp) arms it with the
   first body -- so the ROM's own collect handler hands that player a real star
   and frees its marker. It lets a proof make a JOINER carry stars and then
   take it out of the match; the ladder's trigger only ever scores for slot 0.
   Same env on every console, same frame: lockstep state. Inert unless set.
   g_vs_frame is the walk's frame number, the one the star trigger counts in
   (the end poll records it). */
static int g_vs_frame = -1;

static void vs_give_fixture(void)
{
    static int slot = -2;
    static const char *frames;
    if (slot == -2) {
        slot = -1;
        if (const char *e = std::getenv("SM64DS_VS_GIVE_STARS")) {
            char *end;
            const long v = std::strtol(e, &end, 10);
            if (end != e && *end == ':' && v >= 0 && v < kPortMaxPlayers) {
                slot = (int)v;
                frames = end + 1;
            }
        }
    }
    if (slot < 0)
        return;
    const int now = g_vs_frame + 1;
    int want = 0;
    for (const char *p = frames; *p;) {
        char *end;
        const long f = std::strtol(p, &end, 10);
        if (end == p)
            break;
        if (f == now) { want = 1; break; }
        p = (*end == ',') ? end + 1 : end;
        if (end == p) break;
    }
    const char *body = (const char *)data_0209f394[slot];
    if (!want || !body)
        return;
    struct Node { void *x0; Node *next; char **x8; };
    for (Node *nd = *(Node **)&data_0209b468; nd; nd = nd->next) {
        char *a = (char *)nd->x8;
        if (!a || *(unsigned short *)(a + 0xc) != 0xb2)
            continue;
        const int st = *(int *)(a + 0x440);
        if (st != 4 && st != 8)
            continue;
        *(unsigned int *)(a + 0x134) = *(const unsigned int *)(body + 0x4);
        *(unsigned int *)(a + 0x130) |= 0x400000;
        fprintf(stderr, "  [vsgive] f%d armed a star's touch (state %d) with "
                "slot %d's body; slot %d carries %d before the collect\n", now,
                st, slot, slot, (int)data_0209f310[slot]);
        return;
    }
    fprintf(stderr, "  [vsgive] f%d no reachable star to arm\n", now);
}

static void vs_track_seats(void)
{
    vs_leave_probe();
    if (data_0209f2d8 == 1)
        vs_give_fixture();
    if (data_0209f2d8 != 1 || g_end_fired >= 0)
        return;
    const unsigned live = port_vs_seat_live_mask();
    if (live == 0 || port::comms_rb_mode())
        return;
    const int np = vs_players();
    const unsigned all = np >= 32 ? ~0u : ((1u << np) - 1u);
    g_vs_seen |= live & all;
    const unsigned gone = g_vs_seen & ~live & ~g_vs_left & all;
    if (!gone)
        return;
    const unsigned round = port_vs_seat_mask_round();
    for (int k = 0; k < np; ++k) {
        if (!(gone >> k & 1u))
            continue;
        g_vs_left |= 1u << k;
        const char *nick = vs_name_for(k);
        const int had = data_0209f310[k] > 0 ? (int)data_0209f310[k] : 0;
        const int spilled = vs_spill_stars(k);
        char what[200];
        vs_remove_body(k, what, (int)sizeof what);
        fprintf(stderr, "[vs] slot %d left at round %u (%s): %s; %d of its %d "
                "star(s) spilled where it stood; %d player(s) still in the "
                "match\n", k, round, nick ? nick : "no name", what, spilled,
                had, vs_popcount(g_vs_seen & ~g_vs_left));
    }
    fflush(stderr);
    if (g_leave_probe < 0)
        g_leave_probe = std::getenv("SM64DS_VS_LEAVE_PROBE") ? 1 : 0;
    if (g_leave_probe) {
        g_leave_probe_slots |= gone;
        g_leave_probe_left = 150;
        g_leave_probe_tick = 0;
        vs_leave_probe();
    }
}

/* THE LAST PLAYER STANDING: the slot that is alone in a match two or more
   people started, or -1. The host is the round clock, so this is the host in
   practice (a host that leaves ends the match for everyone instead,
   comms_loopback.cpp host_left). */
static int vs_last_standing(void)
{
    const unsigned in = g_vs_seen & ~g_vs_left;
    if (vs_popcount(g_vs_seen) < 2 || vs_popcount(in) != 1)
        return -1;
    for (int k = 0; k < kPortMaxPlayers; ++k)
        if (in >> k & 1u)
            return k;
    return -1;
}

extern "C" void port_vs_match_end_hold(void)
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_VS_END_FREEZE");
        on = (e && e[0] == '0') ? 0 : 1;
    }
    /* ARENA MODE (lane STARREF1): the referee client's pre-walk half -- the
       stream, the claims, the stars and hits on this screen. Inert outside an
       arena with a referee. */
    port::vs_ref_pre_walk(0);
    vs_track_seats();
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

/* ==== SEAMVS1: THE VS MATCH END RETURNS TO THE TITLE IN THIS PROCESS (run after093; the one-process plan's M5a) =================
 * The end path below used to close the game once the winner had been on screen for the grace window ("closing IS going to the lobby":
 * the launcher was the lobby and started the next match as a new process). With the launcher hand-off (hal/handoff_poll.h) the
 * running game takes the next match itself, so the match now ends the way a quit row does: the title is asked for with the ROM's own
 * call (dScene_c::StartSceneFade(1, 0, 0)), port_front_end_in_place lets the level-to-scene crossing take scene 1 (SEAMQUIT1), the ROM
 * fades and marks the Stage, the crossing tears it down through Stage::CleanupResources and runs the title in this window. The ROM's
 * Stage::Behavior keeps asking for scene 7 (the VS results screen, not hosted) every frame its end guard is open; while this end is
 * under way that ask is answered with the title instead of the sentinel, so the fade runs to the title.
 *
 * AT THE TITLE (the first game frame with the title live; port_sdat_frame_hook, chained):
 *   1. the arena is taken down (hal/vs_arena.cpp vs_arena_leave_reset: the hub torn down with SEAMHUB1's comms_hub_teardown, the
 *      arena's install latch and comms_hub_install_arena's forgotten, the referee client, eat, grab and puppets reset), or a plain hub;
 *   2. the ghost layer, the hub body count and presence back to a process that never joined (SEAMHUB1's leave resets);
 *   3. the session record cleared and every launcher name out of both copies of the environment (hal/online_join.cpp
 *      port_session_env_clear: the 43 names the restart clears), the Adventure flag's env latch read again;
 *   4. the mods latched again to the offline list (hal/mods.cpp port_mods_relatch_after_level);
 *   5. the save medium back to the single player save (ntr/backup.cpp select_online(0));
 *   6. this file's VS latches back to unread (the end state, the players, the names, the king target, the end knobs), the Luigi
 *      Infection latch reset, the game mode word data_0209f2d8 back to 0 (adventure: what the title's own file pick writes,
 *      src/game/stages/d_s_title.cpp, and what main() writes before a first level);
 *   7. one line says what is now in effect, and the SETTINGS line is armed again for the next level.
 * SM64DS_VS_EXIT_ON_END=1 keeps the old close (a tester knob; the launcher never sets it); =0 keeps the old opt-out (the match stays on
 * screen). REFUSALS take the old close (exit 0), with one line: the lockstep carrier (hal/comms_loopback.cpp has no teardown), a
 * scene-booted process, a build that does not host the title; at the title a hub thread that does not stop or a relatch the mods layer
 * refuses. */
extern "C" {
int port_scene_live_id(void);                          /* hal/scene_boot.cpp: 1 = the title is the live scene */
int port_scene_is_hosted(int id);
extern void (*port_sdat_frame_hook)(void);             /* hal/sdat/consumer.cpp */
extern int port_front_end_in_place;                    /* hal/level_change.cpp (SEAMQUIT1) */
int port_session_env_clear(int *fields, char *names, int cap);   /* hal/online_join.cpp (SEAMVS1) */
int port_session_env_list_size(void);
int port_mods_relatch_after_level(const char *reason); /* hal/mods.cpp */
int port_mods_count(void);
int port_mods_multiplayer(void);
const char *port_mods_id(int i);
const char *port_mods_kind(int i);
void port_join_settings_rearm(void);                   /* hal/menu_join_plate.cpp */
void port_luigi_match_reset(void);
void port_window_quit(int code);                       /* hal/handoff_poll.cpp: WM_QUIT, the window's own way out */
}
extern "C++" {   /* this part of the file sits inside an extern "C" block: these are C++ names */
namespace ntr { namespace backup { void select_online(int on); bool online_selected(); } }
namespace port {
bool vs_arena_leave_reset(const char *why);            /* hal/vs_arena.cpp (SEAMVS1) */
bool comms_hub_active();
int comms_hub_state();
bool comms_hub_teardown(const char *why);
void comms_hub_reports_off();
void comms_sync_reports_off();
void comms_sync_leave_reset();
void comms_conductor_leave_reset();
void hub_presence_leave_reset();
bool adventure_ghost_mode();
struct CommsTransport;
const CommsTransport *comms_transport();
bool comms_loopback_leave_reset(const char *why);   /* hal/comms_loopback.cpp (KNOWNNET1) */
}
}

static int g_vs_end_ip;                 /* 0 none, 1 asked (the fade to the title runs), 2 done at the title */
static int g_vs_end_at;                 /* the frame it was asked */
static const char *g_vs_end_why = "";
static void (*g_vs_end_prev)(void);
static int g_vs_matches;                /* VS matches this process has ended in place */
/* the end poll's env-derived knobs, at file scope so a match that ends in place can have them read again for the next one */
static int g_mep_on = -1, g_mep_want_scene = -1, g_mep_want_exit = -1, g_mep_grace = -1, g_mep_star_target = -1;

extern "C" int port_vs_end_pending(void) { return g_vs_end_ip == 1; }
extern "C" int port_vs_matches_ended_in_place(void) { return g_vs_matches; }

/* every VS latch of this file back to a process that never played VS: the disarm the poll makes outside VS, plus the env-derived
   configuration (players, names, king target, the end knobs) read again by the next match */
static void vs_latches_reset(void)
{
    g_end_fired = -1;
    g_end_announced = 0;
    g_end_by_target = 0;
    g_end_by_king = 0;
    g_end_by_luigi = 0;
    g_end_by_last = 0;
    g_end_total = 0;
    for (int i = 0; i < kPortMaxPlayers; ++i) { g_end_scores[i] = 0; g_king_points[i] = 0; }
    port_luigi_match_reset();
    g_vs_seen = 0;
    g_vs_left = 0;
    g_scene7_answered = 0;
    g_arena_results = 0;
    g_king_armed = 0;
    g_king_tick = 0;
    g_king_holder = -1;
    g_king_target = -1;
    g_vs_players_cache = 0;
    g_vs_names_read = 0;
    g_vs_frame = -1;
    g_mep_on = g_mep_want_scene = g_mep_want_exit = g_mep_grace = g_mep_star_target = -1;
    /* the ROM's "the VS clock ran out" latch (src/ui/hud/d_meter.cpp sets it at 0; Stage::InitResources:288 writes 0 for a new Stage on
       the boot path, and a fresh process has 0). A Stage the level-to-scene crossing builds kept the 1 of the match before, and the
       next match's clock read as run out 1.5 s after its start: back to 0 with the rest of the match. */
    data_0209f204 = 0;
}

static const char *vs_end_refusal(void)
{
    static char why[200];
    if (g_vs_end_ip == 1) return "an in-place end is already under way";
    /* SEAMCLEAN1, DECISION (review item 20): LEFT, AND DOCUMENTED. A lockstep (non-arena) VS match has no teardown in
       hal/comms_loopback.cpp, so its end still closes the game (exit 0) and a lockstep hand-off is refused (exit 8): the old
       two-process path, one window at a time. The arena is the lobby's default netmode and is the in-place path; lockstep stays
       for developer pairs. A teardown would be a lane of its own (the carrier owns a thread, a socket and the frame ring). */
    /* KNOWNNET1: the lockstep carrier has its teardown now (comms_loopback_leave_reset, step 1 of vs_end_at_title), so a lockstep
       match ends in place too: the results, then the title in this process, no exit. */
    if (const char *sc = std::getenv("SM64DS_SCENE"))
        if (*sc) return "a scene-booted process";
    if (!port_scene_is_hosted(1)) return "this build does not host the title scene";
    for (int i = 0; i < port_mods_count(); ++i) {
        const char *k = port_mods_kind(i);
        if (k && !std::strcmp(k, "code")) {
            std::snprintf(why, sizeof why, "a code mod is loaded (%s) and a plugin cannot be unloaded", port_mods_id(i));
            return why;
        }
    }
    return 0;
}

static void vs_end_frame_hook(void);

static void vs_end_close(const char *why)
{
    fprintf(stderr, "[seamless] in place refused at the title: %s; closing the game as before (exit 0)\n", why);
    fflush(stderr);
    port_window_quit(0);
}

static void vs_end_at_title(void)
{
    g_vs_end_ip = 2;
    if (port_sdat_frame_hook != vs_end_frame_hook) {
        fprintf(stderr, "[seamless] note: another frame hook was chained after the VS end's; it stays chained\n");
    } else {
        port_sdat_frame_hook = g_vs_end_prev;
    }
    fprintf(stderr, "[seamless] the title is up (the VS match ended in place at f%d: %s): leaving the VS session in this process\n",
            g_vs_end_at, g_vs_end_why);
    /* 1. the lockstep carrier (KNOWNNET1), then the arena (or a plain hub) */
    if (port::comms_transport() && !port::comms_loopback_leave_reset("the VS match ended in place")) {
        vs_end_close("the lockstep carrier could not be torn down");
        return;
    }
    if (!port::vs_arena_leave_reset("the VS match ended in place")) {
        vs_end_close("the hub's I/O thread did not stop");
        return;
    }
    /* 2. the ghost layer, the body count, presence */
    port::comms_sync_leave_reset();
    port::comms_conductor_leave_reset();
    port::hub_presence_leave_reset();
    /* 3. the record and both environment copies */
    int fields = 0;
    char names[1024];
    const int n = port_session_env_clear(&fields, names, (int)sizeof names);
    fprintf(stderr, "[seamless] session record cleared (%d field(s)); %d of the %d launcher name(s) were set and are cleared from both "
            "copies of the environment: %s\n", fields, n, port_session_env_list_size(), names);
    fflush(stderr);
    /* 4. the mods back to the offline list */
    if (!port_mods_relatch_after_level("leaving the VS match")) {
        vs_end_close("the mods layer refused the switch back to the offline list");
        return;
    }
    /* 5. the single player save; 6. the VS latches and the game mode */
    ntr::backup::select_online(0);
    vs_latches_reset();
    data_0209f2d8 = 0;
    ++g_vs_matches;
    port_join_settings_rearm();
    fprintf(stderr, "[seamless] now solo after the VS match: save medium %s; adventure %d; multiplayer mod rule %d; %d mod(s) on; hub %s "
            "(state %d); game mode %d; VS latches reset (%d match(es) ended in place in this process); in-place quit flag %d until a "
            "level is handed back\n", ntr::backup::online_selected() ? "THE ONLINE SAVE" : "the single player save",
            port::adventure_ghost_mode() ? 1 : 0, port_mods_multiplayer(), port_mods_count(),
            port::comms_hub_active() ? "STILL ACTIVE" : "off", port::comms_hub_state(), (int)data_0209f2d8, g_vs_matches,
            port_front_end_in_place);
    fflush(stderr);
}

static void vs_end_frame_hook(void)
{
    if (g_vs_end_prev) g_vs_end_prev();
    if (g_vs_end_ip == 1 && port_scene_live_id() == 1) vs_end_at_title();
}

/* ask for the title in this process; 0 (with the refused line) when the old close must be taken */
static int vs_end_begin(int frame, const char *why)
{
    if (const char *r = vs_end_refusal()) {
        fprintf(stderr, "[seamless] in place refused: %s; closing as before\n", r);
        fflush(stderr);
        return 0;
    }
    g_vs_end_ip = 1;
    g_vs_end_at = frame;
    g_vs_end_why = why;
    port::comms_hub_reports_off();
    port::comms_sync_reports_off();
    g_vs_end_prev = port_sdat_frame_hook;
    port_sdat_frame_hook = vs_end_frame_hook;
    port_front_end_in_place = 1;
    _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
    fprintf(stderr, "[seamless] f%d the VS match ends in this process (%s): the title is asked for with the ROM's own "
            "StartSceneFade(1, 0, 0) (pending scene id %u); the arena stays up through the fade and the title tears it down\n", frame,
            why, (unsigned)data_02092664);
    fflush(stderr);
    return 1;
}

/* hal/vs_arena.cpp: a match handed to a running game that cannot go on (a seat with no body) leaves to the title in place */
extern "C" int port_vs_leave_to_title(const char *why) { return vs_end_begin(g_vs_frame, why); }

extern "C" int port_vs_match_end_poll(int frame)
{
    g_vs_frame = frame;
    /* Outside VS: nothing but the disarm, and the disarm is a branch taken at
       most once per match. No getenv on this path, ever. */
    if (data_0209f2d8 != 1) {
        if (g_end_fired >= 0) {
            g_end_fired = -1;
            g_end_announced = 0;
            g_end_by_target = 0;
            g_end_by_king = 0;
            g_end_by_luigi = 0;
            g_end_by_last = 0;
            /* Leaving VS clears the Luigi Infection latch too, so the next VS
               entry re-seeds one tagger from a clean team array. */
            port_luigi_match_reset();
        }
        /* and who was in the match: the next one starts with nobody gone */
        g_vs_seen = 0;
        g_vs_left = 0;
        g_scene7_answered = 0;
        g_arena_results = 0;
        /* Outside VS the king match is disarmed, so the next VS entry re-zeroes
           the points. This is the only reset point, so a king match always
           starts from zero however it was reached. */
        g_king_armed = 0;
        return 0;
    }

    /* ARENA MODE (lane STARREF1): the referee client's post-walk half, after
       the actor walk and before vs_arena_tick sends this frame's state. */
    port::vs_ref_post_walk(frame);

    /* SEAMVS1: file scope (g_mep_*), so a match that ends in place has them read again for the next one */
    int &on = g_mep_on, &want_scene = g_mep_want_scene, &want_exit = g_mep_want_exit, &grace = g_mep_grace,
        &star_target = g_mep_star_target;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_VS_MATCH_END");
        on = (e && e[0] == '0') ? 0 : 1;
        e = std::getenv("SM64DS_VS_END_SCENE");
        want_scene = e ? (e[0] != '0') : (int)PORT_VS_END_SCENE_DEFAULT;
        e = std::getenv("SM64DS_VS_EXIT_ON_END");
        /* SEAMVS1: unset (the launcher's way) = back to the title in this process; 1 = the old close; 0 = stay on the arena */
        want_exit = (e && e[0] == '0') ? 0 : (e && e[0] == '1') ? 1 : 2;
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
    /* ---- THE ROM'S OWN RESULTS REQUEST, ANSWERED (run mp1, lane VSTIMEUP1) --
     *
     * Stage::Behavior is the ROM's body now (_ZTV5Stage slot 6, port/
     * slice_gate220.txt), so its time-up statement runs for real: every frame
     * the end-of-match guard stays open it calls Scene::StartSceneFade(7, 0, 0),
     * the VS results screen, whatever this file decides. Left standing, the id
     * reaches Scene::BeforeBehavior on the next frame, which fades out and then
     * marks the Stage; scene 7 is not hosted, so hal/level_change.cpp refuses
     * the crossing and the cleanup Process aborts on the trapped slot 3.
     * Measured on the shipped kit and in a player's own match: every VS match
     * that ran to the clock ended in "FATAL: Stage vtable slot 3
     * (CleanupResources) is not hosted", 0xC0000409, on every console.
     *
     * THE PORT'S RESULTS SCREEN IS THIS END PATH: the winner banner over the
     * arena for the grace window, the MATCH OVER marker and the exit to the
     * launcher room, which shows the result. That is the ending a star-target
     * match already has. So a pending 7 in VS is the request this path serves,
     * and once served the sentinel goes back -- Scene::SpawnIfNecessary's own
     * second half, the statement port_scene_request_release completes for the
     * other scenes the port answers itself. The poll runs after the actor tick,
     * so the id is back before the next frame's BeforeBehavior reads it and the
     * Stage is never marked. The ROM asks again every frame the guard is open,
     * and every ask is answered the same way.
     *
     * Every console takes the same branch on the same frame: the id is written
     * by the ROM from lockstep state. SM64DS_VS_END_SCENE=1 leaves the request
     * standing -- the instrument that reproduces the abort from this binary. */
    if (data_02092664 == 7 && !want_scene) {
        if (!g_scene7_answered)
            fprintf(stderr, "  [vs] f%d the ROM asked for scene 7 (the VS results "
                    "screen): this port's results are the MATCH OVER banner and "
                    "the launcher room, so the request is answered here and the "
                    "pending id goes back to the sentinel (the Stage is not torn "
                    "down)\n", frame);
        ++g_scene7_answered;
        /* SEAMVS1: while the match ends in place the ask is answered with the title, so the ROM's fade runs to scene 1 */
        data_02092664 = g_vs_end_ip == 1 ? 1 : 0x187;
    }

    if (!on)
        return 0;

    /* ---- KING OF THE STAR: arm-time reset, then per-second accrual --------
     * All of this is a single compare against a cached constant when king mode
     * is off, so time and stars VS run exactly as before. */
    const int king_target = port_vs_king_target();
    if (king_target > 0 && !g_king_armed) {
        /* First live VS frame of a king match: our points to zero, and the
           ROM's carried-star array too so a stale in-process count cannot read
           as a holder on frame one (the re-arm trap above). King only. */
        const int np = vs_players();
        for (int i = 0; i < kPortMaxPlayers; ++i) g_king_points[i] = 0;
        for (int i = 0; i < np && i < kPortMaxPlayers; ++i) data_0209f310[i] = 0;
        g_king_tick = 0;
        g_king_holder = -1;
        g_king_armed = 1;
        fprintf(stderr, "[vs] KING OF THE STAR: one star all match, the holder "
                "scores +1/sec, first to %d wins (SM64DS_VS_KING_TARGET)\n",
                king_target);
    }
    if (king_target > 0 && g_end_fired < 0) {
        /* The holder this frame, and a deterministic 60-frame gate so exactly
           one point per real second is credited to whoever holds the star.
           Both the holder and the counter are shared lockstep state, so every
           console credits the same slot on the same frame. */
        const int holder = king_holder(vs_players());
        g_king_holder = holder;
        if (++g_king_tick >= 60) {
            g_king_tick = 0;
            if (holder >= 0) ++g_king_points[holder];
        }
        /* SM64DS_VS_KING_PROBE=1: a flight-recorder line on any change of star
           count, holder or total points. stars is the ONE-STAR proof (never 2),
           holder+points are the accrual proof, and it is deterministic shared
           state so two consoles print the same sequence. */
        static int king_probe = -1;
        if (king_probe < 0)
            king_probe = std::getenv("SM64DS_VS_KING_PROBE") ? 1 : 0;
        if (king_probe) {
            static int last_stars = -1, last_holder = -2, last_pts = -1;
            const int stars = king_star_count();
            int psum = 0;
            for (int i = 0; i < vs_players() && i < kPortMaxPlayers; ++i)
                psum += g_king_points[i];
            if (stars != last_stars || holder != last_holder
                || psum != last_pts) {
                last_stars = stars; last_holder = holder; last_pts = psum;
                fprintf(stderr, "[king] f%d stars=%d holder=%d points=%d,%d,%d,%d\n",
                        frame, stars, holder, g_king_points[0], g_king_points[1],
                        g_king_points[2], g_king_points[3]);
                fflush(stderr);
            }
        }
    }

    if (g_end_fired < 0 && port::vs_ref_active()) {
        /* ---- ARENA MODE: THE REFEREE ENDS THE MATCH (lane STARREF1) ---------
         *
         * One authority, so there can never be two winners. None of the five
         * triggers below runs here: first-to-N, the clock, the last player left
         * are all the referee's (referee.py _over), and the ROM's own time-up
         * only tells the client layer to expect the verdict; the client gives up
         * after 3 s with "connection lost". What arrives is MATCH_OVER with the
         * final ledger; it feeds the same latches, the same grace window, the
         * same marker and the same exit as every other end. */
        if (data_ov002_02111188 == 0 && data_0209f204 != 0)
            port::vs_ref_note_local_timeup(frame);
        port::VsRefOver ov;
        if (!port::vs_ref_match_over(&ov))
            return 0;
        const int np_arena = vs_players();
        g_end_fired = frame;
        g_end_by_luigi = 0;
        g_end_by_king = 0;
        g_end_by_last = 0;
        g_end_by_target = (ov.reason == arena_wire::OVER_FIRST_TO_N);
        g_arena_results = (ov.reason == arena_wire::OVER_TIME_UP) ? 1 : 2;
        g_vs_seen = (unsigned)ov.seen_mask;
        g_vs_left = (unsigned)(ov.seen_mask & ~ov.roster_mask);
        int total = 0;
        for (int i = 0; i < np_arena && i < kPortMaxPlayers; ++i) {
            g_end_scores[i] = vs_in_play(i) ? (int)ov.scores[i] : 0;
            if (g_end_scores[i] > 0) total += g_end_scores[i];
        }
        g_end_total = total;
        fprintf(stderr, "[vs] f%d ARENA MATCH OVER from the referee: reason=%d "
                "winners=0x%04x scores %d,%d,%d,%d%s\n", frame, ov.reason,
                (unsigned)ov.winners_mask, g_end_scores[0], g_end_scores[1],
                g_end_scores[2], g_end_scores[3],
                ov.local_timeout ? " (the referee never answered: connection lost)"
                                 : "");
        Sound::StopLoadedMusic_Layer1(0x3c);
        return 0;
    }
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
        /* ---- TRIGGER THREE: KING OF THE STAR, first to N points. Lowest slot
         * first, so if two ever crossed on one frame (they cannot, only the
         * holder accrues) it resolves deterministically. Feeds the same one
         * end path as the other two triggers. */
        int king_winner = -1;
        if (king_target > 0) {
            for (int i = 0; i < np_end; ++i)
                if (g_king_points[i] >= king_target) { king_winner = i; break; }
        }
        /* ---- TRIGGER FOUR: LUIGI INFECTION. Two ways a Luigi Infection match
         * ends, both dual-win rather than a single scoreline:
         *   - every survivor infected -> the LUIGIS win, and it can fire before
         *     the clock (the point of the mode);
         *   - the clock runs out with a survivor still alive -> the SURVIVORS
         *     win. The clock is the ROM's own match timer by default (timeup),
         *     or the SM64DS_VS_LUIGI_TIME test override measured from the frame
         *     the tagger is seeded -- countdown end, kLiCdPick -- so the match
         *     clock starts AFTER the pre-round countdown, not during it.
         * Only after the seed has run, so an empty team array on frame one is
         * never read as "everyone infected". Feeds the same one end path. */
        int luigi_end = 0;   /* 0 none, 1 Luigis win, 2 survivors win */
        if (port_luigi_enabled() && port_luigi_seeded()) {
            const int surv = port_luigi_survivors_alive();
            if (surv == 0) {
                luigi_end = 1;
            } else {
                const int tf = port_luigi_time_frames();
                const int expired = tf > 0 ? (frame >= kLiCdPick + tf) : timeup;
                if (expired) luigi_end = 2;
            }
        }
        /* ---- TRIGGER FIVE: THE LAST PLAYER STANDING (lane DROPOUT1, plan G4).
         * Everybody else left a match two or more people started: the one who
         * stayed wins it, with the normal MATCH OVER and exit 0, where the
         * console used to stop with LOST THE VS MATCH and exit 5 (a win read
         * as a loss). Keyed on the seat mask, so it is the same round on every
         * console that is still running -- which here is the one. Feeds the
         * same one end path; the marker says win=time-up (the match ended
         * before any target, and the launcher and the lobby know no other
         * word for that) and names the winner in winner=. */
        const int last = vs_last_standing();
        if (!timeup && star_winner < 0 && king_winner < 0 && luigi_end == 0 &&
            last < 0)
            return 0;
        g_end_by_luigi = luigi_end;
        g_end_fired = frame;
        g_end_by_target = (star_winner >= 0);
        g_end_by_king = (king_winner >= 0);
        g_end_by_last = (!timeup && star_winner < 0 && king_winner < 0 &&
                         luigi_end == 0) ? last + 1 : 0;
        /* run vs16: latch every player's score. data_0209f310 is hosted 32
           bytes wide as a guarded contiguous band (hal/actor_classes_star.cpp),
           so sixteen is inside the storage the port already had -- this loop
           was the only thing stopping at four. */
        /* King mode is scored by POINTS, not carried stars: latch g_king_points
           so the banner, the winner pick and the marker all read the score the
           king match was actually decided on (this also makes a king time-up
           pick the points leader, not the carried-star leader). */
        /* A slot that LEFT the match scores nothing (lane DROPOUT1, plan G2):
           a player who quit while leading is never named the winner -- not
           by the banner, not by the marker the launcher posts. */
        for (int i = 0; i < np_end; ++i)
            g_end_scores[i] = !vs_in_play(i) ? 0
                            : (king_target > 0) ? g_king_points[i]
                                                : (int)data_0209f310[i];
        g_end_total = (int)NumVsStarsObtained();
        if (luigi_end != 0)
            fprintf(stderr, "[vs] f%d LUIGI INFECTION OVER: %s (survivors "
                    "alive=%d, players=%d)\n", frame,
                    luigi_end == 1 ? "all survivors infected, the LUIGIS win"
                                   : "the clock ran out, the SURVIVORS win",
                    port_luigi_survivors_alive(), port_luigi_match_slots());
        else if (g_end_by_last)
            fprintf(stderr, "[vs] f%d LAST PLAYER STANDING: every other player "
                    "left the match, so slot %d wins it (%d of %d players left "
                    "during the match)\n", frame, g_end_by_last - 1,
                    vs_popcount(g_vs_left), vs_popcount(g_vs_seen));
        else if (king_winner >= 0)
            fprintf(stderr, "[vs] f%d KING TARGET REACHED: player %d has %d "
                    "point(s), the target is %d (points %d,%d,%d,%d)\n", frame,
                    king_winner, g_king_points[king_winner], king_target,
                    g_king_points[0], g_king_points[1],
                    g_king_points[2], g_king_points[3]);
        else if (star_winner >= 0)
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
            _ZN8dScene_c14StartSceneFadeEjjt(7, 0, 0);
            data_02092778 = 1;
            data_0209d4b0 = 0;
            fprintf(stderr, "  [vs] Scene::StartSceneFade(7, 0, 0): the VS "
                    "RESULTS screen requested, pending scene id now %u\n",
                    (unsigned)data_02092664);
        } else {
            /* The port makes no request of its own. The ROM's Stage::Behavior
               makes it from the next frame on, and the block at the top of
               this function answers it. */
            fprintf(stderr, "  [vs] the port makes no results-screen request "
                    "of its own (SM64DS_VS_END_SCENE is off, the default); the "
                    "ROM's own Stage::Behavior asks for scene 7 and this end "
                    "path answers it. Pending scene id now %u.\n",
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
        int t = 0;
        const int b = vs_end_best(np_marker, &t);
        if (!t && g_end_by_luigi == 0) win_slot = b;   /* a Luigi Infection end is a TEAM's: no star scorer is named the winner */
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
    /* results_screen IN AN ARENA (lane RESULTS1): the field says how the match
       ended, not when this console's own clock happened to run out.

       Lockstep reads g_scene7_answered, which is safe there because the clock
       is lockstep state: every console's ROM asks for scene 7 on the same
       frame. In an arena the referee ends the match on its own wall clock and
       each console's ROM clock counts that console's own frames, so the ROM's
       ask lands anywhere from a few frames before to a few seconds after the
       referee's MATCH OVER, and whether it falls inside the grace window is a
       coin flip per console. Measured on the final build-4 exes: the same
       4-player match printed results_screen=port on two consoles and =none on
       the other two (arena4-loss20-relay, arena4-leave-kill-relay), and on
       the kit zip's first arena4cam run on p0 only. What a player SAW was the
       same on all four (the winner banner for the grace window, then the exit;
       the ask is answered and draws nothing), so it was the marker text alone.

       The referee's reason is the same on every console, and it is what the
       ROM's ask stands for: a time-up end is the end the ROM answers with the
       results request (port, as in lockstep), a first-to-N or last-standing
       end is not (none, as in lockstep). The ask the ROM makes is still
       answered every frame by the block at the top of this function, whenever
       it lands. */
    fprintf(stderr, "[vs] MATCH OVER f%d win=%s scores=%s players=%d total=%d "
            "pending_scene=%u results_screen=%s winner=%d%s%s\n", frame,
            g_end_by_luigi == 1 ? "luigi-all-infected"
              : g_end_by_luigi == 2 ? "survivor-timeout"
              : g_end_by_king ? "king-target"
                          : (g_end_by_target ? "star-target" : "time-up"),
            scores_field, np_marker,
            g_end_total, (unsigned)data_02092664,
            data_02092664 == 7 ? "REQUESTED-BUT-UNSERVICED"
              : g_arena_results ? (g_arena_results == 1 ? "port" : "none")
              : g_scene7_answered ? "port"
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
    /* SEAMVS1: the launcher's way now: back to the title in this process (the block over vs_end_begin) */
    if (want_exit == 2 && vs_end_begin(frame, "the winner was on screen for the whole grace window"))
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
   title-confirm path _ZN10dScTitle_c8BehaviorEv -- plus two the port adds, this one
   and hal/level_change.cpp:1415 (the port's copy of that ov003 path). It is
   the NEW FILE seat: lives to 4, health to 0x880 for all four players. This
   call stands in for StartFile's, so it belongs exactly where it is.

   Two of the ROM's three DO enter a level -- StartFile and _ZN10dScTitle_c8BehaviorEv
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
   the latched entry reason says fresh-or-death (src/game/player/Player.cpp:29),
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
    _ZN8dActor_c15GivePlayerCoinsER6Playerhj(actor, player,
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
    return _ZN5Sound8PlayLongEjjjRK7Vector3s(prev, 3, soundId, camSpacePos, 0);
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
/* func_0200ee8c RETIRED (run link100, lane SEAT6, batch B6).
   The u8-return ride-through is a DECLARATION, not a body: the callee
   src/GetStarCameraSetting.c returns `unsigned char` and ends
   `and r0,r0,#0xf / and r0,r0,#0xff`, so masking here said the same
   arithmetic a second time.
   The matched TU src/func_0200ee8c.c is seated in its place: port/tools/hostgen.py's ARG_WIDTH table declares the return the width
   the definition really has, which makes MSVC read AL alone.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
}  // extern "C"
