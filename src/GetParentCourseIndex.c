#include "LinkSettings.h"

/* The course index the parent posted (SetParentCourseIndex). */
u8 GetParentCourseIndex(void) {
    return GetLinkByte(0, LINK_BYTE_COURSE) & LINK_COURSE_INDEX_MASK;
}
