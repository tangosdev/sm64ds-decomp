extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int b, int scale, unsigned int d);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void func_02012718(void *a, int b);
extern int data_ov006_0213b00c[];
extern void *data_ov006_0214041c[];
extern int data_ov006_0213b080[];

struct W2 { int a, b; };

void func_ov006_020c81e0(char *c)
{
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = data_ov006_0213b00c[0];
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x4c, data_ov006_0214041c[0], 0x40000000, 0x800, 0);
    *(int *)(c + 0xa4) = 0;
    _ZN5Sound12PlayBank2_2DEj(0x10f);
    func_02012718((void *)0x1b5, *(short *)(c + 0x36) << 0xc);
    *(struct W2 *)(c + 0x3c) = *(struct W2 *)data_ov006_0213b080;
}
