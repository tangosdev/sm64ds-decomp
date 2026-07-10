//cpp
/* func_ov102_0214d2e8 @ 0x0214d2e8 (ov102, size 0x264)
 * Enemy main behavior dispatcher: yoshi-eat handling, state PMF call,
 * collision update and ground/wall reactions.
 */
typedef int s32;
typedef short s16;
typedef unsigned char u8;
typedef unsigned int u32;

struct C {};
typedef int (C::*PMF)();
struct EState {
    void *pad0;
    void *pad4;
    PMF fn; /* +0x8 */
};

extern char data_ov102_0214ea48;
extern char data_ov102_0214ea58;
extern char data_ov102_0214ea68;
extern char data_ov102_0214ea78;

extern "C" {
extern int func_ov002_020ad660(char *, char *, char *, int);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(char *, char *);
extern void func_ov102_0214d1f8(char *, void *);
extern void func_ov102_0214ce60(char *);
extern void _ZN12CylinderClsn5ClearEv(char *);
extern int DecIfAbove0_Byte(char *);
extern void _ZN9ActorBase18MarkForDestructionEv(char *);
extern int DecIfAbove0_Short(char *);
extern void func_ov102_0214cbec(char *);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char *, char *);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(char *, char *, u32);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *);
extern int _ZNK12WithMeshClsn8IsOnWallEv(char *);
extern void _ZN5Actor8PoofDustEv(char *);
extern void func_ov102_0214c7fc(char *);
extern void func_ov102_0214c84c(char *);
extern void _ZN12CylinderClsn6UpdateEv(char *);

int func_ov102_0214d2e8(char *c)
{
    if (func_ov002_020ad660(c, c + 0x144, c + 0x300, 3) != 0)
        return 1;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x144) != 0) {
        if (*(u8 *)(c + 0x107) != 0) {
            *(s16 *)(c + 0x3bc) = *(s16 *)(c + 0x94);
            func_ov102_0214d1f8(c, &data_ov102_0214ea78);
            *(u32 *)(int)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80000;
            *(u8 *)(c + 0x107) = 0;
        }
        func_ov102_0214ce60(c);
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        return 1;
    }

    if (*(u8 *)(c + 0x3c6) != 0 &&
        *(void **)(c + 0x3ac) == (void *)&data_ov102_0214ea68) {
        if (DecIfAbove0_Byte(c + 0x3c6) == 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return 1;
        }
    } else {
        *(u8 *)(c + 0x3c6) = 0;
    }

    DecIfAbove0_Short(c + 0x100);

    {
        EState *st = *(EState **)(c + 0x3ac);
        int res;
        if (*(int *)&st->fn == 0)
            res = 1;
        else
            res = (((C *)c)->*st->fn)();
        if (res == 0)
            return 1;
    }

    func_ov102_0214cbec(c);

    if (*(u32 *)(c + 0x9c) != 0) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0 ||
            _ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0) {
            if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x144) != 0) {
                if (*(void **)(c + 0x3ac) == (void *)&data_ov102_0214ea78) {
                    _ZN5Actor8PoofDustEv(c);
                    _ZN9ActorBase18MarkForDestructionEv(c);
                    return 0;
                }
            }
            if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) != 0) {
                if (*(void **)(c + 0x3ac) == (void *)&data_ov102_0214ea68) {
                    *(u32 *)(c + 0x9c) = 0;
                    *(u32 *)(c + 0xa8) = 0;
                }
                if (*(void **)(c + 0x3ac) != (void *)&data_ov102_0214ea48 &&
                    *(void **)(c + 0x3ac) != (void *)&data_ov102_0214ea58)
                    func_ov102_0214c7fc(c);
            }
        }
    }

    if (*(u8 *)(c + 0x3c4) == 0)
        func_ov102_0214c84c(c);
    func_ov102_0214ce60(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    if (*(void **)(c + 0x3ac) != (void *)&data_ov102_0214ea48)
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}
}
