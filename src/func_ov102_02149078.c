struct Actor;
extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern signed char LEVEL_ID;
extern int WATER_HEIGHT;

int func_ov102_02149078(struct Actor *self)
{
    if (LEVEL_ID == 0x15) {
        if ((int)(*(int*)((char*)self + 0x3e0) - 0x32000) <= WATER_HEIGHT)
            return 1;
    } else {
        if (LEVEL_ID == 0x21)
            return 0;
        if (*(unsigned char*)((char*)_ZN5Actor13ClosestPlayerEv() + 0x706))
            return 1;
    }
    return 0;
}
