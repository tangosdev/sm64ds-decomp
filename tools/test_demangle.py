"""Pure-Python type-decoding controls; no compiler or ROM dependency."""
import json
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import demangle as D


class QualifierTests(unittest.TestCase):
    def args(self, symbol):
        return D.demangle(symbol)["args"]

    def test_real_g2x_interfaces(self):
        expected = {
            "_ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii": [
                "unsigned short volatile *", "Matrix2x2 *", "int", "int", "int", "int"],
            "_ZN3G2x13SetBlendAlphaEPVttttj": [
                "unsigned short volatile *", "unsigned short", "unsigned short",
                "unsigned short", "unsigned int"],
            "_ZN3G2x18SetBlendBrightnessEPVtts": [
                "unsigned short volatile *", "unsigned short", "short"],
        }
        for symbol, args in expected.items():
            with self.subTest(symbol=symbol):
                actual = D.demangle(symbol)
                self.assertEqual(actual["args"], args)
                self.assertEqual(actual["nargs"], len(args))

    def test_pointee_and_pointer_qualification_are_distinct(self):
        self.assertEqual(self.args("_Z1fPVtVPt"),
                         ["unsigned short volatile *", "unsigned short * volatile"])

    def test_consecutive_qualifiers_have_one_substitution_entry(self):
        self.assertEqual(self.args("_Z1fPVKiS0_"),
                         ["int const volatile *", "int const volatile *"])

    def test_class_and_full_qualified_type_are_substitutable(self):
        self.assertEqual(self.args("_Z1fPVK3FooPS_PS0_S1_"),
                         ["Foo const volatile *", "Foo *", "Foo const volatile *",
                          "Foo const volatile *"])

    def test_nested_pointer_qualifier_levels_are_preserved(self):
        self.assertEqual(self.args("_Z1fPKPVt"), ["unsigned short volatile * const *"])

    def test_restrict_group_uses_one_entry(self):
        self.assertEqual(self.args("_Z1fPrVKiS0_"),
                         ["int const volatile restrict *", "int const volatile restrict *"])

    def test_truncated_or_invalid_qualifier_groups_stay_unknown(self):
        for encoded in ("PV", "PK", "PKVi", "PVVi"):
            with self.subTest(encoded=encoded):
                self.assertTrue(any("T" in a for a in self.args("_Z1f" + encoded)))


class FunctionTypeTests(unittest.TestCase):
    def args(self, encoded):
        return D.demangle("_Z1f" + encoded)["args"]

    def test_real_allocator_callback_keeps_its_nested_parameters(self):
        actual = D.demangle("_ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj")
        self.assertEqual(actual["qualified"], "ExpandingHeapAllocator::DeallocateAll")
        self.assertEqual(actual["args"], [
            "void (*)(void *, ExpandingHeapAllocator *, unsigned int)", "unsigned int"])
        self.assertEqual(actual["nargs"], 2)

    def test_other_real_callback_consumers_keep_outer_arity(self):
        cases = {
            "_Z26LoadOrUnloadObjectOverlaysPFviEi": ["void (*)(int)", "int"],
            "_ZN3IRQ13SetIRQHandlerEjPFvvE": ["unsigned int", "void (*)(void)"],
        }
        for symbol, args in cases.items():
            with self.subTest(symbol=symbol):
                value = D.demangle(symbol)
                self.assertEqual(value["args"], args)
                self.assertEqual(value["nargs"], 2)

    def test_real_unsupported_member_pointers_remain_unknown(self):
        value = D.demangle("_ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE")
        self.assertIn("T", value["args"])
        self.assertEqual(value["qualified"], "fBase_c::Process")

    def test_void_return_is_not_the_outer_empty_parameter_list(self):
        self.assertEqual(self.args("PFvvEi"), ["void (*)(void)", "int"])

    def test_function_return_and_arguments_are_distinct(self):
        self.assertEqual(self.args("PFifEd"), ["int (*)(float)", "double"])

    def test_function_and_pointer_substitutions_keep_structure(self):
        self.assertEqual(self.args("PFviES_PS_S0_"), [
            "void (*)(int)", "void (int)", "void (*)(int)", "void (*)(int)"])

    def test_nested_function_pointer_parameter_does_not_end_outer_function(self):
        self.assertEqual(self.args("PFvPFidEdEj"),
                         ["void (*)(int (*)(double), double)", "unsigned int"])

    def test_pointer_to_pointer_is_not_a_single_callback(self):
        self.assertEqual(self.args("PPFvvE"), ["void (**)(void)"])

    def test_function_references_keep_their_declarators(self):
        self.assertEqual(self.args("RFviEOFviE"), ["void (&)(int)", "void (&&)(int)"])

    def test_pointer_qualifiers_survive_an_outer_pointer(self):
        self.assertEqual(self.args("PKPFviE"), ["void (* const *)(int)"])

    def test_qualified_function_substitution_is_indivisible(self):
        self.assertEqual(self.args("PKFviES0_"),
                         ["void (*)(int) const", "void (*)(int) const"])

    def test_callback_returning_callback_uses_correct_parentheses(self):
        self.assertEqual(self.args("PFPFifEdE"), ["int (*(*)(double))(float)"])

    def test_function_c_linkage_is_not_silently_erased(self):
        self.assertEqual(self.args("PFYviE"), ['extern "C" void (*)(int)'])

    def test_unknown_inner_type_cannot_become_a_known_callback(self):
        for encoded in ("PFvQE", "PFvS99_E", "PFvM1AFvvEE"):
            with self.subTest(encoded=encoded):
                self.assertTrue(any("T" in a for a in self.args(encoded)))

    def test_missing_return_argument_or_terminator_stays_unknown(self):
        for encoded in ("PF", "PFE", "PFvE", "PFvi", "PFvv", "PFvviE", "PFv99MissingE"):
            with self.subTest(encoded=encoded):
                self.assertTrue(any("T" in a for a in self.args(encoded)))

    def test_malformed_nested_names_cannot_become_known_callbacks(self):
        for encoded in ("PFvN1AQEE", "PFvN1ANEE", "PFvN1A0EE", "PFvN1AK1BEE",
                        "PFvN99MissingE", "PFvN1A1B"):
            with self.subTest(encoded=encoded):
                self.assertTrue(any("T" in a for a in self.args(encoded)))
        self.assertEqual(self.args("PFvN1A1BEE"), ["void (*)(A::B)"])
        self.assertEqual(self.args("PFv1AIiEE"), ["void (*)(A<int>)"])

    def test_qualifying_a_substitution_does_not_replace_its_original(self):
        subs = []
        first, _end = D._read_type("FviE", 0, subs)
        qualified, _end = D._read_type("KS_", 0, subs)
        reused, _end = D._read_type("S_", 0, subs)
        self.assertEqual(str(first), "void (int)")
        self.assertEqual(str(qualified), "void (int) const")
        self.assertEqual(str(reused), "void (int)")
        self.assertEqual(list(map(str, subs)), ["void (int)", "void (int) const"])

    def test_nested_outer_c_linkage_remains_explicitly_unsupported(self):
        for plain, qualified in (("PFPFifEdE", "PFYPFifEdE"),
                                 ("PFPFYifEdE", "PFYPFYifEdE")):
            with self.subTest(qualified=qualified):
                self.assertTrue(any("T" in a for a in self.args(qualified)))
                self.assertNotEqual(self.args(plain), self.args(qualified))

    def test_nested_name_prefix_and_complete_type_have_distinct_substitutions(self):
        self.assertEqual(self.args("PFvN1A1BEPS_S0_E"),
                         ["void (*)(A::B, A *, A::B)"])

    def test_substituted_nested_prefix_is_not_added_twice(self):
        self.assertEqual(D.demangle("_ZN1A1fENS_1BES0_")["args"],
                         ["A::B", "A::B"])

    def test_nested_template_prefix_and_instance_are_distinct(self):
        self.assertEqual(self.args("PFvN1AIiE1BEPS0_S1_E"),
                         ["void (*)(A<int>::B, A<int> *, A<int>::B)"])

    def test_callback_substitution_uses_complete_enclosing_class_prefix(self):
        self.assertEqual(D.demangle("_ZN1A1B1fEPFvPS0_E")["args"],
                         ["void (*)(A::B *)"])

    def test_std_abbreviation_does_not_consume_a_numbered_substitution(self):
        self.assertEqual(self.args("PFvNSt1A1BEPS_E"),
                         ["void (*)(std::A::B, std::A *)"])

    def test_enclosing_template_prefix_and_instance_are_both_retained(self):
        self.assertEqual(D.demangle("_ZN1AIiE1fEPFvPS0_E")["args"],
                         ["void (*)(A<int> *)"])

    def test_unsupported_function_template_cannot_hide_callback_behind_void_return(self):
        self.assertEqual(D.demangle("_Z1fIiEvPFvT_E")["args"], ["T"])
        self.assertEqual(D.demangle("_ZN1A1fIiEEvPFvT_E")["args"], ["T"])

    def test_real_particle_nested_entries_keep_their_complete_type(self):
        for symbol in ("_ZN8Particle10SysTracker8Contents4LinkERNS1_5EntryE",
                       "_ZN8Particle10SysTracker8Contents6UnlinkERNS1_5EntryE"):
            with self.subTest(symbol=symbol):
                self.assertEqual(D.demangle(symbol)["args"],
                                 ["Particle::SysTracker::Contents::Entry &"])

    def test_public_results_remain_json_strings(self):
        value = D.demangle("_Z1fPFviES0_")
        self.assertEqual(json.loads(json.dumps(value)), value)
        self.assertTrue(all(type(a) is str for a in value["args"]))


class ExistingInterfaceTests(unittest.TestCase):
    def test_plain_symbols_remain_outside_the_demangler(self):
        for symbol in ("func_02000000", "main", "data_ov002_02100000"):
            self.assertIsNone(D.demangle(symbol))

    def test_scalar_reference_and_template_substitutions(self):
        actual = D.demangle("_ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_")
        self.assertEqual(actual["qualified"], "Actor::SetRanges")
        self.assertEqual(actual["args"], ["Fix12<int>"] * 4)
        self.assertEqual(D.demangle("_Z1fRK7Vector3i")["args"], ["Vector3 const &", "int"])

    def test_constructor_and_destructor_variants(self):
        for kind in ("C1", "C2", "C3", "D0", "D1", "D2"):
            with self.subTest(kind=kind):
                value = D.demangle("_ZN5Actor" + kind + "Ev")
                self.assertEqual(value["variant"], kind)
                self.assertEqual(value["args"], [])
                self.assertEqual(value["ctor"], kind.startswith("C"))
                self.assertEqual(value["dtor"], kind.startswith("D"))

    def test_thunk_adjustment_and_signature_are_preserved(self):
        value = D.demangle("_ZThn8_N5ActorD1Ev")
        self.assertTrue(value["thunk"])
        self.assertEqual(value["thunk_offset"], -8)
        self.assertEqual(D.signature("_ZThn8_N5ActorD1Ev"),
                         "[thunk(-8)] Actor::~Actor() [complete]")

    def test_suite_is_registered_in_the_explicit_ci_command(self):
        workflow = pathlib.Path(__file__).resolve().parents[1] / ".github/workflows/tool-tests.yml"
        lines = workflow.read_text().splitlines()
        registrations = [line.strip().rstrip("\\").strip() for line in lines
                         if line.strip().startswith("tools.test_demangle")]
        self.assertEqual(registrations, ["tools.test_demangle"])


if __name__ == "__main__":
    unittest.main()
