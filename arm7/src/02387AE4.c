/* storage 0x02387AE4 runtime 0x037FF97C linked 0xE0 wram ARM.
 * void so the scratch temps stay in r0. Two Timer slots share the 0x14 frame. */

extern int fn_02387C40(unsigned short *a1);
extern int fn_02387BD4(void);
extern int fn_02382B28(int a1);
extern int *dat_038087A0;

struct Timer {
    unsigned short hw;
    unsigned short pad;
    int val;
};

void fn_02387AE4(int a1, int a2) {
    int flag;
    int v1;
    int v2;
    struct Timer s1;
    struct Timer s2;
    int sum;

    flag = 1;
    v1 = a1;
    v2 = a2;
    sum = v1 + v2;
    if (sum == 0)
        goto tail;
    if (v1 != 0) {
        s1.hw = *(volatile unsigned short *)0x04000006;
        s1.val = v1;
        while (fn_02387C40(&s1.hw))
            fn_02382B28(0);
    }
    if (v2 == 0)
        goto els;
    s2.hw = *(volatile unsigned short *)0x04000006;
    s2.val = v2 - v1;
    flag = 0;
    while (flag == 0 && fn_02387C40(&s2.hw)) {
        fn_02382B28(0);
        flag = fn_02387BD4();
    }
    goto tail;
els:
    flag = fn_02387BD4();
tail:
    if (flag == 0)
        *dat_038087A0 = 4;
}
