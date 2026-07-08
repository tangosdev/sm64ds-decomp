extern int FIRST_ACTOR_LIST_NODE[];
extern int *func_02043f4c(int *a, unsigned int j, int b);
int _ZN5Actor15FindWithActorIDEjPS_(unsigned int j, int p) {
  int *r;
  if (p) r = func_02043f4c(FIRST_ACTOR_LIST_NODE, j, p+0x50);
  else r = func_02043f4c(FIRST_ACTOR_LIST_NODE, j, 0);
  if (r) return r[2];
  return 0;
}
