/* func_ov006_020ced84 at 0x020ced84
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int data_ov006_02140838;
extern char *data_ov006_0214082c;
extern void func_ov006_020ce988(char *c);

void func_ov006_020ced84(void)
{
    int i;
    for (i = 0; i < data_ov006_02140838; i++)
    {
        char *p = data_ov006_0214082c + i * 0x1d0;
        if (*(int *)(p + 0x84) != 0)
            func_ov006_020ce988(p);
    }
}
