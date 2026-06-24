//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern int _ZN4cstd4fdivEii(int a, int b);
extern int Vec3_HorzLen(void* v);
extern short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int b, unsigned int e);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* thiz);
extern void _ZN12WithMeshClsn12Unk_0203589cEv(void* thiz);
extern void _ZN12WithMeshClsn22ClearJustHitGroundFlagEv(void* thiz);
extern void _ZN12WithMeshClsn15ClearGroundFlagEv(void* thiz);
extern void func_02012694(int a, void* v);
extern int data_ov084_02130cc0[];

void func_ov084_02129168(char* c, char* actor)
{
    *(short*)(c + 0x452) = 0x3c;
    *(int*)(c + 0xa8) = _ZN4cstd4fdivEii(0xd000 - *(int*)(c + 0x9c), *(int*)(c + 0xd8));
    *(int*)(c + 0x98) = Vec3_HorzLen(c + 0xa4) * -1;
    if (actor != 0)
        *(short*)(c + 0x94) = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)(actor + 0x5c));
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x370, *(void**)((char*)data_ov084_02130cc0 + 4), 0, 0x1000, 0);
    *(int*)(c + 0x434) = 3;
    *(unsigned char*)(c + 0x107) = 0;
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x1b4);
    _ZN12WithMeshClsn12Unk_0203589cEv(c + 0x1b4);
    _ZN12WithMeshClsn22ClearJustHitGroundFlagEv(c + 0x1b4);
    _ZN12WithMeshClsn15ClearGroundFlagEv(c + 0x1b4);
    func_02012694(0x13a, c + 0x74);
    *(unsigned char*)(c + 0x467) = 0;
}
}
