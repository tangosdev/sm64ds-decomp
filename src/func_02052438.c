void func_02052438(char *self)
{
    if (*(int *)(self + 8) < *(int *)(self + 0xc)) {
        *(int *)(((long long)(int)(self + 8)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }
}
