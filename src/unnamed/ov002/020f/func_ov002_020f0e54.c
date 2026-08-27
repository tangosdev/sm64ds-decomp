void func_ov002_020f0e54(char *a, char *b)
{
    if (b == 0) return;
    *(int *)(a + 0x138) = *(int *)(b + 4);
    *(int *)(a + 0x148) = *(int *)(a + 0x60) - *(int *)(b + 0x60);
}
