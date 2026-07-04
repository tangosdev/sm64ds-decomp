extern int data_020a6134[];

void func_020584d0(char* node)
{
    int cur = data_020a6134[3];
    int prev = 0;

    while (cur != 0 && *(unsigned int*)(cur + 0x70) < *(unsigned int*)(node + 0x70)) {
        prev = cur;
        cur = *(int*)(cur + 0x68);
    }

    if (prev == 0) {
        *(int*)(node + 0x68) = data_020a6134[3];
        data_020a6134[3] = (int)node;
    } else {
        *(int*)(node + 0x68) = *(int*)(prev + 0x68);
        *(int*)(prev + 0x68) = (int)node;
    }
}
