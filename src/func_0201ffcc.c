extern unsigned char data_0209e64c;
extern char data_0209d574[];
extern unsigned char data_0208f274;
extern unsigned char func_0201a9ec(char *c);
extern void func_0203df40(void);
extern int func_0203daac(void);
extern void func_0201a9fc(char *c);
extern unsigned char func_0201a96c(char *self, unsigned char newVal);

void func_0201ffcc(void)
{
    unsigned char saved;
    if (data_0209e64c == 0) return;
    saved = func_0201a9ec(data_0209d574);
    func_0203df40();
    data_0208f274 = func_0203daac();
    func_0201a9fc(data_0209d574);
    func_0201a96c(data_0209d574, saved);
}
