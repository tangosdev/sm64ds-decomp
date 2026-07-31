typedef unsigned char u8;
typedef unsigned short u16;
extern u8 data_ov007_0210342c[];
extern void Sprite_SetAnimation(char *c, int r1, int r2, int r3, int sp0);

void func_ov007_020ae070(char *self)
{
    int *p = *(int**)self;
    u16 state = *(u16*)p;
    int ip2 = *(int*)((char*)*(int**)((char*)p + 4) + 0xc);
    int *g = *(int**)data_ov007_0210342c;
    short idx = *(short*)(*(int**)((char*)g + 0xc));

    if (state != 1) return;
    if (ip2 != 0) return;

    switch (idx) {
    case 3:  Sprite_SetAnimation(*(char**)((char*)self + 4), 0, 0, 0, 0); break;
    case 14: Sprite_SetAnimation(*(char**)((char*)self + 4), 3, 0, 0, 0); break;
    case 4:  Sprite_SetAnimation(*(char**)((char*)self + 4), 2, 0, 0, 0); break;
    case 5:  Sprite_SetAnimation(*(char**)((char*)self + 4), 1, 0, 0, 0); break;
    case 7:
        if (*(int*)((char*)g + 0x20) == 1)
            Sprite_SetAnimation(*(char**)((char*)self + 4), 1, 0, 0, 0);
        else
            Sprite_SetAnimation(*(char**)((char*)self + 4), 2, 0, 0, 0);
        break;
    case 8:  Sprite_SetAnimation(*(char**)((char*)self + 4), 4, 0, 0, 0); break;
    case 9:  Sprite_SetAnimation(*(char**)((char*)self + 4), 5, 0, 0, 0); break;
    case 10: Sprite_SetAnimation(*(char**)((char*)self + 4), 6, 0, 0, 0); break;
    case 12:
        if (*(int*)((char*)g + 0x58) == -1)
            Sprite_SetAnimation(*(char**)((char*)self + 4), 7, 0, 0, 0);
        else
            Sprite_SetAnimation(*(char**)((char*)self + 4), 8, 0, 0, 0);
        break;
    case 13: Sprite_SetAnimation(*(char**)((char*)self + 4), 9, 0, 0, 0); break;
    }
}
