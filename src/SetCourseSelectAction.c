#include "LinkSettings.h"

/* The parent only: post the course-select action. dScEntry_c::
   UpdateCourseSelect posts 0 for a touched course, 1 or 2 for SELECT or
   START, then raises the go signal (RaiseGoSignal); GetCourseSelectAction
   reads it back on every console. */
void SetCourseSelectAction(u32 action) {
    if (GetLocalPlayerId() != 0)
        return;
    int v = GetLinkByte(0, LINK_BYTE_COURSE_ACTION);
    SetLinkByte(LINK_BYTE_COURSE_ACTION, (u8)(action | (v & ~LINK_COURSE_ACTION_MASK)));
}
