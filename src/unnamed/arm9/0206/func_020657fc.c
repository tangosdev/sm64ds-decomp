
extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int x);
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
  int saved = (0, _ZN3IRQ7DisableEv());
  struct Node *head = (struct Node *) func_02065ae0();
  struct Node *cur;
  struct Node *prev;
  if (head == 0)
  {
    _ZN3IRQ7RestoreEj(saved);
    return 0;
  }
  if (head->key == key)
  {
    func_02065ad0((int) head->next);
    _ZN3IRQ7RestoreEj(saved);
    return head;
  }
  prev = head;
  cur = prev->next;
  while (cur != 0)
  {
    if (cur->key == key)
    {
      prev->next = cur->next;
      _ZN3IRQ7RestoreEj(saved);
      return cur;
    }
    prev = cur;
    cur = cur->next;
  }

  _ZN3IRQ7RestoreEj(saved);
  return 0;
}
