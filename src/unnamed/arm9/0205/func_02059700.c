typedef unsigned long long u64;
extern volatile u64 data_020a6438;
extern int data_020a6434;
extern void func_02056e4c(int a, void *b, int c);
extern void func_02059700(void);

void func_02059700(void)
{
    data_020a6438++;
    if (data_020a6434 != 0) {
        *(volatile unsigned short *)0x4000102 = 0;
        *(volatile unsigned short *)0x4000100 = 0;
        *(volatile unsigned short *)0x4000102 = 0xc1;
        data_020a6434 = 0;
    }
    func_02056e4c(0, (void *)func_02059700, 0);
}
