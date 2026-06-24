//cpp
struct CylinderClsn { void Clear(); void Update(); };
struct WithMeshClsn { int JustHitGround() const; int IsOnGround() const; };
struct Actor { void UpdatePos(CylinderClsn *cc); };

extern "C" void func_02038420(WithMeshClsn *w);
extern "C" void func_ov080_02124acc(char *c);

extern "C" int func_ov080_0212500c(char *c)
{
    func_02038420((WithMeshClsn *)(c + 0x180));
    if (((WithMeshClsn *)(c + 0x180))->JustHitGround()) {
        int v = *(int *)(c + 0xa8) * -0x3c;
        *(int *)(c + 0xa8) = v / 100;
    } else if (((WithMeshClsn *)(c + 0x180))->IsOnGround()) {
        *(int *)(c + 0xa8) = 0xc000;
    }
    ((Actor *)c)->UpdatePos((CylinderClsn *)(c + 0x14c));
    func_ov080_02124acc(c);
    ((CylinderClsn *)(c + 0x14c))->Clear();
    ((CylinderClsn *)(c + 0x14c))->Update();
    return 1;
}
