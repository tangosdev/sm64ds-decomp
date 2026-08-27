extern unsigned char data_0209fc5c[];
extern int func_02020364(int i);

int func_ov075_02116d40(void)
{
    int r5 = 0;
    unsigned char *r4 = data_0209fc5c;
    for (;;) {
        if (*r4 != 0) {
            int result = func_02020364(r5);
            if (result == 0) return 0;
        }
        r5++;
        r4++;
        if (r5 >= 4) break;
    }
    return 1;
}
