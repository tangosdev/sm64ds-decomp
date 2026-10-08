extern int func_02054d88(void);
extern void func_02031028(int a);
extern void func_020316d8(int a, int b, int c, int d);

extern unsigned char data_0209fc7c;
extern unsigned char data_0208f174[];
extern unsigned char data_0209fca0;
extern unsigned char *data_0209fd0c;
extern int data_0209fd18;
extern unsigned char data_02092818[];

void func_020318a4(int sl)
{
    int base;
    int acc;
    int m;
    int z;
    int i;
    unsigned char *cmd;
    unsigned char op;

    acc = data_0209fc7c;
    base = func_02054d88() + 0x8000;
    m = -1;
    z = 0;

    for (;;) {
        cmd = data_0209fd0c;
        op = cmd[0];
        switch (op) {
        case 0xfd:
            return;
        case 0xfe: {
            int param = cmd[2];
            int val;
            data_0209fd18 = param;
            val = (unsigned short)(cmd[4] | (cmd[3] << 8));
            if (param == 1 && val == 1) {
                func_02031028(m);
                i = z;
                if (i < data_0209fca0) {
                    unsigned char *p = data_02092818;
                    do {
                        func_020316d8(sl, *p, acc, base);
                        acc = (acc + data_0208f174[*p]) & 0xff;
                        p++;
                        i++;
                    } while (i < data_0209fca0);
                }
            }
            data_0209fd0c = data_0209fd0c + data_0209fd0c[1];
            break;
        }
        case 0xff:
            return;
        default:
            func_020316d8(sl, op, acc, base);
            acc = (acc + data_0208f174[op]) & 0xff;
            data_0209fd0c = data_0209fd0c + 1;
            break;
        }
    }
}
