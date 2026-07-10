//cpp
struct Actor; typedef int (Actor::*PMF)();
struct WithMeshClsn;
struct CylinderClsn;

extern "C" {
int _ZN5Actor13DistToCPlayerEv(Actor* self);
void func_ov079_02123f34(Actor* self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(Actor* self, CylinderClsn* c);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(Actor* self, WithMeshClsn* w, int a, short b, int c, int d, void* e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Actor* self, WithMeshClsn* w, unsigned int n);
void func_ov079_02124188(Actor* self);
int func_ov079_021243e0(char* c, int r4);
int func_ov079_02123a8c(Actor* self);
void func_ov079_02124008(Actor* self);
}

extern int data_0209f318;
extern PMF data_ov079_02128280[];

extern "C" int _ZN5Whomp8BehaviorEv(Actor* self)
{
    char* c = (char*)self;

    if (*(unsigned char*)(c + 0x414) != 0 && *(int*)(c + 0x3b0) != 9) {
        if (_ZN5Actor13DistToCPlayerEv(self) < 0x1770000) {
            *(int*)(*(int*)&data_0209f318 + 0x114) = (int)self;
        }
    }

    func_ov079_02123f34(self);
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);

    if (*(int*)(c + 0x98) != 0) {
        if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(self, (WithMeshClsn*)(c + 0x110), 0x3c000, (short)0x2888, 0, 0, (void*)0x32000)) {
            *(int*)(c + 0x5c) = *(int*)(c + 0x3d4);
            *(int*)(c + 0x60) = *(int*)(c + 0x3d8);
            *(int*)(c + 0x64) = *(int*)(c + 0x3dc);
        } else {
            *(int*)(c + 0x3d4) = *(int*)(c + 0x5c);
            *(int*)(c + 0x3d8) = *(int*)(c + 0x60);
            *(int*)(c + 0x3dc) = *(int*)(c + 0x64);
        }
    } else {
        *(int*)(c + 0x3d4) = *(int*)(c + 0x5c);
        *(int*)(c + 0x3d8) = *(int*)(c + 0x60);
        *(int*)(c + 0x3dc) = *(int*)(c + 0x64);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, (WithMeshClsn*)(c + 0x110), 0);

    {
        int idx = *(int*)(c + 0x3b0);
        PMF* pmf = &data_ov079_02128280[idx];
        (self->**pmf)();

        {
            unsigned short* ctr = (unsigned short*)(c + 0x100);
            *ctr = *ctr + 1;
            if (idx != *(int*)(c + 0x3b0)) {
                *ctr = 0;
                *(unsigned char*)(c + 0x40c) = 0;
            }
        }
    }

    func_ov079_02124188(self);

    if (func_ov079_021243e0((char*)self, 0) == 0 || func_ov079_02123a8c(self) != 0) {
        func_ov079_02124008(self);
    }

    *(unsigned char*)(c + 0x403) = 0;
    return 1;
}
