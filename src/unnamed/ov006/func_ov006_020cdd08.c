typedef struct { int x, y, z; } Vec3;
extern void func_020731dc(int a, int b, void **node);
extern void func_0203cc28(int *p, int angle);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void func_0203cf00(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Quaternion_FromVector3(int* q, Vec3* axis, Vec3* v);
extern void Quaternion_Normalize(int *q);
extern int func_020072c0(void);

extern int data_ov006_02140824;
extern Vec3 data_ov006_021408fc;
extern int data_ov006_021408f0;
extern int data_ov006_0214080c;
extern Vec3 data_ov006_02140914;
extern int data_ov006_02140908;

void func_ov006_020cdd08(char* self)
{
    Vec3 a, b, c, d;

    if (!(data_ov006_02140824 & 1)) {
        data_ov006_021408fc.x = 0;
        data_ov006_021408fc.y = 0x1000;
        data_ov006_021408fc.z = 0;
        func_020731dc((int)&data_ov006_021408fc, (int)&func_020072c0, (void**)&data_ov006_021408f0);
        data_ov006_02140824 |= 1;
    }
    if (!(data_ov006_0214080c & 1)) {
        data_ov006_02140914.x = 0;
        data_ov006_02140914.y = 0;
        data_ov006_02140914.z = 0x1000;
        func_020731dc((int)&data_ov006_02140914, (int)&func_020072c0, (void**)&data_ov006_02140908);
        data_ov006_0214080c |= 1;
    }
    func_0203cc28((int*)(self + 0x38), 0x100);
    func_0203ce80(&a, (Vec3*)(self + 0x38));
    func_0203cf00(&b, (Vec3*)(self + 0x38), &data_ov006_02140914);
    *(int*)(self + 0x44) = b.x;
    *(int*)(self + 0x48) = b.y;
    *(int*)(self + 0x4c) = b.z;
    func_0203ce80(&c, (Vec3*)(self + 0x44));
    Quaternion_FromVector3((int*)(self + 0x74), &data_ov006_021408fc, (Vec3*)(self + 0x38));
    Quaternion_Normalize((int*)(self + 0x74));
}
