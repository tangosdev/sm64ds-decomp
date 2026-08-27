extern int data_ov006_0213ac90[];
int func_ov006_020c16b4(char *c)
{
    int *G = data_ov006_0213ac90;
    int r3 = *(int *)c;
    int r4 = 0;
    int lr = 1;
    if (r3 == G[0] && (*(int *)(c + 4) == G[1] || r3 == 0))
        lr = 0;
    if (lr == 0 && *(int *)(c + 0x7c) == *(int *)(c + 0x1fc))
        r4 = 1;
    return r4;
}
