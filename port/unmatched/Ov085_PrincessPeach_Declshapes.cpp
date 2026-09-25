/* RETIRED HOST COPY. Nothing in this file is compiled any more.
 *
 * It held PrincessPeach's two per-frame helpers while MSVC refused their
 * matched sources, and both now run as the matched ROM code:
 *
 *   _ZN13PrincessPeach12UpdateLookAtEv  retired by run linkfull lane
 *       FACEFLIP1: src/_ZN13PrincessPeach12UpdateLookAtEv.cpp is on
 *       port/slice_w24_faceflip.txt.
 *
 *   _ZN13PrincessPeach21UpdateGroundCollisionEP10dBgCh_Actr  retired by run
 *       linkfull lane SMALLS2. The C4716 refusal this file was written for
 *       (an int body falling off the end) is gone from the synced src: the
 *       matched TU now defines the real member
 *       `void PrincessPeach::UpdateGroundCollision(dBgCh_Actr *)` and compiles
 *       clean under the port's own compile line. Its port/faces_sync.txt row
 *       is a REVERSE row now and the TU is on port/slice_w31_smalls2.txt. Its
 *       two ROM callers (ov085 relocs.txt, from:0x0212a0d0 and
 *       from:0x0212a1bc) are PrincessPeach::InitState0 and InitState4,
 *       matched C++ that calls the member directly.
 *
 * The file stays, empty, because three source lists in port/CMakeLists.txt
 * still name it; deleting it is a one-line change to each of those lists.
 */
