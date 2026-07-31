//cpp
struct MgBounceAndPounce {
    int BeforeRender();
};

int MgBounceAndPounce::BeforeRender()
{
extern int func_ov004_020b04f4();
  extern int _ZN8Particle9RenderAllEv();
  if(func_ov004_020b04f4()==0) return 0;
  _ZN8Particle9RenderAllEv();
  return 1;

}
