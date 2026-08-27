typedef int s32;

extern s32 GetOwnerLanguage(void);

s32 func_0200f0bc(void)
{
    s32 lang = GetOwnerLanguage();
    switch (lang)
    {
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    default:
        return 0;
    }
}
