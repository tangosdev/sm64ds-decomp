/* Forget the front-end message bank and its decoded section pointers.
 * This resets references only; it does not free the caller-owned bank.
 * These four words are used only by lookup/reset/bind at 020c94a0..020c95fc.
 * Preserve the existing integer declarations until the pointer ABI is unified.
 */
extern int data_ov007_02104c20; /* bank header */
extern int data_ov007_02104c1c; /* INF1 header */
extern int data_ov007_02104c18; /* INF1 entries */
extern int data_ov007_02104c24; /* DAT1 payload */

void func_ov007_020c94e4(void) {
    data_ov007_02104c20 = 0;
    data_ov007_02104c1c = 0;
    data_ov007_02104c18 = 0;
    data_ov007_02104c24 = 0;
}
