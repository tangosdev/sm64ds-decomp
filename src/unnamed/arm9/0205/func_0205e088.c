struct Pair { void *a; int b; };

extern int data_020a8074;

void func_0205e088(struct Pair *dst, char *src)
{
    struct Pair v;
    v.a = &data_020a8074;
    v.b = *(int*)(src + 0x18);
    *dst = v;
}
