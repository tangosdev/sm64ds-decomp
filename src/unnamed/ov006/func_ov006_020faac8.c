void SetBg2Offset(int a, int b);

void func_ov006_020faac8(char* c, int i)
{
    int* b = (int*)(c + 0x5c04 + i * 0x14);
    int* a = (int*)(c + 0x5bfc + i * 0x14);
    int v;
    int x;

    *a = *a + *b;

    if (*b <= -0x2000) {
        *b = *b - 0x100;
    }

    v = *a >> 12;
    SetBg2Offset(v, *(int*)(c + i * 0x14 + 0x5c00) >> 12);

    if (v > -0x90) return;

    *(int*)(c + i * 0x14 + 0x5c08) = 0x800;
    *(unsigned char*)(c + i * 0x14 + 0x5c0f) = 3;
}
