#pragma opt_dead_assignments off

extern int data_02092138;
extern int data_0209212c;

void func_0202a850(int a, int b)
{
    data_02092138 = a;
    data_0209212c = b;
    data_02092138 = a - 0x64000;
    data_0209212c = b + 0x64000;
}
