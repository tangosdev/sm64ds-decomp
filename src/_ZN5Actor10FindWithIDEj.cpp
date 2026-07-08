//cpp
extern "C" {
struct Actor;
extern struct Actor* FIRST_ACTOR_LIST_NODE;
extern struct Actor* func_02043f98(struct Actor**, unsigned int);
struct Actor* _ZN5Actor10FindWithIDEj(unsigned int id){
  struct Actor* p = func_02043f98(&FIRST_ACTOR_LIST_NODE, id);
  if(p) return *(struct Actor**)((char*)p+8);
  return 0;
}
}
