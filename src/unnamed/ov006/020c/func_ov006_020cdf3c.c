/* func_ov006_020cdf3c at 0x020cdf3c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov006_020cdeec(char* c);
extern void func_ov006_020cde7c(char* c);
extern void func_ov006_020cde28(char* c);
extern void func_ov006_020cdf20(char* c);
extern void func_ov006_020cdce4(char* c);
extern void func_ov006_020cdc68(char* c);
extern void func_ov006_020cdad0(char* c);
extern void func_ov006_020cdc14(char* c);
extern void func_ov006_020cd98c(char* c);

void func_ov006_020cdf3c(char* c)
{
    int a = _Z14ApproachLinearRiii((int*)(c + 0x6c), 0x1000, 0xc0);
    int b = _Z14ApproachLinearRiii((int*)(c + 0x68), 0x1000, 0x180);
    int d = _Z14ApproachLinearRiii((int*)(c + 0x70), 0x1000, 0x180);
    int e = _Z14ApproachLinearRiii((int*)(c + 0x88), 0x800, 0x60);
    if (a == 0) return;
    if (b == 0) return;
    if ((d & e) == 0) return;
    switch (*(short*)(c + 0x98)) {
    case 1: func_ov006_020cdeec(c); break;
    case 2: func_ov006_020cde7c(c); break;
    case 3: func_ov006_020cde28(c); break;
    case 0: func_ov006_020cdf20(c); break;
    case 4: func_ov006_020cdce4(c); break;
    case 6: func_ov006_020cdc68(c); break;
    case 8: func_ov006_020cdad0(c); break;
    case 5: func_ov006_020cdc14(c); break;
    case 7: func_ov006_020cd98c(c); break;
    }
}
