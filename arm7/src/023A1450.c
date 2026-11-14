/* storage 0x023A1450 runtime 0x027D33C8 linked 0x3D8 main ARM. */
#pragma opt_propagation off

extern int fn_0238D594(int a, int b);
extern int fn_0239AC04(void *p);
extern int fn_0239A96C(int a1, unsigned short a2);
extern void fn_0239A8F8(int a1, int a2);
extern void fn_023A2C70(unsigned short *p);
extern int fn_023A2948(unsigned short *p);
extern int fn_023A26E4(unsigned short *p);
extern void fn_023A2344(unsigned short *p, int z);
extern void fn_023A2F48(unsigned short *p);
extern int fn_023A2E30(unsigned short *p);
extern void fn_023A1D1C(unsigned short *p);
extern void fn_023A1C38(unsigned short *p);
extern void fn_023A1BAC(unsigned short *p);
extern void fn_023A2AD4(unsigned short *p);
extern void fn_023A278C(unsigned short *p);
/* local extern: caller passes the packet; the swapped body is void(void) */
extern void fn_023A1BA8(unsigned short *p);
extern void fn_02394DF8(void *list, int node);
extern void fn_0239477C(int a, int b);

struct PktBits {
    unsigned short low:2;
    unsigned short kind:2;
    unsigned short nib:4;
    unsigned short mid:4;
    unsigned short b12:1;
    unsigned short rest:3;
};

void fn_023A1450(void) {
    int *base;
    int *ctx;
    unsigned short *p7;
    unsigned short *entry;
    unsigned short *p5;
    unsigned int bits;
    unsigned int nib;
    unsigned short mode;

    base = *(int **)0x0380FFF4;
    ctx = (int *)((char *)base + 0x4F4);
    entry = *(unsigned short **)((char *)base + 0x1A0);
    if (entry == (unsigned short *)0xFFFFFFFF)
        return;
    p7 = (unsigned short *)((char *)entry + 0x10);
    p5 = (unsigned short *)((char *)p7 + 0x14);
    if (p7[12] & 1)
        ++*(int *)((char *)ctx + 0x2C);
    else
        ++*(int *)((char *)ctx + 0x28);
    {
        int *ctr = (int *)((char *)ctx + 0x24);
        *ctr += fn_0238D594(p7[4] & 0xF0, 16) - 1;
    }
    int idx = fn_0239AC04((char *)p5 + 0xA);
    p7[1] = (unsigned short)idx;
    if (idx == 0xFF) {
        p7[1] = 0;
        if (*(unsigned short *)((char *)*(int **)0x0380FFF4 + 0x314) == 1) {
            {
                struct PktBits *pk = (struct PktBits *)p5;
            if (pk->kind == 0) {
                nib = pk->nib;
                if (nib == 0)
                    goto nib0;
                if (nib == 4)
                    goto nib4;
                if (nib != 11)
                    goto nibend;
                fn_023A1D1C(p7);
                goto nibend;
            nib4:
                fn_023A26E4(p7);
                goto nibend;
            nib0:
                fn_023A2C70(p7);
            nibend:;
            }
            }
        }
    } else {
        fn_0239A96C((unsigned short)idx, (unsigned short)(p7[9] & 0xFF));
        if (((struct PktBits *)p5)->kind == 0 &&
            p7[21] == *(unsigned short *)(*(int *)((char *)*(int **)0x0380FFF4 + 0x2E0) +
                                          idx * 0x1C + 0x14)) {
            ++*(int *)((char *)ctx + 0x3C);
        } else {
            base = *(int **)0x0380FFF4;
            mode = *(unsigned short *)((char *)base + 0x314);
            if (mode == 1)
                goto mode1;
            if (mode == 2)
                goto mode23;
            if (mode != 3)
                goto modeend;
            goto mode23;
        mode1:
                fn_0239A8F8((int)((unsigned int)(idx << 16) >> 16),
                            ((struct PktBits *)p5)->b12);
                {
                    struct PktBits *pk = (struct PktBits *)p5;
                if (pk->kind == 0) {
                    *(unsigned short *)(*(int *)((char *)*(int **)0x0380FFF4 + 0x2E0) +
                                        idx * 0x1C + 0x14) = p5[11];
                    switch (pk->nib) {
                    case 8:
                        fn_023A2F48(p7);
                        break;
                    case 0:
                        fn_023A2C70(p7);
                        break;
                    case 2:
                        fn_023A2948(p7);
                        break;
                    case 4:
                        fn_023A26E4(p7);
                        break;
                    case 5:
                        fn_023A2344(p7, 0);
                        break;
                    case 10:
                        fn_023A2E30(p7);
                        break;
                    case 11:
                        fn_023A1D1C(p7);
                        break;
                    case 12:
                        fn_023A1C38(p7);
                        break;
                    default:
                        break;
                    }
                } else if (pk->kind == 1 && pk->nib == 0xA) {
                    fn_023A1BAC(p7);
                }
                }
                goto modeend;
        mode23:
                *(unsigned short *)(*(int *)((char *)base + 0x2E0) + idx * 0x1C + 0x14) = p5[11];
                {
                    struct PktBits *pk = (struct PktBits *)p5;
                if (pk->kind == 0) {
                    switch (pk->nib) {
                    case 8:
                        fn_023A2F48(p7);
                        break;
                    case 1:
                        fn_023A2AD4(p7);
                        break;
                    case 3:
                        fn_023A278C(p7);
                        break;
                    case 5:
                        fn_023A2344(p7, 0);
                        break;
                    case 10:
                        fn_023A2E30(p7);
                        break;
                    case 11:
                        fn_023A1D1C(p7);
                        break;
                    case 12:
                        fn_023A1C38(p7);
                        break;
                    default:
                        break;
                    }
                } else if (pk->kind == 1) {
                    nib = pk->nib;
                    if (nib == 0xE)
                        goto call_ba8;
                    if (nib != 0xF)
                        goto skip_ba8;
                call_ba8:
                    fn_023A1BA8(p7);
                skip_ba8:;
                }
                }
        modeend:;
            }
        }
    fn_02394DF8((char *)*(int **)0x0380FFF4 + 0x1A0, (int)entry);
    if (*(unsigned short *)((char *)*(int **)0x0380FFF4 + 0x1A8) != 0)
        fn_0239477C(1, 7);
}
