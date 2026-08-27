struct Node5b1a4;

typedef struct N5b358 {
    struct N5b358 *next;
    int pad[5];
} N5b358;

extern N5b358 data_020a6760[256];
extern N5b358 *data_020a6484;
extern unsigned char data_020a7760[];
extern int data_020a648c;
extern int data_020a6490;
extern int data_020a64a0;
extern int data_020a6498;
extern int data_020a649c;
extern int data_020a6488;
extern void *data_020a7f48;
extern void *data_020a6494;
extern int data_020a64a4;
extern int data_020a64e0;
extern void *data_020a7fc0;

void func_0205ae64(void);
void func_0205b554(char *p);
void *func_0205b1d8(int arg);
void func_0205b1a4(struct Node5b1a4 *node);
void func_0205b070(int arg);

void func_0205b358(void)
{
    int i;
    void *p;

    func_0205ae64();

    data_020a6484 = data_020a6760;
    for (i = 0; i < 0xff; i++)
        data_020a6760[i].next = &data_020a6760[i + 1];

    *(int *)(data_020a7760 + 0x7e8) = 0;
    data_020a6494 = &data_020a7f48;
    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0 = 0;
    data_020a6498 = 0;
    data_020a649c = 0;
    data_020a64a4 = 1;
    data_020a6488 = 0;
    data_020a7fc0 = &data_020a64e0;

    func_0205b554((char *)&data_020a64e0);

    p = func_0205b1d8(1);
    if (p == 0)
        return;

    *(int *)((char *)p + 4) = 0x19;
    *(void **)((char *)p + 8) = data_020a7fc0;
    func_0205b1a4((struct Node5b1a4 *)p);
    func_0205b070(1);
}
