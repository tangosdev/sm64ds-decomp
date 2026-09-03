/* HOST COPY of src/_ZN7Minimap8BehaviorEv.cpp -- the sixteen-player minimap.
 *
 * Byte-for-byte the matched TU, with ONE change: the three PER-PLAYER tables
 * (mPlayerIconX @0x70, mPlayerIconY @0x80, mPlayerMapIDs @0x21e) move out of the
 * four-entry struct members a70/a80/a21e into the sixteen-wide host storage in
 * Minimap_wide.h, and the seat loop runs all sixteen slots instead of four. The
 * ROM sizes those arrays at the cartridge's four-player cap; this port hosts
 * sixteen (port/hal/vs_width.h), so a console seated on slot 4..15 wrote its own
 * blip past the end of the four-entry array. See Minimap_wide.h for the wall and
 * why src/ cannot carry the fix. src/ stays byte-frozen; this file is excluded
 * from slice_gate28.txt and linked in its place.
 *
 * Everything else -- the map centring, angle, scale, star/cap/spike-bomb seats,
 * the DISPCNT writes -- is the matched body unchanged, still through the struct.
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef int Fix12;

#include "Minimap_wide.h"

/* The single Minimap actor's per-player tables, sixteen wide, shared with the
 * Render host copy (declared extern there). Zero-initialised; reseated below
 * every frame the minimap ticks. */
int         g_mm_iconX[kMinimapWidePlayers];
int         g_mm_iconY[kMinimapWidePlayers];
signed char g_mm_mapID[kMinimapWidePlayers];

struct Vector3 { s32 x, y, z; };

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

struct Minimap {
    char pad0[0x50];
    s32 f50, f54, f58, f5c, f60, f64;   /* 0x50 */
    char padA[8];
    s32 a70[4];         /* 0x70  -- now hosted in g_mm_iconX (unused here) */
    s32 a80[4];         /* 0x80  -- now hosted in g_mm_iconY (unused here) */
    char pad1[0x10];    /* 0x90 */
    s32 aa0[12];        /* 0xa0 */
    s32 ad0[12];        /* 0xd0 */
    s32 a100[9];        /* 0x100 */
    s32 a124[9];        /* 0x124 */
    char pad2[0x178 - 0x148];
    s32 f178, f17c;     /* 0x178 */
    s32 a180[8];        /* 0x180 */
    s32 a1a0[8];        /* 0x1a0 */
    char pad3[0x1d4 - 0x1c0];
    Fix12 f1d4;         /* 0x1d4 */
    s32 f1d8;
    s32 f1dc;           /* 0x1dc */
    Vector3 v1e0;       /* 0x1e0 */
    s32 f1ec;
    Fix12 f1f0;         /* 0x1f0 */
    Vector3 v1f4;       /* 0x1f4 */
    char pad4[0x214 - 0x200];
    s32 f214;           /* 0x214 */
    s32 f218;           /* 0x218 */
    s16 ang;            /* 0x21c */
    s8 a21e[4];         /* 0x21e -- now hosted in g_mm_mapID (unused here) */
    s8 a222[12];        /* 0x222 */
    char pad5[0x23a - 0x22e];
    s8 a23a[9];         /* 0x23a */
    char pad6[0x248 - 0x243];
    s8 f248;            /* 0x248 */
    s8 a249[8];         /* 0x249 */
    char pad7[0x254 - 0x251];
    u8 f254;            /* 0x254 */
    u8 f255;            /* 0x255 */
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

extern "C" s32 _ZN7Minimap8BehaviorEv(Minimap *self);

s32 _ZN7Minimap8BehaviorEv(Minimap *self)
{
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

    if (self->f254 != 0) {
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
        if (v == 0 && self->f254 == 0) goto L200;
        data_0209d454 |= 4;
        if (v != 0) {
            self->f254 = 0x1e;
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
        self->f254 = 0;
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
        if (self->f255 != 0) goto L360;
    }
L350:
    self->ang = 0;
    goto L3e0;

L360:
    if (self->f255 == 0) goto L3a4;
    if (data_ov002_0211114c == 0) goto L3e0;
    FANG += 0x40;
    if ((u16)self->ang >= 0x8000)
        data_ov002_0211114c = 0;
    goto L3e0;

L3a4:
    if (data_0209d660 != 0) goto L3e0;
    {
        s32 f = cam->f154;
        if (f & 0xc000) goto L3e0;
        if (f & 8) goto L3e0;
    }
    self->ang = (s16)(cam->f17c & 0xffe0);

L3e0:
    obj = (Obj *)cam->f110;

L3e8:
    if (obj == 0) goto Lae4;

    if (self->f255 == 0) goto L44c;
    if (data_ov002_02111144 == 0) goto L4d8;
    F218 -= 9;
    if (self->f214 <= self->f218) goto L4d8;
    self->f218 = self->f214;
    data_ov002_02111144 = 0;
    data_ov002_0211114c = 1;
    goto L4d8;

L44c:
    if (data_0209d660 == 0) goto L47c;
    if (self->f218 < 0xbb8)
        F218 += 0x1c;
    goto L4d8;

L47c:
    if (self->f214 <= self->f218) goto L4b0;
    F218 += 0x1c;
    if (self->f214 < self->f218)
        self->f218 = self->f214;
    goto L4d8;

L4b0:
    if (self->f214 >= self->f218) goto L4d8;
    F218 -= 0x1c;
    if (self->f214 > self->f218)
        self->f218 = self->f214;

L4d8:
    self->f1f0 = _ZN4cstd4fdivEii(self->f1d4, self->f218);
    { s32 s1 = data_02082214[(((s32)(u16)self->ang >> 4) * 2) + 1];
      self->f50 = FMUL(s1, self->f218); }
    { s32 s2 = data_02082214[((s32)(u16)self->ang >> 4) * 2];
      self->f54 = FMUL(s2, self->f218); }
    self->f58 = -self->f54;
    self->f5c = self->f50;
    self->v1f4.x = self->v1e0.x;
    self->v1f4.y = self->v1e0.y;
    self->v1f4.z = self->v1e0.z;
    self->f60 = self->f1dc + ((FMUL(self->v1f4.x, self->f1d4) + 0x800) >> 12);
    self->f64 = self->f1dc + ((FMUL(self->v1f4.z, self->f1d4) + 0x800) >> 12);

    op = (Vector3 *)(((int)obj + 0x5c));
    v14 = *op;
    _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(&v14);
    _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&v14, &self->v1f4, self->f1f0, self->ang, &v8);
    {
        s32 p = data_0209f250;
        g_mm_iconX[p] = (v8.x + 0x800) >> 12;
        g_mm_iconY[p] = (v8.z + 0x800) >> 12;

        if (SublevelToLevel(data_0209f2f8) != 0x1d) goto B1;
        if (data_0209f2f8 != 1) goto L6a4;
        if (data_0209f2f8 == 0x33) goto B1;
        if (data_0209f2f8 != 3) goto L6a4;
B1:
        if (SublevelToLevel(data_0209f2f8) != 0x13) goto L6f0;
        if (data_0209f2f8 != 0x2e) goto L6f0;
L6a4:
        if (g_mm_iconX[p] < 0x60) { v8.x = 0x60000; }
        else if (g_mm_iconX[p] > 0xa0) { v8.x = 0xa0000; }
        if (g_mm_iconY[p] < 0x40) { v8.z = 0x40000; }
        else if (g_mm_iconY[p] > 0x80) { v8.z = 0x80000; }
        goto L738;
L6f0:
        if (g_mm_iconX[p] < 0x24) { v8.x = 0x24000; }
        else if (g_mm_iconX[p] > 0xdc) { v8.x = 0xdc000; }
        if (g_mm_iconY[p] < 0x24) { v8.z = 0x24000; }
        else if (g_mm_iconY[p] > 0x9c) { v8.z = 0x9c000; }
    }
L738:
    _ZN7Minimap20GetPosFromMinimapPosER7Vector3S1_5Fix12IiEsS1_(&v8, &self->v1f4, self->f1f0, self->ang, &v20);
    Vec3_Sub(&v38, &v14, &v20);
    AddVec3(&self->v1f4, &v38, &self->v1f4);
    self->f60 = self->f1dc + ((FMUL(self->v1f4.x, self->f1d4) + 0x800) >> 12);
    self->f64 = self->f1dc + ((FMUL(self->v1f4.z, self->f1d4) + 0x800) >> 12);

    /* SIXTEEN slots (was four). Empty slots -> mapID -1, exactly as the ROM did
       for slots without a player, so four-player output is unchanged. */
    for (i = 0; i < kMinimapWidePlayers; i++) {
        Obj *o = data_0209f394[i];
        if (o != 0) {
        v2c = *(Vector3 *)(((int)o + 0x5c));
        _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(&v2c);
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&v2c, &self->v1f4, self->f1f0, self->ang, &v8);
        g_mm_iconX[i] = (v8.x + 0x800) >> 12;
        g_mm_iconY[i] = (v8.z + 0x800) >> 12;
        if (i != data_0209f250)
            g_mm_mapID[i] = (s8)GetMinimapID(o, -1);
        else
            g_mm_mapID[i] = (s8)GetMinimapID(o, data_ov002_02111148);
        } else {
            g_mm_mapID[i] = -1;
        }
    }

    for (i = 0; i < 0xc; i++) {
        Obj *o = data_0209f40c[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, &self->v1f4, self->f1f0, self->ang, &v8);
        self->aa0[i] = (v8.x + 0x800) >> 12;
        self->ad0[i] = (v8.z + 0x800) >> 12;
        if (data_0209f37c[i] != 4)
            self->a222[i] = (s8)GetMinimapID(o, -1);
        else
            self->a222[i] = 1;
        } else {
            self->a222[i] = -1;
        }
    }

    for (i = 0; i < 9; i++) {
        Obj *o = data_0209f3e8[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, &self->v1f4, self->f1f0, self->ang, &v8);
        self->a100[i] = (v8.x + 0x800) >> 12;
        self->a124[i] = (v8.z + 0x800) >> 12;
        self->a23a[i] = (s8)GetMinimapID(o, -1);
        } else { self->a23a[i] = -1; }
    }

    if (data_0209caa0[1] & 0x40) goto La64;
    if ((data_0209caa0[2] & 0x20000) == 0) goto La64;
    {
        Obj *o = data_0209f33c;
        if (o == 0) { self->f248 = -1; goto La64; }
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, &self->v1f4, self->f1f0, self->ang, &v8);
        self->f178 = (v8.x + 0x800) >> 12;
        self->f17c = (v8.z + 0x800) >> 12;
        self->f248 = 1;
    }

La64:
    for (i = 0; i < 8; i++) {
        Obj *o = data_0209f3a4[i];
        if (o != 0) {
        _ZN7Minimap15GetPosOnMinimapER7Vector3S1_5Fix12IiEsS1_(&o->pos, &self->v1f4, self->f1f0, self->ang, &v8);
        self->a180[i] = (v8.x + 0x800) >> 12;
        self->a1a0[i] = (v8.z + 0x800) >> 12;
        self->a249[i] = o->f0cc;
        } else { self->a249[i] = -1; }
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
        self->f214 = GetMinimapScale(id);
    }
Lc30:
    UpdateMinimap(&self->f50, self->f60, self->f64, self->f60 - 0x80, self->f64 - 0x60);
    return 1;
}
