extern int data_02099f24;
extern int data_020a4ba8, data_020a4b88, data_020a4b78, data_020a4b98, data_020a4b6c;
extern void func_02043fdc(int *p);
extern void func_020441cc(int *p);

void func_02044120(void)
{
    data_02099f24 = 4;
    func_02043fdc(&data_020a4ba8);
    data_02099f24 = 2;
    func_02043fdc(&data_020a4b88);
    data_02099f24 = 3;
    func_02043fdc(&data_020a4b78);
    data_02099f24 = 5;
    func_02043fdc(&data_020a4b98);
    data_02099f24 = 1;
    func_020441cc(&data_020a4b6c);
    data_02099f24 = 0;
}
