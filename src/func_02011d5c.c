extern unsigned char data_0209b480;
extern void func_02048e94(void);
extern void func_02048e74(void);
extern void func_02048e54(void);
extern void func_02049c48(int self);

void func_02011d5c(int self) {
    if (data_0209b480 == 0) return;
    switch (self) {
    case 1: func_02048e94(); break;
    case 2: func_02048e74(); break;
    default: func_02048e54(); break;
    }
    func_02049c48(self);
}
