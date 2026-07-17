typedef struct { int a, b; } Pair;

void test_fn(char *self, Pair *arr)
{
    int i;
    for (i = 3; i >= 1; i--) {
        Pair next = arr[i+1];
        int off = *(int*)(self + i*4 + 0x4c);
        arr[i].a = next.a - (off + (i+1)*0x2000);
        arr[i].b = next.b;
    }
}
