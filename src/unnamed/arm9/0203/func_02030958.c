extern signed char data_0209fc64[];

int func_02030958(int val)
{
    signed char i = 0;
    signed char *p = data_0209fc64;
    do {
        if (val == *p) return i;
        i = i + 1;
        p = p + 1;
    } while (i < 4);
    return -1;
}
