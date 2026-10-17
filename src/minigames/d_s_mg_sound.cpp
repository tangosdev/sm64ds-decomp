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
 * Leftover: DealNotes's deal loop reaches mPairUses and
 *   mPairNotes as q + 0x561a and q + 0x561f, with q = this + pair.
 *   Indexing the members by pair grows the function by 8 bytes.
 * Leftover: InitResources passes raw + 0x4660, the camera that
 *   dScMgSingle3DBase_c.h still leaves as padding.
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
struct PmfEntry { Pmf pmf; };


namespace Sound {
    void PlayBank2_2D(unsigned int id);
    void LoadAndSetMusic_Layer1(int music);
}

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
extern int Hud_RenderSprite(int a,int b,int c,int d,int e);
extern int sSoundWinSpriteBSprites[];
extern unsigned short sSoundWinSpriteBTimes[];
extern int sSoundWinSpriteASprites[];
extern u16 sSoundWinSpriteATimes[];
extern void Mg_DrawStockCounter(int a, int b);
extern const short data_02082214[];
extern void *sSoundPopsSprites[];
extern int sSoundPop12XOffsets[];
extern int sSoundPop12VX[];
extern int sSoundPop11XOffsets[];
extern int sSoundPop11VX[];
extern int sSoundPop10XOffsets[];
extern int sSoundPop10VX[];
extern int sSoundPop09XOffsets[];
extern int sSoundPop09VX[];
extern int sSoundPop08XOffsets[];
extern int sSoundPop07XOffsets[];
extern u32 sSoundPop06VX[];
extern int sSoundPop05XOffsets[];
extern int sSoundPop04VX[];
extern int sSoundPop03XOffsets[];
extern int sSoundPop03VX[];
extern int sSoundPop02XOffsets[];
extern int sSoundPop02VX[];
extern int sSoundPop01XOffsets[];
extern int sSoundPop01VX[];
extern int sSoundPop00XOffsets[];
extern int sSoundPop00VX[];
extern unsigned char sSoundPop00Dirs[];
extern u8 data_ov006_0212ee30[];
extern void func_02012790(int);
extern void Presenter_PlayMatch(void *c);
extern void Presenter_PlayMiss(char *c);
extern int sSoundRowLengthAlt[];
extern int sSoundRowLength[];
extern int sSoundScrollRowLengthAlt[];
extern int sSoundScrollRowLength[];
extern unsigned short sSoundNoteClosingTimes[];
void Presenter_PlayCheerLoop(char *p);
void func_02012174(u32 bank, u32 id);
extern u16 sSoundNoteOpeningTimes[];
extern u16 data_ov006_0213f794[];
extern u16 sSoundNoteSfx[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern u8 sSoundDealSpeeds[];
extern u8 sSoundDealCounts[];
extern u8 data_ov006_0212ee20[];
extern int sSoundPairNoteOffsets[];
extern int sSoundBankByLevel[];
extern u16 *sSoundNotePositions[];
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

// @symbol dScMgSound_c_classInit
/* The MG_SOUND registry factory. It sits directly after InitResources and
   installs this class's vtable, so it was compiled with the class.
   Still a hand-written constructor: the header declares none, so the base
   and Particle::SysTracker constructors are called by their mangled names.
   The two vtable stores differ on purpose. dScMgSingle3DBase_c's table is
   external and its symbol is the address point; this class's table is
   emitted here, where the symbol sits two words lower, hence the `+ 2`. */
extern "C" void *_ZN11dScMgBase_cC2Ev(void *);
extern "C" void _ZN8Particle10SysTrackerC1Ev(void *);
extern "C" void Presenter_Construct(void *);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV12dScMgSound_c[];

extern "C" void *dScMgSound_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x562c);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        *(void **)p = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(p + 0x471c);
        *(void **)p = _ZTV12dScMgSound_c + 2;
        Presenter_Construct(p + 0x4f38);
    }
    return p;
}

// @symbol _ZN12dScMgSound_c13InitResourcesEv
/* Loads the minigame's graphics for both screens, sets up the camera and
   the table, then deals the first round. */
s32 dScMgSound_c::InitResources()
{
    u8 *raw = (u8 *)this;
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

    Camera_InitPresenterView((void *)(raw + 0x4660));
    if (Presenter_Load((void *)&mTable) == 0)
        return 0;

    mTable.mSuppressSound = 1;
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
/* One frame of the round, by mState: 0 starts, 1 counts down mIntroTimer,
   2 plays the round and, when mResultTimer runs out, bumps the win counter
   (capped at 9999), 3 counts down a retry.
   The `(int)` casts on the three countdown decrements are needed: without
   them the address folds into the decrement and the function comes out
   short. */
s32 dScMgSound_c::Behavior()
{
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
                    mTable.mSuppressSound = 0;
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
                    Mg_SubmitScore(gMgScene != 0
                                            ? gMgScene->mHudScore : 0);
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
                mTable.mSuppressSound = 0;
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
s32 dScMgSound_c::Render()
{
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
void dScMgSound_c::StartRound(int mode)
{
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

    mTable.mSuppressSound = 1;
    ResetRound();

    mTries = 3;
    DealNotes();

    mIntroTimer = 0x20;
    Mg_ShowBanner(0xd, 0x80, 0xa8, 1, -1, 0xd);

    Sound::LoadAndSetMusic_Layer1(6);
}

// @symbol _ZN12dScMgSound_c11OnNewRecordEv
/* Slot 20, OnNewRecord: plays the table model's win animation. */
void dScMgSound_c::OnNewRecord()
{
    Presenter_PlayCheer((char *)&mTable);
}

// @symbol _ZN12dScMgSound_c10ResetRoundEv
void dScMgSound_c::ResetRound()
{
        int i;
    int cnt1;
    int cnt2;
    char *b = (char *)this;

    for (cnt1 = 0; cnt1 < 10; cnt1++) {
        dMgSoundNoteCur *note = (dMgSoundNoteCur *)b;
        note->note.x = 0;
        note->note.y = 0;
        note->note.timer = 0;
        note->note.state = 0;
        note->note.alive = 0;
        note->note.shown = 0;
        note->note.frame = 0;
        note->note.note = 0;
        b += 0x14;
    }

    for (cnt2 = 0; cnt2 < 5; cnt2++) {
        this->mPairUses[cnt2] = 0;
        this->mPairNotes[cnt2] = 0;
    }

    b = (char *)this;
    for (i = 0; i < 0x1e; i++) {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)b;
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
        b += 0x24;
    }

    this->mSpriteB.active = 0;
    this->mSpriteB.visible = 1;
    this->mSpriteB.x = 0xbd000;
    this->mSpriteB.y = 0x97000;
    this->mSpriteB.frame = 0;
    this->mSpriteA.active = 0;
    this->mSpriteA.visible = 1;
    this->mSpriteA.x = 0xa0000;
    this->mSpriteA.y = 0x9d000;
    this->mSpriteA.frame = 0;

    this->mQueue[1] = 0;
    this->mQueue[0] = this->mQueue[1];
    this->mQueueTimer = 0;
    this->mQueueLen = 0;
    this->mTouchCount = 0;
    this->mResultTimer = 0;

    Presenter_ResetPose((char *)&this->mTable);
}

// @symbol _ZN12dScMgSound_c9DealNotesEv
void dScMgSound_c::DealNotes()
{
        int sel;
    int k;
    int count;
    int n;
    int half;
    int speed;
    int i;

    sel = this->mLevel;
    this->mPrevPattern = this->mPattern;
    if (sel >= 5) {
        sel = (u32)(RND * 5) >> 15;
        if (sel == this->mPrevPattern) {
            sel += ((u32)(RND << 2) >> 15) + 1;
            if (sel >= 5)
                sel -= 5;
        }
    }
    this->mPattern = sel;
    k = this->mPattern;
    count = sSoundDealCounts[k];
    speed = sSoundDealSpeeds[k];
    if (k == 3) {
        int a = (u32)(RND << 2) >> 15;
        int b = (u32)(RND * 3) >> 15;
        this->mPairNotes[0] = b;
        this->mPairNotes[1] = this->mPairNotes[0] + 1;
        if (this->mPairNotes[1] >= 3)
            this->mPairNotes[1] -= 3;
        this->mPairNotes[2] = this->mPairNotes[1] + 1;
        if (this->mPairNotes[2] >= 3)
            this->mPairNotes[2] -= 3;
        this->mPairNotes[0] += a * 3;
        this->mPairNotes[1] += a * 3;
        this->mPairNotes[2] += a * 3;
    } else {
        half = count >> 1;
        for (i = 0; i < half; i++) {
            int v = (u32)(RND * speed) >> 15;
            this->mPairNotes[i] = v;
            if (i != 0) {
                int dup;
                int j;
                for (;;) {
                    dup = 0;
                    j = 0;
                    for (; j < i; j++) {
                        if (this->mPairNotes[i] == this->mPairNotes[j]) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup == 0)
                        break;
                    this->mPairNotes[i] = (u32)(RND * speed) >> 15;
                }
            }
        }
    }
    k = this->mPattern;
    if (k == 1 || k == 4) {
        this->mBank = sSoundBankByLevel[((u32)this->mLevel >> 2) & 3];
    }
    half = count >> 1;
    for (i = 0; i < half; i++) {
        this->mPairNotes[i] += sSoundPairNoteOffsets[this->mPattern];
    }
    {
        int xi;
        char *p;
        int zi;
        char *q;
        u8 *slot;
        int t;

        n = 0;
        if (count <= 0)
            return;
        zi = 1;
        xi = 0;
        p = (char *)this;
        do {
            dMgSoundNoteCur *note = (dMgSoundNoteCur *)p;
            t = data_ov006_0212ee20[this->mPattern];
            note->note.x = sSoundNotePositions[t][xi] << 12;
            note->note.y = (*(u16 *volatile *)&sSoundNotePositions[t])[zi] << 12;
            note->note.timer = 0;
            note->note.state = 0;
            note->note.alive = 1;
            note->note.shown = 1;
            note->note.frame = 0;
            note->note.checked = 0;
            do {
                q = (char *)this + ((u32)(RND * half) >> 15);
                slot = (u8 *)LAUNDER(q + 0x561a);
            } while (*slot >= 2);
            note->note.note = *(u8 *)(q + 0x561f);
            (*slot)++;
            xi += 2;
            p += 0x14;
            zi += 2;
            n++;
        } while (n < count);
    }
}

// @symbol _ZN12dScMgSound_c14Note_StateWaitEi
void dScMgSound_c::Note_StateWait(int slot)
{
    u8 player;
    int offset;
    int active;
    int dx;
    int dy;

    if (this->mState != 1) {
        return;
    }
    if (this->mTouchCount >= 2) {
        return;
    }

    player = gActivePlayerSlot;
    offset = player * 4;
    active = 0;
    if (gTouchHeld[offset] != 0) {
        if (gTouchEdge[offset] != 0) {
            active = 1;
        }
    }
    if (active == 0) {
        return;
    }

    dx = gTouchX[player * 4] - (this->mNotes[slot].x >> 12);
    dy = gTouchY[player * 4] - (this->mNotes[slot].y >> 12);

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

    this->mNotes[slot].state = 1;
    this->mNotes[slot].timer = 0;
    this->mNotes[slot].frame = 0;
    this->mTouchCount++;
    Sound::PlayBank2_2D(0x201);
}

// @symbol _ZN12dScMgSound_c17Note_StateOpeningEi
/* Advances one note slot's animation. When the slot's last frame is
   reached its note goes on mQueue and is played; mPattern and IsAltVariant
   choose between func_02012790, the bank handle mBank and
   Sound::PlayBank2_2D.
   SpawnPops is passed `idx` even though it is already in r1:
   dropping it changes the register allocation of the queue block. */
void dScMgSound_c::Note_StateOpening(int idx)
{
    u8 note;
    u8 instrument;

    this->mNotes[idx].timer = this->mNotes[idx].timer + 1;
    if (this->mNotes[idx].timer < sSoundNoteOpeningTimes[this->mNotes[idx].frame])
        return;
    this->mNotes[idx].timer = 0;
    this->mNotes[idx].frame = this->mNotes[idx].frame + 1;
    if (this->mNotes[idx].frame <= 6) return;
    this->mNotes[idx].frame = 6;
    this->mNotes[idx].state = 2;

    this->mQueue[this->mQueueLen] = this->mNotes[idx].note + 1;
    this->mQueue[this->mQueueLen] |= idx << 8;
    this->mQueueLen++;
    this->mQueueTimer = 0x20;

    SpawnPops(idx);
    note = this->mNotes[idx].note;

    if (this->IsAltVariant()) {
        instrument = this->mPattern;
        if (instrument == 0 || instrument == 2) {
            if (data_ov006_0213f794[note] == 2) {
                func_02012790(sSoundNoteSfx[note]);
            } else {
                func_02012174(this->mBank, sSoundNoteSfx[note]);
            }
        } else if (instrument == 1) {
            int n = note - 0x1a;
            int k = 0;
            while (n >= 4) { n -= 4; k++; }
            func_02012174(n, sSoundNoteSfx[0x1a + k * 4]);
        } else {
            Sound::PlayBank2_2D(sSoundNoteSfx[note]);
        }
    } else {
        instrument = this->mPattern;
        if (instrument == 0 || instrument == 2) {
            func_02012790(sSoundNoteSfx[note]);
        } else if (instrument == 1) {
            func_02012174(this->mBank, sSoundNoteSfx[note]);
        } else if (instrument == 4) {
            if (data_ov006_0213f794[note] == 2) {
                Sound::PlayBank2_2D(sSoundNoteSfx[note]);
            } else {
                func_02012174(this->mBank, sSoundNoteSfx[note]);
            }
        } else {
            Sound::PlayBank2_2D(sSoundNoteSfx[note]);
        }
    }

    if (this->mTouchCount == 1)
        Presenter_PlayCheerLoop((char *)&this->mTable);
}

// @symbol _ZN12dScMgSound_c14Note_StateOpenEi
void dScMgSound_c::Note_StateOpen(int slot) {
    if (this->mTouchCount == 0) {
        this->mNotes[slot].state = 3;
    }
}

// @symbol _ZN12dScMgSound_c17Note_StateClosingEi
void dScMgSound_c::Note_StateClosing(int slot){
    unsigned char b;
    this->mNotes[slot].timer = (unsigned short)(this->mNotes[slot].timer + 1);
    b = this->mNotes[slot].frame;
    if (this->mNotes[slot].timer < sSoundNoteClosingTimes[b])
        return;
    this->mNotes[slot].timer = 0;
    this->mNotes[slot].frame = (unsigned char)(this->mNotes[slot].frame - 1);
    if (this->mNotes[slot].frame == 0) this->mNotes[slot].state = 0;
}

// @symbol _ZN12dScMgSound_c16Note_StateScrollEi
/* Scrolls one note slot left by 0x10 per call and clears it once it has
   gone off screen. Its column comes from the per-mode tables
   sSoundScrollRowLengthAlt and sSoundScrollRowLength. */
void dScMgSound_c::Note_StateScroll(int slot)
{
    int n;
    int i;
    int limit;

    this->mNotes[slot].x -= 0x10000;

    if (this->IsAltVariant() != 0) {
        limit = sSoundScrollRowLengthAlt[this->mPattern];
    } else {
        limit = sSoundScrollRowLength[this->mPattern];
    }

    if (this->mNotes[slot].checked == 0) {
        n = slot;
        if (slot >= limit) {
            do {
                n = n - limit;
            } while (n >= limit);
        }
        if (n == 0) {
            this->mNotes[slot].checked = 1;
            return;
        }
        for (i = 0; i < n; i++) {
            int prev = slot - i - 1;
            if (this->mNotes[prev].alive != 0) {
                if ((this->mNotes[slot].x - this->mNotes[prev].x) >> 0xc <= 4) {
                    this->mNotes[slot].checked = 1;
                    this->mNotes[prev].state = 4;
                }
            }
        }
    }

    if (this->mNotes[slot].x >> 0xc > -0x18) return;

    this->mNotes[slot].alive = 0;
    this->mNotes[slot].shown = 0;
}

// @symbol _ZN12dScMgSound_c15RevealUnmatchedEv
void dScMgSound_c::RevealUnmatched() {
        int lr;
    int ip;
    int found;
    int off;
    if (this->IsAltVariant()) {
        lr = sSoundRowLengthAlt[this->mPattern];
    } else {
        lr = sSoundRowLength[this->mPattern];
    }
    for (ip = 0, off = 0; ip < 2; ip++) {
        int sb;
        found = -1;
        for (sb = 0; sb < lr; sb++) {
            int e = sb + off;
            if (this->mNotes[e].alive != 0)
                found = e;
        }
        if (found != -1) {
            this->mNotes[found].state = 4;
        }
        off += lr;
    }
}

// @symbol _ZN12dScMgSound_c11UpdateNotesEv
void dScMgSound_c::UpdateNotes(){
  int i=0;
  char *r5 = (char *)this;
  do{
    dMgSoundNoteCur *note = (dMgSoundNoteCur *)r5;
    if(note->note.alive != 0){
      int idx=note->note.state;
      (this->*sSoundNoteStateTable[idx].pmf)(i);
    }
    i++;
    r5+=0x14;
  }while(i<10);
}

// @symbol _ZN12dScMgSound_c9CheckPairEv
void dScMgSound_c::CheckPair(){
    if(this->mQueueLen < 2) return;
  if(this->mQueueTimer != 0){
    this->mQueueTimer -= 1;
    if((short)this->mQueueTimer <= 0) this->mQueueTimer = 0;
    return;
  }
  if((this->mQueue[0] & 0xff) == (this->mQueue[1] & 0xff)){
    this->mNotes[this->mQueue[0] >> 8].alive = 0;
    this->mNotes[this->mQueue[1] >> 8].alive = 0;
    func_02012790(0x26);
    Presenter_PlayMatch(&this->mTable);
  } else {
    this->mNotes[this->mQueue[0] >> 8].state = 3;
    this->mNotes[this->mQueue[1] >> 8].state = 3;
    func_02012790(0xe);
    this->mTries--;
    func_02012790(0x12f);
    Presenter_PlayMiss((char *)&this->mTable);
  }
  this->mQueue[1] = 0;
  this->mQueue[0] = this->mQueue[1];
  this->mQueueLen = 0;
  this->mTouchCount = 0;
}

// @symbol _ZN12dScMgSound_c14CheckRoundOverEv
void dScMgSound_c::CheckRoundOver()
{
        if (this->mTries == 0) {
        this->mState = 2;
        this->mResultTimer = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char *p = (char *)this;
        do {
            if (((dMgSoundNoteCur *)p)->note.alive != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        this->mResultTimer = 0x50;
        this->mState = 2;
    }
}

// @symbol _ZN12dScMgSound_c9SpawnPopsEi
void dScMgSound_c::SpawnPops(int n)
{
    int k;
    int i;
    u8* g = data_ov006_0212ee30;

    for (k = 0; k < 3; g++, k++) {
        for (i = 0; i < 30; i++) {
            if (this->mPops[i].active == 0) {
                this->mPops[i].active = 1;
                this->mPops[i].x = this->mNotes[n].x;
                this->mPops[i].y = this->mNotes[n].y - 0x8000;
                this->mPops[i].startY = this->mPops[i].y;
                this->mPops[i].sprite = *g;
                this->mPops[i].kind = *g;
                this->mPops[i].timer = 0;
                if (this->mNotes[n].note == 7) {
                    if (k >= 3) { this->mPops[i].timer = 0x10; }
                    else { this->mPops[i].timer = 8; }
                }
                this->mPops[i].scale = 0x1000;
                this->mPops[i].velX = 0;
                this->mPops[i].velY = 0;
                this->mPops[i].state = 0;
                this->mPops[i].phase = 0;
                break;
            }
        }
    }
}

// @symbol _ZN12dScMgSound_c10UpdatePopsEv
void dScMgSound_c::UpdatePops(){
  int i=0;
  char *r5 = (char *)this;
  do{
    dMgSoundPopCur *pop = (dMgSoundPopCur *)r5;
    if(pop->pop.active != 0){
      int idx=pop->pop.state;
      (this->*sSoundPopUpdateTable[idx].pmf)(i);
    }
    i++;
    r5+=0x24;
  }while(i<30);
}

// @symbol _ZN12dScMgSound_c12Pop00_UpdateEi
void dScMgSound_c::Pop00_Update(int slot)
{
    unsigned char idx = this->mPops[slot].phase;
    (this->*sSoundPop00Table[idx])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop00_StartEi
void dScMgSound_c::Pop00_Start(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].x += sSoundPop00XOffsets[this->mPops[slot].kind];
    this->mPops[slot].velY = 0;
    this->mPops[slot].velX = sSoundPop00VX[this->mPops[slot].kind];
    this->mPops[slot].dir = sSoundPop00Dirs[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop00_FlyEi
void dScMgSound_c::Pop00_Fly(int slot)
{
    int dir;

    this->mPops[slot].x += this->mPops[slot].velX;

    dir = this->mPops[slot].dir;
    if (dir == 0) {
        this->mPops[slot].velX -= 0x100;
        if (this->mPops[slot].velX < -0xc00) {
            this->mPops[slot].velX = -0xc00;
            this->mPops[slot].dir = 1;
        }
    } else if (dir != 0) {
        this->mPops[slot].velX += 0x100;
        if (this->mPops[slot].velX > 0xc00) {
            this->mPops[slot].velX = 0xc00;
            this->mPops[slot].dir = 0;
        }
    }

    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0xc0;

    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 12 >= 0x40) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop00_FinishEv
void dScMgSound_c::Pop00_Finish()
{
}

// @symbol _ZN12dScMgSound_c12Pop01_UpdateEi
void dScMgSound_c::Pop01_Update(int slot)
{
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop01Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop01_StartEi
void dScMgSound_c::Pop01_Start(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += sSoundPop01XOffsets[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x2800;
  this->mPops[slot].velX = sSoundPop01VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop01_FlyEi
void dScMgSound_c::Pop01_Fly(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x200;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop01_FinishEi
void dScMgSound_c::Pop01_Finish(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x100;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x18)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c12Pop02_UpdateEi
void dScMgSound_c::Pop02_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop02Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop02_StartEi
void dScMgSound_c::Pop02_Start(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].x += sSoundPop02XOffsets[this->mPops[slot].kind];
    this->mPops[slot].velY = -0x2000;
    this->mPops[slot].velX = sSoundPop02VX[this->mPops[slot].kind];
    this->mPops[slot].timer = 0;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop02_FlyEi
void dScMgSound_c::Pop02_Fly(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x100;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop02_FinishEi
void dScMgSound_c::Pop02_Finish(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x100;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x10)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c12Pop03_UpdateEi
void dScMgSound_c::Pop03_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop03Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop03_StartEi
void dScMgSound_c::Pop03_Start(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += sSoundPop03XOffsets[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x4800;
  this->mPops[slot].velX = sSoundPop03VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop03_FlyEi
void dScMgSound_c::Pop03_Fly(int slot){
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop03_FinishEi
void dScMgSound_c::Pop03_Finish(int slot){
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc) > 0x20)
    return;
  if (this->mPops[slot].velY > 0) {
    this->mPops[slot].active = 0;
    this->mPops[slot].visible = 0;
  }
}

// @symbol _ZN12dScMgSound_c12Pop04_UpdateEi
void dScMgSound_c::Pop04_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop04Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop04_StartEi
void dScMgSound_c::Pop04_Start(int slot) {
    this->mPops[slot].scale = 0x3000;
    this->mPops[slot].visible = 1;
    this->mPops[slot].velY = -0x1000;
    this->mPops[slot].velX = sSoundPop04VX[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop04_FlyEi
void dScMgSound_c::Pop04_Fly(int slot)
{
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    if (this->mPops[slot].scale > 0x800) {
        this->mPops[slot].scale -= 0x100;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].velY -= 0x20;
    {
        int d = (this->mPops[slot].startY - this->mPops[slot].y) >> 12;
        if (d >= 0x38) {
            this->mPops[slot].active = 0;
            this->mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c12Pop04_FinishEv
void dScMgSound_c::Pop04_Finish()
{
}

// @symbol _ZN12dScMgSound_c12Pop05_UpdateEi
void dScMgSound_c::Pop05_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop05Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop05_StartEi
void dScMgSound_c::Pop05_Start(int slot) {
    unsigned char k;
    this->mPops[slot].visible = 1;
    k = this->mPops[slot].kind;
    this->mPops[slot].x += sSoundPop05XOffsets[k];
    this->mPops[slot].velY = -0x4000;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop05_FlyEi
void dScMgSound_c::Pop05_Fly(int slot)
{
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop05_FinishEi
void dScMgSound_c::Pop05_Finish(int slot) {
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x100;
    if (((this->mPops[slot].startY - this->mPops[slot].y) >> 12) <= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop06_UpdateEi
void dScMgSound_c::Pop06_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop06Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop06_StartEi
/* Arms one pop: visible, no vertical speed, horizontal speed from its kind. */
void dScMgSound_c::Pop06_Start(int slot)
{
    this->mPops[slot].visible = 1;
    this->mPops[slot].velY = 0;
    this->mPops[slot].velX = sSoundPop06VX[this->mPops[slot].kind];
    this->mPops[slot].phase = 1;
    this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c9Pop06_FlyEi
void dScMgSound_c::Pop06_Fly(int slot)
{
    int v;
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    v = this->mPops[slot].velX;
    if (v > 0x20) {
        this->mPops[slot].velX = v - 0x20;
    } else if (v < -0x20) {
        this->mPops[slot].velX = v + 0x20;
    }
    this->mPops[slot].velY -= 0x40;
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x28) {
        this->mPops[slot].timer = 0;
        this->mPops[slot].phase = 2;
        this->mPops[slot].dir = 0;
        this->mPops[slot].velX = 0xc00;
    }
}

// @symbol _ZN12dScMgSound_c12Pop06_FinishEi
void dScMgSound_c::Pop06_Finish(int slot)
{
    int f;
    this->mPops[slot].x += this->mPops[slot].velX;
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0x40;
    f = this->mPops[slot].dir;
    if (f == 0) {
        this->mPops[slot].velX -= 0x100;
        if (this->mPops[slot].velX < -0xc00) {
            this->mPops[slot].velX = -0xc00;
            this->mPops[slot].dir = 1;
        }
    } else if (f != 0) {
        this->mPops[slot].velX += 0x100;
        if (this->mPops[slot].velX > 0xc00) {
            this->mPops[slot].velX = 0xc00;
            this->mPops[slot].dir = 0;
        }
    }
    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 12 >= 0x30) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop07_UpdateEi
void dScMgSound_c::Pop07_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop07Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop07_StartEi
void dScMgSound_c::Pop07_Start(int slot) {
  unsigned short v = this->mPops[slot].timer;
  if (v != 0) {
    this->mPops[slot].timer = v - 1;
    if (*(short *)&this->mPops[slot].timer < 0)
      this->mPops[slot].timer = 0;
    return;
  }
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += sSoundPop07XOffsets[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x4000;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop07_FlyEi
void dScMgSound_c::Pop07_Fly(int slot)
{
    int a = this->mPops[slot].velY;
    int b = this->mPops[slot].y;
    this->mPops[slot].y = b + a;
    a = this->mPops[slot].velY;
    this->mPops[slot].velY = a + 0x400;
    {
        int d = this->mPops[slot].startY - this->mPops[slot].y;
        d >>= 12;
        if (d >= 0x20) {
            this->mPops[slot].active = 0;
            this->mPops[slot].visible = 0;
        }
    }
}

// @symbol _ZN12dScMgSound_c12Pop07_FinishEv
void dScMgSound_c::Pop07_Finish()
{
}

// @symbol _ZN12dScMgSound_c12Pop08_UpdateEi
void dScMgSound_c::Pop08_Update(int slot) {
    unsigned char state = this->mPops[slot].phase;
    (this->*sSoundPop08Table[state].pmf)(slot);
}

// @symbol _ZN12dScMgSound_c11Pop08_StartEi
void dScMgSound_c::Pop08_Start(int slot) {
    unsigned char k;
    this->mPops[slot].visible = 1;
    k = this->mPops[slot].kind;
    this->mPops[slot].x += sSoundPop08XOffsets[k];
    this->mPops[slot].velY = -0x4000;
    this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop08_FlyEi
void dScMgSound_c::Pop08_Fly(int slot)
{
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY += 0x400;
    if (this->mPops[slot].velY > 0) {
        this->mPops[slot].velY = 0;
        this->mPops[slot].phase = 2;
    }
}

// @symbol _ZN12dScMgSound_c12Pop08_FinishEi
void dScMgSound_c::Pop08_Finish(int slot) {
    this->mPops[slot].y += this->mPops[slot].velY;
    this->mPops[slot].velY -= 0x60;
    if ((this->mPops[slot].startY - this->mPops[slot].y) >> 0xc >= 0x3c) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop09_UpdateEi
void dScMgSound_c::Pop09_Update(int slot)
{
    unsigned char idx = this->mPops[slot].phase;
    (this->*sSoundPop09Table[idx])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop09_StartEi
void dScMgSound_c::Pop09_Start(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += sSoundPop09XOffsets[idx];
  this->mPops[slot].velY = -0x3800;
  this->mPops[slot].velX = sSoundPop09VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop09_FlyEi
void dScMgSound_c::Pop09_Fly(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop09_FinishEi
void dScMgSound_c::Pop09_Finish(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0x800) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x20) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop10_UpdateEi
void dScMgSound_c::Pop10_Update(int slot){
  unsigned char sel = this->mPops[slot].phase;
  (this->*sSoundPop10Table[sel])(slot);
}

// @symbol _ZN12dScMgSound_c11Pop10_StartEi
void dScMgSound_c::Pop10_Start(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += sSoundPop10XOffsets[idx];
  this->mPops[slot].velY = -0x3200;
  this->mPops[slot].velX = sSoundPop10VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop10_FlyEi
void dScMgSound_c::Pop10_Fly(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop10_FinishEi
void dScMgSound_c::Pop10_Finish(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0x800) {
        this->mPops[slot].scale = v - 0x70;
        if (this->mPops[slot].scale < 0x800)
            this->mPops[slot].scale = 0x800;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x20) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop11_UpdateEi
void dScMgSound_c::Pop11_Update(int slot){
  unsigned char phase = this->mPops[slot].phase;
  (this->*(sSoundPop11Table[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c11Pop11_StartEi
void dScMgSound_c::Pop11_Start(int slot)
{
  this->mPops[slot].visible = 1;
  this->mPops[slot].x += sSoundPop11XOffsets[this->mPops[slot].kind];
  this->mPops[slot].velY = -0x2800;
  this->mPops[slot].velX = sSoundPop11VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop11_FlyEi
void dScMgSound_c::Pop11_Fly(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop11_FinishEi
void dScMgSound_c::Pop11_Finish(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0xa80) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0xa80)
            this->mPops[slot].scale = 0xa80;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c12Pop12_UpdateEi
void dScMgSound_c::Pop12_Update(int slot){
  unsigned char phase = this->mPops[slot].phase;
  (this->*(sSoundPop12Table[phase].pmf))(slot);
}

// @symbol _ZN12dScMgSound_c11Pop12_StartEi
void dScMgSound_c::Pop12_Start(int slot)
{
  unsigned char idx;
  this->mPops[slot].visible = 1;
  idx = this->mPops[slot].kind;
  this->mPops[slot].x += sSoundPop12XOffsets[idx];
  this->mPops[slot].velY = -0x2400;
  this->mPops[slot].velX = sSoundPop12VX[this->mPops[slot].kind];
  this->mPops[slot].timer = 0;
  this->mPops[slot].phase = 1;
}

// @symbol _ZN12dScMgSound_c9Pop12_FlyEi
void dScMgSound_c::Pop12_Fly(int slot) {
  this->mPops[slot].x += this->mPops[slot].velX;
  this->mPops[slot].y += this->mPops[slot].velY;
  this->mPops[slot].velY += 0x200;
  if (this->mPops[slot].velY <= 0) return;
  this->mPops[slot].velY = 0;
  this->mPops[slot].phase = 2;
  this->mPops[slot].timer = 0;
}

// @symbol _ZN12dScMgSound_c12Pop12_FinishEi
void dScMgSound_c::Pop12_Finish(int slot)
{
    int v = this->mPops[slot].scale;
    if (v > 0xd80) {
        this->mPops[slot].scale = v - 0x60;
        if (this->mPops[slot].scale < 0xd80)
            this->mPops[slot].scale = 0xd80;
    }
    this->mPops[slot].timer += 1;
    if (this->mPops[slot].timer >= 0x18) {
        this->mPops[slot].active = 0;
        this->mPops[slot].visible = 0;
    }
}

// @symbol _ZN12dScMgSound_c8DrawPopsEv
extern "C" void Mg_RenderSpriteBothPalMtx(void* a0, int a1, int a2, int a3, void* a4);

void dScMgSound_c::DrawPops()
{
    char *p = (char *)this;
    int i;
    int tb = data_02082214[0];
    long long ta = data_02082214[1];

    for (i = 0; i < 30; i++) {
        dMgSoundPopCur *pop = (dMgSoundPopCur *)p;
        if (pop->pop.visible) {
            int m[4];
            long long vv = pop->pop.scale;
            int x = pop->pop.x;
            int y = pop->pop.y;
            int a = (int)((ta * vv + 0x800) >> 12);
            int b = FX_MUL(tb, (int)vv);
            m[0] = a;
            m[3] = a;
            m[1] = b;
            m[2] = -b;
            Mg_RenderSpriteBothPalMtx(sSoundPopsSprites[pop->pop.sprite], (x >> 12) - 8,
                                      (y >> 12) - 0x10, -1, m);
        }
        tb = data_02082214[0];
        p += 0x24;
    }
}

// @symbol _ZN12dScMgSound_c9DrawNotesEv
extern void *sSoundNotesSprites[];
void dScMgSound_c::DrawNotes() {
  char *c = (char *)this;
  int i;
  char* o = c;
  for (i = 0; i < 10; i++) {
    dMgSoundNoteCur *note = (dMgSoundNoteCur *)o;
    if (note->note.shown != 0 && note->note.alive != 0) {
      int a1 = note->note.x >> 12;
      int a2 = note->note.y >> 12;
      int a4 = 0;
      if (mState != 1)
          a4 = 1;
      unsigned char sel = note->note.frame;
      Hud_RenderSprite((int)sSoundNotesSprites[sel], a1, a2, -1, a4);
    }
    o += 0x14;
  }
}

// @symbol _ZN12dScMgSound_c9DrawTriesEv
void dScMgSound_c::DrawTries()
{
    Mg_DrawStockCounter(this->mTries, 1);
}

// @symbol _ZN12dScMgSound_c15StartWinSpriteAEv
void dScMgSound_c::StartWinSpriteA()
{
        this->mSpriteA.active = 1;
    this->mSpriteA.timer = 0;
    this->mSpriteA.frame = 0;
}

// @symbol _ZN12dScMgSound_c16UpdateWinSpriteAEv
void dScMgSound_c::UpdateWinSpriteA()
{
        if (this->mSpriteA.active == 0)
        return;
    if (this->mSpriteA.frame >= 2)
        return;
    (*(u16 *)(int)&this->mSpriteA.timer)++;
    if (this->mSpriteA.timer < sSoundWinSpriteATimes[this->mSpriteA.frame])
        return;
    this->mSpriteA.timer = 0;
    (*(u8 *)(int)&this->mSpriteA.frame)++;
}

// @symbol _ZN12dScMgSound_c14DrawWinSpriteAEv
void dScMgSound_c::DrawWinSpriteA(){
    if (this->mSpriteA.visible == 0) return;
    Hud_RenderSprite(sSoundWinSpriteASprites[this->mSpriteA.frame], this->mSpriteA.x >> 0xc,
                     this->mSpriteA.y >> 0xc, -1, -1);
}

// @symbol _ZN12dScMgSound_c15StartWinSpriteBEv
void dScMgSound_c::StartWinSpriteB()
{
        this->mSpriteB.active = 1;
    this->mSpriteB.timer = 0;
    this->mSpriteB.frame = 0;
}

// @symbol _ZN12dScMgSound_c16UpdateWinSpriteBEv
void dScMgSound_c::UpdateWinSpriteB()
{
        if (this->mSpriteB.active == 0) return;
    if (this->mSpriteB.frame >= 3) return;
    (*(u16 *)(int)&this->mSpriteB.timer) += 1;
    if (this->mSpriteB.timer < sSoundWinSpriteBTimes[this->mSpriteB.frame])
        return;
    this->mSpriteB.timer = 0;
    (*(u8 *)(int)&this->mSpriteB.frame) += 1;
}

// @symbol _ZN12dScMgSound_c14DrawWinSpriteBEv
void dScMgSound_c::DrawWinSpriteB(){
    if (this->mSpriteB.visible == 0) return;
    Hud_RenderSprite(sSoundWinSpriteBSprites[this->mSpriteB.frame], this->mSpriteB.x >> 0xc,
                     this->mSpriteB.y >> 0xc, -1, -1);
}

// @symbol _ZN12dScMgSound_cD1Ev
// @symbol _ZN12dScMgSound_cD0Ev
/* Both destructor variants come from the inline body in dScMgSound_c.h.
   Declared first in the class, so D1 is emitted before D0. */
