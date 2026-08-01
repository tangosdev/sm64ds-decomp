//cpp
// @symbol func_ov100_02145550
// recovered name: daDoor_c_Behavior
/* recovered: renamed to Class_Method */
/* daDoor_c::Behavior - recovered from vtable slot identity */
struct Base {};
typedef void (Base::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern int func_ov100_02145370(char *c);
extern "C" int func_ov100_02145550(char *c) {
    int res = func_ov100_02145370(c);
    CallbackNode *node = *(CallbackNode**)((char*)c + 0x140);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)c;
        (base->*(node->callback))(res);
    }
    return 1;
}
