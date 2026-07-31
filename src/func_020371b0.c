#include "types.h"
struct Actor {
    char pad[0xa4];
    int speed_x; /* 0xa4 */
    int speed_y; /* 0xa8 */
    int speed_z; /* 0xac */
};

struct WithMeshClsn {
    char pad[0x10];
    u32 flags;   /* 0x10 */
    struct Actor *actor; /* 0x14 */
};

struct ClsnResult;

extern void func_0203573c(struct WithMeshClsn *clsn);
extern struct ClsnResult *_ZNK12WithMeshClsn14GetFloorResultEv(struct WithMeshClsn *clsn);
extern void func_02038234(struct ClsnResult *result, struct Actor *actor);
extern void _ZN12WithMeshClsn13SetGroundFlagEv(struct WithMeshClsn *clsn);
extern u32 _ZNK12WithMeshClsn13GetLimMovFlagEv(struct WithMeshClsn *clsn);

void func_020371b0(struct WithMeshClsn *clsn, s32 justHit)
{
    struct ClsnResult *floorResult;
    if (justHit == 0)
        func_0203573c(clsn);
    floorResult = _ZNK12WithMeshClsn14GetFloorResultEv(clsn);
    func_02038234(floorResult, clsn->actor);
    _ZN12WithMeshClsn13SetGroundFlagEv(clsn);
    if (_ZNK12WithMeshClsn13GetLimMovFlagEv(clsn) == 0)
        clsn->actor->speed_y = 0;
}
