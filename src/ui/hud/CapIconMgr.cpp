//cpp
/* CapIconMgr -- the cap-icon request manager, ov001.
 *
 * This is the "other TU" that dCapIcon_c's header cites: the free-function
 * module that runs the per-character cap-icon lists. It owns the request
 * flags in data_ov001_020ad628/ad62c, the list heads in
 * data_ov001_020ad634, the flash timer data_ov001_020ad620, and the
 * dispatch lock data_ov001_020ad624.
 *
 * The per-frame tick (func_ov001_020aaf40) counts down the flash timer,
 * then dispatches: func_ov001_020aa420 in versus mode, the reset pass
 * func_ov001_020aadac when the local player has no cap, or the main
 * dispatcher func_ov001_020aaa54. func_ov001_020aa960 gates a player,
 * func_ov001_020aa7b8 and func_ov001_020aa858 are the eligibility tests,
 * func_ov001_020aa79c the in-flight test, func_ov001_020aa6e4 the commit
 * (sound + 60-frame flash), and func_ov001_020aa6b0/func_ov001_020aa6cc
 * the state-byte setters.
 *
 * deslop leftovers:
 * - Symbols stay func_ov001_020aa* linker names: the cartridge records no
 *   mangled names for this module and callers cross TU boundaries.
 * - data_ov001_020ad62* / data_0209f394 / data_0209f3e8 keep linker names
 *   (S14 data outside this .text; the player array's type is not in ROM).
 * - The CapNode overlay in func_ov001_020aa420 and the int* walks in
 *   func_ov001_020aadac are kept: byte-neutral retyping on dCapIcon_c was
 *   measured and left as the shard bodies wrote it.
 */

#include "types.h"
#include "CapIcon.h"
#include "dActor_c.h"
#include "Player.h"

extern "C" {

extern unsigned char data_ov001_020ad620;
extern unsigned char data_ov001_020ad624;
extern unsigned char data_ov001_020ad628[];
extern unsigned char data_ov001_020ad62c[];
extern unsigned char data_ov001_020ad630[];
extern dCapIcon_c *data_ov001_020ad634[];
extern unsigned char data_ov002_02111154[];
extern unsigned char data_0208a0e0;
extern signed char data_02092114;
extern int data_0209f3e8[];
/* The Player*[] array. The shards disagreed scalar-vs-array; the array
 * spelling gives the same ldr for both uses ([0] = first element). */
extern void *data_0209f394[];
extern unsigned char data_0209f284;
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;

/* local extern: func_0202a8e0's first parameter is an address the callers
 * pass as object pointers; the definition spells it int. */
extern int func_0202a8e0(int a, u8 b);
extern int func_02029600(int x);
extern unsigned int func_02012790(unsigned int n);
extern int SublevelToLevel(int i);

void func_ov001_020aa420(void);
void func_ov001_020aa6b0(int *node, int value);
void func_ov001_020aa6cc(int character);
void func_ov001_020aa6e4(int idx, unsigned r1, int *obj);
int func_ov001_020aa79c(int x);
int func_ov001_020aa7b8(void *self, char *a);
int func_ov001_020aa858(int self, char *a, int b, int d);
int func_ov001_020aa960(int x, char *arg);
void func_ov001_020aaa54(void);
void func_ov001_020aadac(void);
void func_ov001_020aaf40(void);

/* data_0209f2d8 is 1 in versus mode. This int-returning predicate is what
 * makes the ROM materialize 0/1 before testing it. */
static inline int IsVsMode(void) {
    return data_0209f2d8 == 1;
}

/* Definitions emit in reverse source order. */

void func_ov001_020aaf40(void)
{
    int j;
    int i;

    if (data_ov001_020ad620 != 0) {
        data_ov001_020ad620 -= 1;
        for (i = 0; i < 3; i++) {
            char *node = (char *)data_ov001_020ad634[i];
            while (node) {
                if (*(int*)(node + 0x14) != -1 && *(unsigned char*)(node + 0x1a) != 0) {
                    int v;
                    *(unsigned char *)(int)(node + 0x1a) -= 1;
                    v = *(unsigned char*)(node + 0x1a);
                    if (v == 0) {
                        func_ov001_020aa6b0((int *)node, 1);
                    } else if (v % 10 == 0) {
                        func_ov001_020aa6b0((int *)node, ((v / 10) & 1) == 0 ? 1 : 0);
                    }
                }
                node = *(char**)(node + 0x10);
            }
        }
        if (data_ov001_020ad620 == 0)
            data_0209f284 = 0;
    }

    if (IsVsMode()) {
        func_ov001_020aa420();
        return;
    }

    {
        char *pl = (char *)data_0209f394[0];
        if (((Player *)pl)->HasNoCap() && !((Player *)pl)->IsCollectingCap()) {
            func_ov001_020aadac();
            data_ov001_020ad624 = 1;
            return;
        }
        for (j = 0; j < 3; j++) {
            if (data_ov001_020ad62c[j] & 2)
                data_ov001_020ad62c[j] &= ~3;
        }
        func_ov001_020aaa54();
        data_ov001_020ad624 = 0;
    }
}

#define LAUNDER_U8_PTR(p) ((unsigned char*)(p))

void func_ov001_020aadac(void) {
    char* fp = (char *)data_0209f394[0];
    int* sl;
    unsigned i1, i2;

    for (i1 = 0; i1 < 3; i1 = (i1 + 1) & 0xff) {
        if (data_ov001_020ad62c[i1] & 2) continue;
        func_ov001_020aa6cc(i1);
        sl = (int *)data_ov001_020ad634[i1];
        while (sl != 0) {
            unsigned char* fl = LAUNDER_U8_PTR((char*)sl + 0x1b);
            *fl &= ~2;
            if (sl[0x14/4] != -1) {
                func_ov001_020aa6b0(sl, 0);
                data_0209f3e8[sl[0x14/4]] = 0;
            }
            sl = (int*)sl[0x10/4];
        }
        if (i1 != *(unsigned char*)(fp + 0x6d9)) {
            data_ov001_020ad62c[i1] |= 2;
        }
    }

    for (i2 = 0; i2 < 3; i2 = (i2 + 1) & 0xff) {
        unsigned char* flags;
        int* o;
        flags = &data_ov001_020ad62c[i2];
        if (*flags & 2) continue;
        o = (int *)data_ov001_020ad634[i2];
        if (o == 0) continue;
        do {
            void* fw = dActor_c::FindWithID(o[8/4]);
            if (fw != 0 || (int)fw != o[4/4]) {
                if (*(unsigned char*)((char*)o + 0x19) == 4) {
                    unsigned char* fl = LAUNDER_U8_PTR((char*)o + 0x1b);
                    *fl |= 2;
                    o[0x14/4] = func_0202a8e0(o[4/4], *(unsigned char*)((char*)o + 0x18));
                    func_ov001_020aa6b0(o, 1);
                    func_ov001_020aa6e4(i2, *(unsigned char*)((char*)o + 0x19), 0);
                    *flags |= 2;
                }
            } else {
                ((dCapIcon_c *)o)->Unlink();
            }
            o = (int*)o[0x10/4];
        } while (o != 0);
    }
}

/* Route the +0x1b flag-byte address through 64-bit arithmetic so it is
 * materialized (add rX,base,#0x1b) for the read-modify-write, matching the ROM. */
#define FLAGS1B(p) (*(u8*)(((int)(p) + 0x1b)))

void func_ov001_020aaa54(void)
{
    unsigned sp4;
    int typ;
    int spc;
    void* player = data_0209f394[0];
    int zero = 0;
    int three = 3;
    void* z1 = 0;
    void* z2 = 0;
    void* z3 = 0;
    dCapIcon_c* sl;
    dCapIcon_c* sb;
    unsigned r8;
    int r7;
    unsigned r6;
    int fp;
    u8* flagp;

    if (((Player *)player)->IsCollectingCap() != 0) {
        unsigned i = 0;
        do {
            u8 v = data_ov001_020ad628[i];
            if (v & 0x40) data_ov001_020ad628[i] |= 0x80;
            i = (i + 1) & 0xff;
        } while (i < 3);
    }

    r6 = 0;
    do {
        u8 tb = data_ov001_020ad628[r6] & 3;
        r8 = zero;
        spc = zero;
        typ = tb;
        sb = (dCapIcon_c*)zero;
        r7 = zero;
        flagp = &data_ov001_020ad628[r6];
        fp = zero;

        if (func_ov001_020aa960(r6, (char *)player) != 0) {
            sl = data_ov001_020ad634[r6];
            while (sl != 0) {
                void* found = dActor_c::FindWithID(sl->mOwnerUniqueID);
                if (found != 0 || found != sl->mOwner) {
                    int b1b = sl->mFlags;
                    int bit0 = (unsigned)(b1b << 0x1f) >> 0x1f;
                    if (bit0 == 0 && (unsigned)(b1b << 0x1e) >> 0x1f) {
                        fp = 1;
                        r8 = sl->unk_19;
                    } else {
                        int t = *flagp;
                        if ((t & 0x30) || sl->unk_19 >= 2) {
                            if (bit0) {
                                if (sl->mSlot == -1) {
                                    sl->mSlot = func_0202a8e0((int)sl->mOwner, sl->mCharacter);
                                    FLAGS1B(sl) |= 2;
                                    func_ov001_020aa6b0((int *)sl, 1);
                                } else {
                                    spc = 1;
                                }
                                r7 = 1;
                                sp4 = sl->unk_19;
                                if ((*flagp & 3) == 0)
                                    *flagp |= 1;
                                if (sl->unk_19 >= 2) *flagp |= 0x20;
                            } else {
                                if (sl->unk_19 != 0) {
                                    if (r8 < sl->unk_19) {
                                        r8 = sl->unk_19;
                                        sb = sl;
                                    }
                                    if ((t & 3) == 0)
                                        *flagp |= 1;
                                    if (sl->unk_19 >= 2)
                                        *flagp |= 0x20;
                                } else {
                                    if ((t & 3) == 1 && r8 == 0) {
                                        if ((unsigned)(((volatile u8 *)sl)[0x1b] << 0x1c) >> 0x1f) {
                                            sb = sl;
                                        } else {
                                            FLAGS1B(sl) |= 8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else {
                    sl->Unlink();
                }
                sl = sl->mNext;
            }

            /* The ROM's caller passes a fifth argument the callee ignores
             * (the shards' extern declared five). The cast preserves the
             * pushed r7; the definition takes four. */
            if (((int (*)(int, void *, int, int, int))func_ov001_020aa858)(r6, sb, typ, fp, r7) != 0) {
                FLAGS1B(sb) |= 2;
                sb->mSlot = func_0202a8e0((int)sb->mOwner, sb->mCharacter);
                func_ov001_020aa6b0((int *)sb, 1);
                func_ov001_020aa6e4(r6, sb->unk_19, (int *)sb);
            } else {
                if (r7 != 0 && (((Player *)player)->IsCollectingCap() != 0 || (*flagp & 0x40) != 0)) {
                    if (spc != 0) {
                        func_ov001_020aa6e4(r6, three, (int *)z1);
                    } else {
                        func_ov001_020aa6e4(r6, sp4, (int *)z2);
                    }
                } else {
                    if (fp != 0) {
                        func_ov001_020aa6e4(r6, three, (int *)z3);
                    }
                }
            }
        }

        if ((*flagp & 3) == 0) {
            int lvl = SublevelToLevel(data_0209f2f8);
            if ((unsigned)(lvl - 0xf) <= 2) *flagp |= 1;
            else *flagp |= 2;
        }

        r6 = (r6 + 1) & 0xff;
    } while (r6 < 3);
}

int func_ov001_020aa960(int x, char* arg){
  if((data_ov001_020ad628[x] & 0xc0) == 0xc0)
    data_ov001_020ad628[x] &= ~0x40;
  unsigned char v = data_ov001_020ad628[x];
  if(v & 8){
    data_ov001_020ad628[x] = v & ~8;
    return 1;
  }
  if(data_ov001_020ad630[x] == 0)
    return 0;
  if(v & 0x40)
    return 0;
  if(x == *(unsigned char*)(arg+0x6d9))
    return 0;
  if(func_ov001_020aa79c(x))
    return 0;
  if((data_ov001_020ad628[x] & 3) == 2)
    return 0;
  if (!((Player *)arg)->IsCollectingCap() && x == *(int *)(arg + 8) && (data_ov001_020ad628[x] & 3))
      return 0;
  return 1;
}

int func_ov001_020aa858(int self, char* a, int b, int d)
{
    if (b == 0 && self == data_02092114) {
        data_ov001_020ad628[self] |= 0x40;
        data_ov001_020ad62c[self] |= 0x80;
        return 0;
    }
    if (a == 0) return 0;
    unsigned char k = *(unsigned char*)(a+0x19);
    if (k >= 3) return 1;
    if (d != 0) return 0;
    void* p = data_0209f394[0];
    if (k != 4) {
        if (((Player *)p)->IsCollectingCap()) return 0;
    }
    if (!((Player *)p)->IsCollectingCap()) {
        if (self == *(int*)((char*)p+8)) return 0;
    }
    return 1;
}

int func_ov001_020aa7b8(void* self, char* a)
{
    if (a == 0) return 0;
    if (*(unsigned char*)(a+0x19) == 3) return 1;
    int i = 0;
    for (i = 0; i < ((unsigned char *)&data_0208a0e0)[0]; i++) {
        void* p = data_0209f394[i];
        if (p == 0) continue;
        if (((Player *)p)->IsCollectingCap()) return 0;
        if (self == *(void**)((char*)p+8)) return 0;
    }
    return 1;
}

int func_ov001_020aa79c(int x) {
    return (((unsigned char*)data_ov001_020ad62c)[x] & 3) != 0;
}

void func_ov001_020aa6e4(int idx, unsigned r1, int* obj){
  data_ov001_020ad62c[idx] |= 1;
  if((data_ov001_020ad62c[idx] & 0x80) && data_ov001_020ad624==0){
    if(obj==0) return;
    if(r1>=3) return;
    if(func_02029600(obj[1])==0) return;
    data_0209f284=1;
    func_02012790(0x24);
    *((char*)obj+0x1a)=0x3c;
    data_ov001_020ad620=*((unsigned char*)obj+0x1a);
    return;
  }
  data_ov001_020ad62c[idx] |= 0x80;
}

void func_ov001_020aa6cc(int r0) {
    ((unsigned char*)data_ov001_020ad62c)[r0] &= ~3;
}

void func_ov001_020aa6b0(int* r0, int r1) {
    int r2 = r0[5];
    if (r2 != -1) {
        data_ov002_02111154[r2] = (unsigned char)r1;
    }
}

void func_ov001_020aa420(void) {
    typedef struct {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char pad : 4;
    } Flags;

    typedef struct CapNode {
        u8 pad0[4];
        int field4;
        int field8;
        u8 padC[4];
        struct CapNode *next;
        int soundHandle;
        u8 field18;
        u8 field19;
        u8 pad1A;
        Flags flags;
    } CapNode;

    typedef struct CapRequest {
        u8 pad0[8];
        int field8;
    } CapRequest;

    CapRequest *player;
    CapNode *best;
    int handled;
    u8 *flagByte;
    u8 reqFlag;
    CapNode *node;
    u8 maxPriority;
    int spC;
    int sp10;
    unsigned char i;
    void *found;
    u8 cnt;
    int idx;

    maxPriority = 0;
    i = 0;
    spC = 0;
    sp10 = 0;

    for (; i < 3; i++) {
        best = 0;
        node = (CapNode *)data_ov001_020ad634[i];
        handled = (int)best;
        flagByte = &data_ov001_020ad628[i];
        reqFlag = data_ov001_020ad628[i];

        if (reqFlag == 0) {
            if (node != 0) {
                do {
                    *(u8 *)((char *)node + 0x1b) &= ~2;
                    if (node->field19 == 1) {
                        *flagByte = 1;
                    }
                    node = node->next;
                } while (node != 0);
            }
            if (*flagByte == 0) {
                *flagByte = 2;
            } else {
                *flagByte = 1;
            }
        }

        node = *(CapNode *volatile *)&data_ov001_020ad634[i];

        if (func_ov001_020aa79c(i) != 0) continue;
        if (*flagByte == 2) continue;
        if (data_ov001_020ad630[i] == 0) continue;

        if (node != 0) {
            do {
                found = dActor_c::FindWithID(node->field8);
                if (found != 0 || (int)found != node->field4) {
                    if (node->flags.b0) {
                        if (node->soundHandle == -1) {
                            node->soundHandle = func_0202a8e0(node->field4, node->field18);
                            *(u8 *)((char *)node + 0x1b) |= 2;
                            func_ov001_020aa6b0((int *)node, 1);
                        }
                        handled = 1;
                        maxPriority = node->field19;
                    } else if (node->field19 != 0) {
                        if (maxPriority < node->field19) {
                            maxPriority = node->field19;
                            best = node;
                        }
                    } else if (maxPriority == 0) {
                        if (node->flags.b3) {
                            best = node;
                        } else {
                            *(u8 *)((char *)node + 0x1b) |= 8;
                        }
                    }
                } else {
                    ((dCapIcon_c *)node)->Unlink();
                }
                node = node->next;
            } while (node != 0);
        }

        if (func_ov001_020aa7b8((void *)i, (char *)best) != 0) {
            *(u8 *)((char *)best + 0x1b) |= 2;
            best->soundHandle = func_0202a8e0(best->field4, best->field18);
            func_ov001_020aa6b0((int *)best, 1);
            func_ov001_020aa6e4(i, best->field19, (int *)best);
            continue;
        }

        if (handled == 0) continue;

        cnt = data_0208a0e0;
        for (idx = spC; idx < cnt; idx++) {
            player = (CapRequest *)data_0209f394[idx];
            if (i == player->field8) break;
            player = 0;
        }
        if (player != 0 && ((Player *)player)->IsCollectingCap() != 0) {
            func_ov001_020aa6e4(i, maxPriority, (int *)sp10);
        }
    }
}

}
