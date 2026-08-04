typedef unsigned char u8;

extern void func_ov006_02112ad8(char *c);
extern void func_ov006_0211470c(int *a, int *b);
extern int _ZN4cstd4fdivEii(int a, int b);

void func_ov006_021128fc(char *c)
{
    int cur;
    int val;
    char *mgr;
    int vm1;
    int *p;
    int local1[2];
    int local2[2];
    int newpos;
    int pos;

    val = *(int *)(c + 0x2c);
    mgr = *(char **)(c + 4);
    cur = *(int *)(mgr + 0x4664);

    if (val == cur) {
        if (*(u8 *)(c + 0x121) != 0) {
            func_ov006_02112ad8(c);
            return;
        }
    }

    vm1 = val - 1;
    if (vm1 == cur) {
        p = (cur >= 0xd) ? (int *)0 : *(int **)(mgr + 0x4688 + (cur << 2));
        if (*((u8 *)p + 0x121) != 0)
            goto block1;
    }

    if (val != cur || *(u8 *)(c + 0x121) != 0)
        goto block2;

block1:
    p = (cur >= 0xd) ? (int *)0 : *(int **)(mgr + 0x4688 + (cur << 2));
    func_ov006_0211470c(local1, p);
    newpos = local1[0] - 0x10000;
    pos = *(int *)(c + 8);
    if (pos < newpos && pos < 0xc8000)
        *(int *)(c + 8) = newpos;
    else
        *(int *)(c + 8) = *(int *)(c + 0x10);
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = 0;

    *(int *)(c + 0xc) = _ZN4cstd4fdivEii(
        (int)(((long long)*(int *)(c + 8) * 0xa000 + 0x800) >> 12), 0xd8000) + 0x90000;

    if (*(int *)(c + 0x2c) > *(int *)(*(char **)(c + 4) + 0x4664)) {
        *(int *)(c + 0x104) =
            (*(int *)(c + 0x2c) - *(int *)(*(char **)(c + 4) + 0x4664)) * 0xf;
    }
    return;

block2:
    p = (vm1 >= 0xd) ? (int *)0 : *(int **)(mgr + 0x4688 + (vm1 << 2));
    func_ov006_0211470c(local2, p);
    *(int *)(c + 8) = local2[0] - 0x10000;
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = 0;

    *(int *)(c + 0xc) = _ZN4cstd4fdivEii(
        (int)(((long long)*(int *)(c + 8) * 0xa000 + 0x800) >> 12), 0xd8000) + 0x90000;

    if (*(int *)(c + 0x2c) > *(int *)(*(char **)(c + 4) + 0x4664)) {
        *(int *)(c + 0x104) =
            (*(int *)(c + 0x2c) - *(int *)(*(char **)(c + 4) + 0x4664)) * 0xf;
    }
}
