int IsLevelInsideCastle(int level)
{
    if (level == 2 || level == 4 || level == 5 || level == 0x32)
        return 1;
    return 0;
}
