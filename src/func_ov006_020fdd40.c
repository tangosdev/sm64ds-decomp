// NONMATCHING: size 0x430 vs target 0x468 (div=size). Logic complete and verified
// vs the ROM: all 7 RandomIntInternal call sites, and every strh/strb/conditional
// store match; the difference is register/stack allocation only (the ROM
// precomputes and spills 5 field pointers across the RNG calls to keep them live,
// mwcc keeps fewer live and rematerialises). Verified instruction-by-instruction.
// dScMgPachinko_c bob-omb spawner: countdown timer, find a free slot (of 30),
// seed position/type/launch direction from RNG + data_ov006_0212eb80 /
// data_ov006_0213d974 / the sin-cos table, then advance the wave counters.
typedef unsigned char u8;
typedef unsigned short u16;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212eb80[];
extern u8 data_ov006_0213d974[];
extern const short data_02082214[];

void func_ov006_020fdd40(char *o)
{
    int i;

    if (*(u16 *)(o + 0x5c24) != 0)
    {
        *(u16 *)(o + 0x5c24) -= 1;
        if (*(short *)(o + 0x5c24) < 0)
            *(u16 *)(o + 0x5c24) = 0;
        return;
    }

    *(u16 *)(o + 0x5c24) = 0x50;

    for (i = 0; i < 0x1e; i++)
    {
        int idx = i * 0x38;
        if (*(u8 *)(o + 0x468c + idx) == 0)
        {
            int ra, rb, rc;

            *(u8 *)(o + 0x468c + idx) = 1;
            ra = RandomIntInternal(&data_0209d4b8);
            rb = RandomIntInternal(&data_0209d4b8);
            *(int *)(o + 0x4660 + idx) =
                (data_ov006_0212eb80[(((rb >> 16) & 0x7fff) * 5) >> 15]
                 + (((((ra >> 16) & 0x7fff) << 2) >> 15) << 3)) << 12;
            *(int *)(o + 0x4664 + idx) = -0x100000;
            *(int *)(o + 0x4668 + idx) = 0;
            *(int *)(o + 0x466c + idx) = 0;
            *(u8 *)(o + 0x468f + idx) = 0;
            *(u8 *)(o + 0x4690 + idx) = 0;

            rc = RandomIntInternal(&data_0209d4b8);
            *(u16 *)(o + 0x4688 + idx) = (u16)((((((rc >> 16) & 0x7fff) << 3) >> 15) << 4));
            *(u16 *)(o + 0x4684 + idx) = 0;
            *(int *)(o + 0x467c + idx) = 0;
            *(u8 *)(o + 0x4696 + idx) = (u8)*(u16 *)(o + 0x5c28);
            *(u16 *)(o + 0x5c26) += 1;

            if (*(u16 *)(o + 0x5c28) != 0)
            {
                int rd = RandomIntInternal(&data_0209d4b8);
                int cc = *(u16 *)(o + 0x5c28) >> 1;
                if (cc >= 4)
                    cc = 4;
                if (data_ov006_0213d974[(((((rd >> 16) & 0x7fff) << 4) >> 15)) + (cc << 2)] != 0)
                {
                    int re = RandomIntInternal(&data_0209d4b8);
                    if (((((re >> 16) & 0x7fff) << 1) >> 15) != 0)
                    {
                        int rf;
                        *(int *)(o + 0x4660 + idx) = 0x100000;
                        rf = RandomIntInternal(&data_0209d4b8);
                        *(u16 *)(o + 0x4686 + idx) =
                            (u16)(0xc000 - ((((((rf >> 16) & 0x7fff) * 0xc) >> 15) + 1) << 9));
                    }
                    else
                    {
                        int rf;
                        *(int *)(o + 0x4660 + idx) = 0;
                        rf = RandomIntInternal(&data_0209d4b8);
                        *(u16 *)(o + 0x4686 + idx) =
                            (u16)(((((((rf >> 16) & 0x7fff) * 0xc) >> 15) + 1) << 9) + 0xc000);
                    }

                    *(int *)(o + 0x4678 + idx) = 0;
                    *(int *)(o + 0x4670 + idx) = 0;
                    *(int *)(o + 0x4674 + idx) = 0;
                    *(u16 *)(o + 0x4684 + idx) = (u16)(*(u16 *)(o + 0x4686 + idx) - 0x4000);
                    *(int *)(o + 0x4664 + idx) = -0x60000;
                    {
                        int ang = *(u16 *)(o + 0x4686 + idx) >> 4;
                        *(int *)(o + 0x4668 + idx) =
                            (int)(((long long)data_02082214[ang * 2 + 1] * 0xe80 + 0x800) >> 12);
                        *(int *)(o + 0x466c + idx) =
                            (int)(((long long)data_02082214[ang * 2] * 0xe80 + 0x800) >> 12);
                    }
                    *(u8 *)(o + 0x468f + idx) = 3;
                    *(u8 *)(o + 0x468d + idx) = 1;
                    *(u8 *)(o + 0x4693 + idx) = 0;
                    *(int *)(o + 0x4680 + idx) = 0x1000;
                    *(u16 *)(o + 0x4688 + idx) = 0x10;
                }
            }
            break;
        }
    }

    {
        int rem = *(u16 *)(o + 0x5c26);
        int q = 0;
        while (rem >= 0xa)
        {
            rem -= 0xa;
            q++;
        }
        if (q != 0)
        {
            if (rem == 0)
                *(u16 *)(o + 0x5c28) = *(u16 *)(o + 0x5c28) + 1;
        }
    }

    if (*(u16 *)(o + 0x5c28) > 0x28)
        *(u16 *)(o + 0x5c28) = 0x27;

    *(u16 *)(o + 0x5c24) = (u16)(*(u16 *)(o + 0x5c24) - (*(u16 *)(o + 0x5c28) << 2));
    if (*(short *)(o + 0x5c24) <= 0x20)
        *(u16 *)(o + 0x5c24) = 0x20;
}
