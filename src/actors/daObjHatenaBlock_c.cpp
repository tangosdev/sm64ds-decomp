//cpp
/* ov102/daObjHatenaBlock_c -- the whole translation unit, 41 functions,
 * .text 0x02148fbc..0x0214a96c. This is the production source: it is the
 * single `complete` claim over that range in
 * config/arm9/overlays/ov102/delinks.txt, and the 41 one-function sources it
 * was folded from are gone. config/tu_manifest.d/ov102/daObjHatenaBlock_c.json
 * carries the boundary evidence and the verification record.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * ROM order, which is the reverse of the order below:
 *   [0] 0x02148fbc  _ZN18daObjHatenaBlock_cD1Ev
 *   [1] 0x02149010  _ZN18daObjHatenaBlock_cD0Ev
 *   [2] 0x02149078  func_ov102_02149078
 *   [3] 0x02149100  func_ov102_02149100
 *   [4] 0x02149220  func_ov102_02149220
 *   [5] 0x02149288  func_ov102_02149288
 *   [6] 0x021492d4  func_ov102_021492d4
 *   [7] 0x02149384  func_ov102_02149384
 *   [8] 0x021493dc  func_ov102_021493dc
 *   [9] 0x02149428  func_ov102_02149428
 *   [10] 0x02149478  func_ov102_02149478
 *   [11] 0x021494cc  func_ov102_021494cc
 *   [12] 0x0214953c  func_ov102_0214953c
 *   [13] 0x02149610  func_ov102_02149610
 *   [14] 0x02149684  func_ov102_02149684
 *   [15] 0x021496a4  _ZN18daObjHatenaBlock_c19OnHitFromUnderneathER8dActor_c
 *   [16] 0x02149710  _ZN18daObjHatenaBlock_c15OnHitByMegaCharER6Player
 *   [17] 0x02149770  _ZN18daObjHatenaBlock_c8OnKickedER8dActor_c
 *   [18] 0x021497c8  _ZN18daObjHatenaBlock_c11OnAttacked1ER8dActor_c
 *   [19] 0x02149820  _ZN18daObjHatenaBlock_c15OnGroundPoundedER8dActor_c
 *   [20] 0x02149878  func_ov102_02149878
 *   [21] 0x021498c4  func_ov102_021498c4
 *   [22] 0x021498e0  func_ov102_021498e0
 *   [23] 0x02149c78  func_ov102_02149c78
 *   [24] 0x02149ccc  func_ov102_02149ccc
 *   [25] 0x02149d80  func_ov102_02149d80
 *   [26] 0x02149da8  func_ov102_02149da8
 *   [27] 0x02149df0  func_ov102_02149df0
 *   [28] 0x02149e38  func_ov102_02149e38
 *   [29] 0x02149ea4  func_ov102_02149ea4
 *   [30] 0x02149ff0  func_ov102_02149ff0
 *   [31] 0x0214a08c  _ZN18daObjHatenaBlock_c16CleanupResourcesEv
 *   [32] 0x0214a2ac  _ZN18daObjHatenaBlock_c6RenderEv
 *   [33] 0x0214a32c  _ZN18daObjHatenaBlock_c8BehaviorEv
 *   [34] 0x0214a4a4  _ZN18daObjHatenaBlock_c13InitResourcesEv
 *   [35] 0x0214a7ec  daObjHatenaBlock_c_classInit_CAP_BLOCK_W
 *   [36] 0x0214a82c  daObjHatenaBlock_c_classInit_CAP_BLOCK_L
 *   [37] 0x0214a86c  daObjHatenaBlock_c_classInit_CAP_BLOCK_M
 *   [38] 0x0214a8ac  daObjHatenaBlock_c_classInit_ITEM_BLOCK
 *   [39] 0x0214a8ec  daObjHatenaBlock_c_classInit_HATENA_BLOCK
 *   [40] 0x0214a92c  daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK
 */

/* Include ORDER IS LOAD-BEARING. common.h and types.h must come first:
 * Matrix4x3 has two spellings -- the flat s32 m[12] in common.h and the
 * Matrix3x3 r; Vector3 t; in math/Matrix.h, which the class header reaches
 * through Model.h -- and whichever a TU sees first is the one it compiles
 * against. With the class header first, func_ov102_02149e38 and
 * func_ov102_02149ff0 differ by 0x18 and 0x1c bytes. */
#include "common.h"
#include "types.h"
#include "daObjHatenaBlock_c.h"
#include "dBgCh_Gnd.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Declarations the folded sources needed, reconciled to one spelling each.
 * include/decl_common.h is deliberately NOT included here: it declares five of
 * this unit's own functions as taking void *, which contradicts their
 * definitions below. The two symbols only it supplied are restated instead. */
/* shadow struct 'dActor_c' */
struct dActor_c;

/* Vector3 and Vector3_16 come from types.h / common.h -- the shadow typedefs the
 * legacy one-function files carried are layout-identical and were dropped. */
/* shadow struct 'Vector3_16' */
struct Vector3_16;

/* shadow struct 'V3' */
struct V3 {
  int x, y, z;
  V3(int a, int b, int d) { x = a; y = b; z = d; }
  V3() {}
};

/* shadow struct 'Obj' */
struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void call(dActor_c *player);
};

/* shadow struct 'M12' */
struct M12 { int w[12]; };

/* TUBUILD CONFLICT -- alternate body of typedef 'Vector3', from the legacy file for func_ov102_0214953c, NOT applied:
typedef struct Vector3 { int x, y, z; } Vector3;
*/

/* The state-machine dispatch table's object view, shared by
 * func_ov102_02149da8, func_ov102_02149df0 and func_ov102_021498e0. Defined
 * once, at its first occurrence in source order (ROM ordinal 27 below);
 * forward-declared here because mwccarm emits .text in the REVERSE of source
 * order, so the members that call those helpers are written above them. */
struct C;

/* Reconciled shared declarations. Each external below is declared exactly
 * once, in the widest spelling any of the legacy one-function sources
 * carried; a call site that held a narrower view casts at the call instead.
 *
 * dActor_c::Spawn and dActor_c::ClosestPlayer are NOT restated here: both are
 * real members of include/dActor_c.h, and the nine mangled free-function
 * spellings the legacy sources carried (five of them mutually contradictory)
 * are replaced by calls through that header.
 *
 * include/decl_common.h is deliberately NOT included. It declares five of this
 * TU's own functions -- func_ov102_02149078, _02149da8, _02149df0, _02149ea4
 * and _02149e38 -- as taking void*, which contradicts the parameter spellings
 * the ROM bytes were reproduced from. The two symbols this TU actually needed
 * from it are restated below. */
extern "C" {

/* --- globals ----------------------------------------------------------- */
extern signed char data_0209f2f8;
extern int data_0209f32c;
extern int data_0209e650;
extern unsigned char data_0209f2d8;
extern char data_0209f318[];
extern char data_020a0edc[];
extern int data_0209caa0[];

/* File handles. Every use is either `&name` (InitResources, loading) or a
 * decay to the same address (CleanupResources, releasing), so one array
 * spelling serves both. */
extern char data_ov002_0210d954[];
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9c0[];
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210d9e0[];
extern char data_ov002_0210da18[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210da40[];
extern char data_ov002_0210da58[];
extern char gPFlowerOpenModelFile[];
extern char gPFlowerCloseModelFile[];
extern char data_ov102_0214e7d0[];
extern char data_ov102_0214e7d8[];
extern char data_ov102_0214e7e0[];
extern char data_ov102_0214e7e8[];
extern char data_ov102_0214e7f0[];
extern char data_ov102_0214e7f8[];
extern char data_ov102_0214e800[];
extern char data_ov102_0214e808[];

/* --- functions this TU calls but does not define ----------------------- */
extern int RandomIntInternal(int *seed);
extern int Vec3_HorzDist(const void *a, const void *b);
extern int DecIfAbove0_Short(void *p);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void *self);
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, void *pos, s32 radius);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *shadow, void *mtx, int fix, int t1, int t2, unsigned int n);
extern void _ZN8dActor_c11UntrackStarERa(void *self, void *p);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern unsigned char _ZN8dActor_c9TrackStarEjj(void *self, unsigned int a, unsigned int b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void _ZN9Animation7AdvanceEv(void *a);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bca, int a, int fx, unsigned int f);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
/* Restated from include/decl_common.h, which this TU does not include. */
extern void func_ov102_0214ad14(void *actor);
extern void func_ov002_020f0438(void *actor);

/* --- this TU's own members, forward-declared --------------------------- */
/* mwccarm emits one .text section per function in the REVERSE of source
 * order, so every function below is DEFINED further down this file. */
int func_ov102_02149078(dActor_c *self);
void func_ov102_02149100(char *c, Vector3 *pos, int n, unsigned int speed, short arg5);
void *func_ov102_02149220(char *c);
void func_ov102_0214953c(char *c, int p1, int p2);
int func_ov102_02149610(char *c);
void func_ov102_02149684(int *dst, int *src);
void func_ov102_02149da8(C *c, int i);
void func_ov102_02149df0(C *c);
void func_ov102_02149e38(char *self);
void func_ov102_02149ea4(char *c);
void func_ov102_02149ff0(char *c);
}

/* -------------------------------------------------------------------------- */
/* Registry factories, ROM ordinals 35-40, 0x0214a7ec..0x0214a96c.
 *
 * Six one-per-registry-profile constructors for the same class. They abut the
 * class's own members with no gap -- the entry below them in
 * config/arm9/overlays/ov102/delinks.txt ends at 0x0214a7ec and the entry
 * above them begins at 0x0214a96c, where the next TU's first function starts.
 * They are part of this object, not a separate one.
 *
 * Each is byte-identical to the others apart from its symbol name: the
 * registry profile is carried by which factory the table names, not by
 * anything the body does. The 1016-byte allocation is 0x3f8, the size
 * include/daObjHatenaBlock_c.h asserts.
 *
 * Written in DESCENDING address order because mwccarm emits one .text section
 * per function in the REVERSE of source order.
 */
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern void _ZN9ModelAnimC1Ev(void *self);
extern void _ZN11ShadowModelC1Ev(void *self);
extern int _ZTV18daObjHatenaBlock_c[];
}

/* The vptr store below is spelled &_ZTV18daObjHatenaBlock_c[2], not the bare
 * array name the legacy one-function sources used. Both name the same word in
 * the ROM -- the vtable's public address point, two header words in. The
 * spelling has to change because the legacy files IMPORTED the vtable, so the
 * eight-byte bias was folded into the relocation addend, while this TU is the
 * one that EMITS it and the compiler relocates against the section base. Same
 * bytes either way; the bare name here would carry addend 0 and the
 * relocation audit rejects it. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK, 0x0214a92c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the VS_ITEM_BLOCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExclamationBlockVs_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_VS_ITEM_BLOCK(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- daObjHatenaBlock_c_classInit_HATENA_BLOCK, 0x0214a8ec, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_HATENA_BLOCK
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the HATENA_BLOCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: QuestionBlock_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_HATENA_BLOCK(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- daObjHatenaBlock_c_classInit_ITEM_BLOCK, 0x0214a8ac, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_ITEM_BLOCK
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the ITEM_BLOCK registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExclamationBlock_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_ITEM_BLOCK(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- daObjHatenaBlock_c_classInit_CAP_BLOCK_M, 0x0214a86c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_M
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the CAP_BLOCK_M registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CapBlockMario_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_CAP_BLOCK_M(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- daObjHatenaBlock_c_classInit_CAP_BLOCK_L, 0x0214a82c, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_L
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the CAP_BLOCK_L registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CapBlockLuigi_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_CAP_BLOCK_L(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- daObjHatenaBlock_c_classInit_CAP_BLOCK_W, 0x0214a7ec, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daObjHatenaBlock_c_classInit_CAP_BLOCK_W
/* Reconstructed source-style name: SM64DS proves daObjHatenaBlock_c through RTTI,
 * allocation size, vtable identity, and the CAP_BLOCK_W registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: CapBlockWario_Spawn. */
extern "C" int *daObjHatenaBlock_c_classInit_CAP_BLOCK_W(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1016);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)&_ZTV18daObjHatenaBlock_c[2];
        _ZN9ModelAnimC1Ev((char *)p + 0x320);
        _ZN11ShadowModelC1Ev((char *)p + 0x384);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN18daObjHatenaBlock_c13InitResourcesEv, 0x0214a4a4, size 0x348 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjHatenaBlock_c::InitResources()
{
    void *r5 = 0;
    switch (actorID - 0x14) {
    case 0:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e8);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModelAnim, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e808), 1, 0x19);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov102_0214e7f8), 0, 0x1000, 0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 1:
    case 2:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e800);
        break;
    case 3:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7f0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 5:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7d8);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 4:
        r5 = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov102_0214e7e0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    }

    _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModel, r5, 1, -1);
    mShadowModel.InitCuboid();
    func_ov102_02149da8((C *)this, 0);
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    func_ov102_02149ff0((char *)this);
    func_ov102_02149e38((char *)this);
    *(struct M12 *)&mShadowMat = *(struct M12 *)((char *)&mModel + 0x1c);
    {
        void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov102_0214e7d0);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((char *)&mMeshCollider, kcl, (char *)&mClsnMat, 0x199, mAngleY, &data_ov002_0210d954);
    }
    mHomePosY = mPosY;
    mContentType = (unsigned char)param1;
    if (mContentType == 0xff)
        mContentType = 0;
    if (mContentType == 1) {
        mStarId = (unsigned char)(param1 >> 8);
        if (mStarId == 0xff)
            mStarId = 0;
        mStarTracked = _ZN8dActor_c9TrackStarEjj((char *)this, mStarId, 2);
    }

    {
        int b16 = !(actorID != 0x16);
        if (b16 != 0) {
            mContentType = 0;
            _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da58);
        }
    }

    {
        int b14 = !(actorID != 0x14);
        if (b14 == 0) {
            int b15 = !(actorID != 0x15);
            if (b15 == 0)
                goto end;
        }
    }

    switch (mContentType) {
    case 0:
    case 1:
        break;
    case 3:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da18);
        break;
    case 2:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
        break;
    case 4:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
        break;
    case 7:
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerOpenModelFile);
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerCloseModelFile);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
        break;
    case 5:
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da58);
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerOpenModelFile);
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerCloseModelFile);
        break;
    case 6:
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerOpenModelFile);
        _ZN5Model8LoadFileER13SharedFilePtr(&gPFlowerCloseModelFile);
        break;
    }
end:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN18daObjHatenaBlock_c8BehaviorEv, 0x0214a32c, size 0x178 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjHatenaBlock_c::Behavior()
{
    func_ov102_02149df0((C *)this);
    if (mState != 2) {
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
        if (mPosY <= mHomePosY) mPosY = mHomePosY;
        func_ov102_02149ff0(((char*)this));
        func_ov102_02149ea4(((char*)this));
    }
    func_020393a4((int*)&mMeshCollider, 0x8c000);
    func_02039394((int*)&mMeshCollider, 0x46000);
    if (mState != 0) {
        if (((dBgW *)&mMeshCollider)->IsEnabled() != 0) {
            ((dBgW *)&mMeshCollider)->Disable();
        }
        goto end;
    }
    if ((data_0209caa0[1] & 0x80000000) == 0) {
        int b = (int)(actorID == 0x14);
        if (b != 0) {
            _ZN9Animation7AdvanceEv((char*)(Animation *)&mModelAnim);
            if (((dBgW *)&mMeshCollider)->IsEnabled() != 0) {
                ((dBgW *)&mMeshCollider)->Disable();
            }
            goto end;
        }
    }
    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) {
            if (((dBgW *)&mMeshCollider)->IsEnabled() == 0) {
                ((dBgW *)(&mMeshCollider))->Enable((dActor_c *)(((char*)this)));
            }
            goto end;
        }
    }
    if (data_0209f2f8 == 0x1c) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x460000, 0x46000) != 0) {
            func_ov102_02149e38(((char*)this));
        }
        goto end;
    }
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x118000, 0x46000) != 0) {
        func_ov102_02149e38(((char*)this));
    }
end:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN18daObjHatenaBlock_c6RenderEv, 0x0214a2ac, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c6RenderEv
/* recovered: named members + shared header, real C++ method */

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int a);
};

/* No `#pragma optimize_for_size on` here, although the legacy one-function
   source for Render carried one. That pragma is file-global whatever line it
   sits on, so carrying it across turned 20 of 35 members into byte matches;
   dropping it gives 41 of 41, Render included. Re-adding it is a regression,
   not a restoration. */

int daObjHatenaBlock_c::Render()
{
    if (mState == 2)
        goto done;
    if ((*(int*)((char*)data_0209caa0 + 4) & 0x80000000) == 0) {
        int b = (actorID == 0x14);
        if (b != 0) {
            Sub* s = (Sub*)&mModelAnim;
            s->m(0);
            goto done;
        }
    }
    {
        Sub* s2 = (Sub*)&mModel;
        s2->m((int)((char*)&mScaleX));
    }
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN18daObjHatenaBlock_c16CleanupResourcesEv, 0x0214a08c, size 0x220 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daObjHatenaBlock_c::CleanupResources()
{
    int b, b2, b3;
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();

    b = (int)(actorID == 0x16);
    if (b)
        ((SharedFilePtr *)(data_ov002_0210da58))->Release();

    b2 = (int)(actorID == 0x14);
    if (b2)
        goto dosw;
    b3 = (int)(actorID == 0x15);
    if (b3) {
    dosw:
        switch (mContentType) {
        case 1:
            _ZN8dActor_c11UntrackStarERa(((char *)this), ((char *)this) + 0x3f0);
            break;
        case 3:
            ((SharedFilePtr *)(data_ov002_0210da18))->Release();
            break;
        case 2:
            ((SharedFilePtr *)(data_ov002_0210d9d8))->Release();
            break;
        case 4:
            ((SharedFilePtr *)(data_ov002_0210da30))->Release();
            break;
        case 7:
            ((SharedFilePtr *)(gPFlowerOpenModelFile))->Release();
            ((SharedFilePtr *)(gPFlowerCloseModelFile))->Release();
            ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
            break;
        case 5:
            ((SharedFilePtr *)(data_ov002_0210da58))->Release();
            ((SharedFilePtr *)(gPFlowerOpenModelFile))->Release();
            ((SharedFilePtr *)(gPFlowerCloseModelFile))->Release();
            break;
        case 6:
            ((SharedFilePtr *)(gPFlowerOpenModelFile))->Release();
            ((SharedFilePtr *)(gPFlowerCloseModelFile))->Release();
            break;
        }
    }

    switch (actorID - 0x14) {
    case 0:
        ((SharedFilePtr *)(data_ov102_0214e7e8))->Release();
        ((SharedFilePtr *)(data_ov102_0214e808))->Release();
        ((SharedFilePtr *)(data_ov102_0214e7f8))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 1:
    case 2:
        ((SharedFilePtr *)(data_ov102_0214e800))->Release();
        break;
    case 3:
        ((SharedFilePtr *)(data_ov102_0214e7f0))->Release();
        ((SharedFilePtr *)(data_ov002_0210da40))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 5:
        ((SharedFilePtr *)(data_ov102_0214e7d8))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9a0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
        break;
    case 4:
        ((SharedFilePtr *)(data_ov102_0214e7e0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9c0))->Release();
        ((SharedFilePtr *)(data_ov002_0210d9e0))->Release();
    }
    ((SharedFilePtr *)(data_ov102_0214e7d0))->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov102_02149ff0, 0x02149ff0, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149ff0
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149ff0(char *c)
{
    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + *(int *)(c + 0x3dc)) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    if (data_0209caa0[1] & 0x80000000)
        return;
    int b = (int)(*(unsigned short *)(c + 0xc) == 0x14);
    if (b == 0)
        return;
    *(struct Matrix4x3 *)(c + 0x33c) = *(struct Matrix4x3 *)(c + 0xf0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov102_02149ea4, 0x02149ea4, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149ea4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149ea4(char *c)
{
    int r4, r5, r2v;
    int b0, b1;

    b0 = (*(int*)(c + 0xb0) & 8) ? 1 : 0;
    if (b0 != 0) {
        b1 = (*(volatile unsigned char *)&data_0209f2d8 == 1) ? 1 : 0;
        if (b1 == 0) goto skipcall;
    }
    *(int*)(c + 0x3e4) = func_ov102_02149610(c);
skipcall:
    r5 = *(int*)(c + 0x60) - *(int*)(c + 0x3e4);
    if (r5 <= 0x1000) r5 = 0x1000;
    r4 = (int)(((long long)r5 * 0x180 + 0x800) >> 12);
    r4 = 0xb4000 - r4;
    r2v = r5 + 0x214000;
    if (r4 < 0xa000) r4 = 0xa000;
    if (r2v < 0x200000) r2v = 0x200000;
    *(int*)(c + 0xb4) = -((int)((r5 + 0x14000) + ((unsigned)(r5 + 0x14000) >> 31)) >> 1);
    *(int*)(c + 0xb8) = (int)(r2v + ((unsigned)r2v >> 31)) >> 4;
    r4 = (int)(((long long)r4 * *(int*)(c + 0x80) + 0x800) >> 12);
    Matrix4x3_FromRotationY(c + 0x3ac, *(short*)(c + 0x8e));
    *(int*)(c + 0x3d0) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3d4) = (*(int*)(c + 0x60) - 0x20000) >> 3;
    *(int*)(c + 0x3d8) = *(int*)(c + 0x64) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x384, c + 0x3ac, r4, r5 + 0x14000, r4, 0xf);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov102_02149e38, 0x02149e38, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149e38
/* recovered: shared common types */
extern "C" {

/* Uniquified: the file scope this member now shares already has an `Obj`,
   func_ov102_02149ccc's vtable view of the same actor. */
struct MMC { char p[0x124]; };
struct HbMbgObj { char p[0x2ec]; Matrix4x3 m; };
int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC*, Matrix4x3&, short);
void func_ov102_02149e38(char* self){
    HbMbgObj* o = (HbMbgObj*)self;
    o->m = *(Matrix4x3*)(self + 0xf0);
    *(int*)(self+0x310) = *(int*)(self+0x5c);
    *(int*)(self+0x314) = *(int*)(self+0x60) + *(int*)(self+0x3dc);
    *(int*)(self+0x318) = *(int*)(self+0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC*)(self+0x124), o->m, *(short*)(self+0x8e));
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov102_02149df0, 0x02149df0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149df0
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf[2]; };
extern Entry data_ov102_0214e890[];
struct C { char pad[0x3e8]; int idx; };
extern "C" void func_ov102_02149df0(C *c) { int j = c->idx; (c->*data_ov102_0214e890[j].pmf[1])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov102_02149da8, 0x02149da8, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149da8
/* C, PMF, Entry and data_ov102_0214e890 are declared once, above, at
   func_ov102_02149df0 -- the first of the pair in source order. */
extern "C" void func_ov102_02149da8(C *c, int i) { c->idx = i; int j = c->idx; (c->*data_ov102_0214e890[j].pmf[0])(); }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov102_02149d80, 0x02149d80, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149d80
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149d80(void *c) {
    *(s32*)((char*)c + 0x3dc) = 0;
    *(s32*)((char*)c + 0x80) = 0x1000;
    *(s32*)((char*)c + 0x84) = 0x1000;
    *(s32*)((char*)c + 0x88) = 0x1000;
    *(s32*)((char*)c + 0x9c) = -0x8000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov102_02149ccc, 0x02149ccc, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149ccc
// func_ov102_02149ccc at 0x02149ccc
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov102).
extern "C" void func_ov102_02149ccc(char *self)
{
    dActor_c *player;
    if (!(data_0209caa0[1] & 0x80000000)) {
        int b = (int)(*(unsigned short *)(self + 0xc) == 0x14);
        if (b)
            return;
    }
    player = (dActor_c *)((dActor_c *)self)->ClosestPlayer();
    if (*(unsigned char *)((char *)player + 0x703) == 0)
        return;
    if (Vec3_HorzDist(self + 0x5c, (char *)player + 0x5c) >= 0xc8000)
        return;
    if (*(int *)(self + 0x60) <= *(int *)((char *)player + 0x60))
        return;
    ((Obj *)self)->call(player);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov102_02149c78, 0x02149c78, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149c78
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149c78(void *c)
{
    s32 vec[3];
    void *r4 = c;
    vec[0] = *(s32*)((char*)r4 + 0x5c);
    vec[1] = *(s32*)((char*)r4 + 0x60);
    vec[2] = *(s32*)((char*)r4 + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(r4, vec, 0x5dc000);
    *(u16*)((char*)r4 + 0x3ec) = 0x4000;
    *(u16*)((char*)r4 + 0x3ee) = 7;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov102_021498e0, 0x021498e0, size 0x398 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_021498e0
extern s16 data_02082214[];
extern PMF data_ov102_0214e870[][4];
extern PMF data_ov102_0214e8c0[][4];
extern "C" {
void _ZN5Sound9PlayBank3EjRK7Vector3(int bank, void *pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x, int y, int z);
int _ZN8SaveData16HasPlayerLostCapEv(void);
}
extern "C" void func_ov102_021498e0(C *self)
{
    char *c = (char *)self;
    Vector3 pos;
    int ch;
    u16 typ;
    void *held;
    func_ov102_02149684((int *)&pos, (int *)c);
    if (DecIfAbove0_Short((s16 *)(c + 0x3ee)) != 0) {

        s16 *pang = (s16 *)((int)(c + 0x3ec));
        u16 ang = *(u16 *)(c + 0x3ec);
        s16 s = data_02082214[(ang >> 4) * 2];
        int conf = 0x999;
        int t = (int)s + 0x1000;
        int y = (int)(((s64)t * conf + 0x800) >> 12);
        *(int *)(c + 0x84) = y + 0x666;
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        int u = 0x1000 - (int)s;
        *(int *)(c + 0x80) = (int)(((s64)u * 0x1000 + 0x800) >> 12) + 0x1000;
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        ang = *(u16 *)(c + 0x3ec);
        s = data_02082214[(ang >> 4) * 2];
        *(int *)(c + 0x3dc) = (0x1000 - (int)s) * 0xd;
        *pang += 0x1000;

        return;
    }
    held = *(void **)(c + 0x3f4);
    if (held != 0) {
        if (*(u16 *)((char *)held + 0xc) == 0x149)
            func_ov002_020f0438(held);
        *(void **)(c + 0x3f4) = 0;
    }
    _ZN5Sound9PlayBank3EjRK7Vector3(0, c + 0x74);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xb, pos.x, pos.y, pos.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd, pos.x, pos.y, pos.z);
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: case 2:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10, pos.x, pos.y, pos.z); break;
    case 0:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc, pos.x, pos.y, pos.z); break;
    case 3: case 4: case 5:
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(9, pos.x, pos.y, pos.z); break;
    }
    ch = (int)*(u8 *)(c + 0x3f2);
    if (ch < 0) ch = 0; else if (ch > 3) ch = 3;
    typ = *(u16 *)(c + 0xc);
    switch (typ - 0x14) {
    case 1: {
        PMF (*tbl)[4] = data_ov102_0214e8c0;
        u8 content = *(u8 *)(c + 0x3f3);
        (self->*tbl[content][0])();
        break;
    }
    case 2: {
        PMF (*tbl)[4] = data_ov102_0214e870;
        u8 content = *(u8 *)(c + 0x3f3);
        if (ch >= 4) ch = 0;
        (self->*tbl[content][ch])();
        break;
    }
    case 0:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0 || data_0209f2f8 == 0x1f) {
            PMF (*tbl)[4] = data_ov102_0214e8c0;
            u8 content = *(u8 *)(c + 0x3f3);
            if (ch >= 4) ch = 0;
            (self->*tbl[content][ch])();
        } else {
            func_ov102_02149220(c);
        }
        break;
    case 3:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 0, 0x12);
        else func_ov102_02149220(c);
        break;
    case 5:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 1, 0x12);
        else func_ov102_02149220(c);
        break;
    case 4:
        if (_ZN8SaveData16HasPlayerLostCapEv() == 0) func_ov102_0214953c(c, 2, 0x12);
        else func_ov102_02149220(c);
        break;
    }
    func_ov102_02149da8((C *)c, 2);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov102_021498c4, 0x021498c4, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_021498c4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_021498c4(void *p) {
    *(short *)((char *)p + 0x3ee) = 0x12c;
    *(int *)((char *)p + 0x9c) = -0x8000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov102_02149878, 0x02149878, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149878
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov102_02149878(char *c)
{
    int r = DecIfAbove0_Short(c + 0x3ee);
    if (r != 0) return r;
    r = _ZN8dActor_c13DistToCPlayerEv(c);
    if (r <= 0x64000) return r;
    /* THE TAIL SETS NO RETURN VALUE. The ROM runs `bl func_ov102_02149da8`
       and falls straight into the epilogue, leaving r0 holding whatever the
       call left there, so the faithful shape is a body that reaches its
       closing brace with nothing to return -- the same spelling this class's
       OnAttacked1 and OnHitFromUnderneath already use. func_ov102_02149da8
       returns void, so `return` on it would not compile either. */
    func_ov102_02149da8((C *)c, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN18daObjHatenaBlock_c15OnGroundPoundedER8dActor_c, 0x02149820, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c15OnGroundPoundedER8dActor_c
/* daObjHatenaBlock_c::OnGroundPounded -- vtable slot 21, ov102 0x02149820.
 * include/dActor_c.h's own slot 21 supplies the signature -- `void`,
 * corrected from `int` by Stump::OnGroundPounded (include/Stump.h);
 * re-verified here rather than assumed.
 *
 * mState/mHitterParam are this class's own fields (include/daObjHatenaBlock_c.h).
 * func_ov102_02149078 and func_ov102_02149da8 are unmigrated helpers shared
 * by all five of this class's combat callbacks; still called by their
 * func_ov102_ names, same as the pre-migration recovery. */
void daObjHatenaBlock_c::OnGroundPounded(dActor_c &other)
{
    if (mState == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    mHitterParam = other.param1;
    func_ov102_02149da8((C *)this, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN18daObjHatenaBlock_c11OnAttacked1ER8dActor_c, 0x021497c8, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c11OnAttacked1ER8dActor_c
/* daObjHatenaBlock_c::OnAttacked1 -- vtable slot 22, ov102 0x021497c8.
 * include/dActor_c.h's own slot 22 supplies the signature. Same body shape
 * as this class's own OnGroundPounded (slot 21), whose comment in this file
 * carries the field and helper notes. */
/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
int daObjHatenaBlock_c::OnAttacked1(dActor_c &other)
{
    int v = mState;
    if (v != 1) {
        if (!func_ov102_02149078(this)) {
            int val = other.param1;
            mHitterParam = val;
            func_ov102_02149da8((C *)this, 1);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN18daObjHatenaBlock_c8OnKickedER8dActor_c, 0x02149770, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c8OnKickedER8dActor_c
/* daObjHatenaBlock_c::OnKicked -- vtable slot 24, ov102 0x02149770.
 *
 * `void`, not `int`: include/dActor_c.h's slot 24 was corrected to `void` by
 * BigBrickBlock::OnKicked (see include/BigBrickBlock.h and
 * src/_ZN8dActor_c8OnKickedERS_.cpp) and every override of it must match.
 *
 * Same body shape as this class's own OnGroundPounded (slot 21), whose
 * comment in this file carries the field and helper notes. */
void daObjHatenaBlock_c::OnKicked(dActor_c &other)
{
    if (mState == 1) return;
    int r = func_ov102_02149078(this);
    if (r != 0) return;
    mHitterParam = other.param1;
    func_ov102_02149da8((C *)this, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN18daObjHatenaBlock_c15OnHitByMegaCharER6Player, 0x02149710, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c15OnHitByMegaCharER6Player
/* daObjHatenaBlock_c::OnHitByMegaChar -- vtable slot 27, ov102 0x02149710.
 * include/dActor_c.h's own slot 27 supplies the signature -- `void`,
 * corrected from `int` by Stump::OnHitByMegaChar (include/Stump.h);
 * re-verified here rather than assumed.
 *
 * `player.param1` is fBase_c's own field, inherited by Player. Same body
 * shape as this class's own OnGroundPounded (slot 21), plus the
 * IncMegaKillCount() call; OnGroundPounded's own comment in this file carries
 * the field and helper notes. */
void daObjHatenaBlock_c::OnHitByMegaChar(Player &player)
{
    if (mState == 1) return;
    if (func_ov102_02149078(this) != 0) return;
    player.IncMegaKillCount();
    mHitterParam = player.param1;
    func_ov102_02149da8((C *)this, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN18daObjHatenaBlock_c19OnHitFromUnderneathER8dActor_c, 0x021496a4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjHatenaBlock_c19OnHitFromUnderneathER8dActor_c
/* daObjHatenaBlock_c::OnHitFromUnderneath -- vtable slot 28, ov102 0x021496a4.
 * include/dActor_c.h's own slot 28 supplies the signature.
 *
 * mVertAccel/mVertSpeed are dActor_c's own fields (include/dActor_c.h),
 * already named there -- this override resets the block's own vertical
 * physics to a fixed pop before running the shared combat-callback body
 * (OnGroundPounded's comment in this file carries the
 * mState/mHitterParam/helper notes). */
/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
int daObjHatenaBlock_c::OnHitFromUnderneath(dActor_c &other)
{
    if (mState != 1) {
        mVertAccel = -0x8000;
        mVertSpeed = 0x1e000;
        if (!func_ov102_02149078(this)) {
            mHitterParam = other.param1;
            func_ov102_02149da8((C *)this, 1);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov102_02149684, 0x02149684, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149684
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149684(int* dst, int* src){
  int v3 = src[0x19];
  int v2 = src[0x18] + 0x32000;
  dst[0] = src[0x17];
  dst[1] = v2;
  dst[2] = v3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov102_02149610, 0x02149610, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149610
extern "C" int func_ov102_02149610(char *c){
  V3 pos(*(int*)(c+0x5c), *(int*)(c+0x60)+0x28000, *(int*)(c+0x64));
  dBgCh_Gnd rg;
  rg.SetObjAndPos(*(Vector3*)&pos, 0);
  rg.mProbeHeight = 0x3e8000;
  int r = *(int*)(c+0x60);
  if (rg.DetectClsn()) r = rg.clsnY;
  return r;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov102_0214953c, 0x0214953c, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_0214953c
/* Named rather than anonymous. Vector3 has an inline destructor (types.h), so
 * this aggregate gets a compiler-generated one; at block scope mwccarm names
 * that class @class$<counter><file>_cpp, and the counter moves whenever
 * anything above it in the file moves. A file-scope name gives the same four
 * instructions a symbol that does not drift, which is what the compiler-only
 * allow-list in this TU's manifest has to spell exactly. */
struct HbSpawnFrame { Vector3 pos; int vel[3]; };

extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_0214953c(char* c, int p1, int p2)
{
    HbSpawnFrame f;
    int rnd;
    char* o;
    char* g;
    func_ov102_02149684((int*)&f.pos, (int *)c);
    o = (char*)dActor_c::Spawn(
        0x10d, (unsigned int)(p2 | (p1 << 8)), f.pos, 0, *(signed char*)(c + 0xcc), -1);
    if (o == 0) return;
    g = *(char**)data_0209f318;
    f.vel[0] = 0;
    f.vel[1] = 0x11000;
    f.vel[2] = 0;
    rnd = RandomIntInternal(&data_0209e650);
    {
        int vsum = rnd + (*(short*)(g + 0x17c) + 0x8000);
        *(short*)(o + 0x92) = 0;
        *(short*)(o + 0x94) = (short)vsum;
        *(short*)(o + 0x96) = 0;
        *(int*)(o + 0x98) = 0x3320;
        *(int*)(o + 0xa4) = f.vel[0];
        *(int*)(o + 0xa8) = f.vel[1];
        *(int*)(o + 0xac) = f.vel[2];
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov102_021494cc, 0x021494cc, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_021494cc
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov102_021494cc(char* c){
  int s[3];
  func_ov102_02149684(s, (int*)c);
  int r2 = (*(unsigned int*)(c+8) >> 8) & 0xff;
  if(r2 == 0xff) r2 = 1;
  int w[3];
  w[0] = s[0]; w[1] = s[1]; w[2] = s[2];
  func_ov102_02149100(c, (Vector3 *)w, r2, 0x1800, 0);
  return _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov102_02149478, 0x02149478, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149478(char* c){
  char local[12];
  func_ov102_02149684((int *)local, (int *)c);
  dActor_c::Spawn(0xb2, *(unsigned char*)(c+0x3f1)|0x40, *(Vector3 *)local, 0, *(signed char*)(c+0xcc), -1);
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov102_02149428, 0x02149428, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149428
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* The legacy one-function source named this parameter `this`, legal in C and
   reserved in C++. Renamed; a parameter name is not observable in the bytes. */
void func_ov102_02149428(char* c){
  struct Vector3 pos;
  func_ov102_02149684((int *)&pos, (int *)c);
  dActor_c::Spawn(0x114, 0, pos, 0, *(signed char*)(c+0xcc), -1);
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov102_021493dc, 0x021493dc, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_021493dc
/* recovered: shared common types */
extern "C" void func_ov102_021493dc(void* c) {
    Vector3 v;
    func_ov102_02149684((int *)&v, (int *)c);
    signed char cc = *(signed char*)((char*)c+0xcc);
    dActor_c::Spawn(0x115, 0, v, (const Vector3_16*)0, cc, -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov102_02149384, 0x02149384, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149384
/* recovered: shared common types */
extern "C" {

void* func_ov102_02149384(void* c){
  Vector3 v;
  func_ov102_02149684((int *)&v, (int *)c);
  void* a=dActor_c::Spawn(0x11d,0,v,0,*(signed char*)((char*)c+0xcc),-1);
  if(a) *(unsigned char*)((char*)a+0x3c6)=0xb4;
  return a;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov102_021492d4, 0x021492d4, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_021492d4
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_021492d4(char* c) {
  struct Vector3 pos;
  struct Vector3_16 rot;
  func_ov102_02149684((int*)&pos, (int*)c);
  rot.x = *(u16*)(data_020a0edc);
  rot.y = *(u16*)(data_020a0edc+2);
  rot.z = *(u16*)(data_020a0edc+4);
  if ((int)(*(unsigned char*)(&data_0209f2d8) == 1) != 0) {
    rot.y = *(s16*)(c+0x8e);
  } else {
    rot.y = *(s16*)(*(char**)data_0209f318 + 0x17c) + 0x4000;
  }
  dActor_c::Spawn(0x159, 0, pos, &rot, *(signed char*)(c+0xcc), -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov102_02149288, 0x02149288, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149288
/* recovered: shared common types */
extern "C" void func_ov102_02149288(char* c){
    Vector3 v;
    func_ov102_02149684((int *)&v, (int *)c);
    dActor_c::Spawn(0x132u, 0u, v, 0, *(signed char*)(c+0xcc), -1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov102_02149220, 0x02149220, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149220
/* recovered: shared common types */


extern "C" {
extern void* func_ov102_0214b384(void*, int);
void* func_ov102_02149220(char* c){
  Vector3 v;
  func_ov102_02149684((int *)&v, (int *)c);
  void* a = dActor_c::Spawn(0xce, 4, v, 0, *(signed char*)(c+0xcc), -1);
  if(a == 0) return a;
  func_ov102_0214ad14(a);
  return func_ov102_0214b384(a, 0x3c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov102_02149100, 0x02149100, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149100
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov102_02149100(char *c, Vector3 *pos, int n, unsigned int speed, short arg5)
{
    char *a;
    int dir;
    unsigned int rnd;
    unsigned int q;
    int prevDir = 0xff;
    int i = 0;

    if (n <= 0) return;

    do {
        a = (char *)dActor_c::Spawn(0x120, 2, *pos, 0, *(signed char*)(c+0xcc), -1);
        if (a != 0) {
            do {
                rnd = (unsigned int)RandomIntInternal(&data_0209e650);
                dir = (int)((rnd >> 0x10) << 0x1b) >> 0x10;
            } while (dir == prevDir);
            rnd = (unsigned int)RandomIntInternal(&data_0209e650);
            *(short*)(a+0x92) = 0;
            q = rnd >> 0x10;
            speed = (speed * (q % 50 + 100)) / 100;
            prevDir = dir;
            *(short*)(a+0x94) = arg5 + dir;
            *(short*)(a+0x96) = 0;
            *(unsigned int*)(a+0x98) = speed;
        }
        i++;
    } while (i < n);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov102_02149078, 0x02149078, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov102_02149078
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov102_02149078(dActor_c *self)
{
    if (data_0209f2f8 == 0x15) {
        if ((int)(*(int*)((char*)self + 0x3e0) - 0x32000) <= data_0209f32c)
            return 1;
    } else {
        if (data_0209f2f8 == 0x21)
            return 0;
        if (*(unsigned char*)((char*)self->ClosestPlayer() + 0x706))
            return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* Below this, at 0x02149010 and 0x02148fbc, the compiler emits D0 and D1 from
 * the header's inline destructor body. Nothing is written for them here.
 *
 * D1 destroys the object through this class and its bases: this class's vptr,
 * then dBgActor_c's -- inlined, because dBgActor_c's destructor is defined in
 * its own class body -- then dBgActor_c's Model and dBgW_KcMbg, then
 * dActor_c. This class adds no member with a destructor of its own. D0 is the
 * deleting variant: the same work, then an inlined operator delete, which is
 * why neither body names a heap.
 */
