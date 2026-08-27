extern int data_020a6134[];
void func_02058488(int target)
{
    int cur;
    int prev;
    cur = data_020a6134[3];
    prev = 0;
    while (cur != 0 && cur != target) {
        prev = cur;
        cur = *(int*)(cur + 0x68);
    }
    if (prev == 0)
        data_020a6134[3] = *(int*)(target + 0x68);
    else
        *(int*)(prev + 0x68) = *(int*)(target + 0x68);
}
