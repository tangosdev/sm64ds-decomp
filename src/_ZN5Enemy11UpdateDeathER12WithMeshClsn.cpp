//cpp
struct WithMeshClsn;
struct Enemy;
typedef int (Enemy::*PMF)(WithMeshClsn&);
extern PMF data_ov002_0210dbc0[];
extern "C" {
typedef unsigned int u32;
extern void DecIfAbove0_Short(short *p);
extern int _ZN5Actor9UpdatePosEP12CylinderClsn(Enemy *thiz, void *clsn);
extern int _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *clsn, u32 sel);
}
struct Enemy { char pad[0x200]; };
extern "C" int _ZN5Enemy11UpdateDeathER12WithMeshClsn(Enemy *thiz, WithMeshClsn *clsn)
{
    int ret;
    if (*(int*)((char*)thiz+0x10c) == 0)
        return 0;
    DecIfAbove0_Short((short*)((char*)thiz+0x102));
    ret = (thiz->*data_ov002_0210dbc0[*(int*)((char*)thiz+0x10c)-1])(*clsn);
    _ZN5Actor9UpdatePosEP12CylinderClsn(thiz, 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(thiz, clsn, 0);
    return ret;
}
