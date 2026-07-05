void func_02057e00(char *self, unsigned char value)
{
    if (*(int *)self != 0) {
        *(unsigned char *)(*(char **)(self + 4)) = value;
        *(int *)self -= 1;
    }

    *(int *)(((long long)(int)(self + 4)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
