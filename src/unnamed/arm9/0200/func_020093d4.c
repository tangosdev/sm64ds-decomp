typedef struct {
    int _pad[0x108/4];
    int field_108;
} S;

unsigned int func_020093d4(S *p, int a)
{
    unsigned long long prod = (unsigned long long)a * (unsigned long long)p->field_108;
    unsigned long long sum  = prod + 0x800u;
    unsigned int lo = (unsigned int)sum;
    unsigned int hi = (unsigned int)(sum >> 32);
    return (lo >> 12) | (hi << 20);
}
