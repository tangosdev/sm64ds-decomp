#ifndef DADEMO_C_H
#define DADEMO_C_H

#ifdef __cplusplus

#include "dActor_c.h"
#include "dExtAnmModel_c.h"
#include "dPa_c.h"

namespace Memory {
void operator_delete2(void *);
}

struct dExtAnmTexPat_c;

/* The nested model helpers below derive from the real model classes, which
 * carry the cartridge's RTTI spellings: dExtSimpleModel_c (_ZTI at
 * arm9:0x0208e794) and dExtAnmModel_c (_ZTI at arm9:0x0208e924), over the
 * root dExtModel_c (arm9:0x0208e73c). daDemo_c's typeinfo and vtable records
 * are emitted by this translation unit, and their base pointers name those
 * records directly. notes/model-rtti-names.md maps the spellings. */

/* The cutscene-only actor family, vtable _ZTV8daDemo_c.
 *
 * The factory allocates 0x104 bytes, constructs dActor_c, and installs this
 * vtable. The destructor performs no class-local teardown: it changes the
 * vptr, runs dActor_c's destruction, and releases the actor allocation. That
 * is exactly the code generated for an empty destructor on this inheritance
 * graph; none of those operations belongs in the source body.
 *
 * The two owned render objects are selected by param1. InitResources writes a
 * dExtSimpleModel_c pointer at 0xdc for the static variants and a dExtAnmModel_c pointer at
 * 0xe0 for the animated variants; CleanupResources destroys whichever exists.
 */
struct daDemo_c : dActor_c {
    /* The model helpers nested in daDemo_c share a scale-bearing base the
       ROM's RTTI names daDemo_c::param_c. It is declared first so it lands
       as the non-primary base at the object tail (0x64 in anmModel_c, 0x50
       in simpleModel_c): bases destroy in reverse declaration order, so the
       dExtSimpleModel_c/dExtAnmModel_c base runs before the Vector3 array, which is the
       ROM's order and what its vmi typeinfo records encode. Members a
       nested class declares land after both bases (simpleModel_c's mFile
       at 0x5c, anmModel_c's files at 0x70). */
    struct param_c {
        Vector3 mScale[1];

        param_c();
    };

    struct anmModel_c : param_c, dExtAnmModel_c {
        /* Stored by func_ov002_020f6618, released by the destructor. */
        SharedFilePtr *mModelFile;    /* 0x70 - BMD handed to SetFile */
        SharedFilePtr **mAnimFiles;   /* 0x74 - BCA files for SetAnim */
        SharedFilePtr **mTexSeqFiles; /* 0x78 - BTP files for the dExtAnmTexPat_c */
        dExtAnmTexPat_c *mTexSeq;     /* 0x7c - allocated only when BTP files are given */
        u8 mNumAnimFiles;             /* 0x80 */
        u8 mNumTexSeqFiles;           /* 0x81 */
        /* Entry of the 4-byte animation table (anim file, BTP file, loop
           flag, next entry) last applied by func_ov002_020f6514;
           func_ov002_020f64ac follows the entry's next field when the
           animation finishes. */
        u8 mAnimEntry; /* 0x82 */
        /* func_ov002_020f6618's fourth argument (1 for param1 0x12/0x13):
           while set, Behavior probes for the floor and lifts the actor
           back onto it when it has sunk below. */
        u8 mSnapToFloor; /* 0x83 */

        /* No declared constructor: InitResources inlines the implicit one at
           every new (param_c's constructor on the tail base, dExtAnmModel_c's
           C2, then both vptr stores). Declaring an empty anmModel_c() moves
           this class's destructor variants out of ROM order (tubuild verify
           fails at ordinals 112/113), so both nested classes keep theirs
           implicit. */
        void func_ov002_020f64ac(char *r4);
        void func_ov002_020f6514(void *tbl, unsigned char arg);
        void func_ov002_020f65b8();
        void func_ov002_020f65ec();
        int func_ov002_020f6618(SharedFilePtr *mdl, int nAnims,
                                SharedFilePtr **anims, int arg5,
                                unsigned char texByte, SharedFilePtr **texs,
                                int tsData);
        virtual ~anmModel_c();
        void operator delete(void *ptr) {
            Memory::operator_delete2(ptr);
        }
    };

    struct simpleModel_c : param_c, dExtSimpleModel_c {
        /* Stored and loaded by func_ov002_020f6960, released by the destructor. */
        SharedFilePtr *mFile; /* 0x5c */

        int func_ov002_020f6960(SharedFilePtr *fp, int n);
        virtual ~simpleModel_c();
        void operator delete(void *ptr) {
            Memory::operator_delete2(ptr);
        }
    };

    /* The 0x36-byte block param1 0x2f allocates (func_ov002_020f2630) and
       steps through func_ov002_020f23f0's seven states. Names describe what
       the code does with each field; the source type is not recovered. The
       sprite arrays have four slots, of which the loops touch three. */
    struct ScrollState {
        s16 bg0X;            /* 0x00 - SetSubBg0Offset x */
        s16 bg1X;            /* 0x02 - SetSubBg1Offset x */
        s16 bg0Y;            /* 0x04 - SetSubBg0Offset y */
        s16 bg1Y;            /* 0x06 - SetSubBg1Offset y */
        u16 centerSpriteY[2]; /* 0x08 - y of the two data_ov002_0210bddc sprites at x 0x80 */
        s16 spriteX[4];      /* 0x0c */
        s16 spriteY[4];      /* 0x14 */
        s16 spriteVelX[4];   /* 0x1c - +1 or -1 */
        u16 spriteDelay[4];  /* 0x24 - frames until the slot respawns */
        u16 fadeTimer;       /* 0x2c - counts to 0x18, then fadeLevel steps */
        u16 fadeLevel;       /* 0x2e - sub-screen blend weight, 0x10 down to 0 */
        u8 spriteActive[4];  /* 0x30 */
        u8 state;            /* 0x34 - 0..6 */
    };

    /* The 0x518-byte block param1 0x2e allocates (func_ov002_020f6448),
       sets up (func_ov002_020f5fe4: the D_2D_STAFF_ROLL_* and D_2D_ED_STAR
       assets), steps (5fb8) and draws (5f8c). Its helpers are free
       functions taking the block as char *; the cue tables they dispatch
       through are data_ov002_02110f34 and data_ov002_02110f9c. Names
       describe what the code does with each field; the source type is not
       recovered. */
    struct StaffRoll {
        /* One of the four main-screen stars, one per unlocked character
           (func_ov002_020f5a94 counts them). 3978 draws it, 39ec copies
           its animation frame out of starChars, and 5990 runs its state
           handler and lets it emit the sparkle particle. */
        struct Star {
            s32 x; /* 0x00 - fix12 screen position */
            s32 y; /* 0x04 */
            /* Speed along heading while approaching the target; the x
               velocity while flying off (x += speed); 569c steps x by it
               and y by speedY. */
            s32 speed;       /* 0x08 */
            s32 speedY;      /* 0x0c - only 569c (and 57c0, which sets it) */
            s32 radius;      /* 0x10 - orbit radius around (0x80, 0x60) */
            u8 pad_14[8];    /* 0x14 */
            s32 targetX;     /* 0x1c - approach target, fix12 */
            s32 targetY;     /* 0x20 */
            s32 orbitTravel; /* 0x24 - sum of angSpeed; 0x10000 is one turn */
            /* Handed by 5990 to func_ov002_020f2790, which stores them in
               the kira callback's unk308/unk30c before spawning the
               sparkle; they ease up from 0x199 to 0x1000 and 0x800. */
            s32 kiraDefParam; /* 0x28 */
            s16 kiraSysParam; /* 0x2c */
            u16 heading;      /* 0x2e - angle stepped toward the target */
            u16 delay;        /* 0x30 - frames before the state handler runs */
            u16 frameTimer;   /* 0x32 - 39ec advances frame every 4 frames */
            u16 kiraTimer;    /* 0x34 - 5894 counts it to 0x1e */
            u16 flyTimer;     /* 0x36 - frames since the fly-off began */
            u16 pad_38;       /* 0x38 - 5d34 clears it; nothing reads it */
            u16 frame;        /* 0x3a - index into data_ov002_021002a0 */
            /* Counts the frames on which frame and frameTimer are both 0
               (40fc, 4d70); the re-arm loops clear it and nothing else
               reads it. Medium confidence. */
            u16 loops;       /* 0x3c */
            u16 kiraPeriods; /* 0x3e - kiraTimer wraps; kiraSysParam grows from 5 */
            u8 pad_40[2];    /* 0x40 */
            u16 angSpeed;    /* 0x42 - added to heading while orbiting */
            u8 active;       /* 0x44 - set by 5d34, cleared by 5a6c */
            u8 visible;      /* 0x45 - 3978 draws the star while set */
            u8 state;        /* 0x46 - index into data_ov002_02110e7c */
            u8 step;         /* 0x47 - step index the state handlers dispatch on */
            u8 phase;        /* 0x48 - 0 approaching, then orbiting or flying off */
            u8 sparkling;    /* 0x49 - 5990 spawns its sparkle while set */
            u8 holdFrame;    /* 0x4a - 39ec stops frame at 0x10 instead of wrapping at 0x20 */
            u8 pad_4b;       /* 0x4b */
        };

        /* The sub-screen window over the cake: its BG1/BG2 scroll and the
           window-1 rectangle written to 0x4001042 (x1 << 8 | x2) and
           0x4001046 (y1 << 8 | y2). Indexed by the handlers' int
           argument, which is always 0. */
        struct Window {
            s32 bgX;      /* 0x00 - fix12, SetSubBg1Offset/SetSubBg2Offset */
            s32 bgY;      /* 0x04 */
            u8 pad_08[8]; /* 0x08 */
            s32 left;     /* 0x10 - fix12 */
            s32 top;      /* 0x14 */
            s32 right;    /* 0x18 */
            s32 bottom;   /* 0x1c */
            s32 stepX;    /* 0x20 - 30f8 widens the window by it per frame */
            s32 stepY;    /* 0x24 */
            u16 delay;    /* 0x28 - 30f8 waits it out first */
            u8 pad_2a[2]; /* 0x2a */
            u8 active;    /* 0x2c - func_ov002_020f37a0 does nothing while 0 */
            u8 kind;      /* 0x2d - index into data_ov002_02110e4c */
            /* step index func_ov002_020f3310 and func_ov002_020f3740
               dispatch on; the step handlers advance it. */
            u8 step;   /* 0x2e */
            u8 pad_2f; /* 0x2f */
        };

        /* One of the five sub-screen sprites placed on the cake
           (func_ov002_020f2f9c, one per unlocked character plus the fixed
           ones); 2f18 moves them with the window scroll. */
        struct Doll {
            s32 x;        /* 0x00 - fix12 */
            s32 y;        /* 0x04 */
            u8 pad_08[8]; /* 0x08 */
            u8 active;    /* 0x10 - 2f18 moves and 2f64 animates it while set */
            u8 visible;   /* 0x11 - 2eac draws it while set */
            u8 frame;     /* 0x12 - index into data_ov002_0210bba0 */
            u8 priority;  /* 0x13 - OAM::RenderSub priority */
        };

        /* The credit text on sub BG0: func_0201ef50 shows message
           `message`, and the step handlers fade it with the BG0 blend at
           0x4001050. */
        struct Credit {
            u16 timer;      /* 0x00 */
            u16 blendTimer; /* 0x02 - blend steps every 2 frames */
            u8 active;      /* 0x04 - func_ov002_020f2dd4 does nothing while 0 */
            u8 step;        /* 0x05 - index into data_ov002_02110ebc */
            u8 blend;       /* 0x06 - blend weight, 0..0x10 */
            u8 message;     /* 0x07 - func_0201ef50's argument */
        };

        Star stars[4];    /* 0x000 */
        Window window[1]; /* 0x130 */
        Doll dolls[5];    /* 0x160 */
        Credit credit[1]; /* 0x1c4 */
        /* The language's "The End" sprite (data_ov002_0210b988), drawn by
           2990 while endVisible is set. */
        s32 endX;      /* 0x1cc - fix12 */
        s32 endY;      /* 0x1d0 */
        u8 pad_1d4[4]; /* 0x1d4 */
        /* Written with endVisible by 2a48 and 2a68; nothing reads it.
           Medium confidence. */
        u8 endActive;   /* 0x1d8 */
        u8 endVisible;  /* 0x1d9 */
        u8 pad_1da[10]; /* 0x1da */
        /* The sparkle burst 27e8 emits twice a frame at (0x80, 0x60): the
           same easing as a star's kira fields. */
        s32 burstDefParam; /* 0x1e4 */
        s16 burstSysParam; /* 0x1e8 */
        u16 burstPeriods;  /* 0x1ea */
        u8 burstTimer;     /* 0x1ec - wraps at 0x1e */
        u8 burstActive;    /* 0x1ed */
        u8 pad_1ee[2];     /* 0x1ee */
        u8 pad_1f0[8];     /* 0x1f0 - 5ad4 clears both words; nothing reads them */
        s32 kiraStar;      /* 0x1f8 - the star whose sparkle 5990 spawns this frame, 0..3 */
        u32 kiraParticle;  /* 0x1fc - Particle::System::New handle */
        dPa_c::level_c::edStarKiraCallback_c kira; /* 0x200 - the sparkle callback */
        u8 *starChars; /* 0x510 - the 0x4000-byte D_2D_ED_STAR characters */
        /* Set by 5b8c; while set an orbiting star does not start its
           fly-off. */
        u8 holdOrbit;  /* 0x514 */
        u8 pad_515[3]; /* 0x515 */
    };

    u8 pad_0d0[4];          /* 0x0d0 */
    ScrollState *mScroll;   /* 0x0d4 - param1 0x2f only */
    StaffRoll *mStaffRoll;  /* 0x0d8 - param1 0x2e only */
    dExtSimpleModel_c *mModel;  /* 0x0dc */
    dExtAnmModel_c *mAnmModel;  /* 0x0e0 */
    u32 mSoundHandle;       /* 0x0e4 - Sound::PlayLong handle, passed back on every call */
    u32 mParticleID[3];     /* 0x0e8 - Particle::System::New handles, passed back likewise */
    /* A fix12 magnitude each script handler sets or eases, then scales by:
       the speed along the model's facing (6c60, 6e48, 6f48) or along
       mPrevAngleX (7780), the orbit radius (79c0), the opacity level that
       mOpacity takes as mMagnitude >> 12 (71c4, 7410, 7538), and the
       segment progress func_02008b4c advances (7bb8). Medium confidence:
       no single role covers every reader. */
    s32 mMagnitude; /* 0x0f4 */
    /* A fix12 rate eased toward a target: the sink speed 7038 subtracts
       from mPosY (scaled by 0x199), the x drift 79c0 adds to mPosX, and
       the 0..0x1000 blend weight 7538 interpolates two vectors by. */
    s32 mRate; /* 0x0f8 */
    /* The handler's phase: the path segment func_02008b4c walks (7bb8),
       the frame count 7780 compares against 0x3c/0x6a/0xa9, and the sine
       angle 79c0 bobs mPosY by. */
    s16 mPhase;     /* 0x0fc */
    s16 mPhaseStep; /* 0x0fe - 79c0 adds it to mPhase each frame and eases it toward 0x200 */
    /* Script cue: handlers 20, 21 and 23 copy it from the record's first
       byte. 23f0 waits for cues 3 and 4; 63f8 and 6424 index the staff-roll
       block's dispatch tables with it. */
    u8 mCue;                /* 0x100 */
    u8 pad_101;             /* 0x101 */
    u8 mOpacity;            /* 0x102 */
    /* Bit 0: Behavior lifted the actor back onto the floor (mSnapToFloor);
       7038 switches the animation to entry 1 once it is set. */
    u8 mGroundFlags; /* 0x103 */

    /* Keep the destructor first: it is the class's key function and remains
       the translation-unit owner selected by the ROM's lifecycle symbols. */
    virtual ~daDemo_c();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
    virtual void OnPendingDestroy();

    void func_ov002_020f1fcc();
    void func_ov002_020f20f4();
    void func_ov002_020f2210();
    void func_ov002_020f2340();
    void func_ov002_020f237c();
    int func_ov002_020f23d0();
    int func_ov002_020f23f0();
    int func_ov002_020f26c4(unsigned char *src);
    int func_ov002_020f63a0();
    int func_ov002_020f63d4();
    int func_ov002_020f63f8(unsigned char *src);
    int func_ov002_020f6424();
    int func_ov002_020f6448(unsigned char *arg1);
    int func_ov002_020f6a9c();
    int func_ov002_020f6ab8(unsigned char *p);
    int func_ov002_020f6ae4(unsigned char *p);
    int func_ov002_020f6b28(unsigned char *p);
    int func_ov002_020f6b4c(unsigned char *p);
    s32 func_ov002_020f6bc0(unsigned char *data);
    int func_ov002_020f6c24(unsigned char *src);
    int func_ov002_020f6c34(unsigned char *src);
    int func_ov002_020f6c60(void *arg1, int arg2, int arg3);
    int func_ov002_020f6e48(unsigned char *p);
    void func_ov002_020f6f48(Vector3 *v, int amt);
    int func_ov002_020f7020();
    int func_ov002_020f7038(int a, int arg);
    int func_ov002_020f71c4(int a, int arg);
    int func_ov002_020f72bc();
    int func_ov002_020f7384(unsigned char *flag, int val);
    int func_ov002_020f7410(unsigned char *in, int sel);
    int func_ov002_020f7538(unsigned char *arg1, int arg2);
    int func_ov002_020f7780(void *unused, int mode);
    int func_ov002_020f79c0(void *unused, int mode);
    int func_ov002_020f7bb8(unsigned char *p, u16 id);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daDemo_c_size_must_be_0x104[
    sizeof(daDemo_c) == 0x104 ? 1 : -1];
typedef char daDemo_c_anmModel_c_size_must_be_0x84[sizeof(daDemo_c::anmModel_c) == 0x84 ? 1 : -1];
typedef char
    daDemo_c_simpleModel_c_size_must_be_0x60[sizeof(daDemo_c::simpleModel_c) == 0x60 ? 1 : -1];
typedef char daDemo_c_ScrollState_size_must_be_0x36[
    sizeof(daDemo_c::ScrollState) == 0x36 ? 1 : -1];
typedef char daDemo_c_StaffRoll_size_must_be_0x518[sizeof(daDemo_c::StaffRoll) == 0x518 ? 1 : -1];
typedef char
    daDemo_c_StaffRoll_Star_size_must_be_0x4c[sizeof(daDemo_c::StaffRoll::Star) == 0x4c ? 1 : -1];
typedef char daDemo_c_StaffRoll_Window_size_must_be_0x30[sizeof(daDemo_c::StaffRoll::Window) == 0x30
                                                             ? 1
                                                             : -1];
typedef char
    daDemo_c_StaffRoll_Doll_size_must_be_0x14[sizeof(daDemo_c::StaffRoll::Doll) == 0x14 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DADEMO_C_H */
