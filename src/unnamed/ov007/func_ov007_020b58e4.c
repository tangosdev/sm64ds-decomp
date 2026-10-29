#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
#include "Ov007Work.h"
extern u8 data_ov007_0210342c[];

/* The 10-byte layout row func_ov007_020aea6c's item points at; names as in
 * func_ov007_020b5bf0, only the slot this file reads. */
typedef struct Ov007ItemLayout {
    u16 kind; /* 0x0 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b46b0 and func_ov007_020ae834, only the slots this file
 * touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00 */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388 */
    char pad_08[8];
    s32 fade; /* 0x10: Q12; selectable once fully faded in (>= 0x1000) */
    char pad_14[0x10];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

typedef char Ov007Item_layout_must_be_at_0x24[(int)&((Ov007Item *)0)->layout == 0x24 ? 1 : -1];

/* One of the manager's itemViews (func_ov007_020b4c04); names as there. */
typedef struct Ov007ItemView {
    Ov007Item *item;     /* 0x00 */
    Ov007Sprite *sprite; /* 0x04: func_ov007_020c44e4 */
} Ov007ItemView;

typedef char Ov007Work_transitionMode_must_be_at_0x20[(int)&((Ov007Work *)0)->transitionMode == 0x20
                                                          ? 1
                                                          : -1];

#define WORK (*(Ov007Work **)data_ov007_0210342c)

inline Ov007Item *inline_fn(char *arg0) {
    return ((Ov007ItemView *)arg0)->item;
}

int func_ov007_020b58e4(char *self)
{
  char *g = *((char **) data_ov007_0210342c);
  Ov007MenuState *b = WORK->states[2];
  int flag = b->stateTicks;
  Ov007Item *a = inline_fn(self);
  int sel = b->currentState;
  Ov007ItemLayout *r3ptr = a->layout;
  int r7 = flag == 0;
  Ov007MenuState *c = a->state;
  int lr_v = r3ptr->kind;
  int ip_v = a->id;
  int ret = c->requestedState;
  switch (sel)
  {
    case 0:
      if (r7)
    {
      ret = 1;
    }
      break;

    case 1:
      if (r7)
    {
          int d = b->previousState;
          if (d == 0xe) {
              if (lr_v == 0) {
                  ret = 3;
              } else if (lr_v == 7) {
                  ret = 4;
              } else if (lr_v == 6) {
                  ret = 5;
              }
          } else if ((lr_v == 0) || (lr_v == 6)) {
              ret = 0;
          } else {
              ret = 1;
          }
    }
      break;

    case 3:
      if (r7)
    {
      if (lr_v == 0)
      {
        ret = 3;
      }
      else
        if (lr_v == 6)
      {
          a->fade = 0;
          ret = 5;
      }
      else
        if (lr_v == 7)
      {
        ret = 4;
      }
    }
      break;

    case 2:
      if (r7)
    {
      if ((lr_v == 0) || (lr_v == 6))
      {
        ret = 4;
      }
    }
      break;

    case 7:
      if (r7)
    {
      if (lr_v == 2)
      {
          int e = WORK->transitionMode;
          if (((e == 1) && (ip_v == 4)) || ((e == 0) && ((ip_v == 3) || (ip_v == 5)))) {
              ret = 3;
          }
        if (ip_v == 6)
        {
          int f = *((int *) (((char *) g) + 0xf0));
          ret = 3;
          if (f == 1)
          {
              ((Ov007ItemView *)self)->sprite->texS = 0;
          }
          else
          {
              ((Ov007ItemView *)self)->sprite->texS = 0x28;
          }
        }
      }
    }
      break;

    case 4:
      if (r7)
    {
      if (ip_v == 3)
      {
        ret = 0;
      }
      else
        if (ip_v == 5)
      {
        ret = 3;
      }
    }
      break;

    case 5:
      if (r7)
    {
      if (ip_v == 4)
      {
        ret = 0;
      }
      else
        if (ip_v == 5)
      {
        ret = 4;
      }
    }
      break;

    case 6:
      if (r7)
    {
      if (lr_v == 2)
      {
        ret = 4;
      }
    }
      break;

    case 14:
      if (r7)
    {
      if (lr_v == 0)
      {
        ret = 4;
      }
      else
        if (lr_v == 7)
      {
        ret = 3;
      }
    }
      break;

    case 8:
      if (r7)
    {
          int t = WORK->states[1]->previousState;
          if (t < 0) {
              if (lr_v == 3) {
                  ret = 0;
              } else {
                  ret = 1;
              }
          } else if (lr_v == 3) {
              ret = (ip_v != 0x15) ? (5) : (3);
          } else if (lr_v == 4) {
              ret = 4;
          }
    }
      break;

    case 9:
      if (r7)
    {
      if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 6;
      }
      else
        if (ip_v == 0x15)
      {
        ret = 4;
      }
    }
    else
      if (flag == 1)
    {
      if (lr_v == 4)
      {
          int t = b->previousState;
          if (t == 8) {
              ret = 3;
          } else {
              a->fade = 0;
              ret = 5;
          }
      }
    }
      break;

    case 10:

    case 12:

    case 13:
      if (r7)
    {
      if (lr_v == 4)
      {
        ret = 4;
      }
      else
        if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 5;
      }
    }
      break;

    case 11:
      if (r7)
    {
      if (((u32) (ip_v - 0xe)) <= 2)
      {
        ret = 6;
      }
    }
      break;

  }

  return ret;
}
