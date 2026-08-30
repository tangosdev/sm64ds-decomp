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
 * cnt >= data_0209fc50 is the ROM's own readiness gate: SetNumPlayers writes
 * data_0209fc50 (SetNumPlayers -> func_020308d0), and +0x711 is set by the
 * player's own level-enter step func_ov002_020c71e0, which is in the link and
 * dispatched through hal/player_states.inc:299. That same step then holds the
 * player where he lands while data_0209f2bc != 0 -- the DS's 3-2-1 freeze --
 * so seating the counter without ticking it down would stop the match dead.
 * The two land together for that reason.
 *
 * func_02012790 is Sound::Play2D(2, id): 0x2b three times for the counts,
 * 0x2a for the GO. Both are the ROM's ids at the ROM's frames. */
void port_vs_countdown_tick(void)
{
    if (data_0209f2d8 != 1)
        return;
    int cnt = 0;
    for (int i = 0; i < 4; i++) {
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
