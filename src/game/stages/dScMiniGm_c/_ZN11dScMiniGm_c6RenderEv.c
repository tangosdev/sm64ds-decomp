// @symbol _ZN11dScMiniGm_c6RenderEv
// NONMATCHING: missing logic (ROM does more) (div=30). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched. dScMiniGm_c::Render() -- vtable slot 9.
// See include/dScMiniGm_c.h.
struct Obj {
    char pad0[0x54];
    unsigned char mPageFlipped;
    char pad1[3];
    int mGroupBase;
    char pad2[0x8c - 0x5c];
    int mArrowBobPhase;
    int mPrevPageTimer;
    int mNextPageTimer;
    int mExitTimer;
    int mIconBlinkPhase;
};

extern unsigned char data_0209b300;
extern unsigned char data_0209b304;
extern int data_ov005_020c2250[];
extern int data_ov005_020c2c28[];
extern int data_ov005_020c2e88;
extern int data_ov005_020c2ea4[];
extern int data_ov005_020c2f60[];
extern int data_ov005_020c2f88;
extern int data_ov005_020c2fcc;

extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, int, int, int, int, int, int, int, int, int);
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int, int, int, int, int, int, int, int);
extern int _ZN3OAM9RenderSubEP7OamAttriiii(int, int, int, int, int);
extern int func_ov005_020c00b4(void *, int);
extern int func_ov005_020c007c(void *, int);
extern int func_ov005_020c00e4(void *);
extern void func_ov005_020c1130(void *);
extern void func_ov005_020c0f38(void *);

int _ZN11dScMiniGm_c6RenderEv(struct Obj *self)
{
    volatile int v[10];
    int sb, fp, r8, r7, r6, r5, r4;
    int t, t2, scale;

    if (data_0209b300 == 1) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2f88, 0x80, 0xA4, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2fcc, 0x80, 0xA4, -1, 0, 0x1000, 0x1000, 0, -1);
    }

    v[1] = 0x18;
    v[6] = 0x18;
    v[9] = 0x18;
    sb = 0;
    v[2] = 1;
    fp = 0x28;
    v[3] = 0;
    v[4] = 0;
    v[5] = 0;
    v[8] = 0;
    v[7] = -1;
    do {
        r7 = v[4];
        r8 = sb;
        r5 = r7;
        if (self->mIconBlinkPhase >= 0x20)
            v[0] = v[2];
        else
            v[0] = v[3];
        r4 = data_ov005_020c2250[sb];
        r6 = r4;
        do {
            if (func_ov005_020c00b4(self, r6) != 0 && func_ov005_020c007c(self, r5 + r4) != 0) {
                _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f60[v[0]], v[6], fp, v[7], v[5]);
                break;
            }
            r6 += 4;
            r5 += 4;
            r7 += 1;
        } while (r7 < 9);
        if (self->mGroupBase != r4)
            r8 += 4;
        _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2c28[r8], v[9], v[1], v[7], v[8]);
        sb += 1;
        fp += 0x30;
        v[1] += 0x30;
    } while (sb < 4);

    if (self->mPageFlipped == 0) {
        if (data_0209b304 != 0) {
            t = self->mPrevPageTimer;
            if (t < 0xA)
                scale = 0x1000;
            else if (t < 0xE)
                scale = (((t - 0xA) << 0xC) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 0xC) / 16) + 0x1000;
            t2 = self->mArrowBobPhase;
            if (t2 < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - t2 / 2, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - (0x40 - t2) / 2, 0x60, -1, 0, scale, 0);
        } else if (func_ov005_020c00e4(self) != 0) {
            t = self->mNextPageTimer;
            if (t < 0xA)
                scale = 0x1000;
            else if (t < 0xE)
                scale = (((t - 0xA) << 0xC) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 0xC) / 16) + 0x1000;
            t2 = self->mArrowBobPhase;
            if (t2 < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], t2 / 2 + 0xE0, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], (0x40 - t2) / 2 + 0xE0, 0x60, -1, 0, scale, 0);
        }
    }

    func_ov005_020c1130(self);
    func_ov005_020c0f38(self);

    t = self->mExitTimer;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 0xC) / 16) + 0x1000;
    else
        scale = (((0x1C - t) << 0xC) / 16) + 0x1000;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2e88, 0xE8, 0xA0, -1, 0, scale, 0);
    return 1;
}
