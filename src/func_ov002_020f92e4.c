// @symbol func_ov002_020f92e4
// @emits Fireball_OnYoshiTryEat
/* recovered: renamed to Class_Method */
/* daFPknBall_c::OnYoshiTryEat - recovered from vtable slot identity */
int Fireball_OnYoshiTryEat(char *p)
{
    unsigned char b = *(unsigned char *)(p + 0x36d);
    if (b != 0 && b != 4)
        return 5;
    return 0;
}
