#pragma opt_loop_invariants off

extern int data_ov006_02142f78[];

extern void *func_02054efc(void);
extern void *func_02054ea8(void);
extern void *_ZN2G213GetBG2CharPtrEv(void);
extern int func_02054d88(void);
extern void MultiCopy_Int(int *dst, int *src, int len);

void func_ov006_02122cb0(void *obj, int x_base, int y, int val, int n)
{
    int half;
    int x0;
    int j;
    int x;
    int *addr;
    int i;
    int yy;
    int mask = 0xf;

    half = n / 2;
    j = 0;
    if (j < n)
    {
        yy = y - half;
        x0 = x_base - half;
        do
        {
            for (i = 0; i < n; i++)
            {
                x = i + x0;
                if (x >= 0 && x < 0x100)
                {
                    if (yy >= 0 && yy < 0xc0)
                    {
                        void *ret;
                        int mode = *(int *)((char *)obj + 0x6c);

                        switch (mode)
                        {
                            case 0:
                                ret = func_02054efc();
                                break;
                            case 1:
                                ret = func_02054ea8();
                                break;
                            case 2:
                                ret = _ZN2G213GetBG2CharPtrEv();
                                break;
                            case 3:
                                ret = (void *)func_02054d88();
                                break;
                            default:
                                return;
                        }

                        addr = (int *)((char *)ret +
                            ((x / 8) + (yy / 8) * 32) * 32 +
                            (yy & 7) * 4);

                        MultiCopy_Int(addr, data_ov006_02142f78, 4);
                        data_ov006_02142f78[0] =
                            (data_ov006_02142f78[0] &
                             (-1 ^ (mask << ((x & 7) * 4)))) |
                            (val << ((x & 7) * 4));
                        MultiCopy_Int(data_ov006_02142f78, addr, 4);
                    }
                }
            }
            yy++;
            j++;
        } while (j < n);
    }
}
