typedef struct { int x, y, z; } Vec3;

extern int data_ov006_0214081c;
extern Vec3 data_ov006_021408a8;
extern void *data_ov006_0214089c;
extern int data_02092768[4];
extern int data_020a0ebc[3];
extern void func_020072c0(void);
extern void func_020731dc(int a, int b, void **node);
extern void func_0203cc28(int *p, int angle);
extern void Quaternion_FromVector3(int *q, Vec3 *a, Vec3 *b);
extern void func_ov006_020ce0ac(char *c);

void func_ov006_020ceabc(char *self, int *v1, int *v2, int a3, short a4)
{
    if ((data_ov006_0214081c & 1) == 0) {
        data_ov006_021408a8.x = 0;
        data_ov006_021408a8.y = 0x1000;
        data_ov006_021408a8.z = 0;
        func_020731dc((int)&data_ov006_021408a8, (int)func_020072c0, &data_ov006_0214089c);
        data_ov006_0214081c |= 1;
    }
    *(int *)(self + 0x8) = v1[0];
    *(int *)(self + 0xc) = v1[1];
    *(int *)(self + 0x10) = v1[2];
    *(int *)(self + 0x38) = v2[0];
    *(int *)(self + 0x3c) = v2[1];
    *(int *)(self + 0x40) = v2[2];
    {
        int *p38 = (int *)(int)(((long long)(int)(self + 0x38)));
        *(int *)(self + 0x44) = p38[0];
        *(int *)(self + 0x48) = p38[1];
        *(int *)(self + 0x4c) = p38[2];
    }
    func_0203cc28((int *)(self + 0x44), -0x4000);
    *(int *)(self + 0x14) = *(int *)(self + 0x8);
    *(int *)(self + 0x18) = *(int *)(self + 0xc);
    *(int *)(self + 0x1c) = *(int *)(self + 0x10);
    {
        int *p14 = (int *)(int)(((long long)(int)(self + 0x14)));
        *(int *)(self + 0x20) = p14[0];
        *(int *)(self + 0x24) = p14[1];
        *(int *)(self + 0x28) = p14[2];
    }
    *(int *)(self + 0x50) = *(int *)(self + 0x38);
    *(int *)(self + 0x54) = *(int *)(self + 0x3c);
    *(int *)(self + 0x58) = *(int *)(self + 0x40);
    *(int *)(self + 0x5c) = *(int *)(self + 0x44);
    *(int *)(self + 0x60) = *(int *)(self + 0x48);
    *(int *)(self + 0x64) = *(int *)(self + 0x4c);
    *(int *)(self + 0x74) = data_02092768[0];
    *(int *)(self + 0x78) = data_02092768[1];
    *(int *)(self + 0x7c) = data_02092768[2];
    *(int *)(self + 0x80) = data_02092768[3];
    Quaternion_FromVector3((int *)(self + 0x74), &data_ov006_021408a8, (Vec3 *)(self + 0x38));
    *(short *)(self + 0x96) = a3;
    *(short *)(self + 0x98) = a4;
    *(int *)(self + 0x84) = 1;
    switch (*(short *)(self + 0x96)) {
    case 0:
        *(void **)(self + 0x190) = self + 0xa0;
        break;
    case 1:
        *(void **)(self + 0x190) = self + 0xf0;
        break;
    case 2:
    default:
        *(void **)(self + 0x190) = self + 0x140;
        break;
    }
    *(int *)(self + 0x68) = data_020a0ebc[0];
    *(int *)(self + 0x6c) = data_020a0ebc[1];
    *(int *)(self + 0x70) = data_020a0ebc[2];
    *(int *)(self + 0x88) = 0;
    *(short *)(self + 0x92) = 0x5a;
    func_ov006_020ce0ac(self);
}
