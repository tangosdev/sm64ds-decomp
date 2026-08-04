extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void *data_ov006_02138a20[];
extern void *data_ov006_02138a38[];

void func_ov006_02111268(char *c)
{
    if (((*(int **)(c + 4))[2] & 0xff) == 0) {
        func_ov004_020afdd0(data_ov006_02138a20[0], *(int *)(c + 8) >> 12, *(int *)(c + 0xc) >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02138a20[1], *(int *)(c + 8) >> 12, (*(int *)(c + 0xc) >> 12) + 5, -1, 2);
    } else {
        func_ov004_020afdd0(data_ov006_02138a38[0], *(int *)(c + 8) >> 12, *(int *)(c + 0xc) >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02138a38[1], *(int *)(c + 8) >> 12, (*(int *)(c + 0xc) >> 12) + 5, -1, 2);
    }
}
