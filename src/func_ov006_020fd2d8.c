/* NONMATCHING div=84: all diffs are ONE 3-cycle callee-saved rename
 * (ROM o=r7,i=r6,idx=r8 vs cand o=r8,i=r7,idx=r6; ranks follow first-def
 * order, idx-highest unreachable: named idx fixed vel->r5, but register kw
 * and param-copy decls break size / coalesce away - rotation wall family)
 * + one assoc pair (0x4688 store base) + 1 pool word.
 * func_ov006_020fd2d8 @ 0x020fd2d8 size 0x5bc (ov006) — per-slot drift/bob update,
 * sibling of matched func_ov006_020fd894 (same 0x38-stride cluster + helpers). */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

#define ATI(p, off) ((char *)(int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))
#define ATU(p, off) ((char *)(unsigned int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void func_ov006_020fcd8c(char *o, int i);
extern void func_ov006_020fdaf0(char *o, int i);
extern void func_ov006_020fca1c(char *o, int i);
extern void func_ov006_020fc9b0(char *o, int i);
extern void func_ov006_020fcb4c(char *o, int i);

void func_ov006_020fd2d8(char *o, int i)
{
    int idx;
    func_ov006_020fcd8c(o, i);
    idx = i * 0x38;
    if (*(unsigned char *)(o + 0x4690 + idx) == 0)
    {
        *(unsigned char *)(o + 0x4690 + idx) += 1;
        if (*(unsigned short *)(o + 0x5c28) > 0xc)
        {
            *(int *)(ATI(o, idx) + 0x466c) =
                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0xb80
                + ((*(unsigned short *)(o + 0x5c28) - 0xc) << 7);
        }
        else
        {
            *(int *)(ATU(o, idx) + 0x466c) =
                *(unsigned short *)(o + 0x5c28) * 0xa0
                + (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 3) >> 0xf) << 7) + 0x400);
        }
        if ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) != 0)
        {
            *(int *)(o + idx + 0x4668) = (*(unsigned short *)(o + 0x5c28) << 7) + 0x600;
            *(unsigned char *)(o + idx + 0x4691) = 0;
        }
        else
        {
            *(int *)(o + idx + 0x4668) = -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600);
            *(unsigned char *)(o + idx + 0x4691) = 0;
        }
        *(unsigned short *)(ATI(o, idx + 0x4600) + 0x88) =
            (((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3)) + 0x20;
    }
    else
    {
        int v12;
        int *vel;
        *(int *)(o + 0x4664 + idx) += *(int *)(o + idx + 0x466c);
        vel = (int *)(o + 0x4668 + idx);
        *(int *)(o + 0x4660 + idx) += *(int *)(o + 0x4668 + idx);
        v12 = *(int *)(o + 0x4660 + idx) >> 12;
        if (v12 > 0xf0)
        {
            u8 d;
            *vel = -*vel;
            d = *(unsigned char *)(o + 0x4691 + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + 0x4691 + idx) = 2;
                else
                    *(unsigned char *)(o + 0x4691 + idx) = 1;
            }
            *(int *)(o + 0x4660 + idx) = 0xf0000;
        }
        if (v12 < 0x10)
        {
            u8 d;
            *(int *)(o + 0x4668 + idx) = -*(int *)(o + 0x4668 + idx);
            d = *(unsigned char *)(o + 0x4691 + idx);
            if (d != 0)
            {
                if (d == 1)
                    *(unsigned char *)(o + 0x4691 + idx) = 2;
                else
                    *(unsigned char *)(o + 0x4691 + idx) = 1;
            }
            *(int *)(o + 0x4660 + idx) = 0x10000;
        }
        func_ov006_020fdaf0(o, i);
        func_ov006_020fca1c(o, i);
        func_ov006_020fc9b0(o, i);
        {
            unsigned short t = *(unsigned short *)(o + 0x4688 + idx);
            if (t != 0)
            {
                *(unsigned short *)(o + 0x4688 + idx) = t - 1;
                if (*vel > 0)
                {
                    *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                    if (*(short *)(o + 0x4684 + idx) <= -0x1800)
                        *(unsigned short *)(o + 0x4684 + idx) = 0xe800;
                }
                else if (*vel < 0)
                {
                    *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                    if (*(short *)(o + 0x4684 + idx) >= 0x1800)
                        *(short *)(o + 0x4684 + idx) = 0x1800;
                }
                if (*(short *)(o + 0x4688 + idx) < 0)
                    *(short *)(o + 0x4688 + idx) = 0;
                return;
            }
            {
                u8 st = *(unsigned char *)(o + 0x4691 + idx);
                if (st == 0)
                {
                    int v = *vel;
                    if (v > 0)
                    {
                        *vel = v - 0x20;
                        *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                        if (*(short *)(o + 0x4684 + idx) >= 0)
                            *(short *)(o + 0x4684 + idx) = 0;
                        if (*vel <= 0)
                        {
                            *(unsigned char *)(o + 0x4691 + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) + 1;
                            *(unsigned short *)(o + 0x4688 + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) << 2);
                            *(short *)(o + 0x4684 + idx) = 0;
                        }
                    }
                    else if (v < 0)
                    {
                        *vel = v + 0x20;
                        *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                        if (*(short *)(o + 0x4684 + idx) < 0)
                            *(short *)(o + 0x4684 + idx) = 0;
                        if (*vel >= 0)
                        {
                            *(unsigned char *)(o + 0x4691 + idx) =
                                (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1) >> 0xf) + 1;
                            *(unsigned short *)(o + 0x4688 + idx) =
                                ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) << 2);
                            *(short *)(o + 0x4684 + idx) = 0;
                        }
                    }
                }
                else if (st == 1)
                {
                    *vel += 0x20;
                    *(unsigned short *)(o + 0x4684 + idx) -= 0x80;
                    if (*(short *)(o + 0x4684 + idx) <= -0x1800)
                        *(unsigned short *)(o + 0x4684 + idx) = 0xe800;
                    if (*vel >= (*(unsigned short *)(o + 0x5c28) << 7) + 0x600)
                    {
                        *(unsigned char *)(o + 0x4691 + idx) = 0;
                        *vel = (*(unsigned short *)(o + 0x5c28) << 7) + 0x600;
                        *(unsigned short *)(o + 0x4688 + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3) + 0x10;
                    }
                }
                else
                {
                    *vel -= 0x20;
                    *(unsigned short *)(o + 0x4684 + idx) += 0x80;
                    if (*(short *)(o + 0x4684 + idx) >= 0x1800)
                        *(short *)(o + 0x4684 + idx) = 0x1800;
                    if (*vel <= -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600))
                    {
                        *(unsigned char *)(o + 0x4691 + idx) = 0;
                        *vel = -((*(unsigned short *)(o + 0x5c28) << 7) + 0x600);
                        *(unsigned short *)(o + 0x4688 + idx) =
                            ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 3) + 0x10;
                    }
                }
            }
        }
    }
    func_ov006_020fcb4c(o, i);
}
