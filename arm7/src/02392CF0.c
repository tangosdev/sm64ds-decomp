/* storage 0x02392CF0 runtime 0x027C4C68 size 0x164 main. Pool +0x164: 0000FFFF. */

extern int *fn_0238E688(void);
extern void fn_0238E158(int);
extern int fn_0238E0E4(void);
extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_02393A90(int);
extern void fn_02393C18(void);

void fn_02392CF0(void) {
    int *base;
    unsigned short *obj;
    int flag;
    int token;

    base = fn_0238E688();
    obj = *(unsigned short **)base[1412];
    obj[68] = 1;
    {
        unsigned short *countdown;

        countdown = obj + 94;
        *countdown = (unsigned short)(*countdown - 1);
        if (obj[94] == 0) {
        if (obj[93] != 0xFFFF) {
            fn_0238E158(obj[93] & 0xFF);
            obj[92] = (unsigned short)fn_0238E0E4();
            obj[93] = 0xFFFF;
        }
            obj[94] = 4;
        }
    }
    if (obj[0] == 9) {
        unsigned short *sum;

        token = fn_02382AC4();
        flag = obj[31] == 0;
        sum = obj + 31;
        *sum = (unsigned short)(*sum + obj[30]);
        if (obj[31] > 0x100)
            obj[31] = 0x100;
        if (flag != 0 && obj[31] != 0)
            flag = 1;
        else
            flag = 0;
        fn_02382AD8(token);
        if (flag != 0)
            fn_02393A90(0xFFFF);
        if (obj[73] == 1) {
            unsigned short *timer;

            timer = obj + 78;
            *timer = (unsigned short)(*timer - 1);
            if (obj[78] == 0) {
                obj[77] = 1;
                obj[78] = 0x3C;
            }
        }
    } else if (obj[0] == 10) {
        flag = 0;
        token = fn_02382AC4();
        if (obj[29] == 1 && *(int *)((char *)obj + 0x92C) != 1) {
            flag = 1;
            obj[29] = 0;
        }
        fn_02382AD8(token);
        if (flag == 1)
            fn_02393C18();
    }
}
