#include "MessageBank.h"

extern int GetOwnerLanguage(void);
extern void *LoadFile(int id);
extern void ParseMessageBankSections(void);
extern void *gMessageBankBase;

void LoadMessageBankForLanguage(void)
{
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
    ParseMessageBankSections();
}
