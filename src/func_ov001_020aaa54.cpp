//cpp
#include "types.h"
#include "CapIcon.h"
extern "C" {
int _ZN6Player15IsCollectingCapEv(void* p);
int func_ov001_020aa960(int x, void* arg);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
int func_0202a8e0(void* a, u8 b);
void func_ov001_020aa6b0(void* p, int r1);
int func_ov001_020aa858(int self, void* a, int b, int d, int e);
void func_ov001_020aa6e4(int idx, u32 pri, void* obj);
int SublevelToLevel(int i);

extern void* data_0209f394;
extern u8 data_ov001_020ad628[];
extern dCapIcon_c* data_ov001_020ad634[];
extern signed char data_0209f2f8;

/* Route the +0x1b flag-byte address through 64-bit arithmetic so it is
 * materialized (add rX,base,#0x1b) for the read-modify-write, matching the ROM. */
#define FLAGS1B(p) (*(u8*)(((int)(p) + 0x1b)))

void func_ov001_020aaa54(void)
{
    unsigned sp4;
    int typ;
    int spc;
    void* player = data_0209f394;
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

    if (_ZN6Player15IsCollectingCapEv(player) != 0) {
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

        if (func_ov001_020aa960(r6, player) != 0) {
            sl = data_ov001_020ad634[r6];
            while (sl != 0) {
                void* found = _ZN8dActor_c10FindWithIDEj(sl->mOwnerUniqueID);
                if (found == 0 && found == sl->mOwner) goto do_ab110;
                {
                    int b1b = sl->mFlags;
                    int bit0 = (unsigned)(b1b << 0x1f) >> 0x1f;
                    if (bit0 == 0) {
                        if ((unsigned)(b1b << 0x1e) >> 0x1f) {
                            fp = 1;
                            r8 = sl->unk_19;
                            goto cont;
                        }
                    }
                    {
                        int t = *flagp;
                        if (!(t & 0x30)) {
                            if (sl->unk_19 < 2) goto cont;
                        }
                        if (bit0) {
                            if (sl->mSlot == -1) {
                                sl->mSlot = func_0202a8e0(sl->mOwner, sl->mCharacter);
                                FLAGS1B(sl) |= 2;
                                func_ov001_020aa6b0(sl, 1);
                            } else {
                                spc = 1;
                            }
                            r7 = 1;
                            sp4 = sl->unk_19;
                            if ((*flagp & 3) == 0) *flagp |= 1;
                            if (sl->unk_19 >= 2) *flagp |= 0x20;
                        } else {
                            if (sl->unk_19 != 0) {
                                if (r8 < sl->unk_19) {
                                    r8 = sl->unk_19;
                                    sb = sl;
                                }
                                if ((t & 3) == 0) *flagp |= 1;
                                if (sl->unk_19 >= 2) *flagp |= 0x20;
                            } else {
                                if ((t & 3) == 1 && r8 == 0) {
                                    if ((unsigned)(((volatile u8*)sl)[0x1b] << 0x1c) >> 0x1f) {
                                        sb = sl;
                                    } else {
                                        FLAGS1B(sl) |= 8;
                                    }
                                }
                            }
                        }
                    }
                }
                goto cont;
            do_ab110:
                sl->Unlink();
            cont:
                sl = sl->mNext;
            }

            if (func_ov001_020aa858(r6, sb, typ, fp, r7) != 0) {
                FLAGS1B(sb) |= 2;
                sb->mSlot = func_0202a8e0(sb->mOwner, sb->mCharacter);
                func_ov001_020aa6b0(sb, 1);
                func_ov001_020aa6e4(r6, sb->unk_19, sb);
            } else {
                if (r7 != 0 && (_ZN6Player15IsCollectingCapEv(player) != 0 || (*flagp & 0x40) != 0)) {
                    if (spc != 0) {
                        func_ov001_020aa6e4(r6, three, z1);
                    } else {
                        func_ov001_020aa6e4(r6, sp4, z2);
                    }
                } else {
                    if (fp != 0) {
                        func_ov001_020aa6e4(r6, three, z3);
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

}
