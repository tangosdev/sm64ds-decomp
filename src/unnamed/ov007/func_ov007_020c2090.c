struct E { int w[1]; };
void func_ov007_020c2090(struct E* c){
  int i;
  for(i=0;i<0xc;i++){ c[i].w[0xd] = 0; c[i].w[1] = c[i].w[0xd]; }
}
