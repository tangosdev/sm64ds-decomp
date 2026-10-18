#ifndef DSCMGHANACHAN_C_H
#define DSCMGHANACHAN_C_H
#include "dScMgBase_c.h"

/* Which Wiggler? Up to fifteen wigglers crawl on the touch screen and one
 * of them has the star. mTimeLeft is the seconds remaining; a wrong touch
 * takes ten off.
 *
 * The name is the ROM's RTTI name, and rtti_extract.py confirms it is a
 * leaf (no RTTI record names it as a base). dScMgHanachan_c_classInit
 * (alias MgWhichWiggler_Spawn) installs its vtable for the MG_HANACHAN
 * profile.
 *
 * The destructor is non-trivial: D1 and D0 both call __cxa_vec_cleanup on
 * the 15x0x98 array at 0x4678, as dScMgBase_c's own D1/D0 do. D0's
 * deallocation comes from dScMgBase_c's operator delete. __cxa_vec_cleanup
 * and the array's element destructor Wiggler_Destroy are declared here,
 * where both the destructor and dScMgHanachan_c_classInit reach them, like
 * func_ov004_020b929c in dScMgBase_c.h.
 */
extern "C" void __cxa_vec_cleanup(void *arr, int count, int elemSize, void *dtor);
extern "C" void *Wiggler_Destroy(void *self);

struct dScMgHanachan_c : dScMgBase_c {
    virtual ~dScMgHanachan_c();
    virtual s32 InitResources();      /* slot 0 */
    virtual s32 CleanupResources();   /* slot 3 */
    virtual s32 Behavior();           /* slot 6 */
    virtual s32 Render();             /* slot 9 */
    virtual void StartRound(int arg); /* slot 18 */

    /* Behavior dispatches the scene phase; Render compares it with its
       phase methods. The target ABI stores this member pointer in 8 bytes. */
    typedef void (dScMgHanachan_c::*Phase)();
    Phase mPhase;    /* 0x4660 */
    s32 mDelay;      /* 0x4668 frames before the prompt */
    s32 mPhaseTimer; /* 0x466c per-phase countdown: 0x4b0 at play start
                            (020ed81c), 0xb4 in 020ed300; Render and
                            020ed34c key the hit/miss beats off 0x5a/0x8a */
    s32 mTimeLeft;   /* 0x4670 seconds left */
    s32 mTimeTick;   /* 0x4674 counts down one second */
    /* Descriptive record names; the ROM supplies the field layout, not names.
       Lifecycle calls remain explicit; Wiggler callback methods are native. */
    struct Point {
        s32 x, y;
    };
    struct Wiggler {
        typedef void (Wiggler::*State)();
        State state;         /* 0x00 tick callback */
        State saved;         /* 0x08 */
        State sub;           /* 0x10 callback the pop states run */
        Point seg[5];        /* 0x18 segment positions */
        int speed;           /* 0x40 */
        int speedTarget;     /* 0x44 */
        int flower;          /* 0x48 bloom, Fix12; 0x3000 when the flower shows */
        int stretch[5];      /* 0x4c extra gap per segment */
        int popCount;        /* 0x60 */
        int index;           /* 0x64 index in the scene array */
        int voice;           /* 0x68 sound handle */
        int voiceId;         /* 0x6c */
        int decoy;           /* 0x70 0 = this wiggler has the star */
        u16 color;           /* 0x74 four 2-bit body colors */
        s16 aim;             /* 0x76 */
        s16 timer;           /* 0x78 */
        s16 ang[5];          /* 0x7a */
        s16 bob;             /* 0x84 */
        s16 bobRate;         /* 0x86 */
        int round;           /* 0x88 */
        s16 tries;           /* 0x8c */
        s16 flash;           /* 0x8e */
        s16 hold;            /* 0x90 */
        u8 alive;            /* 0x92 */
        u8 busy;             /* 0x93 */
        u8 bloom;            /* 0x94 draw the flower */
        u8 face;             /* 0x95 head sprite */
        u8 pad96[2];

        void Update();
        void StateHold();
        void StateMoveA();
        void StateWait();
        void StateMoveB();
        void MoveBounce();
        void CheckOffLeftA();
        void CheckOffLeftB();
        void TurnTowardAim();
    };
    Wiggler mWiggler[15]; /* 0x4678, five segment points per wiggler */
    Wiggler *mTarget;     /* 0x4f60 the starred wiggler */
    u8 mHit;              /* 0x4f64 the star was touched */

    /* Phase and tap handlers of the scene. These descriptive method names
       are reconstructions; no ROM spelling survives. Four are the PMF targets of the phase records
       (Phase_WaitTouch, Phase_Result, Phase_Play,
       Phase_WaitIntro); the rest are called on `this` from them and from
       StartRound. */
    void Phase_NoneA();
    void Phase_NoneB();
    void Phase_WaitTouch();
    void BeginResultWait();
    void SetPhaseIdle();
    void Phase_Result();
    void BeginResult();
    void Phase_Play();
    void BeginPlay();
    void Phase_WaitIntro();
    void Phase_SetupRound();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    dScMgHanachan_c_Phase_size_must_be_0x08[sizeof(dScMgHanachan_c::Phase) == 0x08 ? 1 : -1];
typedef char dScMgHanachan_c_point_size_must_be_8[sizeof(dScMgHanachan_c::Point) == 8 ? 1 : -1];
typedef char
    dScMgHanachan_c_wiggler_size_must_be_0x98[sizeof(dScMgHanachan_c::Wiggler) == 0x98 ? 1 : -1];
typedef char dScMgHanachan_c_size_must_be_0x4f68[sizeof(dScMgHanachan_c) == 0x4f68 ? 1 : -1];
#endif

#endif
