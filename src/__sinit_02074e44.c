extern int RNG_STATE;
extern int data_0209e654;
extern void func_0201973c(void);

extern void func_0203b9b4(int *p, int v);
extern void func_020731dc(int a, int b, void **node);

void __sinit_02074e44(void) {
    func_0203b9b4(&RNG_STATE, 1);
    func_020731dc((int)&RNG_STATE, (int)&func_0201973c, (void **)&data_0209e654);
}
