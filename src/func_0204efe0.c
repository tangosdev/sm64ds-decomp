/* func_0204efe0 at 0x020137fc
 * Converts a sublevel ID to a course ID, then opens the cannon for that course.
 */
typedef signed char s8;

extern s8 SublevelToLevel(s8 levelID);
extern void OpenCannon(s8 courseID);

void func_0204efe0(s8 levelID)
{
    s8 courseID = SublevelToLevel(levelID);
    OpenCannon(courseID);
}
