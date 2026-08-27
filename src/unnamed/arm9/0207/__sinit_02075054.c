extern void func_020731dc(short* a, void* b, void* node);
extern short data_020a0dbc[];
extern short data_020a0db8[];
extern char NoOpDestructor[];
extern char data_020a0dc0[];
extern char data_020a0dcc[];

void __sinit_02075054(void)
{
    data_020a0dbc[0] = 0;
    data_020a0dbc[1] = 0;
    func_020731dc(data_020a0dbc, NoOpDestructor, data_020a0dc0);
    data_020a0db8[0] = 1;
    data_020a0db8[1] = 1;
    func_020731dc(data_020a0db8, NoOpDestructor, data_020a0dcc);
}
