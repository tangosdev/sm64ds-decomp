typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct 
{
  u8 _pad[0x2A];
  u8 unk_2A;
  u8 _pad2B[2];
  u8 unk_2D;
  u8 _pad2E[2];
} Struct30;
typedef struct 
{
  s32 v[5];
} Five;
extern void func_ov006_020d99ec(char *c, int n);
extern const Five data_ov006_0213bcd8;
int func_ov006_020da174(Struct30 *base)
{
  s16 hist[6];
  Five order;
  s16 i;
  s32 j;
  s32 t;
  s32 n;
  s32 neg1;
  s32 neg2;
  s32 zero;
  Struct30 *p;
  s16 *h;
  s32 max_val;
  s32 second_val;
  s32 max_idx;
  s32 second_idx;
  max_val = 0;
  h = (s16 *) ((u32) ((u64) ((u32) hist)));
  h[0] = max_val;
  h[1] = max_val;
  h[2] = max_val;
  h[3] = max_val;
  h[4] = max_val;
  max_idx = 6;
  second_val = max_val;
  second_idx = max_idx;
  i = max_val;
  h[5] = max_val;
  zero = max_val;
  for (i = 0; i < 5; i++)
  {
    s32 idx = base[i].unk_2A;
    hist[idx] = (s16) (hist[idx] + 1);
  }
  p = base;

  for (i = 0; i < 6; i++)
  {
    s32 v = hist[i];
    if (max_val <= v)
    {
      second_val = max_val;
      second_idx = max_idx;
      max_val = v;
      max_idx = i;
    }
    else
      if (second_val <= v)
    {
      second_val = v;
      second_idx = i;
    }
  }

  if (max_val == second_val)
  {
    if (max_val > 1)
    {
      if (second_val > 1)
      {
        p = base;
        for (t = 0; t < 5; t++)
        {
          s32 c = p->unk_2A;
          if (max_idx == c)
          {
            break;
          }
          if (second_idx == c)
          {
            s32 tmp = second_idx;
            second_idx = max_idx;
            max_idx = tmp;
            break;
          }
          p = (Struct30 *) (((u32) p) + 0x30);
        }

      }
    }
  }
  order = data_ov006_0213bcd8;
  n = 4;
  zero = 0;
  neg1 = -1;
  neg2 = -2;

  {
    do
    {
      s32 k = 0;
      while (k < n)
      {
        s32 v1 = order.v[k];
        s32 stride = 0x30;
        Struct30 *ptr1 = (Struct30 *) (((u32) base) + (v1 * stride));
        s32 u1 = ptr1->unk_2A;
        s32 next = k + 1;
        s32 v2;
        Struct30 *ptr2 = (Struct30 *) (((u32) base) + (order.v[next] * stride));
        s32 u2 = ptr2->unk_2A;
        if (u1 == max_idx)
        {
          if (max_val > 1)
          {
            u1 = neg2;
            goto u1_done;
          }
        }
        if (u1 == second_idx)
        {
          if (second_val > 1)
          {
            u1 = neg1;
            goto u1_done;
          }
        }
        u1 = zero;
        u1_done:
        if (u2 == max_idx)
        {
          if (max_val > 1)
          {
            u2 = neg2;
            goto u2_done;
          }
        }

        if (u2 == second_idx)
        {
          if (second_val > 1)
          {
            u2 = neg1;
            goto u2_done;
          }
        }
        u2 = zero;
        u2_done:
        if (u1 > u2)
        {
          order.v[k] = order.v[next];
          order.v[next] = v1;
        }

        k++;
      }

      n = n - 1;
    }
    while (n > 0);
  }

  {
    Struct30 *q = base;
    s32 one = 1;
    s32 z;
    j = 0;
    z = j;
    for (; j < 5; j++)
    {
      s32 c = q->unk_2A;
      if (max_idx == c)
      {
        if (max_val > 1)
        {
          goto store1c;
        }
      }
      if (second_idx == c)
      {
        if (second_val <= 1)
        {
          goto store0c;
        }
      }
      else
      {
        goto store0c;
      }
      store1c:
      q->unk_2D = one;

      goto flag_nextc;
      store0c:
      q->unk_2D = z;

      flag_nextc:
      q = (Struct30 *) (((u32) q) + 0x30);

    }

  }
  for (j = 0; j < 5; j++)
  {
    s32 v = order.v[j];
    Struct30 *ptr = (Struct30 *) (((u32) base) + (v * 0x30));
    func_ov006_020d99ec((char *) ptr, j);
  }

  for (j = 0; j < 5; j++)
  {
    if (j != order.v[j])
    {
      return 1;
    }
  }

  return 0;
}
