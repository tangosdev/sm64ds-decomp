//cpp
// @symbol _ZN4Door8BehaviorEv
// recovered name: Door::Behavior
/* recovered: renamed to Class_Method, vtable slot 6 */
/* Door::Behavior -- vtable slot 6, ov100 0x02145550. Same idiom as
 * _ZN4Door13InitResourcesEv.c: declared as an override in include/Door.h,
 * defined here as an extern "C" free function under the mangled symbol. */
struct Base {};
typedef void (Base::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern "C" {
extern int func_ov100_02145370(char *c);
}
extern "C" int _ZN4Door8BehaviorEv(char *c) {
    int res = func_ov100_02145370(c);
    CallbackNode *node = *(CallbackNode**)((char*)c + 0x140);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)c;
        (base->*(node->callback))(res);
    }
    return 1;
}
