extern void func_ov007_020c6550(void* o);
extern void func_ov007_020c5c14(void* o, void* p);
extern void func_ov007_020c5ef0(void* o);
extern void func_ov007_020c510c(char* c);
extern char** data_ov007_02104ba0;
extern int* data_ov007_02104b9c;

void func_ov007_020b9f0c(void)
{
    if (*(unsigned short*)(*(char**)(*data_ov007_02104ba0 + 0x20) + 8) != 0) {
        func_ov007_020c6550(*data_ov007_02104ba0);
    }
    func_ov007_020c5c14(*data_ov007_02104ba0, *data_ov007_02104ba0 + 0x70);
    func_ov007_020c5ef0(*data_ov007_02104ba0);
    func_ov007_020c510c(*data_ov007_02104ba0);

    *(int*)((char*)data_ov007_02104ba0 + 0x28) = 0;
    *(short*)(*(char**)((char*)data_ov007_02104ba0 + 0xc) + 2) = 0;
    *(short*)(*(char**)((char*)data_ov007_02104ba0 + 0x10) + 2) = 0;
    *(int*)((char*)data_ov007_02104ba0 + 0x28) = 0xf0;
    *data_ov007_02104b9c = 0x1000;
}
