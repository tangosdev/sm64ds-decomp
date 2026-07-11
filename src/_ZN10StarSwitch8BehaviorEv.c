extern unsigned char IsAreaShowing(int idx);
extern void func_ov002_020ba01c(char *c, int mask, int b, int base, int target);
extern void func_ov002_020ba4d8(char *c, int i);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020e6d88(void *c);
extern int func_ov002_020b9f00(char *c);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char *c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char *c, int a, int b);
extern void func_ov002_020b9f80(char *c);
extern void func_ov002_020ba520(char *c);
extern int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int id, int vol);

extern int data_0209b454;

int _ZN10StarSwitch8BehaviorEv(char *c)
{
    void *a;
    int id344;
    int v;

    if (IsAreaShowing(*(signed char *)(c + 0x353)) == 0) {
        *(unsigned char *)(c + 0xcc) = *(signed char *)(c + 0x353);
        *(short *)(c + 0x338) = 1;
        func_ov002_020ba01c(c, 2, 1, 0x333, 0x1000);
        func_ov002_020ba4d8(c, 0);
        a = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x344));
        if (a != 0) {
            func_ov002_020e6d88(a);
        }
    }

    {
        int isType = (int)(*(unsigned short *)(c + 0xc) == 0xc);
        if (isType != 0) {
            if (func_ov002_020b9f00(c) != 0) {
                a = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x344));
                if (a != 0) {
                    *(void **)(c + 0x348) = a;
                }
            }
        }
    }

    if ((int)(*(unsigned short *)(c + 0xc) == 0xc) != 0) {
        id344 = *(unsigned int *)(c + 0x344);
        if (id344 == 0) {
            func_ov002_020ba01c(c, 2, 3, 0x333, 0x1000);
            _ZN8Platform21UpdateModelPosAndRotYEv(c);
            if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
                func_ov002_020b9f80(c);
            }
            return 1;
        }
        if (_ZN5Actor10FindWithIDEj(id344) == 0) {
            *(unsigned int *)(c + 0x344) = 0;
            *(unsigned char *)(c + 0xcc) = *(signed char *)(c + 0x353);
            return 1;
        }
    }

    if ((data_0209b454 & 0x4000000) == 0) {
        unsigned short *p = (unsigned short *)(((long long)(int)(c + 0x338)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p + 1;
    }
    func_ov002_020ba520(c);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
        func_ov002_020b9f80(c);
    }

    if (*(int *)(c + 0x33c) == 2) {
        if (*(unsigned char *)(c + 0x34d) == 0) {
            v = *(int *)(c + 0x334);
            if (v != 0x40) {
                if (v == 0x7f) {
                    if ((data_0209b454 & 0x4000000) == 0) {
                        *(unsigned char *)(c + 0x34d) = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0x64cc);
                    }
                }
            } else {
                *(unsigned char *)(c + 0x34d) = _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(v, 0xc999);
            }
        }
    }

    *(unsigned char *)(c + 0x350) = 0;
    return 1;
}
