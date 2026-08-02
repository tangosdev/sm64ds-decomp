#include "MessageBank.h"

extern int gMessageBankBase;
extern int gMessageBankHeader;
extern int gMessageInfoSection;
extern int gMessageDataSection;
extern int gMessageEntries;

void ParseMessageBankSections(void)
{
    int bankBase = gMessageBankBase;
    int infoSection = bankBase + 0x20;
    gMessageBankHeader = bankBase;
    gMessageInfoSection = infoSection;
    int dataSection = infoSection + *(int *)(infoSection + 4);
    gMessageDataSection = dataSection;
    gMessageEntries = bankBase + 0x30;
}
