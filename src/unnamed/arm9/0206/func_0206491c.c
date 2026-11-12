#include "nitro/wbt.h"

extern WBTBlockInfoList *func_02065ae0(void);
extern int func_02065bc0(void);
extern int data_0209a064;
extern int data_0209a068;
extern int data_0209a06c;

int func_0206491c(unsigned int key, int val)
{
  int n;
  WBTBlockInfoList *node;
  int q;

  node = func_02065ae0();
  if (key < 0x3e8)
    return val;

  while (node != 0)
  {
      if (node->data_info.id == key) {
          if (key == data_0209a068) {
              n = 0;
              while (val == data_0209a064 || val == data_0209a06c) {
                  val += 1;
                  q = func_02065bc0();
                  q = (node->data_info.block_size + q - 1) / func_02065bc0();
                  q -= 1;
                  if (val > q)
                      val = 0;
                  n += 1;
                  if (n > 2)
                      break;
              }
          }

          data_0209a068 = key;
          data_0209a06c = data_0209a064;
          data_0209a064 = val;
          return val;
      }
    node = node->next;
  }

  return -1;
}
