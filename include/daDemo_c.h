#ifndef DADEMO_C_H
#define DADEMO_C_H

#ifdef __cplusplus

#include "ModelAnim.h"

/* The model helpers nested in daDemo_c share a virtual scale-bearing base.
   That virtual base is not speculative: it is the class shape that makes
   mwccarm destroy Model/ModelAnim before the Vector3 array and emits the
   retail -0x50 anmModel_c adjustment thunks. The remaining resource pointers
   trail the virtual base in the containing allocation and are therefore kept
   as evidenced raw offsets in the destructor bodies. */
struct ScaleHolder {
    Vector3 mScale[1];
};

struct daDemo_c {
    struct anmModel_c : ModelAnim, virtual ScaleHolder {
        virtual ~anmModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };

    struct simpleModel_c : Model, virtual ScaleHolder {
        virtual ~simpleModel_c();
        void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }
    };
};

#endif /* __cplusplus */

#endif /* DADEMO_C_H */
