//cpp
struct CylinderClsn { void Clear(); void Update(); };
struct WithMeshClsn;
struct Actor { void UpdatePos(CylinderClsn *c); };
struct Enemy { void UpdateWMClsn(WithMeshClsn &w, unsigned int j); };
struct WithMeshClsn2 { int JustHitGround() const; int IsOnGround() const; };
extern "C" void func_ov102_0214b53c(char *c);
extern "C" void func_ov102_0214ad40(char *c);
extern "C" void func_ov102_0214c0b8(char *c);
extern "C" void func_0200fc44(char *c, void *v, int x);

struct V3 { int x, y, z; };

extern "C" int func_ov102_0214aa18(Actor *self)
{
    char *s = (char*)self;
    if (*(unsigned char*)(s + 0x3f5) == 2) {
        if (*(int*)(s + 0xc8) != 0) {
            func_ov102_0214b53c(s);
            return 1;
        }
        self->UpdatePos((CylinderClsn*)(s + 0x110));
        ((Enemy*)self)->UpdateWMClsn(*(WithMeshClsn*)(s + 0x144), 0);
        func_ov102_0214b53c(s);
        func_ov102_0214ad40(s);
        ((CylinderClsn*)(s + 0x110))->Clear();
        ((CylinderClsn*)(s + 0x110))->Update();
        if (((WithMeshClsn2*)(s + 0x144))->JustHitGround()) {
            V3 v;
            v.x = *(int*)(s + 0x5c);
            v.y = *(int*)(s + 0x60);
            v.z = *(int*)(s + 0x64);
            func_0200fc44(s, &v, 1);
        }
        if (((WithMeshClsn2*)(s + 0x144))->IsOnGround() == 0)
            return 1;
        int *fl = (int*)(((int)s + 0x128) & 0xFFFFFFFFFFFFFFFF);
        *fl = *fl & ~2;
        *(unsigned char*)(s + 0x3f5) = 3;
        func_ov102_0214c0b8(s);
        return 1;
    }
    return 0;
}
