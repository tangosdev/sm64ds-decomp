void func_ov002_020f2f64(char* self)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (*(unsigned char*)(self + 0x170) != 0) {
            (*(unsigned char*)(((int)self + 0x172) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        self += 0x14;
    }
}
