typedef unsigned int u32;
typedef int Fix12i;

extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void* a, unsigned short* p);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* w);
extern void _ZN5Actor11LandingDustEb(void* a, int b);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* w);
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player15JumpIntoBooCageER7Vector3(void* p, void* v);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 eff, Fix12i x, Fix12i y, Fix12i z, void* rot, void* cb);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* a, void* wm, u32 n);
extern void func_ov063_021169c4(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* cl);
extern void _ZN12CylinderClsn6UpdateEv(void* cl);

int _ZN7BooCage8BehaviorEv(char* self)
{
    int r4 = 0;
    int r6 = 1;
    void* player;
    if (*(unsigned char*)(self + 0x37e) == 0)
        r6 = _ZN5Sound15PlaySecretSoundEP5ActorPt(self, (unsigned short*)(self + 0x37c));
    if (_ZNK12WithMeshClsn13JustHitGroundEv(self + 0x144) != 0) {
        *(int*)(self + 0xa8) = (-*(int*)(self + 0xa8)) >> 1;
        _ZN5Actor11LandingDustEb(self, 0);
    } else if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x144) != 0) {
        r4 = 1;
        if (r6 != 0 || *(unsigned short*)(self + 0x37c) > 0x3c) {
            if (*(u32*)(self + 0x134) != 0) {
                player = _ZN5Actor10FindWithIDEj(*(u32*)(self + 0x134));
                if (player != 0) {
                    if ((*(int*)(self + 0x130) & 0x400000) != 0) {
                        _ZN6Player15JumpIntoBooCageER7Vector3(player, self + 0x5c);
                    }
                }
            }
        }
    }
    if (r4 == 0) {
        *(u32*)(self + 0x378) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(self + 0x378), 0x119,
            *(int*)(self + 0x5c), *(int*)(self + 0x60) + 0x64000, *(int*)(self + 0x64),
            0, 0);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, self + 0x144, 0);
    func_ov063_021169c4(self);
    _ZN12CylinderClsn5ClearEv(self + 0x110);
    _ZN12CylinderClsn6UpdateEv(self + 0x110);
    return 1;
}
