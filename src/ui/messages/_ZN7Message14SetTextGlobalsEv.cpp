//cpp
#include "Message.h"
#include "MessageBank.h"

extern int gMessageBankBase;
extern int gMessageBankHeader;
extern int gMessageInfoSection;
extern int gMessageDataSection;
extern int gMessageEntries;

/* Splits the loaded message bank into the pointers the renderer reads; the
   layout it pins is spelled out in MessageBank.h. The VS build's twin is
   Message::SetTextGlobalsVS (word-identical with relocations masked). */
void Message::SetTextGlobals() {
    int bankBase = gMessageBankBase;
    int infoSection = bankBase + 0x20;
    gMessageBankHeader = bankBase;
    gMessageInfoSection = infoSection;
    int dataSection = infoSection + *(int *)(infoSection + 4);
    gMessageDataSection = dataSection;
    gMessageEntries = bankBase + 0x30;
}
