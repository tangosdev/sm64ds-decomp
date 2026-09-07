#ifndef DSCMGPANEL_C_H
#define DSCMGPANEL_C_H
#include "dScMgBase_c.h"

/* dScMgPanel_c : dScMgBase_c -- the Puzzle Panic minigame scene, MG_PANEL in
 * the profile registry.  A confirmed leaf: no RTTI record in the cartridge
 * names it as a base (tools/rtti_extract.py).
 *
 * Six vtable slots are its own: 0 InitResources, 6 Behavior, 9 Render, the
 * 16/17 destructor pair, and 18 OnYoshiTryEat(int), an override of
 * dScMgBase_c's.  Everything below 0x4660 belongs to dScMgBase_c and is
 * reached through a char* view of `this`, the same way every other
 * dScMgBase_c leaf reaches it.
 *
 * The field names below are READINGS of what the code does, not recovered
 * identifiers -- the offsets, widths and array counts are ROM facts, the
 * English is not.  Fields the non-virtual helpers touch but the six slots do
 * not are left inside the padding runs rather than guessed at.
 *
 * The factory dScMgPanel_c_classInit is a reconstruction (historical alias
 * MgPuzzlePanelPuzzlePanic_Spawn); it installs this vtable for MG_PANEL.
 * Only the CLASS name is a cartridge string.
 */
struct dScMgPanel_c : dScMgBase_c {
    virtual ~dScMgPanel_c();
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */

    u8  pad_4660[0x648];
    s32 unk_4ca8;            /* 0x4ca8 */
    s32 unk_4cac;            /* 0x4cac */
    s32 unk_4cb0;            /* 0x4cb0 */
    s32 unk_4cb4;            /* 0x4cb4 */
    s32 unk_4cb8;            /* 0x4cb8 -- panel count (<= 0x24) */
    s32 unk_4cbc;            /* 0x4cbc -- layout index + 4 */
    s32 unk_4cc0;            /* 0x4cc0 -- pick count (<= 0x30) */
    s32 unk_4cc4[0x24];      /* 0x4cc4 -- panel x, Fix12, from the layout table */
    s32 unk_4d54[0x24];      /* 0x4d54 -- panel y, Fix12, from the layout table */
    u8  pad_4de4[0xe0];
    s16 unk_4ec4;            /* 0x4ec4 */
    u8  pad_4ec6[0x34];
    u8  unk_4efa[0x24];      /* 0x4efa -- panel kind, indexed below unk_4cb8 */
    u8  unk_4f1e[0x24];      /* 0x4f1e -- panel face, from the face table */
    u8  unk_4f42[0x24];      /* 0x4f42 -- panel face copy, flipped by picks */
    u8  pad_4f66[0x48];
    u8  unk_4fae[0x30];      /* 0x4fae -- picked panel indices */
    u8  unk_4fde;            /* 0x4fde -- number of picks so far */
    u8  unk_4fdf;            /* 0x4fdf */
    u8  pad_4fe0[0x2];
    u8  unk_4fe2;            /* 0x4fe2 */
    u8  pad_4fe3[0x6];
    u8  unk_4fe9;            /* 0x4fe9 */
    u8  unk_4fea;            /* 0x4fea */
};

typedef char dScMgPanel_c_size_must_be_0x4fec[sizeof(dScMgPanel_c) == 0x4fec ? 1 : -1];

#endif
