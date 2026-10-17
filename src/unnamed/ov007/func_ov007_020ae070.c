#include "types.h"
extern u8 data_ov007_0210342c[];
extern void Sprite_SetAnimation(char *c, int r1, int r2, int r3, int sp0);

/* The icon this updates: an owner whose state gates the change, and the
 * sprite whose animation follows the menu's current mode. */
typedef struct {
    u16 state; /* 0x00 */
} IconOwner;

typedef struct {
    IconOwner *owner; /* 0x00 */
    char *sprite;     /* 0x04 */
} Icon;

void func_ov007_020ae070(char *self)
{
    Icon *icon = (Icon *)self;
    IconOwner *p = icon->owner;
    u16 state = p->state;
    int ip2 = *(int *)(*(char **)((char *)p + 4) + 0xc);
    int *g = *(int**)data_ov007_0210342c;
    short idx = *(short*)(*(int**)((char*)g + 0xc));

    if (state != 1) return;
    if (ip2 != 0) return;

    switch (idx) {
    case 3:
        Sprite_SetAnimation(icon->sprite, 0, 0, 0, 0);
        break;
    case 14:
        Sprite_SetAnimation(icon->sprite, 3, 0, 0, 0);
        break;
    case 4:
        Sprite_SetAnimation(icon->sprite, 2, 0, 0, 0);
        break;
    case 5:
        Sprite_SetAnimation(icon->sprite, 1, 0, 0, 0);
        break;
    case 7:
        if (*(int*)((char*)g + 0x20) == 1)
            Sprite_SetAnimation(icon->sprite, 1, 0, 0, 0);
        else
            Sprite_SetAnimation(icon->sprite, 2, 0, 0, 0);
        break;
    case 8:
        Sprite_SetAnimation(icon->sprite, 4, 0, 0, 0);
        break;
    case 9:
        Sprite_SetAnimation(icon->sprite, 5, 0, 0, 0);
        break;
    case 10:
        Sprite_SetAnimation(icon->sprite, 6, 0, 0, 0);
        break;
    case 12:
        if (*(int*)((char*)g + 0x58) == -1)
            Sprite_SetAnimation(icon->sprite, 7, 0, 0, 0);
        else
            Sprite_SetAnimation(icon->sprite, 8, 0, 0, 0);
        break;
    case 13:
        Sprite_SetAnimation(icon->sprite, 9, 0, 0, 0);
        break;
    }
}
