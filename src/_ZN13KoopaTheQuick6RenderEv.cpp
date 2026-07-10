//cpp
struct Model {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5(void* p);
  void HideMaterial(int a, int b);
};
extern "C" {
int _ZN13KoopaTheQuick6RenderEv(char* c){
  Model* m = (Model*)(c+0x300);
  m->HideMaterial(0, 1);
  ((Model*)(c+0x300))->v5(c+0x80);
  return 1;
}
}
