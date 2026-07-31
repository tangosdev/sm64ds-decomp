#include "types.h"
// @symbol func_ov072_02120a44
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daBgSnwmn_c.h"
// @emits daBgSnwmn_c_InitResources
/* recovered: renamed to Class_Method */
/* daBgSnwmn_c::InitResources - recovered from vtable slot identity */
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, void *pos, void *ang, int a, int b);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, u32 u);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int f1, int f2, u32 u1, u32 u2);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);

extern int data_ov072_02122c70[];

int daBgSnwmn_c_InitResources(char *c)
{
    struct daBgSnwmn_c *self = (struct daBgSnwmn_c *)(void *)c;
    char rg[0x50];
    int v[3];
    void *m;

    if (IsStarCollectedInLevel(0xa, 5) == 0)
    {
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x111, 0, c + 0x5c, c + 0x8c, self->unk_0cc, -1);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, 1);
    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov072_02122c40);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x124, m, 1, 1);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov072_02122c50);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void *)data_ov072_02122c48[1], (void *)data_ov072_02122c50[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x174, (void *)data_ov072_02122c50[1], 0, 0x1000, 0);

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x188) == 0)
        return 0;

    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x1b0, c, data_ov072_02122c70, 0xc3000, 0x17c000, 0x800004, 0);

    v[0] = self->unk_05c;
    v[1] = self->unk_060;
    v[2] = self->unk_064;
    v[1] += 0x14000;
    _ZN13RaycastGroundC1Ev(rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(rg))
        self->unk_060 = *(int *)(rg + 0x44);
    else
        self->unk_060 = v[1];
    (*(int *)(((int)c + 0x60))) += 0xc3000;
    self->unk_09c = 0;
    self->unk_0a0 = 0;
    self->unk_080 = 0x1800;
    self->unk_084 = 0x1800;
    self->unk_088 = 0x1800;
    func_ov072_021208d8(c);
    _ZN13RaycastGroundD1Ev(rg);
    return 1;
}
