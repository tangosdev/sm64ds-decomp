/* storage 0x0239AC04 runtime 0x027CCB7C size 0x148 main. */

extern int fn_0239A284(int index, unsigned short *a2);

#define G (*(char **)0x0380FFF4)

int fn_0239AC04(unsigned short *a1) {
    char *e;
    unsigned short cnt;
    unsigned int i;
    unsigned int used;
    unsigned int flag;
    unsigned int lim;
    unsigned int key;
    unsigned int min;
    unsigned short h;

    key = a1[0];
    if (key & 1)
        return 0;
    cnt = *(unsigned short *)(G + 0x4E8);
    if (cnt > 1) {
        e = *(char **)(G + 0x2E0) + 0x1C;
        flag = 0;
        used = flag;
        i = 1;
        lim = *(unsigned short *)(G + 0x2E6);
        for (; i < lim; i++, e += 0x1C) {
            if (*(unsigned short *)e != 0) {
                if (*(unsigned short *)(e + 8) == a1[2] &&
                    *(unsigned short *)(e + 6) == a1[1] &&
                    *(unsigned short *)(e + 4) == key)
                    return i;
                if (++used >= cnt)
                    break;
            } else if (flag == 0) {
                flag = i;
            }
        }
        if (flag != 0)
            i = flag;
    } else {
        i = 1;
    }
    if (i >= *(unsigned short *)(G + 0x2E6)) {
        e = *(char **)(G + 0x2E0);
        min = 0x10000;
        used = 1;
        i = 0;
        for (; used < *(unsigned short *)(G + 0x2E6); used++) {
            char *t = e + used * 0x1C;
            if (*(unsigned short *)t < 0x30 && *(unsigned short *)(t + 0x16) == 0) {
                h = *(unsigned short *)(t + 0x18);
                if (min > h) {
                    min = h;
                    i = used;
                }
            }
        }
        if (i == 0)
            return 0xFF;
    }
    fn_0239A284(i, a1);
    return i;
}
