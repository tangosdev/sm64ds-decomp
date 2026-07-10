//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void func_0200f760(void* a, void* b);
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* thiz, void* w);
extern void func_ov020_0211216c(void* c);
extern void func_ov020_02112080(void* c);
extern void func_ov020_0211174c(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* thiz);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void* thiz, const Vector3& v);
extern void _ZN12CylinderClsn6UpdateEv(void* thiz);

int _ZN8BookShot8BehaviorEv(char* c)
{
    func_0200f760(c, c + 0x21c);
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, c + 0x25c) != 0) {
        if (*(unsigned char*)(c + 0x107) != 0 && *(unsigned short*)(c + 0x104) == 5) {
            *(int*)(c + 0x428) = *(int*)(c + 0x424);
            *(int*)(c + 0x424) = 5;
            *(unsigned char*)(c + 0x107) = 0;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0x8000;
        }
        func_ov020_0211216c(c);
        return 1;
    }
    switch (*(int*)(c + 0x420)) {
    case 0:
        func_ov020_02112080(c);
        break;
    case 1:
        func_ov020_0211174c(c);
        break;
    }
    func_ov020_0211216c(c);
    _ZN12CylinderClsn5ClearEv(c + 0x21c);
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x21c, *(Vector3*)(c + 0x438));
    _ZN12CylinderClsn6UpdateEv(c + 0x21c);
    return 1;
}
}
