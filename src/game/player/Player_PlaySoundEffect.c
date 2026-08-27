extern unsigned char data_0209b480;
extern signed char data_0208e428;
extern int func_02051f1c(int a, int b, int c);
extern int func_02051e60(int a, int b, int c, int d, int e, int f);

int Player_PlaySoundEffect(int arg0, int arg1, int arg2)
{
    if (data_0209b480 == 0) return 1;
    if (arg1 != 3)
        return func_02051f1c(arg0, arg1, arg2);
    return func_02051e60(arg0, -1, data_0208e428, -1, arg1, arg2);
}
