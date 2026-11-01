/* storage 0x023810B8 runtime 0x037F8F50 size 0x50 wram. Pool +0x50: 03806E58, 03806E74. */

extern void fn_02382AC4(void);
extern void fn_0238172C(int);
extern void fn_0238133C(int);
extern void fn_02380FE0(void *);
extern int fn_02380EC0(void);
extern int *dword_3806E58;
extern int dword_3806E74[];

int fn_023810B8(void) {
    int *obj;

    fn_02382AC4();
    obj = (int *)*dword_3806E58;
    fn_0238172C((int)obj);
    fn_0238133C((int)obj);
    dword_3806E74[obj[20]] = 0;
    obj[18] = 2;
    fn_02380FE0((char *)obj + 0x74);
    return fn_02380EC0();
}
