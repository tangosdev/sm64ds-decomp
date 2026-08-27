struct Pair { int a, b; };
extern struct Pair data_ov006_0213ca5c;

void func_ov006_020ed300(char *p)
{
    *(int *)(p + 0x466c) = 0xb4;
    *(struct Pair *)(p + 0x4660) = data_ov006_0213ca5c;
}
