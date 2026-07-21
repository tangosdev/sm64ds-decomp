//cpp
extern "C" void func_ov060_02113564(char *c);
extern "C" void func_ov060_021134ac(void* thiz);
extern "C" int func_ov060_02113404(char* c);
extern "C" int func_ov060_021130c0(char* c);
extern "C" int func_ov060_02112ee0(char* c);

extern "C" void func_ov060_02112ddc(char *self)
{
    unsigned char *p;
    switch (*(unsigned char*)(self + 0x423)) {
    case 0: func_ov060_02113564(self); break;
    case 1: func_ov060_021134ac(self); break;
    case 2:
        if (func_ov060_02113404(self) == 0) break;
        *(short*)(self + 0x3fe) = 0;
        if (*(unsigned char*)(self + 0x414) == 2) { *(unsigned char*)(self + 0x423) = 0xa; break; }
        p = (unsigned char*)((long long)((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL);
        *p += 1;
        break;
    case 3:
        if (func_ov060_021130c0(self) == 0) break;
        p = (unsigned char*)((long long)((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL);
        *p += 1;
        break;
    case 10:
        if (func_ov060_02112ee0(self) != 0) { p = (unsigned char*)((long long)((int)self + 0x423) & 0xFFFFFFFFFFFFFFFFLL); *p += 1; }
        break;
    case 11: break;
    }
}
