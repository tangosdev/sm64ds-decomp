//cpp
/* Player::St_YoshiPower_Main at 0x020d7504 (ov002), size 0x9cc
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (also base/sp2)
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
struct Vec3 { int x, y, z; };
struct Vec3s { short x, y, z; };
struct PVec { int x, y, z; PVec() {} ~PVec() {} };
struct Obj {
  virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3();
  virtual int v4(); virtual int v5(); virtual int v6(); virtual int v7();
  virtual int v8(); virtual int v9(); virtual int v10(); virtual int v11();
  virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
  virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19(char* c);
};
extern "C" {
extern void _Z14ApproachLinearRiii(int*, int, int);
extern void func_ov002_020c2f64(char*);
extern int _ZNK6Player14GetBodyModelIDEjb(char*, unsigned int, int);
extern int func_ov002_020d6998(char*);
extern int _ZN6Player12FinishedAnimEv(char*);
extern void func_ov002_020d71ec(char*, int);
extern void _Z15ApproachLinear2Rsss(short*, short, short);
extern void MulVec3Mat4x3(void*, void*, void*);
extern void Vec3_MulScalarInPlace(void*, int);
extern int _ZNK9Animation12WillHitFrameEi(void*, int);
extern int func_ov002_020d5ed0(void*);
extern void func_ov002_020d718c(char*);
extern void func_ov002_020d8118(char*);
extern void _ZN6Player11ChangeStateERNS_5StateE(char*, void*);
extern int func_ov002_020e0ccc(char*, void*);
extern void func_ov002_020d7430(char*);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void func_ov002_020d5cec(void*);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, void*);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(void*, unsigned int, int, int, int, void*, void*);
extern void func_ov002_020dc09c(char*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void ApproachAngle(void*, short, int, int, int);
extern void func_ov002_020daa74(char*);
extern void func_ov002_020bedd4(char*);
extern void _ZN9Animation7AdvanceEv(void*);

extern short SINE_TABLE[];
extern int data_ov002_0210e3d8[];
extern char _ZN6Player14ST_YOSHI_POWERE;
extern char _ZN6Player7ST_FALLE;
extern char _ZN6Player7ST_WALKE;
extern char _ZN6Player7ST_SWIME;

int _ZN6Player18St_YoshiPower_MainEv(char* c)
{
    Vec3s dir;
    Vec3 tmp;
    PVec pos;
    Obj* obj0;
    char* dp;

    if (*(unsigned char*)(c+0x6de) == 0) {
        _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x1000);
        func_ov002_020c2f64(c);
    }

    switch (*(unsigned char*)(c+0x6e3)) {
    case 0: {
        if (*(unsigned char*)(c+0x70c) == 0) {
            int t4 = (*(int*)((char*)*(void**)(c+0x160) + 0x58) >> 0xc) << 0x10;
            int id0 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
            dp = (char*)*(void**)(c + (id0<<2) + 0xdc);
            dp = (char*)(((long long)(int)(dp + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
            *(unsigned int*)(dp + 8) = (unsigned int)t4 >> 4;
            *(unsigned char*)(c+0x70c) = 1;
        }
        if (func_ov002_020d6998(c) != 0) {
            return 1;
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            func_ov002_020d71ec(c, 0);
        }
        goto common_tail;
    }

    case 1: {
        _Z15ApproachLinear2Rsss((short*)(c+0x764), 0, 0x200);
        _Z15ApproachLinear2Rsss((short*)(c+0x766), 0, 0x200);
        int flag = 1;
        *(unsigned char*)(c+0x742) = flag;
        if (*(void**)(c+0x360) != 0) {
            int b = *(unsigned char*)(c+0x714);
            char* t4 = (char*)(*(int*)(*(int*)(c + ((b<<2)+3)*4 + 0x154) + 0x14) + 0x90);
            if (*(void**)(c+0x360) != 0) {
                if (!(*(int*)((char*)*(void**)(c+0x360)+0xb0) & 0x20000)) flag = 0;
                if (flag != 0) {
                    int id1 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
                    MulVec3Mat4x3(t4 + 0x24, (char*)*(void**)(c + (id1<<2) + 0xdc) + 0x1c, &tmp);
                    Vec3_MulScalarInPlace(&tmp, 0x8000);
                    dp = (char*)*(void**)(c+0x360);
                    *(int*)(dp+0x5c) = tmp.x;
                    *(int*)(dp+0x60) = tmp.y;
                    *(int*)(dp+0x64) = tmp.z;
                }
            }
            int id2 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
            if (_ZNK9Animation12WillHitFrameEi((char*)*(void**)(c + (id2<<2) + 0xdc) + 0x50, 0xa) != 0) {
                Vec3* src = (Vec3*)(((int)c + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
                dp = (char*)*(void**)(c+0x360);
                *(int*)(dp+0x5c) = src->x;
                *(int*)(dp+0x60) = src->y;
                *(int*)(dp+0x64) = src->z;
                Obj* o = *(Obj**)(c+0x360);
                int cond = (*(unsigned short*)((char*)o+0xc) == 0xbf);
                if (cond != 0) {
                    *(int*)(((int)o + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20000;
                    if (func_ov002_020d5ed0(*(void**)(c+0x360)) == 0) {
                        func_ov002_020d718c(c);
                    }
                } else {
                    *(int*)(((int)o + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40000;
                    *(int*)(((int)*(void**)(c+0x360) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20000;
                    if ((*(Obj**)(c+0x360))->v18() == 4 || (*(Obj**)(c+0x360))->v18() == 2) {
                        *(unsigned char*)(c+0x6e3) = 3;
                        int f2 = 1;
                        *(unsigned char*)(c+0x714) = f2;
                        *(unsigned short*)(c+0x6a4) = 8;
                        if (*(unsigned short*)((char*)*(void**)(c+0x360)+0xc) != 0x10d) f2 = 0;
                        if (f2 != 0) *(unsigned short*)(c+0x6a4) = 0xc;
                        *(unsigned char*)(c+0x6ee) = 1;
                        goto common_tail;
                    }
                }
            } else {
                int id3 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
                if (_ZNK9Animation12WillHitFrameEi((char*)*(void**)(c + (id3<<2) + 0xdc) + 0x50, 0xc) != 0) {
                    *(unsigned char*)(c+0x714) = 1;
                    if ((*(Obj**)(c+0x360))->v18() == 5) {
                        int cond2 = (*(unsigned short*)((char*)*(void**)(c+0x360)+0xc) == 0x132);
                        if (cond2 != 0) {
                            func_ov002_020d8118(c);
                        }
                        (*(Obj**)(c+0x360))->v19(c);
                        *(void**)(c+0x360) = 0;
                        *(unsigned char*)(c+0x6f4) = 1;
                    }
                }
            }
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            obj0 = *(Obj**)(c+0x360);
            if (obj0 != 0) {
                if (obj0->v18() == 3) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player14ST_YOSHI_POWERE);
                    return 1;
                }
                if (func_ov002_020e0ccc(c, *(void**)(c+0x360)) != 0) {
                    return 1;
                }
                *(unsigned short*)(c+0x6c6) = 0x5a;
            }
            if (*(unsigned char*)(c+0x6de) != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_FALLE);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            }
            return 1;
        }
        goto common_tail;
    }

    case 2: {
        _Z15ApproachLinear2Rsss((short*)(c+0x764), 0, 0x200);
        _Z15ApproachLinear2Rsss((short*)(c+0x766), 0, 0x200);
        *(unsigned char*)(c+0x742) = 1;
        if (*(unsigned short*)(c+0x6a4) != 0) {
            return 1;
        }
        int id4 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
        int* a50b = (int*)(((long long)(int)((char*)*(void**)(c + (id4<<2) + 0xdc) + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
        if ((unsigned short)(a50b[2] >> 0xc) == 0) {
            if (*(unsigned char*)(c+0x6de) != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_FALLE);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            }
            return 1;
        }
        goto common_tail;
    }

    case 3:
        if (*(unsigned short*)(c+0x6a4) == 0) {
            if (*(void**)(c+0x360) == 0) {
                if (*(unsigned char*)(c+0x706) != 0) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_SWIME);
                } else {
                    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
                }
            } else {
                func_ov002_020d7430(c);
            }
            return 1;
        }
        goto common_tail;

    case 4: {
        int id5 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi((char*)*(void**)(c + (id5<<2) + 0xdc) + 0x50, 3) != 0) {
            int z = 0;
            *(unsigned char*)(c+0x714) = z;
            if (*(unsigned char*)(c+0x714) == 0) {
                int id6 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, z);
                int* a50c = (int*)(((long long)(int)((char*)*(void**)(c + (id6<<2) + 0xdc) + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void**)(c+0x160), (void*)data_ov002_0210e3d8[1], 0x40000000, 0x1000, (unsigned int)(a50c[2] << 4) >> 0x10);
            }
        }
        int id7 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi((char*)*(void**)(c + (id7<<2) + 0xdc) + 0x50, 4) != 0) {
            obj0 = *(Obj**)(c+0x360);
            if (obj0 != 0) {
                int cond3 = (*(unsigned short*)((char*)obj0+0xc) == 0xbf);
                if (cond3 != 0) {
                    func_ov002_020d5cec(obj0);
                    *(unsigned short*)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
                } else {
                    *(int*)(((int)obj0 + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80000;
                    *(int*)(((int)*(void**)(c+0x360) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x40000;
                }
            }
            *(void**)(c+0x360) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0xff, c+0x74);
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            return 1;
        }
        goto common_tail;
    }

    case 5: {
        if (*(unsigned short*)(c+0x6a4) != 0) {
            int id8 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
            int* a50d = (int*)(((long long)(int)((char*)*(void**)(c + (id8<<2) + 0xdc) + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
            if (((unsigned int)(a50d[2] << 4) >> 0x10) >= 4) {
                int i2 = (*(unsigned short*)(c+0x8e) >> 4) * 2;
                short* tbl = SINE_TABLE;
                int zv = *(int*)(c+0x64) + (int)(((((long long)tbl[i2+1]) << 18) + 0x800) >> 12);
                int yv = *(int*)(c+0x60) + 0x37000;
                pos.x = *(int*)(c+0x5c) + (int)(((((long long)tbl[i2]) << 18) + 0x800) >> 12);
                pos.z = zv;
                pos.y = yv;
                dir.x = SINE_TABLE[(*(unsigned short*)(c+0x8e) >> 4) * 2];
                dir.y = 0;
                dir.z = SINE_TABLE[(*(unsigned short*)(c+0x8e) >> 4) * 2 + 1];
                *(void**)(c+0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(void**)(c+0x628), 0x13c, pos.x, pos.y, pos.z, (Vec3s*)(((int)&dir) & 0xFFFFFFFFFFFFFFFFLL), 0);
                *(void**)(c+0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(void**)(c+0x62c), 0x13d, pos.x, pos.y, pos.z, &dir, 0);
                func_ov002_020dc09c(c);
                _ZN12CylinderClsn5ClearEv(c+0x314);
                _ZN12CylinderClsn6UpdateEv(c+0x314);
                if (*(unsigned short*)(c+0x6a4) == 1 && *(unsigned char*)(c+0x6f4) == 0) {
                    int z2 = 0;
                    *(unsigned char*)(c+0x714) = z2;
                    int id9 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, z2);
                    int* a50e = (int*)(((long long)(int)((char*)*(void**)(c + (id9<<2) + 0xdc) + 0x50)) & 0xFFFFFFFFFFFFFFFFLL);
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(*(void**)(c+0x160), (void*)data_ov002_0210e3d8[1], 0x40000000, 0x1000, (unsigned int)(a50e[2] << 4) >> 0x10);
                }
                return 1;
            }
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            return 1;
        }
        goto common_tail;
    }

    case 6: {
        void* p = *(void**)(c+0x358);
        if (p != 0) {
            ApproachAngle(c+0x8e, *(short*)((char*)p+0x8e), 4, 0x2000, 0x10);
        }
        *(short*)(c+0x94) = *(short*)(c+0x8e);
        int id10 = _ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c+8) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi((char*)*(void**)(c + (id10<<2) + 0xdc) + 0x50, 6) != 0) {
            func_ov002_020daa74(c);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x102, c+0x74);
            goto common_tail;
        }
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            return 1;
        }
        goto common_tail;
    }

    default:
    common_tail:
        func_ov002_020bedd4(c);
        if (*(unsigned char*)(c+0x714) == 0 && *(unsigned char*)(c+0x6e3) != 6) {
            _ZN9Animation7AdvanceEv((char*)*(void**)(c+0x160) + 0x50);
        }
        return 1;
    }
}
}
