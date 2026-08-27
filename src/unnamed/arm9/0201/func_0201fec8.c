extern unsigned char data_0209e64c;
extern char data_0209d574[];
extern unsigned char data_0208f274;
extern char data_02075358[];
extern char data_02075320[];
extern char data_0207533c[];
extern char data_020752cc[];
extern char data_020752e8[];
extern char data_02075304[];
extern int func_0201a9ec(char *c);
extern void func_02059eb0(unsigned char *dst);
extern void func_0203db64(char *a, char *b);
extern int func_0203daac(void);
extern void func_0201a9fc(char *c);
extern void func_0201a96c(char *self, int newVal);

void func_0201fec8(void)
{
    unsigned char buf[0x50];
    int saved;
    if (data_0209e64c != 0) return;
    saved = func_0201a9ec(data_0209d574);
    func_02059eb0(buf);
    switch (buf[0]) {
    case 2:
        func_0203db64(data_02075358, data_02075320);
        break;
    case 3:
        func_0203db64(data_02075358, data_0207533c);
        break;
    case 4:
        func_0203db64(data_02075358, data_020752cc);
        break;
    case 5:
        func_0203db64(data_02075358, data_020752e8);
        break;
    default:
        func_0203db64(data_02075358, data_02075304);
        break;
    }
    data_0208f274 = func_0203daac();
    func_0201a9fc(data_0209d574);
    func_0201a96c(data_0209d574, saved);
    data_0209e64c = 1;
}
