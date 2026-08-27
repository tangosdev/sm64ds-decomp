//cpp
extern "C" {
extern void func_ov006_020e6e3c(int a, int b);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int data_ov006_0213b0ec[];
extern void *data_ov006_02140540;
extern double data_ov006_0213b1cc;
void func_ov006_020cc37c(char *c);
void func_ov006_020cc37c(char *c) {
    if (data_ov006_0213b0ec[0] > 1)
        func_ov006_020e6e3c(0x1ca, *(int*)(c + 0x1c));
    else
        _ZN5Sound12PlayBank2_2DEj(0x1c9);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x6c, *(void**)&data_ov006_02140540, 0, 0x800, 0);
    *(short*)(c + 0x60) = 0x28;
    *(double*)(c + 0x64) = data_ov006_0213b1cc;
}
}
