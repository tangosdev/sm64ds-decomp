//cpp
struct PMF { int a, b; };
extern "C" {
extern int func_ov081_021245e8(void*);
extern int func_ov081_02125488(void*, PMF*);
extern PMF data_ov081_02128ea4;
int func_ov081_02125038(char* c){
  if(func_ov081_021245e8(c) < 0x258000)
    func_ov081_02125488(c, &data_ov081_02128ea4);
  return 1;
}
}
