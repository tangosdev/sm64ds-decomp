extern unsigned char IS_PLAYER_ACTIVE[];
extern int func_020201f0(int);
int func_ov075_02116ce4(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (IS_PLAYER_ACTIVE[i]) {
            int r = func_020201f0(i);
            if (!r) return 0;
        }
    }
    return 1;
}
