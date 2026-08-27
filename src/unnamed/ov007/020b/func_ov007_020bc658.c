extern int func_ov007_020c92ac();
extern void func_02046b64(int, int);

void func_ov007_020bc658(int *c, int *tbl, int count){
  int i;
  c[7]=count;
  c[8]=(int)tbl;
  c[9]=func_ov007_020c92ac();
  for(i=0;i<count;i++){
    int v=((int*)c[8])[i];
    if(v!=0){
      func_02046b64(*(int*)c[0], v);
    }
  }
}
