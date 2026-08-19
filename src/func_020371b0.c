#include "types.h"
struct dActor_c {
    char pad[0xa4];
    int speed_x; /* 0xa4 */
    int speed_y; /* 0xa8 */
    int speed_z; /* 0xac */
};

struct dBgCh_Actr {
    char pad[0x10];
    u32 flags;   /* 0x10 */
    struct dActor_c *actor; /* 0x14 */
};

struct dBgPi;

extern void func_0203573c(struct dBgCh_Actr *clsn);
extern struct dBgPi *_ZNK10dBgCh_Actr14GetFloorResultEv(struct dBgCh_Actr *clsn);
extern void func_02038234(struct dBgPi *result, struct dActor_c *actor);
extern void _ZN10dBgCh_Actr13SetGroundFlagEv(struct dBgCh_Actr *clsn);
extern u32 _ZNK10dBgCh_Actr13GetLimMovFlagEv(struct dBgCh_Actr *clsn);

void func_020371b0(struct dBgCh_Actr *clsn, s32 justHit)
{
    struct dBgPi *floorResult;
    if (justHit == 0)
        func_0203573c(clsn);
    floorResult = _ZNK10dBgCh_Actr14GetFloorResultEv(clsn);
    func_02038234(floorResult, clsn->actor);
    _ZN10dBgCh_Actr13SetGroundFlagEv(clsn);
    if (_ZNK10dBgCh_Actr13GetLimMovFlagEv(clsn) == 0)
        clsn->actor->speed_y = 0;
}
