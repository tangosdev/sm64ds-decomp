void func_0206e030(char *self)
{
    *(int *)(self + 0x24) = *(int *)(self + 0x1c);
    *(int *)(self + 0x28) = *(int *)(self + 0x20);
    *(int *)(((long long)(int)(self + 0x28)) & 0xFFFFFFFFFFFFFFFFLL) -=
        (*(int *)(self + 0x18) & *(int *)(self + 0x2c));
    *(int *)(self + 0x34) = *(int *)(self + 0x18);
}
