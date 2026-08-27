struct Pair { int a, b; };
extern struct Pair data_ov006_0213c9cc;

void func_ov006_020ed81c(char *p)
{
    *(int *)(p + 0x466c) = 0x4b0;
    *(struct Pair *)(p + 0x4660) = data_ov006_0213c9cc;
}
