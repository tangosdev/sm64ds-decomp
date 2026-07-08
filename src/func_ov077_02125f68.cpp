//cpp
extern "C" {
int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void* c, int d);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN9ActorBase18MarkForDestructionEv(void* c);
void func_ov077_02124c28(void* c);
void func_ov077_02125e20(void* c);
void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void* c, void* cyl);
void func_ov077_02125304(char* c);
void _ZN5Actor8PoofDustEv(void* c);
void func_02012694(int a, void* p);

extern signed char LEVEL_ID;

int func_ov077_02125f68(char* c)
{
    int s = *(int*)(c + 0x3d8);
    if (s != 1 || _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1e4)) {
        s = *(int*)(c + 0x3d8);
        if (s != 4 && s != 5 && _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000)) {
            if (DecIfAbove0_Byte((unsigned char*)(c + 0x3e9)) == 0) {
                _ZN9ActorBase18MarkForDestructionEv(c);
                return 1;
            }
            goto done;
        }
    }
    func_ov077_02124c28(c);
    func_ov077_02125e20(c);
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x1b0);
    func_ov077_02125304(c);
    if (LEVEL_ID == 0x1c && *(int*)(c + 0x60) <= -0x1600000) {
        _ZN5Actor8PoofDustEv(c);
        func_02012694(0xc4, c + 0x74);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
done:
    return 1;
}
}
