//cpp
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int _ZN6Player9GetHealthEv(void);
extern void func_ov002_020c5dec(void*, int);
extern int func_ov002_020cede0(void*);
extern int func_ov002_020cec2c(void*);
extern void func_ov002_020cd71c(void*);
extern void func_ov002_020cd550(void*);
extern unsigned int func_02022d44(unsigned int, unsigned int, int, int, int, void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern void* _ZNK12WithMeshClsn14GetFloorResultEv(void*);
extern void* func_02037dc4(void*);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern void Vec3_RotateYAndTranslate(int*, void*, short, int*);
extern void _Z14ApproachLinearRiii(int*, int, int);
extern int _ZN6Player6IsAnimEj(void*, unsigned int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern int _ZNK6Player14GetBodyModelIDEjb(void*, unsigned char, int);
extern int func_ov002_020ce324(void*);
extern void func_ov002_020cd364(void*);
extern void func_ov002_020cd3e0(void*);
extern int func_ov002_020cedb0(void*, int);
extern void func_ov002_020cd39c(void*);
extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int, void*);
extern void func_ov002_020cd2c4(void*);
extern void func_ov002_020cd308(void*);
extern void func_ov002_020dbf4c(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void func_ov002_020bd928(void*, unsigned int);
extern void func_ov002_020bd8c0(void*, unsigned int);
extern void func_ov002_020dab14(void*);
extern void func_ov002_020daa74(void*);
extern void func_ov002_020cd218(void*, int, void*, void*);
extern int func_ov002_020ceaf4(void*);
extern int func_ov002_020ce5f8(void*);
extern void func_ov002_020bedd4(void*);

extern char data_ov002_021106ac[];
extern char data_ov002_021101b4[];
extern unsigned char data_020a0e40;
extern unsigned short data_0209f49c;
extern unsigned short data_0209f49e;
extern char data_0209ee90[];

int _ZN6Player12St_Swim_MainEv(char* c)
{
    int t4;
    volatile int pos[3];
    int* fr;
    int ang;
    int out[3];
    int vec[3];
    short yaw;
    int var_r2;
    unsigned short a4;
    int var_r6;
    int var_r1;
    int has;
    int* p;
    int* sub;
    unsigned int t5;


    if (*(unsigned char*)(c+0x6f9) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021106ac);
        return 1;
    }
    if (_ZN6Player9GetHealthEv() == 0) {
        func_ov002_020c5dec(c, 8);
        return 1;
    }
    if (func_ov002_020cede0(c) == 0) {
        *(unsigned char*)(c+0x706) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
        return 1;
    }
    *(int*)(c+0x684) = *(int*)(c+0x60);
    if (func_ov002_020cec2c(c) != 0) return 1;

    t4 = (*(unsigned short*)((char*)&data_0209f49c + data_020a0e40 * 0x18)) & 0x802;
    *(short*)(c+0x69e) = *(short*)(c+0x90);
    func_ov002_020cd71c(c);
    func_ov002_020cd550(c);

    if (*(unsigned short*)(c+0x6a8) != 0) {
        int eid = 0xcd;
        int y, z;
        pos[0] = *(int*)(c+0x5c);
        y = *(int*)(c+0x60);
        pos[1] = y;
        z = *(int*)(c+0x64);
        pos[2] = z;
        y = y + 0x32000;
        pos[1] = y;
        *(unsigned int*)(c+0x630) = func_02022d44(*(volatile unsigned int*)(c+0x630), eid, pos[0], pos[1], z, 0);
    }

    if (*(int*)(c+0x640) == 0 && _ZNK12WithMeshClsn10IsOnGroundEv(c+0x380) != 0) {
        fr = (int*)func_02037dc4((char*)_ZNK12WithMeshClsn14GetFloorResultEv(c+0x380) + 4);
        ang = fr[1];
        if (ang < 0x720 && ang > 0x400 && *(int*)(c+0x60) >= *(int*)(c+0x64c) - 0xc8000) {
            int at;
            *(int*)(c+0x60) = *(int*)(c+0x54c) - 0x8000;
            at = _ZN4cstd5atan2E5Fix12IiES1_(fr[0], fr[2]);
            vec[0] = 0;
            vec[1] = 0;
            at = at + 0x8000;
            yaw = (short)at;
            vec[2] = (*(int*)((char*)data_0209ee90 + 0x244)) << 0xc;
            Vec3_RotateYAndTranslate(out, c+0x5c, yaw, vec);
            *(int*)(c+0x5c) = out[0];
            *(int*)(c+0x60) = out[1];
            *(int*)(c+0x64) = out[2];
        }
    }

    switch (*(unsigned char*)(c+0x6e3)) {
    case 0:
        *(short*)(c+0x92) = -0x4000;
        *(short*)(c+0x96) = 0;
        *(short*)(c+0x8c) = 0;
        *(short*)(c+0x90) = 0;
        *(short*)(c+0x69c) = 0;
        _Z14ApproachLinearRiii((int*)(c+0x640), 0, 0x5000);
        if (_ZN6Player6IsAnimEj(c, 0xa8) == 0 && _ZN6Player12FinishedAnimEv(c) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa8, 0, 0x1000, 0);
        }
        if (*(int*)(c+0x640) == 0) {
            *(unsigned char*)(c+0x6e3) = 1;
            *(short*)(c+0x92) = 0;
            *(unsigned short*)(c+0x6a4) = 0x1e;
        }
        break;
    case 1:
        var_r2 = 0x800;
        if (*(unsigned short*)(c+0x6a4) != 0) var_r2 = 0x200;
        _Z14ApproachLinearRiii((int*)(c+0x640), 0, var_r2);
        if (_ZN6Player6IsAnimEj(c, 0xa8) == 0) {
            if (_ZN6Player12FinishedAnimEv(c) != 0) {
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa8, 0, 0x1000, 0);
            }
        } else if (*(short*)(c+0x8c) >= 0x1000) {
            sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x3000) {
                sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x3000;
            }
        } else if (*(short*)(c+0x69c) != 0) {
            sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x2000) {
                sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x2000;
            }
        } else {
            sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
            if (*(int*)((char*)sub + 0xc) != 0x1000) {
                sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
                *(int*)((char*)sub + 0xc) = 0x1000;
            }
        }
        if (func_ov002_020ce324(c) != 0) return 1;
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            func_ov002_020cd364(c);
        } else if (t4 != 0) {
            func_ov002_020cd3e0(c);
        }
        break;
    case 2:
        a4 = *(unsigned short*)(c+0x6a4);
        if (a4 >= 0xa) var_r6 = 0x800;
        else if (a4 <= 4) var_r6 = 0x1800;
        else var_r6 = 0;
        _Z14ApproachLinearRiii((int*)(c+0x640), func_ov002_020cedb0(c, 0x10000), var_r6);
        if (*(unsigned short*)(c+0x6a4) < 0xa && t4 == 0) {
            *(unsigned char*)(((unsigned int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFFULL) |= 1;
        }
        if (*(unsigned short*)(c+0x6a4) == 0 && _ZN6Player6IsAnimEj(c, 0xa9) != 0 && _ZN6Player12FinishedAnimEv(c) != 0) {
            if (!(*(unsigned char*)(c+0x6e5) & 1)) {
                *(unsigned char*)(c+0x6e3) = 3;
            } else {
                func_ov002_020cd39c(c);
            }
        }
        break;
    case 3:
        _Z14ApproachLinearRiii((int*)(c+0x640), func_ov002_020cedb0(c, 0xc000), 0x200);
        if (func_ov002_020ce324(c) != 0) return 1;
        if (t4 != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa6, 0, 0x1000, 0);
            if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc) + 0x50), 0) != 0) {
                _ZN5Sound9PlayBank0EjRK7Vector3(0x15, c+0x74);
            }
        } else {
            func_ov002_020cd39c(c);
        }
        break;
    case 4:
        _Z14ApproachLinearRiii((int*)(c+0x640), 0, 0x400);
        if (func_ov002_020ce324(c) != 0) return 1;
        if (*(unsigned short*)(c+0x6a4) == 0 && t4 != 0) {
            func_ov002_020cd3e0(c);
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0 && _ZN6Player6IsAnimEj(c, 0xaa) != 0) {
            func_ov002_020cd2c4(c);
        }
        break;
    case 5:
        if (*(unsigned short*)(c+0x6a4) == 0) {
            func_ov002_020cd308(c);
        }
        break;
    case 6:
        func_ov002_020dbf4c(c);
        var_r1 = func_ov002_020cedb0(c, 0x10000);
        a4 = *(unsigned short*)(c+0x6a4);
        if (a4 >= 5) var_r2 = 0x1000;
        else if (a4 == 0) { var_r2 = 0x800; var_r1 = 0; }
        else var_r2 = 0;
        _Z14ApproachLinearRiii((int*)(c+0x640), var_r1, var_r2);
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            has = (*(int*)(c+0x358) != 0) ? 1 : 0;
            if (has != 0) {
                *(unsigned char*)(c+0x6e3) = 8;
                *(unsigned short*)(c+0x6a4) = 0xf0;
                p = (int*)(((*(int*)(c+0x358) + 0xb0) & 0xFFFFFFFFFFFFFFFFULL));
                *p |= 0x4000;
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xb0, 0, 0x1000, 0);
            } else {
                func_ov002_020cd2c4(c);
            }
        }
        _ZN12CylinderClsn5ClearEv(c+0x314);
        sub = (int*)((char*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc)) + 0x50);
        t5 = ((unsigned int)(*(int*)((char*)sub + 8)) << 4) >> 0x10;
        if ((int)t5 >= 5 && (int)t5 <= 7) {
            _ZN12CylinderClsn6UpdateEv(c+0x314);
        }
        break;
    case 7:
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            *(unsigned char*)(c+0x6e3) = 8;
            *(unsigned short*)(c+0x6a4) = 0xf0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xb0, 0, 0x1000, 0);
            p = (int*)(((*(int*)(c+0x358) + 0xb0) & 0xFFFFFFFFFFFFFFFFULL));
            *p |= 0x4000;
            func_ov002_020bd928(c, 0x33);
            *(unsigned char*)(c+0x6f7) = 1;
        }
        break;
    case 8:
        _Z14ApproachLinearRiii((int*)(c+0x640), func_ov002_020cedb0(c, 0x1e000), 0x400);
        if ((*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18)) & 1) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xb1, 0x40000000, 0x1000, 0);
            *(unsigned char*)(c+0x6e3) = 9;
            func_ov002_020bd8c0(c, 0x33);
        }
        if (*(unsigned short*)(c+0x6a4) != 0) {
            has = (*(int*)(c+0x358) != 0) ? 1 : 0;
            if (has == 0) goto case8_block107;
        } else {
        case8_block107:
            func_ov002_020dab14(c);
            func_ov002_020cd2c4(c);
            func_ov002_020bd8c0(c, 0x33);
            *(unsigned char*)(c+0x6f7) = 0;
        }
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc) + 0x50), 0) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0x15, c+0x74);
        }
        break;
    case 9:
        if (_ZNK9Animation12WillHitFrameEi((void*)(*(int*)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0)*4 + 0xdc) + 0x50), 0xd) != 0) {
            func_ov002_020daa74(c);
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            func_ov002_020cd2c4(c);
        }
        break;
    }

    func_ov002_020cd218(c, *(int*)(c+0x640), c+0x98, c+0xa8);
    {
        int* pa8 = (int*)(((unsigned int)(c + 0xa8)) & 0xFFFFFFFFFFFFFFFFULL);
        *pa8 = *pa8 + func_ov002_020ceaf4(c);
    }
    if (func_ov002_020ce5f8(c) != 0) return 1;
    func_ov002_020bedd4(c);
    return 1;
}
}
