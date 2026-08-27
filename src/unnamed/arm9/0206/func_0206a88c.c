extern void func_0206a5c0(void);
extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void func_0205ba64(int a, void *fn);
extern void func_0206a6d0(void);
extern void func_0206a694(void);
extern void func_0206a634(void);

extern int data_020a9de8;
extern int data_020a9dec;

void func_0206a88c(void)
{
    if (data_020a9de8) return;
    data_020a9de8 = 1;
    func_0206a5c0();
    func_0205b858();
    while (func_0205ba3c(0xd, 1) == 0) ;
    func_0205ba64(0xd, (void *)func_0206a694);
    func_0206a6d0();
    func_0205ba64(0xd, 0);
    func_0205ba64(0xd, (void *)func_0206a634);
    data_020a9dec = 0;
}
