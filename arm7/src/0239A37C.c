typedef struct {
    int w[7];
} Rec;

extern void fn_02382C80(int, void *, int); /* local extern: callee owned by another job */

int fn_0239A37C(void)
{
    char **pp = (char **)0x0380FFF4;
    Rec *v1 = *(Rec **)(*pp + 0x2E0);
    unsigned int i;

    for (i = 1; i < *(unsigned short *)(*pp + 0x2E6); i++) {
        volatile unsigned short z = 0;
        fn_02382C80(z, (void *)(v1 + i), 0x1A);
    }
    *(unsigned short *)(*pp + 0x4E8) = 1;
    *(unsigned short *)(*pp + 0x4EA) = 0;
    *(unsigned short *)(*pp + 0x4EC) = 1;
    *(unsigned short *)(*pp + 0x4EE) = 0xFFFE;
    *(unsigned short *)(*pp + 0x4F0) = 0;
    *(unsigned short *)(*pp + 0x4F2) = 1;
    return 1;
}
