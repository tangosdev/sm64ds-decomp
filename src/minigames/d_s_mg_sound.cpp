//cpp
/* Boom-box minigame. Each round deals mNotes on the touch screen; a touch
 * plays that pitch and pops icons in mPops. The class is a
 * dScMgSingle3DBase_c (its RTTI is at 0x0213f6e4).
 *
 * The compiler emits .text in reverse source order, so the functions run
 * here from the highest address down. Do not reorder.
 *
 * Leftover: DrawNotes reads mState as the raw word at 0x5608.
 *   The member spelling in the note draw loop grows it by 8 bytes.
 * Leftover: DealNotes's deal loop reaches mPairUses[0] and
 *   mPairNotes[0] through the stepped (dScMgSound_c *)q, q = this + pair.
 *   Indexing the members by pair grows the function by 8 bytes.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum dScMgSound_c_AssetId {
    ASSET_D_2D_MG_BG_SOUND2_NCG_BIN = 0x0098,
    ASSET_D_2D_MG_BG_SOUND2_NCL_BIN = 0x0099,
    ASSET_D_2D_MG_BG_SOUND2_NSC_BIN = 0x009a,
    ASSET_D_2D_MG_SOUND_NCG_BIN = 0x00ff,
    ASSET_D_2D_MG_SOUND_NCL_BIN = 0x0100
};

#include "dScMgSound_c.h"
#include "types.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "Sound.h"

/* A cursor keeps the add-#0x5000 split on a walk that advances the scene
   pointer. Pointing the record itself at the slot pools the base instead. */
struct dMgSoundNoteCur {
    char pad[0x50e8];
    dMgSoundNote_c note;
};
struct dMgSoundPopCur {
    char pad[0x51b0];
    dMgSoundPop_c pop;
};

/* The state/phase dispatch tables hold pointer-to-member records: the
   sinit copies {pmf, delta} descriptors into them. */
typedef void (dScMgSound_c::*Pmf)(int);
struct PmfEntry {
    Pmf pmf;
};


namespace G2S {
unsigned GetBG2CharPtr();
}

namespace GX {
void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

namespace GXS {
void LoadBGPltt(const void *data, u32 offset, u32 size);
void LoadOBJPltt(const void *data, u32 offset, u32 size);
}

#define RND (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)
#define LAUNDER(p) ((int)(p))

/* ROM PMF descriptor constants; the runtime tables below use native C++ PMFs. */
extern Pmf pmf_dScMgSound_c_Note_StateWait;
extern Pmf pmf_dScMgSound_c_Note_StateOpening;
extern Pmf pmf_dScMgSound_c_Note_StateOpen;
extern Pmf pmf_dScMgSound_c_Note_StateClosing;
extern Pmf pmf_dScMgSound_c_Note_StateScroll;
extern Pmf pmf_dScMgSound_c_Pop00_Update;
extern Pmf pmf_dScMgSound_c_Pop01_Update;
extern Pmf pmf_dScMgSound_c_Pop02_Update;
extern Pmf pmf_dScMgSound_c_Pop03_Update;
extern Pmf pmf_dScMgSound_c_Pop04_Update;
extern Pmf pmf_dScMgSound_c_Pop05_Update;
extern Pmf pmf_dScMgSound_c_Pop06_Update;
extern Pmf pmf_dScMgSound_c_Pop07_Update;
extern Pmf pmf_dScMgSound_c_Pop08_Update;
extern Pmf pmf_dScMgSound_c_Pop09_Update;
extern Pmf pmf_dScMgSound_c_Pop10_Update;
extern Pmf pmf_dScMgSound_c_Pop11_Update;
extern Pmf pmf_dScMgSound_c_Pop12_Update;
extern Pmf pmf_dScMgSound_c_Pop00_Start;
extern Pmf pmf_dScMgSound_c_Pop00_Fly;
extern Pmf pmf_dScMgSound_c_Pop00_Finish;
extern Pmf pmf_dScMgSound_c_Pop01_Start;
extern Pmf pmf_dScMgSound_c_Pop01_Fly;
extern Pmf pmf_dScMgSound_c_Pop01_Finish;
extern Pmf pmf_dScMgSound_c_Pop02_Start;
extern Pmf pmf_dScMgSound_c_Pop02_Fly;
extern Pmf pmf_dScMgSound_c_Pop02_Finish;
extern Pmf pmf_dScMgSound_c_Pop03_Start;
extern Pmf pmf_dScMgSound_c_Pop03_Fly;
extern Pmf pmf_dScMgSound_c_Pop03_Finish;
extern Pmf pmf_dScMgSound_c_Pop04_Start;
extern Pmf pmf_dScMgSound_c_Pop04_Fly;
extern Pmf pmf_dScMgSound_c_Pop04_Finish;
extern Pmf pmf_dScMgSound_c_Pop05_Start;
extern Pmf pmf_dScMgSound_c_Pop05_Fly;
extern Pmf pmf_dScMgSound_c_Pop05_Finish;
extern Pmf pmf_dScMgSound_c_Pop06_Start;
extern Pmf pmf_dScMgSound_c_Pop06_Fly;
extern Pmf pmf_dScMgSound_c_Pop06_Finish;
extern Pmf pmf_dScMgSound_c_Pop07_Start;
extern Pmf pmf_dScMgSound_c_Pop07_Fly;
extern Pmf pmf_dScMgSound_c_Pop07_Finish;
extern Pmf pmf_dScMgSound_c_Pop08_Start;
extern Pmf pmf_dScMgSound_c_Pop08_Fly;
extern Pmf pmf_dScMgSound_c_Pop08_Finish;
extern Pmf pmf_dScMgSound_c_Pop09_Start;
extern Pmf pmf_dScMgSound_c_Pop09_Fly;
extern Pmf pmf_dScMgSound_c_Pop09_Finish;
extern Pmf pmf_dScMgSound_c_Pop10_Start;
extern Pmf pmf_dScMgSound_c_Pop10_Fly;
extern Pmf pmf_dScMgSound_c_Pop10_Finish;
extern Pmf pmf_dScMgSound_c_Pop11_Start;
extern Pmf pmf_dScMgSound_c_Pop11_Fly;
extern Pmf pmf_dScMgSound_c_Pop11_Finish;
extern Pmf pmf_dScMgSound_c_Pop12_Start;
extern Pmf pmf_dScMgSound_c_Pop12_Fly;
extern Pmf pmf_dScMgSound_c_Pop12_Finish;

extern "C" {
extern int Hud_RenderSprite(int a, int b, int c, int d, int e);
extern int sSoundWinSpriteBSprites[];
/* WinSpriteB reads indices 0 through 2; the fourth stored value is unused here. */
extern const u16 sSoundWinSpriteBTimes[4] = {50, 3, 3, 50};
extern int sSoundWinSpriteASprites[];
/* Stored timing suffixes are retained; WinSpriteA reads only indices 0 and 1. */
extern const u16 sSoundWinSpriteATimes[4] = {50, 4, 50, 0};
extern void Mg_DrawStockCounter(int a, int b);
extern const short data_02082214[];
extern u8 data_ov006_0213f54c[];
extern u8 data_ov006_0213f544[];
extern u8 data_ov006_0213f65c[];
void *sSoundPopsSprites[3] = {data_ov006_0213f54c, data_ov006_0213f544, data_ov006_0213f65c};
extern const int sSoundPop12XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop12VX[3] = {0, 2560, -2560};
extern const int sSoundPop11XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop11VX[3] = {0, 3072, -3072};
extern const int sSoundPop10XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop10VX[3] = {0, 3584, -3584};
extern const int sSoundPop09XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop09VX[3] = {0, 4096, -4096};
extern const int sSoundPop08XOffsets[3] = {0, 65536, -65536};
extern const int sSoundPop07XOffsets[3] = {0, 65536, -65536};
extern const u32 sSoundPop06VX[3] = {0x0, 0x1000, 0xfffff000};
extern const int sSoundPop05XOffsets[3] = {0, 65536, -65536};
extern const int sSoundPop04VX[3] = {0, 3072, -3072};
extern const int sSoundPop03XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop03VX[3] = {0, 2048, -2048};
extern const int sSoundPop02XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop02VX[3] = {0, 2560, -2560};
extern const int sSoundPop01XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop01VX[3] = {0, 1536, -1536};
extern const int sSoundPop00XOffsets[3] = {0, 32768, -32768};
extern const int sSoundPop00VX[3] = {4096, 8192, -8192};
extern const u8 sSoundPop00Dirs[3] = {0, 0, 1};
/* SpawnPops consumes the first three selectors. The other two stored values
 * are retained; their original grouping is unknown. */
extern const u8 sSoundPopSpawnKinds[5] = {0, 1, 2, 2, 1};
extern void Presenter_PlayMatch(void *c);
extern void Presenter_PlayMiss(char *c);
extern int sSoundRowLengthAlt[];
extern int sSoundRowLength[];
extern int sSoundScrollRowLengthAlt[];
extern int sSoundScrollRowLength[];
/* Note frames use indices 0 through 6. The final stored timing value in each
 * array is retained without asserting its original purpose or declared bound. */
extern const u16 sSoundNoteClosingTimes[8] = {2, 2, 3, 4, 4, 4, 4, 4};
void Presenter_PlayCheerLoop(char *p);
void func_02012174(u32 bank, u32 id);
extern const u16 sSoundNoteOpeningTimes[8] = {2, 2, 3, 4, 4, 4, 6, 4};
/* Parallel playback selectors and sound IDs retain all 42 stored entries.
 * Some entries are not exercised by the current deal logic. */
u16 sSoundNotePlaybackKinds[42] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
                                   2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
u16 sSoundNoteSfx[42] = {70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  56,  57,  58,  59,
                         169, 170, 171, 172, 173, 174, 176, 177, 178, 266, 267, 268, 56,  120,
                         184, 248, 57,  121, 185, 249, 58,  122, 186, 250, 59,  123, 187, 251};
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern const u8 sSoundPitchCounts[5] = {10, 4, 10, 4, 14};
extern const u8 sSoundDealCounts[5] = {6, 6, 8, 6, 8};
extern const u8 sSoundNoteLayoutByPattern[5] = {0, 0, 1, 0, 1};
extern const int sSoundPairNoteOffsets[5] = {0, 10, 0, 14, 0};
/* The level selector uses indices 0 through 3; the fifth stored bank is retained. */
extern const int sSoundBankByLevel[5] = {3, 0, 1, 2, 3};
/* Interleaved x/y pairs for the six-, eight- and ten-note layouts.
 * The pointer table retains the ten-note layout although current dealing
 * selects only the first two. These descriptive names are reconstructed. */
u16 sSoundNotePositions6[12] = {64, 32, 128, 32, 192, 32, 64, 112, 128, 112, 192, 112};
u16 sSoundNotePositions8[16] = {32, 32,  96, 32,  160, 32,  224, 32,
                                32, 112, 96, 112, 160, 112, 224, 112};
u16 sSoundNotePositions10[20] = {32, 32,  80, 32,  128, 32,  176, 32,  224, 32,
                                 32, 112, 80, 112, 128, 112, 176, 112, 224, 112};
u16 *sSoundNotePositions[3] = {sSoundNotePositions6, sSoundNotePositions8, sSoundNotePositions10};
extern void Presenter_ResetPose(char *c);
extern void Presenter_PlayCheer(void *c);
extern "C" void Mg_DrawScoreTally(void *c, int a, int b, int d);
void FreeGfxSlotsById(int id);
int LoadFile(int handle);
void DecompressLZ16(int src, void *dst);
void Camera_InitPresenterView(void *p);
int Presenter_Load(void *p);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

/* The note and pop dispatch tables belong to this scene. Declaration
 * order preserves the cartridge static-initialization sequence. */

PmfEntry sSoundNoteStateTable[5] = {
    {pmf_dScMgSound_c_Note_StateWait},   {pmf_dScMgSound_c_Note_StateOpening},
    {pmf_dScMgSound_c_Note_StateOpen},   {pmf_dScMgSound_c_Note_StateClosing},
    {pmf_dScMgSound_c_Note_StateScroll},
};
PmfEntry sSoundPopUpdateTable[13] = {
    {pmf_dScMgSound_c_Pop00_Update}, {pmf_dScMgSound_c_Pop01_Update},
    {pmf_dScMgSound_c_Pop02_Update}, {pmf_dScMgSound_c_Pop03_Update},
    {pmf_dScMgSound_c_Pop04_Update}, {pmf_dScMgSound_c_Pop05_Update},
    {pmf_dScMgSound_c_Pop06_Update}, {pmf_dScMgSound_c_Pop07_Update},
    {pmf_dScMgSound_c_Pop08_Update}, {pmf_dScMgSound_c_Pop09_Update},
    {pmf_dScMgSound_c_Pop10_Update}, {pmf_dScMgSound_c_Pop11_Update},
    {pmf_dScMgSound_c_Pop12_Update},
};
Pmf sSoundPop00Table[3] = {
    pmf_dScMgSound_c_Pop00_Start,
    pmf_dScMgSound_c_Pop00_Fly,
    pmf_dScMgSound_c_Pop00_Finish,
};
PmfEntry sSoundPop01Table[3] = {
    {pmf_dScMgSound_c_Pop01_Start},
    {pmf_dScMgSound_c_Pop01_Fly},
    {pmf_dScMgSound_c_Pop01_Finish},
};
PmfEntry sSoundPop02Table[3] = {
    {pmf_dScMgSound_c_Pop02_Start},
    {pmf_dScMgSound_c_Pop02_Fly},
    {pmf_dScMgSound_c_Pop02_Finish},
};
PmfEntry sSoundPop03Table[3] = {
    {pmf_dScMgSound_c_Pop03_Start},
    {pmf_dScMgSound_c_Pop03_Fly},
    {pmf_dScMgSound_c_Pop03_Finish},
};
PmfEntry sSoundPop04Table[3] = {
    {pmf_dScMgSound_c_Pop04_Start},
    {pmf_dScMgSound_c_Pop04_Fly},
    {pmf_dScMgSound_c_Pop04_Finish},
};
PmfEntry sSoundPop05Table[3] = {
    {pmf_dScMgSound_c_Pop05_Start},
    {pmf_dScMgSound_c_Pop05_Fly},
    {pmf_dScMgSound_c_Pop05_Finish},
};
PmfEntry sSoundPop06Table[3] = {
    {pmf_dScMgSound_c_Pop06_Start},
    {pmf_dScMgSound_c_Pop06_Fly},
    {pmf_dScMgSound_c_Pop06_Finish},
};
PmfEntry sSoundPop07Table[3] = {
    {pmf_dScMgSound_c_Pop07_Start},
    {pmf_dScMgSound_c_Pop07_Fly},
    {pmf_dScMgSound_c_Pop07_Finish},
};
PmfEntry sSoundPop08Table[3] = {
    {pmf_dScMgSound_c_Pop08_Start},
    {pmf_dScMgSound_c_Pop08_Fly},
    {pmf_dScMgSound_c_Pop08_Finish},
};
Pmf sSoundPop09Table[3] = {
    pmf_dScMgSound_c_Pop09_Start,
    pmf_dScMgSound_c_Pop09_Fly,
    pmf_dScMgSound_c_Pop09_Finish,
};
Pmf sSoundPop10Table[3] = {
    pmf_dScMgSound_c_Pop10_Start,
    pmf_dScMgSound_c_Pop10_Fly,
    pmf_dScMgSound_c_Pop10_Finish,
};
PmfEntry sSoundPop11Table[3] = {
    {pmf_dScMgSound_c_Pop11_Start},
    {pmf_dScMgSound_c_Pop11_Fly},
    {pmf_dScMgSound_c_Pop11_Finish},
};
PmfEntry sSoundPop12Table[3] = {
    {pmf_dScMgSound_c_Pop12_Start},
    {pmf_dScMgSound_c_Pop12_Fly},
    {pmf_dScMgSound_c_Pop12_Finish},
};

/* The body is empty: everything the factory's `new` runs (dScMgBase_c's
   C2, dScMgSingle3DBase_c's vptr and mSysTracker, this class's vptr, then
   mTable's constructor) is the compiler's inlining of the base and member
   constructors. */
inline dScMgSound_c::dScMgSound_c() {}

// @symbol dScMgSound_c_classInit
/* The MG_SOUND registry factory. It sits directly after InitResources and
   installs this class's vtable, so it was compiled with the class. */
extern "C" void *dScMgSound_c_classInit(void) {
    return new dScMgSound_c;
}

// @symbol _ZN12dScMgSound_c13InitResourcesEv
/* Loads the minigame's graphics for both screens, sets up the camera and
   the table, then deals the first round. */
s32 dScMgSound_c::InitResources() {
    int objChars, objPltt, file;

    data_0209d45c |= 0x11;
    objChars = LoadFile(ASSET_D_2D_MG_SOUND_NCG_BIN);
    objPltt = LoadFile(ASSET_D_2D_MG_SOUND_NCL_BIN);
    DecompressLZ16(objChars, (void *)0x6400000);
    GX::LoadOBJPltt((const void *)objPltt, 0, 0x100);

    data_0209d454 |= 4;
    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NCG_BIN);
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x214;
    DecompressLZ16(file, (void *)G2S::GetBG2CharPtr());
    Deallocate((void *)file);

    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NCL_BIN);
    GXS::LoadBGPltt((const void *)file, 0x60, 0x1a0);
    Deallocate((void *)file);

    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NSC_BIN);
    func_02056374((const void *)file, 0, 0x800);
    Deallocate((void *)file);

    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    DecompressLZ16(objChars, (void *)0x6600000);
    GXS::LoadOBJPltt((const void *)objPltt, 0, 0x100);
    Deallocate((void *)objChars);
    Deallocate((void *)objPltt);

    /* pad_4660 is the head of the base's 0xbc camera (dScMgSingle3DBase_c.h);
       d_s_mg_cup.cpp passes it the same way. */
    Camera_InitPresenterView((void *)pad_4660);
    if (Presenter_Load((void *)&mTable) == 0)
        return 0;

    mTable.mQuiet = 1;
    ResetRound();
    DealNotes();
    mTries = 3;
    mState = 1;
    mIntroTimer = 0x20;
    Mg_ResetRecordBannerPos();
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);
    mHudScore = 0;
    return 1;
}

// @symbol _ZN12dScMgSound_c8BehaviorEv
/* One frame of the round, by mState: 0 starts; 1 plays the round while
   mIntroTimer controls the prompt; 2 waits for the result, then celebrates
   a win or advances to 3 for the loss delay. Wins increment the HUD score
   up to 9999.
   The `(int)` casts on the three countdown decrements are needed: without
   them the address folds into the decrement and the function comes out
   short. */
s32 dScMgSound_c::Behavior() {
    char *raw = (char *)this;

    switch (mState) {
    case 0:
        mState = 1;
        break;
    case 1:
        if (mIntroTimer != 0) {
            (*(u16 *)(int)&mIntroTimer)--;
            if (mIntroTimer == 0) {
                FreeGfxSlotsById(0x1d);
                if (mPromptBlinkCount == 0) {
                    mPromptEnabled = 1;
                    mPromptBlinkCount = 1;
                    mPromptBlinkTimer = 0;
                }
            }
        }
        UpdateNotes();
        CheckPair();
        UpdatePops();
        CheckRoundOver();
        break;
    case 2:
        UpdateNotes();
        UpdatePops();
        if (mResultTimer != 0) {
            (*(u16 *)(int)&mResultTimer)--;
            if (mResultTimer == 0) {
                if (mTries != 0) {
                    mTable.mQuiet = 0;
                    Presenter_PlayCheer((char *)&mTable);
                    if (mTries == 3)
                        Mg_SetPerfectBanner();
                    Mg_SetSharedState(MG_STATE_ROUND_CLEAR);
                    {
                        dScMgBase_c *active = gMgScene;
                        if (active != 0) {
                            if (active->mHudScore < 9999)
                                (active->mHudScore)++;
                            if (active->mHudScore > active->mBestHudScore)
                                active->mBestHudScore = active->mHudScore;
                        }
                    }
                    Mg_SubmitScore(gMgScene != 0 ? gMgScene->mHudScore : 0);
                    StartWinSpriteA();
                    StartWinSpriteB();
                    mPromptEnabled = 0;
                } else {
                    mState = 3;
                    mResultTimer = 0x20;
                    RevealUnmatched();
                }
            }
        } else {
            UpdateWinSpriteA();
            UpdateWinSpriteB();
        }
        break;
    case 3:
        UpdateNotes();
        UpdatePops();
        if (mResultTimer != 0) {
            (*(u16 *)(int)&mResultTimer)--;
            if (*(s16 *)&mResultTimer <= 0) {
                mTable.mQuiet = 0;
                Presenter_PlayLose((char *)&mTable);
                Mg_SetSharedState(MG_STATE_GAME_OVER);
                mPromptEnabled = 0;
                mResultTimer = 0;
            }
        }
        break;
    }

    Presenter_Update((char *)&mTable);
    return 1;
}

// @symbol _ZN12dScMgSound_c6RenderEv
/* Draws the HUD and each of the scene's layers, then the table. */
s32 dScMgSound_c::Render() {
    char *raw = (char *)this;

    Mg_DrawScoreTally(raw, 0xe0, 0x14, 1);
    DrawPops();
    DrawNotes();
    DrawTries();
    DrawWinSpriteB();
    DrawWinSpriteA();
    Presenter_Draw(&mTable);
    return 1;
}

// @symbol _ZN12dScMgSound_c10StartRoundEi
/* Starts a round. With mode 0 the streak at 0xbc goes up (capped at 9998);
   otherwise it and the scene's total are cleared.
   `raw` must stay a `char *`: laundering `this` through an int puts the large
   offsets in the literal pool and the function grows by 0x10. */
void dScMgSound_c::StartRound(int mode) {
    char *raw = (char *)this;

    dScMgBase_c *active;
    int total;

    mState = 0;

    if (mode == 0) {
        mLevel = mLevel + 1;
        if (mLevel > 0x270e)
            mLevel = 0x270e;
    } else {
        mLevel = 0;
        if (mLevel > 0x270e)
            mLevel = 0x270e;

        active = gMgScene;
        if (active)
            active->mHudScore = 0;

        active = gMgScene;
        if (active)
            total = active->mHudScore;
        else
            total = 0;
        Mg_SubmitScore(total);
    }

    mTable.mQuiet = 1;
    ResetRound();

    mTries = 3;
    DealNotes();

    mIntroTimer = 0x20;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);

    Sound::LoadAndSetMusic_Layer1(6);
}

// @symbol _ZN12dScMgSound_c11OnNewRecordEv
/* Slot 20, OnNewRecord: plays the table model's win animation. */
void dScMgSound_c::OnNewRecord() {
    Presenter_PlayCheer((char *)&mTable);
}

// @symbol _ZN12dScMgSound_c10ResetRoundEv
void dScMgSound_c::ResetRound() {
    int popSlot;
    int noteSlot;
    int pairIndex;
    char *cursor = (char *)this;

    for (noteSlot = 0; noteSlot < 10; noteSlot++) {
        dMgSoundNoteCur *note = (dMgSoundNoteCur *)cursor;
        note->note.x = 0;
        note->note.y = 0;
        note->note.timer = 0;
        note->note.state = 0;
        note->note.alive = 0;
        note->note.shown = 0;
        note->note.frame = 0;
        note->note.note = 0;
        cursor += 0x14;
    }

    for (pairIndex = 0; pairIndex < 5; pairIndex++) {
        mPairUses[pairIndex] = 0;
        mPairNotes[pairIndex] = 0;
    }

    cursor = (char *)this;
    for (popSlot = 0; popSlot < 0x1e; popSlot++) {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)cursor;
        pop->pop.x = 0;
        pop->pop.y = 0;
        pop->pop.velX = 0;
        pop->pop.velY = 0;
        pop->pop.scale = 0;
        pop->pop.timer = 0;
        pop->pop.active = 0;
        pop->pop.visible = 0;
        pop->pop.sprite = 0;
        pop->pop.unk_1f = 0;
        pop->pop.active = 0; /* stored twice, as in the ROM */
        pop->pop.state = 0;
        pop->pop.phase = 0;
        pop->pop.kind = 0;
        pop->pop.dir = 0;
        cursor += 0x24;
    }

    mSpriteB.active = 0;
    mSpriteB.visible = 1;
    mSpriteB.x = 0xbd000;
    mSpriteB.y = 0x97000;
    mSpriteB.frame = 0;
    mSpriteA.active = 0;
    mSpriteA.visible = 1;
    mSpriteA.x = 0xa0000;
    mSpriteA.y = 0x9d000;
    mSpriteA.frame = 0;

    mQueue[1] = 0;
    mQueue[0] = mQueue[1];
    mQueueTimer = 0;
    mQueueLen = 0;
    mTouchCount = 0;
    mResultTimer = 0;

    Presenter_ResetPose((char *)&mTable);
}

// @symbol _ZN12dScMgSound_c9DealNotesEv
/* Choose distinct pitches, make one pair of each, then deal the pairs
 * into the selected screen layout with exactly two uses per pair. */
void dScMgSound_c::DealNotes() {
    int selectedPattern;
    int pattern;
    int noteCount;
    int noteIndex;
    int pairCount;
    int pitchCount;
    int pairIndex;

    selectedPattern = mLevel;
    mPrevPattern = mPattern;
    if (selectedPattern >= 5) {
        selectedPattern = (u32)(RND * 5) >> 15;
        if (selectedPattern == mPrevPattern) {
            selectedPattern += ((u32)(RND << 2) >> 15) + 1;
            if (selectedPattern >= 5)
                selectedPattern -= 5;
        }
    }
    mPattern = selectedPattern;
    pattern = mPattern;
    noteCount = sSoundDealCounts[pattern];
    pitchCount = sSoundPitchCounts[pattern];
    if (pattern == 3) {
        int pitchGroup = (u32)(RND << 2) >> 15;
        int firstPitch = (u32)(RND * 3) >> 15;
        mPairNotes[0] = firstPitch;
        mPairNotes[1] = mPairNotes[0] + 1;
        if (mPairNotes[1] >= 3)
            mPairNotes[1] -= 3;
        mPairNotes[2] = mPairNotes[1] + 1;
        if (mPairNotes[2] >= 3)
            mPairNotes[2] -= 3;
        mPairNotes[0] += pitchGroup * 3;
        mPairNotes[1] += pitchGroup * 3;
        mPairNotes[2] += pitchGroup * 3;
    } else {
        pairCount = noteCount >> 1;
        for (pairIndex = 0; pairIndex < pairCount; pairIndex++) {
            int pitch = (u32)(RND * pitchCount) >> 15;
            mPairNotes[pairIndex] = pitch;
            if (pairIndex != 0) {
                int duplicate;
                int previousPair;
                for (;;) {
                    duplicate = 0;
                    previousPair = 0;
                    for (; previousPair < pairIndex; previousPair++) {
                        if (mPairNotes[pairIndex] == mPairNotes[previousPair]) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (duplicate == 0)
                        break;
                    mPairNotes[pairIndex] = (u32)(RND * pitchCount) >> 15;
                }
            }
        }
    }
    pattern = mPattern;
    if (pattern == 1 || pattern == 4) {
        mBank = sSoundBankByLevel[((u32)mLevel >> 2) & 3];
    }
    pairCount = noteCount >> 1;
    for (pairIndex = 0; pairIndex < pairCount; pairIndex++) {
        mPairNotes[pairIndex] += sSoundPairNoteOffsets[mPattern];
    }
    {
        int xIndex;
        char *noteCursor;
        int yIndex;
        char *q;
        u8 *pairUses;
        int layout;

        noteIndex = 0;
        if (noteCount <= 0)
            return;
        yIndex = 1;
        xIndex = 0;
        noteCursor = (char *)this;
        do {
            dMgSoundNoteCur *note = (dMgSoundNoteCur *)noteCursor;
            layout = sSoundNoteLayoutByPattern[mPattern];
            note->note.x = sSoundNotePositions[layout][xIndex] << 12;
            note->note.y = (*(u16 *volatile *)&sSoundNotePositions[layout])[yIndex] << 12;
            note->note.timer = 0;
            note->note.state = 0;
            note->note.alive = 1;
            note->note.shown = 1;
            note->note.frame = 0;
            note->note.checked = 0;
            do {
                /* q is this advanced by the picked pair, so its mPairUses[0] and
                   mPairNotes[0] are that pair's. */
                q = (char *)this + ((u32)(RND * pairCount) >> 15);
                pairUses = (u8 *)LAUNDER(&((dScMgSound_c *)q)->mPairUses[0]);
            } while (*pairUses >= 2);
            note->note.note = ((dScMgSound_c *)q)->mPairNotes[0];
            (*pairUses)++;
            xIndex += 2;
            noteCursor += 0x14;
            yIndex += 2;
            noteIndex++;
        } while (noteIndex < noteCount);
    }
}

// @symbol _ZN12dScMgSound_c14Note_StateWaitEi
void dScMgSound_c::Note_StateWait(int slot) {
    u8 player;
    int offset;
    int touchPressed;
    int dx;
    int dy;

    if (mState != 1) {
        return;
    }
    if (mTouchCount >= 2) {
        return;
    }

    player = gActivePlayerSlot;
    offset = player * 4;
    touchPressed = 0;
    if (gTouchHeld[offset] != 0) {
        if (gTouchEdge[offset] != 0) {
            touchPressed = 1;
        }
    }
    if (touchPressed == 0) {
        return;
    }

    dx = gTouchX[player * 4] - (mNotes[slot].x >> 12);
    dy = gTouchY[player * 4] - (mNotes[slot].y >> 12);

    if (dx > 0x18) {
        return;
    }
    if (dx < -0x18) {
        return;
    }
    if (dy > 0x18) {
        return;
    }
    if (dy < -0x18) {
        return;
    }

    mNotes[slot].state = 1;
    mNotes[slot].timer = 0;
    mNotes[slot].frame = 0;
    mTouchCount++;
    Sound::PlayBank2_2D(0x201);
}

// @symbol _ZN12dScMgSound_c17Note_StateOpeningEi
/* Advances one note slot's animation. When the slot's last frame is
   reached its note goes on mQueue and is played; mPattern and IsAltVariant
   choose between func_02012790, the bank handle mBank and
   Sound::PlayBank2_2D.
   SpawnPops is passed `idx` even though it is already in r1:
   dropping it changes the register allocation of the queue block. */
void dScMgSound_c::Note_StateOpening(int idx) {
    u8 note;
    u8 pattern;

    mNotes[idx].timer = mNotes[idx].timer + 1;
    if (mNotes[idx].timer < sSoundNoteOpeningTimes[mNotes[idx].frame])
        return;
    mNotes[idx].timer = 0;
    mNotes[idx].frame = mNotes[idx].frame + 1;
    if (mNotes[idx].frame <= 6)
        return;
    mNotes[idx].frame = 6;
    mNotes[idx].state = 2;

    mQueue[mQueueLen] = mNotes[idx].note + 1;
    mQueue[mQueueLen] |= idx << 8;
    mQueueLen++;
    mQueueTimer = 0x20;

    SpawnPops(idx);
    note = mNotes[idx].note;

    if (IsAltVariant()) {
        pattern = mPattern;
        if (pattern == 0 || pattern == 2) {
            if (sSoundNotePlaybackKinds[note] == 2) {
                func_02012790(sSoundNoteSfx[note]);
            } else {
                func_02012174(mBank, sSoundNoteSfx[note]);
            }
        } else if (pattern == 1) {
            int bankIndex = note - 0x1a;
            int soundGroup = 0;
            while (bankIndex >= 4) {
                bankIndex -= 4;
                soundGroup++;
            }
            func_02012174(bankIndex, sSoundNoteSfx[0x1a + soundGroup * 4]);
        } else {
            Sound::PlayBank2_2D(sSoundNoteSfx[note]);
        }
    } else {
        pattern = mPattern;
        if (pattern == 0 || pattern == 2) {
            func_02012790(sSoundNoteSfx[note]);
        } else if (pattern == 1) {
            func_02012174(mBank, sSoundNoteSfx[note]);
        } else if (pattern == 4) {
            if (sSoundNotePlaybackKinds[note] == 2) {
                Sound::PlayBank2_2D(sSoundNoteSfx[note]);
            } else {
                func_02012174(mBank, sSoundNoteSfx[note]);
            }
        } else {
            Sound::PlayBank2_2D(sSoundNoteSfx[note]);
        }
    }

    if (mTouchCount == 1)
        Presenter_PlayCheerLoop((char *)&mTable);
}

// @symbol _ZN12dScMgSound_c14Note_StateOpenEi
void dScMgSound_c::Note_StateOpen(int slot) {
    if (mTouchCount == 0) {
        mNotes[slot].state = 3;
    }
}

// @symbol _ZN12dScMgSound_c17Note_StateClosingEi
void dScMgSound_c::Note_StateClosing(int slot) {
    unsigned char frame;
    mNotes[slot].timer = (unsigned short)(mNotes[slot].timer + 1);
    frame = mNotes[slot].frame;
    if (mNotes[slot].timer < sSoundNoteClosingTimes[frame])
        return;
    mNotes[slot].timer = 0;
    mNotes[slot].frame = (unsigned char)(mNotes[slot].frame - 1);
    if (mNotes[slot].frame == 0)
        mNotes[slot].state = 0;
}

// @symbol _ZN12dScMgSound_c16Note_StateScrollEi
/* Scrolls one note slot left by 0x10 per call and clears it once it has
   gone off screen. Its column comes from the per-mode tables
   sSoundScrollRowLengthAlt and sSoundScrollRowLength. */
void dScMgSound_c::Note_StateScroll(int slot) {
    int column;
    int previousOffset;
    int rowSize;

    mNotes[slot].x -= 0x10000;

    if (IsAltVariant() != 0) {
        rowSize = sSoundScrollRowLengthAlt[mPattern];
    } else {
        rowSize = sSoundScrollRowLength[mPattern];
    }

    if (mNotes[slot].checked == 0) {
        column = slot;
        if (slot >= rowSize) {
            do {
                column = column - rowSize;
            } while (column >= rowSize);
        }
        if (column == 0) {
            mNotes[slot].checked = 1;
            return;
        }
        for (previousOffset = 0; previousOffset < column; previousOffset++) {
            int previousSlot = slot - previousOffset - 1;
            if (mNotes[previousSlot].alive != 0) {
                if ((mNotes[slot].x - mNotes[previousSlot].x) >> 0xc <= 4) {
                    mNotes[slot].checked = 1;
                    mNotes[previousSlot].state = 4;
                }
            }
        }
    }

    if (mNotes[slot].x >> 0xc > -0x18)
        return;

    mNotes[slot].alive = 0;
    mNotes[slot].shown = 0;
}

// @symbol _ZN12dScMgSound_c15RevealUnmatchedEv
/* Start scrolling from the last surviving note in each row. Earlier
 * notes follow through Note_StateScroll once their neighbour reaches them. */
void dScMgSound_c::RevealUnmatched() {
    int rowSize;
    int row;
    int lastAliveSlot;
    int rowOffset;
    if (IsAltVariant()) {
        rowSize = sSoundRowLengthAlt[mPattern];
    } else {
        rowSize = sSoundRowLength[mPattern];
    }
    for (row = 0, rowOffset = 0; row < 2; row++) {
        int column;
        lastAliveSlot = -1;
        for (column = 0; column < rowSize; column++) {
            int slot = column + rowOffset;
            if (mNotes[slot].alive != 0)
                lastAliveSlot = slot;
        }
        if (lastAliveSlot != -1) {
            mNotes[lastAliveSlot].state = 4;
        }
        rowOffset += rowSize;
    }
}

// @symbol _ZN12dScMgSound_c11UpdateNotesEv
void dScMgSound_c::UpdateNotes() {
    int slot = 0;
    char *noteCursor = (char *)this;
    do {
        dMgSoundNoteCur *note = (dMgSoundNoteCur *)noteCursor;
        if (note->note.alive != 0) {
            int state = note->note.state;
            (this->*sSoundNoteStateTable[state].pmf)(slot);
        }
        slot++;
        noteCursor += 0x14;
    } while (slot < 10);
}

// @symbol _ZN12dScMgSound_c9CheckPairEv
/* Queue entries pack the note slot in the high byte and pitch + 1 in
 * the low byte. Compare pitches after the reveal delay. */
void dScMgSound_c::CheckPair() {
    if (mQueueLen < 2)
        return;
    if (mQueueTimer != 0) {
        mQueueTimer -= 1;
        if ((short)mQueueTimer <= 0)
            mQueueTimer = 0;
        return;
    }
    if ((mQueue[0] & 0xff) == (mQueue[1] & 0xff)) {
        mNotes[mQueue[0] >> 8].alive = 0;
        mNotes[mQueue[1] >> 8].alive = 0;
        func_02012790(0x26);
        Presenter_PlayMatch(&mTable);
    } else {
        mNotes[mQueue[0] >> 8].state = 3;
        mNotes[mQueue[1] >> 8].state = 3;
        func_02012790(0xe);
        mTries--;
        func_02012790(0x12f);
        Presenter_PlayMiss((char *)&mTable);
    }
    mQueue[1] = 0;
    mQueue[0] = mQueue[1];
    mQueueLen = 0;
    mTouchCount = 0;
}

// @symbol _ZN12dScMgSound_c14CheckRoundOverEv
void dScMgSound_c::CheckRoundOver() {
    if (mTries == 0) {
        mState = 2;
        mResultTimer = 0x50;
        return;
    }
    {
        int hasAliveNote = 0;
        int slot = 0;
        char *noteCursor = (char *)this;
        do {
            if (((dMgSoundNoteCur *)noteCursor)->note.alive != 0) {
                hasAliveNote++;
                break;
            }
            slot++;
            noteCursor += 0x14;
        } while (slot < 0xa);
        if (hasAliveNote != 0)
            return;
        mResultTimer = 0x50;
        mState = 2;
    }
}

// @symbol _ZN12dScMgSound_c9SpawnPopsEi
void dScMgSound_c::SpawnPops(int noteSlot) {
    int spawnIndex;
    int popSlot;
    const u8 *spriteEntry = sSoundPopSpawnKinds;

    for (spawnIndex = 0; spawnIndex < 3; spriteEntry++, spawnIndex++) {
        for (popSlot = 0; popSlot < 30; popSlot++) {
            if (mPops[popSlot].active == 0) {
                mPops[popSlot].active = 1;
                mPops[popSlot].x = mNotes[noteSlot].x;
                mPops[popSlot].y = mNotes[noteSlot].y - 0x8000;
                mPops[popSlot].startY = mPops[popSlot].y;
                mPops[popSlot].sprite = *spriteEntry;
                mPops[popSlot].kind = *spriteEntry;
                mPops[popSlot].timer = 0;
                if (mNotes[noteSlot].note == 7) {
                    if (spawnIndex >= 3) {
                        mPops[popSlot].timer = 0x10;
                    } else {
                        mPops[popSlot].timer = 8;
                    }
                }
                mPops[popSlot].scale = 0x1000;
                mPops[popSlot].velX = 0;
                mPops[popSlot].velY = 0;
                mPops[popSlot].state = 0;
                mPops[popSlot].phase = 0;
                break;
            }
        }
    }
}

// @symbol _ZN12dScMgSound_c10UpdatePopsEv
void dScMgSound_c::UpdatePops() {
    int slot = 0;
    char *popCursor = (char *)this;
    do {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)popCursor;
        if (pop->pop.active != 0) {
            int state = pop->pop.state;
            (this->*sSoundPopUpdateTable[state].pmf)(slot);
        }
        slot++;
        popCursor += 0x24;
    } while (slot < 30);
}

// @symbol _ZN12dScMgSound_c12Pop00_UpdateEi
void dScMgSound_c::Pop00_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop00Table[phase])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop00_StartEi
void dScMgSound_c::Pop00_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop00XOffsets[mPops[slot].kind];
    mPops[slot].velY = 0;
    mPops[slot].velX = sSoundPop00VX[mPops[slot].kind];
    mPops[slot].dir = sSoundPop00Dirs[mPops[slot].kind];
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop00_FlyEi
void dScMgSound_c::Pop00_Fly(int slot) {
    int dir;

    mPops[slot].x += mPops[slot].velX;

    dir = mPops[slot].dir;
    if (dir == 0) {
        mPops[slot].velX -= 0x100;
        if (mPops[slot].velX < -0xc00) {
            mPops[slot].velX = -0xc00;
            mPops[slot].dir = 1;
        }
    } else if (dir != 0) {
        mPops[slot].velX += 0x100;
        if (mPops[slot].velX > 0xc00) {
            mPops[slot].velX = 0xc00;
            mPops[slot].dir = 0;
        }
    }

    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY -= 0xc0;

    if ((mPops[slot].startY - mPops[slot].y) >> 12 >= 0x40) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop00_FinishEv
void dScMgSound_c::Pop00_Finish() {}

// @symbol _ZN12dScMgSound_c12Pop01_UpdateEi
void dScMgSound_c::Pop01_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop01Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop01_StartEi
void dScMgSound_c::Pop01_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop01XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2800;
    mPops[slot].velX = sSoundPop01VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop01_FlyEi
void dScMgSound_c::Pop01_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop01_FinishEi
void dScMgSound_c::Pop01_Finish(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (((mPops[slot].startY - mPops[slot].y) >> 0xc) > 0x18)
        return;
    if (mPops[slot].velY > 0) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop02_UpdateEi
void dScMgSound_c::Pop02_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop02Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop02_StartEi
void dScMgSound_c::Pop02_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop02XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2000;
    mPops[slot].velX = sSoundPop02VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop02_FlyEi
void dScMgSound_c::Pop02_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop02_FinishEi
void dScMgSound_c::Pop02_Finish(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (((mPops[slot].startY - mPops[slot].y) >> 0xc) > 0x10)
        return;
    if (mPops[slot].velY > 0) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop03_UpdateEi
void dScMgSound_c::Pop03_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop03Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop03_StartEi
void dScMgSound_c::Pop03_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop03XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x4800;
    mPops[slot].velX = sSoundPop03VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop03_FlyEi
void dScMgSound_c::Pop03_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop03_FinishEi
void dScMgSound_c::Pop03_Finish(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (((mPops[slot].startY - mPops[slot].y) >> 0xc) > 0x20)
        return;
    if (mPops[slot].velY > 0) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop04_UpdateEi
void dScMgSound_c::Pop04_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop04Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop04_StartEi
void dScMgSound_c::Pop04_Start(int slot) {
    mPops[slot].scale = 0x3000;
    mPops[slot].visible = 1;
    mPops[slot].velY = -0x1000;
    mPops[slot].velX = sSoundPop04VX[mPops[slot].kind];
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop04_FlyEi
void dScMgSound_c::Pop04_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    if (mPops[slot].scale > 0x800) {
        mPops[slot].scale -= 0x100;
        if (mPops[slot].scale < 0x800)
            mPops[slot].scale = 0x800;
    }
    mPops[slot].velY -= 0x20;
    {
        int risePixels = (mPops[slot].startY - mPops[slot].y) >> 12;
        if (risePixels >= 0x38) {
            mPops[slot].active = 0;
            mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c12Pop04_FinishEv
void dScMgSound_c::Pop04_Finish() {}

// @symbol _ZN12dScMgSound_c12Pop05_UpdateEi
void dScMgSound_c::Pop05_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop05Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop05_StartEi
void dScMgSound_c::Pop05_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop05XOffsets[kind];
    mPops[slot].velY = -0x4000;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop05_FlyEi
void dScMgSound_c::Pop05_Fly(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop05_FinishEi
void dScMgSound_c::Pop05_Finish(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (((mPops[slot].startY - mPops[slot].y) >> 12) <= 0x18) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop06_UpdateEi
void dScMgSound_c::Pop06_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop06Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop06_StartEi
/* Arms one pop: visible, no vertical speed, horizontal speed from its kind. */
void dScMgSound_c::Pop06_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].velY = 0;
    mPops[slot].velX = sSoundPop06VX[mPops[slot].kind];
    mPops[slot].phase = 1;
    mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c9Pop06_FlyEi
void dScMgSound_c::Pop06_Fly(int slot) {
    int velocityX;
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    velocityX = mPops[slot].velX;
    if (velocityX > 0x20) {
        mPops[slot].velX = velocityX - 0x20;
    } else if (velocityX < -0x20) {
        mPops[slot].velX = velocityX + 0x20;
    }
    mPops[slot].velY -= 0x40;
    mPops[slot].timer += 1;
    if (mPops[slot].timer >= 0x28) {
        mPops[slot].timer = 0;
        mPops[slot].phase = 2;
        mPops[slot].dir = 0;
        mPops[slot].velX = 0xc00;
    }
}

// @symbol _ZN12dScMgSound_c12Pop06_FinishEi
void dScMgSound_c::Pop06_Finish(int slot) {
    int direction;
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY -= 0x40;
    direction = mPops[slot].dir;
    if (direction == 0) {
        mPops[slot].velX -= 0x100;
        if (mPops[slot].velX < -0xc00) {
            mPops[slot].velX = -0xc00;
            mPops[slot].dir = 1;
        }
    } else if (direction != 0) {
        mPops[slot].velX += 0x100;
        if (mPops[slot].velX > 0xc00) {
            mPops[slot].velX = 0xc00;
            mPops[slot].dir = 0;
        }
    }
    if ((mPops[slot].startY - mPops[slot].y) >> 12 >= 0x30) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop07_UpdateEi
void dScMgSound_c::Pop07_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop07Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop07_StartEi
void dScMgSound_c::Pop07_Start(int slot) {
    unsigned short delay = mPops[slot].timer;
    if (delay != 0) {
        mPops[slot].timer = delay - 1;
        if (*(short *)&mPops[slot].timer < 0)
            mPops[slot].timer = 0;
        return;
    }
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop07XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x4000;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop07_FlyEi
void dScMgSound_c::Pop07_Fly(int slot) {
    int velocityY = mPops[slot].velY;
    int positionY = mPops[slot].y;
    mPops[slot].y = positionY + velocityY;
    velocityY = mPops[slot].velY;
    mPops[slot].velY = velocityY + 0x400;
    {
        int rise = mPops[slot].startY - mPops[slot].y;
        rise >>= 12;
        if (rise >= 0x20) {
            mPops[slot].active = 0;
            mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c12Pop07_FinishEv
void dScMgSound_c::Pop07_Finish() {}

// @symbol _ZN12dScMgSound_c12Pop08_UpdateEi
void dScMgSound_c::Pop08_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop08Table[phase].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop08_StartEi
void dScMgSound_c::Pop08_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop08XOffsets[kind];
    mPops[slot].velY = -0x4000;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop08_FlyEi
void dScMgSound_c::Pop08_Fly(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop08_FinishEi
void dScMgSound_c::Pop08_Finish(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY -= 0x60;
    if ((mPops[slot].startY - mPops[slot].y) >> 0xc >= 0x3c) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop09_UpdateEi
void dScMgSound_c::Pop09_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop09Table[phase])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop09_StartEi
void dScMgSound_c::Pop09_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop09XOffsets[kind];
    mPops[slot].velY = -0x3800;
    mPops[slot].velX = sSoundPop09VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop09_FlyEi
void dScMgSound_c::Pop09_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY <= 0)
        return;
    mPops[slot].velY = 0;
    mPops[slot].phase = 2;
    mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop09_FinishEi
void dScMgSound_c::Pop09_Finish(int slot) {
    int scale = mPops[slot].scale;
    if (scale > 0x800) {
        mPops[slot].scale = scale - 0x60;
        if (mPops[slot].scale < 0x800)
            mPops[slot].scale = 0x800;
    }
    mPops[slot].timer += 1;
    if (mPops[slot].timer >= 0x20) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop10_UpdateEi
void dScMgSound_c::Pop10_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop10Table[phase])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop10_StartEi
void dScMgSound_c::Pop10_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop10XOffsets[kind];
    mPops[slot].velY = -0x3200;
    mPops[slot].velX = sSoundPop10VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop10_FlyEi
void dScMgSound_c::Pop10_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY <= 0)
        return;
    mPops[slot].velY = 0;
    mPops[slot].phase = 2;
    mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop10_FinishEi
void dScMgSound_c::Pop10_Finish(int slot) {
    int scale = mPops[slot].scale;
    if (scale > 0x800) {
        mPops[slot].scale = scale - 0x70;
        if (mPops[slot].scale < 0x800)
            mPops[slot].scale = 0x800;
    }
    mPops[slot].timer += 1;
    if (mPops[slot].timer >= 0x20) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop11_UpdateEi
void dScMgSound_c::Pop11_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*(sSoundPop11Table[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c11Pop11_StartEi
void dScMgSound_c::Pop11_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop11XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2800;
    mPops[slot].velX = sSoundPop11VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop11_FlyEi
void dScMgSound_c::Pop11_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY <= 0)
        return;
    mPops[slot].velY = 0;
    mPops[slot].phase = 2;
    mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop11_FinishEi
void dScMgSound_c::Pop11_Finish(int slot) {
    int scale = mPops[slot].scale;
    if (scale > 0xa80) {
        mPops[slot].scale = scale - 0x60;
        if (mPops[slot].scale < 0xa80)
            mPops[slot].scale = 0xa80;
    }
    mPops[slot].timer += 1;
    if (mPops[slot].timer >= 0x18) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop12_UpdateEi
void dScMgSound_c::Pop12_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*(sSoundPop12Table[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c11Pop12_StartEi
void dScMgSound_c::Pop12_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop12XOffsets[kind];
    mPops[slot].velY = -0x2400;
    mPops[slot].velX = sSoundPop12VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop12_FlyEi
void dScMgSound_c::Pop12_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY <= 0)
        return;
    mPops[slot].velY = 0;
    mPops[slot].phase = 2;
    mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop12_FinishEi
void dScMgSound_c::Pop12_Finish(int slot) {
    int scale = mPops[slot].scale;
    if (scale > 0xd80) {
        mPops[slot].scale = scale - 0x60;
        if (mPops[slot].scale < 0xd80)
            mPops[slot].scale = 0xd80;
    }
    mPops[slot].timer += 1;
    if (mPops[slot].timer >= 0x18) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c8DrawPopsEv
extern "C" void Mg_RenderSpriteBothPalMtx(void *a0, int a1, int a2, int a3, void *a4);

void dScMgSound_c::DrawPops() {
    char *popCursor = (char *)this;
    int slot;
    int sinAngle = data_02082214[0];
    long long cosAngle = data_02082214[1];

    for (slot = 0; slot < 30; slot++) {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)popCursor;
        if (pop->pop.visible) {
            int matrix[4];
            long long scale = pop->pop.scale;
            int x = pop->pop.x;
            int y = pop->pop.y;
            int scaledCos = (int)((cosAngle * scale + 0x800) >> 12);
            int scaledSin = FX_MUL(sinAngle, (int)scale);
            matrix[0] = scaledCos;
            matrix[3] = scaledCos;
            matrix[1] = scaledSin;
            matrix[2] = -scaledSin;
            Mg_RenderSpriteBothPalMtx(sSoundPopsSprites[pop->pop.sprite], (x >> 12) - 8,
                                      (y >> 12) - 0x10, -1, matrix);
        }
        sinAngle = data_02082214[0];
        popCursor += 0x24;
    }
}

// @symbol _ZN12dScMgSound_c9DrawNotesEv
extern u8 data_ov006_02133c58[];
extern u8 data_ov006_02133c40[];
extern u8 data_ov006_02133c10[];
extern u8 data_ov006_02133c28[];
extern u8 data_ov006_02133bf0[];
extern u8 data_ov006_02133be0[];
extern u8 data_ov006_02133c00[];
void *sSoundNotesSprites[7] = {data_ov006_02133c58, data_ov006_02133c40, data_ov006_02133c10,
                               data_ov006_02133c28, data_ov006_02133bf0, data_ov006_02133be0,
                               data_ov006_02133c00};
void dScMgSound_c::DrawNotes() {
    char *sceneBytes = (char *)this;
    int slot;
    char *noteCursor = sceneBytes;
    for (slot = 0; slot < 10; slot++) {
        dMgSoundNoteCur *note = (dMgSoundNoteCur *)noteCursor;
        if (note->note.shown != 0 && note->note.alive != 0) {
            int screenX = note->note.x >> 12;
            int screenY = note->note.y >> 12;
            int priority = 0;
            if (mState != 1)
                priority = 1;
            unsigned char frame = note->note.frame;
            Hud_RenderSprite((int)sSoundNotesSprites[frame], screenX, screenY, -1, priority);
        }
        noteCursor += 0x14;
    }
}

// @symbol _ZN12dScMgSound_c9DrawTriesEv
void dScMgSound_c::DrawTries() {
    Mg_DrawStockCounter(mTries, 1);
}

// @symbol _ZN12dScMgSound_c15StartWinSpriteAEv
void dScMgSound_c::StartWinSpriteA() {
    mSpriteA.active = 1;
    mSpriteA.timer = 0;
    mSpriteA.frame = 0;
}

// @symbol _ZN12dScMgSound_c16UpdateWinSpriteAEv
void dScMgSound_c::UpdateWinSpriteA() {
    if (mSpriteA.active == 0)
        return;
    if (mSpriteA.frame >= 2)
        return;
    (*(u16 *)(int)&mSpriteA.timer)++;
    if (mSpriteA.timer < sSoundWinSpriteATimes[mSpriteA.frame])
        return;
    mSpriteA.timer = 0;
    (*(u8 *)(int)&mSpriteA.frame)++;
}

// @symbol _ZN12dScMgSound_c14DrawWinSpriteAEv
void dScMgSound_c::DrawWinSpriteA() {
    if (mSpriteA.visible == 0)
        return;
    Hud_RenderSprite(sSoundWinSpriteASprites[mSpriteA.frame], mSpriteA.x >> 0xc, mSpriteA.y >> 0xc,
                     -1, -1);
}

// @symbol _ZN12dScMgSound_c15StartWinSpriteBEv
void dScMgSound_c::StartWinSpriteB() {
    mSpriteB.active = 1;
    mSpriteB.timer = 0;
    mSpriteB.frame = 0;
}

// @symbol _ZN12dScMgSound_c16UpdateWinSpriteBEv
void dScMgSound_c::UpdateWinSpriteB() {
    if (mSpriteB.active == 0)
        return;
    if (mSpriteB.frame >= 3)
        return;
    (*(u16 *)(int)&mSpriteB.timer) += 1;
    if (mSpriteB.timer < sSoundWinSpriteBTimes[mSpriteB.frame])
        return;
    mSpriteB.timer = 0;
    (*(u8 *)(int)&mSpriteB.frame) += 1;
}

// @symbol _ZN12dScMgSound_c14DrawWinSpriteBEv
void dScMgSound_c::DrawWinSpriteB() {
    if (mSpriteB.visible == 0)
        return;
    Hud_RenderSprite(sSoundWinSpriteBSprites[mSpriteB.frame], mSpriteB.x >> 0xc, mSpriteB.y >> 0xc,
                     -1, -1);
}
// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, so D1 is emitted before D0. */
