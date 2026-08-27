
extern int func_0205b63c(int arg);
extern void func_0205aa68(void *a, int b, int c);
struct E
{
  int field0;
  int field4;
};
extern struct E data_020a5578[];
void func_0204fe5c(char *obj, int arg1)
{
  unsigned char new_var;
  int i = 0;
  *((int *) (obj + 0x24)) = arg1;
  if ((*((int *) (obj + 0x30))) > 0)
  {
    if (1)
    {
    }
    do
    {
      unsigned char idx = new_var = *((unsigned char *) ((obj + 0x34) + i));
      int r = func_0205b63c((*((int *) (obj + 0x24))) + data_020a5578[idx].field4);
      func_0205aa68((void *) (1 << idx), r & 0xff, r >> 8);
      i++;
    }
    while (i < (*((int *) (obj + 0x30))));
  }
}
