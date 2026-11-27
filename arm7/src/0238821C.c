/* fn_0238821C: service loop, dispatching each received message. */

extern int unk_3809244[];

extern void fn_0238155C(int *, int **, int);
extern int fn_02388798(int *);
extern int fn_0238B3E8(int *);
extern int fn_02389454(int *);
extern int fn_0238C484(int *);

void fn_0238821C(void)
{
    int *v0;
    for (;;) {
        fn_0238155C(unk_3809244, &v0, 1);
        switch (*v0) {
        case 0:
            fn_02388798(v0);
            break;
        case 2:
            fn_0238B3E8(v0);
            break;
        case 3:
            fn_02389454(v0);
            break;
        case 1:
            fn_0238C484(v0);
            break;
        }
    }
}
