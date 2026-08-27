/* GetLevelPart at 0x02013548
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern signed char data_02075264[];
signed char GetLevelPart(int idx) {
    return data_02075264[idx];
}
