//cpp
extern "C" {
struct ActorBase; struct Vector3;
extern int _ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(void* c, ActorBase* a, unsigned int b, const Vector3* v, unsigned int d, unsigned int e);
int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(char* c, ActorBase* a, unsigned int b, const Vector3* v, unsigned char d, unsigned char e){
  if(_ZN6Player12ShowMessage2ER9ActorBasejPK7Vector3jj(c,a,b,v,d,e)){
    *(unsigned char*)(c+0x6e5)=1;
    return 1;
  }
  return 0;
}
}
