//cpp
#include "Message.h"
#include "MessageBank.h"

extern "C" {
extern int GetOwnerLanguage(void);
extern void *LoadFile(int id);
}
extern void *gMessageBankBase;

/* Loads the message bank for the console's language and splits it with
   SetTextGlobals. The VS build's Message::LoadTextVS has the same descending
   language chain, but points at text tables linked into arm9 instead of
   loading a file. */
void Message::LoadText() {
    if (GetOwnerLanguage() == 5) {
        gMessageBankBase = LoadFile(MESSAGE_BANK_SPN_ASSET);
    } else if (GetOwnerLanguage() == 4) {
        gMessageBankBase = LoadFile(MESSAGE_BANK_ITL_ASSET);
    } else if (GetOwnerLanguage() == 3) {
        gMessageBankBase = LoadFile(MESSAGE_BANK_GMN_ASSET);
    } else if (GetOwnerLanguage() == 2) {
        gMessageBankBase = LoadFile(MESSAGE_BANK_FRN_ASSET);
    } else {
        gMessageBankBase = LoadFile(MESSAGE_BANK_ENG_ASSET);
    }
    SetTextGlobals();
}
