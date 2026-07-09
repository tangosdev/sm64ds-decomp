typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 data_0209f5bc[];

extern u8 data_0209f1e8;
extern u8 data_020a0e40;
extern u16 data_020a0e58[];
extern u16 data_020a0e5a[];
extern int data_0208ee44;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern u8 data_0209d454;

extern int func_0201a1bc(void);
extern void func_0201fe08(void);
extern void func_02005348(void *self);
extern int func_0203da3c(void);
extern void _ZN5Scene14StartSceneFadeEjjt(u32 a, u32 b, u16 c);
extern int GetOwnerLanguage(void);
extern u32 func_02054e88(void);
extern u32 LoadCompressedFileAt(u16 fileID, void *target);
extern int LoadFile(int handle);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void func_0201cd08(int arg);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void func_02012790(int a);
extern void _ZN7Message21DisplaySaveStatusTextEt(u16 a);
extern void _ZN8SaveData16EraseAllSaveDataEv(void);

#define LADR(p) ((void *)(unsigned int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

#pragma opt_common_subs off

int func_02005418(void *arg0)
{
    char *c = (char *)arg0;
    u16 h58, h5a;
    int r4;

    r4 = data_0208ee44;
    h58 = *(u16 *)((char *)data_020a0e58 + data_020a0e40 * 4);
    h5a = *(u16 *)((char *)data_020a0e5a + data_020a0e40 * 4);

    if (data_0209f1e8 == 0) {
        data_0209f1e8 = (u8)func_0201a1bc();
        if (data_0209f1e8 == 0) {
            if ((int)*(volatile u16 *)(c + 0x50) > (int)(r4 * 2)) {
                *(u16 *)LADR(c + 0x50) -= r4;
            }
            return 1;
        }
        func_0201fe08();
    }

    {
    void *obj = *(void **)data_0209f5bc;
    int (**vt)(void *) = *(int (***)(void *))obj;
    if (vt[5](obj) != 0) {
        int f52;

        if (*(volatile u8 *)(c + 0x54) != 0) {
            *(u8 *)LADR(c + 0x54) -= r4;
            if (*(u8 *)(c + 0x54) == 0) {
                func_02005348(c);
            }
        }
        if (*(volatile u8 *)(c + 0x55) != 0) {
            *(u8 *)LADR(c + 0x55) -= r4;
            return 1;
        }

        f52 = *(u8 *)(c + 0x52);
        switch (f52) {
        case 0:
        case 7:
            if (*(volatile u16 *)(c + 0x50) != 0) {
                *(u16 *)LADR(c + 0x50) -= r4;
                if (*(u16 *)(c + 0x50) == 0) {
                    _ZN5Scene14StartSceneFadeEjjt(func_0203da3c() != 0 ? 6 : 1, 0, 0);
                } else if (*(u8 *)(c + 0x52) == 0 && h58 == 0xf03) {
                    u16 lang;
                    int f;

                    if (GetOwnerLanguage() == 5) {
                        lang = 0xb00d;
                    } else if (GetOwnerLanguage() == 4) {
                        lang = 0xac0d;
                    } else if (GetOwnerLanguage() == 3) {
                        lang = 0xa80d;
                    } else if (GetOwnerLanguage() == 2) {
                        lang = 0xa40d;
                    } else {
                        lang = 0xa00d;
                    }
                    LoadCompressedFileAt(lang, (void *)func_02054e88());
                    f = LoadFile(0x9807);
                    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x1c0);
                    Deallocate((void *)f);
                    LoadCompressedFileAt(0x22d, _ZN3G2S12GetBG1ScrPtrEv());
                    LoadCompressedFileAt(0x9803, (char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x800);
                    func_0201cd08(0x29a);
                    *(u8 *)(c + 0x53) = 1;
                    func_02005348(c);
                    SetSubBg0Offset(0, 0);
                    SetSubBg1Offset(0, 0);
                    data_0209d454 |= 3;
                    data_0209d454 &= ~4;
                    *(u8 *)(c + 0x52) = 1;
                }
            }
            break;

        case 1:
        case 3:
        {
            int pi = data_020a0e40;
            u8 e8;

            r4 = 0;
            e8 = data_020a0de8[pi * 4];
            if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                r4 = 1;
            }
            if (r4 != 0 || (h5a & 0x39)) {
                int ok2;

                if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                    ok2 = 1;
                } else {
                    ok2 = 0;
                }
                if ((ok2 != 0
                     && (u8)(data_020a0dea[pi * 4] - 0x28) < 0x50
                     && (u8)(data_020a0deb[pi * 4] - 0x98) < 0x20)
                    || (h5a & 0x20)) {
                    if (*(u8 *)(c + 0x53) == 0) {
                        *(u8 *)(c + 0x54) = 0x10;
                    }
                    *(u8 *)(c + 0x53) = 0;
                    func_02005348(c);
                    func_02012790(0);
                    if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x28) < 0x50
                        && (u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                        *(u8 *)(c + 0x55) = 0x20;
                        if (*(u8 *)(c + 0x52) == 1) {
                            *(u8 *)(c + 0x52) = 2;
                        } else {
                            *(u8 *)(c + 0x52) = 4;
                        }
                    }
                } else {
                    int ok3;

                    if (e8 != 0 && data_020a0de9[pi * 4] != 0) {
                        ok3 = 1;
                    } else {
                        ok3 = 0;
                    }
                    if ((ok3 != 0
                         && (u8)(data_020a0dea[pi * 4] - 0x88) < 0x50
                         && (u8)(data_020a0deb[pi * 4] - 0x98) < 0x20)
                        || (h5a & 0x10)) {
                        if (*(u8 *)(c + 0x53) == 1) {
                            *(u8 *)(c + 0x54) = 0x10;
                        }
                        *(u8 *)(c + 0x53) = 1;
                        func_02005348(c);
                        func_02012790(0);
                        if ((u8)(data_020a0dea[data_020a0e40 * 4] - 0x88) < 0x50) {
                            if ((u8)(data_020a0deb[data_020a0e40 * 4] - 0x98) < 0x20) {
                                *(u8 *)(c + 0x55) = 0x20;
                                *(u8 *)(c + 0x52) = 6;
                            }
                        }
                    } else if (h5a & 9) {
                        *(u8 *)(c + 0x54) = 0x10;
                        *(u8 *)(c + 0x55) = 0x20;
                        func_02005348(c);
                        func_02012790(0);
                        if (*(u8 *)(c + 0x53) == 0) {
                            if (*(u8 *)(c + 0x52) == 1) {
                                *(u8 *)(c + 0x52) = 2;
                            } else {
                                *(u8 *)(c + 0x52) = 4;
                            }
                        } else {
                            *(u8 *)(c + 0x52) = 6;
                        }
                    }
                }
            }
            break;
        }

        case 2:
            func_0201cd08(0x29b);
            *(u8 *)(c + 0x53) = 1;
            func_02005348(c);
            *(u8 *)(c + 0x52) = 3;
            break;

        case 4:
            *(volatile u16 *)0x0400100a = (u16)((*(volatile u16 *)0x0400100a & 0x43) | 0x218);
            _ZN7Message21DisplaySaveStatusTextEt(0x29c);
            *(u8 *)(c + 0x56) = 0x78;
            *(u8 *)(c + 0x52) = 5;
            break;

        case 5:
            if (*(u8 *)(c + 0x56) == 0x3c) {
                _ZN8SaveData16EraseAllSaveDataEv();
            }
            if (*(volatile u8 *)(c + 0x56) != 0) {
                u8 t;

                *(u8 *)LADR(c + 0x56) -= r4;
                t = *(u8 *)(c + 0x56);
                if (t == 0x3c) {
                    _ZN7Message21DisplaySaveStatusTextEt(0x29d);
                } else if (t == 0) {
                    data_0209d454 &= ~3;
                    data_0209d454 |= 4;
                    *(u16 *)(c + 0x50) = 0x3c;
                    *(u8 *)(c + 0x52) = 7;
                }
            }
            break;

        case 6:
            data_0209d454 &= ~3;
            data_0209d454 |= 4;
            *(u16 *)(c + 0x50) = 0x3c;
            *(u8 *)(c + 0x52) = 0;
            break;
        }
    }
    }
    return 1;
}
