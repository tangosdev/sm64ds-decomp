
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, unsigned int, int);
extern void *data_ov006_02140580;
struct Vec2i { int x; int y; };
extern struct Vec2i data_ov006_0213b16c;
void func_ov006_020c9e7c(unsigned char *c)
{
  *((short *) (c + 0x6c)) = 0x3c;
  *((int *) (c + 0x28)) = 0x100000;
  *((int *) (c + 0x30)) = *((int *) (c + 0x24));
  *((int *) (c + 0x34)) = *((int *) (c + 0x28));
  *((int *) (c + 0x38)) = *((int *) (c + 0x2c));
  *((int *) (c + 0x40)) = 0;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x78, data_ov006_02140580, 0x40000000, 0x800, 0);
  *((struct Vec2i *) (c + 0x70)) = data_ov006_0213b16c;
}
