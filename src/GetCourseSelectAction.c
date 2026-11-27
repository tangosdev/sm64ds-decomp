#include "LinkSettings.h"

/* The course-select action the parent posted (SetCourseSelectAction). */
u8 GetCourseSelectAction(void) {
    return GetLinkByte(0, LINK_BYTE_COURSE_ACTION) & LINK_COURSE_ACTION_MASK;
}
