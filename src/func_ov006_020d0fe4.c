extern int __cxa_vec_cleanup(void *dest, int a, int size, void (*func)(void));
extern char data_ov006_02140990;
extern void func_ov006_020d1008(void);

void func_ov006_020d0fe4(void)
{
    __cxa_vec_cleanup(&data_ov006_02140990, 4, 0x32c, func_ov006_020d1008);
}
