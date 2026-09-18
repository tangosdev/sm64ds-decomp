//cpp
/* ov006/dScMgJump_c -- genuine translation unit, 17 of the class's 18
 * cartridge functions. Licensed .text is 0x020edec0..0x020ee994.
 *
 * Merged by tools/tubuild.py create from the per-function sources below, then
 * reconciled by hand.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order). Each
 * lived directly under src/ and none of them exists any more; this file
 * replaces them, and the manifest entry
 * config/tu_manifest.d/ov006/dScMgJump_c.json keeps every one of their full
 * paths in its functions[].legacy_source rows, which is the historical record:
 *   [0] 0x020edec0  _ZN11dScMgJump_cD1Ev.cpp
 *   [1] 0x020edf54  _ZN11dScMgJump_cD0Ev.cpp
 *   [2] 0x020edffc  _ZN11dScMgJump_c16CleanupResourcesEv.cpp
 *   [3] 0x020ee034  _ZN11dScMgJump_c6RenderEv.cpp
 *   [4] 0x020ee27c  _ZN11dScMgJump_c8BehaviorEv.cpp
 *   [5] 0x020ee2c0  func_ov006_020ee2c0.c
 *   [6] 0x020ee2c4  func_ov006_020ee2c4.c
 *   [7] 0x020ee3bc  func_ov006_020ee3bc.c
 *   [8] 0x020ee3ec  func_ov006_020ee3ec.c
 *   [9] 0x020ee44c  func_ov006_020ee44c.c
 *   [10] 0x020ee4e0  func_ov006_020ee4e0.c
 *   [11] 0x020ee508  func_ov006_020ee508.c
 *   [12] 0x020ee598  func_ov006_020ee598.c
 *   [13] 0x020ee5b8  func_ov006_020ee5b8.c
 *   [14] 0x020ee658  func_ov006_020ee658.c
 *   [15] 0x020ee690  _ZN11dScMgJump_c13InitResourcesEv.cpp
 *   [16] 0x020ee8dc  _ZN11dScMgJump_c13OnTurnIntoEggEi.cpp
 *
 * ORDINAL 17 IS NOT HERE, AND THAT IS THE TU BOUNDARY.
 * _ZN11dScMgJump_c13OnYoshiTryEatEi at 0x020ee994 (size 0x168) is the
 * eighteenth function of the cartridge's run, and it is NOT byte-matched: its
 * source carries a draft banner declaring itself a non-match (the draft
 * assembles four bytes short) -- the literal marker word is deliberately not
 * repeated here, because has_draft_banner() in tools/asm_policy.py scans this
 * file's
 * whole leading comment block and would read a mention of it as a claim that
 * THIS file is a draft, which would silently drop all 17 members below out of
 * tools/enroll.py's candidate set and out of the ROM build.
 * tools/enroll.py leaves that one out of the ROM build, and it therefore has no
 * delinks.txt entry at all -- its cartridge bytes are kept as they are. An
 * unmatched function cannot sit inside a licensed `complete` span, and this
 * one sits at the TOP of the run, so the licensed range simply stops below it
 * at 0x020ee994 and its own file stays where it is, unchanged and still
 * unenrolled. When it matches byte-for-byte it moves in here, the span
 * extends to 0x020eeafc, and the manifest's boundary note comes out.
 *
 * DESTRUCTORS: ORDINALS 0 AND 1 ARE NOT WRITTEN OUT HERE EITHER.
 * ~dScMgJump_c() is defined in the class body in include/dScMgJump_c.h; see
 * the note there for why the out-of-line spelling cannot be linked as one
 * translation unit. mwccarm emits D1 and then D0 from that one definition and
 * emits no D2, which is exactly the cartridge's order -- D1 at 0x020edec0,
 * below D0 at 0x020edf54.
 *
 * With the destructor inline, this class's KEY FUNCTION is InitResources, the
 * first virtual the header declares without defining, and it is defined here.
 * So this TU, and no other, emits _ZTV11dScMgJump_c and the inheritance
 * chain's typeinfo. Every one of those is compiler-generated, byte-compared
 * against the cartridge by romdata_check, and then discarded; the manifest's
 * compiler_only_output rows record each one's canonical cartridge address.
 */

#include "dScMgJump_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "types.h"

/* Local shadow declarations, reconciled across the sixteen merged sources.
 *
 * `C` is a bare non-polymorphic stand-in for the receiver, kept rather than
 * replaced by dScMgJump_c: a pointer-to-member-function on a POLYMORPHIC
 * class is a different representation under mwccarm, so the stand-in is
 * codegen and not decoration -- the same reason
 * src/actors/daObjMarioCap_c.cpp keeps its own. The forward
 * declaration before the typedef is what the legacy source had and what the
 * pointer-to-member spelling needs. */
struct C;
typedef void (C::*PMF)();
struct C { char pad[0x5004]; PMF m; };

/* THE EIGHT-BYTE PAIR, under ONE name where the merged sources had four.
 * func_ov006_020ee2c4 spelled it `P { int a, b; }`, 020ee3bc / 020ee4e0 /
 * 020ee598 spelled it `Pair { int a, b; }`, 020ee3ec spelled it
 * `G2 { int w[2]; }` and 020ee658 spelled it `S2 { int w[2]; }`. All four are
 * the same eight bytes assigned whole, and a merged TU should carry one type
 * per thing. What is being copied is the state callback at +0x5004, which
 * include/dScMgJump_c.h records as the unk_5004/unk_5008 pair a
 * pointer-to-member occupies -- so every member below that writes a `Pair` is
 * installing the next state, and Behavior dispatches through it. It stays a
 * raw pair rather than a typed PMF because naming it would fix a signature
 * for every state function in the table and none of them is recovered. */
struct Pair { int w[2]; };

/* One 4x3 matrix spelling, likewise: Render carried it as
 * `M48 { int w[12]; }` and InitResources as `Mtx43 { int m[12]; }`. */
struct Mtx43 { int m[12]; };
struct V3 { int w[3]; };

/* Raw vtable shim for the self-dispatch at the end of InitResources -- slot 18
 * called through this class's own table. Left as a shim, the same shape the
 * pre-migration file used, rather than a qualified OnYoshiTryEat call: the ROM
 * dispatches virtually here, and a qualified call compiles to a direct `bl`. */
struct Base {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int x);
};

/* THE ABI SEAM, ALL OF IT, ABOVE THE FIRST `// @symbol` MARKER.
 * tools/tiers.py slices a promoted TU at those markers, so a declaration
 * written between two definitions is charged to the earlier member. Everything
 * this TU reaches that include/decl_common.h does not already declare is
 * therefore collected here.
 *
 * THREE RECONCILIATIONS, because the merged sources disagreed:
 *
 * - func_ov004_020b0a54 was `(void *)` in func_ov006_020ee2c4 and `(int)` in
 *   func_ov006_020ee3ec. It is not declared here at all now: decl_common.h
 *   line 2285 already declares it `(int)`, which is also what
 *   src/minigames/d_s_mg_jump2.cpp settled on, and both call sites below pass
 *   a small integer id (0 and 0x12) rather than an address.
 *
 * - _Z15ApproachLinear2Rsss was `int (short *, short, short)` in
 *   func_ov006_020ee44c and `short (short &, short, short)` in OnTurnIntoEgg.
 *   The mangled name itself settles it: `Rs` is a reference. One spelling is
 *   kept and 020ee44c's call site dereferences instead of casting to a
 *   pointer, which is byte-neutral -- a reference is passed as the same
 *   address in the same register.
 *
 * - data_020a0e68 was `int` in Render and `struct Mtx43` in InitResources.
 *   It stays `int` and both whole-matrix copies cast at the use site, which
 *   is the spelling src/minigames/d_s_mg_jump2.cpp settled on for this same
 *   symbol.
 *
 * GetGameLanguage and Matrix4x3_FromTranslation were also flagged, but only
 * differed by the `extern` keyword and by int-vs-s32, which is one type. */
extern "C" {
void func_ov004_020ad90c(void);
void func_ov004_020b1e34(void *a, int b, int c, int d);
int  GetGameLanguage(void);
void DrawOamSprite(void *a0, void *a1, int a2, void *a3);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void func_0203cd80(int *m, short angle);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_02012790(int a0);
void FreeGfxSlotsById(int arg);
int  LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void _ZN2GX11LoadOBJPlttEPKvjj(void const *src, unsigned int offset, unsigned int size);
s32  _ZN4cstd4fdivEii(s32 a, s32 b);
void Camera_UpdateMatrices(void *cam);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
int  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int b, int c);
short _Z15ApproachLinear2Rsss(short &r, short b, short c);

void func_ov006_020c40e8(void);
void func_ov006_020c42bc(void);
void func_ov006_020c712c(void);
void func_ov006_020c7388(void);
void func_ov006_020c7418(void);
void func_ov006_020c7490(void);
int  func_ov006_020c7300(void);
int  func_ov006_020c4060(void);
int  func_ov006_020c4684(char *ptr, int n);
int  func_ov006_020c7574(char *base, int count);

/* Defined below in this TU; declared here because they call each other. */
void func_ov006_020ee3bc(char *c);
void func_ov006_020ee4e0(char *p);
void func_ov006_020ee598(char *p);

extern int  data_020a0e68;
extern u8   data_0209d45c;
extern s16  data_02082614;
extern int  data_ov006_02140418;
extern int  data_ov006_02140304;
extern int  data_ov006_02140434;
extern struct Pair data_ov006_0213cb4c;
extern struct Pair data_ov006_0213cb54;
extern struct Pair data_ov006_0213cb5c;
extern struct Pair data_ov006_0213cb6c;
extern struct Pair data_ov006_0213cb74;
extern struct Pair data_ov006_0213cb7c;
extern struct Pair data_ov006_0213cb84;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- _ZN11dScMgJump_c13OnTurnIntoEggEi, 0x020ee8dc, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgJump_c13OnTurnIntoEggEi
/* dScMgJump_c::OnTurnIntoEgg -- vtable slot 19, recovered from vtable slot
 * identity. */
int dScMgJump_c::OnTurnIntoEgg(int sel)
{
    char *thiz = (char *)this;

    struct dScMgJump_c *self = (struct dScMgJump_c *)(void *)thiz;
    if (sel == 0) {
        if (self->unk_0bc % 5 == 4) {
            if (_Z15ApproachLinear2Rsss(self->unk_5014, 0, 1) != 0) {
                if ((&data_ov006_02140428)[0] < 3) {
                    func_ov006_020c72dc();
                    func_02012790(0x26);
                    *(short*)(thiz + 0x5000 + 0x14) = 0x3c;
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
    func_ov006_020c8a9c(0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- _ZN11dScMgJump_c13InitResourcesEv, 0x020ee690, size 0x24c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgJump_c13InitResourcesEv
/* dScMgJump_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgJump_c, this class's own slot 0
 * (fBase_c::InitResources). The old file's `recovered name:
 * dScMgJump_c_InitResources` agreed.
 *
 * THIS IS ALSO THE CLASS'S KEY FUNCTION now that the destructor is defined in
 * the class body: it is the first virtual the header declares without
 * defining. That is what makes this TU, and no other, emit _ZTV11dScMgJump_c
 * -- and slots 16 and 17 of that table are what odr-use D1 and D0 and get
 * them emitted out of line in the cartridge's order.
 *
 * The final `((Base *)base)->m48(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *base` parameter. */
s32 dScMgJump_c::InitResources()
{
    char *base = (char *)this;
    s32 idx;
    int buf1, buf2;
    s32 fov;
    void *modelFile;
    int r;

    idx = GetGameLanguage();
    buf1 = LoadFile(data_ov006_0213cbc8[idx]);
    buf2 = LoadFile(0xd1);
    DecompressLZ16((void *)buf1, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((void *)buf2, 0, 0x80);
    Deallocate((void *)buf1);
    Deallocate((void *)buf2);

    data_0209d45c = 0x11;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082614);

    this->mCamera0EyeX = 0;
    this->mCamera0EyeY = -0x64000;
    this->mCamera0EyeZ = 0;
    this->mCamera0TargetX = 0;
    this->mCamera0TargetY = 0;
    this->mCamera0TargetZ = fov;
    this->mCamera0Angle = 0x800;
    Camera_UpdateMatrices(base + 0x466c);

    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x800;
    Camera_UpdateMatrices(base + 0x4728);

    if (func_ov006_020c4684(base + 0x5294, 6) == 0)
        return 0;
    if (func_ov006_020c7574(base + 0x506c, 3) == 0)
        return 0;

    if (*(int *)(base + 8) & 1) {
        data_ov006_02142184 = &data_ov006_02142188;
    } else {
        data_ov006_02142184 = &data_ov006_02142190;
    }
    modelFile = _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02142184);
    r = _ZN9ModelBase7SetFileEP8BMD_Fileii(base + 0x501c, modelFile, 1, -1);
    if (r == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(struct Mtx43 *)(base + 0x5038) = *(struct Mtx43 *)&data_020a0e68;
        return 0;
    }
    func_ov004_020b04d0(0x10);
    ((Base *)base)->m48(-1);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020ee658, 0x020ee658, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee658
/* Still spelled func_<module>_<address> because the symbol is unnamed in
 * config/arm9/overlays/ov006/symbols.txt. Arms the countdown at +0x5014 and
 * installs the next state pair. Its only caller is OnYoshiTryEat, which is
 * outside this TU's licensed range. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee658(char *c) {
    *(short *)(c + 0x5014) = 0x78;
    *(int *)(c + 0x500c) = 0;
    *(int *)(c + 0x5010) = 0x4000;
    *(struct Pair *)(c + 0x5004) = data_ov006_0213cb54;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020ee5b8, 0x020ee5b8, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee5b8
/* Still spelled func_<module>_<address>: unnamed in symbols.txt.
 *
 * PRAGMA NOT CARRIED, AND THAT IS A MEASUREMENT. The legacy source this member
 * came from, func_ov006_020ee5b8.c -- it lived directly under src/ and does
 * not exist any more; the banner lists it and the manifest's
 * functions[].legacy_source row keeps its full path -- had
 * `#pragma optimize_for_size on` at file scope. In mwccarm 2004/b56 that
 * pragma is file-global last-wins rather than positional, so carrying it into
 * a merged TU silently recompiles every other member -- and it is not
 * load-bearing here: with it dropped outright this member still compiles
 * byte-exact at -O4,p. The control that says the compile is live either way is
 * in this TU's manifest boundary_evidence. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee5b8(char* c){
    int v;
    short* d = (short*)((unsigned long long)(c+0x5014));
    char* base = c + 0x5000;
    *d = *d - 1;
    v = *(short*)(base + 0x14);
    if (v == 0) {
        FreeGfxSlotsById(0x1d);
        if (*(unsigned char*)(c+0xc4) == 0) {
            *(unsigned char*)(c+0xc3) = 1;
            *(unsigned char*)(c+0xc4) = 1;
            *(short*)(c+0xc0) = 0;
        }
        func_ov006_020c7490();
        func_ov006_020ee598(c);
    } else if (v == 0x77) {
        func_ov004_020b0cac(0xd, 0x80, 0x60, 1, -1, 0xd);
    }
    func_ov006_020c42bc();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020ee598, 0x020ee598, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee598
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee598(char *p) {
    *(struct Pair *)(p + 0x5000 + 4) = data_ov006_0213cb84;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020ee508, 0x020ee508, size 0x90 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee508
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee508(char *c){
    int *acc = (int*)(((int)c + 0x500c));
    *acc = *acc + (((int*)(c+0x5000))[4] >> 12);
    if(((int*)(c+0x5000))[3] > 0x1000){
        *acc -= 0x1000;
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
    if(data_ov006_02140428 == 0){
        func_ov006_020ee4e0(c);
        return;
    }
    if(data_ov006_02140304 != 0)
        return;
    func_ov006_020ee3bc(c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020ee4e0, 0x020ee4e0, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee4e0
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee4e0(char *p)
{
    *(unsigned short *)(p + 0x5014) = 0x20;
    *(struct Pair *)(p + 0x5004) = data_ov006_0213cb5c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020ee44c, 0x020ee44c, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee44c
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee44c(char *c)
{
  int *p = (int *) ((((int) c) + 0x500c));
  char *b = c + 0x5000;
  *p = (*p) + ((*((int *) ((c + 0x5000) + 0x10))) >> 12);
  if ((*((int *) (b + 0xc))) > 0x1000)
  {
    *p = (*p) - 0x1000;
  }
  func_ov006_020c42bc();
  func_ov006_020c712c();
  if (_Z15ApproachLinear2Rsss(*(short *) (c + 0x5014), 0, 1) == 0)
  {
    return;
  }
  func_ov006_020c7388();
  func_ov006_020c40e8();
  *((struct Pair *) (c + 0x5004)) = data_ov006_0213cb4c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020ee3ec, 0x020ee3ec, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee3ec
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee3ec(char* c)
{
    int r = func_ov006_020c7300();
    if (r) {
        r = func_ov006_020c4060();
        if (r) {
            func_ov004_020b0a54(0x12);
            *(u8*)(c + 0xc3) = 0;
            *(u16*)(c + 0x5000 + 0x14) = 0;
            *(struct Pair*)(c + 0x5000 + 4) = data_ov006_0213cb74;
        }
    }
    func_ov006_020c42bc();
    func_ov006_020c712c();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020ee3bc, 0x020ee3bc, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee3bc
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee3bc(char *c)
{
    func_ov006_020c7418();
    *(struct Pair*)(c + 0x5004) = data_ov006_0213cb6c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020ee2c4, 0x020ee2c4, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee2c4
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. This is the
 * helper whose writes to +0x5004 and +0x5014 bound dScMgD3DBase_c from above
 * at 0x5004; see the banner of include/dScMgD3DBase_c.h. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee2c4(char* c){
  *(int *)(((int)c + 0x500c)) += *(int*)(c + 0x5010) >> 12;
  *(int *)(((int)c + 0x5010)) += 0x600;
  func_ov006_020c712c();
  int eq = (int)(data_ov006_02140434 == data_ov006_02140418);
  if(eq == 0) return;
  char* g = (char *)data_ov004_020beb68;
  if(g != 0){
    if(*(int*)(g+0xb4) < 0x270f){
      *(int *)(((int)g + 0xb4)) += 1;
    }
    if(*(int*)(g+0xb4) > *(int*)(g+0xb8)){
      *(int*)(g+0xb8) = *(int*)(g+0xb4);
    }
  }
  func_ov004_020adb1c(*(int*)(c+0xb4));
  func_ov004_020b0a54(0);
  *(unsigned char*)(c+0xc3) = 0;
  *(short*)(c+0x5014) = 0;
  *(struct Pair*)(c+0x5004) = data_ov006_0213cb7c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020ee2c0, 0x020ee2c0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ee2c0
/* Still spelled func_<module>_<address>: unnamed in symbols.txt. One `bx lr`
 * -- an empty state slot in the same table the helpers above install into. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ee2c0(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11dScMgJump_c8BehaviorEv, 0x020ee27c, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgJump_c8BehaviorEv
/* dScMgJump_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgD3DBase_c's. The whole body is one dispatch through the
 * pointer-to-member the object carries at 0x5004, which is the state machine --
 * include/dScMgJump_c.h records that pair as unk_5004/unk_5008, the two words a
 * PMF occupies, and every `struct Pair` store above writes to it. */
s32 dScMgJump_c::Behavior()
{
    C *c = (C *)this;
    (c->*c->m)();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11dScMgJump_c6RenderEv, 0x020ee034, size 0x248 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgJump_c6RenderEv
/* dScMgJump_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the third of the three slots where this class's
 * table differs from dScMgD3DBase_c's.
 *
 * Draws the language-dependent instruction rows on the touch screen while
 * unk_4664 == 1, then sets up the 3D engine's fog registers twice -- once for the
 * scene, once with bit 30 set for the pass that follows -- and hands the model at
 * 0x501c its transform. 0x040004c8 / 0x040004cc are memory-mapped registers, which
 * is why the stores are volatile; that is I/O, not codegen steering. */
s32 dScMgJump_c::Render()
{
    void *self = (void *)this;
    char *c = (char *)self;

    if (*(u16 *)(c + 0x4664) == 1) {
        int count;
        int a1v;
        int i;

        func_ov004_020b1e34(self, 0xe0, 0x14, 1);

        count = data_ov006_02140428;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = GetGameLanguage();
                DrawOamSprite((void *)data_ov006_0213cbb4[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                RenderOamMainScreen(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            (((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20);
        *(volatile int *)0x40004cc = 0x7fff;
        *(volatile int *)0x40004cc = 0x40007fff;
        Matrix4x3_FromTranslation(&data_020a0e68, 0, -0xef000, 0);
    }
    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            ((((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
    }

    *(struct Mtx43 *)(c + 0x5038) = *(struct Mtx43 *)&data_020a0e68;

    {
        V3 t;
        void *obj;
        void *p;

        t = *(V3 *)data_ov006_0213cb8c;

        obj = c + 0x5024;
        p = *(void **)((char *)obj + 4);
        func_02045e44(obj, 1, 0);
        *(int *)((char *)p + 0x18) = *(int *)(c + 0x500c);

        {
            void *obj2 = (void *)(c + 0x501c);
            ((void (**)(void *, void *))(*(int *)obj2))[5](obj2, &t);
        }
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN11dScMgJump_c16CleanupResourcesEv, 0x020edffc, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dScMgJump_c16CleanupResourcesEv
/* dScMgJump_c::CleanupResources -- vtable slot 3.
 *
 * Attributed by the ROM's vtable: slot 3 is one of the three where this class's
 * table differs from dScMgD3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * IT IGNORES `this` ENTIRELY -- the only state it releases is the file handle in
 * the ov006 global at 0x02142184, not anything on the object. That is why the
 * pre-migration file could be spelled `int func_ov006_020edffc(void)` and still
 * match: the ROM leaves r0 untouched, so a receiver it never reads costs nothing.
 * The signature has to be fBase_c::CleanupResources's, and it now is. */
s32 dScMgJump_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov006_02142184)->Release();
    data_ov006_02142184 = 0;
    func_ov004_020ad90c();
    return 1;
}

/* --------------------------------------------------------------------------
 * ROM ordinals 1 and 0 -- _ZN11dScMgJump_cD0Ev at 0x020edf54 (0xa8)
 *                     and _ZN11dScMgJump_cD1Ev at 0x020edec0 (0x94).
 *
 * NO BODY HERE, AND NO `@symbol` MARKER ON EITHER, DELIBERATELY.
 *
 * The destructor is defined in the class body in include/dScMgJump_c.h.
 * mwccarm emits D1 and then D0 from that one definition and emits no D2,
 * which is the cartridge's order; an out-of-line member definition emits D0
 * ahead of D1 plus a homeless D2, and a single TU covering this whole run
 * could not then be linked, because the functions would not be in ROM address
 * order. The legacy per-function split had to write the same body twice
 * because each file could hold only one symbol; writing it twice in one
 * translation unit is a redefinition.
 *
 * tools/tiers.py scores these two through its lifecycle path, off that inline
 * definition in a directly included header, so they need no marker -- and a
 * marker here would only mis-slice CleanupResources above.
 * -------------------------------------------------------------------------- */
