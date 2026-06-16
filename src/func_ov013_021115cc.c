typedef unsigned int u32;
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, int, int, int);
extern void func_ov013_02111430(char* t);
extern void* data_ov051_021116b0[];

int func_ov013_021115cc(void* c) {
    int eq = (int)(*(unsigned short*)((char*)c + 0xc) == 0x125);
    if (eq != 0)
        *(unsigned char*)((char*)c + 0x124) = 0;
    else
        *(unsigned char*)((char*)c + 0x124) = 1;
    {
        unsigned char i = *(unsigned char*)((char*)c + 0x124);
        int file = _ZN5Model8LoadFileER13SharedFilePtr(data_ov051_021116b0[i]);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c + 0xd4, file, 1, -1);
    }
    func_ov013_02111430((char*)c);
    return 1;
}
