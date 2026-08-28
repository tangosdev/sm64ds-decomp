//cpp
#include "Sound.h"

extern "C" {

int func_02051bd0(void *thiz, unsigned int a, unsigned int b, unsigned int c, void *entry, unsigned int id);

}

extern "C" int func_02051fb4(void *thiz, unsigned int id) {
    Sound::InfoSequenceEntry *e = Sound::InfoSequenceEntry::GetWithID(id);
    if (!e) return 0;
    return func_02051bd0(thiz, e->playerNumber, e->bankId, e->playerPriority, e, id);
}
