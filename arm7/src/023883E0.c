/* fn_023883E0: report mailbox idle state. */

extern int dword_3808FC4[];

int fn_023883E0(void)
{
    return *dword_3808FC4 == 0;
}
