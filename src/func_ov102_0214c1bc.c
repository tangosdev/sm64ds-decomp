// NONMATCHING: missing logic (ROM does more) (div=40). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov102_0214ae1c(void*);
extern int func_ov102_0214ab1c(void*);
extern int func_ov102_0214aa18(void*);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void*,void*,void*,unsigned int);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(void*,void*);
extern void func_ov102_0214b128(void*);
extern void func_ov102_0214b53c(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
extern void func_ov102_0214b03c(void*);
extern void *_ZN5Actor10FindWithIDEj(unsigned int);
extern void func_020ada40(void*,void*,void*,unsigned int);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*,void*);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void*,void*,unsigned int);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void*);
extern int func_02037e78(int*);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void*);
extern void func_ov102_0214beb4(void*);
extern int func_ov102_0214b248(void*);
extern void func_ov102_0214b384(void*,int);
extern void func_ov102_0214ad40(void*);

extern signed char LEVEL_ID;
extern unsigned char STAR_ID;

int func_ov102_0214c1bc(char *c)
{
    int flag;
    int r0;
    void *other;

    if (*(unsigned char*)(c+0x3f6) != 0) {
        func_ov102_0214ae1c(c);
        return 1;
    }
    if (func_ov102_0214ab1c(c)) {
        return 1;
    }
    if (func_ov102_0214aa18(c)) {
        return 1;
    }

    r0 = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c+0x144, c+0x300, 0);
    if (r0 != 0) {
        if (r0 == 2) {
            func_ov102_0214ae1c(c);
        }
        return 1;
    }

    if (*(int*)(c+0x10c) != 0) {
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(c, c+0x144);
        func_ov102_0214b128(c);
        flag = (*(int*)(c+0xb0) & 0x100) != 0;
        if (flag != 0) {
            *(int*)(c+0x10c) = 0;
        } else if (*(int*)(c+0x10c) != 0) {
            func_ov102_0214b53c(c);
            _ZN12CylinderClsn5ClearEv(c+0x110);
            _ZN12CylinderClsn6UpdateEv(c+0x110);
            return 1;
        }
    }

    func_ov102_0214b03c(c);
    if (*(int*)(c+0x3dc) == 5) {
        return 1;
    }

    if (*(int*)(c+0x9c) != 0) {
        if ((*(int*)(c+0x130) & 0x10) != 0) {
            short v[3];
            other = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0x134));
            v[0] = -0x2000;
            v[1] = 0;
            v[2] = 0;
            func_020ada40(c, v, other, 0x32000);
            _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x110);
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x144, 0);
            _ZN12CylinderClsn5ClearEv(c+0x110);
            return 1;
        }

        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x110);
        if (LEVEL_ID == 6 && STAR_ID == 3) {
            if (*(int*)(c+0x98) == 0x5000) {
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x144, 3);
            } else {
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x144, 2);
            }
        } else {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c+0x144, 2);
        }

        if (_ZNK12WithMeshClsn10IsOnGroundEv(c+0x144)) {
            if (func_02037e78((int*)((char*)_ZNK12WithMeshClsn14GetFloorResultEv(c+0x144)+4))) {
                func_ov102_0214ae1c(c);
                return 1;
            }
            if (_ZNK12WithMeshClsn8IsOnWallEv(c+0x144) && *(int*)(c+0x3dc) == 0) {
                func_ov102_0214beb4(c);
            }
        }
    }

    r0 = func_ov102_0214b248(c);
    if (r0 == 0) {
        return 0;
    }

    if (*(unsigned int*)(c+0x134) != 0) {
        if ((*(int*)(c+0x130) & 0x4000) != 0) {
            func_ov102_0214b384(c, 4);
        }
        if (*(int*)(c+0x3dc) == 4) {
            unsigned char b = *(unsigned char*)(c+0x3f5);
            if (b == 2 || b == 3) {
                other = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0x134));
                if (other != 0) {
                    int flag2 = (*(unsigned short*)((char*)other+0xc) == 0xbd);
                    if (flag2) {
                        func_ov102_0214b384(c, 2);
                        *(int*)(c+0x128) |= 0x4000;
                    }
                }
            }
        }
    }

    func_ov102_0214b53c(c);
    _ZN12CylinderClsn5ClearEv(c+0x110);
    _ZN12CylinderClsn6UpdateEv(c+0x110);
    func_ov102_0214ad40(c);
    return 1;
}
