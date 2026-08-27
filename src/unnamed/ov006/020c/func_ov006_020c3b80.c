extern int data_ov006_0213aee0[];
int func_ov006_020c3b80(int* c){
  int* g=data_ov006_0213aee0;
  int r2=c[0];
  int ip=1;
  if(r2==g[0]){
    if(c[1]==g[1] || r2==0) ip=0;
  }
  return ip==0;
}
