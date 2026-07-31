#pragma opt_strength_reduction off

extern char data_02099e6c[];
extern int data_020a1fc0;
extern char data_020a2400[];

extern void *func_0205d23c(void *a, int b);
extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern void func_020580f0(unsigned short *self);
extern int func_0205d5e8(char *self, int a1, int a2, int a3, int a4);
extern int FS_ReadFile(char *a, char *b, int c);
extern void func_02041d28(char *base, void *node, int d);
extern int FS_CloseFile(char *self);
extern void func_02041a94(char *base, void *node);

void func_02042254(void)
{
    int i;
    char *sb;
    int a = (int)func_0205d23c(data_02099e6c, 3);
    int b = 2;
    int c = 3;
    int d = 1;
    int *g = &data_020a1fc0;
    char *base = data_020a2400;

    while (1) {
        unsigned int irq = _ZN3IRQ7DisableEv();
        char *sub = base + 0x2000;
        sb = *(char **)(sub + 0x708);
        while (sb == 0) {
            if (g[3] == 0) {
                _ZN3IRQ7RestoreEj(irq);
                return;
            }
            func_020580f0(0);
            sb = *(char **)(sub + 0x708);
        }
        _ZN3IRQ7RestoreEj(irq);

        if (*(int *)(sb + 0x7c) == 1) {
            func_0205d5e8(sb + 0x38, a, *(int *)(sb + 0x8c),
                          *(int *)(sb + 0x8c) + *(int *)(sb + 0x90),
                          *(int *)(sb + 0x88));
            i = 0;
            {
                int val = 0;
                char *ptr = sb + 0xc0;
                for (; i < 3; i++) {
                    *(int *)(sb + i * 4 + 0xa0) = val;
                    FS_ReadFile(sb + 0x38, ptr, 0x400);
                    val += 0x400;
                    ptr += 0x400;
                }
            }
            *(int *)(sb + 0x94) = 0;
            *(int *)(sb + 0x9c) = b;
            *(int *)(sb + 0x98) = c;
            irq = _ZN3IRQ7DisableEv();
            func_02041d28(base, sb, d);
            _ZN3IRQ7RestoreEj(irq);
        } else {
            FS_CloseFile(sb + 0x38);
            irq = _ZN3IRQ7DisableEv();
            func_02041a94(base, sb);
            _ZN3IRQ7RestoreEj(irq);
        }
    }
}
