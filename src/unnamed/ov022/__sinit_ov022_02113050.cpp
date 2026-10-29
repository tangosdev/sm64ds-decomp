//cpp
/* Static initializer for the rolling log's model and collision handles
 * (fl_log.bmd, fl_log.kcl), each registered with __register_global_object
 * (func_020731dc) on a 12-byte node.
 *
 * Not folded: the owner is daObjFlMaruta_c. Its resource list
 * data_ov022_02112c9c points at both handles, and this .init sits between
 * daObjFl_Fall_Block_c's and the one d_a_obj_fl_maruta.cpp emits, with no
 * room in .text for another TU. But d_a_obj_fl_maruta.cpp merges
 * daObjFlMaruta_c and daObj_volcanoCannon_c into one object, and that object
 * already emits the cannon's initializer at 0x021130bc. One object cannot
 * emit two, so this fold needs the merged file split back into two TUs. */
#include "SharedFilePtr.h"

/* Runtime asset handles resolved through the overlay 0 file table. */
enum sinit_ov022_02113050_AssetId { ASSET_FL_LOG_BMD = 0x05fb, ASSET_FL_LOG_KCL = 0x05fc };

/* local extern: SharedFilePtr.cpp defines these veneers with C linkage, and
   SharedFilePtr.h, the only header for that file, does not declare them. */
extern "C" {
void *func_02017acc(void *, unsigned int); /* model handle constructor */
int func_02017ab4(int);                    /* model handle destructor */
void *func_02017b4c(void *, unsigned int); /* collision handle constructor */
int SharedFilePtr_Destruct_Clsn(int);
void func_020731dc(void *, void *, void **); /* __register_global_object */
}

extern SharedFilePtr gFlLogModelFile;
extern SharedFilePtr gFlLogCollisionFile;
extern void *data_ov022_02114678[3];
extern void *data_ov022_02114684[3];

extern "C" void __sinit_ov022_02113050(void) {
    func_02017acc(&gFlLogModelFile, ASSET_FL_LOG_BMD);
    func_020731dc(&gFlLogModelFile, (void *)func_02017ab4, data_ov022_02114678);
    func_02017b4c(&gFlLogCollisionFile, ASSET_FL_LOG_KCL);
    func_020731dc(&gFlLogCollisionFile, (void *)SharedFilePtr_Destruct_Clsn, data_ov022_02114684);
}
