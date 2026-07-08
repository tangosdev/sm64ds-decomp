extern int data_020a55fc;
extern int data_020a55f8;
extern int data_020a5600;
extern int data_020a5614;
extern int data_020a5684;
extern int _ZN5Sound8SDAT_RAM3PTRE;
extern void func_02050038(void);

extern void func_02058940(int *s, int *v1, int v2);
extern void func_02058200(int *a, void (*f)(void), int c, int *d, int e, int g);
extern void func_02058048(int *a);

void func_020506fc(int arg)
{
    if (data_020a55fc != 0)
        return;
    data_020a55f8 = 0;
    func_02058940(&data_020a5600, &data_020a5614, 8);
    func_02058200(&data_020a5684, func_02050038, 0, &_ZN5Sound8SDAT_RAM3PTRE, 0x400, arg);
    data_020a55fc = 1;
    func_02058048(&data_020a5684);
}
