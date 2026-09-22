//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *);
extern int SurfaceInfo_TestFlag0x20(int *p);
int func_ov098_02138bb8(char *c)
{
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x320))
    {
        if (SurfaceInfo_TestFlag0x20((int *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(c + 0x320) + 4)))
            return 1;
    }
    return 0;
}
}
