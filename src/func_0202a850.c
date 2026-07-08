// NONMATCHING: pure instruction scheduling (2 words). Logic verified correct vs ROM,
// but mwccarm 1.2/sp2p3 schedules `sub ip,r0,#0x64000` after `str r1,[r2]` while the
// ROM emits it before; statement reordering does not move it (known scheduling wall).
// Stores a pair of hardware-register globals, then overwrites with the shifted pair.
extern volatile int data_02092138;
extern volatile int data_0209212c;
void func_0202a850(int a, int b)
{
    data_02092138 = a;
    data_0209212c = b;
    data_02092138 = a - 0x64000;
    data_0209212c = b + 0x64000;
}
