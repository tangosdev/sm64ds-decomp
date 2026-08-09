//cpp
// @symbol _ZN7Minimap8BehaviorEv
/* recovered: named members + shared header, real C++ method, shadow struct removed
 *
 * One frame of the minimap. This file's whole point is that it no longer
 * carries its own idea of what a Minimap is: the pre-image declared a private
 * `struct Minimap` describing the object in full, and every field access went
 * through it. That shadow is gone and the shared header serves instead.
 *
 * The shadow was RICHER than Minimap.h, which is why it existed. It declared
 * twelve ranges as ARRAYS and indexed them -- 0x070, 0x080, 0x0a0, 0x0d0,
 * 0x100, 0x124, 0x180, 0x1a0, 0x21e, 0x222, 0x23a, 0x249 -- where the header
 * had flat padding, so the header simply could not express what this function
 * does. Those arrays are in the header now and the reconstruction is
 * offset-neutral: the struct still spans 0x256.
 *
 * Two disagreements between the two views, both settled toward the header:
 *   0x1e0 and 0x1f4  the shadow called each a Vector3; only two sites need
 *                    that, and they take `(Vector3*)&unk_1f4` rather than the
 *                    header asserting a type the other eight matched
 *                    functions never see.
 *   0x21c            the shadow said s16 and then cast EVERY read to (u16).
 *                    The header's u16 says the same thing without the casts.
 */
#include "Minimap.h"
typedef int Fix12;

struct Obj {
    char pad0[0x5c];
    Vector3 pos;        /* 0x5c */
    char pad1[0xcc - 0x68];
    s8 f0cc;            /* 0xcc */
    char pad2[0x110 - 0xcd];
    void *f110;         /* 0x110 */
    char pad3[0x154 - 0x114];
    s32 f154;           /* 0x154 */
    char pad4[0x17c - 0x158];
    s16 f17c;           /* 0x17c */
};


struct VtblOwner;
struct Vtbl { s32 (*f[8])(void *); };
struct VtblOwner { Vtbl *vt; };

extern "C" {
extern VtblOwner *data_0209f5bc;
extern u8   data_0209f250;
extern Obj  *data_0209f318;
extern Obj  *data_0209f394[];
extern u32  data_0209b454;
extern s32  data_0209d4b0;
extern u8   data_0209f350[];
extern s32  data_0208ee44;
extern u8   data_0209f2c4;
extern u8   data_0209f20c;
extern u8   data_0209f294;
extern s8   data_02092110;
extern u8   data_0209f204;
extern u8   data_ov002_02111150;
extern u8   data_0209d660;
extern u8   data_020a0e40;
extern u8   data_0209f4ac[];
extern u8   data_0209d454;
extern u8   data_0209f4a8[];
extern u8   data_0209f4a9[];
extern u8   data_0209f4ae[];
extern u8   data_0209f2d8;
extern u32  data_0209caa0[];
extern s8   data_0209f2f8;
extern u8   data_ov002_0211114c;
extern u8   data_ov002_02111144;
extern s16  data_02082214[];
extern u8   data_0209f37c[];
extern Obj  *data_0209f40c[];
extern Obj  *data_0209f3e8[];
extern Obj  *data_0209f33c;
extern Obj  *data_0209f3a4[];
extern s8   data_ov002_02111148;

extern s32  _ZN6Player12Unk_020ca8f8Ev(Obj *p);
extern void SetSubBg2Offset(s32 a, s32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern s32  SublevelToLevel(s32 i);
extern Fix12 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(Vector3 *v);
extern void _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(Vector3 *a, Vector3 *b, Fix12 c, s16 d, Vector3 *e);
extern void _ZN7Minimap20GetPosFromMinimapPosER7Vector3S1_5Fix12IiEsS1_(Vector3 *a, Vector3 *b, Fix12 c, s16 d, Vector3 *e);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern s32  GetMinimapID(Obj *obj, s32 arg);
extern s32  GetMinimapScale(s32 idx);
extern void UpdateMinimap(s32 *a, s32 b, s32 c, s32 d, s32 e);
}

#define F218 (*(s32 *)(((int)self + 0x218)))
#define FANG (*(s16 *)(((int)self + 0x21c)))
#define F254 (*(u8 *)(((int)self + 0x254)))
#define FMUL(a, b) ((s32)((((long long)(a) * (b)) + 0x800) >> 12))


s32 Minimap::Behavior()
{
    Minimap *self = this;
    Obj *obj;
    Vector3 v8, v14, v20, v2c, v38;
    Vector3 *op;
    Obj *player;
    Obj *cam;
    u32 orv;
    s32 i;

    cam = data_0209f318;
    player = data_0209f394[data_0209f250];
    obj = 0;

    if (data_0209f5bc->vt->f[5](data_0209f5bc) == 0) goto L274;
    if (data_0209b454 & 0x40000000) goto L274;
    if (data_0209d4b0 != 0) goto L274;
    if (data_0209f350[data_0209f250] != 0) goto L274;
    if (_ZN6Player12Unk_020ca8f8Ev(player) == 1) goto L274;

    if (self->unk_254 != 0) {
        F254 -= data_0208ee44;
    }

    orv = data_0209f2c4 | data_0209f20c | data_0209f294;
    if ((u8)orv) goto L200;
    if (data_02092110 >= 0) goto L200;
    if (data_0209f204 != 0) goto L200;
    if (data_ov002_02111150 != 0) goto L200;
    if (data_0209d660 != 0) goto L200;
    {
        u8 v = data_0209f4ac[data_020a0e40 * 0x18];
        if (v == 0 && self->unk_254 == 0) goto L200;
        data_0209d454 |= 4;
        if (v != 0) {
            self->unk_254 = 0x1e;
            SetSubBg2Offset(0x100 - data_0209f4a8[data_020a0e40 * 0x18],
                            0x80 - data_0209f4a9[data_020a0e40 * 0x18]);
        }
        _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16 *)0x4001050, 4, 0x28, 4, 0xd);
        if (data_0209f4ae[data_020a0e40 * 0x18] != 2)
            *(volatile u16 *)0x400100c = (u16)((*(volatile u16 *)0x400100c & 0x43) | 0x5300);
        else
            *(volatile u16 *)0x400100c = (u16)((*(volatile u16 *)0x400100c & 0x43) | 0x5500);
        goto L2a4;
    }

L200:
    {
        s32 b = (data_0209f2d8 == 1);
        if (b == 0) {
            if ((data_0209caa0[2] & 0x80) == 0) goto L2a4;
        }
        data_0209d454 &= ~4;
        if (!(u8)orv) {
            *(volatile s32 *)0x4001000 = (*(volatile s32 *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
            *(volatile u16 *)0x4001050 = 0;
        }
        self->unk_254 = 0;
        goto L2a4;
    }

L274:
    if ((data_0209caa0[2] & 0x80) == 0) {
        if (data_0209d4b0 == 0) goto L2a4;
    }
    data_0209d454 &= ~4;

L2a4:
    if (cam == 0) goto L3e8;
    if (SublevelToLevel(data_0209f2f8) != 0x1d) goto A1;
    if (data_0209f2f8 == 1) goto A1;
    if (data_0209f2f8 == 0x33) goto A1;
    if (data_0209f2f8 != 3) goto L350;
A1:
    if (SublevelToLevel(data_0209f2f8) == 4) goto L350;
    if (SublevelToLevel(data_0209f2f8) != 0x13) goto L318;
    if (data_0209f2f8 == 0x2e) goto L350;
L318:
    {
        s32 b = (data_0209f2d8 == 0);
        if (b == 0) goto L360;
        if (data_0209caa0[2] & 0x80) goto L360;
        if (self->unk_255 != 0) goto L360;
    }
L350:
    self->unk_21c = 0;
    goto L3e0;

L360:
    if (self->unk_255 == 0) goto L3a4;
    if (data_ov002_0211114c == 0) goto L3e0;
    FANG += 0x40;
    if ((u16)self->unk_21c >= 0x8000)
        data_ov002_0211114c = 0;
    goto L3e0;

L3a4:
    if (data_0209d660 != 0) goto L3e0;
    {
        s32 f = cam->f154;
        if (f & 0xc000) goto L3e0;
        if (f & 8) goto L3e0;
    }
    self->unk_21c = (s16)(cam->f17c & 0xffe0);

L3e0:
    obj = (Obj *)cam->f110;

L3e8:
    if (obj == 0) goto Lae4;

    if (self->unk_255 == 0) goto L44c;
    if (data_ov002_02111144 == 0) goto L4d8;
    F218 -= 9;
    if (self->unk_214 <= self->unk_218) goto L4d8;
    self->unk_218 = self->unk_214;
    data_ov002_02111144 = 0;
    data_ov002_0211114c = 1;
    goto L4d8;

L44c:
    if (data_0209d660 == 0) goto L47c;
    if (self->unk_218 < 0xbb8)
        F218 += 0x1c;
    goto L4d8;

L47c:
    if (self->unk_214 <= self->unk_218) goto L4b0;
    F218 += 0x1c;
    if (self->unk_214 < self->unk_218)
        self->unk_218 = self->unk_214;
    goto L4d8;

L4b0:
    if (self->unk_214 >= self->unk_218) goto L4d8;
    F218 -= 0x1c;
    if (self->unk_214 > self->unk_218)
        self->unk_218 = self->unk_214;

L4d8:
    self->unk_1f0 = _ZN4cstd4fdivEii(self->unk_1d4, self->unk_218);
    { s32 s1 = data_02082214[(((s32)(u16)self->unk_21c >> 4) * 2) + 1];
      self->unk_050 = FMUL(s1, self->unk_218); }
    { s32 s2 = data_02082214[((s32)(u16)self->unk_21c >> 4) * 2];
      self->unk_054 = FMUL(s2, self->unk_218); }
    self->unk_058 = -self->unk_054;
    self->mPosX = self->unk_050;
    ((Vector3*)&self->unk_1f4)->x = ((Vector3*)&self->unk_1e0)->x;
    ((Vector3*)&self->unk_1f4)->y = ((Vector3*)&self->unk_1e0)->y;
    ((Vector3*)&self->unk_1f4)->z = ((Vector3*)&self->unk_1e0)->z;
    self->unk_060 = self->unk_1dc + ((FMUL(((Vector3*)&self->unk_1f4)->x, self->unk_1d4) + 0x800) >> 12);
    self->unk_064 = self->unk_1dc + ((FMUL(((Vector3*)&self->unk_1f4)->z, self->unk_1d4) + 0x800) >> 12);

    op = (Vector3 *)(((int)obj + 0x5c));
    v14 = *op;
    _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(&v14);
    _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&v14, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
    {
        s32 p = data_0209f250;
        self->unk_070[p] = (v8.x + 0x800) >> 12;
        self->unk_080[p] = (v8.z + 0x800) >> 12;

        if (SublevelToLevel(data_0209f2f8) != 0x1d) goto B1;
        if (data_0209f2f8 != 1) goto L6a4;
        if (data_0209f2f8 == 0x33) goto B1;
        if (data_0209f2f8 != 3) goto L6a4;
B1:
        if (SublevelToLevel(data_0209f2f8) != 0x13) goto L6f0;
        if (data_0209f2f8 != 0x2e) goto L6f0;
L6a4:
        if (self->unk_070[p] < 0x60) { v8.x = 0x60000; }
        else if (self->unk_070[p] > 0xa0) { v8.x = 0xa0000; }
        if (self->unk_080[p] < 0x40) { v8.z = 0x40000; }
        else if (self->unk_080[p] > 0x80) { v8.z = 0x80000; }
        goto L738;
L6f0:
        if (self->unk_070[p] < 0x24) { v8.x = 0x24000; }
        else if (self->unk_070[p] > 0xdc) { v8.x = 0xdc000; }
        if (self->unk_080[p] < 0x24) { v8.z = 0x24000; }
        else if (self->unk_080[p] > 0x9c) { v8.z = 0x9c000; }
    }
L738:
    _ZN7Minimap20GetPosFromMinimapPosER7Vector3S1_5Fix12IiEsS1_(&v8, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v20);
    Vec3_Sub(&v38, &v14, &v20);
    AddVec3((Vector3*)&self->unk_1f4, &v38, (Vector3*)&self->unk_1f4);
    self->unk_060 = self->unk_1dc + ((FMUL(((Vector3*)&self->unk_1f4)->x, self->unk_1d4) + 0x800) >> 12);
    self->unk_064 = self->unk_1dc + ((FMUL(((Vector3*)&self->unk_1f4)->z, self->unk_1d4) + 0x800) >> 12);

    for (i = 0; i < 4; i++) {
        Obj *o = data_0209f394[i];
        if (o != 0) {
        v2c = *(Vector3 *)(((int)o + 0x5c));
        _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(&v2c);
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&v2c, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
        self->unk_070[i] = (v8.x + 0x800) >> 12;
        self->unk_080[i] = (v8.z + 0x800) >> 12;
        if (i != data_0209f250)
            self->unk_21e[i] = (s8)GetMinimapID(o, -1);
        else
            self->unk_21e[i] = (s8)GetMinimapID(o, data_ov002_02111148);
        } else {
            self->unk_21e[i] = -1;
        }
    }

    for (i = 0; i < 0xc; i++) {
        Obj *o = data_0209f40c[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
        self->unk_0a0[i] = (v8.x + 0x800) >> 12;
        self->unk_0d0[i] = (v8.z + 0x800) >> 12;
        if (data_0209f37c[i] != 4)
            self->unk_222[i] = (s8)GetMinimapID(o, -1);
        else
            self->unk_222[i] = 1;
        } else {
            self->unk_222[i] = -1;
        }
    }

    for (i = 0; i < 9; i++) {
        Obj *o = data_0209f3e8[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
        self->unk_100[i] = (v8.x + 0x800) >> 12;
        self->unk_124[i] = (v8.z + 0x800) >> 12;
        self->unk_23a[i] = (s8)GetMinimapID(o, -1);
        } else { self->unk_23a[i] = -1; }
    }

    if (data_0209caa0[1] & 0x40) goto La64;
    if ((data_0209caa0[2] & 0x20000) == 0) goto La64;
    {
        Obj *o = data_0209f33c;
        if (o == 0) { self->unk_248 = -1; goto La64; }
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
        self->unk_178 = (v8.x + 0x800) >> 12;
        self->unk_17c = (v8.z + 0x800) >> 12;
        self->unk_248 = 1;
    }

La64:
    for (i = 0; i < 8; i++) {
        Obj *o = data_0209f3a4[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, (Vector3*)&self->unk_1f4, self->unk_1f0, self->unk_21c, &v8);
        self->unk_180[i] = (v8.x + 0x800) >> 12;
        self->unk_1a0[i] = (v8.z + 0x800) >> 12;
        self->unk_249[i] = o->f0cc;
        } else { self->unk_249[i] = -1; }
    }

Lae4:
    {
        s32 id = GetMinimapID(obj, data_ov002_02111148);
        if (id == data_ov002_02111148) goto Lc30;
        if (id >= 0x10) goto Lc30;
        if (id < 0) goto Lbf0;

        if (SublevelToLevel(data_0209f2f8) != 0x1d) goto C1;
        if (data_0209f2f8 == 1) goto C1;
        if (data_0209f2f8 == 0x33) goto C1;
        if (data_0209f2f8 != 3) goto Lb9c;
C1:
        if (SublevelToLevel(data_0209f2f8) == 4) goto Lb9c;
        if (SublevelToLevel(data_0209f2f8) != 0x13) goto Lb84;
        if (data_0209f2f8 == 0x2e) goto Lb9c;
Lb84:
        if (SublevelToLevel(data_0209f2f8) != -1) goto Lbc0;
Lb9c:
        *(volatile u16 *)0x400100e = ((0x1f - id) << 8) | ((*(volatile u16 *)0x400100e & 0x43) | 0x4010);
        goto Lbdc;
Lbc0:
        *(volatile u16 *)0x400100e = ((0x1f - id) << 8) | ((*(volatile u16 *)0x400100e & 0x43) | 0x10);
Lbdc:
        data_0209d454 |= 8;
        goto Lc00;
Lbf0:
        data_0209d454 &= ~8;
Lc00:
        *(volatile s32 *)0x4001000 = (*(volatile s32 *)0x4001000 & ~0x1f00) | (data_0209d454 << 8);
        data_ov002_02111148 = (s8)id;
        self->unk_214 = GetMinimapScale(id);
    }
Lc30:
    UpdateMinimap(&self->unk_050, self->unk_060, self->unk_064, self->unk_060 - 0x80, self->unk_064 - 0x60);
    return 1;
}
