void func_ov002_020d3498(char* c)
{
    *(int*)(c + 0x98) = -*(int*)(c + 0x98);
    *(short*)(c + 0x8e) = *(short*)(c + 0x8e) + 0x8000;
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
}
