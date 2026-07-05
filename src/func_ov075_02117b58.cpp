//cpp
extern "C" {
void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int a);
}
extern int data_0208ee44;
extern unsigned char data_02092778;

extern "C" void func_ov075_02117b58(char* c)
{
    *(int*)(((int)c + 0x264) & 0xFFFFFFFFFFFFFFFF) -= data_0208ee44;
    if (*(int*)(c + 0x264) > 0) return;
    _ZN5Scene14StartSceneFadeEjjt(1, 0, 0);
    _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
    data_02092778 = 1;
}
