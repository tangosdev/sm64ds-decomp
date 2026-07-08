// NONMATCHING: push-set / frame (div=9). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned int u32;
extern void func_0204f070(void);
extern void* _ZN6Memory8AllocateEj(u32 size);
extern void* func_0205130c(u32 addr, u32 size);
extern void func_0201a9fc(char *c);
extern void func_02050f34(void *self, int a1, int a2, int a3);
extern int func_0203d974(void);
extern void func_02052008(int arg);
extern int func_020134d8(int a, int b);
extern void func_0204f94c(void *p);
extern void func_02011a28(char* c);
extern void func_02048f34(void *owner);
extern void func_020506fc(int arg);
extern void func_02049cd8(void *a, int b, int mode);

extern int data_0209b498;
extern char data_0208e498[];
extern char data_0209d574[];
extern char data_0209b4b4[];
extern int _ZN5Sound7FileRef5PTR_0E;
extern int data_0209b4b0;
extern int data_0209b4a4;
extern char data_0209b53c[];
extern int data_0209baa0;
extern u8 data_0209b480;

void func_020133bc(void)
{
    func_0204f070();
    data_0209b498 = (int)func_0205130c((u32)_ZN6Memory8AllocateEj(0x100000), 0x100000);
    func_0201a9fc(data_0209d574);
    func_02050f34(&data_0209b4b4, (int)data_0208e498, data_0209b498, 0);
    func_0201a9fc(data_0209d574);
    func_02052008(func_0203d974() == 0 ? data_0209b498 : 0);
    if (func_0203d974() == 0) {
        func_0201a9fc(data_0209d574);
        func_020134d8(1, data_0209b498);
        func_0201a9fc(data_0209d574);
    }
    func_0204f94c(&_ZN5Sound7FileRef5PTR_0E);
    func_0204f94c(&data_0209b4b0);
    func_0204f94c(&data_0209b4a4);
    func_02011a28(data_0209b53c);
    func_02048f34(&data_0209b4b4);
    func_020506fc(2);
    func_02049cd8(&data_0209baa0, 0x1000, 0);
    data_0209b480 = 1;
}
