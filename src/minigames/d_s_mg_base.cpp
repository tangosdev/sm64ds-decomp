//cpp
/* ov004/dScMgBase_c -- the behavior every minigame scene shares, and the
 * sprite, OAM and graphics helpers that sit with it
 * (.text 0x020ad660..0x020b2c58, 125 functions).
 *
 * This is the whole ov004 linker unit, from dScMgBase_c::Virtual8C at the
 * first byte of ov004's .text up to the constructor, which ends where
 * func_ov004_020b2c58 begins. It holds the eight scene overrides at
 * 0x020b04e8..0x020b0a38 plus 117 former one-function sources folded in
 * around them, among them the score-number drawer func_ov004_020b2220, the
 * engine and background set-up slots 31 to 33, the destructor and the
 * constructor. Each former source keeps its own namespace block: their local
 * struct views and extern declarations disagree with each other, and
 * unifying them changes code generation, so that is left as separate work.
 *
 * Definitions are in ROM order, lowest address first, under
 * defer_codegen off. The one exception is the constructor and destructor
 * at the end, which sit in a defer_codegen on bracket with the constructor
 * written first: see the note there.
 *
 * GraphCallback0, the key function of dScMgBase_c::graphCallback_c, is here,
 * and so is ~dScMgBase_c, the key function of dScMgBase_c itself, so this
 * file emits both classes' vtables and RTTI.
 *
 * Still raw: unk_0a4, 0a8, 0ac, 0b8, 0c8, 462c and 465c are unnamed in the
 * header, and the func_ov004 and data_ov004 helpers they feed are unnamed
 * in symbols.txt.
 */

#include "dScMgBase_c.h"
#include "dWipe_c.h"
#include "decl_common.h"
#include "Sound.h"
#include "types.h"
#include "dEnemyBase_c.h"
#include "private/ov004_obj_vtbl.h"
#include "PlayerInput.h"

#pragma defer_codegen off

/* The 0x40 records at data_ov004_020bebe8 are 0x20 bytes each; only the s16
   at 0x1a (nonzero while the record is live) and the dispatch index at 0x1e
   are known, so the type stays a local view. */
struct Ent {
    char pad[0x1a];
    s16 f;                              /* 0x1a */
    s16 g;                              /* 0x1c */
    s16 idx;                            /* 0x1e -- state table index */
    Ent() {}
    ~Ent() {}
    void func_ov004_020adc80();
    void func_ov004_020adcc8();
    void func_ov004_020addcc();
    void func_ov004_020adeb0();
};

typedef void (Ent::*EntPMF)();

extern "C" {
/* Static minigame base state: the graphCallback_c registry instance, the
   Ent record pool, and the two PMF state tables the dispatchers call
   through. Declaration order is the ROM's __sinit order. */
dScMgBase_c::graphCallback_c data_ov004_020beb74;
Ent data_ov004_020bebe8[0x40];
EntPMF data_ov004_020beb88[2] = { &Ent::func_ov004_020adeb0, &Ent::func_ov004_020addcc };
EntPMF data_ov004_020beb98[2] = { &Ent::func_ov004_020adcc8, &Ent::func_ov004_020adc80 };
int data_ov004_020beba8[16];
}

/* Local view of the stylus owner at data_0209f5bc, also used by
 * dScMiniGm_c. Only slot 5 is known: it says whether input is live. The
 * owner's real class is not recovered. */
struct SceneVCall6 {
    virtual int v0(); virtual int v1(); virtual int v2();
    virtual int v3(); virtual int v4(); virtual int IsActive();
};

int ApproachLinear(int &value, int target, int step);

extern "C" {
extern void *data_ov004_020beb60;
extern void **data_0209d4a8;
int func_ov004_020b8ee0(char *p);
void func_ov004_020aeb24(char *c);
void func_ov004_020b321c(char *c);
void func_ov004_020adf2c(char *c);
extern SceneVCall6 *data_0209f5bc;
extern unsigned short data_020a0e5a[];
extern int data_0208ee44;
extern void func_ov004_020ad90c(void);
extern void FreeGfxSlotsById(int arg);
extern void func_0203cbc0(void *ptr);
extern void func_02012e1c(void);
void func_ov004_020ae330();
extern void Enable3dEngines(void);
extern char data_0209b308[];
extern dWipe_c data_0209f61c;
extern unsigned char data_0209d460[];
extern unsigned char data_0209d458[];
}

namespace s20ad660 {
extern "C" {
// @symbol _ZN11dScMgBase_c9Virtual8CEv
/* dScMgBase_c::Virtual8C - slot 35, the LAST slot of dScMgBase_c's own
   eighteen.  Twenty bytes, and the end of the 18-35 keystone range: with this
   declared the class emits its full 36-slot vtable from source and no table in
   the minigame family is a prefix any more.

   It is a predicate on the scene's own spawn parameter: `param1 & 0xff`, the
   fBase_c word at +0x08 that every fBase_c is constructed with, tested against
   zero.  dScMgAmida_c is the only class in the family that overrides it, and
   it asks the narrower question (`== 1`); the other 31 tables carry this body.

   Thirteen call sites, all in ov006, all the same shape --
   `mov r0,<this>; ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1` -- spread across
   FOUR leaf classes' code regions: dScMgCoin_c (2), dScMgPanel_c (4),
   dScMgSound_c (3), dScMgSnowball_c (4).  Each is a class asking the question
   of itself and branching on the answer, which is how a single minigame runs
   in two variants: two asset tables at ov006:0x02105488, a different field
   path at 0x0211b9e0, a whole block skipped at 0x02126f58.

   All thirteen follow the call with `cmp r0, #0`.  That makes this the one
   slot in the campaign whose return value is not merely permitted by the ROM
   but seen to be consumed by it.

   `Virtual8C` is a placeholder after the +0x8c vtable offset, not a ROM name;
   as at slots 33 and 34 there was no `recovered name:` line here to correct.

   The rename had to be scoped: 0x020ad660 is an overlay load base, so ov000,
   ov002, ov003, ov004 and ov007 all have unrelated symbols at that address
   (ov003's is dScTitle_c's D1).  Only ov004's is this function. */
}
}

int dScMgBase_c::Virtual8C()
{
    return (param1 & 0xff) != 0;
}

// @symbol GetGameLanguage
namespace s20ad674 {
extern "C" {
typedef int s32;
extern s32 GetOwnerLanguage(void);
s32 GetGameLanguage(void)
{
    switch (GetOwnerLanguage())
    {
    case 5:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}
}
}

// @symbol func_ov004_020ad6f4
namespace s20ad6f4 {
extern "C" {
int func_ov004_020ad6f4(int *p)
{
    return p[25];
}
}
}

// @symbol func_ov004_020ad6fc
namespace s20ad6fc {
extern "C" {
extern int data_0209b308[];
extern unsigned func_ov004_020adc3c(void* c);
extern unsigned func_02013580(unsigned a, int b);
int func_ov004_020ad6fc(void* c, int v){
  int i;
  if(data_0209b308[2]==5){
    if(v<=0) return -1;
    for(i=0;i<5;i++){
      if(func_02013580(func_ov004_020adc3c(c), i) > (unsigned)v) return i;
    }
  } else {
    for(i=0;i<5;i++){
      if(func_02013580(func_ov004_020adc3c(c), i) < (unsigned)v) return i;
    }
  }
  return -1;
}
}
}

// @symbol func_ov004_020ad79c
namespace s20ad79c {
extern "C" {
extern int data_ov004_020beb68;
extern int data_0209b308[];
extern int data_0209b300[];
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
int func_ov004_020adc3c(void *c);
void func_02013568(int a, int b, int v);

void func_ov004_020ad79c(int r0arg, int r1arg) {
    int v0 = r0arg, v1 = r1arg;
    if (data_ov004_020beb68 == 0) return;
    if (v0 <= 0) {
        v0 = data_0209b308[5];
        v1 = data_0209b308[6];
    } else {
        if (v0 > 0x270f) v0 = 0x270f;
    }
    if (v1 <= 0) {
        v1 = data_0209b308[6];
    } else {
        if (v1 > 0x270f) v1 = 0x270f;
    }
    if (v0 == func_ov004_020ad8b8()) {
        if (v1 == func_ov004_020ad878()) return;
    }
    func_02013568(func_ov004_020adc3c((void*)data_ov004_020beb68), 0, v0);
    func_02013568(func_ov004_020adc3c((void*)data_ov004_020beb68), 1, v1);
    *(unsigned char*)data_0209b300 = 1;
}
}
}

// @symbol func_ov004_020ad878
namespace s20ad878 {
extern "C" {
extern int* data_ov004_020beb68;
extern int func_ov004_020adc3c(int* p);
extern int func_02013580(int a, int b);
int func_ov004_020ad878(void){
  int* p=data_ov004_020beb68;
  if(p==0) return 0;
  return func_02013580(func_ov004_020adc3c(p), 1);
}
}
}

// @symbol func_ov004_020ad8b8
namespace s20ad8b8 {
extern "C" {
extern int data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc3c(void);
extern int func_02013580(int a, int b);
int func_ov004_020ad8b8(void){
  if(data_ov004_020beb68 != 0){
    int r=func_02013580(func_ov004_020adc3c(),0);
    if(r==0) return data_0209b308[0x14/4];
    return r;
  }
  return 0;
}
}
}

// @symbol func_ov004_020ad90c
namespace s20ad90c {
extern "C" {
extern void func_ov004_020ad940(char *c);
extern int data_ov004_020beb68;

void func_ov004_020ad90c(void) {
    char *p = *(char **)&data_ov004_020beb68;
    if (p == 0) return;
    func_ov004_020ad940(p);
}
}
}

// @symbol func_ov004_020ad940
namespace s20ad940 {
extern "C" {
extern "C" {
int func_ov004_020adafc(void);
int func_ov004_020ad6fc(void* c, int v);
void func_ov004_020ada20(void);
void func_ov004_020ada40(int);
int func_ov004_020adc3c(void *c);
int func_02013580(int a, int b);
void func_02013568(int a, int b, int v);
int func_ov004_020adbc0(void);
extern unsigned char data_0209b300[];

void func_ov004_020ad940(char* c)
{
    int n = func_ov004_020ad6fc(c, func_ov004_020adafc());
    *(int*)(c+0x64) = n;
    if (*(int*)(c+0x64) < 0) {
        func_ov004_020ada20();
        *(int*)(c+0x4000+0x658) = func_ov004_020adafc();
        func_ov004_020ada40(0);
        return;
    }
    int i;
    for (i = 4; i > *(int*)(c+0x64); i--) {
        int a = func_ov004_020adc3c(c);
        int b = func_ov004_020adc3c(c);
        int v = func_02013580(b, i-1);
        func_02013568(a, i, v);
    }
    int a = func_ov004_020adc3c(c);
    int v = func_ov004_020adbc0();
    func_02013568(a, *(int*)(c+0x64), v);
    data_0209b300[0] = 1;
    func_ov004_020ada20();
    *(int*)(c+0x4000+0x658) = func_ov004_020adafc();
    func_ov004_020ada40(0);
}
}
}
}

// @symbol func_ov004_020ada20
namespace s20ada20 {
extern "C" {
extern int data_ov004_020beb68;

void func_ov004_020ada20(void) {
    char *ptr = *(char **)&data_ov004_020beb68;
    if (ptr != 0) {
        *(ptr + 0x4000 + 0x65c) = 0;
    }
}
}
}

// @symbol func_ov004_020ada40
namespace s20ada40 {
extern "C" {
struct Obj;
extern struct Obj *data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc1c(void);
extern void func_ov004_020adc00(int v);

void func_ov004_020ada40(int thiz, int a1, int a2)
{
    char *g = (char*)data_ov004_020beb68;
    if (g == 0) return;
    g += 0x4000;
    *(int*)(g + 0x654) = thiz;
    if (data_0209b308[2] == 5) {
        if (thiz == 0) return;
        if (func_ov004_020adc1c() != 0) {
            if ((unsigned)thiz >= (unsigned)func_ov004_020adc1c()) return;
        }
        func_ov004_020adc00(thiz);
        *((char*)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
        return;
    }
    if ((unsigned)thiz <= (unsigned)func_ov004_020adc1c()) return;
    func_ov004_020adc00(thiz);
    *((char*)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
}
}
}

// @symbol func_ov004_020adafc
namespace s20adafc {
extern "C" {
/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

int func_ov004_020adafc(void) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x4654);
    }
    return 0;
}
}
}

// @symbol func_ov004_020adb1c
namespace s20adb1c {
extern "C" {
extern int func_ov004_020adafc(void);
extern int func_ov004_020ada40(int);
extern char* data_ov004_020beb68[];
extern int data_0209b308[];
void func_ov004_020adb1c(int self){
  char* r2 = data_ov004_020beb68[0];
  if(r2 == 0) return;
  if(data_0209b308[2] == 5){
    *(int*)(r2 + 0x4000 + 0x64c) = self;
    if(func_ov004_020adafc() != 0){
      if((unsigned)self >= (unsigned)func_ov004_020adafc()) return;
    }
    func_ov004_020ada40(self);
  } else {
    int o = data_0209b308[3];
    if(o != 0){
      if((unsigned)self > (unsigned)o) self = o;
    }
    *(int*)(r2 + 0x4000 + 0x64c) = self;
    if((unsigned)self <= (unsigned)func_ov004_020adafc()) return;
    func_ov004_020ada40(self);
  }
}
}
}

// @symbol func_ov004_020adbc0
namespace s20adbc0 {
extern "C" {
/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

int func_ov004_020adbc0(void) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x464c);
    }
    return 0;
}
}
}

// @symbol func_ov004_020adbe0
namespace s20adbe0 {
extern "C" {
/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

int func_ov004_020adbe0(void) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        return *(unsigned char*)(val + 0x465c);
    }
    return 0;
}
}
}

// @symbol func_ov004_020adc00
namespace s20adc00 {
extern "C" {
/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

void func_ov004_020adc00(int v) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        *(int*)(val + 0x4650) = v;
    }
}
}
}

// @symbol func_ov004_020adc1c
namespace s20adc1c {
extern "C" {
/* The state block pointer is a char*: with an int-typed base mwcc materializes
   the +0x46xx offset from the literal pool instead of splitting it as
   add r0,r0,#0x4000 / ldr r0,[r0,#0x6xx] the way the cartridge does. */
extern char *data_ov004_020beb68;

int func_ov004_020adc1c(void) {
    char *val = data_ov004_020beb68;
    if (val != 0) {
        return *(int*)(val + 0x4650);
    }
    return 0;
}
}
}

// @symbol func_ov004_020adc3c
namespace s20adc3c {
extern "C" {
int func_ov004_020adc3c(void *c) {
    return (*(unsigned int*)((char*)c + 8) & 0xff00) >> 8;
}
}
}

// @symbol func_ov004_020adc4c
namespace s20adc4c {
extern "C" {
extern int data_ov004_020beb60[];
int func_ov004_020adc4c(void) { return data_ov004_020beb60[0]; }
}
}

// @symbol Ov004_Deallocate
namespace s20adc5c {
extern "C" {
extern void Deallocate(void *ptr);

void Ov004_Deallocate(void *ptr)
{
    Deallocate(ptr);
}
}
}

// @symbol func_ov004_020adc68
namespace s20adc68 {
extern "C" {
extern void LoadFile(void);
void func_ov004_020adc68(void) { LoadFile(); }
}
}

// @symbol func_ov004_020adc74
namespace s20adc74 {
extern "C" {
extern void *func_020182bc(const char *path);

void *func_ov004_020adc74(const char *path)
{
    return func_020182bc(path);
}
}
}

// @symbol _ZN3Ent19func_ov004_020adc80Ev
namespace s20adc80 {
extern "C" {
extern int GetGameLanguage(void);
extern int *data_ov004_020bbfa8[];
extern int func_ov004_020b1d60(int,int,int,long long);
}
}
void Ent::func_ov004_020adc80(){
    int idx=s20adc80::GetGameLanguage();
    int *t=s20adc80::data_ov004_020bbfa8[idx];
    s20adc80::func_ov004_020b1d60(t[2], ((int*)this)[0]>>0xc, ((int*)this)[1]>>0xc, -1);
}

// @symbol _ZN3Ent19func_ov004_020adcc8Ev
namespace s20adcc8 {
extern "C" {
extern void func_ov004_020b1d60(int a0,int a1,int a2,int a3,int a4);
extern int data_ov004_020beb70;
extern int data_ov004_020bea14[];
extern int data_ov004_020beba8[];
}
}
void Ent::func_ov004_020adcc8(){
  if((s20adcc8::data_ov004_020beb70 & 1) == 0){
    int* s = s20adcc8::data_ov004_020bea14;
    int* d = s20adcc8::data_ov004_020beba8;
    d[0]=s[0];d[1]=s[0];d[2]=s[0];d[3]=s[0];d[4]=s[0];
    d[5]=s[4];d[6]=s[4];d[7]=s[3];d[8]=s[2];d[9]=s[1];
    d[10]=s[2];d[11]=s[2];d[12]=s[3];d[13]=s[4];d[14]=s[4];d[15]=s[0];
    s20adcc8::data_ov004_020beb70 |= 1;
  }
  {
    int idx=((int)f)&0xf;
    int x=*(int*)this>>0xc; int y=*(int*)((char*)this+4)>>0xc;
    s20adcc8::func_ov004_020b1d60(s20adcc8::data_ov004_020beba8[idx],x,y,-1,-1);
  }
}

// @symbol func_ov004_020add88
namespace s20add88 {
extern "C" {
extern EntPMF data_ov004_020beb98[];
extern "C" void func_ov004_020add88(Ent *c) {
  (c->*data_ov004_020beb98[c->idx])();
}
}
}

// @symbol _ZN3Ent19func_ov004_020addccEv
namespace s20addcc {
extern "C" {
int _Z15ApproachLinear2Rsss(short *a, short b, short c);
void func_ov004_020b1b40(void *c);
int _ZN4cstd4fdivEii(int a, int b);
inline long long inline_fn(int arg0)
{
  return (long long) arg0;
}
}
}
void Ent::func_ov004_020addcc()
{
  char *r5 = (char *)this;
  using namespace s20addcc;
  {
  if (_Z15ApproachLinear2Rsss((short *) (r5 + 0x1a), 0, 1))
  {
    func_ov004_020b1b40((void *) 1);
  }
  int a = *((short *) (r5 + 0x1a));
  int b = *((short *) (r5 + 0x1c));
  int s = _ZN4cstd4fdivEii(a << 0xc, b << 0xc);
  int c3 = *((short *) (r5 + 0x1c));
  int c2 = *((short *) (r5 + 0x1a));
  int t = _ZN4cstd4fdivEii((c3 - c2) << 0xc, c3 << 0xc);
  int v8 = *((int *) (r5 + 8));
  int v10 = *((int *) (r5 + 0x10));
  long long m1 = inline_fn(v8) * inline_fn(s);
  long long m2 = inline_fn(v10) * inline_fn(t);
  long long m3 = inline_fn(s) * inline_fn(s);
  long long m4 = inline_fn((int) ((m3 + 0x800) >> 0xc)) * inline_fn((*((int *) (r5 + 0xc))) - (*((int *) (r5 + 0x14))));
  *((int *) r5) = ((int) ((m1 + 0x800) >> 0xc)) + ((int) ((m2 + 0x800) >> 0xc));
  *((int *) (r5 + 4)) = (*((int *) (r5 + 0x14))) + ((int) ((m4 + 0x800) >> 0xc));
  }
}

// @symbol _ZN3Ent19func_ov004_020adeb0Ev
namespace s20adeb0 {
extern "C" {
extern int _Z15ApproachLinear2Rsss(short* dst, short to, short step);
extern void func_0203d630(int* p, int m);
}
}
void Ent::func_ov004_020adeb0()
{
    char* c = (char*)this;
    s20adeb0::_Z15ApproachLinear2Rsss((short*)(c + 0x1a), 0, 1);
    s20adeb0::func_0203d630((int*)(c + 8), 0xff8);
    int *p = (int*)(((int)c + 8));
    int *p2 = (int*)(((int)c + 0xc));
    *p += *(int*)(c + 0x10);
    *p2 += *(int*)(c + 0x14);
    p = (int*)(((int)c + 4));
    *(int*)c += *(int*)(c + 8);
    *p += *(int*)(c + 0xc);
}

// @symbol func_ov004_020adf2c
namespace s20adf2c {
extern "C" {
extern EntPMF data_ov004_020beb88[];
extern "C" void func_ov004_020adf2c(Ent *c) {
  (c->*data_ov004_020beb88[c->idx])();
}
}
}

// @symbol func_ov004_020adf70
namespace s20adf70 {
extern "C" {
void func_ov004_020adf70(char* c, short a1, short a2, int* r3, int* sp0, int* sp4){
  *(short*)(c+0x1e)=a1;
  *(short*)(c+0x1a)=a2;
  *(short*)(c+0x1c)=*(short*)(c+0x1a);
  *(int*)(c)=r3[0];
  *(int*)(c+4)=r3[1];
  if(sp0){
    *(int*)(c+8)=sp0[0];
    *(int*)(c+0xc)=sp0[1];
  }
  if(sp4){
    *(int*)(c+0x10)=sp4[0];
    *(int*)(c+0x14)=sp4[1];
  }
}
}
}

// @symbol func_ov004_020adfc4
namespace s20adfc4 {
extern "C" {
extern void func_ov004_020adf70(void* tbl, char* c, short a1, short a2, int* r3, int* sp0);
void func_ov004_020adfc4(char* c, short a1, short a2, int* r3, int* sp0, int* sp4){
  if(a2==0) return;
  int i;
  for(i=0;i<0x40;i++){
    if(data_ov004_020bebe8[i].f == 0){
      func_ov004_020adf70(&data_ov004_020bebe8[i], c, a1, a2, r3, sp0);
      return;
    }
  }
}
}
}

namespace s20ae03c {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback3Ev
/* dScMgBase_c::graphCallback_c::GraphCallback3 -- slot 3, ov004.
 *
 * Forwards to the registered scene's own virtual at vtable offset +0x64
 * (slot 25) and reports handled.
 *
 * THE DISPATCH STAYS RAW. dScMgBase_c's slots 18-35 are eighteen virtuals whose
 * signatures are not reconstructed yet (see include/dScMgBase_c.h), so slot
 * 25 cannot be called by name -- declaring it would require declaring
 * slots 18-25 above it in order, and guessing those would misnumber the
 * table. The compiler owns this function's symbol, `this` and mScene; only the
 * callee's identity is still an offset.
 */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback3()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        void (*fn)(dScMgBase_c *) = *(void (**)(dScMgBase_c *))((char *)*(void **)p + 0x64);
        fn(p);
    }

    return 1;
}

namespace s20ae06c {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev
/* dScMgBase_c::graphCallback_c::GraphCallback2 -- slot 2, ov004 0x020ae06c.
 *
 * Unlike slots 0, 1 and 3, this one RETURNS the scene virtual's own result
 * rather than discarding it and reporting handled; it only reports handled
 * when no scene is registered.
 *
 * The dispatch stays raw for the reason the sibling slots give: dScMgBase_c's
 * slots 18-35 have no reconstructed signatures, so slot 24 (+0x60) cannot be
 * called by name.
 */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback2()
{
    dScMgBase_c *p = mScene;

    if (p == 0) {
        return 1;
    }

    return (*(int (**)(dScMgBase_c *))((char *)*(void **)p + 0x60))(p);
}

namespace s20ae0a4 {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback1Ev
/* dScMgBase_c::graphCallback_c::GraphCallback1 -- slot 1, ov004.
 *
 * Forwards to the registered scene's own virtual at vtable offset +0x58
 * (slot 22) and reports handled.
 *
 * THE DISPATCH STAYS RAW. dScMgBase_c's slots 18-35 are eighteen virtuals whose
 * signatures are not reconstructed yet (see include/dScMgBase_c.h), so slot
 * 22 cannot be called by name -- declaring it would require declaring
 * slots 18-22 above it in order, and guessing those would misnumber the
 * table. The compiler owns this function's symbol, `this` and mScene; only the
 * callee's identity is still an offset.
 */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback1()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        void (*fn)(dScMgBase_c *) = *(void (**)(dScMgBase_c *))((char *)*(void **)p + 0x58);
        fn(p);
    }

    return 1;
}

namespace s20ae0d4 {
extern "C" {
// @symbol _ZN11dScMgBase_c15graphCallback_c14GraphCallback0Ev
/* dScMgBase_c::graphCallback_c::GraphCallback0 -- slot 0, ov004.
 *
 * Forwards to the registered scene's own virtual at vtable offset +0x5c
 * (slot 23) and reports handled.
 *
 * THE DISPATCH STAYS RAW. dScMgBase_c's slots 18-35 are eighteen virtuals whose
 * signatures are not reconstructed yet (see include/dScMgBase_c.h), so slot
 * 23 cannot be called by name -- declaring it would require declaring
 * slots 18-23 above it in order, and guessing those would misnumber the
 * table. The compiler owns this function's symbol, `this` and mScene; only the
 * callee's identity is still an offset.
 */
}
}

int dScMgBase_c::graphCallback_c::GraphCallback0()
{
    dScMgBase_c *p = mScene;

    if (p != 0) {
        void (*fn)(dScMgBase_c *) = *(void (**)(dScMgBase_c *))((char *)*(void **)p + 0x5c);
        fn(p);
    }

    return 1;
}

// @symbol _ZN11dScMgBase_c15graphCallback_cC1Ev
/* graphCallback_c ctor -- the base callback_c vptr store, the derived vptr
   store, and mScene = 0 are the three writes the ROM body makes. */
dScMgBase_c::graphCallback_c::graphCallback_c() : mScene(0) {}

namespace s20ae128 {
extern "C" {
// @symbol _ZN11dScMgBase_c8OnPushedEv
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgBase_c_OnPushed
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnPushed - recovered from vtable slot identity.
   Converted from .c to .cpp: dScMgBase_c.h now includes the real dScene_c.h,
   which -- like dBase_c.h -- has no C spelling. Same shape as dScene_c.h's
   own two affected .c files in the prior slice. */
}
}

int dScMgBase_c::OnPushed()
{
    return mMenuOpen == 0;
}

namespace s20ae140 {
extern "C" {
// @symbol _ZN11dScMgBase_c8OnKickedEv
// recovered name: dScMgBase_c_OnKicked
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnKicked - recovered from vtable slot identity */
struct Obj {
    virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
    virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
    virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
    virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
    virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
    virtual void v50(); virtual void v54(); virtual void v58(); virtual void v5c();
    virtual void v60(); virtual void v64(); virtual void v68(); virtual void v6c();
    virtual void v70();
    virtual void method74();
    virtual void method78();
};
}
}

int dScMgBase_c::OnKicked()
{ using s20ae140::Obj;
    Obj *self = (Obj *)this;

    void* base = (char*)self + 0x4000;
    int v2 = *(int*)((char*)base + 0x628);
    int v1 = *(int*)((char*)base + 0x62c);
    if (v1 == v2) goto done;
    if (v2 == 0) {
        self->method78();
    } else {
        self->method74();
    }
    *(int*)((char*)self + 0x462c) = *(int*)((char*)self + 0x4628);
done:
    return 1;
}

namespace s20ae198 {
extern "C" {
// @symbol _ZN11dScMgBase_c11OnAttacked1Ev
// recovered name: dScMgBase_c_OnAttacked1
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnAttacked1 - recovered from vtable slot identity */
}
}

int dScMgBase_c::OnAttacked1()
{
    return 1;
}

namespace s20ae1a0 {
extern "C" {
// @symbol _ZN11dScMgBase_c11OnAttacked2Ev
// recovered name: dScMgBase_c_OnAttacked2
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnAttacked2 - recovered from vtable slot identity */
}
}

int dScMgBase_c::OnAttacked2()
{
    return 1;
}

// @symbol func_ov004_020ae1a8
namespace s20ae1a8 {
extern "C" {
extern int data_ov004_020beb68;
extern int data_0209b308[];
int func_ov004_020ae1a8(void) {
    if (data_ov004_020beb68 != 0) {
        return data_0209b308[2];
    }
    return -1;
}
}
}

// @symbol func_ov004_020ae1cc
namespace s20ae1cc {
extern "C" {
extern int data_ov004_020beb68;
extern int data_0209b308[];

int func_ov004_020ae1cc(void) {
    int r0 = data_ov004_020beb68;
    if (r0 != 0) {
        return *(int*)((char*)data_0209b308 + 0x2c);
    }
    return 0;
}
}
}

// @symbol func_ov004_020ae1f0
namespace s20ae1f0 {
extern "C" {
extern int *data_ov004_020beb68;
int func_ov004_020ae1f0(void) {
    int *g = data_ov004_020beb68;
    if (g != 0) return *(int *)((char *)g + 0xc8);
    return 5;
}
}
}

// @symbol func_ov004_020ae20c
namespace s20ae20c {
extern "C" {
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern int func_ov004_020ae1cc(void);
extern int func_ov004_020ae1f0(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int x);
extern int data_ov004_020beb68;
void func_ov004_020ae20c(void)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0, 0x7f, 0, 0x7f000, 1);
    if (data_ov004_020beb68 == 0) return;
    if (func_ov004_020ae1cc() != 1) return;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
}
}

// @symbol func_ov004_020ae274
namespace s20ae274 {
extern "C" {
extern int func_ov004_020ae1cc(void);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);

void func_ov004_020ae274(void* c) {
    extern int data_ov004_020beb68[];
    if (*data_ov004_020beb68 == 0) return;
    func_ov004_020ae1cc();
    _ZN5Sound7PlaySubEjjj5Fix12IiEb((unsigned int)c, 0x40, 0x7f, 0x7f000, 1);
}
}
}

// @symbol func_ov004_020ae2c8
namespace s20ae2c8 {
extern "C" {
extern "C" {
extern void *data_ov004_020beb68;
int func_ov004_020ae1cc(void);
int func_ov004_020ae1f0(void);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
void func_ov004_020ae2c8(void);
}
void func_ov004_020ae2c8(void) {
    if (data_ov004_020beb68 == 0) return;
    int r = func_ov004_020ae1cc();
    if (r == 0) {
        _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        return;
    }
    if (r != 1) return;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
}
}

// @symbol func_ov004_020ae330
namespace s20ae330 {
extern "C" {
extern int data_ov004_020beb68;
extern int func_ov004_020ae1cc(void);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
extern int func_ov004_020ae1f0(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int);
extern unsigned short data_0209b308;

void func_ov004_020ae330(void){
    if(data_ov004_020beb68==0) return;
    if(func_ov004_020ae1cc()==0){
        _ZN5Sound22StopLoadedMusic_Layer1Ej(1);
        if(data_0209b308 != 0x16f) return;
        _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
        return;
    }
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
}
}

namespace s20ae3b4 {
extern "C" {
// @symbol _ZN11dScMgBase_c9Virtual88Eiiii
/* dScMgBase_c::Virtual88 - slot 34.  THE BRUSH.

   Walks a `size` x `size` square centred on (cx, cy) and paints every cell
   inside it with palette index `colour`.  The address arithmetic is the DS
   4bpp background-character layout: `(x/8 + (y/8)*32)*32 + (y&7)*4` picks the
   word holding eight pixels of one row of one 8x8 tile, `(x&7)*4` is the
   nibble inside it, and MultiCopy_Int is used for the read and the write-back
   rather than a plain load/store.  Sixteen colours, one nibble per pixel.  All
   four sides clip.

   It reads exactly the two object fields slot 33 (Virtual84) initialises.
   `obj+0x6c` is the layer index: 0..3 select G2S::GetBG0CharPtr through
   GetBG3CharPtr, and anything else returns without drawing -- which is what
   slot 33's -1 buys.  `obj+0x68` gates the wrapped region above the touch
   screen, where the MAIN engine's G2::GetBG*CharPtr are used instead and y is
   folded by `+ data_ov004_020beb6c + 0xc0`.  So slot 33 leaves the brush
   disabled and a minigame arms it by choosing a layer.

   Its only in-family caller is ov004:0x020ae5c4, the line rasteriser sitting
   immediately after it in the image, which dispatches through +0x88 at seven
   separate sites as it walks a segment.  See the slot-34 block in
   include/dScMgBase_c.h for the arity and return-type measurements.

   `Virtual88` is a placeholder after the +0x88 vtable offset, not a ROM name --
   but unlike slots 29-32 there is no wrong name being retired here, because
   the recovery pass never guessed one for this address. */
extern "C" {
extern int* func_02054efc(void);
extern int* func_02054ea8(void);
extern int* _ZN2G213GetBG2CharPtrEv(void);
extern int* func_02054d88(void);
extern int* _ZN3G2S13GetBG0CharPtrEv(void);
extern int* _ZN3G2S13GetBG1CharPtrEv(void);
extern int* _ZN3G2S13GetBG2CharPtrEv(void);
extern int* _ZN3G2S13GetBG3CharPtrEv(void);
extern void MultiCopy_Int(void* a, void* b, int n);
}

extern "C" {
extern int data_ov004_020beb64;
extern int data_ov004_020beb6c;
}
}
}

void dScMgBase_c::Virtual88(int cx, int cy, int val, int n)
{ using s20ae3b4::MultiCopy_Int; using s20ae3b4::_ZN2G213GetBG2CharPtrEv; using s20ae3b4::_ZN3G2S13GetBG0CharPtrEv; using s20ae3b4::_ZN3G2S13GetBG1CharPtrEv; using s20ae3b4::_ZN3G2S13GetBG2CharPtrEv; using s20ae3b4::_ZN3G2S13GetBG3CharPtrEv; using s20ae3b4::data_ov004_020beb64; using s20ae3b4::data_ov004_020beb6c; using s20ae3b4::func_02054d88; using s20ae3b4::func_02054ea8; using s20ae3b4::func_02054efc;
    char *obj = (char *)this;

    int oi;
    int x0;
    int ci0;
    int m;
    int xx;
    int yy;
    int ci;
    int yrow;
    int four;
    int half;

    half = n / 2;
    oi = 0;
    if (n <= 0)
        return;
    x0 = cx - half;
    ci0 = oi;
    m = 0xf;
    yrow = cy - half;
    four = 4;
    do {
        ci = ci0;
        if (n > 0) {
            do {
                yy = yrow;
                xx = ci + x0;
                int* base;
                int* cell;
                int sh;

                if (xx < 0)
                    goto inc;
                if (xx >= 0x100)
                    goto inc;
                if (yrow < -0xc0 - data_ov004_020beb6c)
                    goto inc;
                if (yrow < -data_ov004_020beb6c) {
                    if (*(u8*)(obj + 0x68) == 0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = func_02054efc(); break;
                    case 1: base = func_02054ea8(); break;
                    case 2: base = _ZN2G213GetBG2CharPtrEv(); break;
                    case 3: base = func_02054d88(); break;
                    default: return;
                    }
                    yy += data_ov004_020beb6c + 0xc0;
                } else {
                    if (yrow < 0)
                        goto inc;
                    if (yrow >= 0xc0)
                        goto inc;
                    switch (*(int*)(obj + 0x6c)) {
                    case 0: base = _ZN3G2S13GetBG0CharPtrEv(); break;
                    case 1: base = _ZN3G2S13GetBG1CharPtrEv(); break;
                    case 2: base = _ZN3G2S13GetBG2CharPtrEv(); break;
                    case 3: base = _ZN3G2S13GetBG3CharPtrEv(); break;
                    default: return;
                    }
                }
                cell = (int*)((char*)base + ((xx / 8 + (yy / 8) * 32) * 32) + ((yy & 7) * 4));
                MultiCopy_Int(cell, &data_ov004_020beb64, four);
                sh = (xx & 7) << 2;
                data_ov004_020beb64 = (data_ov004_020beb64 & (-1 ^ (m << sh))) | (val << sh);
                MultiCopy_Int(&data_ov004_020beb64, cell, four);
            inc:;
            } while (++ci < n);
        }
        yrow += 1;
        oi += 1;
    } while (oi < n);
}

// @symbol func_ov004_020ae5c4
namespace s20ae5c4 {
extern "C" {
/* Minigame canvas line rasteriser (ov004 0x020ae5c4, 0x294 bytes). Walks a
 * Bresenham line from (x0, y0) to (x1, y1) and plots every point through the
 * canvas's virtual dScMgBase_c::Virtual88(x, y, colour, size) (vtable slot 34),
 * stepping along the major axis with the error term seeded at half the major
 * span; a zero-length line plots once. Callers: the trampoline minigame's
 * trajectory trace (src/minigames/d_s_mg_trampoline.cpp).
 *
 * Written against include/dScMgBase_c.h so the plot call is the real virtual
 * call: the ROM's ldr rN,[r0]; ldr rN,[rN,#0x88]; blx chain with the fifth
 * argument stored to the stack first is what the header's declaration
 * produces; a function-pointer temp evaluated before the call put the vtable
 * load before the stack argument and loaded it into a separate register.
 * The end row is copied into a local (yy1) that the vertical loop compares
 * against, the ROM's [sp, #0xc] slot; the horizontal loop compares x1 itself.
 * The two absolute values must be spelled as the copies first and the two
 * sign tests after (adx = dx; ady = dy; if (adx < 0) ...; if (ady < 0) ...):
 * that order colours dx/dy into r6/fp and lets the x-span test reuse the
 * flags of the dx subtraction (subs + rsbmi) while the y-span gets its own
 * cmp; testing them in the other order, or each right after its copy, swaps
 * the pair or the flag reuse. */

extern "C" void func_ov004_020ae5c4(dScMgBase_c *thiz, int x0, int y0, int x1, int y1, int colour, int size)
{
    int dx, dy, adx, ady, err;
    int yy1;
    if (x0 == x1 && y0 == y1) {
        thiz->Virtual88(x0, y0, colour, size);
        return;
    }
    yy1 = y1;
    thiz->Virtual88(x0, y0, colour, size);
    dy = y1 - y0;
    dx = x1 - x0;
    adx = dx;
    ady = dy;
    if (adx < 0) adx = -adx;
    if (ady < 0) ady = -ady;
    if (adx >= ady) {
        err = adx / 2;
        for (;;) {
            if (dx == 0) {
                thiz->Virtual88(x0, y0, colour, size);
                return;
            }
            if (dx > 0) {
                err += ady;
                x0++;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (x0 == x1) return;
            } else {
                err += ady;
                x0--;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (x0 == x1) return;
            }
        }
    } else {
        err = ady / 2;
        for (;;) {
            if (dy == 0) {
                thiz->Virtual88(x0, y0, colour, size);
                return;
            }
            if (dy > 0) {
                err += adx;
                y0++;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (y0 == yy1) return;
            } else {
                err += adx;
                y0--;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (y0 == yy1) return;
            }
        }
    }
}
}
}

#pragma push
#pragma opt_common_subs off
// @symbol func_ov004_020ae858
namespace s20ae858 {
extern "C" {
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern "C" {
int func_ov004_020b8f18(void *c);
int GetGameLanguage(void);
int func_02053200(int x);
int func_ov004_020aea78(void *a, void *b, void *c, void *d);
void func_ov004_020aea30(void *self, int a1, int a2, int a3);
}

extern int *data_ov004_020bc020[];
extern int *data_ov004_020bbfbc[];
extern int *data_ov004_020bbfd0[];
extern s16 data_02082214[];
extern int data_ov006_021346bc;

extern "C" void func_ov004_020ae858(char *self)
{
    int extra[3];
    s16 *tbl[3];
    int buf[4];
    int zero1;
    int zero2;
    int zero0;
    int i;

    if (func_ov004_020b8f18((void *)(self + 0xf4)) != 0)
        return;

    extra[0] = *data_ov004_020bc020[GetGameLanguage()];
    extra[1] = *data_ov004_020bbfbc[GetGameLanguage()];
    extra[2] = *data_ov004_020bbfd0[GetGameLanguage()];

    tbl[0] = (s16 *)(self + 0x4634);
    tbl[1] = (s16 *)(self + 0x4638);
    tbl[2] = (s16 *)(self + 0x463c);

    i = 0;
    zero1 = 0;
    zero2 = 0;
    zero0 = 0;

    for (; i < 3; i++) {
        if (i == *(s16 *)(self + 0x4646)) {
            unsigned int field = *(unsigned int *)(self + 0x4640);
            unsigned int shifted = (field << 0xf) >> 0x10;
            int idx0 = (int)shifted >> 4;
            int s0 = data_02082214[idx0 * 2];
            char *cursor = (char *)extra[i];
            int arg0;
            int result;
            s16 *pos;
            u16 v;
            int *pbuf = buf;

            arg0 = (int)((((s64)s0 << 11) + 0x800) >> 12) + 0x1000;
            result = func_02053200(arg0);

            {
                volatile int *vp = pbuf;
                vp[0] = 0;
                vp[1] = 0;
                vp[2] = 0;
                vp[3] = 0;
            }
            buf[0] = result;
            buf[3] = result;

            pos = tbl[i];
            do {
                func_ov004_020aea78((void *)cursor, (void *)(int)pos[0], (void *)(int)pos[1], (void *)pbuf);
                v = *(u16 *)(cursor + 6);
                cursor += 8;
            } while (v != 0xffff);

            pos = tbl[i];
            func_ov004_020aea30((void *)data_ov006_021346bc, pos[0], pos[1], zero0);
        } else {
            s16 *pos = tbl[i];
            char *ep = (char *)extra[i];
            func_ov004_020aea30((void *)ep, pos[0], pos[1], zero1);
            func_ov004_020aea30((void *)data_ov006_021346bc, pos[0], pos[1], zero2);
        }
    }
}
}
}
#pragma pop

// @symbol func_ov004_020aea30
namespace s20aea30 {
extern "C" {
// @symbol func_ov004_020aea30
/* recovered: named members + shared header, declarations from a shared header */
/* recovered: named members + shared header */
extern "C" void func_ov004_020aea30(void *self, void *actor, void *c2, void *c3)
{
    void *r8 = self;
    void *r7 = actor;
    void *r6 = c2;
    void *r5 = c3;
    u16 sentinel = 0xffff;
    do {
        func_ov004_020aea78(r8, r7, r6, r5);
        u16 v = *(u16*)((char*)r8 + 6);
        r8 = (char*)r8 + 8;
        if (v == sentinel) break;
    } while (1);
}
}
}

// @symbol func_ov004_020aea78
namespace s20aea78 {
extern "C" {
struct C {
    virtual int f0();
    virtual int f1();
    virtual int f2();
    virtual int f3();
    virtual int f4();
    virtual int f5();
    virtual int f6();
    virtual int f7();
    virtual int f8();
    virtual int f9();
    virtual int f10();
    virtual int f11();
    virtual int f12();
    virtual int f13();
    virtual int f14();
    virtual int f15();
    virtual int f16();
    virtual int f17();
    virtual int f18();
    virtual int f19();
    virtual int f20();
    virtual int f21();
    virtual int f22();
    virtual int f23();
    virtual int f24();
    virtual int f25();
    virtual int f26();
};
struct M { int _00, _01, _10, _11; };
extern "C" {
extern C* data_ov004_020beb68;
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
}

struct BF { unsigned int lo:10; unsigned int hi:22; };
struct S { int x; BF y; };

extern "C" void func_ov004_020aea78(S* self, int a1, int a2, struct M* a3)
{
    S v = *self;
    if (data_ov004_020beb68->f26() == 2) {
        v.y.lo = (self->y.lo - 0x200) & 0x3ff;
    }
    func_ov004_020b1c68(&v, a1, a2, 0, -1, a3);
}
}
}

// @symbol func_ov004_020aeb24
namespace s20aeb24 {
extern "C" {
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28();
};

extern "C" {
extern void func_02012790(int a);
extern int func_ov004_020b8f78(char* p);
extern int _Z15ApproachLinear2Rsss(short* v, short a, short b);
extern void func_0203b958(short* o, short* a, short* b);
extern int _Z14ApproachLinearRiii(int* v, int a, int b);
extern void func_02012dd0(int a);
extern void func_ov004_020b9220(char* p);

extern unsigned short data_020a0e5a[];
extern dWipe_c data_0209f61c;

void func_ov004_020aeb24(char* c)
{
    unsigned char idx = gActivePlayerSlot;
    unsigned short flags = data_020a0e5a[idx * 2];
    int unlocked;
    short tmp[2];
    short d0[2];
    short d1[2];
    short d2[2];
    short a, b;

    if ((flags & 0x40) || (flags & 0x80) || (flags & 0x20) || (flags & 0x10)) {
        func_02012790(0xe);
    }
    if (func_ov004_020b8f78(c + 0xf4) != 0) return;

    _Z15ApproachLinear2Rsss((short*)(c + 0x4634), 0x80, 0x10);
    _Z15ApproachLinear2Rsss((short*)(c + 0x4638), 0x80, 0x10);
    _Z15ApproachLinear2Rsss((short*)(c + 0x463e), 0x90, 5);

    idx = gActivePlayerSlot;
    unlocked = 0;
    if (gTouchHeld[idx * 4] != 0) {
        unlocked = (gTouchEdge[idx * 4] != 0);
    }
    if (unlocked == 0) goto after;
    if (*(short*)(c + 0x4646) >= 0) goto after;
    tmp[0] = gTouchX[idx * 4];
    tmp[1] = gTouchY[idx * 4];
    func_0203b958(d0, tmp, (short*)(c + 0x4634));
    func_0203b958(d1, tmp, (short*)(c + 0x4638));
    func_0203b958(d2, tmp, (short*)(c + 0x463c));
    a = d0[0]; if (a < 0) a = -a;
    if (a < 0x60) {
        b = d0[1]; if (b < 0) b = -b;
        if (b < 0x18) {
            func_02012790(0x62);
            *(short*)(c + 0x4646) = 0;
            *(short*)(c + 0x4644) = 0x10;
            goto after;
        }
    }
    a = d1[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d1[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x63);
            *(short*)(c + 0x4646) = 1;
            *(short*)(c + 0x4644) = 0;
            goto after;
        }
    }
    a = d2[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d2[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x62);
            *(short*)(c + 0x4646) = 2;
            *(short*)(c + 0x4644) = 0;
        }
    }

after:
    if (*(short*)(c + 0x4646) < 0) return;
    if (_Z14ApproachLinearRiii((int*)(c + 0x4640), 0x10000, 0x1000) == 0) return;
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x4644), 0, 1) == 0) return;

    switch (*(short*)(c + 0x4646)) {
    case 0:
        ((Obj*)c)->v28();
        return;
    case 1:
        dScene_c::SetFaders(&data_0209f61c);
        data_0209f61c.func_0202ec9c(1);
        dScene_c::StartSceneFade(5, 0, 0);
        if (*(int*)(c + 0x4648) != 0) return;
        func_02012dd0(0x3c);
        *(int*)(c + 0x4648) = 1;
        return;
    case 2:
        func_ov004_020b9220(c + 0xf4);
        *(short*)(c + 0x4646) = -1;
        *(int*)(c + 0x4640) = 0;
        return;
    default:
        ((Obj*)c)->v28();
        return;
    }
}
}
}
}

namespace s20aeed8 {
extern "C" {
// @symbol _ZN11dScMgBase_c25OnAimedAtWithEggReturnVecEv
// recovered name: dScMgBase_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations
   from a shared header */
/* dScMgBase_c::OnAimedAtWithEggReturnVec - slot 30.

   The 27-method shadow scaffold this file used to carry is gone.  Its only
   purpose was `o->f68()`, index 26 in a hand-counted stand-in vtable, and slot
   26 is dScMgBase_c::OnHitByCannonBlastedChar now -- so the call is spelled
   with its name and the scaffold is dead weight.

   WHAT IT DOES, because the name says the opposite of both halves.  This is
   the exact mirror of slot 29: 29 saves the screen and draws the three-item
   overlay menu over it, and this restores everything 29 saved.  POWCNT1's
   screen-swap bit comes back out of mSavedScreenSwap; both DISPCNTs get their
   layer-enable fields back from data_0209d460 / data_0209d458 and then their
   BG-enable fields back from mSavedMainBgBits / mSavedSubBgBits; 0x400 bytes
   of BG palette come back out of the buffer at +0x4228, the second 0x200 of it
   to the sub screen; and 0x2000 bytes of sub-screen OBJ VRAM go back to
   0x06606000 from +0x2228.  dScMgBase_c::OnKicked calls this one when
   mMenuOpen falls to zero and slot 29 when it rises.  It is the menu coming
   down.  It is not a Yoshi egg, and it does not return a vector -- see the
   slot-30 block in include/dScMgBase_c.h for the AAPCS measurement that
   refutes the `ReturnVec` in the name. */
extern "C" {
    extern void MultiCopy_Int(int*, int*, int);
}
extern "C" u8 data_0209d45c;
extern "C" u8 data_0209d454;
extern "C" u8 data_0209d460;
extern "C" u8 data_0209d458;

extern "C" int _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern "C" int _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);

/* RETURN TYPE, MEASURED -- and it corrects the header's earlier `int`, which
   that slot's own evidence block called "a HINT".  Declaring this member `int`
   and letting control fall off the end does not cost an instruction, but it
   does reserve r0 as the result register, and the closing block wants four
   scratch registers.  mwcc then allocates r1-r4 where the ROM allocates
   r0-r3, and fourteen of the function's ninety-three words differ with no
   other change:

       ROM   ldr r0,[pc,#0x38] / mov r3,#0x4000000 / ldr r1,[r3] / ...
       int   ldr r1,[pc,#0x38] / mov r4,#0x4000000 / ldr r2,[r4] / ...

   Spelled `void` it is byte-exact.  Both overrides and decl_common.h:2321
   already said void; the retail source did too. */
}
}

void dScMgBase_c::OnAimedAtWithEggReturnVec()
{ using s20aeed8::MultiCopy_Int; using s20aeed8::_ZN2GX10LoadBGPlttEPKvjj; using s20aeed8::_ZN3GXS10LoadBGPlttEPKvjj; using s20aeed8::data_0209d454; using s20aeed8::data_0209d458; using s20aeed8::data_0209d45c; using s20aeed8::data_0209d460;
    struct dScMgBase_c *self = this;
    char *c = (char *)this;

    data_0209d45c = 0;
    data_0209d454 = 0;

    *(u32*)0x4000000 &= ~0x1f00;
    *(u32*)0x4001000 &= ~0x1f00;
    *(u16*)0x4000304 = (self->mSavedScreenSwap << 15) | (*(u16*)0x4000304 & ~0x8000);
    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0xe000) | ((u32)data_0209d460 << 13);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0xe000) | ((u32)data_0209d458 << 13);

    _ZN2GX10LoadBGPlttEPKvjj(c + 0x4228, 0x20, 0x1e0);
    {
        char *p = c + 0x4228; p += 0x200;
        _ZN3GXS10LoadBGPlttEPKvjj(p, 0, 0x80);
    }

    if (self->OnHitByCannonBlastedChar() != 2)
    {
        char *src = (char*)0x6600000; src += 0x6000;
        MultiCopy_Int((int*)(c + 0x2228), (int*)src, 0x2000);
    }

    data_0209d45c = (u8)self->mSavedMainBgBits;
    data_0209d454 = (u8)self->mSavedSubBgBits;

    if (self->OnHitByCannonBlastedChar() == 2)
        return;

    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0x1f00) | ((u32)data_0209d45c << 8);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0x1f00) | ((u32)data_0209d454 << 8);
}

namespace s20af04c {
extern "C" {
// @symbol _ZN11dScMgBase_c19OnHitFromUnderneathEv
// recovered name: dScMgBase_c_OnHitFromUnderneath
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* dScMgBase_c::OnHitFromUnderneath - recovered from vtable slot identity.

   THE 27-METHOD SHADOW SCAFFOLD IS GONE, and declaring slot 26 is what let it
   go.  This file used to carry a local `struct Base` of twenty-six placeholder
   virtuals plus `struct Obj : Base` with a `char pad[0x4627]`, existing for one
   reason: so that `self->Init()` would compile to a load of vtable+0x68.  Slot
   26 is OnHitByCannonBlastedChar and dScMgBase_c declares it now, so the class
   itself does that job and the call can be written for what it is.  The two
   raw offsets go with the scaffold -- 0x4628 is mMenuOpen and 0xf4 is
   mTouchOptions, both named fields on the real struct.

   Reading it back: a hit from underneath closes the three-item overlay menu
   that OnHitByMegaChar opens, resets the polymorphic touch-icon set, and
   re-enables the 3D engines only if the class's own OnHitByCannonBlastedChar
   says so. */

extern "C" void func_02012e1c(char *c);
extern "C" void Enable3dEngines();

/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
}
}

int dScMgBase_c::OnHitFromUnderneath()
{ using s20af04c::Enable3dEngines; using s20af04c::func_02012e1c;
    void *c = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    func_02012e1c((char *)self);
    self->mMenuOpen = 0;
    func_ov004_020b91fc((char *)&self->mTouchOptions);
    int r = self->OnHitByCannonBlastedChar();
    if (r != 0) Enable3dEngines();
}

namespace s20af094 {
extern "C" {
// @symbol _ZN11dScMgBase_c16OnAimedAtWithEggEv
// recovered name: dScMgBase_c_OnAimedAtWithEgg
/* recovered: renamed to Class_Method, RTTI class fields named, declarations
   from a shared header */
/* dScMgBase_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   THE 27-METHOD SHADOW SCAFFOLD IS GONE.  This file used to carry a local
   `struct Base` of twenty-six placeholder virtuals plus `struct Obj : Base`
   with a `char pad[0x4700]`, existing only so that `self->Init()` would compile
   to a load of vtable+0x68.  Slot 26 is OnHitByCannonBlastedChar and
   dScMgBase_c declares it now, so the class does that job and both calls can be
   written for what they are.  Every field this body touches was already named
   in the header before this change -- mSavedMainBgBits's comment there names
   this function -- so those offsets go too.  The two buffers at +0x2228 and
   +0x4228 stay as raw offsets: they are inside pad_228, and this body is the
   only evidence for their extent.

   WHAT IT DOES, worth writing down because the NAME DOES NOT SAY IT.
   `OnAimedAtWithEgg` is transplanted from include/dActor_c.h at the same slot
   index; nothing in the cartridge carries a method name for this class.  The
   body saves the screen and then draws over it.  It stashes POWCNT1's
   screen-swap bit and the two BG-enable bytes into the object, clears every
   layer- and window-enable bit in both DISPCNTs, saves 0x400 bytes of BG
   palette out of palette RAM into +0x4228 and loads the menu's palettes over
   the top, then saves 0x2000 bytes of sub-screen OBJ VRAM at 0x06606000 into
   +0x2228 and decompresses a per-language image over that.  Its one caller in
   this module is dScMgBase_c::OnKicked, which reaches it only on a rising edge
   of mMenuOpen.  That is the three-item overlay menu going up, not a Yoshi egg.
   See the slot 29 block in include/dScMgBase_c.h for why the name is kept. */
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *addr, unsigned int size);
extern "C" void _ZN2GX10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void MultiStore16(unsigned short val, char *dst, int nbytes);
extern "C" void func_0201f32c(int arg0);
extern "C" int GetGameLanguage(void);
extern "C" void DecompressLZ16(void *src, void *dst);

extern "C" unsigned char data_0209d45c;
extern "C" unsigned char data_0209d454;

/* The parameter is spelled void* to agree with decl_common.h, which the two
   overrides reach this symbol through; `this` is recovered on the first line. */
/* THE EARLY EXITS ARE SPELT AS NESTED IFS, NOT `return;`. mwccarm accepts a
   valueless `return` in a non-void function; C++ does not, and no host option
   reaches it (MSVC C2561). The ROM sets no return value on these paths -- it
   leaves r0 holding whatever the last call left there and branches straight to
   the epilogue -- so the faithful shape is a body that reaches its closing
   brace with nothing to return, which is what the host already accepts for the
   rest of this family. Byte-identical under 2004/b56: the compiled object is
   unchanged. */
}
}

int dScMgBase_c::OnAimedAtWithEgg()
{ using s20af094::DecompressLZ16; using s20af094::GetGameLanguage; using s20af094::MultiStore16; using s20af094::_ZN2GX10LoadBGPlttEPKvjj; using s20af094::_ZN3GXS10LoadBGPlttEPKvjj; using s20af094::_ZN4CP1527FlushAndInvalidateDataCacheEjj; using s20af094::data_0209d454; using s20af094::data_0209d45c; using s20af094::func_0201f32c;
    void *cv = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)cv;
    char *c = (char *)cv;

    if (self->OnHitByCannonBlastedChar() != 0)
        func_02019028();

    self->mSavedScreenSwap = (*(volatile u16 *)0x4000304 & 0x8000) >> 15;

    self->mSavedMainBgBits = data_0209d45c;
    self->mSavedSubBgBits = data_0209d454;

    *(volatile u16 *)0x4000304 |= 0x8000;
    data_0209d45c = 0;
    data_0209d454 = 0;

    *(volatile int *)0x4000000 &= ~0x1f00;
    *(volatile int *)0x4001000 &= ~0x1f00;
    *(volatile int *)0x4000000 &= ~0xe000;
    *(volatile int *)0x4001000 &= ~0xe000;

    MultiCopy_Int((int *)0x5000020, (int *)(c + 0x4228), 0x1e0);
    {
        int *ip4228 = (int *)(c + 0x4228);
        MultiCopy_Int((int *)0x5000400, ip4228 + 0x80, 0x80);
    }
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x4228), 0x400);

    _ZN2GX10LoadBGPlttEPKvjj((const void *)data_ov004_020bea28, 0x20, 0xa0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)data_ov004_020beac8, 0, 0x80);

    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000000, 2);
    }
    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000400, 2);
    }

    func_0201f32c(self->mSceneKind);

    data_0209d45c = 0x12;
    data_0209d454 = 0x10;

    if (self->OnHitByCannonBlastedChar() != 2) {
        int *vbase = (int *)0x6600000;
        int *vram = vbase + 0x1800;
        MultiCopy_Int(vram, (int *)(c + 0x2228), 0x2000);
        int idx = GetGameLanguage();
        DecompressLZ16(data_ov004_020bbf94[idx], vram);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x2228), 0x2000);
    }
}

namespace s20af27c {
extern "C" {
// @symbol _ZN11dScMgBase_c15OnHitByMegaCharEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: dScMgBase_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnHitByMegaChar - recovered from vtable slot identity.
   Converted from .c to .cpp -- see the banner on
   src/minigames/d_s_mg_base.cpp. */
}
}

void dScMgBase_c::OnHitByMegaChar()
{
    void *c = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    if (self->unk_4630 != 0) return;
    func_02012e78();
    self->mMenuItem0X = -0x80;
    self->mMenuItem0Y = 0x30;
    self->mMenuItem1X = 0x180;
    self->mMenuItem1Y = 0x60;
    self->mMenuItem2X = 0x80;
    self->mMenuItem2Y = 0xe0;
    self->mPromptEnabled = 0;
    self->mMenuOpen = 1;
    self->mMenuCursor = -1;
    self->mMenuCursorPhase = 0;
}

// @symbol func_ov004_020af2f8
namespace s20af2f8 {
extern "C" {
extern "C" {
extern int _ZN2G212GetBG0ScrPtrEv(void);
extern void MultiStore16(int val, int ptr, int n);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern int _ZN2G212GetBG1ScrPtrEv(void);
extern int _ZN3G2S12GetBG1ScrPtrEv(void);
extern int _ZN2G212GetBG2ScrPtrEv(void);
extern int _ZN3G2S12GetBG2ScrPtrEv(void);
extern int _ZN2G212GetBG3ScrPtrEv(void);
extern int _ZN3G2S12GetBG3ScrPtrEv(void);
extern int func_02054efc(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int func_02054ea8(void);
extern int _ZN3G2S13GetBG1CharPtrEv(void);
extern int _ZN2G213GetBG2CharPtrEv(void);
extern int _ZN3G2S13GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern int _ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);

extern char data_ov004_020b9468;

void func_ov004_020af2f8(char* sb, char en, int mode, int base)
{
    volatile unsigned short v0, v1, v2, v3, v4, v5, v6, v7;
    volatile unsigned short w0, w1, w2, w3, w4, w5, w6, w7;
    int val;
    int i;
    int off;
    int t;

    *(unsigned char*)(sb + 0x68) = en;
    i = 0;
    off = i;
    *(int*)(sb + 0x6c) = mode;
    val = i;
    val += base << 12;

    for (; i < 0x300; i++, val++, off += 2) {
        switch (*(int*)(sb + 0x6c)) {
        case 0:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG0ScrPtrEv();
                t = t + off;
                MultiStore16(v0 = val, t, 2);
            }
            t = _ZN3G2S12GetBG0ScrPtrEv();
            t = t + off;
            MultiStore16(v1 = val, t, 2);
            break;
        case 1:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG1ScrPtrEv();
                t = t + off;
                MultiStore16(v2 = val, t, 2);
            }
            t = _ZN3G2S12GetBG1ScrPtrEv();
            t = t + off;
            MultiStore16(v3 = val, t, 2);
            break;
        case 2:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG2ScrPtrEv();
                t = t + off;
                MultiStore16(v4 = val, t, 2);
            }
            t = _ZN3G2S12GetBG2ScrPtrEv();
            t = t + off;
            MultiStore16(v5 = val, t, 2);
            break;
        case 3:
            if (*(unsigned char*)(sb + 0x68)) {
                t = _ZN2G212GetBG3ScrPtrEv();
                t = t + off;
                MultiStore16(v6 = val, t, 2);
            }
            t = _ZN3G2S12GetBG3ScrPtrEv();
            t = t + off;
            MultiStore16(v7 = val, t, 2);
            break;
        default:
            return;
        }
    }

    switch (*(int*)(sb + 0x6c)) {
    case 0:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054efc();
            w0 = 0;
            MultiStore16(w0, t, 0x6000);
        }
        t = _ZN3G2S13GetBG0CharPtrEv();
        w1 = 0;
        MultiStore16(w1, t, 0x6000);
        break;
    case 1:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054ea8();
            w2 = 0;
            MultiStore16(w2, t, 0x6000);
        }
        t = _ZN3G2S13GetBG1CharPtrEv();
        w3 = 0;
        MultiStore16(w3, t, 0x6000);
        break;
    case 2:
        if (*(unsigned char*)(sb + 0x68)) {
            t = _ZN2G213GetBG2CharPtrEv();
            w4 = 0;
            MultiStore16(w4, t, 0x6000);
        }
        t = _ZN3G2S13GetBG2CharPtrEv();
        w5 = 0;
        MultiStore16(w5, t, 0x6000);
        break;
    case 3:
        if (*(unsigned char*)(sb + 0x68)) {
            t = func_02054d88();
            w6 = 0;
            MultiStore16(w6, t, 0x6000);
        }
        t = _ZN3G2S13GetBG3CharPtrEv();
        w7 = 0;
        MultiStore16(w7, t, 0x6000);
        break;
    default:
        return;
    }

    _ZN2GX10LoadBGPlttEPKvjj(&data_ov004_020b9468, base << 5, 0x20);
    _ZN3GXS10LoadBGPlttEPKvjj(&data_ov004_020b9468, base << 5, 0x20);
}
}
}
}

#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov004_020af5e0
namespace s20af5e0 {
extern "C" {
typedef struct Item {
    u32 w0;
    u32 val : 10;
    u32 rest : 22;
} Item;

int func_ov004_020af5e0(Item* src, Item* dst, int add)
{
    int n = 1;
    int i = 0;
    Item* q = dst;
    do {
        u32 w;
        u32* p;
        dst[i].w0 = src->w0;
        *(u32*)((char*)&dst[i] + 4) = *(u32*)((char*)src + 4);
        w = (add + src->val) & 0x3ff;
        p = (u32*)((char*)q + 4);
        *p = (*p & ~0x3ff) | ((u32)(w) & 0x3ff);
        if (*(u16*)((char*)src + 6) == 0xffff)
            break;
        if (i == 0x1f) {
            *(u16*)((char*)&dst[i] + 6) = 0xffff;
            break;
        }
        i++;
        src++;
        n++;
        q++;
    } while (i < 0x20);
    return n;
}
}
}
#pragma pop

/* OAM::Render is a C++-linkage member of the global OAM namespace, so its
 * declaration sits outside the address namespace and its extern "C" block. */
struct OamAttr; struct Matrix2x2;
namespace OAM {
void Render(bool draw, OamAttr *obj, int px, int py, int pal, int prio, Matrix2x2 *mtx);
void RenderSub(OamAttr *data, s32 x, s32 y, s32 palette, s32 priority);
}

// @symbol Hud_RenderSprite
namespace s20af68c {
extern "C" {
extern "C" {
extern void *data_ov004_020beb68;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, OamAttr*, s32, s32, int, int, int, int, int, int);
void Hud_RenderSprite(void* attr, int x, int y, int palette, int priority){
  dScMgBase_c *scene = (dScMgBase_c *)data_ov004_020beb68;
  if(scene == 0) return;
  if(scene->mMenuOpen == 0){
    if(scene->OnHitByCannonBlastedChar() == 2){
      if(*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 0) return;
      _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr*)attr, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
      return;
    }
  }
  OAM::RenderSub((OamAttr*)attr, x, y, palette, priority);
}
}
}
}

// @symbol func_ov004_020af770
namespace s20af770 {
extern "C" {
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
}
typedef int Fix12i;
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, Fix12i e, int f);

extern "C" int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return 0;
    if (*(int*)(g + 0x4628) == 0) {
        int (*vf)(char*) = *(int(**)(char*))(*(char**)g + 0x68);
        if (vf(g) == 2) {
            if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) != 0) return 0;
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
}
}
}

// @symbol func_ov004_020af868
namespace s20af868 {
extern "C" {
extern "C" {
extern char* data_ov004_020beb68;
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
typedef int (*GetFn)(void*);
void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) != 0) return;
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, a3, a4, a5);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, a0, a1, a2, a3, a4, a5);
}
}
}
}

// @symbol func_ov004_020af948
namespace s20af948 {
extern "C" {
extern "C" {
extern void* data_ov004_020beb68;
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool b, void* a, int x, int y, int z, int w, void* m);

void func_ov004_020af948(void* a, int b, int c, void* m)
{
    if (data_ov004_020beb68 == 0) return;
    if (*(int*)((char*)data_ov004_020beb68 + 0x4628) == 0) {
        int (**vt)(void*) = *(int(***)(void*))data_ov004_020beb68;
        if (vt[0x68/4]((void*)data_ov004_020beb68) == 2) {
            if (*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 0) return;
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, a, b, c, -1, -1, m);
            return;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(true, a, b, c, -1, -1, m);
}
}
}
}

// @symbol RenderOamMainScreen
namespace s20afa20 {
extern "C" {
extern "C" {
extern void *data_ov004_020beb68;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, OamAttr*, s32, s32, int, int, int, int, int, int);

extern "C" void RenderOamMainScreen(int oam, int x, int y, int palette, int priority)
{
    dScMgBase_c *scene = (dScMgBase_c *)data_ov004_020beb68;
    if (scene == 0) return;
    if (scene->mMenuOpen == 0) {
        if (scene->OnHitByCannonBlastedChar() == 2) {
            if (*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 1) return;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr*)oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (OamAttr*)oam, x, y, palette, priority, 0x1000, 0x1000, 0, -1);
}
}
}
}

// @symbol func_ov004_020afb20
namespace s20afb20 {
extern "C" {
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
}
typedef int Fix12i;
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, Fix12i e, int f);

extern "C" int func_ov004_020afb20(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return 0;
    if (*(int*)(g + 0x4628) == 0) {
        int (*vf)(char*) = *(int(**)(char*))(*(char**)g + 0x68);
        if (vf(g) == 2) {
            if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) != 1) return 0;
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
}
}
}

// @symbol func_ov004_020afc18
namespace s20afc18 {
extern "C" {
extern "C" {
extern char* data_ov004_020beb68;
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
typedef int (*GetFn)(void*);
void func_ov004_020afc18(void* a0, int a1, int a2, int a3, int a4, void* a5){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) != 1) return;
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, a3, a4, a5);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, a3, a4, a5);
}
}
}
}

// @symbol DrawOamSprite
namespace s20afcf8 {
extern "C" {
struct Base {
    virtual int v00(); virtual int v01(); virtual int v02(); virtual int v03();
    virtual int v04(); virtual int v05(); virtual int v06(); virtual int v07();
    virtual int v08(); virtual int v09(); virtual int v10(); virtual int v11();
    virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
    virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19();
    virtual int v20(); virtual int v21(); virtual int v22(); virtual int v23();
    virtual int v24(); virtual int v25(); virtual int m();
};
extern "C" Base *data_ov004_020beb68;

extern "C" void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3)
{
    Base *g = data_ov004_020beb68;
    if (g == 0) return;
    if (*(int*)((char*)g + 0x4628) == 0 && g->m() == 2) {
        if (*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) != 1) return;
        OAM::Render(false, (OamAttr*)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2*)arg3);
        return;
    }
    OAM::Render(false, (OamAttr*)arg0, (int)arg1, arg2, -1, -1, (Matrix2x2*)arg3);
}
}
}

// @symbol func_ov004_020afdd0
namespace s20afdd0 {
extern "C" {
extern "C" {
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
int _ZN3OAM9RenderSubEP7OamAttriiii(void*, int, int, int, int);
typedef int (*GetFn)(void*);
void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) == 1){
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, 0x1000, 0x1000, 0, -1);
        return;
      }
      _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2, a3, a4, 0x1000, 0x1000, 0, -1);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, 0x1000, 0x1000, 0, -1);
  _ZN3OAM9RenderSubEP7OamAttriiii(a0, a1, a2, a3, a4);
}
}
}
}

// @symbol func_ov004_020aff38
namespace s20aff38 {
extern "C" {
// func_ov004_020aff38 at 0x020aff38
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
}
typedef int Fix12i;
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, Fix12i e, int f);

extern "C" int func_ov004_020aff38(struct OamAttr* a0, int a1, int a2, int a3, int a4, Fix12i a5, int a6)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return 0;
    if (*(int*)(g + 0x4628) == 0 && (*(int(**)(char*))(*(char**)g + 0x68))(g) == 2) {
        if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) == 1) {
            int hp = data_ov004_020beb6c;
            if (a2 >= -0x100 - hp && a2 < -hp) {
                return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
            }
            if (a2 >= -0x40 && a2 < 0xc0) {
                return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2, a3, a4, a5, a6);
            }
        }
    } else {
        int hp = data_ov004_020beb6c;
        if (a2 >= -0x100 - hp && a2 < -hp) {
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
        }
        if (a2 >= -0x40 && a2 < 0xc0) {
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return 0;
}
}
}

// @symbol RenderOamBothScreens
namespace s20b0104 {
extern "C" {
extern "C" {
extern void *data_ov004_020beb68;
extern int data_ov004_020beb6c;
void RenderOamBothScreens(void* attr, int x, int y, int palette, int priority, void* mtx){
  dScMgBase_c *scene = (dScMgBase_c *)data_ov004_020beb68;
  if(scene == 0) return;
  if(scene->mMenuOpen == 0){
    if(scene->OnHitByCannonBlastedChar() == 2){
      if(*(unsigned short*)((char*)data_ov004_020beb68 + 0x4664) == 1){
        OAM::Render(false, (OamAttr*)attr, x, y + 0xc0 + data_ov004_020beb6c, palette, priority, (Matrix2x2*)mtx);
        return;
      }
      OAM::Render(false, (OamAttr*)attr, x, y, palette, priority, (Matrix2x2*)mtx);
      return;
    }
  }
  OAM::Render(false, (OamAttr*)attr, x, y + 0xc0 + data_ov004_020beb6c, palette, priority, (Matrix2x2*)mtx);
  OAM::Render(true, (OamAttr*)attr, x, y, palette, priority, (Matrix2x2*)mtx);
}
}
}
}

// @symbol func_ov004_020b023c
namespace s20b023c {
extern "C" {
// func_ov004_020b023c at 0x020b023c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
extern "C" {
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
typedef int (*GetFn)(void*);
void func_ov004_020b023c(void* a0, int a1, int a2, int a3, void* a4){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) == 1){
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, -1, a4);
        return;
      }
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, a3, -1, a4);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, -1, a4);
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, a0, a1, a2, a3, -1, a4);
}
}
}
}

// @symbol func_ov004_020b0380
namespace s20b0380 {
extern "C" {
// func_ov004_020b0380 at 0x020b0380
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
extern "C" {
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
typedef int (*GetFn)(void*);
void func_ov004_020b0380(void* a0, int a1, int a2, void* a3){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) == 1){
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, -1, -1, a3);
        return;
      }
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, -1, -1, a3);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, -1, -1, a3);
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, a0, a1, a2, -1, -1, a3);
}
}
}
}

// @symbol func_ov004_020b04c0
namespace s20b04c0 {
extern "C" {
extern int data_ov004_020beb6c[];
int func_ov004_020b04c0(void) { return data_ov004_020beb6c[0]; }
}
}

// @symbol func_ov004_020b04d0
namespace s20b04d0 {
extern "C" {
extern int data_ov004_020beb6c[];
void func_ov004_020b04d0(int v) { data_ov004_020beb6c[0] = v; }
}
}

namespace s20b04e0 {
extern "C" {
// @symbol _ZN11dScMgBase_c24OnHitByCannonBlastedCharEv
// recovered name: dScMgBase_c_OnHitByCannonBlastedChar
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnHitByCannonBlastedChar - slot 26.  The recovered name above is
 * this one's own and is correct; the two overrides' were not, and both are
 * corrected where they live.
 *
 * `return 0;` here, `return 1;` in dScMgSingle3DBase_c's override and
 * `return 2;` in dScMgD3DBase_c's.  Three distinct constants out of three
 * bodies is what makes the return type int rather than void, and it is why the
 * caller has to be able to tell the three apart. */
}
}

int dScMgBase_c::OnHitByCannonBlastedChar()
{
    return 0;
}

// @symbol _ZN11dScMgBase_c16OnPendingDestroyEv
void dScMgBase_c::OnPendingDestroy()
{
}

// @symbol _ZN11dScMgBase_c6RenderEv
s32 dScMgBase_c::Render()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c12BeforeRenderEv
/* Draws the menu instead of the scene while it is open. The two loops draw
 * the three UI records in two passes, those with 0x30 clear first. */
int dScMgBase_c::BeforeRender()
{
    int i; char *p; int i2; char *p2; int j;

    if (dScene_c::BeforeRender() == 0)
        return 0;

    if (mMenuOpen != 0) {
        func_ov004_020ae858(this);
        return 0;
    }

    if (mStateController.unk_024 == 0) {
        p = data_ov004_020bf648;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (*(int *)(p + 0x20) == 0x1d) continue;
            if (*(s16 *)(p + 0x30) != 0) continue;
            func_ov004_020b31b4(p);
        }
        p2 = data_ov004_020bf648;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (*(int *)(p2 + 0x20) == 0x1d) continue;
            if (*(s16 *)(p2 + 0x30) == 0) continue;
            func_ov004_020b31b4(p2);
        }
        mStateController.Render();
    }

    p = (char *)data_ov004_020bebe8;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (data_ov004_020bebe8[j].f != 0)
            func_ov004_020add88(p);
    }

    func_ov004_020b0de0(this);
    return 1;
}

// @symbol _ZN11dScMgBase_c8BehaviorEv
s32 dScMgBase_c::Behavior()
{
    return 1;
}

// @symbol _ZN11dScMgBase_c14BeforeBehaviorEv
/* Handles input and the menu, runs the UI state controller and the shared
 * records, approaches unk_0ac toward unk_0a8, and advances the 40-frame
 * counter. Returns 0 to stop the derived scene's behavior this frame. */
int dScMgBase_c::BeforeBehavior()
{
    int mode;
    unsigned short flags;

    if (dScene_c::BeforeBehavior() == 0)
        return 0;

    if (data_0209f5bc->IsActive()) {
        mode = gActivePlayerSlot;
        flags = data_020a0e5a[mode * 2];
        if ((flags & 8) || (flags & 4) || (flags & 1) || (flags & 2)) {
            if (mMenuOpen != 0) {
                if (mMenuCursor < 0) {
                    if (func_ov004_020b8ee0((char *)&mTouchOptions))
                        OnHitFromUnderneath();
                }
            } else {
                if (gTouchHeld[mode * 4] == 0)
                    OnHitByMegaChar();
            }
        }
    }

    if (mMenuOpen != 0) {
        func_ov004_020aeb24((char *)this);
        return 0;
    }
    if (unk_462c != 0)
        return 0;

    if (mStateController.unk_024 == 0) {
        int i;
        char *g;
        mStateController.Behavior();
        g = data_ov004_020bf648;
        for (i = 0; i < 3; i++) {
            if (*(int *)(g + 0x20) != 0x1d)
                func_ov004_020b321c(g);
            g += 0x134;
        }
    }

    {
        int j;
        char *p = (char *)data_ov004_020bebe8;
        Ent *base = data_ov004_020bebe8;
        for (j = 0; j < 0x40; j++) {
            if (base[j].f != 0)
                func_ov004_020adf2c(p);
            p += 0x20;
        }
    }

    if (unk_0a4 == 0) {
        if (data_0209f5bc->IsActive() == 0)
            return 0;
    }

    ApproachLinear(unk_0ac, unk_0a8, data_0208ee44);

    mFrameCounter++;
    if (mFrameCounter >= 0x28)
        mFrameCounter = 0;

    return 1;
}

// @symbol _ZN11dScMgBase_c21AfterCleanupResourcesEj
void dScMgBase_c::AfterCleanupResources(u32 vfSuccess)
{
    if (vfSuccess == 2) {
        if (((int *)data_0209b308)[4] == 0)
            func_ov004_020ad90c();
        data_0209d4a8 = 0;
        data_ov004_020beb74.mScene = 0;
        FreeGfxSlotsById(0x1d);
        if (data_ov004_020beb60 != 0) {
            func_0203cbc0(data_ov004_020beb60);
            data_ov004_020beb60 = 0;
        }
        if (mMenuOpen != 0) {
            func_02012e1c();
            Sound::StopLoadedMusic_Layer1(1);
        }
        func_ov004_020b2c84();
    }
    dScene_c::AfterCleanupResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c18AfterInitResourcesEj
/* Finishes the display and font setup, then reports to dScene_c. */
void dScMgBase_c::AfterInitResources(u32 vfSuccess)
{
    Virtual80();
    LoadFont(2);
    func_ov004_020ae330();
    dScene_c::AfterInitResources(vfSuccess);
}

// @symbol _ZN11dScMgBase_c19BeforeInitResourcesEv
/* Sets up the scene and both displays. Slots 26, 31 and 33 must stay
 * virtual calls so derived scenes can override them. The block at 0x4000 is
 * padding in the header, so it is still passed by offset. */
bool dScMgBase_c::BeforeInitResources()
{
    if (dScene_c::BeforeInitResources() == 0) return 0;
    if (OnHitByCannonBlastedChar() == 0)
        func_02019028();
    else
        Enable3dEngines();
    unk_0c8 = *(int*)(data_0209b308 + 0x28);
    if (data_ov004_020beb60 == 0)
        data_ov004_020beb60 = _ZN6Memory13operator_new2Ej(0x4000);
    if (data_ov004_020beb68 != 0)
        *(int*)((char*)data_ov004_020beb68 + 0xb0) = 0;
    mHudScore = 0;
    unk_0b8 = 0;
    unk_465c = 0;
    func_ov004_020b8a8c((char *)this + 0x4000);
    Virtual84();
    func_ov004_020b2cb8();
    dScene_c::SetFaders(&data_0209f61c);
    data_0209f61c.func_0202ec9c(0);
    data_0209d460[0] = 0;
    data_0209d458[0] = 0;
    Virtual7C();
    return 1;
}

// @symbol func_ov004_020b0a38
namespace s20b0a38 {
extern "C" {
extern int *data_ov004_020beb68[];

void func_ov004_020b0a38(void)
{
    int *p = (int *)data_ov004_020beb68[0];
    if (p == 0) return;
    *(int *)((char *)p + 0xf0) = 0;
}
}
}

// @symbol func_ov004_020b0a54
namespace s20b0a54 {
extern "C" {
extern "C" void func_ov004_020b0a54(s32 state)
{
    dScMgBase_c *scene = (dScMgBase_c *)data_ov004_020beb68;
    if (!scene)
        return;
    scene->mStateController.SetState(state);
    ((dScMgBase_c *)data_ov004_020beb68)->unk_060 = 0;
}
}
}

// @symbol FreeGfxSlotsById
namespace s20b0aa0 {
extern "C" {
extern void func_ov004_020b3194(void *c);
struct E
{
  char pad[0x20];
  int f20;
  char pad2[0x134 - 0x24];
};
extern struct E data_ov004_020bf648[];
void FreeGfxSlotsById(int arg)
{
  int i;
  struct E *p;
  i = arg;
  if (i == 0x1d)
  {
    p = data_ov004_020bf648;
    for (i = 0; i < 3; i++)
    {
      if (p->f20 != 0x1d)
      {
        func_ov004_020b3194(p);
      }
      p++;
    }

  }
  else
  {
    p = data_ov004_020bf648;
    for (i = 0; i < 3; i++)
    {
      if (arg == p->f20)
      {
        func_ov004_020b3194(p);
      }
      p++;
    }

  }
}
}
}

// @symbol func_ov004_020b0b1c
namespace s20b0b1c {
extern "C" {

struct Vec2 {
    short x;
    short y;
};

struct Obj004 {
    char pad0[0x10];
    struct Vec2 pos;  /* 0x10 */
    char pad14[0xc];
    int f20;          /* 0x20 */
    char pad24[0xe];
    unsigned short f32;/* 0x32 */
    char pad34[0x100];
};

extern struct Obj004 data_ov004_020bf648[3];
extern int data_ov004_020bc150;
extern int data_ov004_020beb68;

extern void func_ov004_020b422c(struct Obj004 *o);
extern int func_ov004_020b40c0(struct Obj004 *o);

// The ROM scales idx into the addressing mode here, which only the 4-byte-record view of the lane reproduces.
#define TOUCH_REC(lane) ((unsigned char (*)[4])(lane))
int func_ov004_020b0b1c(int arg) {
    int idx = gActivePlayerSlot;
    int flag = 0;
    int i;

    if (TOUCH_REC(gTouchHeld)[idx][0] != 0) {
        if (TOUCH_REC(gTouchEdge)[idx][0] != 0) flag = 1;
    }

    if (flag) {
        struct Obj004 *o = data_ov004_020bf648;
        int cx = TOUCH_REC(gTouchX)[idx][0];
        int cy = TOUCH_REC(gTouchY)[idx][0];
        for (i = 0; i < 3; i++, o++) {
            if (arg == o->f20 && o->f32 != 0) {
                volatile struct Vec2 pos;
                int dx, dy;
                short ty;
                pos.x = o->pos.x;
                ty = o->pos.y;
                dx = pos.x - cx;
                pos.y = ty;
                dy = pos.y - cy;
                if (dx <= 0x40 && dx >= -0x40 && dy <= 0x13 && dy >= -0x13) {
                    if (data_ov004_020bc150 != 0) {
                        func_ov004_020b422c(o);
                        if ((unsigned int)(arg - 1) <= 1) {
                            /* The state block pointer is read as a char*: an int-typed base
                               makes mwcc materialize 0x4630 from the literal pool and store
                               with a register index (strls r4,[r1,r0]); the cartridge splits
                               it as addls r0,r0,#0x4000 / strls r4,[r0,#0x630]. */
                            *(int *)(*(char **)&data_ov004_020beb68 + 0x4000 + 0x630) = 1;
                        }
                    }
                }
            }
        }
    } else {
        int j;
        struct Obj004 *o = data_ov004_020bf648;
        for (j = 0; j < 3; j++, o++) {
            if (arg == o->f20) {
                if (func_ov004_020b40c0(o) != 0) {
                    if (arg == 1) {
                    } else if (arg == 2) {
                    } else {
                        data_ov004_020bc150 = 1;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}
}
}

// @symbol func_ov004_020b0cac
namespace s20b0cac {
extern "C" {
struct Elem { char pad[0x134]; };
extern struct Elem data_ov004_020bf648[];
extern void func_ov004_020b3278(struct Elem *e, int c, int a1, int a2, int a3, int s1, short s2);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6) {
    int i;
    for (i = 0; i < 3; i++) {
        if (*(int*)((char*)&data_ov004_020bf648[i] + 0x20) == 0x1d) {
            func_ov004_020b3278(&data_ov004_020bf648[i], c, a1, a2, a3, arg5, arg6);
            return;
        }
    }
}
}
}

// @symbol func_ov004_020b0d30
namespace s20b0d30 {
extern "C" {
extern void FreeGfxSlotsById(int);
extern Ent data_ov004_020bebe8[];
extern char data_ov004_020bf3e8[];

void func_ov004_020b0d30(void) {
    FreeGfxSlotsById(0x1d);
    int i = 0;
    do {
        data_ov004_020bebe8[i].f = 0;
        i++;
    } while (i < 0x40);
    int j = 0;
    char* p = data_ov004_020bf3e8;
    do {
        j++;
        *p++ = 0;
    } while (j < 3);
}
}
}

// @symbol func_ov004_020b0d8c
namespace s20b0d8c {
extern "C" {
extern s32 GetGameLanguage(void);
extern void Hud_RenderSprite(void *fn, s32 a, s32 b, s32 r3arg, s32 stack_arg);
extern void **data_ov004_020bbfa8[];

void func_ov004_020b0d8c(void *c, s32 arg1, s32 arg2)
{
    s32 idx1 = *(s32*)((char*)c + 0x5c);
    s32 r4 = (idx1 < 0x14) ? 0x11 : 0x12;
    s32 idx2 = GetGameLanguage();
    void **table = (void**)data_ov004_020bbfa8[idx2];
    void *fn = table[r4];
    Hud_RenderSprite(fn, arg1, arg2, -1, -1);
}
}
}

// @symbol func_ov004_020b0de0
namespace s20b0de0 {
extern "C" {
typedef int s32;
s32 GetGameLanguage(void);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern char* data_ov004_020bbfa8[];

#define LAU(p) ((int)(p))

void func_ov004_020b0de0(char* c)
{
    if (*(unsigned char*)(c + 0xc3) == 0)
        return;
    if ((unsigned int)*(unsigned char*)(c + 0xc4) < 4U) {
        unsigned short* p = (unsigned short*)LAU(c + 0xc0);
        *p = (unsigned short)(*p + 1);
        if ((unsigned int)*(unsigned short*)(c + 0xc0) >= 0x30U) {
            *(unsigned short*)(c + 0xc0) = 0U;
            {
                unsigned char* q = (unsigned char*)LAU(c + 0xc4);
                *q = (unsigned char)(*q + 1);
            }
        }
        if ((unsigned int)*(unsigned short*)(c + 0xc0) >= 0x18U)
            return;
    }
    {
        int i = GetGameLanguage();
        RenderOamMainScreen(*(int*)(data_ov004_020bbfa8[i] + 0x28), 0xc0, 0xb0, -1, -1);
    }
}
}
}

// @symbol func_ov004_020b0e84
namespace s20b0e84 {
extern "C" {
extern "C" void func_02012790(int);
extern "C" int GetGameLanguage(void);
extern "C" void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov004_020b1ea4(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
extern "C" void *data_ov004_020bbfa8[];

static inline int sdiv4(int d) {
    int t = d + (int)((unsigned)(d >> 1) >> 0x1e);
    return t >> 2;
}

static inline int sdiv2(int e) {
    return (e + (int)((unsigned)e >> 31)) >> 1;
}

extern "C" void func_ov004_020b0e84(Obj *sl, int sb) {
    void *r8;
    int r7;
    int i;
    int pin;

    {
        int t = sl->unk50;
        if (t < sb) {
            sl->unk54 = 1;
            sl->unk50 = sb;
            func_02012790(0x1bb);
        } else if (t > sb) {
            sl->unk54 = 0;
            sl->unk50 = sb;
        } else if (sl->unk54 > 0) {
            if (sl->f68() == 2) {
                sl->unk54 += 2;
            } else {
                sl->unk54 += 1;
            }
        }
    }

    r8 = data_ov004_020bbfa8[GetGameLanguage()];
    r7 = 0xa8;

    if (sb < 5) {
        r7 -= 8;
        i = 0;
        if (sb > 0) {
            do {
                if (i == sb - 1 && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < sb);
        }
        if (sl->unk54 >= sl->unk58) {
            sl->unk54 = 0;
        }
        return;
    }

    if (sb == 5) {
        int u = sl->unk54;
        if (u == 0) {
            func_ov004_020b1ea4(0xf0, r7, 5, 1, 0, 0, 0x14);
            RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
        } else if (u < sl->unk58) {
            r7 -= 8;
            i = 0;
            do {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < 4);
            if (sl->unk54 % 0x3c < 0x1e) {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
            }
        } else {
            int d = (sl->unk58 + 0x20) - sl->unk54;
            r7 -= sdiv4(d);
            i = 0;
            do {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                {
                    unsigned e = (sl->unk58 + 0x20) - sl->unk54;
                    r7 -= sdiv2((int)e);
                }
                i += 1;
            } while (i < 5);
        }
        if (sl->f68() == 2) {
            int b = sl->unk58;
            int c = sl->unk54;
            if (c == b || c == b + 1) {
                func_02012790(0x1ba);
            }
        } else if (sl->unk54 == sl->unk58) {
            func_02012790(0x1ba);
        }
        if (sl->unk54 >= sl->unk58 + 0x20) {
            sl->unk54 = 0;
        }
        return;
    }

    {
        int rem = sb % 5;
        if (rem == 0) {
            int u = sl->unk54;
            if (u == 0) {
                func_ov004_020b1ea4(0xf0, r7, sb, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
            } else if (u < sl->unk58) {
                func_ov004_020b1ea4(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
                r7 -= 0x18;
                i = 0;
                do {
                    if (i == 4 && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                        break;
                    }
                    RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                    i += 1;
                    r7 -= 0x10;
                } while (i < 5);
            } else {
                int d;
                func_ov004_020b1ea4(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
                d = (sl->unk58 + 0x20) - sl->unk54;
                r7 -= sdiv4(d);
                i = 0;
                do {
                    unsigned e = (sl->unk58 + 0x20) - sl->unk54;
                    r7 -= sdiv2((int)e);
                    RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                    i += 1;
                } while (i < 5);
            }
            if (sl->f68() == 2) {
                int b = sl->unk58;
                int c = sl->unk54;
                if (c == b || c == b + 1) {
                    func_02012790(0x1ba);
                }
            } else if (sl->unk54 == sl->unk58) {
                func_02012790(0x1ba);
            }
            if (sl->unk54 >= sl->unk58 + 0x20) {
                sl->unk54 = 0;
            }
            return;
        }

        {
            int base5 = (sb / 5) * 5;
            func_ov004_020b1ea4(0xf0, r7, base5, 1, 0, 0, 0x14);
        }
        RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
        r7 -= 0x18;
        sb = 0;
        if (rem > 0) {
            pin = rem - 1;
            do {
                if (sb == pin && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                sb += 1;
                r7 -= 0x10;
            } while (sb < rem);
        }
        if (sl->unk54 >= sl->unk58) {
            sl->unk54 = 0;
        }
    }
}
}
}

// @symbol func_ov004_020b14f0
namespace s20b14f0 {
extern "C" {
typedef long long s64;

#define AT(p,off) ((int*)(int)((char*)(p)+(off)))

extern int func_ov004_020ad6f4(void* self, int a);
extern int func_ov004_020adc3c(void* self);
extern int func_02013580(int a, int b);
extern void func_ov004_020b1710(void* self, int a1, int a2, int a3);

void func_ov004_020b14f0(char* self)
{
    int a2 = 0x28;
    int n;
    int i;
    int j;
    int k;

    *AT(self, 0x60) += 1;
    if (*(int*)(self + 0x60) >= 0x8c)
        *(int*)(self + 0x60) = 0x64;

    n = func_ov004_020ad6f4(self, *(int*)(self + 0x4658));
    if (n < 0)
        goto neg;

    i = 0;
    if (n > 0)
    {
        do
        {
            if (*(int*)(self + 0x60) >= i * 0x14)
                func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), i));
            a2 += 0x18;
            i++;
        } while (i < n);
    }

    if (*(int*)(self + 0x60) >= n * 0x14)
    {
        if (*(int*)(self + 0x60) < 0x64 || *(int*)(self + 0x60) - 0x64 < 0x14)
            func_ov004_020b1710(self, 0xa8, a2, *(int*)(self + 0x4658));
        else
            func_ov004_020b1710(self, 0xa8, a2, -1);
    }

    a2 += 0x18;
    j = n + 1;
    if (j >= 5)
        return;
    do
    {
        if (*(int*)(self + 0x60) >= j * 0x14)
            func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), j));
        a2 += 0x18;
        j++;
    } while (j < 5);
    return;

neg:
    k = 0;
    do
    {
        if (*(int*)(self + 0x60) >= k * 0x14)
            func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), k));
        a2 += 0x18;
        k++;
    } while (k < 5);

    if (*(int*)(self + 0x60) < 0x64)
        return;

    if (*(int*)(self + 0x60) - 0x64 < 0x14)
        func_ov004_020b1710(self, 0xa8, a2 + 0x10, *(int*)(self + 0x4658));
    else
        func_ov004_020b1710(self, 0xa8, a2 + 0x10, -1);
}
}
}

// @symbol func_ov004_020b1710
namespace s20b1710 {
extern "C" {
extern int GetGameLanguage(void);
extern void func_ov004_020b1ea4(int x, int a1, int val, int a3, int a4, int mode, int fb);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);

extern int data_0209b308;
extern int *data_ov004_020bbfa8;

void func_ov004_020b1710(int a0, int x, int y, int val)
{
    int idx = GetGameLanguage();
    int mode = (&data_0209b308)[2];
    int *tbl = (int *)(&data_ov004_020bbfa8)[idx];
    int flag = 0;

    if (mode == 5) {
        int major;
        int rem;
        int pct;
        if (val < 0)
            return;
        major = val / 60;
        func_ov004_020b1ea4(x - 0x18, y, major, flag, -1, 1, 0xa);
        Hud_RenderSprite((void *)tbl[7], x - 0x10, y, -1, -1);

        rem = val % 60;
        pct = rem * 100;
        func_ov004_020b1ea4(x - 8, y, pct / 600, 0, -1, 1, 0xa);

        rem = val % 60;
        pct = rem * 100 / 60;
        func_ov004_020b1ea4(x, y, pct % 10, 0, -1, 1, 0xa);
        return;
    }

    if (mode == 3)
        flag = 1;
    switch (mode) {
    case 1:
        Hud_RenderSprite((void *)tbl[9], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 2:
        Hud_RenderSprite((void *)tbl[2], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    case 3:
        flag = 1;
        break;
    case 4:
        Hud_RenderSprite((void *)tbl[0x13], 0x58, y, -1, -1);
        Hud_RenderSprite((void *)tbl[1], 0x68, y, -1, -1);
        break;
    default:
        break;
    }

    if (flag == 1)
        x -= 4;
    if (val < 0)
        return;
    if (flag == 1)
        func_ov004_020b1ea4(x, y, val, 0, -1, 1, 0xa);
    else
        func_ov004_020b2444(x, y, val, 0, -1, 1, 0);
}
}
}

// @symbol func_ov004_020b19f0
namespace s20b19f0 {
extern "C" {
typedef int s32;
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int func_ov004_020b1ea4(int a, int b, void* c, int d, int e, int f, int g);
extern char* data_ov004_020bbfa8[];
int func_ov004_020b19f0(void* self){
  int i = GetGameLanguage();
  RenderOamMainScreen(*(int*)(data_ov004_020bbfa8[i]+0x20), 0x20, 0xc, -1, -1);
  return func_ov004_020b1ea4(0x48, 0xc, self, 1, -1, 2, 0xa);
}
}
}

// @symbol func_ov004_020b1a5c
namespace s20b1a5c {
extern "C" {
/* func_ov004_020b1a5c at 0x020b1a5c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
 */
typedef int s32;
s32 GetGameLanguage(void);
int RenderOamMainScreen(int a, int b, int c, int d, int e);
int func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern int data_ov004_020b944c[];
extern char* data_ov004_020bbfa8[];
void func_ov004_020b1a5c(int a0, int a1){
  int lim = data_ov004_020b944c[a1];
  int c = a0;
  int r4 = 0xf4;
  int i;
  if (c >= lim) c = lim;
  func_ov004_020b1ea4(0xf4, 0xc, c, 1, -1, 1, 0xa);
  r4 = r4 - (0x44 - (6 - a1) * 8);
  i = GetGameLanguage();
  RenderOamMainScreen(*(int*)(data_ov004_020bbfa8[i]+0x10), r4, 0xc, -1, -1);
}
}
}

// @symbol func_ov004_020b1aec
namespace s20b1aec {
extern "C" {
extern int *data_ov004_020beb68;
int func_ov004_020b1aec(void) {
    int *g = data_ov004_020beb68;
    if (g != 0) return *(int *)((char *)g + 0xbc);
    return 0;
}
}
}

// @symbol func_ov004_020b1b08
namespace s20b1b08 {
extern "C" {
extern void *data_ov004_020beb68;
extern void func_ov004_020b1b78(void *a0, void *a1);
void func_ov004_020b1b08(void *c) {
    void *v = data_ov004_020beb68;
    if (v == 0) return;
    func_ov004_020b1b78(v, c);
}
}
}

// @symbol func_ov004_020b1b40
namespace s20b1b40 {
extern "C" {
extern void *data_ov004_020beb68;
extern void func_ov004_020b1ba0(void *a0, void *a1);
void func_ov004_020b1b40(void *c) {
    void *v = data_ov004_020beb68;
    if (v == 0) return;
    func_ov004_020b1ba0(v, c);
}
}
}

// @symbol func_ov004_020b1b78
namespace s20b1b78 {
extern "C" {
void func_ov004_020b1b78(void* c, int val) {
    int r2 = *(int*)((char*)c + 0xa8);
    r2 -= val;
    if (r2 < 0) r2 = 0;
    else if (r2 > 0x270f) r2 = 0x270f;
    *(int*)((char*)c + 0xa8) = r2;
}
}
}

// @symbol func_ov004_020b1ba0
namespace s20b1ba0 {
extern "C" {
void func_ov004_020b1ba0(void *c, s32 delta)
{
    s32 v = *(s32*)((char*)c + 0xa8) + delta;
    if (v < 0) v = 0;
    else if (v > 0x270f) v = 0x270f;
    *(s32*)((char*)c + 0xa8) = v;
}
}
}

// @symbol func_ov004_020b1bc8
namespace s20b1bc8 {
extern "C" {
extern void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3)
{
    func_ov004_020b1de8(a1, a2, a3, -1);
    int v = *(int*)(a0+0xac);
    if (v < 0x64) {
        func_ov004_020b2444(a1+0x12, a2, v, a3, -1, 2, 0);
    } else {
        func_ov004_020b1ea4(a1+0xc, a2, v, a3, -1, 2, 0x32);
    }
}
}
}

// @symbol func_ov004_020b1c68
namespace s20b1c68 {
extern "C" {
struct M { int _00, _01, _10, _11; };
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020afc18(void* a0, int a1, int a2, int a3, int a4, void* a5);
void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5){
  if(a3==-1){
    RenderOamBothScreens(a0, a1, a2, -1, a4, a5);
    return;
  }
  if(a3==0){
    func_ov004_020af868(a0, a1, a2, -1, a4, a5);
    return;
  }
  func_ov004_020afc18(a0, a1, a2, -1, a4, a5);
}
}
}

// @symbol func_ov004_020b1cf0
namespace s20b1cf0 {
extern "C" {
void func_ov004_020afdd0(int a, int b, int c, int d, int e);
void Hud_RenderSprite(int a, int b, int c, int d, int e);
void RenderOamMainScreen(int a, int b, int c, int d, int e);
void func_ov004_020b1cf0(int a, int b, int c, int sel, int e){
  if(sel == -1){ func_ov004_020afdd0(a,b,c,-1,e); return; }
  if(sel == 0){ Hud_RenderSprite(a,b,c,-1,e); return; }
  RenderOamMainScreen(a,b,c,-1,e);
}
}
}

// @symbol func_ov004_020b1d60
namespace s20b1d60 {
extern "C" {
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020afc18(void* a0, int a1, int a2, int a3, int a4, void* a5);
void func_ov004_020b1d60(void* a0, int a1, int a2, int a3, int a4){
  if(a3==-1){
    RenderOamBothScreens(a0, a1, a2, -1, a4, 0);
    return;
  }
  if(a3==0){
    func_ov004_020af868(a0, a1, a2, -1, a4, 0);
    return;
  }
  func_ov004_020afc18(a0, a1, a2, -1, a4, 0);
}
}
}

// @symbol func_ov004_020b1de8
namespace s20b1de8 {
extern "C" {
extern int GetGameLanguage(void);
extern int *data_ov004_020bbfa8[];
extern int func_ov004_020b1d60(int, int, int, int, int);

void func_ov004_020b1de8(int r0, int r1, int r2, int r3) {
    int idx = GetGameLanguage();
    int *t = data_ov004_020bbfa8[idx];
    func_ov004_020b1d60(t[2], r0, r1, r2, r3);
}
}
}

// @symbol func_ov004_020b1e34
namespace s20b1e34 {
extern "C" {
/* func_ov004_020b1e34 @ 0x20b1e34 (ov004) -- veneer: ldr r1,[r0,#0xb4]; b func_ov004_020b0e84. */
extern void func_ov004_020b0e84(void*, u32);

void func_ov004_020b1e34(void* a, int unused1, int unused2, int unused3) {
    func_ov004_020b0e84(a, *(u32*)((char*)a + 0xb4));
}
}
}

// @symbol func_ov004_020b1e44
namespace s20b1e44 {
extern "C" {
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int GetGameLanguage(void);
extern void DrawOamSprite(int a, int b, int c, int d);
extern int* data_ov004_020bbfa8[];
void func_ov004_020b1e44(int arg) {
    int r;
    func_ov004_020b2444(0x80, 0xc, arg, 1, -1, 0, 0);
    r = GetGameLanguage();
    DrawOamSprite(data_ov004_020bbfa8[r][3], 0x60, 0xc, 0);
}
}
}

// @symbol func_ov004_020b1ea4
namespace s20b1ea4 {
extern "C" {
extern int data_ov006_02137cd8[];
extern void func_ov004_020b1d60(void *glyph, int x, int a2, int a3, int a4);

void func_ov004_020b1ea4(int x, int a1, int val, int a3, int a4, int mode, int fb) {
    int d[6];
    int i;
    if (val >= 0xf423f) val = 0xf423f;
    for (i = 0; i < 6; i++) d[i] = 0;
    d[0] = val;
    while (d[0] >= 0x186a0) { d[5]++; d[0] -= 0x186a0; }
    while (d[0] >= 0x2710) { d[4]++; d[0] -= 0x2710; }
    while (d[0] >= 0x3e8) { d[3]++; d[0] -= 0x3e8; }
    while (d[0] >= 0x64) { d[2]++; d[0] -= 0x64; }
    while (d[0] >= 0xa) { d[1]++; d[0] -= 0xa; }
    if (fb == 0) fb = 0xa;
    else if (fb == 0) fb = 0xa;
    else if (fb == 0x14) fb = 0x1e;
    else if (fb == 0xa) fb = 0xa;

    if (d[5] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x14;
        else if (mode == 1) x -= 0x28;
        r5 = 5;
        for (k = 0; k < 5; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[4] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0x10;
        else if (mode == 1) x -= 0x20;
        r5 = 4;
        for (k = 0; k < 4; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[3] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 0xc;
        else if (mode == 1) x -= 0x18;
        r5 = 3;
        for (k = 0; k < 3; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[2] != 0) {
        int r5;
        int k;
        if (mode == 0) x -= 8;
        else if (mode == 1) x -= 0x10;
        r5 = 2;
        for (k = 0; k < 2; k++) {
            func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[r5]], x, a1, a3, a4);
            x += 8;
            r5--;
        }
    } else if (d[1] != 0) {
        if (mode == 0) x -= 4;
        else if (mode == 1) x -= 8;
        func_ov004_020b1d60((void*)data_ov006_02137cd8[fb + d[1]], x, a1, a3, a4);
        x += 8;
    }
    func_ov004_020b1d60((void*)data_ov006_02137cd8[d[0] + fb], x, a1, a3, a4);
}
}
}

// @symbol func_ov004_020b2220
#pragma push
#pragma opt_propagation off
namespace s20b2220 {
extern "C" {
/* Draws a score number rotated by an angle: splits the value (clamped to
   9999) into thousands, hundreds, tens and units, builds the rotation-and-
   scale matrix from the sine table, and draws the digits centred on x: four
   digits from x-0x30, three from x-0x20, two from x-0x10, a lone unit at x.
   The fixed-point multiply and the 2x2 rotation are the NitroSDK FX_Mul and
   MTX_Rot22 shapes, written as local inlines. Spelled that way the
   allocator keeps the thousands counter in r0 as the cartridge does; with
   the products written out in the body it lands in ip or r3. */
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
extern s16 data_02082214[];
extern int data_ov006_02137cd8[];

static inline int FX_Mul(int v1, int v2)
{
    s64 t = (s64)v1 * v2;
    return (int)((t + 0x800) >> 12);
}

static inline void MTX_Rot22(struct M *p, int sinVal, int cosVal)
{
    p->_00 = cosVal;
    p->_01 = sinVal;
    p->_10 = -sinVal;
    p->_11 = cosVal;
}

static inline s16 FX_SinIdx(int idx) { return data_02082214[idx << 1]; }
static inline s16 FX_CosIdx(int idx) { return data_02082214[(idx << 1) + 1]; }

void func_ov004_020b2220(int x, int y, int value, int a3, int a4, int scale, u16 angle)
{
    struct M m;
    int idx;
    int te, hu, th;

    if (value >= 9999) value = 9999;
    te = hu = th = 0;
    while (value >= 1000) { value -= 1000; th++; }
    while (value >= 100) { value -= 100; hu++; }
    while (value >= 10) { value -= 10; te++; }

    idx = angle >> 4;
    MTX_Rot22(&m, FX_Mul(FX_SinIdx(idx), scale), FX_Mul(FX_CosIdx(idx), scale));

    if (th != 0) {
        func_ov004_020b1c68((void *)data_ov006_02137cd8[th], x - 0x30, y, a3, a4, &m);
        func_ov004_020b1c68((void *)data_ov006_02137cd8[hu], x - 0x10, y, a3, a4, &m);
        func_ov004_020b1c68((void *)data_ov006_02137cd8[te], x + 0x10, y, a3, a4, &m);
        x += 0x30;
    } else if (hu != 0) {
        func_ov004_020b1c68((void *)data_ov006_02137cd8[hu], x - 0x20, y, a3, a4, &m);
        func_ov004_020b1c68((void *)data_ov006_02137cd8[te], x, y, a3, a4, &m);
        x += 0x20;
    } else if (te != 0) {
        func_ov004_020b1c68((void *)data_ov006_02137cd8[te], x - 0x10, y, a3, a4, &m);
        x += 0x10;
    }
    func_ov004_020b1c68((void *)data_ov006_02137cd8[value], x, y, a3, a4, &m);
}
}
}
#pragma pop

// @symbol func_ov004_020b2444
namespace s20b2444 {
extern "C" {
extern int data_ov006_02137cd8[];
extern void func_ov004_020b1d60(int a0, int a1, int a2, int a3, int a4);

void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx)
{
    unsigned t = num;
    int digits = 0;
    int off;
    if (num != 0) {
        do {
            t /= 10;
            digits++;
        } while (t != 0);
    }
    off = 0;
    switch (sel) {
    case 0:
        off = (digits << 3) - 8;
        break;
    case 1:
        break;
    case 2:
        off = (digits << 4) - 0x10;
        break;
    }
    if (num == 0) {
        func_ov004_020b1d60(data_ov006_02137cd8[idx], a1, a2, a4, a5);
        return;
    }
    if (num <= 0)
        return;
    do {
        func_ov004_020b1d60(data_ov006_02137cd8[num % 10 + idx], a1 + off, a2, a4, a5);
        num /= 10;
        off -= 0x10;
    } while (num > 0);
}
}
}

// @symbol func_ov004_020b2574
namespace s20b2574 {
extern "C" {
extern int GetGameLanguage(void);
extern void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_ov004_020af948(void *a, int b, int c, void *m);
extern void **data_ov004_020bbfa8[];

void func_ov004_020b2574(int arg0, int arg1)
{
    void **table = data_ov004_020bbfa8[GetGameLanguage()];
    void *sb = table[0];
    int b = 12;
    if (arg1 != 1) {
        DrawOamSprite(sb, (void *)b, b, 0);
        DrawOamSprite(data_ov004_020bbfa8[GetGameLanguage()][1], (void *)30, b, 0);
        func_ov004_020b2444(0x30, b, arg0, 1, -1, 2, 0);
    } else {
        int i;
        for (i = 0; i < arg0; i++) {
            func_ov004_020af948(sb, b, 12, 0);
            b += 16;
        }
    }
}
}
}

namespace s20b265c {
extern "C" {
// @symbol _ZN11dScMgBase_c9Virtual84Ev
/* dScMgBase_c::Virtual84 - slot 33, and the only slot in this campaign with
   NOTHING to correct.  There was no `recovered name:` line on this function or
   on any of its overrides: the recovery pass never guessed a name here, so
   unlike slots 26, 29, 30, 31 and 32 there is no borrowed label to retire.
   The ROM names nothing either -- dScMgBase_c is a SCENE (fBase_c -> dBase_c
   -> dScene_c -> dScMgBase_c) and dActor_c, whose names slots 18-30 borrowed
   by index, has no slot 33 at all.  Virtual84 is the repo's own no-name
   spelling, after the +0x84 vtable offset, the same convention fBase_c uses
   for Virtual34 and Virtual38.  See the slot-33 block in
   include/dScMgBase_c.h.

   ENGINE BRING-UP.  Graphics modes for both engines (GX mode 1/0/0, GXS mode
   0), VRAM banks assigned to BG and OBJ on both screens, and BOTH BG-enable
   shadows -- data_0209d45c for the main engine, data_0209d454 for the sub --
   initialised to 0x10, which is the value slots 30 and 31 later save, clear
   bits out of and restore.  A language-indexed character file (indexed by
   GetGameLanguage into data_ov004_020bbfe4) is decompressed into BOTH engines'
   BG char VRAM at 0x06404000 and 0x06604000, OBJ palette file 0xc3 is loaded
   into both, and the scene object is published into the global registry --
   data_ov004_020beb74[1] = this, then data_0209d4a8 points at that registry.
   The object fields it touches are its own: +0x68 cleared and +0x6c set to -1.

   WHEN IT RUNS.  First out of dScMgBase_c::BeforeInitResources
   (ov004:0x020b0930): the +0x84 dispatch at 0x020b09d0 is near the top and
   slot 31's +0x7c dispatch at 0x020b0a0c is the last thing before the
   function returns 1.  Slot 32 runs out of AfterInitResources.  So the
   sequence is: bring the engines up (33), dress the sub screen (31), then
   dress the main screen (32).

   arity: no explicit parameters, MEASURED.  Scanning arm9 and all 103 overlays
   for the dispatch pair -- `ldr rD,[rN,#0x84]` with Rn != pc, followed within
   three instructions by `blx rD`/`bx rD` -- finds exactly two sites image-wide,
   of which one is in ov004 or ov006: the 0x020b09d0 call above.  It reads
   `mov r0,r4; ldr r1,[r0]; ldr r1,[r1,#0x84]; blx r1`, so r1 is the loaded
   pointer and cannot also be a second argument.  The scanner was validated by
   re-running it at +0x80 and reproducing slot 32's known call site.
   return type: void, which is what all three bodies do -- none assigns a
   result and the one caller ignores whatever falls out. */
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void func_ov004_020b2980(void);
extern void func_ov004_020b290c(void);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX15SetBankForSubBGEt(u16 a);
extern void _ZN2GX16SetBankForSubOBJEt(u16 a);
extern s32 GetGameLanguage(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void Ov004_Deallocate(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 len);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov004_020b0d30(void);
extern void FreeGfxSlotsById(int arg);
extern int data_ov004_020beb6c;
extern u8 data_0209d45c;
extern int data_ov004_020bbfe4[];
extern u8 data_0209d454;
extern void **data_0209d4a8;
extern int data_0208ee44;
}
}

void dScMgBase_c::Virtual84()
{ using s20b265c::DecompressLZ16; using s20b265c::FreeGfxSlotsById; using s20b265c::GetGameLanguage; using s20b265c::Ov004_Deallocate; using s20b265c::_ZN2GX11LoadOBJPlttEPKvjj; using s20b265c::_ZN2GX12SetBankForBGEt; using s20b265c::_ZN2GX13SetBankForOBJEt; using s20b265c::_ZN2GX15SetBankForSubBGEt; using s20b265c::_ZN2GX15SetGraphicsModeEiii; using s20b265c::_ZN2GX16SetBankForSubOBJEt; using s20b265c::_ZN3GXS11LoadOBJPlttEPKvjj; using s20b265c::_ZN3GXS15SetGraphicsModeEi; using s20b265c::_ZN4CP1527FlushAndInvalidateDataCacheEjj; using s20b265c::data_0208ee44; using s20b265c::data_0209d454; using s20b265c::data_0209d45c; using s20b265c::data_0209d4a8; using s20b265c::data_ov004_020bbfe4; using s20b265c::data_ov004_020beb6c; using s20b265c::func_ov004_020adc68; using s20b265c::func_ov004_020b0d30; using s20b265c::func_ov004_020b290c; using s20b265c::func_ov004_020b2980;
    char *obj = (char *)this;

    void *p;

    data_ov004_020beb6c = 0;
    obj[0x68] = 0;
    *(int *)(obj + 0x6c) = -1;
    *(vu32 *)0x4001000u |= 0x10000u;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);
    *(vu32 *)0x4001000u &= 0xffcfffefu;
    *(vu16 *)0x4000304u |= 0x8000u;
    func_ov004_020b2980();
    func_ov004_020b290c();
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    _ZN2GX12SetBankForBGEt(3);
    _ZN2GX13SetBankForOBJEt(0x10);
    data_0209d45c = 0x10;
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    p = func_ov004_020adc68(data_ov004_020bbfe4[GetGameLanguage()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    Ov004_Deallocate(p);
    p = func_ov004_020adc68(0xc3);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(p, 0x100u);
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    Ov004_Deallocate(p);
    data_0209d454 = 0x10;
    data_ov004_020beb74.mScene = this;
    data_0209d4a8 = (void **)&data_ov004_020beb74;
    func_ov004_020b0d30();
    FreeGfxSlotsById(0x1d);
    data_0208ee44 = 1;
}

namespace s20b27f4 {
extern "C" {
// @symbol _ZN11dScMgBase_c9Virtual80Ev
// recovered name: dScMgBase_c_AfterClsn  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
/* dScMgBase_c::Virtual80 - slot 32.

   The `recovered name:` line above is kept visible because it is wrong.  This
   time the borrowed name is a REAL ROM name -- _ZN16dPathLiftActor_c9AfterClsnEi,
   declared at include/PathLift.h:58 -- which makes it the more misleading of the
   two.  dPathLiftActor_c derives from dBgActor_c, which derives from dActor_c;
   dScMgBase_c is a dScene_c.  The chains share only dBase_c, which adds no
   virtual, so the two slot 32s have fBase_c's first eighteen entries in common
   and nothing else.  That AfterClsn also takes an int.  See the slot-32 block in
   include/dScMgBase_c.h.

   It touches no collision.  It is slot 31 with the other display engine: three
   read-modify-writes leave the MAIN BG1CNT at 0x0400000a holding exactly 0x1000,
   the layer's scroll is reset, BG1's bit is cleared from data_0209d45c -- the
   MAIN BG-enable shadow slot 30 restores the main DISPCNT from, where 31 cleared
   the sub's data_0209d454 -- and a language-indexed character file plus the
   shared screen map (file 0x67 here, 0x5b there) are installed.  It builds this
   minigame's TOP-screen background, from AfterInitResources. */
extern int GetGameLanguage(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d45c[];
}
}

void dScMgBase_c::Virtual80()
{ using s20b27f4::GetGameLanguage; using s20b27f4::LoadCompressedFileAt; using s20b27f4::data_0209d45c;
    int f;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~0x40;
    SetBg1Offset(0, 0);
    data_0209d45c[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(data_ov004_020bbff8[f], (void *)func_02054ea8());
    LoadCompressedFileAt(0x67, _ZN2G212GetBG1ScrPtrEv());
}

namespace s20b2880 {
extern "C" {
// @symbol _ZN11dScMgBase_c9Virtual7CEv
// recovered name: dScMgBase_c_Kill  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
/* dScMgBase_c::Virtual7C - slot 31.

   The `recovered name:` line above is kept visible because it is wrong.  The
   ROM names nothing here: dScMgBase_c is a SCENE (fBase_c -> dBase_c ->
   dScene_c -> dScMgBase_c), not an actor, and dActor_c -- whose names slots
   18-30 borrowed by index -- has no slot 31 at all.  `Kill` was carried across
   from dBgActor_c, dActor_c's own child, where _ZN10dBgActor_c4KillEv
   (ov002:0x020ee55c) is that class's new slot 31.  Different branch, same
   index, no relationship.  See the slot-31 block in include/dScMgBase_c.h.

   It also does not kill anything.  Three read-modify-writes leave the sub
   engine's BG1CNT holding exactly 0x10 -- priority 0, no mosaic -- then the
   layer's scroll is reset, BG1's bit is cleared from the sub BG-enable shadow
   that slot 30 restores the sub DISPCNT from, and a language-indexed character
   file plus the shared screen map (file 0x5b) are installed.  It builds this
   minigame's touch-screen background, from BeforeInitResources. */
extern int GetGameLanguage(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];
}
}

int dScMgBase_c::Virtual7C()
{ using s20b2880::GetGameLanguage; using s20b2880::LoadCompressedFileAt; using s20b2880::_ZN3G2S12GetBG1ScrPtrEv; using s20b2880::_ZN3G2S13GetBG1CharPtrEv; using s20b2880::data_0209d454;
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x10;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(data_ov004_020bc00c[f], (void *)_ZN3G2S13GetBG1CharPtrEv());
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}

// @symbol func_ov004_020b290c
namespace s20b290c {
extern "C" {
void SetBg0Offset(int a, int b);
void SetBg1Offset(int a, int b);
void SetBg2Offset(int a, int b);
void SetBg3Offset(int a, int b);
void SetSubBg0Offset(int a, int b);
void SetSubBg1Offset(int a, int b);
void SetSubBg2Offset(int a, int b);
void SetSubBg3Offset(int a, int b);

void func_ov004_020b290c(void) {
    SetBg0Offset(0, 0);
    SetBg1Offset(0, 0);
    SetBg2Offset(0, 0);
    SetBg3Offset(0, 0);
    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
}
}
}

// @symbol func_ov004_020b2980
namespace s20b2980 {
extern "C" {
extern void _ZN2GX15DisableAllBanksEv(void);
void func_ov004_020b2980(void) { _ZN2GX15DisableAllBanksEv(); }
}
}

// @symbol _ZN11dScMgBase_c15OnGroundPoundedEv
// recovered name: dScMgBase_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnGroundPounded - recovered from vtable slot identity */
void dScMgBase_c::OnGroundPounded()
{
}

// @symbol _ZN11dScMgBase_c9Virtual50Ev
/* Minigame slot 20; Virtual50 is a placeholder, not an original name.
   The reconstructed void contract is documented in dScMgBase_c.h. */
/* The base does nothing: the ROM body is a single `bx lr`. */
void dScMgBase_c::Virtual50()
{
}

// @symbol _ZN11dScMgBase_c13OnTurnIntoEggEi
// recovered name: dScMgBase_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnTurnIntoEgg - recovered from vtable slot identity */
/* vtable slot 19. Both the name and the signature are evidenced here: this
   body carries its own `recovered name:` above, and dScMgJump_c and
   dScMgBSC_c carry theirs. The parameter is an int, not dActor_c.h:132's
   `Player &` -- see include/dScMgBase_c.h. The base ignores the mode. */
int dScMgBase_c::OnTurnIntoEgg(int /* mode */)
{
    return 1;
}

// @symbol _ZN11dScMgBase_c13OnYoshiTryEatEi
/* vtable slot 18. The name is NOT a `recovered name:` -- this body carried
   none. It comes from include/dActor_c.h:131 and from ov006/symbols.txt,
   which already named dScMgCoin_c's override of this slot. The signature,
   unlike the name, is measured; see include/dScMgBase_c.h.
   The base ignores both arguments -- every descendant that cares reads
   them. */
void dScMgBase_c::OnYoshiTryEat(int arg)
{
}

// @symbol func_ov004_020b29a0
namespace s20b29a0 {
/* Calls slot 18 of the object it is given, passing the argument through.
   The class is not recovered, so a local view of nineteen slots stands in. */
struct Base {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(void*);
};
extern "C" void func_ov004_020b29a0(Base *c, void *arg) { c->v18(arg); }
}

/* The constructor and destructor are the end of the unit, and the one place
   the file is not in ROM order. Under defer_codegen off the out-of-line
   destructor comes out D1, D0, D2; the cartridge has D2, D0, D1 and then C2.
   With deferred codegen mwccarm emits a run of definitions in reverse, so
   writing the constructor first and the destructor after it, inside this
   bracket, produces D2, D0, D1, C2 as on the cartridge. */
#pragma push
#pragma defer_codegen on

namespace s20b2adc {
extern "C" {
// @symbol _ZN11dScMgBase_cC2Ev
int func_ov004_020adc3c(void *scene);
int func_02013580(int value, int arg);
void func_ov004_020adc00(int value);

extern char data_0209b308;
extern short data_ov004_020bc070[];
}
}

dScMgBase_c::dScMgBase_c()
    : unk_0a4(0), unk_0c2(1)
{ using s20b2adc::data_0209b308; using s20b2adc::data_ov004_020bc070; using s20b2adc::func_02013580; using s20b2adc::func_ov004_020adc00; using s20b2adc::func_ov004_020adc3c;
    mMenuOpen = 0;
    unk_462c = 0;
    unk_4630 = 0;
    unk_4648 = 0;

    data_ov004_020beb68 = this;
    unk_0bc = *(u32 *)(&data_0209b308 + 0x30);

    mSceneKind = data_ov004_020bc070[(param1 >> 16) & 0xff];
    *(s32 *)((char *)this + 8) =
        *(u32 *)((char *)this + 8) & 0xffff;
    unk_050 = 0;
    unk_054 = 0;

    if (actorID == 0x16e || actorID == 0x185 ||
        actorID == 0x16d || actorID == 0x182)
        mTimeLimit = 0x3c;
    else
        mTimeLimit = 0x78;

    unk_060 = 0;
    mFrameCounter = 0;
    unk_464c = 0;
    unk_4654 = 0;
    unk_4658 = 0;
    unk_064 = -1;

    func_ov004_020adc00(func_02013580(func_ov004_020adc3c(this), 0));
}

// @symbol _ZN11dScMgBase_cD2Ev
// @symbol _ZN11dScMgBase_cD0Ev
// @symbol _ZN11dScMgBase_cD1Ev
/* The destructor clears the shared minigame pointer; CodeWarrior supplies the
   vptr restores and the dScene_c teardown. This one out-of-line definition
   emits all three variants, D2 at 0x020b29c0, D0 at 0x020b2a18 and D1 at
   0x020b2a84, in that order inside the deferred bracket. As the first
   declared virtual it is the key function, so this file also emits the
   dScMgBase_c vtable and RTTI. */
dScMgBase_c::~dScMgBase_c()
{
    data_ov004_020beb68 = 0;
}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D2 AND D0 NAMES, AND MSVC NEVER EMITS THEM.
 * MSVC folds the Itanium destructor variants into the one ~dScMgBase_c()
 * above. Descendants tearing down a dScMgBase_c base subobject call the
 * base-object variant by its flat name; the deleting variant is the D1 body
 * followed by the class-specific operator delete. Each is spelled here in
 * terms of the one host destructor, called qualified so it is a direct call.
 * mwccarm never defines _MSC_VER, so nothing here reaches the cartridge
 * object. */
extern "C" dScMgBase_c *_ZN11dScMgBase_cD2Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    return thiz;
}

extern "C" dScMgBase_c *_ZN11dScMgBase_cD0Ev(dScMgBase_c *thiz)
{
    thiz->dScMgBase_c::~dScMgBase_c();
    dScMgBase_c::operator delete(thiz);
    return thiz;
}
#endif

#pragma pop
