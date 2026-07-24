//cpp
struct CylinderClsn;
struct WithMeshClsn {
    int IsOnGround() const;
    int JustHitGround() const;
    int IsOnWall() const;
};
struct Actor {
    void UpdatePosWithHorzSpeedAndAng();
    void UpdatePosWithOnlySpeed(CylinderClsn*);
    short ReflectAngle(int, int, short);
};
struct Enemy : Actor {
    void UpdateWMClsn(WithMeshClsn&, unsigned int);
};
extern "C" int Vec3_HorzLen(void*);

extern "C" void func_ov002_020aefb8(char* self);
void func_ov002_020aefb8(char* self) {
    int *px;
    int *pz;
    ((Actor*)self)->UpdatePosWithHorzSpeedAndAng();
    if (((WithMeshClsn*)(self + 0x144))->IsOnGround()) {
        px = (int*)(int)(((long long)(int)(self + 0xa4)) & 0xFFFFFFFFFFFFFFFFLL);
        *px += *(int*)(self + 0xd4) * 0xa;
        pz = (int*)(int)(((long long)(int)(self + 0xac)) & 0xFFFFFFFFFFFFFFFFLL);
        *pz += *(int*)(self + 0xdc) * 0xa;
        if (((WithMeshClsn*)(self + 0x144))->JustHitGround()) {
            *(int*)(self + 0xa8) = -(*(int*)(self + 0xa8) << 2) / 10;
        } else {
            *(int*)(self + 0xa8) = 0;
        }
        if (Vec3_HorzLen(self + 0xa4) > *(int*)(self + 0x98)) {
            *(int*)(self + 0x98) = Vec3_HorzLen(self + 0xa4);
            if (*(int*)(self + 0x98) >= 0xf000) *(int*)(self + 0x98) = 0xf000;
        }
    }
    ((Actor*)self)->UpdatePosWithOnlySpeed((CylinderClsn*)(self + 0x110));
    ((Enemy*)self)->UpdateWMClsn(*(WithMeshClsn*)(self + 0x144), 0);
    if (!((WithMeshClsn*)(self + 0x144))->IsOnWall()) return;
    *(short*)(self + 0x94) = ((Actor*)self)->ReflectAngle(*(int*)(self + 0xe0), *(int*)(self + 0xe8), *(short*)(self + 0x94));
}
