/* fn_0238853C: wait for the mailbox, then record the request. */

extern short word_380941C[];
extern int dword_3808FC4[];

extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern void fn_02381050(short *);
extern int fn_023883C8(int);

int fn_0238853C(int a1)
{
    int v5;
    for (;;) {
        v5 = fn_02382AC4();
        if (*dword_3808FC4 == 0)
            break;
        fn_02382AD8(v5);
        fn_02381050(word_380941C);
    }
    fn_023883C8(4);
    dword_3808FC4[279] = a1;
    return fn_02382AD8(v5);
}
