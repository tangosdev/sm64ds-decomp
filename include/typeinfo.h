#ifndef TYPEINFO_H
#define TYPEINFO_H

/* The cartridge keeps the canonical RTTI names and records for this hierarchy
 * at 0x0209a700..0x0209a7d7. Their typeinfo records prove the inheritance below:
 * both single- and multiple-inheritance descriptors derive directly from
 * __class_type_info, which in turn derives from std::type_info.
 *
 * These are the Metrowerks/MSL runtime types used by EAD's 2004 build, not
 * local stand-ins for the destructor code. Keeping the declarations together
 * lets mwcc own the vptr restoration, base destruction, and D0/D1/D2 variants.
 * Member spellings below are descriptive; the ROM proves their offsets and
 * roles, while only the class and namespace spellings survive in RTTI.
 */
namespace Memory {
void operator_delete2(void *ptr);
}

namespace std {

class type_info {
public:
    virtual ~type_info();

    void operator delete(void *ptr) { Memory::operator_delete2(ptr); }

protected:
    const char *typeName;
};

} // namespace std

namespace abi {

class __class_type_info : public std::type_info {
public:
    virtual ~__class_type_info();

    void operator delete(void *ptr) { Memory::operator_delete2(ptr); }
};

class __si_class_type_info : public __class_type_info {
public:
    virtual ~__si_class_type_info();

    void operator delete(void *ptr) { Memory::operator_delete2(ptr); }

private:
    const __class_type_info *baseType;
};

struct __base_class_type_info {
    const __class_type_info *baseType;
    long offsetFlags;
};

class __vmi_class_type_info : public __class_type_info {
public:
    virtual ~__vmi_class_type_info();

    void operator delete(void *ptr) { Memory::operator_delete2(ptr); }

private:
    unsigned long flags;
    unsigned long baseCount;
    __base_class_type_info baseInfo[1];
};

} // namespace abi

#endif
