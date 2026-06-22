extern int func_ov004_020ad674(void);
extern void func_ov006_020c8a9c(int a0, int a1);
extern int data_ov006_0213fb18[];
int func_ov006_02121f70(void)
{
    func_ov006_020c8a9c(0, data_ov006_0213fb18[func_ov004_020ad674()]);
    return 1;
}
