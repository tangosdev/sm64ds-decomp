//cpp
extern int data_ov004_020bc0c0[];
extern int data_ov004_020beb68[];
extern int _ZTV5Scene[];
extern int _ZTV12ActorDerived[];
extern void func_ov004_020b929c(void *);

struct ActorBase { virtual ~ActorBase(); };

extern "C" void *func_ov004_020b29c0(void *c) {
    *(int *)c = (int)data_ov004_020bc0c0;
    *(int *)data_ov004_020beb68 = 0;
    func_ov004_020b929c((char *)c + 0xf4);
    *(int *)c = (int)_ZTV5Scene;
    *(int *)c = (int)_ZTV12ActorDerived;
    ((ActorBase *)c)->ActorBase::~ActorBase();
    return c;
}
