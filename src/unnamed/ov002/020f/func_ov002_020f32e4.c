struct S { unsigned char b[0x30]; };

void func_ov002_020f32e4(struct S *arr, int i)
{
    char *p = (char*)&arr[i];
    *(unsigned short*)(p + 0x158) = 0x10;
    *(int*)(p + 0x150) = 0x1800;
    *(int*)(p + 0x154) = 0x1800;
    *(unsigned char*)(p + 0x15e) = 1;
}
