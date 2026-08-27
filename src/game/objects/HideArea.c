struct Area { char a; char b; char c; char d; char flag; char pad[7]; };
extern struct Area* data_0209f314;
void HideArea(int idx)
{
    data_0209f314[idx].flag = 0;
}
