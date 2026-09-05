// THE DS BACKUP MEDIUM, HOSTED -- the save chip the cartridge carries.
//
// port/ntr/backup.cpp is the body and carries the whole derivation. This is the
// small surface the rest of the port may touch: where the file is, how big it
// is, and the three operations the ROM's card driver asks the ARM7 for. Nothing
// here decides SAVE POLICY -- the game's own SaveData code does that, and it is
// the ROM's, linked from src/.
//
// THE FILE IS A CARTRIDGE IMAGE, NOT A CONTAINER. It is exactly kSize bytes
// with no host header of any kind, because the ROM writes its own framing into
// it (a 2-byte rolling checksum, the 8-byte record tag, a primary copy and a
// mirror). Deleting the file is a fresh cartridge; a fresh cartridge reads back
// as 0xFF everywhere, which is what an erased EEPROM cell holds.

#ifndef PORT_NTR_BACKUP_H
#define PORT_NTR_BACKUP_H

namespace ntr {
namespace backup {

// The chip the ROM asks for: src/func_0201a054.c calls
// func_02042f68(0xd01, "ds mario"), and 0xd01 is the 64 Kbit (8192 byte)
// serial EEPROM row of the card driver's own device table.
enum { kSize = 0x2000 };

// The erased state of an EEPROM cell, and therefore what a brand new save file
// is filled with and what a never-written region reads back as.
enum { kErased = 0xFF };

// The save file's absolute path, resolved once. Never null.
const char *path();

// Was the medium present on disk when it was first opened? False means the
// port created a fresh (0xFF) chip this run -- a new cartridge.
bool was_fresh();

// The three medium operations. `addr` is a byte offset into the chip, exactly
// the addressing the ROM's card driver uses. All three refuse an out-of-range
// or wrapping span rather than clamping it, which is the honest answer for a
// chip that does not have those cells.
bool read(unsigned addr, void *dst, unsigned len);
bool write(unsigned addr, const void *src, unsigned len);   // persists
bool verify(unsigned addr, const void *src, unsigned len);

// Statistics, for proofs and traces. Cheap; no I/O.
struct Stats {
    unsigned long long reads;
    unsigned long long writes;
    unsigned long long verifies;
    unsigned long long verify_failures;
    unsigned long long flushes;
    unsigned long long flush_failures;
};
Stats stats();

}  // namespace backup
}  // namespace ntr

#endif  // PORT_NTR_BACKUP_H
