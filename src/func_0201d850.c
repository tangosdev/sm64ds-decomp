#pragma opt_strength_reduction off
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

extern u8 data_0209d660;
extern u8 data_0209d6a8;
extern u8 data_0209d65c;
extern u8 data_0209d678;
extern u8 data_0209d6a4;
extern u8 data_0209d6b0;
extern u8 data_0209d6b8;
extern u8 data_0209d6c0;
extern s32 data_0209d704;
extern s32 data_0209d6f4;
extern s32 data_0209d6fc;
extern void *data_0209d6f0;
extern void *data_0209d708;
extern void *data_0209d70c;
extern s32 data_0209d74c[];
extern u8 data_0209f20c;
extern u8 data_0209f220;
extern u8 data_0209f228;
extern u8 data_0208ee74[];
extern u8 data_0208f074[];

extern void func_0201eaac(void);
extern int func_02054d88(void);
extern void MultiStore_Int(int val, void *dst, int len);
extern void *_ZN2G212GetBG3ScrPtrEv(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void SetBg3Offset(int a, int b);
extern void func_0201b7cc(void);
extern void _ZN7Message7AddCharEc(int c);
extern void func_0201b388(int a);
extern int IsStarCollected(u32 a, u8 b);

void func_0201d850(u32 arg0)
{
    u16 var_r5;
    u16 var_r4;
    volatile u16 sp0;
    volatile s32 sp4;
    volatile s32 sp8;

    data_0209d660 = 0;
    var_r5 = 0x200;
    var_r4 = 0x80;
    func_0201eaac();
    data_0209d6a8 = 0;
    data_0209d65c = 0;

    {
        void *pa = (char *)func_02054d88() + 0x4000;
        sp4 = 0;
        MultiStore_Int(sp4, pa, 0x2000);
    }
    {
        void *pb = _ZN2G212GetBG3ScrPtrEv();
        sp0 = 0x2ff;
        MultiStore16(sp0, pb, 0x800);
    }
    SetBg3Offset(0, 0);

    if (arg0 < 0xf) {
        char *ep = (char *)(((int)data_0209d708 + 0x1468));
        s32 base;
        s32 num;
        s32 n;
        s32 i;
        u16 *p;

        data_0209d6f0 = ep;
        base = data_0209d6fc + 0x28 + *(s32 *)((char *)data_0209d70c + 4);
        data_0209d6f4 = base + *(s32 *)ep;
        func_0201b7cc();
        _ZN7Message7AddCharEc(0x4d);

        num = (arg0 + 1) & 0xff;
        if (num / 10 != 0) {
            _ZN7Message7AddCharEc(data_0208ee74[num / 10]);
        }
        _ZN7Message7AddCharEc(data_0208ee74[num % 10]);

        data_0209d6a8 = (data_0209d6b0 + 7) / 8;
        func_0201b388(data_0209d6a8);
        data_0209d6c0 = 1;
        n = data_0209d6a8;
        p = (u16 *)_ZN2G212GetBG3ScrPtrEv() + (var_r4 + ((0x20 - n) >> 1));
        for (i = 0; i < n; i++) {
            p[i] = (u16)(var_r5 + i);
            *(u16 *)((char *)(p + i) + 0x40) = (u16)(n + (var_r5 + i));
        }
        var_r4 += 0x40;
        var_r5 += (u16)(n * 2);
    }

    if (((data_0209f20c != 0) && (arg0 < 0x15)) || (data_0209f20c == 0)) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        if (data_0209f20c != 0) {
            if (arg0 >= 0xf) {
                var_r4 = 0x100;
            } else {
                goto L_r4_else;
            }
        } else {
L_r4_else:
            if (arg0 >= 0xf) var_r4 = 0x140;
        }

        data_0209d65c = 0;
        data_0209d6a4 = 0;
        data_0209d6f0 = (char *)data_0209d708 +
            (arg0 + 0x196) * 8;
        data_0209d6f4 = data_0209d6fc + 0x28 +
            *(s32 *)((char *)data_0209d70c + 4) +
            *(s32 *)((char *)data_0209d708 + (arg0 + 0x196) * 8);
        data_0209d6f4 += 3;
        func_0201b7cc();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)_ZN2G212GetBG3ScrPtrEv() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                *(u16 *)((char *)(p + i) + 0x40) = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        var_r5 += (u16)(n * 2);
        var_r4 += 0x40;
    }

    if ((arg0 < 0xf) || ((data_0209f20c != 0) && ((arg0 < 0xf) || (arg0 >= 0x15)))) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        sp8 = 0;
        data_0209d65c = 0;
        data_0209d678 = 0;
        data_0209d704 = 0;
        data_0209d6a4 = 0;
        data_0209d6b0 = 0;
        MultiStore_Int(sp8, data_0209d74c, 0xf00);
        data_0209d6b8 = 1;
        if ((IsStarCollected(arg0, data_0209f220) != 0) || (data_0209f20c != 0)) {
            _ZN7Message7AddCharEc(0xf0);
        } else {
            _ZN7Message7AddCharEc(0xf1);
        }
        data_0209d65c += data_0208f074[0xf0];
        _ZN7Message7AddCharEc(0x4d);
        data_0209d65c += data_0208f074[0x4d];
        if (arg0 < 0xf) {
            if (data_0209f20c != 0) {
                s32 off = arg0 * 7 + 0x1b3;
                data_0209d6f0 = (char *)data_0209d708 + (off + data_0209f228) * 8;
            } else {
                s32 off = arg0 * 7 + 0x1b3;
                data_0209d6f0 = (char *)data_0209d708 + (off + data_0209f220) * 8;
            }
            var_r4 = 0x140;
        } else {
            data_0209d6f0 = (char *)data_0209d708 + 0x10e8;
            var_r4 = 0x100;
        }
        {
            s32 base = data_0209d6fc + 0x28 + *(s32 *)((char *)data_0209d70c + 4);
            data_0209d6f4 = base + *(s32 *)data_0209d6f0;
        }
        func_0201b7cc();
        data_0209d65c = 0;

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)_ZN2G212GetBG3ScrPtrEv() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                *(u16 *)((char *)(p + i) + 0x40) = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        var_r5 += (u16)(n * 2);
        var_r4 += 0x40;
    }

    if (data_0209f20c != 0) {
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        if ((arg0 < 0xf) || (arg0 >= 0x15)) {
            data_0209d6f0 = (char *)data_0209d708 + 0x1460;
        } else {
            data_0209d6f0 = (char *)data_0209d708 + 0x1598;
        }
        data_0209d6b8 = 0;
        {
            s32 base = data_0209d6fc + 0x28 + *(s32 *)((char *)data_0209d70c + 4);
            data_0209d6f4 = base + *(s32 *)data_0209d6f0;
        }
        func_0201b7cc();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)_ZN2G212GetBG3ScrPtrEv() + (var_r4 + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                *(u16 *)((char *)(p + i) + 0x40) = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
        data_0209d6a8 += n;
        data_0209d6c0 = 1;
        return;
    }

    if (arg0 >= 0xf) {
        return;
    }

    data_0209d6b8 = 0;
    {
        char *ep = (char *)(((int)data_0209d708 + 0x1470));
        s32 base;
        s32 n;
        s32 tile;
        u16 *p;
        s32 i;

        data_0209d6f0 = ep;
        base = data_0209d6fc + 0x28 + *(s32 *)((char *)data_0209d70c + 4);
        data_0209d6f4 = base + *(s32 *)ep;
        func_0201b7cc();

        n = (u8)((data_0209d6b0 + 7) / 8);
        func_0201b388(n);
        p = (u16 *)_ZN2G212GetBG3ScrPtrEv() +
            ((u16)(var_r4 + 0x40) + ((0x20 - n) >> 1));
        i = 0;
        if (i < n) {
            tile = var_r5;
            do {
                p[i] = (u16)tile;
                *(u16 *)((char *)(p + i) + 0x40) = (u16)(n + tile);
                tile++;
                i++;
            } while (i < n);
        }
    }
}
