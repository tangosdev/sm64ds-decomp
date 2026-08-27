void func_ov002_020af7cc(char* c)
{
    *(unsigned char*)(c + 0x38e) = 1;
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x37c) + 0x64000) return;
    *(int*)(c + 0x60) += 0x5000;
    if (*(int*)(c + 0x60) < *(int*)(c + 0x37c) + 0x64000) return;
    *(int*)(c + 0x60) = *(int*)(c + 0x37c) + 0x64000;
    *(int*)(c + 0x384) = 0;
    *(int*)(c + 0x388) = 0;
    *(unsigned short*)(c + 0x100) = 0xffff;
    *(unsigned short*)(c + 0x38c) = 0xffff;
}
