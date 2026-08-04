extern void func_ov006_02104b4c(char *p);
extern void func_ov006_021048b0(char *o);
extern void func_ov006_02104558(char *p);

#pragma opt_strength_reduction off
void func_ov006_021067a4(char *c)
{
    int i;
    for (i = 0; i < 0x24; i++) {
        *(int *)(c + i * 4 + 0x4cc4) = 0;
        *(int *)(c + i * 4 + 0x4d54) = 0;
        *(short *)(c + i * 2 + 0x4de8) = 0;
        *(short *)(c + i * 2 + 0x4e30) = 0;
        *(short *)(c + i * 2 + 0x4e78) = 0;
        *(char *)(c + i + 0x4f1e) = 0;
        *(char *)(c + i + 0x4efa) = 0;
        *(char *)(c + i + 0x4f42) = 0;
        *(char *)(c + i + 0x4f66) = 0;
        *(char *)(c + i + 0x4f8a) = 0;
    }
    *(int *)(c + 0x4ca8) = 0;
    *(char *)(c + 0x4fe9) = 0;
    *(short *)(c + 0x4ec0) = 0;
    *(short *)(c + 0x4ec2) = 0;
    *(char *)(c + 0x4fde) = 0;
    *(char *)(c + 0x4fe1) = 0;
    *(char *)(c + 0x4fe2) = 0;
    for (i = 0; i < 0x20; i++) {
        *(unsigned char *)(c + i + 0x4fae) = 0xff;
    }
    for (i = 0; i < 0x10; i++) {
        *(char *)(c + i + 0x4fce) = 0;
    }
    *(char *)(c + 0x4fe4) = 0;
    *(char *)(c + 0x4fe0) = 0;
    *(int *)(c + 0x4660) = 0;
    *(int *)(c + 0x4664) = 0;
    *(int *)(c + 0x4668) = 0;
    *(int *)(c + 0x466c) = 0;
    *(short *)(c + 0x4670) = 0;
    *(char *)(c + 0x4675) = 0;
    *(char *)(c + 0x4676) = 0;
    *(char *)(c + 0x4674) = 0;
    *(int *)(c + 0x4678) = 0;
    *(int *)(c + 0x467c) = 0;
    *(short *)(c + 0x4680) = 0;
    *(char *)(c + 0x4684) = 0;
    *(char *)(c + 0x4685) = 0;
    *(char *)(c + 0x4686) = 0;
    *(char *)(c + 0x4fe6) = 0;
    *(short *)(c + 0x4ec6) = 0;
    func_ov006_02104b4c(c);
    func_ov006_021048b0(c);
    func_ov006_02104558(c);
}
