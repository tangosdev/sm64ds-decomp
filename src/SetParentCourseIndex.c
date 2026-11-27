#include "LinkSettings.h"

/* The parent only: post the course index. dScEntry_c::UpdateCourseSelect
   posts the chosen course (data_0209b2e4); the other consoles copy
   GetParentCourseIndex into it. */
void SetParentCourseIndex(u32 course) {
    if (GetLocalPlayerId() != 0)
        return;
    int v = GetLinkByte(0, LINK_BYTE_COURSE);
    SetLinkByte(LINK_BYTE_COURSE, (u8)(course | (v & ~LINK_COURSE_INDEX_MASK)));
}
