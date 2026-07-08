struct Area { char a; char b; char c; char d; char flag; char pad[7]; };
extern struct Area* AREAS;
void ShowArea(int idx)
{
    AREAS[idx].flag = 1;
}
