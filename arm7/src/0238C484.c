extern int fn_02382AC4(void);
extern int fn_023883E0(int);
extern void fn_023883C8(int);
extern void fn_02382AD8(int);
extern int fn_023883FC(int, int);
extern int fn_02388384(int);
extern void fn_0238D31C(void);
extern void fn_0238D2AC(void);
extern void fn_0238D1FC(unsigned short *);
extern int fn_0238C458(void);
extern void fn_0238D084(int, int, int);
extern void fn_0238CEE0(int, int, int);
extern int fn_0238C420(void);
extern void fn_0238CD60(int, int, int);
extern void fn_0238CBE0(int, int, int);
extern void fn_0238CAC4(int);
extern int fn_0238C9A8(unsigned int);
extern void fn_0238C938(void);
extern void fn_0238C8C8(void);

int fn_0238C484(int *a1) {
    int v = fn_02382AC4();
    if (fn_023883E0(1) == 0) {
        fn_02382AD8(v);
        return fn_023883FC((unsigned short)a1[1], 4);
    }
    fn_023883C8(1);
    fn_02382AD8(v);
    switch (a1[1]) {
    case ' ':
        fn_0238D31C();
        break;
    case '!':
        fn_0238D2AC();
        break;
    case '"':
        fn_0238D1FC((unsigned short *)a1[4]);
        break;
    case '#':
        if (fn_0238C458() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238D084(a1[2], a1[3], a1[4]);
        break;
    case '$':
        if (fn_0238C458() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238CEE0(a1[2], a1[3], a1[4]);
        break;
    case '%':
        if (fn_0238C420() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238CD60(a1[2], (unsigned short)a1[3], a1[4]);
        break;
    case '&':
        if (fn_0238C420() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238CBE0(a1[2], (unsigned short)a1[3], a1[4]);
        break;
    case '\'':
        if (fn_0238C420() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238CAC4(a1[2]);
        break;
    case '(':
        if (fn_0238C420() == 0) {
            fn_023883FC((unsigned short)a1[1], 3);
            return fn_02388384(1);
        }
        fn_0238C9A8((unsigned int)a1[2]);
        break;
    case ')':
        fn_0238C938();
        break;
    case '*':
        fn_0238C8C8();
        break;
    default:
        fn_02388384(1);
        return fn_023883FC((unsigned short)a1[1], 1);
    }
    fn_023883FC((unsigned short)a1[1], 0);
    return fn_02388384(1);
}
