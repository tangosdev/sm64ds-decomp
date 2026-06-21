/* LinkSilverStarAndStarMarker at 0x020e71d4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

void LinkSilverStarAndStarMarker(char *o, char *star) {
    if (star != 0) {
        *(int *)(o + 0x1cc) = *(int *)(star + 4);
        *(char *)(o + 0x1da) = *(int *)(star + 0x440);
        short v = *(short *)(star + 0xce);
        if (v >= 0) {
            *(short *)(o + 0x1d6) = v;
        }
        return;
    }
    *(short *)(o + 0x1d6) = -1;
    *(int *)(o + 0x1cc) = 0;
}
