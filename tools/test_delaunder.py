"""The cv-qualifier family: a cast that adds volatile and then takes it straight back.

Both idioms were used to match `dBgW_Kc::DetectClsn(dBgCh_SphCrr&)` and no gate in the
tree could see either, which is what these kinds are for. The negative cases matter at
least as much as the positive ones -- genuine hardware volatile must never be counted,
or the metric is noise and the next person will paper over it.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import delaunder  # noqa: E402

CV = delaunder.CV_KINDS


def kinds(text, ks=CV):
    return [s[0] for s in delaunder.find_sites(text, ks)]


class CvCast(unittest.TestCase):
    """A cv-qualified cast of a PRVALUE. The language discards the qualifier, so it
    emits nothing and can only be there to steer the compiler."""

    def test_round_trip_scalar(self):
        self.assertEqual(kinds("rsc = (s32)(volatile s32)rsc;\n"), ["CVCAST"])

    def test_bare_volatile_cast_in_an_expression(self):
        self.assertEqual(kinds("pos.x = px * (volatile int)m;\n"), ["CVCAST"])

    def test_const_volatile_spelling(self):
        self.assertEqual(kinds("y = (u32)(const volatile u32)y;\n"), ["CVCAST"])

    def test_plain_const_cast_is_not_a_hack(self):
        # `const` on an rvalue is idiomatic and does not move codegen; only volatile does.
        self.assertEqual(kinds("y = (const s32)x;\n"), [])

    def test_plain_widening_cast_is_not_ours(self):
        self.assertEqual(kinds("y = (s32)x;\n"), [])


class HardwareVolatileIsReal(unittest.TestCase):
    """Adding volatile to a POINTER is how you talk to the DS. Never count it."""

    def test_mmio_pointer_cast(self):
        self.assertEqual(kinds("volatile u16 *ime = (volatile u16 *)0x4000208;\n"), [])

    def test_mmio_store_through_a_cast(self):
        self.assertEqual(kinds("*(volatile u64 *)0x40002b8 = x;\n"), [])

    def test_declaring_a_volatile_pointer_alone(self):
        self.assertEqual(kinds("const volatile Vector3 *c;\n"), [])


class CvStrip(unittest.TestCase):
    """Declared volatile so the loads are not CSEd, then cast un-volatile at each read."""

    DECL = "    const volatile Vector3 *c;\n"

    def test_cast_off_at_the_use_site(self):
        t = self.DECL + "    rawX = ((const Vector3 *)c)->x >> 6;\n"
        self.assertEqual(kinds(t), ["CVSTRIP"])

    def test_one_site_per_read(self):
        t = (self.DECL
             + "    rawX = ((const Vector3 *)c)->x >> 6;\n"
             + "    rawY = ((const Vector3 *)c)->y >> 6;\n"
             + "    rawZ = ((const Vector3 *)c)->z >> 6;\n")
        self.assertEqual(kinds(t), ["CVSTRIP"] * 3)

    def test_volatile_void_pointer_laundered_to_void(self):
        t = "{ volatile void* p = (void*)(c+0x4f38); f((void*)p); }\n"
        self.assertEqual(kinds(t), ["CVSTRIP"])

    def test_needs_the_declaration(self):
        # The same cast on a pointer nobody declared volatile is an ordinary cast.
        t = "    const Vector3 *c;\n    rawX = ((const Vector3 *)c)->x >> 6;\n"
        self.assertEqual(kinds(t), [])

    def test_reading_through_the_volatile_pointer_is_not_a_strip(self):
        t = self.DECL + "    rawX = c->x >> 6;\n"
        self.assertEqual(kinds(t), [])


class ProseIsNotCode(unittest.TestCase):
    """The launder metric moved to code precisely so rewording could not change it."""

    def test_comment_is_masked(self):
        t = "/* `(s32)(volatile s32)rsc` is what demotes the local. */\nint x;\n"
        self.assertEqual(kinds(t), [])

    def test_line_comment_is_masked(self):
        t = "// pos.x = px * (volatile int)m;\nint x;\n"
        self.assertEqual(kinds(t), [])


class ExistingKindsUnaffected(unittest.TestCase):
    """The three older idioms must count exactly as before -- a Q12 multiply is still a
    WIDEN, and the cv kinds must not silently fold into that number."""

    def test_widen_still_found(self):
        t = "#define FX12(a, b) ((s32)((((s64)(a) * (b)) + 0x800) >> 12))\n"
        self.assertEqual(kinds(t, delaunder.LAUNDER_KINDS), ["WIDEN"])

    def test_widen_is_not_a_cv_site(self):
        t = "#define FX12(a, b) ((s32)((((s64)(a) * (b)) + 0x800) >> 12))\n"
        self.assertEqual(kinds(t, CV), [])

    def test_families_are_disjoint(self):
        self.assertEqual(set(delaunder.LAUNDER_KINDS) & set(CV), set())
        for k in delaunder.LAUNDER_KINDS + CV:
            self.assertIn(k, delaunder.KINDS)


if __name__ == "__main__":
    unittest.main()
