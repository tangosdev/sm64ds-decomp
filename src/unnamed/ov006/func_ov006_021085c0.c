extern void *func_ov004_020adc68(int id);
extern void _ZN5Model17UpdateFileOffsetsER8BMD_File(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
int func_ov006_021085c0(char *c){
    *(void**)(c + 8) = func_ov004_020adc68(0x21d);
    *(void**)(c + 0xc) = func_ov004_020adc68(0x21e);
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void**)(c + 8));
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void**)(c + 0xc));
    if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x10, *(void**)(c + 8), 1, -1) == 0)
        return 0;
    if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x60, *(void**)(c + 0xc), 1, -1) == 0)
        return 0;
    *(short*)(c + 0xcc) = 0;
    *(int*)(c + 0xbc) = 0;
    return 1;
}
