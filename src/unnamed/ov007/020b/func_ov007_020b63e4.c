extern char *data_ov007_0210342c;

void func_ov007_020c1d8c(int i);

void func_ov007_020b63e4(char **a);
void func_ov007_020b63e4(char **a)
{
    unsigned short v = *(unsigned short *)a[0];
    if ((unsigned int)(v - 10) <= 1) {
        *(short *)(*(char **)(*(char **)(*(char **)(data_ov007_0210342c + 0x138)) + 4) + 2) = 2;
    } else if (v == 9) {
        *(short *)(*(char **)(*(char **)(*(char **)(data_ov007_0210342c + 0x140)) + 4) + 2) = 2;
    }
    func_ov007_020c1d8c(0);
    *(short *)(*(char **)(a[0] + 4) + 2) = 2;
    *(unsigned int *)(data_ov007_0210342c + 0x180) = *(unsigned short *)a[0];
}
