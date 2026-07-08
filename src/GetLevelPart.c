/* GetLevelPart at 0x02013548
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern signed char LEVEL_PART_TABLE[];
signed char GetLevelPart(int idx) {
    return LEVEL_PART_TABLE[idx];
}
