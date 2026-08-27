#include "types.h"
extern s32 LoadArchive(s32 archiveID);
extern s32 GetOwnerLanguage(void);

void LoadTextNarcs(void)
{
    s32 archiveID;
    s32 lang;

    if (!LoadArchive(6))
        return;

    archiveID = 8;
    lang = GetOwnerLanguage();
    switch (lang)
    {
    case 2:
        archiveID = 9;
        break;
    case 3:
        archiveID = 10;
        break;
    case 4:
        archiveID = 11;
        break;
    case 5:
        archiveID = 12;
        break;
    default:
        break;
    }
    LoadArchive(archiveID);
}
