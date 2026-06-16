
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void data_ov052_02111e84(void *c);
extern void func_ov021_02112d64(void);

int func_ov010_02111fc0(void *c) {
    void *file = _ZN5Model8LoadFileER13SharedFilePtr((void*)func_ov021_02112d64);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c + 0xd4, file, 1, -1);
    *(unsigned char*)((char*)c + 0x124) = 0xff;
    data_ov052_02111e84(c);
    return 1;
}
