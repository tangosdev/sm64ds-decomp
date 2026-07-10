//cpp
/* _ZN4Coin8BehaviorEv @ 0x020b2324 (ov002, size 0x1e4)
 * Coin main behavior: pickup-flag sound, magnet flag, physics helpers,
 * state PMF-table dispatch, collision clear/update paths.
 */
struct C {};
typedef void (C::*PMF)();

extern PMF data_ov002_0210dc70[];
extern unsigned char data_0209f2d8;

extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, char *pos);
extern int func_ov002_020b10a0(char *c);
extern int func_ov002_020b12ec(char *c);
extern void func_ov002_020b14d8(char *c);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern void func_ov002_020b10e4(char *c);
extern int func_ov002_020b19dc(char *c);
extern int LenVec3(char *v);
extern void _ZN12CylinderClsn6UpdateEv(char *c);

int _ZN4Coin8BehaviorEv(char *c)
{
    if ((unsigned int)(*(unsigned char *)(c + 0x3ae) << 0x1e) >> 0x1f) {
        *(unsigned char *)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFF) &= ~2;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x30, c + 0x74);
    }
    if ((int)(*(unsigned short *)(c + 0xc) == 0x122) != 0) {
        if ((unsigned int)(*(unsigned char *)(c + 0x3ae) << 0x1f) >> 0x1f)
            *(char *)(c + 0xcc) = -1;
    }
    if (func_ov002_020b10a0(c) != 0) return 1;
    *(short *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF) += 0xc00;
    if (func_ov002_020b12ec(c) != 0) {
        func_ov002_020b14d8(c);
        _ZN12CylinderClsn5ClearEv(c + 0x178);
        return 1;
    }
    func_ov002_020b10e4(c);
    *(int *)(c + 0xd0) = 0;
    if (func_ov002_020b19dc(c) != 0) return 1;
    (((C *)c)->*data_ov002_0210dc70[*(int *)(c + 0x3a4)])();
    if ((int)(data_0209f2d8 == 1) == 0 && (int)((*(int *)(c + 0xb0) & 8) != 0) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x178);
        if (*(unsigned char *)(c + 0x3aa) == 0 && LenVec3(c + 0x74) < 0x64000) {
            if (*(int *)(c + 0x3a0) != 1 || *(unsigned char *)(c + 0x3b0) == 0)
                _ZN12CylinderClsn6UpdateEv(c + 0x178);
        }
    } else {
        func_ov002_020b14d8(c);
        _ZN12CylinderClsn5ClearEv(c + 0x178);
        if (*(unsigned char *)(c + 0x3aa) == 0) {
            if (*(int *)(c + 0x3a0) != 1 || *(unsigned char *)(c + 0x3b0) == 0)
                _ZN12CylinderClsn6UpdateEv(c + 0x178);
        }
    }
    return 1;
}
}
