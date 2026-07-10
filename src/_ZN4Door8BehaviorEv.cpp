//cpp
struct Base {};
typedef void (Base::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern int func_ov100_02145370(char *c);
extern "C" int _ZN4Door8BehaviorEv(char *c) {
    int res = func_ov100_02145370(c);
    CallbackNode *node = *(CallbackNode**)((char*)c + 0x110);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)c;
        (base->*(node->callback))(res);
    }
    return 1;
}
