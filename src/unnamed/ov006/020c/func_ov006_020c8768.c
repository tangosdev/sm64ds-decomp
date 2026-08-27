extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *anim, void *file, int a, int b, unsigned int u);
struct G { int w[2]; };
extern int data_ov006_02140424[];
extern struct G data_ov006_0213b038;
void func_ov006_020c8768(char *p) {
    *(short *)(p + 0x32) = 0x3c;
    *(int *)(p + 0x18) = 0x100000;
    *(int *)(p + 0x24) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0x4c, (void *)data_ov006_02140424[0], 0x40000000, 0x800, 0);
    *(struct G *)(p + 0x3c) = data_ov006_0213b038;
}
