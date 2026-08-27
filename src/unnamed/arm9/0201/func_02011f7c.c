extern int func_0203d974(void);
extern void func_020510a4(void *a, void *b);
extern int func_020134d8(void *a, void *b);
extern int func_0205117c(void *self);

extern void *data_0209b498;
extern void *data_0209b484;
extern int data_0209b488;
extern unsigned char data_0209b478;

void func_02011f7c(void *self)
{
    if (!func_0203d974()) {
        func_020510a4(data_0209b498, data_0209b484);
        func_020134d8(self, data_0209b498);
        data_0209b488 = func_0205117c(data_0209b498);
    }
    data_0209b478 = (unsigned char)(int)self;
}
