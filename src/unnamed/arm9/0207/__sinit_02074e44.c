extern int data_0209e650;
extern int data_0209e654;
extern void func_0201973c(void);

extern void func_0203b9b4(int *p, int v);
extern void func_020731dc(int a, int b, void **node);

void __sinit_02074e44(void) {
    func_0203b9b4(&data_0209e650, 1);
    func_020731dc((int)&data_0209e650, (int)&func_0201973c, (void **)&data_0209e654);
}
