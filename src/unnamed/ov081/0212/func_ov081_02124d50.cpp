//cpp
struct dActor_c {
    static dActor_c* FindWithID(unsigned int id);
};

extern "C" {
    void func_ov081_0212423c(char* c, int a);
    void func_ov081_02125488(char* c, void* p);
    int func_ov081_021245e8(char* c);
}

extern char data_ov081_02128e44[];
extern char data_ov081_02128e64[];

extern "C" int func_ov081_02124d50(char* c)
{
    char* r5 = (char*)dActor_c::FindWithID(*(unsigned int*)(c + 0x3fc));
    if (r5 != 0 && *(int*)(r5 + 0x9c) == 0) {
        func_ov081_0212423c(c, 1);
        *(int*)(r5 + 0x5c) = *(int*)(c + 0x434);
        *(int*)(r5 + 0x60) = *(int*)(c + 0x438);
        *(int*)(r5 + 0x64) = *(int*)(c + 0x43c);
    }

    if (*(unsigned short*)(c + 0x100) == 0) {
        func_ov081_02125488(c, data_ov081_02128e44);
        return 1;
    }

    if (func_ov081_021245e8(c) > 0x320000) {
        func_ov081_02125488(c, data_ov081_02128e64);
    }
    return 1;
}
