extern void func_ov060_02116518(void *c, int a, int b, int d);
extern void func_ov060_0211712c(void *c);
extern int func_ov060_021172c8(void *c, unsigned int n);
extern void _ZN9ActorBase18MarkForDestructionEv(void *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *clsn);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);
extern int data_ov060_02119358[];

void func_ov060_02116b68(char *c)
{
    func_ov060_02116518(c, data_ov060_02119358[*(unsigned char*)(c + 0x378)], 0, 0x46000);
    func_ov060_0211712c(c);
    if (func_ov060_021172c8(c, 0x1c2))
        _ZN9ActorBase18MarkForDestructionEv(c);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110)) {
        if (*(unsigned char*)(c + 0x378) == 0)
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x118, 7, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
        else
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x118, 3, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
        _ZN9ActorBase18MarkForDestructionEv(c);
    } else {
        if (*(int*)(c + 0x60) >= 0) return;
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
}
