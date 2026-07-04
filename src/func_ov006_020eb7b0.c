typedef struct S {
    int a;
    int b;
} S;

int func_ov006_020eb7b0(char* p)
{
    if (*(unsigned char*)(p + 0x93) == 0) {
        S* q = (S*)(long long)(((long long)(int)(p + 8)) & 0xFFFFFFFFFFFFFFFFLL);
        int x = *(int*)p;
        if (x == q->a && (*(int*)(p + 4) == q->b || x == 0))
            return 1;
    }
    return 0;
}
