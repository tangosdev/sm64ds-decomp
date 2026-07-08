extern signed char ENDLESS_STAIRS_FOG_AREA_ID;
extern signed char AREA_ID;
extern int ENDLESS_STAIRS_FOG_PTR;

int func_ov002_020f1c20(void)
{
    if (ENDLESS_STAIRS_FOG_AREA_ID == AREA_ID)
        return ENDLESS_STAIRS_FOG_PTR;
    return 0;
}
