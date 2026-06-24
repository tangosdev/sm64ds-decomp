extern void* func_ov007_020c3df4(int a, int b);
struct M52 { int w[13]; };
void* func_ov007_020cbf8c(void* src)
{
    char *p = (char*)func_ov007_020c3df4(0, 0x34);
    if (src != 0) {
        *(struct M52*)p = *(struct M52*)src;
    } else {
        *(int*)(p) = 0;
        *(int*)(p + 8) = 0;
        *(int*)(p + 4) = *(int*)(p + 8);
        *(int*)(p + 0x10) = 0;
        *(int*)(p + 0xc) = *(int*)(p + 0x10);
        *(int*)(p + 0x24) = 0x1f;
        *(short*)(p + 0x28) = 0x7fff;
        *(short*)(p + 0x2a) = 0;
        *(int*)(p + 0x2c) = 0;
        *(int*)(p + 0x30) = 0;
        *(int*)(p + 0x1c) = 0;
        *(int*)(p + 0x20) = 0;
    }
    return p;
}
