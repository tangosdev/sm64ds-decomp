#pragma push
#pragma inline_depth(0)
void func_ov006_020e0ff0(void* base, int idx) {
    char* b = (char*)base;
    int off = idx * 0x24;

    *(int*)(b + 0x478c + off) += *(int*)(b + 0x4794 + off);
    *(int*)(b + 0x4790 + off) += *(int*)((char*)base + off + 0x4000 + 0x798);

    unsigned short* p1 = (unsigned short*)(b + 0x47a0 + off);
    if (*p1 != 0) {
        *p1 = *p1 - 1;
        short v = *(short*)p1;
        if (v < 0) {
            *p1 = 0;
        }
        return;
    }

    int* p2 = (int*)(b + 0x4794 + off);
    if (*p2 < 0x400) {
        *p2 = *p2 + 0x20;
        if (*p2 >= 0x400) {
            *p2 = 0x400;
        }
    }

    unsigned short* p3 = (unsigned short*)(b + 0x47a2 + off);
    if (*p3 != 0) {
        *p3 = *p3 - 1;
        short v = *(short*)p3;
        if (v < 0) {
            *p3 = 0;
        }
        return;
    }

    *(char*)(b + off + 0x4000 + 0x7aa) = 3;
}
#pragma pop
