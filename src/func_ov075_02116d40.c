extern unsigned char IS_PLAYER_ACTIVE[];
extern int func_02020364(int i);

int func_ov075_02116d40(void)
{
    int r5 = 0;
    unsigned char *r4 = IS_PLAYER_ACTIVE;
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
