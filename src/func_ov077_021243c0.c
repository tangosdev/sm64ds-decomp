//cpp
typedef int Fix12i;
struct V3 { Fix12i x, y, z; };
extern "C" {
int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, struct V3*, void*, int, int);
void func_0201267c(int a, void* p);
int _ZN9Animation8FinishedEv(void* self);
void func_ov077_0212478c(char* c, int a);
void func_ov077_0212390c(char* c);
void func_ov077_02123814(char* c);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov077_02123a74(char* c);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* self, struct V3* v);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int d);
extern int data_ov077_02127b88[];

int func_ov077_021243c0(char* c){
    if ((((unsigned int)*(int*)(c + 0x12c) << 4) >> 16) == 0x3a) {
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x104, 0, (struct V3*)(c + 0x404), (void*)(c + 0x8c),
            *(signed char*)(c + 0xcc), -1);
        func_0201267c(0xd2, c + 0x74);
    }
    if (_ZN9Animation8FinishedEv(c + 0x124)) {
        func_ov077_0212478c(c, 0);
    }
    func_ov077_0212390c(c);
    func_ov077_02123814(c);
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN9Animation7AdvanceEv(c + 0x1b0);
    func_ov077_02123a74(c);
    {
        int* d = data_ov077_02127b88;
        int off = *(int*)(c + 0x414);
        int z = d[2];
        int y = d[1] + off;
        int x = d[0];
        struct V3 v;
        v.x = x; v.y = y; v.z = z;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1c4, &v);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x1c4);
    _ZN12CylinderClsn6UpdateEv(c + 0x1c4);
    *(int*)(c + 0x410) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(c + 0x410), 3, 0x182, c + 0x74, 0);
    return 1;
}
}
