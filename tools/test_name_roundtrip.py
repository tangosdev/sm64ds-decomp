"""Parser and synthesiser tests for tools/name_roundtrip.py.

These are all compiler-free.  The compiler half of the tool is its own oracle
and needs the pinned mwccarm plus a licence, which CI for this file does not
have; what CAN silently rot is the Itanium parser, and a parser bug there does
not fail loudly -- it manufactures a MISMATCH against a name that is actually
fine.  So every test here pins a parse or a synthesis whose correct answer is
fixed by the ABI, not by our tree.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import name_roundtrip as R  # noqa: E402


def emit(mangled):
    tu = R.TU()
    R.synthesise(R.parse_symbol(mangled), tu)
    return tu.text()


class ParserTests(unittest.TestCase):
    def test_member_function_scope_and_params(self):
        sym = R.parse_symbol("_ZN22ExpandingHeapAllocator12AllocateNodeE"
                             "P10MemoryNodeS1_Pvjt")
        self.assertEqual(sym.scope, ["ExpandingHeapAllocator"])
        self.assertEqual(sym.func, "AllocateNode")
        self.assertEqual([p.spell() for p in sym.params],
                         ["MemoryNode *", "MemoryNode *", "void *",
                          "unsigned int", "unsigned short"])

    def test_substitution_back_reference_does_not_swallow_the_next_param(self):
        # RKS_ is `const Vector3 &`; the S_ is complete on its own and must not
        # absorb the 5Fix12IiE that follows it.
        sym = R.parse_symbol("_Z14ApproachLinearR7Vector3RKS_5Fix12IiE")
        self.assertEqual(sym.nargs if hasattr(sym, "nargs") else len(sym.params), 3)
        self.assertEqual([p.spell() for p in sym.params],
                         ["Vector3 &", "const Vector3 &", "Fix12<int>"])

    def test_st_is_the_std_namespace_and_takes_a_member_name(self):
        sym = R.parse_symbol("_ZTISt9type_info")
        self.assertEqual(sym.kind, "typeinfo")
        self.assertEqual(sym.type_name.parts, ["std", "type_info"])

    def test_const_member(self):
        sym = R.parse_symbol("_ZNK5Actor7GetPosBEv")
        self.assertTrue(sym.const)
        self.assertEqual(sym.params, [])

    def test_void_parameter_list_is_no_parameters(self):
        self.assertEqual(R.parse_symbol("_ZN5Stage4InitEv").params, [])

    def test_data_symbol_is_a_static_member_not_a_function(self):
        sym = R.parse_symbol("_ZN3OAM10LIFE_ICONSE")
        self.assertEqual(sym.kind, "data")
        self.assertEqual(sym.scope, ["OAM"])
        self.assertEqual(sym.func, "LIFE_ICONS")

    def test_thunk_offset_is_read(self):
        sym = R.parse_symbol("_ZThn80_N10ModelAnim2D0Ev")
        self.assertEqual(sym.kind, "thunk")
        self.assertEqual(sym.thunk_offset, 80)
        self.assertEqual(sym.scope, ["ModelAnim2"])

    def test_allocation_operator_is_recognised_inside_a_class(self):
        # A substring test gets this backwards: `nw` here follows the `e` of
        # ActorBase, so a letter-lookbehind rejects the real operator.
        self.assertTrue(R._is_alloc_operator("_ZN9ActorBasenwEj"))
        self.assertTrue(R._is_alloc_operator("_Znwj"))
        self.assertFalse(R._is_alloc_operator("_ZN5Actor4nwakEv"))

    def test_function_pointer_parameter(self):
        sym = R.parse_symbol("_Z26LoadOrUnloadObjectOverlaysPFviEi")
        self.assertEqual(sym.params[0].spell(), "void (*)(int)")

    def test_unsupported_constructs_are_untestable_not_guesses(self):
        for name in ("_ZTv0_n12_N1A1fEv", "_ZGV3foo", "_ZZ3fooE3bar"):
            with self.assertRaises(R.Untestable):
                R.parse_symbol(name)


class SynthesisTests(unittest.TestCase):
    def test_forward_declarations_precede_the_scope_that_uses_them(self):
        text = emit("_ZN16MeshColliderBase16UpdatePosAndAngsE"
                    "RS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")
        self.assertLess(text.index("struct Actor;"),
                        text.index("struct MeshColliderBase {"))

    def test_by_value_class_parameter_gets_a_complete_type(self):
        text = emit("_Z14ApproachLinearR7Vector3RKS_5Fix12IiE")
        self.assertIn("template <class _T0> struct Fix12", text)
        self.assertIn("struct Vector3;", text)

    def test_std_is_a_namespace_not_a_struct(self):
        self.assertIn("namespace std {", emit("_ZTISt9type_info"))
        self.assertNotIn("struct std", emit("_ZTISt9type_info"))

    def test_allocation_operator_keeps_its_declared_size_type(self):
        # The whole point: we must hand the compiler exactly what the name
        # claims, so that it can refuse it.
        self.assertIn("void *ActorBase::operator new(unsigned int)",
                      emit("_ZN9ActorBasenwEj"))


class ClassificationTests(unittest.TestCase):
    def test_c3_is_named_as_an_unproducible_variant(self):
        row = {"name": "_ZN5StageC3Ev", "verdict": R.MISMATCH,
               "emitted": ["_ZN5StageC1Ev", "_ZN5StageC2Ev"], "reason": ""}
        self.assertIn("C3", R.classify_cause(row))
        candidates, note = R.propose(row)
        self.assertEqual(candidates, ["_ZN5StageC1Ev", "_ZN5StageC2Ev"])
        self.assertTrue(note, "an ambiguous fix must carry an explanation")

    def test_missing_substitution_is_named_as_such(self):
        row = {"name": "_ZN13HeapAllocatorC1EjPvPvj", "verdict": R.MISMATCH,
               "emitted": ["_ZN13HeapAllocatorC1EjPvS0_j",
                           "_ZN13HeapAllocatorC2EjPvS0_j"], "reason": ""}
        self.assertIn("substitution", R.classify_cause(row))
        self.assertEqual(R.propose(row)[0], ["_ZN13HeapAllocatorC1EjPvS0_j"])

    def test_rejected_allocation_operator_names_the_size_t_divergence(self):
        row = {"name": "_Znwj", "verdict": R.REJECTED, "emitted": [],
               "reason": "line 2: illegal 'operator' declaration"}
        self.assertIn("size_t", R.classify_cause(row))


class ConsistencyTests(unittest.TestCase):
    def test_vtable_without_a_matching_typeinfo_is_reported(self):
        info = {
            "_ZTV5Enemy": {"module": "ov002", "addr": "0x1"},
            "_ZTI12dEnemyBase_c": {"module": "ov002", "addr": "0x2"},
            "_ZTS12dEnemyBase_c": {"module": "ov002", "addr": "0x3"},
            "_ZTV4Heap": {"module": "arm9", "addr": "0x4"},
            "_ZTI4Heap": {"module": "arm9", "addr": "0x5"},
        }
        dups, orphans = R.rtti_triple_conflicts(info)
        self.assertEqual(dups, [])
        self.assertEqual([o[2] for o in orphans], ["_ZTV5Enemy"])

    def test_two_records_of_one_kind_at_one_address(self):
        info = {
            "_ZTV4Coin": {"module": "ov002", "addr": "0x21087ec"},
            "_ZTV8daCoin_c": {"module": "ov002", "addr": "0x21087ec"},
            "_ZTI8daCoin_c": {"module": "ov002", "addr": "0x21087f0"},
            "_ZTS8daCoin_c": {"module": "ov002", "addr": "0x21087f4"},
        }
        dups, _ = R.rtti_triple_conflicts(info)
        self.assertEqual(len(dups), 1)
        self.assertEqual(dups[0][3], ["_ZTV4Coin", "_ZTV8daCoin_c"])


if __name__ == "__main__":
    unittest.main()
