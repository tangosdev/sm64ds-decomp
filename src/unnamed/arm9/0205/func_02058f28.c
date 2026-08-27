extern unsigned int data_020a6378;

unsigned int func_02058df4(int idx);
void func_02058d6c(int idx, unsigned int val);
unsigned int OS_GetInitArenaLo(int id);
void func_02058d58(int idx, unsigned int val);

void func_02058f28(void)
{
    if (data_020a6378 != 0)
        return;
    data_020a6378 = 1;

    func_02058d6c(0, func_02058df4(0));
    func_02058d58(0, OS_GetInitArenaLo(0));

    func_02058d58(2, 0);
    func_02058d6c(2, 0);

    func_02058d6c(3, func_02058df4(3));
    func_02058d58(3, OS_GetInitArenaLo(3));

    func_02058d6c(4, func_02058df4(4));
    func_02058d58(4, OS_GetInitArenaLo(4));

    func_02058d6c(5, func_02058df4(5));
    func_02058d58(5, OS_GetInitArenaLo(5));

    func_02058d6c(6, func_02058df4(6));
    func_02058d58(6, OS_GetInitArenaLo(6));
}
