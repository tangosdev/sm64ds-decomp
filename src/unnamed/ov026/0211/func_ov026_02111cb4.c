
struct Vec3
{
  int x;
  int y;
  int z;
};
extern char *_ZN8dActor_c13ClosestPlayerEv(void *c);
extern short Vec3_HorzAngle(struct Vec3 *a, struct Vec3 *b);
extern int Vec3_HorzDist(struct Vec3 *a, struct Vec3 *b);
int func_ov026_02111cb4(char *c)
{
  char *p = _ZN8dActor_c13ClosestPlayerEv(c);
  if (p)
  {
    struct Vec3 *sp = (struct Vec3 *) (((int) p + 0x5c));
    struct Vec3 t;
    t.x = sp->x;
    t.y = sp->y;
    t.z = sp->z;
    *((short *) (c + 0x1b4)) = Vec3_HorzAngle((struct Vec3 *) (c + 0x5c), &t);
    *((int *) (c + 0x190)) = 0;
    *((int *) (c + 0x194)) = 0;
    *((int *) (c + 0x198)) = 0;
    *((int *) (c + 0x19c)) = t.x;
    *((int *) (c + 0x1a0)) = t.y;
    *((int *) (c + 0x1a4)) = t.z;
    *((int *) (c + 0x194)) = t.y;
    *((int *) (c + 0x198)) = Vec3_HorzDist((struct Vec3 *) (c + 0x1a8), &t);
  }
  return 1;
}
