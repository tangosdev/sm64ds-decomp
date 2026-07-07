// NONMATCHING: materialized byte-RMW tail has r1/r0 where ROM uses r2/r1 (div=4).
extern int data_ov001_020ad470[];

void func_ov001_020ab3c4(char *c)
{
    *(int **)c = data_ov001_020ad470;
    *(int *)(c + 0xc) = 0;
    *(int *)(c + 0x10) = 0;
    *(unsigned char *)(((long long)(int)(c + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
}
