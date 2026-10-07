//cpp
/* SoundEngine -- the sequencer-side half of the sound module: the 32-record
 * player table at data_020a4d6c with its voice allocator, the SDAT info-entry
 * tables (sequence/instrument-bank GetWithID), and the channel/stream worker
 * pool the public Sound:: API drives. The folded Sound TU (0x02011654..)
 * calls deep into this range; span 0x0204eda4..0x020524e4.
 *
 * Deferred codegen emits definitions in reverse source order.
 */
#include "types.h"
#include "Sound.h"
#include "SoundPlayerRecord.h"
#include "NestedHeapIterator.h"
#include "HeapAllocator.h"
#include "SolidHeapAllocator.h"

namespace IRQ {
    /* Defined at 0x02059d1c/0x02059d30 (the IRQ TU); namespace-scope externs
       mangle exactly like the ROM's IRQ::Disable/IRQ::Restore. */
    u32 Disable(void);
    u32 Restore(u32 state);
}

struct Bank_205179c;
struct Elem_204f278;
struct Elem_2051f1c;
struct Entry_20516d4;
struct G_20503a4;
struct HeapAllocator_204f400;
struct HeapAllocator_204f460;
struct HeapAllocator_204f504;
struct HeapAllocator_205212c;
struct MsgQueue;
struct Node_204f63c;
struct Obj_2050fd4;
struct Obj_205212c;
struct S58940;
struct S_204f720;

typedef short s16;
typedef void (*Fn)(void *a, int b, int c, int d);
typedef struct HeapOwner {
    HeapAllocator     *heap;
    NestedHeapIterator iter;
} HeapOwner;
typedef struct AllocNode {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkc;
    u32 unk10;
    u32 unk14;
} AllocNode;
typedef int s32;
typedef struct {
    int f0;
    int pad4[2];
    int* dstc;
    int* dst10;
    int len;
    int f18;
    int pad1c;
    void* f20;
    int f24;
    int f28;
} G_t;
typedef void (*FP)(void *, int, int *, int, int, int);
typedef struct {
    char pad[0xc];
    NestedHeapIterator iter; 
    char pad2[4];             
} IndexEntry;
typedef signed char s8;
struct Obj2 { int f0; int f4; int f8; int fc; };
struct Elem {
    int first;   
    int x4;      
    int x8;      
};
struct Obj {
    char pad[0x1c];
    int count;           
    struct Elem arr[1];   
};
struct Ent {
  char pad[0xf0];
  int b0 : 1;
  int flag : 1;
  int brest : 30;
  char pad2[0x12c - 0xf4];
};
struct Bank { int f0; u16 ids[4]; };
struct A4 { int f0; u16 pad4; u8 b6; u8 b7; };
struct Entry7 {
    unsigned int f0;
    unsigned short ids[6];
};
struct P4 {
    int f0;
    short pad;
    unsigned char f6;
    unsigned char f7;
};
struct P5 {
    char pad[0x18];
    int off18;
};
struct Entry {
    unsigned char kind;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
    int w;
};
struct List {
    unsigned int count;
    struct Entry entries[1];
};
struct Seq { int f0; u16 f4; };
struct Node {
    char pad8[8];
    int f8;      
    Fn fc;       
    int f10;     
    int f14;     
    char pad[8];
    char body;   
};
struct SolidHeapWithIter {
    void* allocator;       
    void* nhi_first;       
    void* nhi_last;        
    u16 id;                
    u16 nhi_offset;        
};
struct TwoInt { int a, b; };
struct data_020a5bb8_t { char pad[0x7c]; struct Hdr84 *f7c; };
struct Hdr84 {
    char pad[8];
    int count;
};
struct G {
    s32 f0;        
    s32 f4;        
    char pad8[0x30];
    s32 f38;       
    char pad3c[0x20 - 0x3c + 0x38];  
};
enum Fmt {
    FMT_A = 0,
    FMT_B = 1
};
struct D {
    int f0;
    int f4;
    char p8[0x1c - 8];
    int f1c;
    int f20;
    int f24;
    int f28;
};
struct Q { int f0, f4, f8, fc, f10; };
struct T {
    int f0, f4;
    int f8;                 
    int fc;                 
    int f10;                
    int f14;                
    int f18;                
    int f1c, f20, f24, f28;
    int f2c;                
    void (*f30)(int, int, int, int, int);   
    int f34;                
};
struct Inner {
    char pad8[8];
    int h8;
    char pad30[0x30 - 0xc];
    void (*h30)(unsigned int, unsigned int, unsigned int, int, int);
    int h34;
};
struct Outer {
    struct Inner *p0;
    int h4;
    char pad[4];
    int hc;
    int h10;
};
struct Obj1 { char pad[0x2c]; void *p; void *q; };
struct Obj0 { char pad[0xc]; int flag : 1; };
struct C {
    char pad0[0xc];
    int fc;
    char pad10[0x10];
    int f20;
    char pad24[4];
    int f28;
    int f2c;
};
struct E
{
  int field0;
  int field4;
};
struct LL {
  int lo;
  int hi;
};
struct E1 {
    unsigned char pad[0x2c];
    unsigned char f2c;
    unsigned char pad2[0xf];
    unsigned char f3c;
    unsigned char pad3[7];
};
struct E2 {
    unsigned char a[0xc];
    unsigned char b[0xc];
    int f18;
};
struct S {
    char pad0[4];
    u8 *ptr;
    char pad8[0x24];
    u8 active;
    char pad2d[0x17];
};
struct Inner83 {
    char pad[0x34];
    short h34;
    short pad36;
    short h38;
    short h3a;
};
struct HeapObj {
    HeapAllocator* unk0;
    HeapAllocator* heap;
    HeapAllocator* unk8;
};
struct S82 {
    char pad0[0x54];
    unsigned char *f54;
    unsigned char *f58;
};
struct Pair { void* a; u32 idx; };
struct HeapAllocator_205212c {
    char pad0[8];
    void *owner_8;    
};
struct Obj_205212c {
    char pad0[0x50];
    u32 u50;           
    char pad1[0xf0 - 0x54];
    u32 uf0;           
    char pad2[0x114 - 0xf4];
    int *u114;         
};
struct Elem_2051f1c { u8 pad0[4]; u16 f4; u8 pad6[2]; u8 f8; u8 f9; };
struct Bank_205179c { int f0; u16 f4; };
struct Entry_20516d4 {
    int field0;
    unsigned short ids[4];
};
struct Obj_2050fd4 {
    struct SolidHeapAllocator *heapAlloc;
    struct NestedHeapIterator iter;
};
struct G_20503a4 {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20, f24, f28, f2c, f30, f34;
    char f38[0x10];
    int f48, f4c;
};
struct S_204f720 {
    char pad34[0x34];
    short a;
    short pad36;
    short b;
};
struct Node_204f63c { unsigned short f0[4]; unsigned int f10[2]; unsigned int f18; };
struct HeapAllocator_204f504 {
    int unk0;
    int unk4;
    void* unk8;  
    void* unkc;  
};
struct HeapAllocator_204f460 {
    char pad[0x04];
    NestedHeapIterator* owner;  
    char pad2[0x35];             
    unsigned char sortKey;       
};
struct HeapAllocator_204f400 {
    char pad[0x3d];
    unsigned char sortKey; 
};
struct Elem_204f278 { char pad[0x1c]; };
typedef struct Node Node;

extern "C" {
/* C-linkage callback types used by foreign decls. */
/* Foreign callees and data. */
extern struct Elem;
extern int FS_CloseFile(char *self);
extern void FS_InitFile(void *s);
extern int FS_ReadFile(int a, int b, int c);
extern struct HeapAllocator;
extern void MultiStore_Int(int val, int *dst, int len);
extern struct NestedHeapIterator;
extern struct Obj;
extern struct Obj1;
extern int _ZN18NestedHeapIterator4NextEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *h);
extern void _ZN18NestedHeapIterator5AddAtEP13HeapAllocatorS1_(NestedHeapIterator* iter, HeapAllocator* pos, HeapAllocator* node);
extern void _ZN18NestedHeapIterator6RemoveEP13HeapAllocator(void *it, void *ha);
extern void _ZN18NestedHeapIterator7AddLastEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *heap);
extern int _ZN18NestedHeapIterator8PreviousEP13HeapAllocator(NestedHeapIterator *iter, HeapAllocator *h);
extern int _ZN18NestedHeapIteratorC1Ej(int a, int b);
extern int _ZN18SolidHeapAllocator10MemoryLeftEi(SolidHeapAllocator *self, int align);
extern void _ZN18SolidHeapAllocator5ResetEj(SolidHeapAllocator *self, u32 params);
extern void *_ZN18SolidHeapAllocator8AllocateEji(HeapAllocator *heap, u32 size, s32 align);
extern s32 _ZN18SolidHeapAllocator9LoadStateEj(void* alloc, u32 id);
extern s32 _ZN18SolidHeapAllocator9SaveStateEj(void* alloc, u32 id);
extern u32 _ZN3IRQ7DisableEv(void);
extern u32 _ZN3IRQ7RestoreEj(u32 state);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern SolidHeapAllocator *_ZN4Heap24CreateSolidHeapAllocatorEPvjj(void *address, u32 size, u32 flags);
extern unsigned int _ZN4cstd6strlenEPKc(const char *s);
extern int __aeabi_idiv(int a, int b);
extern unsigned int __aeabi_uidiv(unsigned int a, unsigned int b);
extern s16 data_02086384[];
extern int data_020a4d44;
extern unsigned int data_020a4d48;
extern unsigned int data_020a4d4c;
extern unsigned int data_020a4d50;
extern char data_020a4d54[];
extern NestedHeapIterator data_020a4d60;
extern SoundPlayerRecord data_020a4d6c[];
extern struct E1 data_020a50ec[];
extern int data_020a552c[];
extern int data_020a5538[];
extern struct E data_020a5578[];
extern int data_020a55f8;
extern int data_020a55fc;
extern int data_020a5600;
extern int data_020a5614;
extern struct D data_020a5634;
extern int data_020a5684;
extern struct Q data_020a5718[];
extern char *data_020a5bb8;
extern NestedHeapIterator data_020a5bbc;
extern NestedHeapIterator data_020a5bc8;
extern struct Ent data_020a5bd4[];
extern void func_0204ebb8(void *alloc);
extern void func_02058048(struct Obj *self);
extern void func_02058200(char *self, int a1, int a2, int end, int arg4, int arg5);
extern int func_020587e4(struct MsgQueue *self, u32 *msg, int flags);
extern int func_02058894(struct Q *q, int val, int flag);
extern void func_02058940(struct S58940 *s, int v1, int v2);
extern void func_0205a82c(void);
extern void func_0205a958(void *a, int b, int c, int d);
extern void func_0205a98c(void *a, int b);
extern void func_0205a9b8(void *a, int b);
extern void func_0205a9e4(void *a, int b);
extern void func_0205aa10(unsigned int p0, unsigned int p1, int p2, unsigned int p3, unsigned int p4, unsigned int p5, unsigned int p6, unsigned int p7, unsigned int p8, unsigned int p9);
extern void func_0205aa68(void *a, int b, int c);
extern void func_0205aa9c(void *a, int b);
extern void func_0205aac8(void *a, int b);
extern void func_0205aaf4(void *a, int b, int c);
extern void func_0205ab28(int a, int b, int c, int d, int e);
extern void func_0205ab6c(unsigned int a, unsigned int b, int c, int d, unsigned int e, unsigned int f, unsigned int g);
extern void func_0205abb8(int a, int b, int c, int d);
extern void func_0205ac28(void *a, int b, int c, int d);
extern int func_0205ac5c(int a, int b, int c);
extern int func_0205ac84(int a, int b, int c);
extern int func_0205acac(int a, int b, int c);
extern int func_0205acd4(int a, int b, int c);
extern int func_0205acfc(int a, int b, int c);
extern void func_0205ad24(int, int);
extern void func_0205ad3c(int a, int b);
extern void func_0205ad54(int, int);
extern void func_0205ad6c(void *a, int b);
extern void func_0205ad98(void *c);
extern void func_0205adc4(void *a, int b, int c, int d);
extern int func_0205af78(unsigned int x);
extern void *func_0205afb4(void);
extern void func_0205afe0(unsigned int c);
extern int func_0205b070(int x);
extern void **func_0205b274(int flag);
extern unsigned int func_0205b608(void);
extern u16 func_0205b63c(int x);
extern void func_0205b6b0(void *c);
extern void func_0205b6f4(void *c);
extern void func_0205b78c(struct Node *base, int index, struct Node *arg);
extern int func_0205caa8(void *thiz);
extern int func_0205cb68(int *self, int a1, int a2, int a3, int a4, int a5, int a6, int a7);
extern void func_0205cbe4(void *thiz);
extern int func_0205cc80(void *thiz, void *a, void *b);
extern void func_0205cd34(void *o);
extern int func_0205d368(void *o, int r1, int sel);
extern int func_0205d568(Node *node, int b, ...);
extern int func_0205d644(void *out_file_id, const char *path);

/* In-span forward declarations. */
extern int func_0204eda4(int r0, char* a, u32 idx);
extern int func_0204ede8(struct S82 *o, unsigned int i);
extern int func_0204ee10(void *thiz);
extern int func_0204ee40(void *self, char *name, char *p);
extern int func_0204ef9c(void* p);
extern void func_0204efe0(s8 levelID);
extern void func_0204effc(void);
extern void func_0204f03c(void);
extern void func_0204f070(void);
extern int func_0204f0b8(unsigned int bit);
extern int func_0204f0d8(void);
extern void func_0204f11c(int mask);
extern int func_0204f138(unsigned int mask);
extern void func_0204f15c(void *node);
extern int func_0204f194(unsigned int a);
extern void func_0204f1e8(void);
extern void func_0204f214(char* thiz, int b);
extern void func_0204f278(char *thiz);
extern void func_0204f2d4(HeapObj* obj);
extern char* func_0204f364(int key);
extern void func_0204f3e0(unsigned char *c);
extern void func_0204f400(HeapAllocator_204f400* self);
extern void func_0204f460(NestedHeapIterator* self, HeapAllocator_204f460* node);
extern void func_0204f4bc(char* obj);
extern void* func_0204f504(int index, void* unk);
extern void func_0204f558(unsigned char* self, int val);
extern void func_0204f5a0(u8 *thiz, int arg1);
extern int func_0204f600(void *thiz, int seqData, int seqOffset, int bank);
extern void func_0204f630(void);
extern void *func_0204f63c(void **p, int idx, int r7);
extern void func_0204f6f8(struct Inner83 **o, short a, short b);
extern void func_0204f720(struct S_204f720 **pp, short v);
extern void func_0204f73c(char **c, int b);
extern void func_0204f76c(char **c, int b, int d);
extern void func_0204f79c(char **c, int b, int d);
extern void func_0204f7cc(char **c, int b, int d);
extern void func_0204f7fc(char **c, int b, int d);
extern void func_0204f82c(void** obj, int b, int idx);
extern void func_0204f86c(char **c, int b, int d);
extern void func_0204f89c(char **c, int b);
extern void func_0204f8cc(char **c, int a, int b);
extern void func_0204f914(void **p, unsigned char v);
extern void func_0204f924(void **p, unsigned char v);
extern void func_0204f934(int **p);
extern void func_0204f94c(int *p);
extern void func_0204f958(int idx, int val);
extern void func_0204f9c4(int idx, int arg1);
extern int func_0204fa2c(int *p, int fade);
extern int func_0204fa3c(int owner, void *b, u32 c);
extern void func_0204fafc(void);
extern void func_0204fc40(void);
extern void func_0204fce8(int *thiz, void *arg);
extern void func_0204fda4(char* c);
extern void func_0204fde8(char* obj);
extern void func_0204fe5c(char *obj, int arg1);
extern void func_0204fed0(struct C *c);
extern void func_0204ff48(char *p);
extern void func_0204ff9c(struct Obj0 *o);
extern void func_0204ffcc(char *self);
extern void func_02050008(struct Obj1 *o);
extern void func_02050038(void);
extern void func_020500a4(struct T *thiz);
extern void func_020501b8(void);
extern void func_02050258(void);
extern void func_020502b8(void);
extern int func_020503a4(int a, int b, int c, int d, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);
extern void func_020506fc(int arg);
extern void func_02050798(void);
extern int func_020507e0(int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6);
extern void func_020508a0(void);
extern void func_02050950(void);
extern void func_02050970(int a0, int a1, int *p);
extern void func_0205097c(int a0, int a1, int *p);
extern void func_02050988(int a0, int a1, int *p);
extern void func_02050994(int i, int val);
extern int func_020509b0(unsigned int i);
extern int func_020509d8(unsigned int idx, int a1, unsigned int len, unsigned int off);
extern int func_02050a5c(unsigned int i);
extern int func_02050a84(unsigned int id);
extern int func_02050ae8(unsigned int id);
extern int func_02050b4c(unsigned int id);
extern int func_02050c14(unsigned int id);
extern void *func_02050cdc(int a, int idx);
extern int func_02050d2c(void);
extern int func_02050d3c(int v);
extern int func_02050d54(char *obj, void *owner, int flag);
extern void func_02050f34(char *self, int a1, int a2, int a3);
extern void func_02050fb0(void);
extern s32 func_02050fd4(struct Obj_2050fd4 *self);
extern s32 func_02051024(struct SolidHeapWithIter* self, void* allocator);
extern int func_02051064(int a);
extern u32 func_02051074(SolidHeapAllocator **pHeap);
extern void func_020510a4(int *thiz, int n);
extern s32 func_0205117c(struct SolidHeapWithIter* self);
extern void *func_020511d4(HeapOwner *owner, u32 elemSize, u32 param2, u32 param3, u32 param4);
extern void func_02051244(int *thiz);
extern void func_020512f0(int *p);
extern void *func_0205130c(unsigned int addr, unsigned int size);
extern void func_020513a8(char *a, int b, int c, int d);
extern void func_020513e4(char *a, int b, int c, int d);
extern void func_02051420(char *a, int b, int c, int d);
extern void func_02051454(int a, int b, unsigned int c);
extern int func_020514b4(int a, int b);
extern int func_02051514(int a, int b);
extern int func_02051574(int a, int b);
extern int func_020515d4(int a, int b);
extern void* func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void* owner);
extern int func_020516d4(unsigned int id, int flags, int c);
extern int func_0205179c(unsigned int id, int flags, int arg2);
extern int func_020518a0(void *a, void *b);
extern int func_020518dc(void *a, void *b);
extern int func_02051918(unsigned int id, int arg);
extern void *func_02051a28(int index, void *unk);
extern void func_02051a48(unsigned char *c, int off);
extern void func_02051a68(unsigned char *c, int off);
extern int func_02051a88(int a, int b);
extern int func_02051a98(void *p0, void *p1, unsigned int p2, void *p3, struct P4 *p4, struct P5 *p5, int p6, int p7);
extern int func_02051bd0(void **p, int a1, unsigned int a2, int a3, struct A4 *a4, int a5);
extern int func_02051e60(int a, int b, int c, int d, unsigned int arg5, int arg6);
extern int func_02051f1c(void *a0, u32 a1, int a2);
extern int func_02051fb4(void *thiz, unsigned int id);
extern int func_02052008(int arg);
extern void func_020520a4(HeapAllocator *node);
extern void func_020520dc(char *self);
extern void func_0205212c(Obj_205212c *self);
extern void func_02052208(void *obj);
extern void func_02052234(void);
extern void func_0205227c(void);
extern void func_020522c4(void);
extern struct Elem* func_020523e0(struct Obj* obj, int idx);
extern int func_02052420(int *o);
extern void func_02052438(char *self);
extern int func_02052458(int *p);
extern void func_02052494(struct Obj2 *o, int a, int b);
extern void func_020524c4(int *r0);
}

// @symbol func_020524c4
extern "C" void func_020524c4(int *r0){
  r0[1]=0;
  r0[0]=r0[1];
  r0[3]=0;
  r0[2]=r0[3];
}

// @symbol func_02052494
extern "C" void func_02052494(struct Obj2 *o, int a, int b) {
    o->f0 = ((extern int(*)(struct Obj2 *o, int a, int b))func_02052458)(o, a, b);
    o->f4 = a;
    o->fc = b;
    o->f8 = 0;
}

// @symbol func_02052458
extern "C" int func_02052458(int *p) {
    int den = p[3];
    int num = p[2];
    if (num >= den) return p[1];
    {
        int lo = p[0];
        int d = p[1] - lo;
        return lo + num * d / den;
    }
}

// @symbol func_02052438
extern "C" void func_02052438(char *self)
{
    if (*(int *)(self + 8) < *(int *)(self + 0xc)) {
        *(int *)(self + 8) += 1;
    }
}

// @symbol func_02052420
extern "C" int func_02052420(int *o) {
    return o[2] >= o[3];
}

// @symbol func_020523e0
extern "C" struct Elem* func_020523e0(struct Obj* obj, int idx) {
    struct Elem* e;
    if (idx < 0) return 0;
    if ((u32)idx >= (u32)obj->count) return 0;
    e = &obj->arr[idx];
    if (e->first == -1) return 0;
    return e;
}

// @symbol func_020522c4
extern "C" void func_020522c4(void)
{
    int i;
    int t1;
    char *r = ((char*)data_020a5bd4);
    for (i = 0; i < 4; i++) {
        int flags = *(int *)(r + 0xf0);
        if ((flags << 31) >> 31) {
            if (*(int *)(r + 0xf4) != 0) {
                func_02052208((void *)(r));
            } else {
                if ((flags << 29) >> 31) {
                    if (*(int *)(r + 0xf8) != 0) {
                        func_0204ffcc(r);
                        {
                            int *flagsp = (int *)(r + 0xf0);
                            *flagsp |= 2;
                            *flagsp &= ~4;
                        }
                    }
                }
                if ((*(int *)(r + 0xf0) << 30) >> 31) {
                    unsigned long t2;
                    func_02052438(r + 0xc8);
                    t1 = data_02086384[*(int *)(r + 0x11c)];
                    t2 = data_02086384[func_02052458((int *)(r + 0xc8)) >> 8];
                    t1 = t2 + t1;
                    if (t1 != *(int *)(r + 0x120)) {
                        func_0204fe5c(r, t1);
                        *(int *)(r + 0x120) = t1;
                    }
                    if ((*(int *)(r + 0xf0) << 28) >> 31) {
                        if (func_02052420((int *)(r + 0xc8))) {
                            func_02052208((void *)(r));
                        }
                    }
                }
            }
        }
        r += 0x12c;
    }
}

// @symbol func_0205227c
extern "C" void func_0205227c(void){
  int i;
  for (i = 0; i < 4; i++){
    if (data_020a5bd4[i].flag) {
      ((void(*)(void* p))func_0204ff48)(&data_020a5bd4[i]);
    }
  }
}

// @symbol func_02052234
extern "C" void func_02052234(void){
  int i;
  for (i = 0; i < 4; i++){
    if (data_020a5bd4[i].flag) {
      ((void(*)(void* p))func_0204fed0)(&data_020a5bd4[i]);
    }
  }
}

// @symbol func_02052208
extern "C" void func_02052208(void *obj) {
    int v = *(int *)((char *)obj + 0xf0);
    if ((v << 30) >> 31) ((void(*)(void *o))func_0204ff9c)(obj);
    ((void(*)(void *o))func_0205212c)(obj);
}

// @symbol func_0205212c
extern "C" void func_0205212c(Obj_205212c *self) {
    if (self->u114 != 0) {
        *self->u114 = 0;
        self->u114 = 0;
    }
    ((void(*)(void *self))func_020520dc)(self);
    int b = (int)((self->u50 & 0x10) != 0);
    if (b != 0) {
        ((void(*)(void *p))FS_CloseFile)((char*)self + 0x44);
    }
    u32 irq = IRQ::Disable();
    HeapAllocator_205212c *node = (HeapAllocator_205212c*)((HeapAllocator_205212c*)(data_020a5bc8.Next(0)));
    while (node != 0) {
        HeapAllocator_205212c *next = (HeapAllocator_205212c*)((HeapAllocator_205212c*)(data_020a5bc8.Next((HeapAllocator*)(node))));
        if (node->owner_8 == self) {
            data_020a5bc8.Remove((HeapAllocator*)(node));
            ((void(*)(HeapAllocator_205212c *node))func_020520a4)(node);
        }
        node = next;
    }
    IRQ::Restore(irq);
    u32 *p = (u32*)(((int)self + 0xf0));
    *p &= ~1u;
    *p &= ~4u;
    *p &= ~2u;
}

// @symbol func_020520dc
extern "C" void func_020520dc(char *self)
{
    if (*(int *)(self + 0x100) == 0)
        return;

    *(int *)(self + 0x100) -= 1;

    if (*(int *)(self + 0x100) != 0)
        return;

    func_02050008((struct Obj1 *)self);
}

// @symbol func_020520a4
extern "C" void func_020520a4(HeapAllocator *node)
{
    u32 state;

    state = IRQ::Disable();
    ((NestedHeapIterator*)(&data_020a5bbc))->AddLast((HeapAllocator*)(node));
    IRQ::Restore(state);
}

// @symbol func_02052008
extern "C" int func_02052008(int arg)
{
    int i;
    func_02050d2c();
    for (i = 0; i < 0x20; i++) {
        unsigned char *p = ((unsigned char *(*)(unsigned int id))func_02050ae8)(i);
        if (p == 0)
            continue;
        Sound::Player::SetPlayableSeqCount(i, p[0]);
        if (*(int *)(p + 4) == 0)
            continue;
        if (arg == 0)
            continue;
        {
            int j;
            for (j = 0; j < p[0]; j++) {
                if (((int(*)(int a, int b, int c))func_0204fa3c)(i, arg, *(int *)(p + 4)) == 0)
                    return 0;
            }
        }
    }
    return 1;
}

// @symbol func_02051fb4
extern "C" int func_02051fb4(void *thiz, unsigned int id) {
    Sound::InfoSequenceEntry *e = (Sound::InfoSequenceEntry*)Sound::InfoSequenceEntry::GetWithID(id);
    if (!e) return 0;
    return ((int(*)(void *thiz, unsigned int a, unsigned int b, unsigned int c, void *entry, unsigned int id))func_02051bd0)(thiz, e->playerNumber, e->bankId, e->playerPriority, e, id);
}

// @symbol func_02051f1c
extern "C" int func_02051f1c(void *a0, u32 a1, int a2)
{
    int *r;
    void *obj;
    struct Elem_2051f1c *e;
    r = ((int *(*)(u32 id))func_02050c14)(a1);
    if (r == 0) return 0;
    obj = ((void *(*)(int x))func_020509b0)(r[0]);
    if (obj == 0) return 0;
    e = ((struct Elem_2051f1c *(*)(void *obj, int idx))func_020523e0)(obj, a2);
    if (e == 0) return 0;
    return ((int(*)(void *a, int b, int c, int d, void *e, void *f, int g, int h))func_02051a98)(a0, e->f9, e->f4, e->f8, e, obj, (int)a1, a2);
}

// @symbol func_02051e60
extern "C" int func_02051e60(int a, int b, int c, int d, unsigned int arg5, int arg6)
{
    int *p;
    void *elem;
    int r4;
    p = (int*)func_02050c14(arg5);
    if (p == 0) return 0;
    r4 = func_020509b0(p[0]);
    if (r4 == 0) return 0;
    elem = ((void *(*)(void *obj, int idx))func_020523e0)((void*)r4, arg6);
    if (elem == 0) return 0;
    if (d < 0) d = *(unsigned char*)((char*)elem + 8);
    if (c < 0) c = *(unsigned short*)((char*)elem + 4);
    if (b < 0) b = *(unsigned char*)((char*)elem + 9);
    return ((int(*)(int a, int b, int c, int d, void *elem, int r4, int arg5, int arg6))func_02051a98)(a, b, c, d, elem, r4, arg5, arg6);
}

// @symbol func_02051bd0
extern "C" int func_02051bd0(void **p, int a1, unsigned int a2, int a3, struct A4 *a4, int a5)
{
    struct Bank *bank;
    void *node;
    int r7;
    void *r6 = 0;
    int r5;
    int i;
    void *np;

    bank = (struct Bank *)Sound::InfoInstrumentBankEntry::GetWithID(a2);
    if (bank == 0) return 0;

    r7 = ((int(*)(void **p, int idx, int r7))func_0204f63c)(p, a1, a3);
    if (r7 == 0) return 0;

    r5 = func_020509b0(bank->f0);
    if (r5 == 0) {
        r6 = func_02051a28(a1, (void *)r7);
        if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
        r5 = (int)((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)(bank->f0, (unsigned int)&func_02051a68, 0, 0, r6);
        if (r5 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
    }

    for (i = 0; i < 4; i++) {
        u16 v = bank->ids[i];
        if (v != 0xffff) {
            int q;
            node = ((void *(*)(unsigned int id))func_02050b4c)(v);
            if (node == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
            q = func_020509b0(*(unsigned int *)node);
            if (q == 0) {
                if (r6 == 0) {
                    r6 = func_02051a28(a1, (void *)r7);
                    if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
                }
                q = (int)((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)(*(unsigned int *)node, (unsigned int)&func_02051a48, 0, 0, r6);
                if (q == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
            }
            ((void(*)(int base, int index, int arg))func_0205b78c)(r5, i, q);
        }
    }

    np = (void *)func_020509b0((unsigned int)a4->f0);
    if (np == 0) {
        if (r6 == 0) {
            r6 = func_02051a28(a1, (void *)r7);
            if (r6 == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
        }
        np = ((void *(*)(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void *owner))func_02051634)((unsigned int)a4->f0, (unsigned int)&func_02051a88, 0, 0, r6);
        if (np == 0) { ((void(*)(int r7))func_0204f630)(r7); return 0; }
    }

    ((int(*)(int thiz, int p1, int p2, int p3))func_0204f600)(r7, (int)((char *)np + *(int *)((char *)np + 0x18)), 0, r5);
    func_0204f914(p, a4->b6);
    ((void(*)(void **c, unsigned char v))func_0204f89c)(p, a4->b7);
    ((void(*)(void *pp, int v))func_0204f720)(p, a5);
    return 1;
}

// @symbol func_02051a98
extern "C" int func_02051a98(void *p0, void *p1, unsigned int p2, void *p3,
                  struct P4 *p4, struct P5 *p5, int p6, int p7)
{
    struct Entry7 *e7;
    void *r6;
    void *r5;
    int i;

    e7 = (struct Entry7*)Sound::InfoInstrumentBankEntry::GetWithID(p2);
    if (e7 == 0)
        return 0;
    r6 = ((void *(*)(void *p, void *idx, void *arg))func_0204f63c)(p0, p1, p3);
    if (r6 == 0)
        return 0;
    r5 = ((void *(*)(unsigned int i))func_020509b0)(e7->f0);
    if (r5 == 0) {
        ((void(*)(void *p))func_0204f630)(r6);
        return 0;
    }
    for (i = 0; i < 4; i++) {
        unsigned int id = e7->ids[i];
        unsigned int *ent;
        void *node;
        if (id != 0xffff) {
            ent = ((unsigned int *(*)(unsigned int id))func_02050b4c)(id);
            if (ent == 0) {
                ((void(*)(void *p))func_0204f630)(r6);
                return 0;
            }
            node = ((void *(*)(unsigned int i))func_020509b0)(*ent);
            if (node == 0) {
                ((void(*)(void *p))func_0204f630)(r6);
                return 0;
            }
            ((void(*)(void *base, int index, void *arg))func_0205b78c)(r5, i, node);
        }
    }
    ((void(*)(void *thiz, void *p, int v, void *node))func_0204f600)(r6, (char *)p5 + p5->off18, p4->f0, r5);
    ((void(*)(void *p, unsigned char v))func_0204f914)(p0, p4->f6);
    ((void(*)(void *p, unsigned char v))func_0204f89c)(p0, p4->f7);
    ((void(*)(void *o, int a, int b))func_0204f6f8)(p0, p6, p7);
    return 1;
}

// @symbol func_02051a88
extern "C" int func_02051a88(int a, int b)
{
    return ((int(*)(int a, int b))func_0205a9e4)(a, a + b);
}

// @symbol func_02051a68
extern "C" void func_02051a68(unsigned char *c, int off)
{
    ((void(*)(void *c, void *p))func_0205a9b8)(c, c + off);
    func_0205b6f4((void *)(c));
}

// @symbol func_02051a48
extern "C" void func_02051a48(unsigned char *c, int off)
{
    ((void(*)(void *c, void *p))func_0205a98c)(c, c + off);
    func_0205b6b0((void *)(c));
}

// @symbol func_02051a28
extern "C" void *func_02051a28(int index, void *unk)
{
    void *p = ((void *(*)(int index, void *unk))func_0204f504)(index, unk);
    if (p) {
        ((void(*)(void *p))func_02051244)(p);
    }
    return p;
}

// @symbol func_02051918
extern "C" int func_02051918(unsigned int id, int arg)
{
    struct List *l;
    unsigned int i;
    struct Entry *e;

    l = (struct List *)func_02050a84(id);
    if (!l)
        return 0;

    i = 0;
    if (i < l->count) {
        e = l->entries;
        do {
            switch (e->kind) {
            case 0:
                if (func_0205179c(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 3:
                if (func_020518dc((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            case 1:
                if (((int(*)(unsigned int id, int flags, int arg2))func_020516d4)(e->w, e->b1, arg) == 0)
                    return 0;
                break;
            case 2:
                if (func_020518a0((void *)e->w, (void *)arg) == 0)
                    return 0;
                break;
            }
            i++;
            e++;
        } while (i < l->count);
    }

    return 1;
}

// @symbol func_020518dc
extern "C" int func_020518dc(void *a, void *b) {
    void *p = ((void *(*)(void *a))func_02050c14)(a);
    if (p == 0) return 0;
    return ((int(*)(void *a, void *b))func_02051574)(*(void **)p, b) != 0;
}

// @symbol func_020518a0
extern "C" int func_020518a0(void *a, void *b) {
    void *p = ((void *(*)(void *a))func_02050b4c)(a);
    if (p == 0) return 0;
    return ((int(*)(void *a, void *b))func_020514b4)(*(void **)p, b) != 0;
}

// @symbol func_0205179c
extern "C" int func_0205179c(unsigned int id, int flags, int arg2)
{
    struct Seq* seq;
    struct Bank_205179c* bank;
    int r = 0;
    int i;

    seq = (struct Seq*)Sound::InfoSequenceEntry::GetWithID(id);
    if (!seq) return r;
    bank = (struct Bank_205179c*)Sound::InfoInstrumentBankEntry::GetWithID(seq->f4);
    if (!bank) return r;

    if (flags & 2) {
        r = func_02051514(bank->f0, arg2);
        if (!r) return 0;
    }
    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            u16 id2 = (&bank->f4)[i];
            if (id2 != 0xffff) {
                void* p = ((void*(*)(unsigned int id))func_02050b4c)(id2);
                int q;
                if (!p) return 0;
                q = func_020514b4(*(int*)p, arg2);
                if (!q) return 0;
                if (r != 0) ((void(*)(int base, int index, int arg))func_0205b78c)(r, i, q);
            }
        }
    }
    if (flags & 1) {
        int s = func_020515d4(seq->f0, arg2);
        if (!s) return 0;
    }
    return 1;
}

// @symbol func_020516d4
extern "C" int func_020516d4(unsigned int id, int flags, int c) {
    struct Entry_20516d4 *entry;
    int base;
    int i;

    base = 0;
    entry = (struct Entry_20516d4*)Sound::InfoInstrumentBankEntry::GetWithID(id);
    if (entry == 0) return 0;

    if (flags & 2) {
        base = func_02051514(entry->field0, c);
        if (base == 0) return 0;
    }

    if (flags & 4) {
        for (i = 0; i < 4; i++) {
            if (entry->ids[i] != 0xffff) {
                int *p;
                int arg;
                p = ((int *(*)(unsigned int id))func_02050b4c)(entry->ids[i]);
                if (p == 0) return 0;
                arg = func_020514b4(*p, c);
                if (arg == 0) return 0;
                if (base != 0) {
                    ((void(*)(int base, int index, int arg))func_0205b78c)(base, i, arg);
                }
            }
        }
    }
    return 1;
}

// @symbol func_02051634
extern "C" void* func_02051634(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, void* owner)
{
    int r5 = ((extern int(*)(unsigned int i))func_02050a5c)(a0);
    void* r4;
    if (r5 == 0) return 0;
    r4 = ((extern void*(*)(void *owner, unsigned int elemSize, unsigned int p2, unsigned int p3, unsigned int p4))func_020511d4)(owner, r5 + 0x20, a1, a2, a3);
    if (r4 == 0) return 0;
    if (r5 != ((extern int(*)(unsigned int a, void* b, int c, int d))func_020509d8)(a0, r4, r5, 0)) return 0;
    _ZN4CP1514FlushDataCacheEjj((unsigned int)r4, r5);
    return r4;
}

// @symbol func_020515d4
extern "C" int func_020515d4(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_02051420, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051574
extern "C" int func_02051574(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_02051420, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051514
extern "C" int func_02051514(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_020513e4, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_020514b4
extern "C" int func_020514b4(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = ((int(*)(int a, void *cb, int d, int a2, int b))func_02051634)(a, (void *)func_020513a8, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}

// @symbol func_02051454
extern "C" void func_02051454(int a, int b, unsigned int c) {
    int t;
    unsigned int irq;
    irq = IRQ::Disable();
    t = func_02050d3c(b);
    if (a == func_020509b0(c)) {
        func_02050994(c, 0);
    }
    func_02050d3c(t);
    IRQ::Restore(irq);
}

// @symbol func_02051420
extern "C" void func_02051420(char *a, int b, int c, int d)
{
    ((void(*)(char *a, int c, int d))func_02051454)(a, c, d);
    ((void(*)(char *a, char *b))func_0205a9e4)(a, a + b);
}

// @symbol func_020513e4
extern "C" void func_020513e4(char *a, int b, int c, int d) {
    ((extern void(*)(void *a, int c, int d))func_02051454)(a, c, d);
    func_0205a9b8((void *)(a),  (int)(a + b));
    ((extern void(*)(void *a))func_0205b6f4)(a);
}

// @symbol func_020513a8
extern "C" void func_020513a8(char *a, int b, int c, int d) {
    ((extern void(*)(void *a, int c, int d))func_02051454)(a, c, d);
    func_0205a98c((void *)(a),  (int)(a + b));
    ((extern void(*)(void *a))func_0205b6b0)(a);
}

// @symbol func_0205130c
extern "C" void *func_0205130c(unsigned int addr, unsigned int size)
{
    unsigned int end = addr + size;
    unsigned int aligned = (addr + 3) & ~3u;
    void *alloc;
    if (aligned > end)
        return 0;
    if (end - aligned < 0x10)
        return 0;
    alloc = ((void *(*)(void *p, unsigned int sz, unsigned int z))_ZN4Heap24CreateSolidHeapAllocatorEPvjj)((void *)(aligned + 0x10),
                                                    (end - aligned) - 0x10, 0);
    if (alloc == 0)
        return 0;
    if (((int(*)(void *self, void *alloc))func_02051024)((void *)aligned, alloc) != 0)
        return (void *)aligned;
    func_0204ebb8(alloc);
    return 0;
}

// @symbol func_020512f0
extern "C" void func_020512f0(int *p) {
    ((void(*)(void *p))func_02051244)(p);
    ((void(*)(void *p))func_0204ebb8)((void*)*p);
}

// @symbol func_02051244
extern "C" void func_02051244(int *thiz)
{
    struct Node *r7;
    struct Node *r6;
    r7 = (struct Node*)(((NestedHeapIterator*)((char*)thiz + 4))->Previous(0));
    if (r7 == 0) goto end;
    do {
        r6 = (struct Node*)(((NestedHeapIterator*)(r7))->Previous(0));
        while (r6 != 0) {
            if (r6->fc != 0) {
                r6->fc(&r6->body, r6->f8, r6->f10, r6->f14);
            }
            r6 = (struct Node*)(((NestedHeapIterator*)(r7))->Previous((HeapAllocator*)(r6)));
        }
        ((NestedHeapIterator*)((char*)thiz + 4))->Remove((HeapAllocator*)(r7));
        r7 = (struct Node*)(((NestedHeapIterator*)((char*)thiz + 4))->Previous(0));
    } while (r7 != 0);
end:
    ((SolidHeapAllocator*)(thiz[0]))->Reset(3);
    func_02050fb0();
    ((void(*)(void *thiz))func_02050fd4)(thiz);
}

// @symbol func_020511d4
extern "C" void *func_020511d4(HeapOwner *owner, u32 elemSize, u32 param2, u32 param3, u32 param4)
{
    AllocNode *node;
    NestedHeapIterator *iter;

    node = (AllocNode *)((SolidHeapAllocator*)(owner->heap))->Allocate(((elemSize + 0x1f) & ~0x1fu) + 0x20, 0x20);
    if (node == 0)
        return 0;

    iter = (NestedHeapIterator*)(((NestedHeapIterator*)(&owner->iter))->Previous(0));
    node->unk8  = elemSize;
    node->unkc  = param2;
    node->unk10 = param3;
    node->unk14 = param4;
    ((NestedHeapIterator*)(iter))->AddLast((HeapAllocator *)node);

    return (void *)((u32)node + 0x20);
}

// @symbol func_0205117c
extern "C" s32 func_0205117c(struct SolidHeapWithIter* self)
{
    u16 id = self->id;
    void* alloc = self->allocator;
    if (!((SolidHeapAllocator*)(alloc))->SaveState((u32)id)) {
        return ~0;
    }
    if (((s32(*)(struct SolidHeapWithIter* self))func_02050fd4)(self)) {
        return (s32)((u32)self->id - 1);
    }
    ((SolidHeapAllocator*)(self->allocator))->LoadState(0);
    return ~0;
}

// @symbol func_020510a4
extern "C" void func_020510a4(int *thiz, int n)
{
    struct Node *a;
    struct Node *first;

    a = 0;
    if (n == 0) {
        func_02051244(thiz);
        return;
    }
    if (n < *(unsigned short *)((char *)thiz + 0xc)) {
        do {
            first = (struct Node*)(((NestedHeapIterator*)((char *)thiz + 4))->Previous(0));
            a = (struct Node*)(((NestedHeapIterator*)(first))->Previous((HeapAllocator*)(a)));
            while (a != 0) {
                if (a->fc != 0) {
                    a->fc(&a->body, a->f8, a->f10, a->f14);
                }
                a = (struct Node*)(((NestedHeapIterator*)(first))->Previous((HeapAllocator*)(a)));
            }
            ((NestedHeapIterator*)((char *)thiz + 4))->Remove((HeapAllocator*)(first));
        } while (n < *(unsigned short *)((char *)thiz + 0xc));
    }
    ((SolidHeapAllocator*)(thiz[0]))->LoadState(n);
    func_02050fb0();
    ((SolidHeapAllocator*)(thiz[0]))->SaveState(*(unsigned short *)((char *)thiz + 0xc));
    ((void(*)(void *thiz))func_02050fd4)(thiz);
}

// @symbol func_02051074
extern "C" u32 func_02051074(SolidHeapAllocator **pHeap)
{
    u32 mem;
    mem = ((SolidHeapAllocator*)(*pHeap))->MemoryLeft(0x20);
    if (mem < 0x20)
        return 0;
    mem -= 0x20;
    mem &= ~0x1fu;
    return mem;
}

// @symbol func_02051064
extern "C" int func_02051064(int a)
{
    return _ZN18NestedHeapIteratorC1Ej(a, 0);
}

// @symbol func_02051024
extern "C" s32 func_02051024(struct SolidHeapWithIter* self, void* allocator)
{
    ((void(*)(void* self, u32 offset))_ZN18NestedHeapIteratorC1Ej)(&self->nhi_first, 0xc);
    self->allocator = allocator;
    if (((s32(*)(struct SolidHeapWithIter* self))func_02050fd4)(self)) {
        return 1;
    }
    return 0;
}

// @symbol func_02050fd4
extern "C" s32 func_02050fd4(struct Obj_2050fd4 *self) {
    struct HeapAllocator *r4 = (struct HeapAllocator *)((SolidHeapAllocator*)(self->heapAlloc))->Allocate(0x14, 4);
    if (!r4) {
        return 0;
    }
    ((extern void(*)(struct HeapAllocator *alloc))func_02051064)(r4);
    ((NestedHeapIterator*)(&self->iter))->AddLast((HeapAllocator*)(r4));
    return 1;
}

// @symbol func_02050fb0
extern "C" void func_02050fb0(void)
{
    void *r = func_0205afb4();
    ((void(*)(int))func_0205b070)(0x1);
    ((void(*)(void *))func_0205afe0)(r);
}

// @symbol func_02050f34
extern "C" void func_02050f34(char *self, int a1, int a2, int a3)
{
    *(int *)(self + 0x84) = 0;
    *(int *)(self + 0x7c) = 0;
    *(int *)(self + 0x80) = 0;
    ((int(*)(void *p, int b))func_0205d644)(self + 0x74, a1);
    FS_InitFile(self + 0x30);
    ((void(*)(void *p, struct TwoInt v))func_0205d568)(self + 0x30, *(struct TwoInt *)(self + 0x74));
    ((void(*)(void *dst, void *src, int n))FS_ReadFile)(self + 0x30, self, 0x30);
    if (a2 != 0)
        ((void(*)(void *self, int a, int b))func_02050d54)(self, a2, a3);
    data_020a5bb8 = self;
}

// @symbol func_02050d54
extern "C" int func_02050d54(char *obj, void *owner, int flag) {
    unsigned int sz;
    int r;
    *(void **)(obj + 0x84) = ((void *(*)(void *owner, unsigned int elemSize, void *cb, void *obj, unsigned int p4))func_020511d4)(owner, *(unsigned int *)(obj + 0x1c), (void *)func_02050988, obj, 0);
    if (*(void **)(obj + 0x84) == 0) return 0;
    *(void **)(obj + 0x7c) = ((void *(*)(void *owner, unsigned int elemSize, void *cb, void *obj, unsigned int p4))func_020511d4)(owner, *(unsigned int *)(obj + 0x24), (void *)func_0205097c, obj, 0);
    if (*(void **)(obj + 0x7c) == 0) return 0;
    if (flag != 0 && (sz = *(unsigned int *)(obj + 0x14)) != 0) {
        *(void **)(obj + 0x80) = ((void *(*)(void *owner, unsigned int elemSize, void *cb, void *obj, unsigned int p4))func_020511d4)(owner, sz, (void *)func_02050970, obj, 0);
        if (*(void **)(obj + 0x80) == 0) return 0;
    }
    if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x18), 0) == 0) return 0;
    r = ((int(*)(void *o, int a, int b))FS_ReadFile)((void *)(obj + 0x30), *(int *)(obj + 0x84), *(int *)(obj + 0x1c));
    if (r != *(int *)(obj + 0x1c)) return 0;
    if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x20), 0) == 0) return 0;
    r = ((int(*)(void *o, int a, int b))FS_ReadFile)((void *)(obj + 0x30), *(int *)(obj + 0x7c), *(int *)(obj + 0x24));
    if (r != *(int *)(obj + 0x24)) return 0;
    if (flag != 0 && *(int *)(obj + 0x14) != 0) {
        if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x10), 0) == 0) return 0;
        r = ((int(*)(void *o, int a, int b))FS_ReadFile)((void *)(obj + 0x30), *(int *)(obj + 0x80), *(int *)(obj + 0x14));
        if (r != *(int *)(obj + 0x14)) return 0;
    }
    return 1;
}

// @symbol func_02050d3c
extern "C" int func_02050d3c(int v) { int old = ((int)data_020a5bb8); *(int*)(&data_020a5bb8) = (v); return old; }

// @symbol func_02050d2c
extern "C" int func_02050d2c(void) { return ((int)data_020a5bb8); }

// @symbol func_02050cdc
extern "C" void *func_02050cdc(int a, int idx)
{
    int *p;
    struct Obj *o;
    struct Elem *e;

    p = ((int *(*)(int a))func_02050c14)(a);
    if (p == 0)
        return 0;
    o = ((struct Obj *(*)(unsigned int i))func_020509b0)((unsigned int)*p);
    if (o == 0)
        return 0;
    e = ((struct Elem *(*)(struct Obj *obj, int idx))func_020523e0)(o, idx);
    if (e == 0)
        return 0;
    return (char *)e + 4;
}

// @symbol _ZN5Sound17InfoSequenceEntry9GetWithIDEj
Sound::InfoSequenceEntry *Sound::InfoSequenceEntry::GetWithID(u32 id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x8);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (InfoSequenceEntry *)(base + v);
}

// @symbol func_02050c14
extern "C" int func_02050c14(unsigned int id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0xc);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol _ZN5Sound23InfoInstrumentBankEntry9GetWithIDEj
Sound::InfoInstrumentBankEntry *Sound::InfoInstrumentBankEntry::GetWithID(u32 id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x10);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (InfoInstrumentBankEntry *)(base + v);
}

// @symbol func_02050b4c
extern "C" int func_02050b4c(unsigned int id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x14);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050ae8
extern "C" int func_02050ae8(unsigned int id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x18);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050a84
extern "C" int func_02050a84(unsigned int id)
{
    char* root = data_020a5bb8;
    char* sub = *(char**)(root + 0x84);
    int off = *(int*)(sub + 0x1c);
    char* tbl = (off == 0) ? 0 : sub + off;
    if (tbl == 0) return 0;
    if ((int)id < 0) return 0;
    if (id >= *(unsigned*)tbl) return 0;
    int v = ((int*)(tbl + id * 4))[1];
    char* base = *(char**)(root + 0x84);
    return (v == 0) ? 0 : (int)(base + v);
}

// @symbol func_02050a5c
extern "C" int func_02050a5c(unsigned int i) {
    struct Hdr84 *h = ((data_020a5bb8_t *)data_020a5bb8)->f7c;
    if (i >= (unsigned int)h->count) return 0;
    return *(int *)((char *)h + i * 16 + 0x10);
}

// @symbol func_020509d8
extern "C" int func_020509d8(unsigned int idx, int a1, unsigned int len, unsigned int off)
{
    char *G = (char *)data_020a5bb8;
    char *T = *(char **)(G + 0x7c);
    char *entry;
    unsigned int avail;

    if (idx >= *(unsigned int *)(T + 8))
        return -1;

    entry = T + 0xc + idx * 16;
    avail = *(unsigned int *)(entry + 4) - off;
    if (len > avail)
        len = avail;

    if (((int(*)(int *o, int r1, int sel))func_0205d368)((int *)(G + 0x30), *(int *)entry + off, 0) == 0)
        return -1;

    return ((int(*)(int *o, int a1, unsigned int len))FS_ReadFile)((int *)(G + 0x30), a1, len);
}

// @symbol func_020509b0
extern "C" int func_020509b0(unsigned int i) {
    struct Hdr84 *h = ((data_020a5bb8_t *)data_020a5bb8)->f7c;
    if (i >= (unsigned int)h->count) return 0;
    return *(int *)((char *)h + i * 16 + 0x14);
}

// @symbol func_02050994
extern "C" void func_02050994(int i, int val) {
    int *p = (int*)((int *)data_020a5bb8)[0x7c / 4];
    (p + i * 4)[5] = val;
}

// @symbol func_02050988
extern "C" void func_02050988(int a0, int a1, int *p)
{
    p[33] = 0;
}

// @symbol func_0205097c
extern "C" void func_0205097c(int a0, int a1, int *p)
{
    p[31] = 0;
}

// @symbol func_02050970
extern "C" void func_02050970(int a0, int a1, int *p)
{
    p[32] = 0;
}

// @symbol func_02050950
extern "C" void func_02050950(void) { data_020a55fc = 0; *(int*)(&data_020a5634) = (0); }

// @symbol func_020508a0
extern "C" void func_020508a0(void)
{
    char *g = ((char*)&data_020a5634);
    s32 *sub;
    int v;
    if (*(s32 *)(g + 0) == 0) return;
    if (*(s32 *)(g + 4) != 0) return;
    sub = (s32 *)(g + 0x38);
    ((void(*)(s32 *p))func_02052438)(sub);
    if (*(s32 *)(g + 0x48) != 0) {
        if (((int(*)(s32 *p))func_02052420)(sub) != 0) {
            func_020502b8();
            return;
        }
    }
    v = ((int(*)(s32 *p))func_02052458)(sub) >> 8;
    if (v == *(s32 *)(g + 0x4c)) return;
    func_0205aa68(*(void **)(g + 0x20), v, 0);
    *(s32 *)(g + 0x4c) = v;
}

// @symbol func_020507e0
extern "C" int func_020507e0(int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6)
{
    volatile int z;
    func_02050798();
    if (((int*)&data_020a5634)[0] != 0) return 0;
    z = 0;
    MultiStore_Int(z, (int *)a0, a1);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)a0, a1);
    return ((int(*)(int a0, int a1, int a2, int a3, int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7, int s8, int s9, int s10))func_020503a4)(1, a0, a0 + (a1 >> 1), a1 >> 1,
                         a2, 0, 0, 1, a3, 0x7f, 0, 0x7f, a4, a5, a6);
}

// @symbol func_02050798
extern "C" void func_02050798(void){
  if (((int*)&data_020a5634)[0] == 0) return;
  if (((int*)&data_020a5634)[1] != 1) return;
  func_020502b8();
}

// @symbol func_020506fc
extern "C" void func_020506fc(int arg)
{
    if (data_020a55fc != 0)
        return;
    data_020a55f8 = 0;
    ((void(*)(int *s, int *v1, int v2))func_02058940)(&data_020a5600, &data_020a5614, 8);
    ((void(*)(int *a, void (*f)(void), int c, int *d, int e, int g))func_02058200)(&data_020a5684, func_02050038, 0, ((int*)&data_020a5bb8), 0x400, arg);
    data_020a55fc = 1;
    ((void(*)(int *a))func_02058048)(&data_020a5684);
}

// @symbol func_020503a4
extern "C" int func_020503a4(int a, int b, int c, int d, int p5, int p6, int p7, int p8,
                  int p9, int p10, int p11, int p12, int p13, int p14, int p15)
{
    int mode = 0;
    int align;
    int total;
    int blocks;
    int q;
    int fmt1;
    int fmt2;
    int m;
    int dd;
    int pp8;
    int pp10;
    u32 qlen;
    int stereo;
    int handle;
    int chunk;
    G_20503a4 *g;

    dd = (int)((d) & ~0ULL);
    pp8 = (int)((p8) & ~0ULL);
    pp10 = (int)((p10) & ~0ULL);

    handle = -1;
    g = ((G_20503a4*)&data_020a5634);
    ((void(*)(int a, int b))_ZN4CP1527FlushAndInvalidateDataCacheEjj)(b, dd);
    ((void(*)(int a, int b))_ZN4CP1527FlushAndInvalidateDataCacheEjj)(c, dd);
    stereo = (p5 == 1) ? 1 : 0;
    align = 0xffb0ff / p9;
    if (p14 != 0) {
        q = dd;
        if (stereo == 0) q = dd >> 1;
        align = (align + 0x10) & ~0x1f;
        blocks = align >> 5;
        q = q / p13;
        total = blocks * q;
        chunk = 0x20;
        if (stereo == 0) chunk = chunk >> 1;
        chunk = chunk * blocks;
    }
    {
        fmt1 = (int)(stereo != 0 ? FMT_A : FMT_B);
        fmt2 = (int)(stereo != 0 ? FMT_B : FMT_A);
        if (a != 2) mode = 0xa;
        if (p14 != 0) {
            handle = func_0204f0d8();
            if (handle < 0) return 0;
        }
        if (((int(*)(int a))func_0204f138)(3) == 0) {
            if (handle >= 0) ((void(*)(int a))func_0204f0b8)(handle);
            return 0;
        }
        if (((int(*)(int a))func_0204f194)(0xa) == 0) {
            if (handle >= 0) ((void(*)(int a))func_0204f0b8)(handle);
            ((void(*)(int a))func_0204f11c)(3);
            return 0;
        }
        qlen = (u32)dd >> 2;
        ((void(*)(int a, int b, int c, int d, int e, u32 f, int g, int h, int i, int j))func_0205aa10)(1, fmt1, b, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p11);
        ((void(*)(int a, int b, int c, u32 d, int e, int f, int g))func_0205ab6c)(0, fmt2, b, qlen, pp8, p6, p7);
        ((void(*)(int a, int b, int c, int d, int e, u32 f, int g, int h, int i, int j))func_0205aa10)(3, fmt1, c, pp8 != 0 ? 1 : 2, 0, qlen, pp10, 0, align, p12);
        ((void(*)(int a, int b, int c, u32 d, int e, int f, int g))func_0205ab6c)(1, fmt2, c, qlen, pp8, p6, p7);
        if (handle >= 0) {
            ((void(*)(int a, int b, int c, int d, void *e))func_0205ab28)(handle, total + chunk, total, (int)func_020500a4, g);
        }
        if (a == 1) {
            ((void(*)(int a, int b, int c, int d))func_0205a958)(3, 3, 1, 1);
        }
        if (handle >= 0) m = 1 << handle; else m = 0;
        ((void(*)(int a, int b, int c, int d))func_0205ac28)(mode, 3, m, 0);
        g->f0 = 1;
        g->f4 = a;
        g->f1c = 0xa;
        g->f20 = mode;
        g->f24 = 3;
        g->f28 = handle;
        g->f8 = p5;
        g->fc = b;
        g->f10 = c;
        g->f14 = dd;
        g->f18 = 0;
        g->f2c = p13;
        g->f30 = p14;
        g->f34 = p15;
        g->f4c = pp10;
        ((void(*)(void *a))func_020524c4)(g->f38);
        ((void(*)(void *a, int b, int c))func_02052494)(g->f38, pp10 << 8, 1);
        g->f48 = 0;
        return 1;
    }
}

// @symbol func_020502b8
extern "C" void func_020502b8(void)
{
    struct D *d = &data_020a5634;
    int has = (d->f28 >= 0);
    int mask = has ? (1 << d->f28) : 0;

    func_0205abb8(d->f20, d->f24, mask, 0);
    if (has) {
        void *x = func_0205afb4();
        ((void(*)(int a))func_0205b070)(1);
        ((void(*)(unsigned int c))func_0205afe0)((unsigned int)x);
        while (((int(*)(void *a, int b, int c))func_020587e4)(&data_020a5600, 0, 0) != 0) {}
    }
    if (d->f24 != 0) func_0204f11c(d->f24);
    if (d->f1c != 0) func_0204f15c((void *)d->f1c);
    if (has) func_0204f0b8(d->f28);
    if (d->f4 == 1) ((void(*)(void *a, int b, int c, int d))func_0205a958)(0, 0, 0, 0);
    d->f0 = 0;
}

// @symbol func_02050258
extern "C" void func_02050258(void)
{
    char *G = ((char*)&data_020a5634);
    int sh;
    int mask;
    void *p;
    if (*(int *)G == 0)
        return;
    sh = *(int *)(G + 0x28);
    if (sh >= 0)
        mask = 1 << sh;
    else
        mask = 0;
    ((void(*)(int a, int b, int mask, int d))func_0205abb8)(*(int *)(G + 0x20), *(int *)(G + 0x24), mask, 0);
    p = func_0205afb4();
    ((void(*)(int v))func_0205b070)(1);
    ((void(*)(void *p))func_0205afe0)(p);
}

// @symbol func_020501b8
extern "C" void func_020501b8(void)
{
    G_t* g = ((G_t*)&data_020a5634);
    volatile int a;
    volatile int b;
    if (g->f0 == 0) return;
    g->f18 = 0;
    a = 0;
    ((extern void(*)(int val, int *dst, int len))MultiStore_Int)(a, g->dstc, g->len);
    b = 0;
    ((extern void(*)(int val, int *dst, int len))MultiStore_Int)(b, g->dst10, g->len);
    ((extern void(*)(unsigned int a, unsigned int b))_ZN4CP1527FlushAndInvalidateDataCacheEjj)((unsigned int)g->dstc, g->len);
    ((extern void(*)(unsigned int a, unsigned int b))_ZN4CP1527FlushAndInvalidateDataCacheEjj)((unsigned int)g->dst10, g->len);
    {
        int sh = g->f28;
        int mask = (sh >= 0) ? (1 << sh) : 0;
        func_0205ac28(g->f20, g->f24, mask, 0);
    }
}

// @symbol func_020500a4
extern "C" void func_020500a4(struct T *thiz)
{
    int quot = thiz->f14 / thiz->f2c;
    int prod = quot * thiz->f18;
    int e6 = thiz->fc + prod;
    int e5 = thiz->f10 + prod;

    if (data_020a55fc != 0) {
        struct Q *slot = &data_020a5718[data_020a55f8];
        slot->f0 = (int)thiz;
        slot->f4 = quot;
        slot->f8 = prod;
        slot->fc = e6;
        slot->f10 = e5;
        ((void(*)(void *a, void *slot, int c))func_02058894)(&data_020a5600, slot, 0);
        data_020a55f8 = data_020a55f8 + 1;
        if (data_020a55f8 >= 8) data_020a55f8 = 0;
    } else {
        _ZN4CP1519InvalidateDataCacheEjj(e6, quot);
        _ZN4CP1519InvalidateDataCacheEjj(e5, quot);
        thiz->f30(e6, e5, quot, thiz->f8, thiz->f34);
    }
    {
        int *pf18 = (int *)((int)&thiz->f18);
        *pf18 = *pf18 + 1;
        if (thiz->f18 >= thiz->f2c) thiz->f18 = 0;
    }
}

// @symbol func_02050038
extern "C" void func_02050038(void)
{
    struct Outer *r;
    void *out;
    for (;;) {
        ((void(*)(void *a, void **out, int c))func_020587e4)(&data_020a5600, &out, 1);
        r = (struct Outer *)out;
        ((void(*)(unsigned int a, unsigned int b))_ZN4CP1519InvalidateDataCacheEjj)((unsigned int)r->hc, (unsigned int)r->h4);
        ((void(*)(unsigned int a, unsigned int b))_ZN4CP1519InvalidateDataCacheEjj)((unsigned int)r->h10, (unsigned int)r->h4);
        r->p0->h30((unsigned int)r->hc, (unsigned int)r->h10, (unsigned int)r->h4, r->p0->h8, r->p0->h34);
    }
}

// @symbol func_02050008
extern "C" void func_02050008(struct Obj1 *o) {
    if (o->p == 0) {
        return;
    }
    ((extern void(*)(void *p))func_0204f15c)(o->p);
    o->p = 0;
    o->q = 0;
}

// @symbol func_0204ffcc
extern "C" void func_0204ffcc(char *self)
{
    ((void(*)(void *a, int b, int c, int d))func_0205ac28)(*(void **)(self + 0x2c), 0, 1 << *(int *)(self + 0x28), 0);
    *(int *)(self + 0xc) |= 2;
}

// @symbol func_0204ff9c
extern "C" void func_0204ff9c(struct Obj0 *o) {
    if (o->flag) {
        ((extern void(*)(struct Obj0 *o))func_0204fde8)(o);
    }
}

// @symbol func_0204ff48
extern "C" void func_0204ff48(char *p) {
    if (!((*(int *)(p + 0xc) << 30) >> 31)) return;
    func_0205abb8(*(int *)(p + 0x2c), 0, 1 << *(int *)(p + 0x28), 0);
    void *x = func_0205afb4();
    ((void(*)(int))func_0205b070)(1);
    ((void(*)(unsigned))func_0205afe0)((unsigned)x);
}

// @symbol func_0204fed0
extern "C" void func_0204fed0(struct C *c) {
    if (((c->fc << 30) >> 31) == 0) return;
    while (c->f20 != 0) {
        unsigned int saved = IRQ::Disable();
        ((int(*)(void *c, int b))func_0204fce8)(c, 1);
        IRQ::Restore(saved);
    }
    ((void(*)(int a, int b, int c, int d))func_0205ac28)(c->f2c, 0, 1 << c->f28, 0);
}

// @symbol func_0204fe5c
extern "C" void func_0204fe5c(char *obj, int arg1)
{
  unsigned char new_var;
  int i = 0;
  *((int *) (obj + 0x24)) = arg1;
  if ((*((int *) (obj + 0x30))) > 0)
  {
    if (1)
    {
    }
    do
    {
      unsigned char idx = new_var = *((unsigned char *) ((obj + 0x34) + i));
      int r = ((int(*)(int arg))func_0205b63c)((*((int *) (obj + 0x24))) + data_020a5578[idx].field4);
      func_0205aa68((void *) (1 << idx), r & 0xff, r >> 8);
      i++;
    }
    while (i < (*((int *) (obj + 0x30))));
  }
}

// @symbol func_0204fde8
extern "C" void func_0204fde8(char* obj) {
    if ((int)(*(int*)(obj + 0xc) << 0x1e) >> 0x1f) {
        void* p;
        int* flags = (int*)(((int)obj + 0xc));
        func_0205abb8(*(int*)(obj + 0x2c), 0, 1 << *(int*)(obj + 0x28), 0);
        *flags &= ~2;
        p = ((void*(*)(void))func_0205afb4)();
        ((void(*)(int a))func_0205b070)(1);
        ((void(*)(void* p))func_0205afe0)(p);
    }
    ((void(*)(char* obj))func_0204fda4)(obj);
}

// @symbol func_0204fda4
extern "C" void func_0204fda4(char* c)
{
    int* p;
    func_0204f0b8(*(unsigned int*)(c + 0x28));
    ((NestedHeapIterator*)&data_020a552c)->Remove((HeapAllocator*)(c));
    p = (int*)(c + 0xc);
    *p &= ~1;
    *p &= ~2;
}

// @symbol func_0204fce8
extern "C" void func_0204fce8(int *thiz, void *arg)
{
  int new_var;
  int i;
  int *pcount;
  unsigned char *p = (unsigned char *) thiz;
  int q = ((int(*)(unsigned int a, unsigned int b))__aeabi_uidiv)(thiz[4], thiz[5]);
  int scaled = q * thiz[8];
  for (i = 0; i < thiz[0xc]; i++)
  {
    new_var = ((LL*)data_020a5578)[(p + i)[0x34]].lo;
    data_020a5538[i] = new_var + scaled;
  }

  ((FP) thiz[6])(arg, thiz[0xc], data_020a5538, q, thiz[2], thiz[7]);
  pcount = (int *) (((int) thiz + 0x20));
  (*pcount)++;
  if (thiz[8] >= thiz[5])
  {
    thiz[8] = 0;
  }
}

// @symbol func_0204fc40
extern "C" void func_0204fc40(void)
{
    int i;
    ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(((char*)&data_020a4d60), 0x14);
    ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(data_020a4d54, 0x14);
    for (i = 0; i < 0x10; i++) {
        data_020a50ec[i].f2c = 0;
        data_020a50ec[i].f3c = (unsigned char)i;
        ((NestedHeapIterator*)&data_020a4d54)->AddLast((HeapAllocator*)(&data_020a50ec[i]));
    }
    for (i = 0; i < 0x20; i++) {
        ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(&((struct E2*)data_020a4d6c)[i], 0xc);
        ((void(*)(void *thiz, unsigned int n))_ZN18NestedHeapIteratorC1Ej)(((struct E2*)data_020a4d6c)[i].b, 0);
        ((struct E2*)data_020a4d6c)[i].f18 = 1;
    }
}

// @symbol func_0204fafc
extern "C" void func_0204fafc(void) {
    char* node;
    char* next;
    unsigned int mask;
    int v;

    mask = func_0205b608();
    node = (char*)(((NestedHeapIterator*)(((char*)&data_020a4d60)))->Next(0));
    if (node == 0) {
        return;
    }
    do {
        next = (char*)(((NestedHeapIterator*)(((char*)&data_020a4d60)))->Next((HeapAllocator*)(node)));
        if (*(u8*)(node + 0x2d) == 0) {
            if (func_0205af78(*(u32*)(node + 0x30))) {
                *(u8*)(node + 0x2d) = 1;
            }
        }
        if (*(u8*)(node + 0x2d) != 0 && (mask & (1 << *(u8*)(node + 0x3c))) == 0) {
            ((extern void(*)(char* p))func_0204f2d4)(node);
        } else {
            int t1, t2, t3;
            ((extern int(*)(int* p))func_02052438)((int*)(node + 0x1c));
            t1 = data_02086384[*(u8*)(node + 0x41)];
            t2 = data_02086384[*(u8*)(node + 0x40)];
            t3 = data_02086384[((extern int(*)(int* p))func_02052458)((int*)(node + 0x1c)) >> 8];
            v = t3 + (t2 + t1);
            if (v < -0x2d3) {
                v = -0x2d3;
            } else if (v > 0) {
                v = 0;
            }
            if (v != *(s16*)(node + 0x3e)) {
                ((extern void(*)(unsigned int a, int b))func_0205ad3c)(*(u8*)(node + 0x3c), v);
                *(s16*)(node + 0x3e) = v;
            }
            if (*(u8*)(node + 0x2c) == 2) {
                if (((extern int(*)(int* o))func_02052420)((int*)(node + 0x1c))) {
                    ((extern void(*)(unsigned char* c))func_0204f3e0)((unsigned char*)node);
                }
            }
        }
        node = next;
    } while (next != 0);
}

// @symbol _ZN5Sound6Player19SetPlayableSeqCountEii
void Sound::Player::SetPlayableSeqCount(int index, int count)
{
    data_020a4d6c[index].mPlayableSeqCount = (unsigned short)count;
}

// @symbol func_0204fa3c
extern "C" int func_0204fa3c(int owner, void *b, u32 c)
{
    char *r4;
    void *res;
    r4 = (char *)((void *(*)(void *a, u32 elemSize, u32 p2, u32 p3, u32 p4))func_020511d4)(b, c + 0x14, (u32)func_0204f278, 0, 0);
    if (!r4) return 0;
    *(int *)(r4 + 0xc) = 0;
    *(int *)(r4 + 0x10) = owner;
    *(int *)(r4 + 8) = 0;
    res = func_0205130c((unsigned int)(r4 + 0x14), c);
    if (!res) return 0;
    *(void **)(r4 + 8) = res;
    ((NestedHeapIterator*)(((char*)data_020a4d6c) + owner * 0x1c + 0xc))->AddLast((HeapAllocator*)(r4));
    return 1;
}

// @symbol func_0204fa2c
extern "C" int func_0204fa2c(int *p, int fade)
{
    return ((int(*)(u8 *thiz, int fade))func_0204f5a0)((u8 *)*p, fade);
}

// @symbol func_0204f9c4
extern "C" void func_0204f9c4(int idx, int arg1) {
    int i;
    struct S *p = ((struct S*)data_020a50ec);
    u8 *thiz = &((u8*)data_020a4d6c)[idx * 0x1c];
    for (i = 0; i < 16; i++, p++) {
        if (p->active != 0 && p->ptr == thiz) {
            func_0204f5a0((u8 *)p, arg1);
        }
    }
}

// @symbol func_0204f958
extern "C" void func_0204f958(int idx, int val) {
    u8 *cur;
    u8 *next;
    char *iter = &((char*)data_020a4d6c)[idx * 0x1c];
    cur = (u8*)(((NestedHeapIterator*)(iter))->Next(0));
    if (cur == 0) return;
    do {
        next = (u8*)(((NestedHeapIterator*)(iter))->Next((HeapAllocator*)(cur)));
        ((void(*)(u8 *thiz, int val))func_0204f558)(cur, val);
        cur = next;
    } while (cur != 0);
}

// @symbol func_0204f94c
extern "C" void func_0204f94c(int *p)
{
    p[0] = 0;
}

// @symbol func_0204f934
extern "C" void func_0204f934(int **p) {
    int *q = *p;
    if (q != 0) {
        *q = 0;
        *p = 0;
    }
}

// @symbol func_0204f924
extern "C" void func_0204f924(void **p, unsigned char v) {
    unsigned char *o = (unsigned char *)*p;
    if (o) o[0x41] = v;
}

// @symbol func_0204f914
extern "C" void func_0204f914(void **p, unsigned char v) {
    unsigned char *o = (unsigned char *)*p;
    if (o) o[0x40] = v;
}

// @symbol func_0204f8cc
extern "C" void func_0204f8cc(char **c, int a, int b) {
    char *p = *c;
    if (p == 0) return;
    if (*(unsigned char *)(p + 0x2c) == 2) return;
    ((void(*)(void *o, int a, int b))func_02052494)(p + 0x1c, a << 8, b);
}

// @symbol func_0204f89c
extern "C" void func_0204f89c(char **c, int b) { unsigned char *p = (unsigned char *)*c; if (p) func_0205ad24(p[0x3c], b); }

// @symbol func_0204f86c
extern "C" void func_0204f86c(char **c, int b, int d) { unsigned char *p = (unsigned char *)*c; if (p) ((void(*)(int, int, int))func_0205aaf4)(p[0x3c], b, d); }

// @symbol func_0204f82c
extern "C" void func_0204f82c(void** obj, int b, int idx) {
    unsigned char* p = (unsigned char*)*obj;
    if (p == 0) return;
    func_0205acfc(p[0x3c], b, data_02086384[idx]);
}

// @symbol func_0204f7fc
extern "C" void func_0204f7fc(char **c, int b, int d) { unsigned char *p = (unsigned char *)*c; if (p) ((void(*)(int, int, int))func_0205acd4)(p[0x3c], b, d); }

// @symbol func_0204f7cc
extern "C" void func_0204f7cc(char **c, int b, int d) { unsigned char *p = (unsigned char *)*c; if (p) ((void(*)(int, int, int))func_0205acac)(p[0x3c], b, d); }

// @symbol func_0204f79c
extern "C" void func_0204f79c(char **c, int b, int d) { unsigned char *p = (unsigned char *)*c; if (p) ((void(*)(int, int, int))func_0205ac84)(p[0x3c], b, d); }

// @symbol func_0204f76c
extern "C" void func_0204f76c(char **c, int b, int d) { unsigned char *p = (unsigned char *)*c; if (p) ((void(*)(int, int, int))func_0205ac5c)(p[0x3c], b, d); }

// @symbol func_0204f73c
extern "C" void func_0204f73c(char **c, int b) { unsigned char *p = (unsigned char *)*c; if (p) func_0205ad54(p[0x3c], b); }

// @symbol func_0204f720
extern "C" void func_0204f720(struct S_204f720 **pp, short v) {
    struct S_204f720 *p = *pp;
    if (p != 0) {
        p->a = 1;
        p = *pp;
        p->b = v;
    }
}

// @symbol func_0204f6f8
extern "C" void func_0204f6f8(struct Inner83 **o, short a, short b) {
    if (*o == 0) return;
    (*o)->h34 = 2;
    (*o)->h38 = a;
    (*o)->h3a = b;
}

// @symbol func_0204f63c
extern "C" void *func_0204f63c(void **p, int idx, int r7)
{
    void *node = &((unsigned char*)data_020a4d6c)[idx * 0x1c];
    void *prev = p[0];
    void **r5;
    if (prev != 0) ((void(*)(void **p))func_0204f934)(p);
    if (*(unsigned short*)((char*)node + 8) >= *(unsigned int*)((char*)node + 0x18)) {
        unsigned char *it = (unsigned char*)(((NestedHeapIterator*)node)->Next(0));
        if (it == 0) return 0;
        if (r7 < (int)it[0x3d]) return 0;
        func_0204f3e0(it);
    }
    r5 = (void**)((void *(*)(int x))func_0204f364)(r7);
    if (r5 == 0) return 0;
    ((void(*)(void *node, void *r5))func_0204f460)(node, r5);
    r5[0] = p;
    p[0] = r5;
    return r5;
}

// @symbol func_0204f630
extern "C" void func_0204f630(void) {
    ((void(*)(void))func_0204f2d4)();
}

// @symbol func_0204f600
extern "C" int func_0204f600(void *thiz, int seqData, int seqOffset, int bank)
{
    func_0205adc4((void *)*(u8 *)((char *)thiz + 0x3c), seqData, seqOffset, bank);
    ((void(*)(void *thiz))func_0204f4bc)(thiz);
    *(void **)((char *)thiz + 0x30) = func_0205afb4();
    *(u8 *)((char *)thiz + 0x2c) = 1;
    return 1;
}

// @symbol func_0204f5a0
extern "C" void func_0204f5a0(u8 *thiz, int arg1)
{
    if (thiz == 0)
        return;
    if (thiz[0x2c] == 0)
        return;
    if (arg1 == 0) {
        ((void(*)(void *c))func_0204f3e0)(thiz);
        return;
    }
    func_02052494((struct Obj2 *)(thiz + 0x1c), 0, arg1);
    ((void(*)(void *o, int a))func_0204f214)(thiz, 0);
    thiz[0x2c] = 2;
}

// @symbol func_0204f558
extern "C" void func_0204f558(unsigned char* self, int val){
  if (self == 0) return;
  if (val == *(unsigned char*)(self + 0x2e)) return;
  ((void(*)(int a, int b))func_0205ad6c)(*(unsigned char*)(self + 0x3c), val);
  *(unsigned char*)(self + 0x2e) = (unsigned char)val;
}

// @symbol func_0204f504
extern "C" void* func_0204f504(int index, void* unk) {
    IndexEntry* entry = &((IndexEntry*)data_020a4d6c)[index];
    HeapAllocator_204f504* first = (HeapAllocator_204f504*)(((NestedHeapIterator*)(&entry->iter))->Next(0));
    if (first == 0)
        return 0;
    ((NestedHeapIterator*)(&entry->iter))->Remove((HeapAllocator*)(first));
    first->unkc = unk;
    ((HeapAllocator_204f504**)((char*)unk))[2] = first; /* unk[8] = first */
    return first->unk8;
}

// @symbol func_0204f4bc
extern "C" void func_0204f4bc(char* obj){
  *(unsigned char*)(obj + 0x2e) = 0;
  *(unsigned char*)(obj + 0x2d) = 0;
  *(unsigned short*)(obj + 0x34) = 0;
  *(unsigned short*)(obj + 0x3e) = 0;
  *(unsigned char*)(obj + 0x40) = 0x7f;
  *(unsigned char*)(obj + 0x41) = 0x7f;
  ((void(*)(void* p))func_020524c4)(obj + 0x1c);
  ((void(*)(void* o, int a, int b))func_02052494)(obj + 0x1c, 0x7f00, 1);
}

// @symbol func_0204f460
extern "C" void func_0204f460(NestedHeapIterator* self, HeapAllocator_204f460* node) {
    HeapAllocator_204f460* cur;
    cur = (HeapAllocator_204f460*)(((NestedHeapIterator*)(self))->Next(0));
    if (cur != 0) {
        for (;;) {
            if (node->sortKey < cur->sortKey)
                break;
            cur = (HeapAllocator_204f460*)(((NestedHeapIterator*)(self))->Next((HeapAllocator*)(cur)));
            if (cur == 0)
                break;
        }
    }
    ((NestedHeapIterator*)(self))->AddAt((HeapAllocator*)(cur), (HeapAllocator*)(node));
    node->owner = self;
}

// @symbol func_0204f400
extern "C" void func_0204f400(HeapAllocator_204f400* self) {
    HeapAllocator_204f400* cur;
    cur = (HeapAllocator_204f400*)(((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->Next(0));
    if (cur != 0) {
        for (;;) {
            if (self->sortKey < cur->sortKey)
                break;
            cur = (HeapAllocator_204f400*)(((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->Next((HeapAllocator*)(cur)));
            if (cur == 0)
                break;
        }
    }
    ((NestedHeapIterator*)(((NestedHeapIterator*)&data_020a4d60)))->AddAt((HeapAllocator*)(cur), (HeapAllocator*)(self));
}

// @symbol func_0204f3e0
extern "C" void func_0204f3e0(unsigned char *c)
{
    ((void(*)(unsigned int x))func_0205ad98)(c[0x3c]);
    ((void(*)(void *c))func_0204f2d4)(c);
}

// @symbol func_0204f364
extern "C" char* func_0204f364(int key)
{
    char* node = (char*)(((NestedHeapIterator*)&data_020a4d54)->Next(0));
    if (node == 0) {
        node = (char*)(((NestedHeapIterator*)(((char*)&data_020a4d60)))->Next(0));
        if (key < *(u8*)(node + 0x3d)) {
            return 0;
        }
        ((extern void(*)(unsigned char* c))func_0204f3e0)((unsigned char*)node);
    }
    ((NestedHeapIterator*)&data_020a4d54)->Remove((HeapAllocator*)(node));
    *(u8*)(node + 0x3d) = key;
    ((extern void(*)(char* self))func_0204f400)(node);
    return node;
}

// @symbol func_0204f2d4
extern "C" void func_0204f2d4(HeapObj* obj) {
    HeapAllocator* p0 = obj->unk0;
    if (p0 != 0) {
        *(HeapAllocator**)p0 = 0;
        obj->unk0 = 0;
    }
    HeapAllocator* heap = obj->heap;
    ((NestedHeapIterator*)(heap))->Remove((HeapAllocator*)obj);
    obj->heap = 0;
    if (obj->unk8 != 0) {
        ((NestedHeapIterator*)((HeapAllocator*)((u8*)heap + 0xc)))->AddLast((HeapAllocator*)(obj->unk8));
        *(HeapAllocator**)((u8*)obj->unk8 + 0xc) = 0;
        obj->unk8 = 0;
    }
    ((NestedHeapIterator*)(((HeapAllocator*)&data_020a4d60)))->Remove((HeapAllocator*)obj);
    ((NestedHeapIterator*)(((HeapAllocator*)data_020a4d54)))->AddLast((HeapAllocator*)obj);
    *(u8*)((u8*)obj + 0x2c) = 0;
}

// @symbol func_0204f278
extern "C" void func_0204f278(char *thiz)
{
    int *p = *(int **)(thiz + 8);
    if (p == 0) return;
    func_020512f0(p);
    int *q = *(int **)(thiz + 0xc);
    if (q != 0) {
        q[2] = 0;
        return;
    }
    int idx = *(int *)(thiz + 0x10);
    ((NestedHeapIterator*)((HeapAllocator *)((char *)&data_020a4d6c[idx] + 0xc)))->Remove((HeapAllocator*)(thiz));
}

// @symbol func_0204f214
extern "C" void func_0204f214(char* thiz, int b)
{
    void* node = *(void**)(thiz + 4);
    if (node != 0) {
        ((NestedHeapIterator*)(node))->Remove((HeapAllocator*)(thiz));
        *(void**)(thiz + 4) = 0;
    }
    ((NestedHeapIterator*)(((char*)&data_020a4d60)))->Remove((HeapAllocator*)(thiz));
    *(unsigned char*)(thiz + 0x3d) = (unsigned char)b;
    if (node != 0) {
        ((void(*)(void* node, void* self))func_0204f460)(node, thiz);
    }
    ((void(*)(void* self))func_0204f400)(thiz);
}

// @symbol func_0204f1e8
extern "C" void func_0204f1e8(void) {
    data_020a4d50 = 0;
    data_020a4d48 = 0;
    data_020a4d4c = 0;
}

// @symbol func_0204f194
extern "C" int func_0204f194(unsigned int a) {
    if (a == 0) return 1;
    if (a & data_020a4d50) return 0;
    func_0205aac8((void *)a, 0);
    data_020a4d50 |= a;
    return 1;
}

// @symbol func_0204f15c
extern "C" void func_0204f15c(void *node)
{
    if (node == 0)
        return;
    func_0205aa9c(node, 0);
    data_020a4d50 &= ~(unsigned int)node;
}

// @symbol func_0204f138
extern "C" int func_0204f138(unsigned int mask)
{
    if (mask & data_020a4d48)
        return 0;
    data_020a4d48 = data_020a4d48 | mask;
    return 1;
}

// @symbol func_0204f11c
extern "C" void func_0204f11c(int mask) {
    data_020a4d48 &= ~mask;
}

// @symbol func_0204f0d8
extern "C" int func_0204f0d8(void)
{
    int i;
    int mask = 1;
    unsigned int v = data_020a4d4c;
    for (i = 0; i < 8; i++)
    {
        if ((v & mask) == 0)
        {
            data_020a4d4c |= mask;
            return i;
        }
        mask <<= 1;
    }
    return -1;
}

// @symbol func_0204f0b8
extern "C" int func_0204f0b8(unsigned int bit)
{
    return data_020a4d4c &= ~(1 << bit);
}

// @symbol func_0204f070
extern "C" void func_0204f070(void){
  if (data_020a4d44 != 0) return;
  data_020a4d44 = 1;
  func_0205a82c();
  func_0204f1e8();
  func_02050950();
  func_0204fc40();
}

// @symbol func_0204f03c
extern "C" void func_0204f03c(void)
{
    while (((int(*)(int))func_0205b274)(0) != 0)
        ;
    func_0204fafc();
    func_020508a0();
    func_020522c4();
    ((void(*)(int))func_0205b070)(0);
}

// @symbol func_0204effc
extern "C" void func_0204effc(void) {
    void* p;
    func_0205227c();
    func_02050258();
    func_0205abb8(0, 0, 0, 0);
    p = ((void*(*)(void))func_0205afb4)();
    ((void(*)(int a))func_0205b070)(1);
    ((void(*)(void* p))func_0205afe0)(p);
}

// @symbol func_0204efe0
extern "C" void func_0204efe0(s8 levelID)
{
    s8 courseID = ((s8(*)(s8 levelID))func_020501b8)(levelID);
    ((void(*)(s8 courseID))func_02052234)(courseID);
}

// @symbol func_0204ef9c
extern "C" int func_0204ef9c(void* p) {
    if (*(u32*)p != 0x4352414e) return 0;
    if (*(u16*)((char*)p + 4) != 0xfffe) return 0;
    return *(u16*)((char*)p + 6) == 0x100;
}

// @symbol func_0204ee40
extern "C" int func_0204ee40(void *self, char *name, char *p)
{
    char *fatb;
    char *fntb;
    char *fimg;
    int i;
    char *cur;
    int n;
    char *fimg_off;

    fatb = 0;
    fntb = 0;
    fimg = 0;

    if (((int(*)(void *p))func_0204ef9c)(p) == 0)
        return 0;

    cur = p + *(unsigned short *)(p + 0xc);
    n = *(unsigned short *)(p + 0xe);
    i = 0;
    if (n > 0) {
        do {
            unsigned int m = *(unsigned int *)cur;
            switch (m) {
            case 0x46415442:
                fatb = cur;
                break;
            case 0x464e5442:
                fntb = cur;
                break;
            case 0x46494d47:
                fimg = cur;
                break;
            }
            cur += *(int *)(cur + 4);
            i++;
        } while (i < n);
    }

    func_0205cd34(self);
    *(char **)((char *)self + 0x50) = p;
    *(char **)((char *)self + 0x54) = fatb;
    fimg_off = fimg + 8;
    *(char **)((char *)self + 0x58) = fimg_off;

    if (func_0205cc80(self, (void *)(name),  (void *)((int(*)(const char *s))_ZN4cstd6strlenEPKc)(name)) == 0)
        return 0;

    if (((int(*)(void *p, int a, int b0, int b4, int c0, int c4, void *fn1, void *fn2))func_0205cb68)(self, (int)fimg_off, (int)(fatb + 0xc) - (int)fimg_off,
                      *(int *)(fatb + 4) - 0xc, (int)(fntb + 8) - (int)fimg_off,
                      *(int *)(fntb + 4) - 8, 0, 0))
        return 1;

    func_0205cbe4(self);
    return 0;
}

// @symbol func_0204ee10
extern "C" int func_0204ee10(void *thiz)
{
    if (!func_0205caa8(thiz)) return 0;
    func_0205cbe4(thiz);
    return 1;
}

// @symbol func_0204ede8
extern "C" int func_0204ede8(struct S82 *o, unsigned int i) {
    unsigned char *base = o->f54;
    int r = 0;
    if (i < *(unsigned short *)(base + 8)) {
        r = (int)o->f58 + *(int *)(base + i * 8 + 0xc);
    }
    return r;
}

// @symbol func_0204eda4
extern "C" int func_0204eda4(int r0, char* a, u32 idx) {
    int r = 0;
    if (idx < *(u16*)(*(char**)(a + 0x54) + 8)) {
        struct Pair p;
        p.a = a;
        p.idx = idx;
        r = ((int(*)(int r0, struct Pair p))func_0205d568)(r0, p);
    }
    return r;
}

