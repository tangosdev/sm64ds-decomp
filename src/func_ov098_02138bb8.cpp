//cpp
extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *);
extern int SurfaceInfo_TestFlag0x20(int *p);
int func_ov098_02138bb8(char *c)
{
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x320))
    {
        if (SurfaceInfo_TestFlag0x20((int *)((char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x320) + 4)))
            return 1;
    }
    return 0;
}
}
