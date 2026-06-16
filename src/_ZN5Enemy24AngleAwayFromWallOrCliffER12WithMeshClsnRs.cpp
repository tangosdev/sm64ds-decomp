//cpp
extern "C" {
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *clsn);
extern short _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
int _ZN5Enemy24AngleAwayFromWallOrCliffER12WithMeshClsnRs(void *thisp, void *clsn, short *outAngle)
{
    if (_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        *outAngle = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(thisp,
            *(int*)((char*)thisp+0xe0), *(int*)((char*)thisp+0xe8), *outAngle);
    } else if (*(unsigned char*)((char*)thisp+0x106)) {
        *outAngle = (short)(*(short*)((char*)thisp+0x94) + 0x8000);
    } else {
        return 0;
    }
    return 1;
}
}
