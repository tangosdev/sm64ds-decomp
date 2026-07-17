typedef struct { int a, b; } Pair;

void test_fn(char *self, Pair *arr)
{
    int i;
    for (i = 3; i >= 1; i--) {
        arr[i] = arr[i + 1];
        arr[i].a -= ((int*)(self + 0x4c))[i] + (i + 1) * 0x2000;
    }
}
