/* HOST COPIES of two ov096 TORNADO (actor 308, daTor_c) sites MSVC cannot
 * deliver from the matched source, run rel0215 wave 2 lane cast-lvl16pair.
 * Both matched TUs stay byte-locked in src/ and are simply not referenced from
 * port/slice_ov096.txt. Pokey_HostSites.cpp header carries the long form of
 * the destructor derivation; only what is DIFFERENT about Tornado is written
 * out here.
 *
 * (1) src/_ZN7TornadoD1Ev.cpp -- THE C++-MANGLING WALL, the same shape as
 *     Pokey. The ROM says TornadoD1 makes exactly TornadoD0 calls in
 *     TornadoD0 order, minus the final Memory::Deallocate and its
 *     data_020a0eac argument (config/arm9/overlays/ov096/relocs.txt):
 *
 *       TornadoD1 0x02136db0..0x02136df8   TornadoD0 0x02136df8..0x02136e54
 *         0x02136dc4 -> 0x0201592c           0x02136e0c -> 0x0201592c
 *         0x02136dcc -> 0x0201691c           0x02136e14 -> 0x0201691c
 *         0x02136dd4 -> 0x020373f8           0x02136e1c -> 0x020373f8
 *         0x02136ddc -> 0x020149a4           0x02136e24 -> 0x020149a4
 *         0x02136de4 -> 0x020112c8           0x02136e2c -> 0x020112c8
 *         0x02136df4 (load) 0x02137a90       0x02136e3c -> 0x0203c1e8  <- only
 *                                            0x02136e4c (load) 0x02137a90
 *                                            0x02136e50 (load) 0x020a0eac
 *
 *     Resolved against src/_ZN7TornadoD0Ev.c own member list:
 *     TextureTransformer +0x328 (0x0201592c), ModelAnim +0x2c4 (0x0201691c),
 *     WithMeshClsn +0x108 (0x020373f8), MovingCylinderClsn +0xd4 (0x020149a4),
 *     then _ZN5ActorD2Ev (0x020112c8). THE LAST THREE ARE THE SAME THREE arm9
 *     ADDRESSES POKEY DESTRUCTOR USES for the same three member types, from a
 *     separately recovered destructor -- the cross-check that makes this a
 *     reading rather than a guess.
 *
 *     The load at 0x02136df4 reads 0x02137a90, the one Tornado table. src
 *     spells it two ways -- Tornado_Spawn says _ZTV7Tornado and D0 says
 *     _ZTV7daTor_c -- and port/ov096_syms.txt proves they are one table by
 *     reading its own vtable[-1] typeinfo through to the string "7daTor_c".
 *     This copy stores the host array _ZTV7Tornado; the /alternatename bridge
 *     in hal/actor_classes_ov096.cpp carries the other spelling to it.
 *
 * (2) src/_ZN7Tornado6RenderEv.cpp -- THE ModelAnim SLOT-5 COLLISION.
 *     hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC numbering, where
 *     slot 5 is Virtual18 (two arguments) and NOT Render, and it cannot be
 *     dual-filled because something real occupies the slot. The matched body
 *     is a TextureTransformer::Update over the model components at +0x2cc
 *     followed by `((Sub *)(c + 0x2c4))->g5(c + 0x80)` through a six-virtual
 *     local shadow that counts in the ROM numbering. +0x2c4 is a ModelAnim,
 *     named as one by src/_ZN7TornadoD1Ev.cpp own member list and by
 *     Tornado::InitResources, which calls ModelAnim::SetAnim on it. The
 *     Whomp/Fish/Butterfly/HootTheOwl/ov077 case.
 *
 *     POKEY RENDER IS THE OPPOSITE CASE AND IS NOT REFUSED. It is the same
 *     six-virtual shadow reaching index 5, but over a plain Model at +0xd4,
 *     which hal/cxxname_bridge.cpp:511 dual-fills for exactly this shape. Two
 *     Renders in one overlay, one refused and one not, entirely on which
 *     member they dispatch. See port/slice_ov096.txt.
 *
 *     The matched TU is already `extern "C"` and defines the flat name itself,
 *     so this copy needs no face beside it -- unlike the seven real C++
 *     methods that stay in the slice.
 */
#include "ModelAnim.h"

extern "C" {

/* ---- (1) the destructor ------------------------------------------------ */
extern int _ZTV7Tornado[];
void _ZN18TextureTransformerD1Ev(void *);    /* 0x0201592c, member +0x328 */
void _ZN9ModelAnimD1Ev(void *);              /* 0x0201691c, member +0x2c4 */
void _ZN12WithMeshClsnD1Ev(void *);          /* 0x020373f8, member +0x108 */
void _ZN18MovingCylinderClsnD1Ev(void *);    /* 0x020149a4, member +0xd4  */
void _ZN5ActorD2Ev(void *);                  /* 0x020112c8               */

/* PORT_HOST_ABI: mwcc Itanium D1 that MSVC emits no flat name for. */
int *_ZN7TornadoD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV7Tornado;
    _ZN18TextureTransformerD1Ev((char *)t + 0x328);
    _ZN9ModelAnimD1Ev((char *)t + 0x2c4);
    _ZN12WithMeshClsnD1Ev((char *)t + 0x108);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}

/* ---- (2) Render -------------------------------------------------------- */
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN7Tornado6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x328, c + 0x2cc);
    /* ((Sub *)(c + 0x2c4))->g5(c + 0x80) -- the ROM slot-5 Render, qualified */
    ((ModelAnim *)(c + 0x2c4))->ModelAnim::Render((const Vector3 *)(c + 0x80));
    return 1;
}

/* ---- (3) THE IMPLICIT r0 RIDE-THROUGH, and it is the dangerous one ------
 * src/_ZN7Tornado8BehaviorEv.cpp dispatches its three-way state switch as
 *     case 0: func_ov096_021372c0(); break;
 *     case 1: func_ov096_02137088(); break;
 *     case 2: func_ov096_02136fd4(); break;
 * -- with NO ARGUMENTS, while all three bodies take a receiver:
 *     src/func_ov096_021372c0.c   void func_ov096_021372c0(void *self)
 *     src/func_ov096_02137088.cpp void func_ov096_02137088(char *c)
 *     src/func_ov096_02136fd4.c   void func_ov096_02136fd4(Actor *thiz)
 * It compiles clean because include/decl_common.h:2927-2929 declares all three
 * `extern void func_ov096_...(void);` -- the decl_common redecl trap.
 *
 * THE ROM PROVES THE RECEIVER RIDES IN r0, and this was found by READING
 * rather than by the first boot (ov077's Lakitu case was found by a crash on
 * four levels). Tornado::Behavior's own prologue at 0x02137448:
 *     e92d4030  push {r4,r5,lr}
 *     e24dd004  sub  sp,sp,#4
 *     e1a04000  mov  r4,r0          <- r4 = this, and r0 STILL holds this
 *     e594535c  ldr  r5,[r4,#0x35c]
 *     e3550000  cmp  r5,#0
 *     0a000004  beq  0x02137474
 *     ...
 *     ebffff91  bl   0x021372c0     <- 0x02137474, r0 never reloaded
 *     ebffff01  bl   0x02137088     <- 0x0213747c, r0 never reloaded
 *     ebfffed2  bl   0x02136fd4     <- 0x02137484, r0 never reloaded
 * Nothing between the prologue and the three branch targets writes r0, so each
 * callee reads `this` straight out of the argument register the caller was
 * entered with. On the host MSVC emits three zero-argument cdecl calls and
 * each callee reads its receiver off an uninitialised stack slot -- a garbage
 * pointer dereferenced on TORNADO's first frame, on the one level that places
 * it.
 *
 * THE CALLER IS DISPLACED, not the callees: the Ov007_ImplicitR0 /
 * Actor_ClosestPlayer / ov077-Lakitu idiom. All three callees are CORRECT and
 * stay in port/slice_ov096.txt; only this one caller moves, and the whole
 * lane's exposure to the trap is this single body -- swept mechanically over
 * every ov092 and ov096 symbol decl_common.h declares, comparing the header's
 * arity against each body's own. ov092 has ZERO such mismatches; ov096 has
 * exactly these three, and all three are called from here and nowhere else.
 *
 * Every other statement is transcribed line for line from the matched body
 * with Tornado.h's own member names. Only the three call sites are respelled.
 */
void func_ov096_021372c0(void *self);
void func_ov096_02137088(void *self);
void func_ov096_02136fd4(void *self);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void *_ZN5Actor18ClosestWithActorIDEj(void *actor, unsigned int id);
int Vec3_Dist(void *a, void *b);
int func_ov002_020de33c(void *a, int b);
void _ZN12CylinderClsn5ClearEv(void *thiz);
void _ZN12CylinderClsn6UpdateEv(void *thiz);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN9Animation7AdvanceEv(void *thiz);

/* PORT_HOST_ABI: the ROM carries this method's receiver through r0 into three
   callees the source calls with no arguments. */
int _ZN7Tornado8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    int s = *(int *)(c + 0x35c);          /* unk_35c */
    switch (s) {
    case 0: func_ov096_021372c0(c); break;
    case 1: func_ov096_02137088(c); break;
    case 2: func_ov096_02136fd4(c); break;
    }
    {
        unsigned short *p = (unsigned short *)(c + 0x350);
        *p = (unsigned short)(*p + 1);
    }
    if (s != *(int *)(c + 0x35c)) {
        *(unsigned short *)(c + 0x350) = 0;   /* unk_350 */
        *(unsigned char *)(c + 0x360) = 0;    /* unk_360 */
    }
    {
        unsigned int id = *(unsigned int *)(c + 0x0f8);   /* unk_0f8 */
        if (id != 0 && (*(int *)(c + 0x0f4) & 0x400000) != 0) {
            void *o = _ZN5Actor10FindWithIDEj(id);
            if (o != 0) {
                void *closest = _ZN5Actor18ClosestWithActorIDEj(c, 0x135);
                if (closest == 0 ||
                    Vec3_Dist((char *)o + 0x5c, (char *)closest + 0x5c) > 0x118000) {
                    if (func_ov002_020de33c((char *)o, (int)(size_t)c) != 0)
                        *(void **)(c + 0x33c) = o;     /* unk_33c */
                }
            }
        }
    }
    _ZN12CylinderClsn5ClearEv(c + 0xd4);      /* mMovingCylinderClsn */
    _ZN12CylinderClsn6UpdateEv(c + 0xd4);
    Matrix4x3_FromTranslation(c + 0x2e0,
                              *(int *)(c + 0x5c) >> 3,
                              *(int *)(c + 0x60) >> 3,
                              *(int *)(c + 0x64) >> 3);
    _ZN9Animation7AdvanceEv(c + 0x314);       /* mAnimation */
    _ZN9Animation7AdvanceEv(c + 0x328);       /* mTextureTransformer */
    return 1;
}

/* ---- (4) THE decl_common REDECL TRAP, a COMPILE failure ----------------
 * src/func_ov096_02137088.cpp declares its own body `void
 * func_ov096_02137088(char *c);` inside an `extern "C"` block AND includes
 * include/decl_common.h, which declares the same symbol
 * `extern void func_ov096_02137088(void);` at line 2928. Two declarations of
 * one C-linkage name with different signatures in one translation unit:
 *     error C2733: 'func_ov096_02137088': you cannot overload a function with
 *                  'extern "C"' linkage
 * mwccarm accepts it. This is a COMPILE failure and a defect in an
 * AUTO-GENERATED header (tools/decl_headers.py), not an ABI wall -- the same
 * wrong `(void)` declaration that hides the ride-through above.
 *
 * WHY A HOST COPY. The two cheap routes both fail:
 *   * a per-source -D cannot rename only the header's occurrence, and renaming
 *     both leaves the same two signatures under a new name;
 *   * -DDECL_COMMON_H would neutralise the header, but this body needs
 *     func_ov002_020de328 and func_ov096_02136e54 FROM it and declares neither
 *     itself, so the TU would stop compiling for a second reason.
 * The real fix is decomp-side, in the header generator or its config -- the
 * known "decl_common redecl trap" class -- and is outside a cast-seating
 * lane's scope. Correcting that one line retires this copy and returns the TU.
 *
 * Every line below is the matched body verbatim; only the two declarations the
 * header used to supply are restated locally.
 */
struct PortOv096Vec3 { int x, y, z; };

short Vec3_HorzAngle(const void *a, const void *b);
int Vec3_HorzDist(const void *a, const void *b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned cc,
                                           const void *v, unsigned e);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void _Z14ApproachLinearRsss(short *v, short target, short step);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void WithMeshClsn_UpdateContinuous_Veneer(void *p);
int _ZNK12WithMeshClsn8IsOnWallEv(void *p);
void *_ZNK12WithMeshClsn13GetWallResultEv(void *p);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, int *out);
short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned a, unsigned b, int x, int y, int z, const void *v, void *cb);
/* the two decl_common.h used to supply */
int func_ov002_020de328(void *p);
void func_ov096_02136e54(void *self, int n);

/* PORT_HOST_ABI: a decl_common.h redeclaration MSVC refuses to compile. */
void func_ov096_02137088(void *cv)
{
    char *c = (char *)cv;
    PortOv096Vec3 pos;
    char *player;
    void *p33c;
    short ang;
    unsigned short *p354 = (unsigned short *)(c + 0x354);
    *p354 = (unsigned short)(*p354 + 1);

    ang = Vec3_HorzAngle(c + 0x5c, c + 0x340);
    *(short *)(c + 0x356) = ang;

    *(unsigned *)(c + 0x36c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned *)(c + 0x36c), 3, 0x85, c + 0x74, 0);

    *(int *)(c + 0x98) = 0x14000;

    player = (char *)_ZN5Actor13ClosestPlayerEv(c);
    if (player == 0) {
        *(int *)(c + 0x35c) = 2;
        return;
    }

    {
        int *pp = (int *)(player + 0x5c);
        pos.x = pp[0];
        pos.y = pp[1];
        pos.z = pp[2];
    }

    if (Vec3_HorzDist(c + 0x340, &pos) < *(int *)(c + 0x34c)
        && *(unsigned char *)(c + 0x360) == 0
        && *(unsigned short *)(c + 0x354) < 0x384) {
        ang = Vec3_HorzAngle(c + 0x5c, &pos);
        *(short *)(c + 0x358) = ang;
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x358), 0x200);
        p33c = *(void **)(c + 0x33c);
        if (p33c != 0) {
            if (func_ov002_020de328(p33c) != 0) {
                unsigned char *pb = (unsigned char *)(c + 0x360);
                *pb = (unsigned char)(*pb + 1);
            }
        }
    } else {
        _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x356), 0x200);
        if (Vec3_HorzDist(c + 0x340, c + 0x5c) < 0xc8000)
            *(int *)(c + 0x35c) = 2;
    }

    if (Vec3_Dist(c + 0x5c, &pos) > 0xbb8000
        || *(unsigned short *)(c + 0x354) >= 0x384) {
        *(int *)(c + 0x35c) = 2;
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    WithMeshClsn_UpdateContinuous_Veneer(c + 0x108);
    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x108) != 0) {
        int n[3];
        void *wr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x108);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)wr + 4, n);
        *(short *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(n[0], n[2]);
    }

    func_ov096_02136e54(c, 0x1000);

    {
        int z = *(int *)(c + 0x64);
        *(unsigned *)(c + 0x364) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned *)(c + 0x364), 0x11f,
                *(int *)(c + 0x5c), *(int *)(c + 0x60), z, 0, 0);
        z = *(int *)(c + 0x64);
        *(unsigned *)(c + 0x368) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned *)(c + 0x368), 0x120,
                *(int *)(c + 0x5c), *(int *)(c + 0x60), z, 0, 0);
    }
}

/* ---- (5) THE WRONG-NAMED LEVEL-DATA REFERENCE -------------------------
 * src/_ZN7Tornado13InitResourcesEv.cpp hands its texture animation descriptor
 * to TextureTransformer twice, spelled `func_02112968`:
 *     TextureTransformer::Prepare(data_ov096_02137ba8[1], func_02112968);
 *     TextureTransformer::SetFile(this + 0x328, func_02112968, 0, 0x1000, 0);
 * and include/decl_common.h:1380 declares it `extern int func_02112968[];`.
 * THERE IS NO func_02112968 ANYWHERE IN config/ -- the name is a recovery
 * artifact. 0x02112968 is LEVEL-OVERLAY data, and ov096's own relocation says
 * so and says how badly:
 *     from:0x021376b0 kind:load to:0x02112968
 *         module:overlays(10,18,22,24,25,26,32,35,45,56)
 * TEN level overlays have a symbol at that address, every one of them marked
 * `ambiguous`, because on the DS the tornado reads its descriptor out of
 * WHICHEVER LEVEL OVERLAY IS RESIDENT. The ov017 ShipWater trap
 * (data_ov055_02111a94 / data_ov056_02111a60) at ten-way scale.
 *
 * THE BYTES SAY IT IS A FILE DESCRIPTOR, and the identification is by SHAPE
 * against a known one rather than by the name. ov024's image at 0x02112968:
 *     00000028 021118b4 021118ac 02111948 00000002 021118bc
 * and ov017's descriptor at 0x02111a60, which the ov017 lane identified as
 * "the six-word file descriptor":
 *     0000012c 021114bc 021114b8 021114dc 00000001 021114c0
 * Same six-word shape, same [count][ptr][ptr][ptr][small][ptr] layout, every
 * pointer into its own overlay's .rodata.
 *
 * WHY port_ov024_at AND NOT A PER-SYMBOL MOUNT. The descriptor carries FOUR
 * relocated pointers into ov024's .rodata, none of which port/ov024_syms.txt
 * mounts (that list is a hand-picked seven), so adding data_ov024_02112968 to
 * it would host the record with four raw DS addresses in it -- the T2 shape,
 * and TextureTransformer would read a file header out of unmapped memory.
 * ov024 is ALSO whole-mounted in PORT_LEVEL_OVERLAYS (port/CMakeLists.txt:1436)
 * and `--whole` rebases the image's internal pointers, so the correctly-fixed
 * copy already exists in this build; port_ov024_at(ds) is the accessor
 * hal/level_boot.cpp already uses for exactly this. Nothing is added to
 * another lane's mount list and nothing new is hosted.
 *
 * THE LIMITATION, STATED RATHER THAN HIDDEN. Binding to ov024 is correct for
 * LEVEL 16, the only level this tree's census places TORNADO on. On the DS the
 * address would resolve to whatever level is loaded; here it always resolves
 * to Shifting Sand Land's. If a later lane finds actor 308 placed on any of
 * the other nine levels (2, 10, 14, 17, 18, 24, 27, 37, 48), this call needs a
 * level-keyed dispatch over the port_ovNNN_at family instead of one binding,
 * and this comment is the record of why.
 *
 * Every other statement is transcribed line for line from the matched body.
 */
void *port_ov024_at(unsigned ds);
int _ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
int _ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *f, int a,
                                                 int fix, unsigned u);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *a, void *b);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *thiz, void *f,
                                                           int a, int fix,
                                                           unsigned u);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor,
                                                        int a, int b,
                                                        unsigned c, unsigned d);
void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *thiz, void *actor, int a, int b, void *v, int e);
void func_02016aac(void *self, unsigned value, int index);
extern void *data_ov096_02137ba8[];
extern void *data_ov096_02137bb0[];

/* PORT_HOST_ABI: a recovery-artifact name for level-overlay data, reached
   through the whole mount's rebased image instead. */
int _ZN7Tornado13InitResourcesEv(void *selfv)
{
    char *c = (char *)selfv;
    void *bta = port_ov024_at(0x02112968);

    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0x2c4,
        (void *)(size_t)_ZN5Model8LoadFileER13SharedFilePtr(data_ov096_02137ba8),
        1, 0x15);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov096_02137bb0);
    func_02016aac(c + 0x2c4, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x2c4,
                                                data_ov096_02137bb0[1],
                                                0, 0x1000, 0);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(
        data_ov096_02137ba8[1], bta);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x328, bta,
                                                          0, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0xd4, c, 0, 0,
                                                       0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        c + 0x108, c, 0x50000, 0x50000, 0, 0);

    *(int *)(c + 0x340) = *(int *)(c + 0x5c);   /* unk_340 = mPosX */
    *(int *)(c + 0x344) = *(int *)(c + 0x60);   /* unk_344 = mPosY */
    *(int *)(c + 0x348) = *(int *)(c + 0x64);   /* unk_348 = mPosZ */
    *(short *)(c + 0x35a) = 0;
    *(int *)(c + 0x35c) = 0;
    *(unsigned char *)(c + 0x360) = 0;
    *(int *)(c + 0x09c) = -0x1000;
    *(int *)(c + 0x0a0) = -0x1e000;
    {
        unsigned int t = (unsigned int)(*(int *)(c + 8)) & 0xff;   /* mParam */
        *(int *)(c + 0x34c) = (t == 0xff) ? 0x5dc000 : (int)(t * 0x64000);
    }
    *(unsigned *)(c + 0x364) = 0;
    *(unsigned *)(c + 0x368) = 0;
    return 1;
}

}  /* extern "C" */
