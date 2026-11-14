/* fn_023883C8: set busy flag and store request word. */

extern int dword_3808FC4[];

int fn_023883C8(int result)
{
    int *v1 = dword_3808FC4;
    *v1 = 1;
    v1[1] = result;
    return result;
}
