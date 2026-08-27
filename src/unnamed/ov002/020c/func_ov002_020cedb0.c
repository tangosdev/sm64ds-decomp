typedef int Fix12i;
extern unsigned short data_ov002_020ff158[];

Fix12i func_ov002_020cedb0(void *c, Fix12i a)
{
    unsigned int t = data_ov002_020ff158[*(int*)((char*)c + 8)];
    return (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
