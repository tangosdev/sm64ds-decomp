/* IsCannonOpenInLevel at 0x02013818
 * Converts a sublevel ID to a course ID, then checks if cannon is open in that course.
 */
typedef signed char s8;

extern s8 SublevelToLevel(s8 levelID);
extern int IsCannonOpen(s8 courseID);

int IsCannonOpenInLevel(s8 levelID)
{
    s8 courseID = SublevelToLevel(levelID);
    return IsCannonOpen(courseID);
}
