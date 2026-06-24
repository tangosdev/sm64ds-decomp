//cpp
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
void _ZN11ShadowModel12InitCylinderEv(void* self);
void func_ov085_0212d268(void* c, void* p);
}
extern char data_ov085_021305d8;
extern char data_ov085_0213071c;

extern "C" int func_ov085_0212d46c(char* c)
{
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305d8);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x110, f, 1, -1) == 0)
        return 0;
    _ZN11ShadowModel12InitCylinderEv(c + 0x160);
    *(int*)(c + 0x19c) = *(int*)(c + 8) & 0xff;
    *(int*)(c + 0xa0) = -0x3c000;
    *(int*)(c + 0x190) = 0;
    func_ov085_0212d268(c, &data_ov085_0213071c);
    return 1;
}
