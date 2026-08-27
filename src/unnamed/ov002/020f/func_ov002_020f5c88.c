void func_ov002_020f5c88(char* c)
{
    if (*(unsigned char*)(c+0x90) == 0) return;
    *(int*)(c+0x4c) = 0x80000;
    *(int*)(c+0x50) = 0x60000;
    *(unsigned short*)(c+0x7c) = 0;
    *(unsigned char*)(c+0x92) = 1;
    *(int*)(c+0x74) = 0xccc;
    *(unsigned short*)(c+0x78) = 0x155;
}
