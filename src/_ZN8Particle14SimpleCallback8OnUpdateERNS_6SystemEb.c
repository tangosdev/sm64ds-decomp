/* _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb at 0x02022630
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

struct System { char pad[0x3a]; short val; };
struct SimpleCallback { char pad[4]; short out; };
int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(struct SimpleCallback *this, struct System *sys, char b) {
    this->out = sys->val;
    return 1;
}
