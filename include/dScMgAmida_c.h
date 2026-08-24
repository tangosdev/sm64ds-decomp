#ifndef DSCMGAMIDA_C_H
#define DSCMGAMIDA_C_H
#include "dScMgBase_c.h"

/* The destructor tears down four arrays with __destroy_arr; both
   per-element destructors are no-ops. */
extern "C" void __destroy_arr(void *arr, int count, int elemSize, void *dtor);
extern "C" void func_ov006_020d116c(void);
extern "C" void NullDestructor_0203d47c(void);

/* One of the 0x80 falling pieces. Behavior adds vel into pos once a tick
   and counts timer up until active clears; Render draws pos >> 12. */
struct dScMgAmida_c_Piece {
    s32 posX;    /* +0x00 */
    s32 posY;    /* +0x04 */
    s32 velX;    /* +0x08 */
    s32 velY;    /* +0x0c */
    s32 timer;   /* +0x10 */
    u8  active;  /* +0x14 */
};
typedef char dScMgAmida_c_Piece_size_must_be_0x18[sizeof(dScMgAmida_c_Piece) == 0x18 ? 1 : -1];

/* The Amida (ghost-leg lottery) minigame scene -- a dScMgBase_c leaf.
   Field evidence and the full vtable census live in
   notes/minigame-provenance.md.

   TWO CONSTRAINTS HERE ARE MEASURED, NOT STYLE:

   1. Unk36 (slot 36) is declared below as a real virtual, but its three
      call sites in InitResources/Behavior/Render MUST keep the
      pre-migration vtable-shim dispatch. Calling it as a plain
      `this->Unk36()` compiles Render 0xc bytes larger (0x2ac vs 0x2a0),
      because mwcc lands its own slot for Unk36 right after dScMgBase_c's
      compiler-visible virtuals -- dScMgBase_c leaves slots 18-35
      undeclared -- not on true ROM slot 36. That one delta cascaded into
      ~1400 unrelated-looking mismatches; traced via final_link.o.xMAP.

   2. AfterCleanupResources returns early when vfSuccess != 2, which skips
      the base-class call as well. That is what the ROM does. Keep it.

   Slot 35 (func_ov006_020d1170) is deliberately not declared, renamed or
   touched: nothing in this class calls it. */
struct dScMgAmida_c : dScMgBase_c {
    virtual ~dScMgAmida_c();
    virtual s32  InitResources();                       /* slot  0 */
    virtual void AfterCleanupResources(u32 vfSuccess);   /* slot  5 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual int  Unk36();                                /* slot 36 */

    u8  pad_465d[0x3];
    u8  unk_4660[4][8];      /* 0x4660 -- only ever passed around whole */
    u8  pad_4680[0x50];
    s32 unk_46d0;             /* 0x46d0 -- Behavior's state switch */
    u8  unk_46d4;             /* 0x46d4 */
    u8  unk_46d5;             /* 0x46d5 */
    u8  pad_46d6[0x2];
    u8  pad_46d8[0x28];       /* 0x46d8 -- slot 34's digit-drawing state */
    s32 unk_4700;             /* 0x4700 */
    u8  pad_4704[0x8];        /* 0x4704 -- slot 34's flag bytes */
    u8 *unk_470c;             /* 0x470c -- 0x15800-byte buffer */
    u8 *unk_4710;             /* 0x4710 -- 0x15800-byte buffer */
    s32 unk_4714[4];          /* 0x4714 */
    s32 arr4724[4][2];        /* 0x4724 -- {x,y} pairs */
    s32 arr4744[4][2];        /* 0x4744 -- {dx,dy} added into arr4724 each tick */
    s32 unk_4764;             /* 0x4764 */
    dScMgAmida_c_Piece arr4768[0x80]; /* 0x4768 */
    s32 unk_5368;              /* 0x5368 */
    s32 unk_536c;              /* 0x536c */
    u8  pad_5370[0x4];
    s32 unk_5374;              /* 0x5374 */
    u8  pad_5378[0x24];
    s32 unk_539c[4];           /* 0x539c -- Render-only counters */
    s32 unk_53ac[4];           /* 0x53ac -- Render-only counters */
    u16 unk_53bc;              /* 0x53bc -- Render's BG2 scroll-offset index */
    u8  pad_53be[0x2];
    s32 unk_53c0;               /* 0x53c0 */
    s32 unk_53c4;               /* 0x53c4 */
    u8  pad_53c8[0x8];
    s32 unk_53d0;               /* 0x53d0 */
    s32 unk_53d4;               /* 0x53d4 */
    u8  pad_53d8[0x4];
    u8  unk_53dc;                /* 0x53dc */
    u8  unk_53dd;                /* 0x53dd */
    u8  pad_53de[0x2];
    s32 unk_53e0;                /* 0x53e0 */
    s32 unk_53e4;                /* 0x53e4 */
    s32 unk_53e8;                /* 0x53e8 */
    u8  pad_53ec[0x10];          /* tail padding to the 0x53fc allocation */
};

typedef char dScMgAmida_c_size_must_be_0x53fc[sizeof(dScMgAmida_c) == 0x53fc ? 1 : -1];

#endif
