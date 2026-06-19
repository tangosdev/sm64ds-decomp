extern int func_0203d974(void);
extern int func_020134d8(int a, int b);
extern void func_0203d7d4(void);
extern int func_0205117c(int self);
extern void func_020510a4(int a, int b);
extern int data_0209b498;
extern unsigned char data_0208e428;
extern unsigned char data_0209b47c;
extern int data_0209b4a8;
extern int data_0209b484;
extern unsigned char data_0209b478;

void _ZN5Sound19LoadGroupAndSetBankEii(int a, int b) {
    if (func_0203d974() != 0) {
        if (a != 0x2f) return;
        func_020134d8(a, data_0209b498);
        func_0203d7d4();
        data_0208e428 = b;
        data_0209b47c = a;
        return;
    }
    if (a == 0) {
        func_020134d8(a, data_0209b498);
        data_0209b4a8 = func_0205117c(data_0209b498);
    } else if (data_0209b47c != a) {
        func_020510a4(data_0209b498, data_0209b4a8);
        func_020134d8(a, data_0209b498);
        data_0209b484 = func_0205117c(data_0209b498);
    }
    data_0208e428 = b;
    data_0209b47c = a;
    data_0209b478 = 0;
}
