//cpp
/* Boom-box minigame. Each round deals mNotes on the touch screen; a touch
 * plays that pitch and pops icons in mPops. The class is a
 * dScMgSingle3DBase_c (its RTTI is at 0x0213f6e4).
 *
 * The compiler emits .text in reverse source order, so the functions run
 * here from the highest address down. Do not reorder.
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
#include "nitro/hw/registers.h"
#include "types.h"
#include "decl_common.h"
#include "LevelRng.h"
#include "PlayerInput.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_load.h"
#include "math/CalcLib.h"
#include "nitro/fx/fx_trig.h"

/* The state/phase dispatch tables hold pointer-to-member records: the
   sinit copies {pmf, delta} descriptors into them. */
typedef void (dScMgSound_c::*Pmf)(int);
struct PmfEntry {
    Pmf pmf;
};

#define RND (((u32)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff)

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

extern void Hud_RenderSprite(void *a, int b, int c, int d, int e);}

extern void *sSoundWinSpriteBSprites[];

/* WinSpriteB reads indices 0 through 2; the fourth stored value is unused here. */
extern const u16 sSoundWinSpriteBTimes[4] = {50, 3, 3, 50};

extern void *sSoundWinSpriteASprites[];

/* Stored timing suffixes are retained; WinSpriteA reads only indices 0 and 1. */
extern const u16 sSoundWinSpriteATimes[4] = {50, 4, 50, 0};
extern "C" {

extern void Mg_DrawStockCounter(int a, int b);}

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
void Presenter_PlayMatch(void *c);
extern "C" {

extern void Presenter_PlayMiss(void *c);}

extern int sSoundRowLengthAlt[];

extern int sSoundRowLength[];

extern int sSoundScrollRowLengthAlt[];

extern int sSoundScrollRowLength[];

/* Note frames use indices 0 through 6. The final stored timing value in each
 * array is retained without asserting its original purpose or declared bound. */
extern const u16 sSoundNoteClosingTimes[8] = {2, 2, 3, 4, 4, 4, 4, 4};
void Presenter_PlayCheerLoop(void *c);


unsigned int func_02012174(u32 bank, u32 id);

extern const u16 sSoundNoteOpeningTimes[8] = {2, 2, 3, 4, 4, 4, 6, 4};

/* Parallel playback selectors and sound IDs retain all 42 stored entries.
 * Some entries are not exercised by the current deal logic. */
u16 sSoundNotePlaybackKinds[42] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
                                   2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

u16 sSoundNoteSfx[42] = {70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  56,  57,  58,  59,
                         169, 170, 171, 172, 173, 174, 176, 177, 178, 266, 267, 268, 56,  120,
                         184, 248, 57,  121, 185, 249, 58,  122, 186, 250, 59,  123, 187, 251};

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
extern "C" {

extern void Presenter_ResetPose(void *c);}
void Presenter_PlayCheer(void *c);
extern "C" {

extern void Mg_DrawScoreTally(void *c, int a, int b, int d);

void FreeGfxSlotsById(int id);

void *LoadFile(int handle);

void DecompressLZ16(void *src, void *dst);

int Presenter_Load(void *p);}

extern u8 gMainScreenLayers;

extern u8 gSubScreenLayers;


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
   C2, dScMgSingle3DBase_c's vptr and mParticle, this class's vptr, then
   mTable's constructor) is the compiler's inlining of the base and member
   constructors. */
inline dScMgSound_c::dScMgSound_c() {}

/* The MG_SOUND registry factory. It sits directly after InitResources and
   installs this class's vtable, so it was compiled with the class. */
 void *dScMgSound_c_classInit(void) {
    return new dScMgSound_c;
}

/* Loads the minigame's graphics for both screens, sets up the camera and
   the table, then deals the first round. */
s32 dScMgSound_c::InitResources() {
    void *objChars, *objPltt, *file;

    gMainScreenLayers |= DISPLAY_LAYER_BG0 | DISPLAY_LAYER_OBJ;
    objChars = LoadFile(ASSET_D_2D_MG_SOUND_NCG_BIN);
    objPltt = LoadFile(ASSET_D_2D_MG_SOUND_NCL_BIN);
    DecompressLZ16(objChars, G2_GetOBJCharPtr());
    GX_LoadOBJPltt(objPltt, 0, 0x100);

    gSubScreenLayers |= 4;
    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NCG_BIN);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x1000,
                          GX_BG_CHARBASE_0x14000);
    DecompressLZ16(file, (void *)G2S_GetBG2CharPtr());
    Deallocate(file);

    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NCL_BIN);
    GXS_LoadBGPltt(file, 0x60, 0x1a0);
    Deallocate(file);

    file = LoadFile(ASSET_D_2D_MG_BG_SOUND2_NSC_BIN);
    GXS_LoadBG2Scr(file, 0, 0x800);
    Deallocate(file);

    G2S_BG2Mosaic(false);
    G2S_SetBG2Offset(0, 0);
    G2S_SetBG2Priority(1);
    DecompressLZ16(objChars, G2S_GetOBJCharPtr());
    GXS_LoadOBJPltt(objPltt, 0, 0x100);
    Deallocate(objChars);
    Deallocate(objPltt);

    Camera_InitPresenterView(&mCamera);
    if (Presenter_Load(&mTable) == 0)
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

/* One frame of the round, by mState: 0 starts; 1 plays the round while
   mIntroTimer controls the prompt; 2 waits for the result, then celebrates
   a win or advances to 3 for the loss delay. Wins increment the HUD score
   up to 9999. */
s32 dScMgSound_c::Behavior() {

    switch (mState) {
    case 0:
        mState = 1;
        break;
    case 1:
        if (mIntroTimer != 0) {
            mIntroTimer--;
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
            mResultTimer--;
            if (mResultTimer == 0) {
                if (mTries != 0) {
                    mTable.mQuiet = 0;
                    Presenter_PlayCheer(&mTable);
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
            mResultTimer--;
            if ((s16)mResultTimer <= 0) {
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

/* Draws the HUD and each of the scene's layers, then the table. */
s32 dScMgSound_c::Render() {
    Mg_DrawScoreTally(this, 0xe0, 0x14, 1);
    DrawPops();
    DrawNotes();
    DrawTries();
    DrawWinSpriteB();
    DrawWinSpriteA();
    Presenter_Draw(&mTable);
    return 1;
}

/* Starts a round. With mode 0 the streak at 0xbc goes up (capped at 9998);
   otherwise it and the scene's total are cleared. */
void dScMgSound_c::StartRound(int mode) {
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

/* Slot 20, OnNewRecord: plays the table model's win animation. */
void dScMgSound_c::OnNewRecord() {
    Presenter_PlayCheer(&mTable);
}

void dScMgSound_c::ResetRound() {
    int popSlot;
    int noteSlot;
    int pairIndex;

    for (noteSlot = 0; noteSlot < 10; noteSlot++) {
        mNotes[noteSlot].x = 0;
        mNotes[noteSlot].y = 0;
        mNotes[noteSlot].timer = 0;
        mNotes[noteSlot].state = 0;
        mNotes[noteSlot].alive = 0;
        mNotes[noteSlot].shown = 0;
        mNotes[noteSlot].frame = 0;
        mNotes[noteSlot].note = 0;
    }

    for (pairIndex = 0; pairIndex < 5; pairIndex++) {
        mPairUses[pairIndex] = 0;
        mPairNotes[pairIndex] = 0;
    }

    for (popSlot = 0; popSlot < 0x1e; popSlot++) {
        mPops[popSlot].x = 0;
        mPops[popSlot].y = 0;
        mPops[popSlot].velX = 0;
        mPops[popSlot].velY = 0;
        mPops[popSlot].scale = 0;
        mPops[popSlot].timer = 0;
        mPops[popSlot].active = 0;
        mPops[popSlot].visible = 0;
        mPops[popSlot].sprite = 0;
        mPops[popSlot].unk_1f = 0;
        mPops[popSlot].active = 0; /* stored twice, as in the ROM */
        mPops[popSlot].state = 0;
        mPops[popSlot].phase = 0;
        mPops[popSlot].kind = 0;
        mPops[popSlot].dir = 0;
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

    Presenter_ResetPose(&mTable);
}

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
    int pair;
    int layout;

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
    for (noteIndex = 0; noteIndex < noteCount; noteIndex++) {
        layout = sSoundNoteLayoutByPattern[mPattern];
        mNotes[noteIndex].x = sSoundNotePositions[layout][noteIndex * 2] << 12;
        mNotes[noteIndex].y = sSoundNotePositions[layout][noteIndex * 2 + 1] << 12;
        mNotes[noteIndex].timer = 0;
        mNotes[noteIndex].state = 0;
        mNotes[noteIndex].alive = 1;
        mNotes[noteIndex].shown = 1;
        mNotes[noteIndex].frame = 0;
        mNotes[noteIndex].checked = 0;
        do {
            pair = (u32)(RND * pairCount) >> 15;
        } while (mPairUses[pair] >= 2);
        mNotes[noteIndex].note = mPairNotes[pair];
        mPairUses[pair]++;
    }
}

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
        Presenter_PlayCheerLoop(&mTable);
}

void dScMgSound_c::Note_StateOpen(int slot) {
    if (mTouchCount == 0) {
        mNotes[slot].state = 3;
    }
}

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

void dScMgSound_c::UpdateNotes() {
    int slot;
    for (slot = 0; slot < 10; slot++) {
        if (mNotes[slot].alive != 0) {
            (this->*sSoundNoteStateTable[mNotes[slot].state].pmf)(slot);
        }
    }
}

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
        Presenter_PlayMiss(&mTable);
    }
    mQueue[1] = 0;
    mQueue[0] = mQueue[1];
    mQueueLen = 0;
    mTouchCount = 0;
}

void dScMgSound_c::CheckRoundOver() {
    if (mTries == 0) {
        mState = 2;
        mResultTimer = 0x50;
        return;
    }
    {
        int hasAliveNote = 0;
        int slot;
        for (slot = 0; slot < 10; slot++) {
            if (mNotes[slot].alive != 0) {
                hasAliveNote++;
                break;
            }
        }
        if (hasAliveNote != 0)
            return;
        mResultTimer = 0x50;
        mState = 2;
    }
}

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

void dScMgSound_c::UpdatePops() {
    int slot;
    for (slot = 0; slot < 30; slot++) {
        if (mPops[slot].active != 0) {
            (this->*sSoundPopUpdateTable[mPops[slot].state].pmf)(slot);
        }
    }
}

void dScMgSound_c::Pop00_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop00Table[phase])(slot);
}

void dScMgSound_c::Pop00_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop00XOffsets[mPops[slot].kind];
    mPops[slot].velY = 0;
    mPops[slot].velX = sSoundPop00VX[mPops[slot].kind];
    mPops[slot].dir = sSoundPop00Dirs[mPops[slot].kind];
    mPops[slot].phase = 1;
}

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

void dScMgSound_c::Pop00_Finish() {}

void dScMgSound_c::Pop01_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop01Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop01_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop01XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2800;
    mPops[slot].velX = sSoundPop01VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

void dScMgSound_c::Pop01_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x200;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

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

void dScMgSound_c::Pop02_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop02Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop02_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop02XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2000;
    mPops[slot].velX = sSoundPop02VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

void dScMgSound_c::Pop02_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

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

void dScMgSound_c::Pop03_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop03Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop03_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop03XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x4800;
    mPops[slot].velX = sSoundPop03VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

void dScMgSound_c::Pop03_Fly(int slot) {
    mPops[slot].x += mPops[slot].velX;
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

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

void dScMgSound_c::Pop04_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop04Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop04_Start(int slot) {
    mPops[slot].scale = 0x3000;
    mPops[slot].visible = 1;
    mPops[slot].velY = -0x1000;
    mPops[slot].velX = sSoundPop04VX[mPops[slot].kind];
    mPops[slot].phase = 1;
}

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

void dScMgSound_c::Pop04_Finish() {}

void dScMgSound_c::Pop05_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop05Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop05_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop05XOffsets[kind];
    mPops[slot].velY = -0x4000;
    mPops[slot].phase = 1;
}

void dScMgSound_c::Pop05_Fly(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

void dScMgSound_c::Pop05_Finish(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x100;
    if (((mPops[slot].startY - mPops[slot].y) >> 12) <= 0x18) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

void dScMgSound_c::Pop06_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop06Table[phase].pmf)(slot);
}

/* Arms one pop: visible, no vertical speed, horizontal speed from its kind. */
void dScMgSound_c::Pop06_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].velY = 0;
    mPops[slot].velX = sSoundPop06VX[mPops[slot].kind];
    mPops[slot].phase = 1;
    mPops[slot].timer = 0;
}

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

void dScMgSound_c::Pop07_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop07Table[phase].pmf)(slot);
}

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

void dScMgSound_c::Pop07_Finish() {}

void dScMgSound_c::Pop08_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop08Table[phase].pmf)(slot);
}

void dScMgSound_c::Pop08_Start(int slot) {
    unsigned char kind;
    mPops[slot].visible = 1;
    kind = mPops[slot].kind;
    mPops[slot].x += sSoundPop08XOffsets[kind];
    mPops[slot].velY = -0x4000;
    mPops[slot].phase = 1;
}

void dScMgSound_c::Pop08_Fly(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY += 0x400;
    if (mPops[slot].velY > 0) {
        mPops[slot].velY = 0;
        mPops[slot].phase = 2;
    }
}

void dScMgSound_c::Pop08_Finish(int slot) {
    mPops[slot].y += mPops[slot].velY;
    mPops[slot].velY -= 0x60;
    if ((mPops[slot].startY - mPops[slot].y) >> 0xc >= 0x3c) {
        mPops[slot].active = 0;
        mPops[slot].visible = 0;
    }
}

void dScMgSound_c::Pop09_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop09Table[phase])(slot);
}

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

void dScMgSound_c::Pop10_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*sSoundPop10Table[phase])(slot);
}

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

void dScMgSound_c::Pop11_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*(sSoundPop11Table[phase].pmf))(slot);
}

void dScMgSound_c::Pop11_Start(int slot) {
    mPops[slot].visible = 1;
    mPops[slot].x += sSoundPop11XOffsets[mPops[slot].kind];
    mPops[slot].velY = -0x2800;
    mPops[slot].velX = sSoundPop11VX[mPops[slot].kind];
    mPops[slot].timer = 0;
    mPops[slot].phase = 1;
}

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

void dScMgSound_c::Pop12_Update(int slot) {
    unsigned char phase = mPops[slot].phase;
    (this->*(sSoundPop12Table[phase].pmf))(slot);
}

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
}extern "C" void Mg_RenderSpriteBothPalMtx(void *a0, int a1, int a2, int a3, void *a4);


void dScMgSound_c::DrawPops() {
    int slot;
    /* sin 0 and cos 0, read from the table by hand: FX_SinIdx(0) and
       FX_CosIdx(0) compile to a different body. */
    int sinAngle = FX_SinCosTable_[0];
    long long cosAngle = FX_SinCosTable_[1];

    for (slot = 0; slot < 30; slot++) {
        if (mPops[slot].visible) {
            int matrix[4];
            long long scale = mPops[slot].scale;
            int x = mPops[slot].x;
            int y = mPops[slot].y;
            int scaledCos = (int)((cosAngle * scale + 0x800) >> 12);
            int scaledSin = FX_MUL(sinAngle, (int)scale);
            matrix[0] = scaledCos;
            matrix[3] = scaledCos;
            matrix[1] = scaledSin;
            matrix[2] = -scaledSin;
            Mg_RenderSpriteBothPalMtx(sSoundPopsSprites[mPops[slot].sprite], (x >> 12) - 8,
                                      (y >> 12) - 0x10, -1, matrix);
        }
        sinAngle = FX_SinCosTable_[0];
    }
}

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
    int slot;
    for (slot = 0; slot < 10; slot++) {
        if (mNotes[slot].shown != 0 && mNotes[slot].alive != 0) {
            int screenX = mNotes[slot].x >> 12;
            int screenY = mNotes[slot].y >> 12;
            int priority = 0;
            if (mState != 1)
                priority = 1;
            Hud_RenderSprite(sSoundNotesSprites[mNotes[slot].frame], screenX, screenY, -1,
                             priority);
        }
    }
}

void dScMgSound_c::DrawTries() {
    Mg_DrawStockCounter(mTries, 1);
}

void dScMgSound_c::StartWinSpriteA() {
    mSpriteA.active = 1;
    mSpriteA.timer = 0;
    mSpriteA.frame = 0;
}

void dScMgSound_c::UpdateWinSpriteA() {
    if (mSpriteA.active == 0)
        return;
    if (mSpriteA.frame >= 2)
        return;
    mSpriteA.timer++;
    if (mSpriteA.timer < sSoundWinSpriteATimes[mSpriteA.frame])
        return;
    mSpriteA.timer = 0;
    mSpriteA.frame++;
}

void dScMgSound_c::DrawWinSpriteA() {
    if (mSpriteA.visible == 0)
        return;
    Hud_RenderSprite(sSoundWinSpriteASprites[mSpriteA.frame], mSpriteA.x >> 0xc, mSpriteA.y >> 0xc,
                     -1, -1);
}

void dScMgSound_c::StartWinSpriteB() {
    mSpriteB.active = 1;
    mSpriteB.timer = 0;
    mSpriteB.frame = 0;
}

void dScMgSound_c::UpdateWinSpriteB() {
    if (mSpriteB.active == 0)
        return;
    if (mSpriteB.frame >= 3)
        return;
    mSpriteB.timer += 1;
    if (mSpriteB.timer < sSoundWinSpriteBTimes[mSpriteB.frame])
        return;
    mSpriteB.timer = 0;
    mSpriteB.frame += 1;
}

void dScMgSound_c::DrawWinSpriteB() {
    if (mSpriteB.visible == 0)
        return;
    Hud_RenderSprite(sSoundWinSpriteBSprites[mSpriteB.frame], mSpriteB.x >> 0xc, mSpriteB.y >> 0xc,
                     -1, -1);
}
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, so D1 is emitted before D0. */
