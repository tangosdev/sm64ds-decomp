#ifndef DMGJUMP3DMARIO_C_H
#define DMGJUMP3DMARIO_C_H

#include "ModelAnim.h"

/* Both class names and the inheritance edge are carried by ov006 RTTI:
 *
 *   dMgJump3DMario_c : dMg3DHeyhoObjAdapter_c
 *
 * The base constructor fixes its 0x4c-byte extent: the derived constructor
 * calls it, installs _ZTV16dMgJump3DMario_c, then constructs ModelAnim at
 * +0x4c. The array helpers in both jump minigames fix sizeof the derived
 * object at 0xb8.
 *
 * The three virtual names remain address-based because RTTI and the vtable
 * prove their ownership and order, but not their original source spelling.
 */
struct dMg3DHeyhoObjAdapter_c {
    dMg3DHeyhoObjAdapter_c();
    ~dMg3DHeyhoObjAdapter_c() {}

    virtual void *Unk_020c76d8() = 0; /* slot 0 */
    virtual void *Unk_020c76d0() = 0; /* slot 1 */
    virtual int Unk_020c762c() = 0;   /* slot 2 */

    u8 unk_004[0x0c]; /* 0x04 */
    s16 unk_010;      /* 0x10 */
    u8 unk_012[0x3a]; /* 0x12 */
};

struct dMgJump3DMario_c : dMg3DHeyhoObjAdapter_c {
    dMgJump3DMario_c();
    ~dMgJump3DMario_c();

    virtual void *Unk_020c76d8(); /* slot 0 */
    virtual void *Unk_020c76d0(); /* slot 1 */
    virtual int Unk_020c762c();   /* slot 2 */

    ModelAnim mModelAnim; /* 0x4c */
    u8 unk_0b0[0x08];     /* 0xb0 */
};

typedef char dMg3DHeyhoObjAdapter_c_size_must_be_0x4c[
    sizeof(dMg3DHeyhoObjAdapter_c) == 0x4c ? 1 : -1];
typedef char dMgJump3DMario_c_size_must_be_0xb8[
    sizeof(dMgJump3DMario_c) == 0xb8 ? 1 : -1];

#endif
