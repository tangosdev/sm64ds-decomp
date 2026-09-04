//cpp
struct C; typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov006_02142440[];
struct C { char pad[0x53d8]; int idx; };
extern "C" int _ZN14dScMgMemory2_c11UpdateCardsEv(C* c);
extern "C" int _ZN14dScMgMemory2_c9StatePlayEv(C* c) {
    int j = c->idx;
    (c->*data_ov006_02142440[j].pmf)();
    return _ZN14dScMgMemory2_c11UpdateCardsEv(c);
}
