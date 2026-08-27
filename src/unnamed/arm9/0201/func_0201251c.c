extern int func_02048a18(int x, int y);
extern int func_02048a1c(int x, int y, int z);
extern int Player_PlaySoundEffect(void* p, int b, int c);
extern int func_02048af4(void* p, int b, int c);

extern char data_0209b4a4;

void func_0201251c(int a, int b, int c, int d)
{
    int r4 = func_02048a18(b, d);
    if (func_02048a1c(c, a, r4) == 0) return;
    Player_PlaySoundEffect(&data_0209b4a4, a, r4);
    func_02048af4(&data_0209b4a4, c, d);
}
