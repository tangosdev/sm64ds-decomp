typedef unsigned int u32;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* self, void* c);
extern void func_ov060_02116518(char* self, int a, int b, int c);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 a, struct Vector3* pos, struct Vector3_16* rot, int e, int f);
extern void _ZN9ActorBase18MarkForDestructionEv(char* self);

void func_ov060_021169f8(char* sl)
{
    int i;
    char* a;
    if (*(int*)(sl+0x360) < 0x5000) {
        *(int*)(((int)sl + 0x360) & 0xFFFFFFFFFFFFFFFF) += 0x200;
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(sl, 0);
    func_ov060_02116518(sl, 0x9a, 1, 0x78000);
    if (*(unsigned short*)(sl+0x374) <= 0x14)
        return;
    if (*(unsigned char*)(sl+0x378) == 0) {
        for (i = 0; i < 3; i++) {
            a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 2, (struct Vector3*)(sl+0x5c), (struct Vector3_16*)(sl+0x92), *(signed char*)(sl+0xcc), -1);
            *(int*)(a+0x360) = 0x5000;
        }
    } else {
        a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 0x12, (struct Vector3*)(sl+0x5c), (struct Vector3_16*)(sl+0x92), *(signed char*)(sl+0xcc), -1);
        *(int*)(a+0x360) = 0x8000;
        a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 0x22, (struct Vector3*)(sl+0x5c), (struct Vector3_16*)(sl+0x92), *(signed char*)(sl+0xcc), -1);
        *(int*)(a+0x360) = 0x8000;
    }
    _ZN9ActorBase18MarkForDestructionEv(sl);
}
