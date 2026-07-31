typedef int s32;
extern s32 GetOwnerLanguage(void);
s32 func_ov004_020ad674(void)
{
    switch (GetOwnerLanguage())
    {
    case 5:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 4;
    default:
        return 0;
    }
}
