//cpp
extern "C" {
int _ZNK10dBgCh_Actr10IsOnGroundEv(void* c);
void* _ZNK10dBgCh_Actr14GetFloorResultEv(void* c);
unsigned int _ZNK5dBgPi9GetClsnIDEv(void* r);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
}
struct dActor_c {
  virtual void v00(); virtual void v04(); virtual void v08(); virtual void v0c();
  virtual void v10(); virtual void v14(); virtual void v18(); virtual void v1c();
  virtual void v20(); virtual void v24(); virtual void v28(); virtual void v2c();
  virtual void v30(); virtual void v34(); virtual void v38(); virtual void v3c();
  virtual void v40(); virtual void v44(); virtual void v48(); virtual void v4c();
  virtual void v50(); virtual void m54(int arg);
};
extern "C" int func_ov002_020ef2a4(void* c, int arg){
  if(_ZNK10dBgCh_Actr10IsOnGroundEv(c)){
    void* res = _ZNK10dBgCh_Actr14GetFloorResultEv(c);
    if(_ZNK5dBgPi9GetClsnIDEv(res) != 0xffffffff){
      dActor_c* a = (dActor_c*)_ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(res));
      if(a != 0){
        a->m54(arg);
        return 1;
      }
    }
  }
  return 0;
}
