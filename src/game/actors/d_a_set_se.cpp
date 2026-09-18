//cpp
/**
 * The level's ambient sound emitter (SET_SE).
 *
 * One actor sustains one positional sound picked by its spawn param.
 * InitResources masks param1 into the sound table and records whether
 * level ambience is silenced; Behavior re-issues the sound every frame
 * through Sound::PlayLong, which recycles mSoundHandle instead of
 * stacking voices. Params 1-4 select the above-water set (0x100-0x103),
 * muted while the underwater music (0x32-0x34) plays or the camera is
 * submerged. On level 8 the actor only spawns once star 1 is collected
 * and play has moved past it.
 *
 * daSetSE_c_classInit is reconstructed (RTTI daSetSE_c, SET_SE registry).
 * Retail does not store that spelling.
 *
 * deslop
 * Leftover: the data_* externs keep their linker spellings; the verified
 *   names (LEVEL_ID, STAR_ID, CURRENT_GAMEMODE, CAMERA, SAVE_DATA,
 *   RUNNING_KUPPA_SCRIPT, WATERFALL_SOUNDS_DISABLED) live in
 *   symbols/verified.tsv and renaming is symbols.txt. data_0209b4ac is
 *   the current music ID, written by Sound::SetMusic/LoadAndSetMusic
 *   and cleared by StopLoadedMusic.
 * Leftover: g_profile_SET_SE is ov002 gap data outside this TU (S14), as
 *   are the sound table and the WATERFALL_SOUNDS_DISABLED flag this TU
 *   reads and writes.
 * Leftover: the PlayLong position is *(Vector3 *)&mCamSpacePosX, the
 *   inherited cam-space triple at 0x74 read as one vector; daBmb_c
 *   passes (char *)this+0x74 the same way. No shared overlay exists.
 * Leftover: InitResources keeps the volatile param1 RMW (param1 &= 0xf
 *   materializes an extra base pointer) and the (int)-cast re-tests
 *   (direct tests come out six words short of the ROM).
 */

#include "daSetSE_c.h"
#include "Camera.h"

/* Sound.h does not declare PlayLong yet, so this TU re-opens the namespace
   the way daBmb_c, daBrq_c and daObjPushblock_c do. Additive only: it does
   not restate anything the header already has. */
namespace Sound {
u32 PlayLong(u32 handle, u32 bank, u32 soundId, const Vector3 &pos, s16 pitch);
}

extern "C" {
extern int data_ov002_0210b498[];  /* sound-ID table, 11 entries: 0x18d, 0x100-0x103, 0x81, 0x89, 0x9c-0x9e, 0; the _ZTV9daSetSE_c header follows it, so spawn params stay 0-10 */
extern int data_ov002_02110aec;    /* WATERFALL_SOUNDS_DISABLED */
extern int data_0209b4ac;          /* current music ID */
extern Camera *data_0209f318;      /* CAMERA */
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;           /* LEVEL_ID */
extern u8 data_0209f220;           /* STAR_ID */
extern u8 data_0209f2d8;           /* CURRENT_GAMEMODE */
extern int data_0209caa0[];        /* SAVE_DATA */
extern int data_0209fc48;          /* RUNNING_KUPPA_SCRIPT */
}

// @symbol daSetSE_c_classInit
extern "C" daSetSE_c *daSetSE_c_classInit()
{
    return new daSetSE_c();
}

// @symbol _ZN9daSetSE_c13InitResourcesEv
int daSetSE_c::InitResources()
{
    int disabled;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    /* Volatile keeps CW from materializing an extra base pointer for this
       inherited-field RMW; the ROM loads and stores [this + 0x08] directly. */
    u32 soundID = *(volatile u32 *)((char *)this + 0x8);
    *(volatile u32 *)((char *)this + 0x8) = soundID & 0xf;

    /* The (int) casts are load-bearing, not noise: each forces its boolean
       to materialize into a word before the re-test, and spelling the
       tests directly comes out six words short of the ROM. */
    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        disabled = 1;
    else
        disabled = 0;

    data_ov002_02110aec = disabled;
    return 1;
}

// @symbol _ZN9daSetSE_c8BehaviorEv
int daSetSE_c::Behavior()
{
    u32 effect;
    int music;
    Camera *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    effect = param1;
    if (effect >= 1 && effect <= 4) {
        cam = data_0209f318;
        music = data_0209b4ac;
        if (music == 0x32 || music == 0x33 || music == 0x34 ||
            cam->IsUnderwater())
            return 1;
    }

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3,
        (u32)data_ov002_0210b498[param1], *(Vector3 *)&mCamSpacePosX, 0);
    return 1;
}

// @symbol _ZN9daSetSE_c6RenderEv
int daSetSE_c::Render()
{
    return 1;
}

// @symbol _ZN9daSetSE_c16OnPendingDestroyEv
void daSetSE_c::OnPendingDestroy()
{
}

// @symbol _ZN9daSetSE_c16CleanupResourcesEv
int daSetSE_c::CleanupResources()
{
    return 1;
}
