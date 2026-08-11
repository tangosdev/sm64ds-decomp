//cpp
// @symbol _ZN11SnowmanHead13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SnowmanHead.h"
typedef int Fix12;

struct BMD_File;
struct BTP_File;
struct TextureSequence { static void Prepare(BMD_File &model, BTP_File &animFile); };

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *base, void *file, int a, int b);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *ref);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *t, void *a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *t, void *a, Fix12 b, Fix12 c, void *d, void *e);
extern void _ZN13RaycastGroundC1Ev(void *t);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *t, const struct Vector3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *t);
extern void func_ov072_0211ffd8(char *c);
extern void _ZN13RaycastGroundD1Ev(void *t);

}

int SnowmanHead::InitResources()
{
    struct Vector3 pos;
    char ray[0x54];
    int i;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov072_02122bc4), 1, -1);

    for (i = 0; i < 2; i++) {
        void *tex = (&data_ov072_02121ffc)[i];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(tex);
        TextureSequence::Prepare(*(BMD_File *)((void **)&data_ov072_02122bc4)[1],
                                 *(BTP_File *)((void **)tex)[1]);
    }

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x138, ((char *)this), 0x96000, 0x12c000, 0x800004, 0);
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x16c, ((char *)this), 0x96000, 0x96000, 0, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    _ZN13RaycastGroundC1Ev(ray);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(ray) != 0)
        mPosY = *(int *)(ray + 0x44);
    else
        mPosY = pos.y;

    func_ov072_021205d4(((char *)this), 0);
    func_ov072_0211ffd8(((char *)this));
    _ZN13RaycastGroundD1Ev(ray);
    return 1;
}
