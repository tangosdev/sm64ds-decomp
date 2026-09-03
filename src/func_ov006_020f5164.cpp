//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142304[];
struct C { char pad[0x5318]; int idx; };
extern "C" int _ZN13dScMgMemory_c11UpdateCardsEv(C* c);
extern "C" int _ZN13dScMgMemory_c9StatePlayEv(C* c) {
    int j = c->idx;
    (c->*data_ov006_02142304[j].pmf)();
    return _ZN13dScMgMemory_c11UpdateCardsEv(c);
}
