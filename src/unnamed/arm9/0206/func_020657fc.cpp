//cpp
#include "types.h"
#include "IRQ.h"

extern "C" {

extern int *func_02065ae0(void);
extern void func_02065ad0(int v);
struct Node
{
  int *key;
  char pad[0x24];
  struct Node *next;
};
struct Node *func_020657fc(int *key)
{
    u32 saved = (0, IRQ::Disable());
    struct Node *head = (struct Node *)func_02065ae0();
    struct Node *cur;
    struct Node *prev;
    if (head == 0) {
        IRQ::Restore(saved);
        return 0;
    }
  if (head->key == key)
  {
    func_02065ad0((int) head->next);
    IRQ::Restore(saved);
    return head;
  }
  prev = head;
  cur = prev->next;
  while (cur != 0)
  {
    if (cur->key == key)
    {
      prev->next = cur->next;
      IRQ::Restore(saved);
      return cur;
    }
    prev = cur;
    cur = cur->next;
  }

  IRQ::Restore(saved);
  return 0;
}
}
