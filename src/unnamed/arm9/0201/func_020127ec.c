extern void Player_PlaySoundEffect(void *obj, int a, int b);
extern void func_02012860(void *obj, int a, int b, int c, int d);

extern int data_0209b4a4;

void func_020127ec(int a0, int a1, int a2, int a3, int a4, int a5)
{
    Player_PlaySoundEffect(&data_0209b4a4, a0, a1);
    func_02012860(&data_0209b4a4, a2, a3, a4, a5);
}
