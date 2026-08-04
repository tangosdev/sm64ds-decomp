
struct Vec3
{
  int x;
  int y;
  int z;
};
extern int DotVec3(struct Vec3 *a, struct Vec3 *b);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern int LenVec3(struct Vec3 *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern int data_ov006_0212e0f0[];
void func_ov006_020cf040(char *sl, void *arg1, struct Vec3 *r2)
{
  int i;
  int n5 = -DotVec3(r2, (struct Vec3 *) (sl + 0x14));
  int *op = (int *) (sl + 0x29c);
  char *vp = sl + 0x11c;
  int new_var;
  int *dp = data_ov006_0212e0f0;
  for (i = 0; i < 4; i++)
  {
    int j;
    for (j = 0; j < 4; j++)
    {
      struct Vec3 d;
      SubVec3((struct Vec3 *) vp, (struct Vec3 *) arg1, &d);
      int r4 = (long long) (*((int *) (sl + 0x58)));
      int len = LenVec3(&d);
      int f = _ZN4cstd4fdivEii(r4, r4 + len);
      int t = (int) (((((long long) n5) * (*dp)) + 0x800) >> 12);
      new_var = (int) (((((long long) t) * f) + 0x800) >> 12);
      *op = new_var;
      vp += 0xc;
      dp++;
      op++;
    }

  }

}
