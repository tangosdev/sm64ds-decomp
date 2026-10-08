#include "types.h"
extern u8 data_020a0f04;
extern u8 data_020a0f08;
extern u8 data_020a0f0c;
extern volatile u16 data_020a0f1c;
extern u16 data_020a0f20;
extern u16 data_020a0f28;
extern void *data_020a0f3c;
extern s32 data_020a0f40;
extern s32 data_020a0f50;
extern s32 data_020a0f6c;
extern s32 data_020a0f70;
extern s32 data_020a0f78;
extern void *data_020a0f7c;
extern u8 data_020a0f9d;
extern u8 data_020a0fbe[24];
extern u8 data_020a0fec[];
extern u8 data_020a1040[];
extern u8 data_020a1064[];
extern u8 data_020a10fc[];
extern u8 data_020a1112[];
extern u8 data_020a1154[];
extern u16 data_020a117c[4];
extern u16 data_020a11a0[4];
extern u16 data_020a11c4[4];
extern s32 data_020a1680;
extern u8 data_020a1780[];

extern int func_02067194(void);
extern void *_ZN6Memory8AllocateEji(unsigned int size, int align);
extern void _ZN6Memory10DeallocateEPv(void *ptr);
extern void func_0203f9d4(int a0, int a1);
extern void func_0203e978(void);
extern int func_02059640(void);
extern void func_02068e4c(void *a, void *b, u16 c, u16 d, u16 e, int f);
extern void func_02068dc8(int a, int b, int c);
extern void func_020672a4(u32 val);
extern void func_0203e9a8(u32 a0, int sel, int a2);
extern int func_02068ae8(int param0);
extern int func_02067e70(void *obj);
extern int func_02067bfc(int a, void *b, int c);
extern int func_02067928(void *a, void *b);
extern void func_0205a588(void *dst, int val, unsigned int size);
extern int func_02061c20(int *arg);
extern void *func_020671b4(u16 idx);
extern int func_02067250(u16 a);
extern int func_0206703c(u16 arg0, int sel);
extern void CpuCopy8(const void *src, void *dst, unsigned int size);
extern int func_02067138(u16 idx);
extern void func_02068a54(void);

#define H(base, off) (*(u16 *)((char *)(base) + (off)))

#pragma opt_dead_assignments off

void func_0203e20c(void)
{
    if (data_020a0f40 != 0 && data_020a0f50 == 3) {
        data_020a0f50 = 6;
    }
    if (data_020a0f50 > 3 && data_020a0f50 < 6 &&
        (data_020a0f08 != (u8)(func_02067194() + 1) || (u32)func_02067194() < 1)) {
        data_020a0f40 = 1;
        data_020a0f50 = 6;
        data_020a0f1c |= 4;
    }

    switch (data_020a0f50) {
    case 0: {
        void *ptr = _ZN6Memory8AllocateEji(0xB000, 0x20);
        data_020a0f3c = ptr;
        data_020a0f0c = 0;
        func_0203f9d4((int)ptr, (int)func_0203e978);
        data_020a0f50 += 1;
        data_020a0f1c &= 0xFFFB;
        break;
    }

    case 1:
        if (data_020a0f0c != 0) {
            data_020a0f50 += 1;
        }
        break;

    case 2: {
        void *p;
        int size;
        data_020a0f20 = (u16)func_02059640();
        func_02068e4c(data_020a0f3c, data_020a10fc, H(data_020a1064, 6), H(data_020a1064, 8), data_020a0f20, 1);
        func_02068dc8(0x1fe, 8, 3);
        func_020672a4((u32)func_0203e9a8);
        func_02068ae8(data_020a0f28);
        size = func_02067e70(0);
        p = _ZN6Memory8AllocateEji(size, 0x20);
        data_020a0f7c = p;
        if (func_02067bfc(0, p, size) != 0) {
            func_02067928(data_020a0fec, data_020a0f7c);
        }
        data_020a0f70 = 0;
        data_020a0f50 += 1;
        break;
    }

    case 3: {
        int cnt2;
        int i2;
        u8 *p9;
        u8 *p11;
        void *found;
        u8 *pIdx;
        u8 *base1112;
        int idx;
        int sel1;
        int oneVal;

        cnt2 = 1;
        i2 = cnt2;
        p9 = &data_020a0f9d;
        p11 = data_020a1112;

        data_020a0f1c |= 0x9000;
        data_020a0f08 = cnt2;
        do {
            *p9 = 0;
            func_0205a588(p11, 0, 0x16);
            i2 += 1;
            p9 += 1;
            p11 += 0x16;
        } while (i2 < 4);

        func_02061c20(&data_020a1680);

        pIdx = &data_020a0f9d;
        base1112 = data_020a1112;
        idx = 1;
        sel1 = idx;
        oneVal = idx;
        do {
            found = func_020671b4(idx);
            if (found != 0 && ((H(data_020a1780, 0x7e) >> idx) & 1)) {
                if (idx < 4) {
                    if (func_02067250(idx) == 0xA) {
                        func_0206703c(idx, sel1);
                    } else if (func_02067250(idx) == 0xE) {
                        cnt2 += 1;
                    }
                    *pIdx = oneVal;
                    CpuCopy8(found, base1112, 0x16);
                    data_020a0f08 += 1;
                } else if (func_02067250(idx) == 0xA) {
                    func_0206703c(idx, 0);
                }
            }
            idx += 1;
            pIdx += 1;
            base1112 += 0x16;
        } while (idx < 0x10);

        if (data_020a0f70 != 0 && data_020a0f08 == cnt2) {
            if ((u32)data_020a0f08 > 1U) {
                u8 j;
                u8 deadcnt;
                int sel2;
                deadcnt = 0;
                j = 1;
                sel2 = 2;
                do {
                    if (func_0206703c(j, sel2) == 1) {
                        deadcnt = (u8)(deadcnt + 1);
                    }
                    j = (u8)(j + 1);
                } while ((u32)j <= 0xF);
            }
            data_020a0f50 += 1;
        }
        break;
    }

    case 4: {
        int ok = 1;
        u8 *p = &data_020a0f9d;
        int k = 1;
        do {
            if (*p != 0 && func_02067138(k) != 1) {
                ok = 0;
            }
            k += 1;
            p += 1;
        } while (k < 4);
        if (ok != 0) {
            data_020a0f50 += 1;
            data_020a0f6c = 0;
            data_020a0f78 = 1;
        }
        break;
    }

    case 5:
        if ((u32)data_020a0f08 > 1U) {
            int result;
            do {
                u8 m;
                u8 cnt;
                cnt = 0;
                m = 1;
                do {
                    if (func_0206703c(m, 3) == 1) {
                        cnt = (u8)(cnt + 1);
                    }
                    m = (u8)(m + 1);
                } while ((u32)m <= 0xF);
                result = (cnt != 0) ? 1 : 0;
            } while (result == 0);
        }
        data_020a0f50 += 1;
        break;

    case 6:
        data_020a0f70 = 0;
        data_020a0f1c &= 0x6FFF;
        func_02068a54();
        data_020a0f50 += 1;
        break;

    case 7:
        if (data_020a0f70 != 0) {
            data_020a0f50 += 1;
        }
        break;

    default:
        data_020a0f50 = 0;
        data_020a0f04 = 0;
        _ZN6Memory10DeallocateEPv(data_020a0f3c);
        _ZN6Memory10DeallocateEPv(data_020a0f7c);
        if (data_020a0f40 != 0) {
            u8 *p9d = &data_020a0f9d;
            u8 *p1112 = data_020a1112;
            u8 *pbe = data_020a0fbe;
            int sl;
            data_020a0f40 = 0;
            data_020a0f08 = 0;
            sl = 1;
            do {
                *p9d = 0;
                func_0205a588(p1112, 0, 0x16);
                func_0205a588(pbe, 0, 6);
                sl += 1;
                p9d += 1;
                p1112 += 0x16;
                pbe += 6;
            } while (sl < 4);
        }
        break;
    }

    {
        u8 *p1040 = data_020a1040;
        u8 *p1154 = data_020a1154;
        int i;
        int sh;
        u16 t78, t7a, t54, t56, t30, t32, h4, h6, h8, ha;
        data_020a0f1c &= 0xF0FF;
        sh = (data_020a0f28 << 8) & 0xF00;
        H(p1154, 0x78) = H(p1040, 0xc);
        t78 = H(p1154, 0x78);
        H(p1154, 0x7a) = H(p1040, 0xe);
        t7a = H(p1154, 0x7a);
        H(p1154, 0x54) = t78;
        H(p1154, 0x56) = t7a;
        t54 = H(p1154, 0x54);
        t56 = H(p1154, 0x56);
        H(p1154, 0x30) = t54;
        H(p1154, 0x32) = t56;
        t30 = H(p1154, 0x30);
        t32 = H(p1154, 0x32);
        H(p1154, 0xc) = t30;
        data_020a0f1c |= sh;
        H(p1154, 0xe) = t32;
        h4 = H(p1040, 4);
        h6 = H(p1040, 6);
        h8 = H(p1040, 8);
        ha = H(p1040, 0xa);
        H(p1154, 0x70) = h4;
        H(p1154, 0x72) = h6;
        H(p1154, 0x74) = h8;
        H(p1154, 0x76) = ha;

        {
            u16 *p11c4 = (u16 *)data_020a11c4;
            H(p1154, 0x4c) = p11c4[0];
            H(p1154, 0x4e) = p11c4[1];
            H(p1154, 0x50) = p11c4[2];
            H(p1154, 0x52) = p11c4[3];
        }
        {
            u16 *p11a0 = (u16 *)data_020a11a0;
            H(p1154, 0x28) = p11a0[0];
            H(p1154, 0x2a) = p11a0[1];
            H(p1154, 0x2c) = p11a0[2];
            H(p1154, 0x2e) = p11a0[3];
        }
        {
            u16 *p117c = (u16 *)data_020a117c;
            H(p1154, 4) = p117c[0];
            H(p1154, 6) = p117c[1];
            H(p1154, 8) = p117c[2];
            {
                u16 last = p117c[3];
                u16 h10 = H(p1040, 0x10);
                H(p1154, 0x7c) = h10;
                h10 = H(p1154, 0x7c);
                H(p1154, 0xa) = last;
                H(p1154, 0x58) = h10;
                h10 = H(p1154, 0x58);
                H(p1154, 0x34) = h10;
                h10 = H(p1154, 0x34);
                H(p1154, 0x10) = h10;
            }
        }

        i = 0;
        do {
            p1154[0x7e] = p1040[0x12];
            i += 1;
            p1040 += 1;
            p1154[0x5a] = p1154[0x7e];
            p1154[0x36] = p1154[0x5a];
            p1154[0x12] = p1154[0x36];
            p1154 += 1;
        } while (i < 0x11);
    }
}
