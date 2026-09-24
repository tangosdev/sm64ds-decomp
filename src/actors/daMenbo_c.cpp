//cpp
/* daMenbo_c -- the skeeter, the water strider that skims the lake surface.
 * ov090 0x02130f00..0x02132654, twenty-four functions, one translation unit.
 *
 * ov090 is the water-creature overlay: this class, daManta_c, daPukupuku_c and
 * daShark_c sit in that order, each a contiguous run of its own. daMenbo_c owns
 * the first run, which starts at the overlay's own .text edge; the run that
 * follows opens with _ZN9daManta_cD1Ev at 0x0213269c. No foreign symbol falls
 * inside this span.
 *
 * CLASS IDENTITY IS THE ROM'S OWN, not a coined name. _ZTS9daMenbo_c at
 * 0x02134120 holds exactly "9daMenbo_c" NUL-terminated and padded to twelve
 * bytes, so the receipt is dated to this spelling. _ZTI9daMenbo_c at 0x02134138
 * is an abi __si_class_type_info record. Its first word points to the shared
 * _ZTVN3abi20__si_class_type_infoE at arm9 0x0209a764, its second to
 * _ZTS9daMenbo_c, and its third to _ZTI12dEnemyBase_c at ov002 0x021081c0,
 * which proves the base. The vtable symbol _ZTV9daMenbo_c
 * at 0x02134168 points past the usual two-word header: the word at -8 is the
 * zero offset-to-top and the word at -4 relocates to _ZTI9daMenbo_c. Thirty-one
 * slots follow. Ten target this run: 0, 3, 6, 9, 12, 16, 17, 18, 19 and 29.
 * Slots 16 and 17 are the D1 and D0 pair at 0x02130f00 and 0x02130f40;
 * slots 18 and 19 target 0x0213264c and 0x02132620.
 *
 * THIS TU OWNS THE CLASS VTABLE. The destructor is the first virtual daMenbo_c
 * declares and it is written out of line here, which makes it the key function
 * and anchors _ZTV9daMenbo_c, _ZTI9daMenbo_c and _ZTS9daMenbo_c to this
 * compiler input. That is why nothing below declares an extern for the vtable
 * and nothing stores a vptr by hand: the compiler emits the table and the
 * destructors it points at. The factory that builds a skeeter,
 * daMenbo_c_classInit at 0x02132654, sits just past this run and stays in its
 * own file, exactly as daBDonketu_c and daBtfly_c left theirs.
 *
 * THE PRAGMA IS REAL AND MEASURED, AND THE SENSE OF THE BRACKET IS INVERTED.
 * The legacy shard for func_ov090_02130f94 carried two file-global pragma
 * lines, and both are load-bearing. Delete-outright control against the
 * cartridge at 0x02130f94, size 0x120, one shard at a time under the pinned
 * 2004 b56 compiler:
 *
 *     both present                        -> MATCH
 *     drop opt_strength_reduction off     -> size 0x130, no match
 *     drop opt_common_subs off            -> size 0x11c, no match
 *     drop both                           -> size 0x12c, no match
 *
 * Neither is inert. Carrying them into the merged input then needed a second
 * measurement, because a bracket that wraps only that one member does NOT give
 * it their setting -- tried four ways (an `off` and `on` pair around the
 * member, a push and pop pair around it, the same with and without a forward
 * declaration ahead of the bracket, and a span reaching from the top of the
 * file to the end of that member) and all four leave it DIFF at 999 words
 * while the other twenty-three match. What does work is the other direction:
 * hold both settings off for the whole file, and bracket the members that want
 * one back ON at their own definitions. Only two members want anything back,
 * and both want the same one:
 *
 *     whole file off, no ON bracket   -> 22 of 24; func_ov090_02131378 off by
 *                                        4 words and Behavior off by 5
 *     ON bracket, strength_reduction  -> still 22 of 24, the same two
 *     ON bracket, common_subs         -> 24 of 24
 *
 * So opt_strength_reduction stays off for every member in the file, and
 * opt_common_subs is off except inside the two brackets below. A positional ON
 * bracket is measurably live in this direction: putting one around Behavior by
 * itself moved Behavior by 5 words and nothing else.
 *
 * The defer_codegen pragma is what makes a positional bracket possible at all,
 * and it is live here on its own control: deleting that one line leaves the
 * bytes alone but turns emission into 23 ordinal pairs out of ROM order. It
 * also flips .text emission from reverse-source to source order, which is why
 * this file is written ROM-ascending rather than ROM-descending.
 *
 * Function bodies are the shards' own, unchanged except where several of them
 * carried private stand-ins for the same ROM symbol and only one spelling can
 * survive in a single compiler input. What that cost is written up under
 * "reconciled declarations" below.
 *
 * ROM order, which is also source order here:
 *   [0]  0x02130f00  _ZN9daMenbo_cD1Ev
 *   [1]  0x02130f40  _ZN9daMenbo_cD0Ev
 *   [2]  0x02130f94  func_ov090_02130f94        <- wants both pragmas off
 *   [3]  0x021310b4  func_ov090_021310b4
 *   [4]  0x02131378  func_ov090_02131378        <- ON bracket
 *   [5]  0x021314a0  func_ov090_021314a0
 *   [6]  0x02131584  func_ov090_02131584
 *   [7]  0x02131608  func_ov090_02131608
 *   [8]  0x02131648  func_ov090_02131648
 *   [9]  0x02131a74  func_ov090_02131a74
 *   [10] 0x02131ac4  func_ov090_02131ac4
 *   [11] 0x02131b94  func_ov090_02131b94
 *   [12] 0x02131c48  func_ov090_02131c48
 *   [13] 0x02131db0  func_ov090_02131db0
 *   [14] 0x02131e00  func_ov090_02131e00
 *   [15] 0x02131e50  func_ov090_02131e50
 *   [16] 0x02131edc  _ZN9daMenbo_c16CleanupResourcesEv
 *   [17] 0x02131f30  _ZN9daMenbo_c16OnPendingDestroyEv
 *   [18] 0x02131f34  _ZN9daMenbo_c6RenderEv
 *   [19] 0x02131f88  _ZN9daMenbo_c8BehaviorEv   <- ON bracket
 *   [20] 0x0213235c  _ZN9daMenbo_c13InitResourcesEv
 *   [21] 0x02132618  _ZN9daMenbo_c16OnAimedAtWithEggEv
 *   [22] 0x02132620  _ZN9daMenbo_c13OnTurnIntoEggER6Player
 *   [23] 0x0213264c  _ZN9daMenbo_c13OnYoshiTryEatEv
 */

#pragma defer_codegen off

/* common.h FIRST, and that is measured. Both spellings of Matrix4x3 share one
 * guard, so whichever header is seen first wins: common.h has the flat twelve
 * words, and the math Matrix header that daMenbo_c.h reaches through ModelAnim
 * has the nested rotation-and-translation pair. The two members that copy that
 * type whole, func_ov090_02130f94 and func_ov090_02131e50, were matched under
 * the flat spelling and only reproduce under it -- with daMenbo_c.h first,
 * func_ov090_02131e50 DIFFs. Neither reads a field by name, so the flat
 * spelling costs the rest of the file nothing. */
#include "common.h"
#include "daMenbo_c.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* FILE DEFAULT. func_ov090_02130f94 at ROM ordinal 2 is the only member that
 * needs either of these, and it needs both. Every other member reproduces with
 * opt_strength_reduction off, so that one stays off for the whole file; exactly
 * two members want common subexpressions back, and each is bracketed at its own
 * definition below. */
#pragma opt_strength_reduction off
#pragma opt_common_subs off


/* ---------------------------------------------------------------------------
 * Local stand-ins with no header of their own.
 * ------------------------------------------------------------------------ */

struct BMD_File;
struct BCA_File;

/* The state machine. mState at 0x370 holds a pointer to a table node whose
 * first entry is the pointer-to-member the actor enters through; Behavior
 * reaches the same node's second entry at +8. The class member is typed void*,
 * so the two functions that dereference the node keep this stand-in rather than
 * retyping the header. */
struct MenboState;
typedef int (MenboState::*MenboStateFn)();
struct MenboState { char pad[0x370]; MenboStateFn* mState; };

/* Render draws through mModelAnim's own vtable, slot 5. */
struct RenderArg;
struct ModelAnimVt {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void Draw(RenderArg*);
};
struct MenboModelView { char pad[0xb0]; unsigned int mFlags; char pad2[0x30c - 0xb4]; ModelAnimVt mModelAnim; };

/* Behavior runs the state node's second entry through a pointer-to-member. */
class MenboSelf {};
typedef void (MenboSelf::*MenboSelfFn)();
struct MenboStateNode { char pad[8]; MenboSelfFn mRun; };

struct CoinVec3 { s32 x, y, z; };

/* The by-value Fix12 signatures below are the ROM's own; mwccarm passes the
 * class template differently from a bare word, so these calls keep the
 * plain-word spelling the shards matched under. */
typedef int LocFix12;

/* ---------------------------------------------------------------------------
 * Reconciled declarations.
 *
 * Several ROM symbols arrived from the shards under more than one spelling, and
 * a single compiler input can only carry one of each. Each one below is the
 * spelling every call site in this file now uses:
 *
 *   func_02012694          took (int, void*) in two shards and
 *                          (unsigned, const Vector3*) in a third
 *   func_ov090_02131e00    took (char*, void*), (void*, void*),
 *                          (void*, const void*) and a member-pointer form
 *   ModelAnim SetAnim      took BCA_File* in three shards and an unprototyped
 *                          form in a fourth, whose second argument is an int
 *   the five file handles  were SharedFilePtr in CleanupResources and three
 *                          different two-word stand-ins elsewhere; they are
 *                          SharedFilePtr here and the animation pointer is read
 *                          at +4, because SharedFilePtr.h carries no fields
 *   Vec3_Dist, Vec3_HorzAngle, dActor_c PoofDust, dActor_c SpawnCoins and
 *   dActor_c KillAndTrackInDeathTable each had a char* form and a void* form
 * ------------------------------------------------------------------------ */

extern "C" {

/* arm9 math and utility */
void  MulMat4x3Mat4x3(Matrix4x3* a, Matrix4x3* b, Matrix4x3* out);
void  MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* out);
void  Matrix4x3_FromRotationY(void* m, s16 angle);
void  Matrix4x3_FromTranslation(void* m, int x, int y, int z);
void  Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
void  Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, s16 rx, s16 ry, s16 rz);
void  Vec3_Lsl(Vector3* out, const Vector3* in, int n);
void  Vec3_Asr(void* dst, void* src, int n);
int   Vec3_Dist(const Vector3* a, const Vector3* b);
int   Vec3_HorzDist(const Vector3* a, const Vector3* b);
s16   Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
int   AngleDiff(int a, int b);
int   ApproachAngle(s16* cur, int target, int divisor, int band, int maxStep);
void  _Z14ApproachLinearRsss(s16* cur, s16 target, s16 step);
void  _Z14ApproachLinearRiii(int& cur, int target, int step);
u16   DecIfAbove0_Short(u16* p);
int   RandomIntInternal(int* seed);
void  func_02012694(int id, const Vector3* pos);

/* arm9 engine */
int   _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
          unsigned int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);
void* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
char* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr& f);
void  _ZN9Animation7AdvanceEv(void* self);
int   _ZN9Animation8FinishedEv(void* self);
int   _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, LocFix12 rate, unsigned int n);

/* arm9 collision and actor services */
void  _ZN5dCc_c5ClearEv(void* self);
void  _ZN5dCc_c6UpdateEv(void* self);
void  _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, void* v);
void  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void* self, dActor_c* a, const Vector3* v, LocFix12 r, LocFix12 h, unsigned int e, unsigned int g);
void  _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, int a, LocFix12 r, LocFix12 h, int p, int q);
int   _ZNK10dBgCh_Actr8IsOnWallEv(void* self);
int   SurfaceInfo_TestFlag0x20(const SurfaceInfo* p);
void  func_0203558c(void* self);
int   func_02035638(u8* p);
void  func_02035684(int* p, int v);

void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void* _ZN8dActor_c13ClosestPlayerEv(void* self);
void* _ZN8dActor_c22ClosestNonVanishPlayerEv(void* self);
int   _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
int   _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* self, void* cyl, void* player);
void  _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* cyl);
void  _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const void* v, u32 n, s32 fix, s16 s);
void  _ZN8dActor_c8PoofDustEv(void* self);
void  _ZN8dActor_c24KillAndTrackInDeathTableEv(void* self);

/* ov002 -- the shared enemy base and the player */
int   _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(void* self, void* wm);
int   _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(void* self, void* wm, void* anim, u32 n);
void  _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* self, void* wm, u32 n);
int   _ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(void* self, void* wm);
void  _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* self, void* v, void* player, LocFix12 a);
void  func_ov002_020aea30(void* self, void* actor, void* collision);
void  _ZN6Player6BounceE5Fix12IiE(void* p, LocFix12 f);
int   _ZN6Player9IsOnShellEv(void* p);
void  _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, int a, LocFix12 b, int d, int e, int f);

/* arm9 and ov090 data */
extern Matrix4x3     data_020a0e68;
extern int           data_02092138;
extern int           data_0209e650;
extern unsigned char data_0209f2d8;
extern signed char   data_0209f2f8;
extern int           data_0209f32c;

extern Vector3       data_ov090_0213412c;
extern SharedFilePtr data_ov090_02134480;
extern SharedFilePtr data_ov090_02134488;
extern SharedFilePtr data_ov090_02134490;
extern SharedFilePtr data_ov090_02134498;
extern SharedFilePtr data_ov090_021344a0;
/* The four state-table nodes. Spelled char, not MenboStateFn*, because
 * decl_common.h already declares data_ov090_02134504 that way and one
 * translation unit cannot contradict a shared header; the other three follow
 * the same convention so the four read alike. Each call site casts. */
extern char          data_ov090_021344e4;
extern char          data_ov090_021344f4;
extern char          data_ov090_02134504;
extern char          data_ov090_02134514;

/* This TU's own free helpers, forward-declared so the ROM-ascending bodies
 * below can call each other in either direction. func_ov090_02130f94 is not
 * listed because its only caller, func_ov090_02131378, is defined after it; it
 * carries its own `extern "C"` at the definition instead. */
void func_ov090_021310b4(char* c);
void func_ov090_02131378(char* c);
int  func_ov090_021314a0(char* c);
int  func_ov090_02131584(char* c);
int  func_ov090_02131608(char* c);
int  func_ov090_02131648(MenboState* c);
int  func_ov090_02131a74(char* c);
int  func_ov090_02131ac4(char* c);
int  func_ov090_02131b94(char* c);
int  func_ov090_02131c48(char* c);
int  func_ov090_02131db0(char* c);
int  func_ov090_02131e00(MenboState* c, MenboStateFn* p);
void func_ov090_02131e50(char* c);

}

/* The animation handle each SharedFilePtr carries sits one word in.
 * SharedFilePtr.h deliberately declares no fields, so the four SetAnim calls
 * read it through the address. */
#define MENBO_BCA(handle) (((BCA_File**)&(handle))[1])

/* One written destructor; the compiler emits both variants, D1 (0x02130f00)
 * and D0 (0x02130f40), and the whole body of each. */
// @symbol _ZN9daMenbo_cD1Ev
// @symbol _ZN9daMenbo_cD0Ev
/* One vtable store and four destructor calls, every one a consequence of
 * `struct daMenbo_c : dEnemyBase_c` and the members that declaration types:
 * its own vptr, then ModelAnim at 0x30c, dBgCh_Actr at 0x150 and dCcAcPos_c at
 * 0x110 in reverse declaration order, then dEnemyBase_c's own destructor. D0 is
 * the deleting variant -- destroy through this class and its bases, then return
 * the object to its heap through dEnemyBase_c's inline operator delete, which
 * is why neither body mentions a heap.
 *
 * This pair is also the class's key function, so writing it out of line here is
 * what anchors the vtable and the RTTI records to this translation unit. */
daMenbo_c::~daMenbo_c()
{
}

/* THE PRAGMA BRACKET. Both lines are measured live against the cartridge; see
 * the file header for the four-way control. The defer_codegen pragma at the top
 * of the file is what makes them bind to this member alone -- without it they
 * would go file-global last-wins and recompile the other twenty-three. */
// @symbol func_ov090_02130f94
/* The four leg ripples. Each iteration copies the actor matrix into scratch at
 * data_020a0e68, multiplies the selected model matrix by scratch into scratch,
 * then extracts the transformed position and spawns a ripple at the water
 * line held in 0x3ac. The model matrix array is an input, not the destination. */
extern "C" void func_ov090_02130f94(char* c_)
{
    int zero;
    int sh;
    char* c;
    int i;
    Matrix4x3* src;
    int idx;
    int b;
    unsigned int id;

    c = c_;
    b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b != 0) return;

    src = (Matrix4x3*)(c + 0x328);
    i = 0;
    idx = 6;
    zero = 0;
    sh = 3;
    id = 0xea;
    for (; i < 4; ++i) {
        Vector3 v;
        Vector3 r;

        data_020a0e68 = *src;
        MulMat4x3Mat4x3((Matrix4x3*)*(void**)(c + 0x320) + idx, &data_020a0e68, &data_020a0e68);
        v.x = *(int*)((char*)&data_020a0e68 + 0x24);
        v.y = *(int*)((char*)&data_020a0e68 + 0x28);
        v.z = *(int*)((char*)&data_020a0e68 + 0x2c);
        Vec3_Lsl(&r, &v, sh);
        v.x = r.x;
        v.y = r.y;
        v.z = r.z;
        v.y = *(int*)(c + 0x3ac);
        *(unsigned int*)(c + i * 4 + 0x380) =
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int*)(c + i * 4 + 0x380), id, v.x, v.y, r.z, zero, zero);
        idx += 3;
    }
}

// @symbol func_ov090_021310b4
/* The whole contact response, in the order the cartridge tests it. Keys held
 * from the shard: the volatile source pointer pins the offset table copy to
 * three loads and a batched store rather than an ldm; the same-type u32 reads
 * let the 0x134 load feed FindWithID; and the ternary that materialises the
 * 0xbf test keeps the moveq-movne pair instead of jump-threading. */
void func_ov090_021310b4(char* c)
{
    short hv[3];
    Vector3 sv;
    Vector3 cv;
    Vector3 hurt;
    int flags;
    char* p;
    int x, y, z;
    volatile Vector3* src = &data_ov090_0213412c;

    x = src->x;
    y = src->y;
    z = src->z;
    sv.z = z;
    sv.x = x;
    sv.y = y;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x110, &sv);

    if (*(unsigned int*)(c + 0x134) == 0) return;
    if ((p = (char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x134))) == 0) return;
    flags = (int)(((long long)*(int*)(c + 0x130)));

    if (flags & 0x2400) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x40) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x9c) = 0;
        *(int*)(c + 0xa4) = 0;
        *(int*)(c + 0xa8) = 0;
        *(int*)(c + 0xac) = 0;
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    if (flags & 0x10) {
        hv[0] = 0x1000;
        hv[1] = 0;
        hv[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, hv, p, 0);
        func_02012694(0x1d, (const Vector3*)(c + 0x74));
        return;
    }
    if (flags & 0x4380) {
        *(unsigned char*)(c + 0x3a1) = 3;
        *(int*)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0);
        return;
    }
    {
        int b = (*(unsigned short*)(p + 0xc) == 0xbf) ? 1 : 0;
        if ((int)(((long long)b)) == 0) return;
    }
    if (*(unsigned char*)(p + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(p) == 1) {
        cv.x = *(int*)(c + 0x5c);
        cv.y = *(int*)(c + 0x60);
        cv.z = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &cv, *(unsigned char*)(c + 0x10a) + 1, 0xa000, 0);
        _ZN8dActor_c8PoofDustEv(c);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        return;
    }
    hurt.x = *(int*)(c + 0x5c);
    hurt.y = *(int*)(c + 0x60);
    hurt.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &hurt, 2, 0xc000, 1, 0, 1);
}

#pragma opt_common_subs on
// @symbol func_ov090_02131378
/* Finds the surface under the skeeter and clamps it up onto that surface. One
 * level and mode pair skips the probe and goes straight to the ripples. */
void func_ov090_02131378(char* c)
{
    int b = (int)(data_0209f2d8 == 2);
    if (b != 0) {
        if (data_0209f2f8 == 0x12) {
            func_ov090_02130f94(c);
            return;
        }
    }
    dBgCh_Gnd rg;
    Vector3 v;
    int vz = *(int*)(c + 0x64);
    int vy = *(int*)(c + 0x60) + 0x32000;
    v.x = *(int*)(c + 0x5c);
    v.y = vy;
    v.z = vz;
    *(unsigned char*)(c + 0x39c) = 0;
    if (Vec3_HorzDist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x68)) != 0) {
        rg.SetObjAndPos(v, (dActor_c*)c);
        if (rg.DetectClsn() != 0)
            *(int*)(c + 0x3a8) = rg.clsnY;
    }
    if (*(int*)(c + 0x3a8) < data_0209f32c) {
        *(int*)(c + 0x3ac) = data_0209f32c;
        if (*(int*)(c + 0x60) <= *(int*)(c + 0x3ac)) {
            *(unsigned char*)(c + 0x39c) = 1;
            func_ov090_02130f94(c);
        }
    } else {
        *(int*)(c + 0x3ac) = *(int*)(c + 0x3a8);
    }
    if (*(int*)(c + 0x60) <= *(int*)(c + 0x3ac)) {
        *(int*)(c + 0x60) = *(int*)(c + 0x3ac);
        *(int*)(c + 0xa8) = 0;
    }
}
#pragma opt_common_subs off

// @symbol func_ov090_021314a0
/* Looks for a player close enough, and near enough to the skeeter's heading, to
 * chase. Returns 1 while a target is held, and releases it otherwise. */
int func_ov090_021314a0(char* c)
{
    char* p = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (p == 0) goto out;
    if (AngleDiff(*(short*)(c + 0x94), _ZN8dActor_c18HorzAngleToCPlayerEv(c)) >= 0x2000) goto out;
    {
        int* sv = (int*)(((int)p + 0x5c));
        int v[3];
        v[0] = sv[0];
        v[1] = sv[1];
        v[2] = sv[2];
        if (Vec3_Dist((const Vector3*)(c + 0x5c), (const Vector3*)v) >= 0x3ac000) goto out;
        int w[3];
        w[0] = v[0];
        w[1] = v[1];
        w[2] = v[2];
        *(short*)((c + 0x300) + 0x9a) = Vec3_HorzAngle((const Vector3*)(c + 0x5c), (const Vector3*)w);
        *(unsigned char*)(c + 0x39d) = 1;
        return 1;
    }
out:
    if (*(unsigned char*)(c + 0x39d) == 1) {
        *(int*)(c + 0x98) = 0x9000;
        *(unsigned char*)(c + 0x39d) = 0;
        *(short*)((c + 0x300) + 0x9a) = Vec3_HorzAngle((const Vector3*)(c + 0x5c), (const Vector3*)(c + 0x374));
    }
    return 0;
}

// @symbol func_ov090_02131584
/* State: resting on the surface. Leaves for the chase once a target turns up or
 * the counter runs long, and for the sinking state on the flag the surface
 * probe sets. */
int func_ov090_02131584(char* c)
{
    unsigned int v;
    *(int*)(c + 0x98) = 0;
    v = ((unsigned int)*(int*)(c + 0x364) << 4) >> 0x10;
    if (v >= 0x3b)
        *(int*)(((int)c + 0x390)) += 1;
    if (*(int*)(c + 0x390) > 2 || func_ov090_021314a0(c) == 1)
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_02134514);
    if (*(unsigned char*)(c + 0x39c) == 1)
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_02134504);
    return 1;
}

// @symbol func_ov090_02131608
/* Entry paired with update handler func_ov090_02131584. Clears the counter,
 * resets the playback rate and selects the animation at data_ov090_02134498. */
int func_ov090_02131608(char* c)
{
    *(int*)(c + 0x390) = 0;
    *(int*)(c + 0x3a4) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, MENBO_BCA(data_ov090_02134498), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131648
/* State: skimming. A line probe ahead of the skeeter, the distance back to the
 * anchor at 0x374, and the mesh wall flag each turn it around, and each one
 * latches its own bit so the turn only fires on the edge. Turning resets the
 * three timers the state runs on; the heading then eases toward the stored
 * angle every frame. */
int func_ov090_02131648(MenboState* c)
{
    char* self = (char*)c;
    int dist;
    u32 rnd;
    int selfY;
    Vector3 a, b, in, out;
    int angleSet;
    s16* p39a;
    int sh;
    s16 ha;
    int* p390;

    rnd = (u32)RandomIntInternal(&data_0209e650) >> 8;
    dist = Vec3_Dist((Vector3*)(self + 0x5c), (Vector3*)(self + 0x374));

    if ((((*(u32*)(self + 0x364)) << 4) >> 0x10 & 0xf) == 0) {
        func_02012694(0xfc, (const Vector3*)(self + 0x74));
    }

    angleSet = 0;
    dBgCh_Lin line;

    a.x = 0; a.y = 0; a.z = 0;
    b.x = 0; b.y = 0; b.z = 0;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;

    a.x = *(int*)(self + 0x5c);
    selfY = *(int*)(self + 0x60);
    a.y = selfY;
    a.z = *(int*)(self + 0x64);
    a.y = selfY + 0x64000;
    in.y = 0x64000;
    in.z = 0x1f4000;

    Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(self + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, 0x2000);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);

    b.x = a.x;
    b.x = a.x + out.x;
    b.y = a.y;
    b.z = a.z;
    b.y = a.y + out.y;
    b.z = a.z + out.z;

    line.SetObjAndLine(a, b, (dActor_c*)c);

    if (!line.DetectClsn()) {
        if (*(u8*)(self + 0x3a0) == 0) {
            ha = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)(self + 0x374));
            sh = (rnd & 3) << 0xc;
            *(s16*)((self + 0x300) + 0x9a) = ha;
            p39a = (s16*)((int)self + 0x39a);
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            *(int*)(self + 0x5c) = *(int*)(self + 0x68);
            *(int*)(self + 0x60) = *(int*)(self + 0x6c);
            *(int*)(self + 0x64) = *(int*)(self + 0x70);
            *(u8*)(self + 0x3a0) = 1;
        }
    } else {
        *(u8*)(self + 0x3a0) = (u8)angleSet;
    }

    if (dist > 0x3c0000) {
        if (*(u8*)(self + 0x39e) == 0 && *(u8*)(self + 0x3a0) == 0) {
            ha = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)(self + 0x374));
            *(s16*)((self + 0x300) + 0x9a) = ha;
            angleSet = 1;
            *(int*)(self + 0x5c) = *(int*)(self + 0x68);
            *(int*)(self + 0x60) = *(int*)(self + 0x6c);
            *(int*)(self + 0x64) = *(int*)(self + 0x70);
            *(u8*)(self + 0x39e) = 1;
        }
    } else {
        *(u8*)(self + 0x39e) = 0;
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv(self + 0x150) != 0) {
        if (*(u8*)(self + 0x39f) == 0 && *(u8*)(self + 0x39e) == 0 && *(u8*)(self + 0x3a0) == 0) {
            ha = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)(self + 0x374));
            sh = (rnd & 3) << 0xc;
            *(s16*)((self + 0x300) + 0x9a) = ha;
            p39a = (s16*)((unsigned int)self + 0x39a);
            angleSet = 1;
            *p39a = *p39a + (0x1800 - sh);
            *(int*)(self + 0x5c) = *(int*)(self + 0x68);
            *(int*)(self + 0x60) = *(int*)(self + 0x6c);
            *(int*)(self + 0x64) = *(int*)(self + 0x70);
            *(u8*)(self + 0x39f) = 1;
        }
    } else {
        *(u8*)(self + 0x39f) = 0;
    }

    if (*(u16*)((self + 0x300) + 0x98) == 0 && *(u8*)(self + 0x39f) == 0 && *(u8*)(self + 0x39e) == 0 && *(u8*)(self + 0x3a0) == 0) {
        *(u16*)((self + 0x300) + 0x98) = (u16)((rnd + 0x32) & 0x3f);
        p39a = (s16*)(self + 0x39a);
        *p39a = *p39a + (0x1800 - ((rnd & 3) << 0xc));
    }

    if (angleSet == 1) {
        *(u16*)((self + 0x300) + 0x94) = 0xa;
        *(u16*)((self + 0x300) + 0x98) = 0x32;
        *(u16*)((self + 0x300) + 0x96) = 0x1e;
        *(int*)(self + 0x98) = 0x9000;
        *(int*)(self + 0x3a4) = 0x1000;
    }

    _Z14ApproachLinearRsss((s16*)(self + 0x94), *(s16*)((self + 0x300) + 0x9a), 0x500);

    if (*(u16*)((self + 0x300) + 0x94) == 0 && *(u16*)((self + 0x300) + 0x96) == 0) {
        *(int*)(self + 0x3a4) = 0x1000;
        *(int*)(self + 0x98) = 0x9000;
        if (func_ov090_021314a0((char*)c) == 1) {
            *(u16*)((self + 0x300) + 0x96) = 0x1e;
            *(u16*)((self + 0x300) + 0x98) = 0x1e;
            *(int*)(self + 0x3a4) = 0x2000;
            *(int*)(self + 0x98) = 0xe000;
        }
    }

    if (((u32)((*(u32*)(self + 0x364)) << 4) >> 0x10) >= 0x10) {
        p390 = (int*)((int)self + 0x390);
        *p390 = *p390 + 1;
    }

    if (*(int*)(self + 0x390) > 0x1e && *(u8*)(self + 0x39e) == 0) {
        func_ov090_02131e00(c, (MenboStateFn*)&data_ov090_021344e4);
    }

    if (*(u8*)(self + 0x39c) == 1) {
        func_ov090_02131e00(c, (MenboStateFn*)&data_ov090_02134504);
    }

    return 1;
}

// @symbol func_ov090_02131a74
/* State entry: skimming. Resets the playback rate, arms the two skim timers and
 * starts the skim animation. */
int func_ov090_02131a74(char* c)
{
    *(int*)(c + 0x3a4) = 0x1000;
    *(short*)(c + 0x396) = 0;
    *(short*)(c + 0x398) = 0x32;
    *(int*)(c + 0x390) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, MENBO_BCA(data_ov090_02134480), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131ac4
/* State: braking. Bleeds the horizontal speed off, and once it reaches zero
 * drops the anchor at the current position, picks a fresh random heading and
 * goes back to skimming. The sinking flag overrides all of it. */
int func_ov090_02131ac4(char* c)
{
    _Z14ApproachLinearRiii(*(int*)(c + 0x98), 0, 0x1000);
    if (*(int*)(c + 0x98) > 0) return 1;
    if (*(unsigned char*)(c + 0x39c) == 0) {
        *(int*)(c + 0x374) = *(int*)(c + 0x5c);
        *(int*)(c + 0x378) = *(int*)(c + 0x60);
        *(int*)(c + 0x37c) = *(int*)(c + 0x64);
        *(short*)(c + 0x39a) = (short)(((unsigned int)RandomIntInternal(&data_0209e650) >> 8) << 0xd);
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344e4);
    }
    ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x39a), 1, 0x100, 0x100);
    if (*(unsigned short*)(c + 0x100) == 0)
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344f4);
    return 1;
}

// @symbol func_ov090_02131b94
/* State entry: resting. Three draws from the random source jitter the heading
 * twice and set how long the rest lasts, then the rest animation starts. */
int func_ov090_02131b94(char* thiz)
{
    char* c = thiz;
    unsigned int r;
    short* s;
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short*)((unsigned int)c + 0x39a);
    *s = (short)(*s + ((int)(((r >> 8) & 3) << 0x1e) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    s = (short*)(c + 0x39a);
    *s = (short)(*s + ((int)(((r >> 8) & 7) << 0x1d) >> 16));
    r = (unsigned)RandomIntInternal(&data_0209e650);
    *(short*)(c + 0x100) = (short)(((r >> 8) & 0x1f) + 0x96);
    *(int*)(c + 0x3a4) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, MENBO_BCA(data_ov090_02134490), 0, 0x1000, 0);
    return 1;
}

// @symbol func_ov090_02131c48
/* State: chasing. Speeds up after a few frames, turns away from a wall it is
 * heading into, and gives the chase up once the animation has run out enough
 * times. The sinking flag drops the anchor and returns to skimming. */
int func_ov090_02131c48(char* c)
{
    if ((((unsigned)*(int*)(c + 0x364)) << 4) >> 16 >= 4)
        *(int*)(c + 0x98) = 0x19000;

    if ((((unsigned)*(int*)(c + 0x364)) << 4) >> 16 == 4)
        func_02012694(0xfd, (const Vector3*)(c + 0x74));

    if (*(u16*)(c + 0x394) == 0
        && _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x150)
        && AngleDiff(*(s16*)(c + 0x94), *(s16*)(c + 0x39a)) < 0x200) {
        s16* p = (s16*)(((int)c + 0x39a));
        *p = *p + 0x4000;
        *(s16*)(c + 0x394) = 8;
    }

    if (*(u16*)(c + 0x394) != 0) {
        ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x39a), 1, 0x1000, 0x1000);
    } else {
        if (_ZN9Animation8FinishedEv(c + 0x35c)) {
            int* q = (int*)(((int)c + 0x390));
            *q = *q + 1;
            if (*(int*)(c + 0x390) > 0x14)
                func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_02134504);
        }
    }

    if (*(u8*)(c + 0x39c) == 0) {
        *(int*)(c + 0x374) = *(int*)(c + 0x5c);
        *(int*)(c + 0x378) = *(int*)(c + 0x60);
        *(int*)(c + 0x37c) = *(int*)(c + 0x64);
        *(s16*)(c + 0x39a) = (s16)(((unsigned)RandomIntInternal(&data_0209e650) >> 8) << 13);
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344e4);
    }

    return 1;
}

// @symbol func_ov090_02131db0
/* Entry paired with update handler func_ov090_02131c48. Selects the animation
 * at data_ov090_02134488 with flag 0x40000000, then resets the rate and counter. */
int func_ov090_02131db0(char* c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x30c, MENBO_BCA(data_ov090_02134488), 0x40000000, 0x1000, 0);
    *(int*)(c + 0x3a4) = 0x1000;
    *(int*)(c + 0x390) = 0;
    return 1;
}

// @symbol func_ov090_02131e00
/* Installs a state and runs its entry. Every state table node starts with the
 * pointer-to-member the actor enters through; a null one means the state has no
 * entry at all. */
int func_ov090_02131e00(MenboState* c, MenboStateFn* p)
{
    c->mState = p;
    MenboStateFn* q = c->mState;
    if (*q == 0) return 1;
    return (c->**q)();
}

// @symbol func_ov090_02131e50
/* Rebuilds the model matrix at 0x328 from the actor's position and rotation.
 * The position goes in shifted down by three, which is the scale the renderer
 * wants. */
void func_ov090_02131e50(char* c)
{
    int src[3];
    int dst[3];
    src[0] = *(int*)(c + 0x5c);
    src[1] = *(int*)(c + 0x60);
    src[2] = *(int*)(c + 0x64);
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(Matrix4x3*)(c + 0x328) = data_020a0e68;
}

// @symbol _ZN9daMenbo_c16CleanupResourcesEv
/* Releases the five shared files InitResources claimed -- one model and four
 * animations. Not in claim order, but every one is paired.
 *
 * TOUCHES NO FIELD. The ROM body takes no `this`; as a method it now receives
 * one and ignores it, which measured byte-free. */
int daMenbo_c::CleanupResources()
{
    data_ov090_021344a0.Release();
    data_ov090_02134488.Release();
    data_ov090_02134480.Release();
    data_ov090_02134490.Release();
    data_ov090_02134498.Release();
    return 1;
}

// @symbol _ZN9daMenbo_c16OnPendingDestroyEv
/* Empty in the ROM -- a single `bx lr`. The override exists to suppress the
 * base's behaviour. */
void daMenbo_c::OnPendingDestroy()
{
}

// @symbol _ZN9daMenbo_c6RenderEv
/* Draws through mModelAnim's own vtable, skipping the frame while the actor
 * carries the hidden flag. */
int daMenbo_c::Render()
{
    unsigned int f = ((MenboModelView*)this)->mFlags;
    int b = ((f & 0x40000) != 0);
    if (b) return 1;
    ((MenboModelView*)this)->mModelAnim.Draw((RenderArg*)((char*)&mScaleX));
    return 1;
}

#pragma opt_common_subs on
// @symbol _ZN9daMenbo_c8BehaviorEv
/* One frame of the skeeter, in four mutually exclusive branches, each returning
 * 1 on its own: in Yoshi's mouth, killed by a mega character, dying, or the
 * ordinary tail.
 *
 * unk_3ac is the WATER LINE, found once in InitResources by raycasting down.
 * The ordinary path clamps mPosY UP to it every frame, so the skeeter floats
 * rather than falls. The death branch reuses the same value as a TEST instead:
 * dying below the line is what pays out coins, so drowning and being squashed
 * on the surface end differently.
 *
 * Two branches share the same trigger check verbatim -- level 0x15, mAreaId
 * == 1, and the mesh reporting water -- and both respond by zeroing the four
 * motion words and setting mDeathState = 1, the knocked-into-water state.
 *
 * The final cylinder Update is gated on the closest player's +0x6fb, so the
 * skeeter stops colliding while that player is in some state of their own. */
int daMenbo_c::Behavior()
{
    char* c = (char*)this;

    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(c, &mWithMeshClsn)) {
        _ZN5dCc_c5ClearEv(&mdCcAcPos_c);
        if (mEatenByYoshi != 0 && unk_104 == 0)
            _ZN5dCc_c6UpdateEv(&mdCcAcPos_c);
        func_ov090_02131e50(c);
        return 1;
    }

    if (_ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(c, &mWithMeshClsn, &mModelAnim, 3))
        return 1;

    if (mDeathState != 0) {
        func_02035684((int*)(&mWithMeshClsn), 0xd2000);
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, &mWithMeshClsn, 0);
        if (_ZN12dEnemyBase_c11UpdateDeathER10dBgCh_Actr(c, &mWithMeshClsn))
            return 1;
        func_ov090_02131378(c);
        func_ov090_02131e50(c);
        if (mDeathState == 0)
            _ZN8dActor_c8PoofDustEv(c);
        if (unk_3a1 == 3) {
            _Z14ApproachLinearRsss(&mAngleX, -32767, 0x500);
            if (AngleDiff(*&mAngleX, -32767) < 0x1000) {
                s16* yaw = &mAngleY;
                *yaw += 0x1000;
            }
        }
        if (mDeathState != 1 && mPosY <= unk_3ac) {
            CoinVec3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, unk_10a + 1, 0xa000, 0);
            _ZN8dActor_c8PoofDustEv(c);
            _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        }
        return 1;
    }

    {
    int flag = (mFlags & 8) != 0;
    if (flag) {
        mHorzSpeed = 0;
        _ZN8dActor_c9UpdatePosEP5dCc_c(c, &mdCcAcPos_c);
        func_ov090_02131378(c);
        if (data_0209f2f8 == 0x15 && mAreaId == 1) {
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, &mWithMeshClsn, 2);
            if (func_02035638((u8*)(&mWithMeshClsn))) {
                mVertAccel = 0;
                unk_0a4 = 0;
                mVertSpeed = 0;
                unk_0ac = 0;
                mDeathState = 1;
                func_ov002_020aea30(c, _ZN8dActor_c13ClosestPlayerEv(c), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, &mdCcAcPos_c);
    func_ov090_02131378(c);
    DecIfAbove0_Short((u16*)&mStateTimer);
    DecIfAbove0_Short(&unk_394);
    DecIfAbove0_Short(&unk_396);
    DecIfAbove0_Short(&unk_398);
    _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(c, &mWithMeshClsn, 2);
    if (mPosY <= unk_3ac)
        mPosY = unk_3ac;
    if (data_0209f2f8 == 0x15 && mAreaId == 1 && func_02035638((u8*)(&mWithMeshClsn))) {
        mVertAccel = 0;
        unk_0a4 = 0;
        mVertSpeed = 0;
        unk_0ac = 0;
        mDeathState = 1;
        func_ov002_020aea30(c, _ZN8dActor_c13ClosestPlayerEv(c), 0);
        return 1;
    }

    {
        MenboStateNode* n = (MenboStateNode*)mState;
        if (n->mRun)
            (((MenboSelf*)c)->*(n->mRun))();
    }
    mAngleY = mPrevAngleY;
    /* MEASURED: these two reach inside mModelAnim (0x368 and 0x35c are its
       +0x5c and +0x50) and they are the ONLY spelling in this function that is
       not free. Writing them as `(char*)&mModelAnim + 0x5c` costs bytes --
       greedy-tested alone against build_pin, everything else here substituted
       for free. Offsetting from a typed sub-object's address is not the same
       to mwcc as offsetting from `this`, so these stay as they are. */
    *(s32*)(c + 0x368) = *(s32*)(c + 0x3a4);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02131e50(c);
    func_ov090_021310b4(c);
    _ZN5dCc_c5ClearEv(&mdCcAcPos_c);
    {
        void* p = _ZN8dActor_c13ClosestPlayerEv(c);
        if (p != 0 && *(u8*)((char*)p + 0x6fb) == 0)
            _ZN5dCc_c6UpdateEv(&mdCcAcPos_c);
    }
    return 1;
}
#pragma opt_common_subs off

// @symbol _ZN9daMenbo_c13InitResourcesEv
/* Loads the model and four animations, builds both collision volumes, and then
 * FINDS THE WATER LINE -- which is the part that matters.
 *
 * It raycasts straight down from 0x32000 above the spawn point using a local
 * dBgCh_Gnd, and what it does with the hit depends on a surface flag: a 0x20
 * surface sets unk_39c and only records the height, while any other surface
 * records it as both unk_3a8 and unk_3ac. mPosY is then snapped to unk_3ac, so
 * the skeeter starts exactly on the surface it found.
 *
 * The level/mode combination (0x12 in mode 2) installs a different starting
 * state and returns before the raycast. The flagged-surface case returns only
 * after DetectClsn and the surface-flag test, also with a different state.
 *
 * Otherwise the heading is randomised -- four bits of RandomIntInternal shifted
 * into the top of a s16 -- and written through mModelAnim's own angle slot
 * before being published to mPrevAngleY and mAngleY. */
int daMenbo_c::InitResources()
{
    char* c = (char*)this;
    BMD_File* f;
    int r;
    Vector3 pos;
    Vector3 v;

    f = (BMD_File*)_ZN5Model8LoadFileER13SharedFilePtr(data_ov090_021344a0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, f, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_02134488);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_02134480);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_02134490);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov090_02134498);

    mTerminalVelocity = -0x3c000;

    v.x = data_ov090_0213412c.x;
    v.y = data_ov090_0213412c.y;
    v.z = data_ov090_0213412c.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, (dActor_c*)c, &v, 0x5a000, 0x5a000, 0x200000, 0x7eff0);

    mAngleY = mPrevAngleY;
    unk_3a4 = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, (int)c, 0xc8000, 0, 0, 0);
    func_0203558c(&mWithMeshClsn);

    unk_108 = 1;
    unk_10a = 2;
    mVertAccel = -0x3000;

    {
        int b = 1;
        if (data_0209f2d8 != 2) b = 0;
        if (b != 0 && data_0209f2f8 == 0x12) {
            unk_3ac = mPosY;
            unk_374 = mPosX;
            unk_378 = mPosY;
            unk_37c = mPosZ;
            func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344f4);
            return 1;
        }
    }

    {
        dBgCh_Gnd ground;
        ground.mProbeHeight = 0xbb8000;
        ground.StartDetectingWater();
        {
            int py = mPosY;
            int pz = mPosZ;
            int px = mPosX;
            int ip = py + 0x32000;
            pos.x = px;
            pos.y = ip;
            pos.z = pz;
        }
        ground.SetObjAndPos(pos, this);
        unk_3a8 = data_02092138;
        if (ground.DetectClsn() != 0) {
            if (SurfaceInfo_TestFlag0x20(&ground.surface) != 0) {
                unk_39c = 1;
                unk_3ac = ground.clsnY;
            } else {
                unk_3a8 = ground.clsnY;
                unk_3ac = ground.clsnY;
            }
        }

        mPosY = unk_3ac;
        unk_374 = mPosX;
        unk_378 = mPosY;
        unk_37c = mPosZ;

        if (unk_39c != 0) {
            func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344f4);
            return 1;
        }

        {
            r = RandomIntInternal(&data_0209e650);
            short ang = (short)((((unsigned int)r >> 8) & 0xf) << 12);
            *(short*)&unk_39a = ang;
            mPrevAngleY = *(short*)&unk_39a;
            mAngleY = mPrevAngleY;
        }
        func_ov090_02131e00((MenboState*)c, (MenboStateFn*)&data_ov090_021344e4);
    }

    return 1;
}

// @symbol _ZN9daMenbo_c16OnAimedAtWithEggEv
/* Vtable slot 29 -- how far Yoshi's aim leads this target. */
s32 daMenbo_c::OnAimedAtWithEgg()
{
    return 131072;
}

// @symbol _ZN9daMenbo_c13OnTurnIntoEggER6Player
/* Vtable slot 19, verified against ov090 relocs.txt: _ZTV9daMenbo_c
 * (0x02134168) + 0x4c relocates to 0x02132620, exactly this function. */
void daMenbo_c::OnTurnIntoEgg(Player& player)
{
    GivePlayerCoins(player, (unsigned char)(unk_10a + 1), 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN9daMenbo_c13OnYoshiTryEatEv
/* Vtable slot 18 -- what Yoshi turns this into when he swallows it. */
s32 daMenbo_c::OnYoshiTryEat()
{
    return 4;
}
