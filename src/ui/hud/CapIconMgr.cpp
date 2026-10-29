//cpp
/* CapIconMgr -- the cap-icon request manager, ov001.
 *
 * This free-function module runs the per-character cap-icon lists.
 * The shared request flags, dispatch state, counts and heads are defined
 * here for this reconstruction; their original TU placement is unknown.
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
 * - data_0209f394 / data_0209f3e8 keep linker names; their ownership and
 *   the player array's type remain outside this reconstruction.
 * - The CapNode overlay in func_ov001_020aa420 and the int* walks in
 *   func_ov001_020aadac are kept: byte-neutral retyping on dCapIcon_c was
 *   measured and left as the shard bodies wrote it.
 */

#include "types.h"
#include "CapIconMgr.h"
#include "dActor_c.h"
#include "Player.h"
#include "Sound.h"

extern "C" {

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

    if (gCapIconFlashTimer != 0) {
        gCapIconFlashTimer -= 1;
        for (i = 0; i < 3; i++) {
            char *node = (char *)gCapIconListHeads[i];
            while (node) {
                if (((dCapIcon_c *)node)->mSlot != -1 && ((dCapIcon_c *)node)->mFlashFrames != 0) {
                    int v;
                    ((dCapIcon_c *)node)->mFlashFrames -= 1;
                    v = ((dCapIcon_c *)node)->mFlashFrames;
                    if (v == 0) {
                        func_ov001_020aa6b0((int *)node, 1);
                    } else if (v % 10 == 0) {
                        func_ov001_020aa6b0((int *)node, ((v / 10) & 1) == 0 ? 1 : 0);
                    }
                }
                node = (char *)((dCapIcon_c *)node)->mNext;
            }
        }
        if (gCapIconFlashTimer == 0)
            data_0209f284 = 0;
    }

    if (IsVsMode()) {
        func_ov001_020aa420();
        return;
    }

    {
        Player *pl = (Player *)data_0209f394[0];
        if (pl->HasNoCap() && !pl->IsCollectingCap()) {
            func_ov001_020aadac();
            gCapIconDispatchLocked = 1;
            return;
        }
        for (j = 0; j < 3; j++) {
            if (gCapIconDispatchFlags[j] & 2)
                gCapIconDispatchFlags[j] &= ~3;
        }
        func_ov001_020aaa54();
        gCapIconDispatchLocked = 0;
    }
}

#define LAUNDER_U8_PTR(p) ((unsigned char*)(p))

void func_ov001_020aadac(void) {
    char* fp = (char *)data_0209f394[0];
    int* sl;
    unsigned i1, i2;

    for (i1 = 0; i1 < 3; i1 = (i1 + 1) & 0xff) {
        if (gCapIconDispatchFlags[i1] & 2)
            continue;
        func_ov001_020aa6cc(i1);
        sl = (int *)gCapIconListHeads[i1];
        while (sl != 0) {
            unsigned char *fl = LAUNDER_U8_PTR(&((dCapIcon_c *)sl)->mFlags);
            *fl &= ~2;
            if (((dCapIcon_c *)sl)->mSlot != -1) {
                func_ov001_020aa6b0(sl, 0);
                data_0209f3e8[((dCapIcon_c *)sl)->mSlot] = 0;
            }
            sl = (int *)((dCapIcon_c *)sl)->mNext;
        }
        if (i1 != ((Player *)fp)->mCharacter) {
            gCapIconDispatchFlags[i1] |= 2;
        }
    }

    for (i2 = 0; i2 < 3; i2 = (i2 + 1) & 0xff) {
        unsigned char* flags;
        int* o;
        flags = &gCapIconDispatchFlags[i2];
        if (*flags & 2) continue;
        o = (int *)gCapIconListHeads[i2];
        if (o == 0) continue;
        do {
            void *fw = dActor_c::FindWithID(((dCapIcon_c *)o)->mOwnerUniqueID);
            if (fw != 0 || fw != ((dCapIcon_c *)o)->mOwner) {
                if (((dCapIcon_c *)o)->mIconKind == 4) {
                    unsigned char *fl = LAUNDER_U8_PTR(&((dCapIcon_c *)o)->mFlags);
                    *fl |= 2;
                    ((dCapIcon_c *)o)->mSlot = func_0202a8e0((int)((dCapIcon_c *)o)->mOwner,
                                                             ((dCapIcon_c *)o)->mCharacter);
                    func_ov001_020aa6b0(o, 1);
                    func_ov001_020aa6e4(i2, ((dCapIcon_c *)o)->mIconKind, 0);
                    *flags |= 2;
                }
            } else {
                ((dCapIcon_c *)o)->Unlink();
            }
            o = (int *)((dCapIcon_c *)o)->mNext;
        } while (o != 0);
    }
}

void func_ov001_020aaa54(void)
{
    unsigned sp4;
    int typ;
    int spc;
    Player *player = (Player *)data_0209f394[0];
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

    if (player->IsCollectingCap() != 0) {
        unsigned i = 0;
        do {
            u8 v = gCapIconRequestFlags[i];
            if (v & 0x40)
                gCapIconRequestFlags[i] |= 0x80;
            i = (i + 1) & 0xff;
        } while (i < 3);
    }

    r6 = 0;
    do {
        u8 tb = gCapIconRequestFlags[r6] & 3;
        r8 = zero;
        spc = zero;
        typ = tb;
        sb = (dCapIcon_c*)zero;
        r7 = zero;
        flagp = &gCapIconRequestFlags[r6];
        fp = zero;

        if (func_ov001_020aa960(r6, (char *)player) != 0) {
            sl = gCapIconListHeads[r6];
            while (sl != 0) {
                void* found = dActor_c::FindWithID(sl->mOwnerUniqueID);
                if (found != 0 || found != sl->mOwner) {
                    int b1b = sl->mFlags;
                    int bit0 = (unsigned)(b1b << 0x1f) >> 0x1f;
                    if (bit0 == 0 && (unsigned)(b1b << 0x1e) >> 0x1f) {
                        fp = 1;
                        r8 = sl->mIconKind;
                    } else {
                        int t = *flagp;
                        if ((t & 0x30) || sl->mIconKind >= 2) {
                            if (bit0) {
                                if (sl->mSlot == -1) {
                                    sl->mSlot = func_0202a8e0((int)sl->mOwner, sl->mCharacter);
                                    sl->mFlags |= 2;
                                    func_ov001_020aa6b0((int *)sl, 1);
                                } else {
                                    spc = 1;
                                }
                                r7 = 1;
                                sp4 = sl->mIconKind;
                                if ((*flagp & 3) == 0)
                                    *flagp |= 1;
                                if (sl->mIconKind >= 2)
                                    *flagp |= 0x20;
                            } else {
                                if (sl->mIconKind != 0) {
                                    if (r8 < sl->mIconKind) {
                                        r8 = sl->mIconKind;
                                        sb = sl;
                                    }
                                    if ((t & 3) == 0)
                                        *flagp |= 1;
                                    if (sl->mIconKind >= 2)
                                        *flagp |= 0x20;
                                } else {
                                    if ((t & 3) == 1 && r8 == 0) {
                                        /* volatile is load-bearing: a plain sl->mFlags read DIFFs. */
                                        if ((unsigned)(((volatile dCapIcon_c *)sl)->mFlags
                                                       << 0x1c) >>
                                            0x1f) {
                                            sb = sl;
                                        } else {
                                            sl->mFlags |= 8;
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
                sb->mFlags |= 2;
                sb->mSlot = func_0202a8e0((int)sb->mOwner, sb->mCharacter);
                func_ov001_020aa6b0((int *)sb, 1);
                func_ov001_020aa6e4(r6, sb->mIconKind, (int *)sb);
            } else {
                if (r7 != 0 && (player->IsCollectingCap() != 0 || (*flagp & 0x40) != 0)) {
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
    if ((gCapIconRequestFlags[x] & 0xc0) == 0xc0)
        gCapIconRequestFlags[x] &= ~0x40;
    unsigned char v = gCapIconRequestFlags[x];
    if (v & 8) {
        gCapIconRequestFlags[x] = v & ~8;
        return 1;
    }
    if (gCapIconCounts[x] == 0)
        return 0;
    if (v & 0x40)
        return 0;
    if (x == ((Player *)arg)->mCharacter)
        return 0;
    if (func_ov001_020aa79c(x))
        return 0;
    if ((gCapIconRequestFlags[x] & 3) == 2)
        return 0;
    if (!((Player *)arg)->IsCollectingCap() && x == *(int *)(arg + 8) &&
        (gCapIconRequestFlags[x] & 3))
        return 0;
    return 1;
}

int func_ov001_020aa858(int self, char* a, int b, int d)
{
    if (b == 0 && self == data_02092114) {
        gCapIconRequestFlags[self] |= 0x40;
        gCapIconDispatchFlags[self] |= 0x80;
        return 0;
    }
    if (a == 0) return 0;
    unsigned char k = ((dCapIcon_c *)a)->mIconKind;
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
    if (((dCapIcon_c *)a)->mIconKind == 3)
        return 1;
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
    return (((unsigned char *)gCapIconDispatchFlags)[x] & 3) != 0;
}

void func_ov001_020aa6e4(int idx, unsigned r1, int* obj){
    gCapIconDispatchFlags[idx] |= 1;
    if ((gCapIconDispatchFlags[idx] & 0x80) && gCapIconDispatchLocked == 0) {
        if (obj == 0)
            return;
        if (r1 >= 3)
            return;
        if (func_02029600(obj[1]) == 0)
            return;
        data_0209f284 = 1;
        func_02012790(0x24);
        ((dCapIcon_c *)obj)->mFlashFrames = 0x3c;
        gCapIconFlashTimer = ((dCapIcon_c *)obj)->mFlashFrames;
        return;
    }
    gCapIconDispatchFlags[idx] |= 0x80;
}

void func_ov001_020aa6cc(int r0) {
    ((unsigned char *)gCapIconDispatchFlags)[r0] &= ~3;
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
        node = (CapNode *)gCapIconListHeads[i];
        handled = (int)best;
        flagByte = &gCapIconRequestFlags[i];
        reqFlag = gCapIconRequestFlags[i];

        if (reqFlag == 0) {
            if (node != 0) {
                do {
                    ((dCapIcon_c *)node)->mFlags &= ~2;
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

        node = *(CapNode *volatile *)&gCapIconListHeads[i];

        if (func_ov001_020aa79c(i) != 0) continue;
        if (*flagByte == 2) continue;
        if (gCapIconCounts[i] == 0)
            continue;

        if (node != 0) {
            do {
                found = dActor_c::FindWithID(node->field8);
                if (found != 0 || (int)found != node->field4) {
                    if (node->flags.b0) {
                        if (node->soundHandle == -1) {
                            node->soundHandle = func_0202a8e0(node->field4, node->field18);
                            ((dCapIcon_c *)node)->mFlags |= 2;
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
                            ((dCapIcon_c *)node)->mFlags |= 8;
                        }
                    }
                } else {
                    ((dCapIcon_c *)node)->Unlink();
                }
                node = node->next;
            } while (node != 0);
        }

        if (func_ov001_020aa7b8((void *)i, (char *)best) != 0) {
            ((dCapIcon_c *)best)->mFlags |= 2;
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

extern "C" {
u8 gCapIconFlashTimer;
u8 gCapIconDispatchLocked;
u8 gCapIconRequestFlags[3];
u8 gCapIconDispatchFlags[3];
u8 gCapIconCounts[4];
dCapIcon_c *gCapIconListHeads[3];
}
