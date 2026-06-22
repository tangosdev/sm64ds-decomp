//cpp
extern "C" {

struct SeqEntry {
    char pad0[4];
    unsigned short h4;
    char pad6[2];
    unsigned char b8;
    unsigned char b9;
};

int func_02051bd0(void *thiz, unsigned int a, unsigned int b, unsigned int c, void *entry, unsigned int id);

}

namespace Sound {
struct InfoSequenceEntry {
    static SeqEntry *GetWithID(unsigned int id);
};
}

extern "C" int func_02051fb4(void *thiz, unsigned int id) {
    SeqEntry *e = Sound::InfoSequenceEntry::GetWithID(id);
    if (!e) return 0;
    return func_02051bd0(thiz, e->b9, e->h4, e->b8, e, id);
}
