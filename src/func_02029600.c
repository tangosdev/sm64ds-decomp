extern unsigned char CURRENT_GAMEMODE;
extern unsigned char CUR_MINIMAP_ID;
extern int GetMinimapID(int a, int b);

int func_02029600(int x)
{
    int b = (int)(CURRENT_GAMEMODE == 2);
    if (b == 0) {
        unsigned char saved = CUR_MINIMAP_ID;
        int r = GetMinimapID(x, -1);
        if (r == saved)
            return 1;
    }
    return 0;
}
