struct P{int x,y;};
extern struct P data_ov006_0213adb8;
int func_ov006_020c2994(struct P* c){
  int ip=1;
  struct P* g=&data_ov006_0213adb8;
  if(c->x==g->x){
    if(c->y!=g->y && c->x!=0) ;
    else ip=0;
  }
  return ip==0;
}
