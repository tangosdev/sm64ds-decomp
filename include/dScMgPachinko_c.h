#ifndef DSCMGPACHINKO_C_H
#define DSCMGPACHINKO_C_H
#include "dScMgBase_c.h"

/* dScMgPachinko_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper). rtti_vtables.py --own also
   reports slots 36/38, but both are spurious: the true vtable ends at 36
   words (address-gap math against the neighbouring g_profile_MG_TAMAIRE
   table, and relocs.txt cross-references confirm it belongs to a wholly
   different actor's spawn descriptor, not this class) -- excluded, along
   with Paddle_StateOrbit and dScMgPachinko2_c_classInit, from this migration.
   Fields below dScMgBase_c's own 0x4660 are INHERITED, not this class's
   own -- accessed via raw offsets on a char* cast of `this` (0xa4, 0xc0,
   0xc3, 0xc4 all touched here, all already dScMgBase_c's own, same as
   dScMgPachinko2_c's).
 *
 * SM64DS RTTI names the implementation dScMgPachinko_c. The reconstructed factory
 * dScMgPachinko_c_classInit (historical alias MgBobOmbSquad_Spawn) installs this class's
 * cartridge vtable for the MG_PACHINKO registry profile.
 */
/* One pachinko ball on the board, 0x38 bytes, thirty of them at 0x4660. */
struct dScMgPachinko_ball {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 unk08;          /* 0x08 */
    s32 unk0c;          /* 0x0c */
    s32 unk10;          /* 0x10 */
    s32 unk14;          /* 0x14 */
    s32 unk18;          /* 0x18 */
    s32 unk1c;          /* 0x1c */
    s32 unk20;          /* 0x20 */
    u16 unk24;          /* 0x24 */
    u16 angle;          /* 0x26 */
    u16 timer;          /* 0x28 seeded with 16 when a hit forces state 6;
                           only ever written here, so the name is inferred */
    u8  unk2a[0x2];     /* 0x2a */
    u8  active;         /* 0x2c */
    u8  unk2d;          /* 0x2d */
    u8  unk2e;          /* 0x2e */
    u8  state;          /* 0x2f 0 rests; a hit below 5 jumps to 6 */
    u8  unk30;          /* 0x30 */
    u8  unk31[0x2];     /* 0x31 */
    u8  unk33;          /* 0x33 */
    u8  unk34[0x2];     /* 0x34 */
    u8  unk36;          /* 0x36 */
    u8  unk37;          /* 0x37 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_ball_size_must_be_0x38[sizeof(struct dScMgPachinko_ball) == 0x38 ? 1 : -1];
#endif

/* One ball being aimed with the stylus, 0x38 bytes, at 0x4ed8. */
struct dScMgPachinko_shot {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    s32 velX;           /* 0x08 */
    s32 velY;           /* 0x0c */
    s32 offX;           /* 0x10, grab offset from the stylus */
    s32 offY;           /* 0x14 */
    s32 prevX;          /* 0x18 x the aim line was last drawn from */
    s32 prevY;          /* 0x1c */
    s32 speed;          /* 0x20 */
    s32 unk24;          /* 0x24 */
    s32 sound;          /* 0x28, sound handle */
    s32 dist;           /* 0x2c, last frame's distance from the launcher */
    u16 angle;          /* 0x30 */
    u16 timer;          /* 0x32 */
    u8  active;         /* 0x34 clear: this shot is gone */
    u8  state;          /* 0x35 at least 2 before it can hit; 2 sits on the launcher */
    u8  unk36;          /* 0x36 cleared with active on a hit; not read here */
    u8  unk37;          /* 0x37 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_shot_size_must_be_0x38[sizeof(struct dScMgPachinko_shot) == 0x38 ? 1 : -1];
#endif

/* A score popup: 30 of them at 0x4cf0 (PachinkoPopup_SpawnScore). */
struct dScMgPachinko_popup {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u16 timer;          /* 0x08 */
    u16 value;          /* 0x0a */
};

/* One of the three swinging rows at 0x4e58 (PachinkoDeco_UpdateAll swings
   angle between -0x1000 and 0x1000; dir picks the way). */
struct dScMgPachinko_row {
    s32 x;              /* 0x00 */
    s32 y;              /* 0x04 */
    u8  pad08[0x8];
    s16 angle;          /* 0x10 */
    s16 unk12;          /* 0x12 */
    u8  active;         /* 0x14 */
    u8  visible;        /* 0x15 */
    u8  dir;            /* 0x16 */
    u8  pad17;
};

/* The two pipes at 0x4ea0. */
struct dScMgPachinko_pipe {
    s32 acc;            /* 0x00 */
    u8  pad04[0x4];
    s32 vel;            /* 0x08 */
    u8  pad0c[0x4];
    u16 timer;          /* 0x10 */
    u8  pad12;
    u8  out2;           /* 0x13 */
    u8  state;          /* 0x14 */
    u8  out;            /* 0x15 */
    u8  frame;          /* 0x16 */
    u8  unk17;          /* 0x17 */
    u8  unk18;          /* 0x18 */
    u8  pad19;
    u8  laps;           /* 0x1a */
    u8  pad1b;
};

/* A hit effect: the burst sprite PachinkoFx_Spawn starts at a hit, worth
   `points` once its animation ends. */
struct dScMgPachinko_fx {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    u16 timer;      /* 0x08 -- frames on the current frame; steps at 6 */
    u16 points;     /* 0x0a -- popup score spawned when it ends; 0 for none */
    u8  active;     /* 0x0c */
    u8  frame;      /* 0x0d */
    u8  visible;    /* 0x0e */
    u8  kind;       /* 0x0f -- 2 uses the long (six-frame) animation */
    u8  target;     /* 0x10 */
    u8  pad11[0x3];
};

/* The flyer that crosses the top of the board: Lakitu on his cloud (seen in
   melonDS). Its state handlers dispatch on `state`, guarded by `active`
   (PachinkoFlyer_StateWaiting..). */
struct dScMgPachinko_flyer {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    s32 vx;         /* 0x08 -- 20.12 */
    s32 vy;         /* 0x0c -- 20.12; StateWaiting seeds 0x2000, StateDescend adds it to y */
    s32 sound;      /* 0x10 -- looping sound handle */
    u16 delay;      /* 0x14 */
    u8  active;     /* 0x16 */
    u8  visible;    /* 0x17 */
    u8  state;      /* 0x18 */
    u8  facing;     /* 0x19 -- flips at each edge; picks the sprite */
    u8  passes;     /* 0x1a -- crossings left */
    u8  pad1b;
};

/* One of the four targets (PachinkoTarget_ShowAll / HideAll): the four
   flowers along the bottom (seen in melonDS; y 0xa0000, x from a table). */
struct dScMgPachinko_target {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    u8  active;     /* 0x08 */
    u8  visible;    /* 0x09 */
    u8  sprite;     /* 0x0a -- index into the target sprite table */
    u8  pad0b;
};

/* The scrolling background (PachinkoBg_StateWaiting..), dispatched on `state`. */
struct dScMgPachinko_bg {
    s32 x;          /* 0x00 -- 20.12 */
    s32 y;          /* 0x04 -- 20.12 */
    s32 vx;         /* 0x08 */
    s32 vy;         /* 0x0c */
    s16 timer;      /* 0x10 */
    u8  active;     /* 0x12 */
    u8  state;      /* 0x13 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char dScMgPachinko_popup_size_must_be_0xc[sizeof(dScMgPachinko_popup) == 0xc ? 1 : -1];
typedef char dScMgPachinko_row_size_must_be_0x18[sizeof(dScMgPachinko_row) == 0x18 ? 1 : -1];
typedef char dScMgPachinko_pipe_size_must_be_0x1c[sizeof(dScMgPachinko_pipe) == 0x1c ? 1 : -1];
typedef char dScMgPachinko_fx_size_must_be_0x14[sizeof(dScMgPachinko_fx) == 0x14 ? 1 : -1];
typedef char dScMgPachinko_flyer_size_must_be_0x1c[sizeof(dScMgPachinko_flyer) == 0x1c ? 1 : -1];
typedef char dScMgPachinko_target_size_must_be_0xc[sizeof(dScMgPachinko_target) == 0xc ? 1 : -1];
typedef char dScMgPachinko_bg_size_must_be_0x14[sizeof(dScMgPachinko_bg) == 0x14 ? 1 : -1];
#endif

struct dScMgPachinko_c : dScMgBase_c {
    virtual ~dScMgPachinko_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void StartRound(int arg);               /* slot 18 */

    dScMgPachinko_ball mBall[30];    /* 0x4660, stride 0x38, ends 0x4cf0 */
    dScMgPachinko_popup mPopup[30];  /* 0x4cf0 */
    dScMgPachinko_row   mRow[3];     /* 0x4e58 */
    dScMgPachinko_pipe  mPipe[2];    /* 0x4ea0 */
    dScMgPachinko_shot mShot[24];    /* 0x4ed8, stride 0x38; 24 puts the
                                        siblings' own 0x5418 field exactly
                                        one record past the end */
    /* 0x5418..0x5958: no reconstructed function reads or writes it. */
    u8  pad_5418[0x540];
    dScMgPachinko_fx     mFx[30];     /* 0x5958 */
    /* mFlyer and mBg are one-element arrays: their state handlers take an
       index and step it (i * 0x1c, i * 0x14) the way the array handlers do,
       and both dispatchers pass 0. [inference from codegen] */
    dScMgPachinko_flyer  mFlyer[1];   /* 0x5bb0 */
    dScMgPachinko_target mTarget[4];  /* 0x5bcc */
    dScMgPachinko_bg     mBg[1];      /* 0x5bfc */
    s32 mPhase;                       /* 0x5c10 -- Behavior: 0 setup, 1 play, 2 results wait */
    u8 pad_5c14[0x4];                 /* 0x5c14 */
    u16 mEndTimer;                    /* 0x5c18 -- 0x40 frames from time up to the results */
    u8 pad_5c1a[0x2];                 /* 0x5c1a */
    u16 mNextBallDelay; /* 0x5c1c -- frames to the next penned Bob-omb (throw speed >> 10) */
    u8 pad_5c1e[0x6];   /* 0x5c1e */
    u16 mWaveTimer;     /* 0x5c24 -- frames to the next spawn; 0x50 - 4 per level, >= 0x20 */
    u16 mWaveSpawns;    /* 0x5c26 -- enemies spawned; every tenth raises mWaveLevel */
    u16 mWaveLevel;     /* 0x5c28 -- 0..0x27: enemy speed, spawn rate and wave size */
    u16 mHitPause;      /* 0x5c2a -- 0x20 frames of only effects after a shot hits a flyer */
    /* trailing extent the ROM's `new dScMgPachinko_c` literal proves; see tools/opnew_sizes.py */
    u8 pad_5c2c[0x5];
    u8 unk_5c31; /* 0x5c31 */
    u8  pad_5c32[0x6];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko_c_size_must_be_0x5c38[sizeof(struct dScMgPachinko_c) == 0x5c38 ? 1 : -1];
#endif

#endif
