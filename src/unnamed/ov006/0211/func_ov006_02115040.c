int func_ov006_02115040(char *base, int i)
{
    int *arr = (int *)(base + 0x5968);
    int v = arr[i - 1] + 1;
    arr[i - 1] = v;
    return v;
}
