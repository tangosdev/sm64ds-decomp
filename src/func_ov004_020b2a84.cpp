//cpp
extern int data_ov004_020bc0c0[];
extern int data_ov004_020beb68[];
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
extern void func_ov004_020b929c(void *);

struct ActorBase { virtual ~ActorBase(); };

extern "C" void *func_ov004_020b2a84(void *c) {
    *(int *)c = (int)data_ov004_020bc0c0;
    *(int *)data_ov004_020beb68 = 0;
    func_ov004_020b929c((char *)c + 0xf4);
    *(int *)c = (int)_ZTV5Stage;
    *(int *)c = (int)data_0208e4b8;
    ((ActorBase *)c)->ActorBase::~ActorBase();
    return c;
}
