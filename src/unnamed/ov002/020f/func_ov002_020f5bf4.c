void func_ov002_020f5bf4(char* c)
{
    if (*(unsigned char*)(c+0x128) == 0) return;
    *(int*)(c+0xe4) = 0x80000;
    *(int*)(c+0xe8) = 0x60000;
    *(unsigned short*)(c+0x114) = 0;
    *(unsigned char*)(c+0x12a) = 1;
    *(int*)(c+0x10c) = 0xccc;
    *(unsigned short*)(c+0x110) = 0x155;
}
