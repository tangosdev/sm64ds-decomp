extern int func_ov007_020c1804(void);
extern int func_02045d00(void);
extern int func_ov007_020c17f4(void);
extern int func_02045ce0(void);
extern void func_0204a0dc(int a, void *fn);
extern void func_0204a028(int a, void *fn);

extern int data_ov007_02103440;
extern int data_ov007_0210343c;
extern int data_ov007_02103438;
extern int data_ov007_02103434;
extern int data_ov007_0210344c;

extern void func_ov007_020b7948(void);
extern void func_ov007_020b7914(void);

void func_ov007_020b77ac(void)
{
    data_ov007_02103440 = func_ov007_020c1804();
    data_ov007_0210343c = func_02045d00();
    data_ov007_02103438 = func_ov007_020c17f4();
    data_ov007_02103434 = func_02045ce0();
    func_0204a0dc(data_ov007_0210344c, func_ov007_020b7948);
    func_0204a028(data_ov007_0210344c, func_ov007_020b7914);
}
