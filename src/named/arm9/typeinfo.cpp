//cpp
#include "typeinfo.h"

// Deferred codegen emits each destructor's D2, D0, D1 group in that order and the
// groups in reverse definition order, so the definitions below run base-to-leaf
// to land cartridge order: vmi D0/D1, si D0/D1 (their emitted D2s deadstrip),
// then __class_type_info and std::type_info D2/D0/D1.

// @symbol _ZNSt9type_infoD2Ev
// @symbol _ZNSt9type_infoD0Ev
// @symbol _ZNSt9type_infoD1Ev
std::type_info::~type_info()
{
}

// @symbol _ZN3abi17__class_type_infoD2Ev
// @symbol _ZN3abi17__class_type_infoD0Ev
// @symbol _ZN3abi17__class_type_infoD1Ev
abi::__class_type_info::~__class_type_info()
{
}

// @symbol _ZN3abi20__si_class_type_infoD0Ev
// @symbol _ZN3abi20__si_class_type_infoD1Ev
abi::__si_class_type_info::~__si_class_type_info()
{
}

// @symbol _ZN3abi21__vmi_class_type_infoD0Ev
// @symbol _ZN3abi21__vmi_class_type_infoD1Ev
abi::__vmi_class_type_info::~__vmi_class_type_info()
{
}
