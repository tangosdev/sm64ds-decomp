struct Vec3 { int x, y, z; };
extern void AddVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *out);
extern void func_ov006_020c49d8(void *this);
void func_ov006_020c61c4(int this) {
    int f24 = *(int*)(this + 0x24);
    int a0  = *(int*)(this + 0xa0);
    if (a0 < f24 + *(int*)(this + 0x18) && ((int*)this)[43] < 0) {
        ((int*)this)[43] = -((int*)this)[43];
        ((short*)this)[117] = 0;
    } else if (a0 > f24 + *(int*)(this + 0x1c) && ((int*)this)[43] > 0) {
        ((int*)this)[43] = -((int*)this)[43];
        ((short*)this)[117] = 0;
    }
    AddVec3((struct Vec3*)(this + 0x9c), (struct Vec3*)(this + 0xa8), (struct Vec3*)(this + 0x9c));
    func_ov006_020c49d8((void*)this);
}
