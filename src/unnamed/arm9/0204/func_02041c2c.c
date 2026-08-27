void func_02041c2c(char *base, int key)
{
    base = *(char **)(base + 0x2704);
    if (base == 0)
        return;
    do {
        if (*(int *)(base + 0x7c) == 2 && *(int *)(base + 4) == key)
            return;
        base = *(char **)base;
    } while (base != 0);
}
