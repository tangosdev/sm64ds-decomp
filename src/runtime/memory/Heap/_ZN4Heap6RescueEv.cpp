//cpp
// @symbol _ZN4Heap6RescueEv
/* Heap::Rescue() at 0x0203c634 -- plain forward to slot 7, the concrete heap's
 * last-ditch recovery. Called from Heap::ResizeToFit just before Crash().
 *
 * RETURN TYPE: void, from the definitions. The old shadow here declared its
 * fake slot as `int m()' and returned it, which is unobservable for a pure
 * forwarder -- r0 simply carries whatever the callee left -- so it byte-matched
 * while claiming a result that does not exist. Both overrides are void
 * (ExpandingHeap::VRescue, SolidHeap::VRescue), and the reconstruction that was
 * already sitting in _ZN4Heap11ResizeToFitEv.c said `void Rescue()' too. */
#include "Heap.h"

void Heap::Rescue()
{
    VRescue();
}
