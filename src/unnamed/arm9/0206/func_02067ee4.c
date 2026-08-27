unsigned short func_02067ee4(unsigned short *p, int len)
{
    int sum;
    int count;
    int r;
    count = len >> 1;
    sum = 0;
    while (count > 0) {
        sum += *p++;
        count--;
    }
    r = (sum & 0xffff) + (unsigned)((unsigned)sum >> 16);
    r = r + ((unsigned)r >> 16);
    return (unsigned short)(r ^ 0xffff);
}
