/* fn_023882B0: queue a command record with up to four payload words. */

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)((int)&(last) & ~3) + 4)
#define va_arg(ap, type) (*(type *)(((ap) += sizeof(type)) - sizeof(type)))

extern int unk_3809244[];
extern int unk_3809298[][6];
extern volatile int dword_3808FC4[];

extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int fn_02381600(int *, int *, int);

int fn_023882B0(int a1, int a2, unsigned short a3, ...)
{
    va_list v7;
    int v5;
    int v4;

    if (a3 > 4u)
        return 0;
    v5 = fn_02382AC4();
    unk_3809298[dword_3808FC4[277]][0] = a1;
    unk_3809298[dword_3808FC4[277]][1] = a2;
    va_start(v7, a3);
    for (int i = 0; i < a3; i++)
        (((int (*)[6])dword_3808FC4)[dword_3808FC4[277]] + i)[183] = va_arg(v7, int);
    v4 = dword_3808FC4[277];
    dword_3808FC4[277] = (v4 + 1) & 0xF;
    fn_02382AD8(v5);
    return fn_02381600(unk_3809244, unk_3809298[v4], 0);
}
