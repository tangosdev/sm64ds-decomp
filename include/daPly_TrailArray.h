/* new Vector3[50] for Mario's trail. Parsed before Vector3's empty
   constructor and destructor are defined, so the compiler passes both.
   The same expression after those bodies passes null. */
inline Vector3 *daPly_AllocTrailPositions() {
    return new Vector3[50];
}
