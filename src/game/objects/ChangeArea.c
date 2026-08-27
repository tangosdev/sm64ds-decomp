typedef signed char s8;

extern s8 data_02092120;

extern void HideArea(void);
extern void ShowArea(int areaID);

void ChangeArea(int areaID)
{
    if (data_02092120 >= 0)
        HideArea();
    data_02092120 = (s8)areaID;
    ShowArea(areaID);
}
