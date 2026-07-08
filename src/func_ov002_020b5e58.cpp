//cpp
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void func_ov002_020b5b98(char *t);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
extern "C" void func_020393c4(void *p, void *v);
extern "C" void func_020393d4(void *p, void *v);

struct V3 { int x, y, z; };
struct RaycastGround { char buf[0x44]; int f44; char rest[8]; };
extern "C" void _ZN13RaycastGroundC1Ev(RaycastGround *self);
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *self, V3 *v, void *a);
extern "C" int _ZN13RaycastGround10DetectClsnEv(RaycastGround *self);
extern "C" void _ZN13RaycastGroundD1Ev(RaycastGround *self);

extern char func_ov002_020b5fc4;
extern char _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern "C" signed char LEVEL_ID;

extern "C" int func_ov002_020b5e58(char *self, char *fp)
{
    RaycastGround rg;
    V3 v;
    BMD_File *bmd;
    KCL_File *kcl;
    int vy, vz, vx;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(**(SharedFilePtr **)fp);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    *(char **)(self + 0x32c) = fp;
    func_ov002_020b5b98(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(**(SharedFilePtr **)(fp + 4));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, self + 0x2ec, 0x199, *(short *)(self + 0x8e), *(void **)(fp + 8));
    func_020393c4(self + 0x124, &func_ov002_020b5fc4);
    func_020393d4(self + 0x124, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    *(int *)(self + 0x330) = 0;
    *(int *)(self + 0x334) = 0;
    *(short *)(self + 0x338) = 0;
    *(int *)(self + 0x320) = *(int *)(self + 0x5c);
    *(int *)(self + 0x324) = *(int *)(self + 0x60);
    *(int *)(self + 0x328) = *(int *)(self + 0x64);
    *(int *)(self + 0x33c) = 0;
    *(char *)(self + 0x340) = 0;
    *(int *)(self + 0x344) = *(int *)(self + 0x60);

    if (LEVEL_ID == 0x15) {
        _ZN13RaycastGroundC1Ev(&rg);
        vy = *(int *)(self + 0x60);
        vz = *(int *)(self + 0x64);
        vx = *(int *)(self + 0x5c);
        {
            int t = vy + 0x50000;
            v.x = vx;
            v.y = t;
            v.z = vz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &v, self);
        if (_ZN13RaycastGround10DetectClsnEv(&rg)) {
            *(int *)(self + 0x344) = rg.f44 + 0x3e000;
        }
        _ZN13RaycastGroundD1Ev(&rg);
    }
    return 1;
}
