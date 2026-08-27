extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

void func_ov029_021122b4(char *self)
{
    Matrix4x3_FromTranslation(self + 0xf0, *(int *)(self + 0x5c) >> 3, *(int *)(self + 0x344) >> 3, *(int *)(self + 0x64) >> 3);
}
