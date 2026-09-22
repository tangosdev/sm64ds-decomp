"""Reference parsing must reject stale names without inventing references."""
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import port_refcheck as P


class PortRefcheck(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name)
        self.port = self.repo / 'port'
        (self.port / 'hal').mkdir(parents=True)
        (self.repo / 'src').mkdir()
        self.stack = mock.patch.multiple(P, REPO=self.repo, PORT=self.port,
                                         SRC_DIRS=[self.repo / 'src'])
        self.stack.start()
        self.addCleanup(self.stack.stop)

    def write(self, path, text):
        path.write_text(text, encoding='utf-8')
        return path

    def cmake(self, text, existing=()):
        self.write(self.port / 'CMakeLists.txt', text)
        owners = {name: self.write(self.repo / 'src' / (name + '.cpp'), '')
                  for name in existing}
        with mock.patch.object(P.SP, 'path_for', side_effect=owners.get):
            return P.check_cmake_symbols()

    def hal(self, files):
        for name, text in files.items():
            self.write(self.port / 'hal' / name, text)
        with mock.patch.object(P, '_present', return_value=set()):
            return P.check_hal_links()

    def test_cmake_historical_comments_and_empty_initializer(self):
        count, failures = self.cmake('''
set(GATE10_SYMS first
    # Three Player state functions (not source symbols)
    second # a comment with an early closing parenthesis )
    third)
set(ACTOR_OV_SYMS "")
''', ('first', 'second', 'third'))
        self.assertEqual((count, failures), (3, []))

    def test_cmake_quotes_brackets_escape_and_real_missing(self):
        count, failures = self.cmake('''
#[=[ set(IGNORED_SYMS invented) ]=]
SET(GATE_SYMS "first;second" [=[third]=]
    "missing" # remains an error, even after a comment
    )
set(OTHER "set(FAKE_SYMS invented)")
''', ('first', 'second', 'third'))
        self.assertEqual(count, 4)
        self.assertEqual(len(failures), 1)
        self.assertEqual(failures[0].line, 4)
        self.assertIn("'missing'", failures[0].message)
        # Literal parentheses inside arguments do not end the command.
        values = list(P._cmake_symbol_lists(r'set(X_SYMS "(" escaped\) last)'))
        self.assertEqual([t.value for _, t in values], ['(', 'escaped)', 'last'])

    def test_cmake_unterminated_input_fails_closed(self):
        for text in ('set(X_SYMS "broken)', 'set(X_SYMS [=[broken)', 'set(X_SYMS first'):
            with self.subTest(text=text):
                _, failures = self.cmake(text)
                self.assertEqual(len(failures), 1)
                self.assertIn('cannot parse', failures[0].message)

    def test_cmake_variable_not_silently_accepted(self):
        count, failures = self.cmake('set(X_SYMS ${UNRESOLVED})')
        self.assertEqual(count, 1)
        self.assertEqual(len(failures), 1)

    def test_host_definitions_resolve_cross_file_reference(self):
        self.write(self.port / "hal" / "sections.h",
                   '#define DSSTATE_BEGIN __pragma(data_seg("sample"))\n'
                   '#define DSSTATE_END __pragma(data_seg())\n')
        count, failures = self.hal({
            'owner.cpp': '''extern "C" {
DSSTATE_BEGIN
unsigned char data_02086e78[0xc];
int data_02086e80 = 7, data_02086e84[3];
void *data_02086e88[1] = {data_02086e78};
DSSTATE_END
}
extern "C" int data_02086e90 = 2;
extern "C" void func_02000000() {}
''',
            'user.cpp': '''extern "C" unsigned char data_02086e78[];
#pragma comment(linker, "/alternatename:_data_02086e78=_data_02086e90")
extern "C" void func_02000000();
''',
        })
        self.assertGreater(count, 6)
        self.assertEqual(failures, [])

    def test_externs_and_initializer_references_are_not_definitions(self):
        _, failures = self.hal({'externs.cpp': '''extern "C" {
extern int data_02000000;
extern int data_02000004[], data_02000008[];
int *data_0200000c = &data_02000010;
void func_02000014();
}
extern "C" int data_02000018;
'''})
        names = {message.message.split("'")[1] for message in failures}
        self.assertEqual(names, {'data_02000000', 'data_02000004',
                                 'data_02000008', 'data_02000010',
                                 'func_02000014', 'data_02000018'})

    def test_non_external_owners_and_wrong_linkage_remain_missing(self):
        owners = '''extern "C" {
static int data_02000000;
const int data_02000004 = 0;
void helper() { int data_02000008 = 0; }
struct Fake { int data_0200000c; };
typedef int data_02000010;
#if 0
int data_02000014;
#endif
#ifdef UNKNOWN_BUILD_OPTION
int data_02000018;
#endif
}
namespace { extern "C" { int data_0200001c; } }
int data_02000020;
'''
        self.assertEqual(P._host_c_definitions(owners), {})
        names = [f'data_{n:08x}' for n in range(0x02000000, 0x02000024, 4)]
        _, failures = self.hal({'owner.cpp': owners, 'user.cpp': '\n'.join(
            f'extern "C" int {name};' for name in names)})
        self.assertEqual({f.message.split("'")[1] for f in failures
                          if f.file.endswith('user.cpp')}, set(names))

    def test_c_owner_does_not_satisfy_cpp_decorated_alias(self):
        _, failures = self.hal({'owner.cpp': '''extern "C" { int data_02000000; }
#pragma comment(linker, "/alternatename:?data_02000000@@3HA=_data_02000000")
'''})
        self.assertEqual(len(failures), 1)
        self.assertIn('?data_02000000@@3HA', failures[0].message)

    def test_comments_and_strings_are_neither_owners_nor_references(self):
        text = r'''// extern "C" int data_02000000;
/* extern "C" { int data_02000004; } */
const char *url = "https://example.invalid/";
const char *raw = R"tag(extern "C" { int data_02000008; })tag";
extern "C" {
void helper() {
    const char *text = "data_0200000c } // comment";
    char ch = '}';
}
extern int data_02000010;
}
'''
        self.assertEqual(P._host_c_definitions(text), {})
        _, failures = self.hal({'strings.cpp': text})
        self.assertEqual(len(failures), 1)
        self.assertIn('data_02000010', failures[0].message)

    def test_unresolved_alias_target_is_not_supplied_by_alias_source(self):
        _, failures = self.hal({'owner.cpp': '\n'.join([
            'extern "C" { int data_02000000; }',
            '#pragma comment(linker, "/alternatename:_data_02000000=_data_02000004")',
        ])})
        self.assertEqual(len(failures), 1)
        self.assertIn('data_02000004', failures[0].message)

    def test_only_proven_pragma_macros_may_prefix_linkage_block(self):
        header = '#define SECTION __pragma(data_seg("sample"))\n'
        source = 'SECTION\nextern "C" { int data_02000000; }\n'
        macros = P._pragma_only_macros([header, source])
        self.assertEqual(macros, {'SECTION'})
        self.assertEqual(set(P._host_c_definitions(source, macros)), {'data_02000000'})
        self.assertFalse(P._pragma_only_macros([header, '#define SECTION static\n']))
        self.assertFalse(P._pragma_only_macros([header, '#undef SECTION\n']))
        self.assertFalse(P._pragma_only_macros(['#define HIDDEN int data_02000000;\n']))
        self.write(self.port / 'hal' / 'sections.h', header)
        self.assertEqual(self.hal({'owner.cpp': source})[1], [])

    def test_cmake_semicolon_escaping_matches_foreach_lists(self):
        # Measured with CMake -P: quoted/bracket escape survives set(), while
        # an unquoted escaped semicolon is unescaped before list expansion.
        text = r'''set(A_SYMS "one\;two" "three;four")
set(B_SYMS one\;two)
set(C_SYMS [=[
one\;two]=])'''
        values = [(name, token.value) for name, token in P._cmake_symbol_lists(text)]
        self.assertEqual(values, [('A_SYMS', 'one;two'), ('A_SYMS', 'three'),
                                  ('A_SYMS', 'four'), ('B_SYMS', 'one'),
                                  ('B_SYMS', 'two'), ('C_SYMS', 'one;two')])

    def test_raw_string_cannot_forge_linker_directive(self):
        text = '\n'.join(['const char *text = R"tag(',
            '#pragma comment(linker, "/alternatename:_data_02000000=_data_02000004")',
            ')tag";'])
        self.assertEqual(self.hal({'strings.cpp': text}), (0, []))

    def test_unknown_storage_type_and_explicit_cpp_linkage_are_not_c_owners(self):
        text = '\n'.join([
            'typedef const int Hidden;',
            'extern "C" { Hidden data_02000000 = 3; }',
            'extern "C" { extern "C++" int data_02000004 = 4; }',
            'extern "C" { int Namespace::data_02000008 = 5; }',
        ])
        self.assertEqual(P._host_c_definitions(text), {})

    def test_imported_cpp_decoration_does_not_accept_c_owner(self):
        _, failures = self.hal({'owner.cpp': '\n'.join([
            'extern "C" { int data_02000000; }',
            '#pragma comment(linker, "/alternatename:__imp_?data_02000000@@3HA=_data_02000000")',
        ])})
        self.assertEqual(len(failures), 1)
        self.assertIn('__imp_?data_02000000@@3HA', failures[0].message)

    def test_continued_line_comment_cannot_define_or_reference_owner(self):
        text = '// continued ' + chr(92) + '\nextern "C" { int data_02000000; }\n'
        self.assertEqual(P._host_c_definitions(text), {})
        self.assertEqual(self.hal({'comment.cpp': text}), (0, []))

    def test_function_storage_macros_and_non_cdecl_conventions_are_not_owners(self):
        for text in (
                '#define LOCAL static\nextern "C" LOCAL void func_02000000() {}',
                'extern "C" void __stdcall func_02000000() {}',
                '#define SPECIAL __stdcall\nextern "C" SPECIAL void func_02000000() {}',
                '#define void static void\nextern "C" void func_02000000() {}'):
            with self.subTest(text=text):
                self.assertEqual(P._host_c_definitions(text), {})
        self.assertEqual(set(P._host_c_definitions(
            'extern "C" void __cdecl func_02000000() {}')), {'func_02000000'})

    def test_identifier_macro_remapping_cannot_supply_unexpanded_name(self):
        text = '#define data_02000000 host_data\nextern "C" { int data_02000000; }'
        self.assertEqual(P._host_c_definitions(text), {})
        self.write(self.port / 'hal' / 'remap.h',
                   '#define func_02000000 host_function\n')
        _, failures = self.hal({'owner.cpp': 'extern "C" void func_02000000() {}'})
        self.assertEqual(len(failures), 1)
        self.assertIn('func_02000000', failures[0].message)

    def test_pragma_failure_points_to_directive_not_previous_comment(self):
        _, failures = self.hal({'owner.cpp': '\n'.join([
            '// explanatory comment', '',
            '#pragma comment(linker, "/alternatename:_data_02000000=_data_02000004")',
        ])})
        self.assertEqual([f.line for f in failures], [3, 3])

    def test_decomp_owner_still_resolves(self):
        self.write(self.port / 'hal' / 'user.cpp', 'extern "C" int data_02000000;')
        with mock.patch.object(P, '_present', return_value={'data_02000000'}):
            self.assertEqual(P.check_hal_links(), (1, []))

    def test_readonly_list_operations_do_not_erase_the_input_list(self):
        for operation in ('LENGTH A count', 'GET A 0 first', 'FIND A first index', 'JOIN A ":" joined', 'SUBLIST A 0 1 sub'):
            with self.subTest(operation=operation):
                count, failures = self.cmake('set(A first second)\nlist(' + operation + ')\nset(X_SYMS ${A})', ('first', 'second'))
                self.assertEqual((count, failures), (2, []))

    def test_literal_list_references_resolve_but_dynamic_values_remain_errors(self):
        count, failures = self.cmake('set(A_SYMS first)\nset(B_SYMS second)\nset(C_SYMS ${A_SYMS} ${B_SYMS})', ('first', 'second'))
        self.assertEqual((count, failures), (4, []))
        for assignment in ('set(A [[${B}]])', r'set(A \${B})', 'if(UNKNOWN)\nset(A first)\nendif()'):
            with self.subTest(assignment=assignment):
                _, failures = self.cmake('set(B first)\n' + assignment + '\nset(X_SYMS ${A})', ('first',))
                self.assertEqual(len(failures), 1)
        _, failures = self.cmake('set(A first)\nunset(A)\nset(X_SYMS ${A})', ('first',))
        self.assertEqual(len(failures), 1)

    def selected(self, text):
        return P._cmake_build_inputs(text)

    def test_selected_inputs_follow_known_foreach_and_list_updates(self):
        sources, requests = self.selected('''
set(MODULES ov001)
list(APPEND MODULES ov002)
set(OUTPUTS "")
foreach(mod IN LISTS MODULES)
  set(out "${CMAKE_BINARY_DIR}/${mod}.c")
  add_custom_command(OUTPUT "${out}" COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${mod} "${out}" --from-list "${CMAKE_CURRENT_SOURCE_DIR}/${mod}_syms.txt" --pack DEPENDS source)
  list(APPEND OUTPUTS "${out}")
endforeach()
add_executable(walk_window ${OUTPUTS} unmatched/one.cpp)
target_sources(walk_window PRIVATE unmatched/two.cpp)
''')
        self.assertEqual(len(sources), 4)
        self.assertEqual(len(requests), 2)
        self.assertEqual([r[2] for r in requests], ['ov001', 'ov002'])

    def test_unused_unknown_guarded_and_unlinked_inputs_do_not_own(self):
        for body in (
            'set(SOURCES unmatched/one.cpp)\n',
            'if(UNKNOWN)\nadd_executable(walk_window unmatched/one.cpp)\nendif()',
            'foreach(mod IN LISTS UNKNOWN)\nadd_executable(walk_window unmatched/one.cpp)\nendforeach()',
            'function(build_it)\nadd_executable(walk_window unmatched/one.cpp)\nendfunction()',
            'target_sources(missing_target PRIVATE unmatched/one.cpp)',
            'add_executable(walk_window ${UNKNOWN})',
        ):
            with self.subTest(body=body):
                sources, requests = self.selected(body)
                self.assertEqual((sources, requests), (set(), []))
        _, requests = self.selected('add_custom_command(OUTPUT unused.c COMMAND python tools/romdata.py unused.c)')
        self.assertEqual(requests, [])
        sources, _ = self.selected('set(S unmatched/one.cpp)\nif(UNKNOWN)\nset(S unmatched/two.cpp)\nendif()\nadd_executable(walk_window ${S})')
        self.assertEqual(sources, set())

    def test_file_and_process_outputs_cannot_reuse_prior_selected_sources(self):
        for operation in (
                'file(STRINGS empty.txt S)', 'file(READ empty.txt S)',
                'file(GLOB S "*.cpp")', 'string(REPLACE "x" "" S "")',
                'string(REGEX REPLACE ".*" "" S "")',
                'execute_process(COMMAND tool OUTPUT_VARIABLE S)',
                'set(NAME S)\nset(${NAME} "")',
                'set(NAME S)\nunset(${NAME})',
                'set(NAME ${UNKNOWN})\nset(${NAME} "")',
                'file(STRINGS empty.txt REMOVE)\nlist(REMOVE_ITEM S ${REMOVE})'):
            for wrapper in ('{}', 'if(TRUE)\n{}\nendif()'):
                with self.subTest(operation=operation, wrapper=wrapper):
                    text = 'set(S unmatched/ghost.cpp)\n' + wrapper.format(operation)
                    selected, _ = self.selected(text + '\nadd_executable(host ${S})')
                    self.assertEqual(selected, set())

    def test_indirect_list_outputs_invalidate_the_resolved_destination(self):
        for operation in ('GET A 0 ${OUT}', 'LENGTH A ${OUT}',
                          'JOIN A ":" ${OUT}', 'FIND A item ${OUT}',
                          'SUBLIST A 0 1 ${OUT}'):
            with self.subTest(operation=operation):
                text = ('set(S unmatched/ghost.cpp)\nset(A unmatched/other.cpp)\n'
                        'set(OUT S)\nlist(' + operation + ')\nadd_executable(host ${S})')
                self.assertEqual(self.selected(text)[0], set())

    def test_indirect_symbol_list_mutations_check_added_references(self):
        for operation in ('APPEND ${NAME} missing', 'PREPEND ${NAME} missing',
                          'INSERT ${NAME} 0 missing'):
            with self.subTest(operation=operation):
                count, failures = self.cmake('set(X_SYMS real)\nset(NAME X_SYMS)\nlist('
                                             + operation + ')', ('real',))
                self.assertEqual(count, 2)
                self.assertEqual(len(failures), 1)
                self.assertIn("'missing'", failures[0].message)
        _, failures = self.cmake('set(X_SYMS real)\nlist(APPEND ${UNKNOWN} missing)', ('real',))
        self.assertTrue(failures)

    def test_terminating_control_flow_cannot_license_unreached_sources(self):
        for body in ('return()', 'if(TRUE)\nreturn()\nendif()',
                     'foreach(x item)\nbreak()\n{}\nendforeach()',
                     'foreach(x item)\ncontinue()\n{}\nendforeach()',
                     'foreach(x item)\nif(UNKNOWN)\ncontinue()\nendif()\n{}\nendforeach()',
                     'macro(stop)\nreturn()\nendmacro()\nstop()'):
            with self.subTest(body=body):
                target = 'add_executable(host unmatched/ghost.cpp)'
                text = body.format(target) if '{}' in body else body + '\n' + target
                if 'foreach' in body:
                    self.assertEqual(self.selected(text)[0], set())
                else:
                    with self.assertRaisesRegex(ValueError, 'control flow'):
                        self.selected(text)

    def test_function_local_control_cannot_supply_outputs_or_skip_caller(self):
        body = ('function(build out)\nforeach(x item)\ncontinue()\n'
                'set(${out} unmatched/ghost.cpp PARENT_SCOPE)\nendforeach()\n'
                'return()\nendfunction()\nset(S unmatched/ghost.cpp)\n'
                'build(S)\nadd_executable(host ${S} unmatched/real.cpp)')
        self.assertEqual(self.selected(body)[0], {self.port / 'unmatched/real.cpp'})

    def test_discarded_unknown_loop_cannot_hide_an_outer_return(self):
        body = 'foreach(x IN LISTS UNKNOWN)\n{}\nendforeach()\nadd_executable(host unmatched/real.cpp)'
        self.assertEqual(self.selected(body.format('continue()'))[0],
                         {self.port / 'unmatched/real.cpp'})
        with self.assertRaisesRegex(ValueError, 'control flow'):
            self.selected(body.format('if(UNKNOWN)\nreturn()\nendif()'))

    def test_nested_helper_output_invalidates_the_actual_callers_variable(self):
        text = ( 'macro(clearit out)\nset(${out} "")\nendmacro()\n'
                 'macro(wrapper out)\nclearit(${out})\nendmacro()\n'
                 'set(S unmatched/ghost.cpp)\nwrapper(S)\nadd_executable(host ${S})')
        self.assertEqual(self.selected(text)[0], set())
        text = text.replace('clearit(${out})', 'clearit("")\nclearit(${out})')
        self.assertEqual(self.selected(text)[0], set())

    def test_recursive_or_unknown_helpers_cannot_preserve_selected_input_claims(self):
        for body in ('unknown(S)', 'unknown()', 'macro(loop out)\nloop(${out})\nendmacro()\nloop(S)'):
            with self.subTest(body=body):
                text = 'set(S unmatched/ghost.cpp)\n' + body + '\nadd_executable(host ${S})'
                with self.assertRaises(ValueError):
                    self.selected(text)

    def test_generated_output_cannot_remain_selected_after_its_list_is_replaced(self):
        self.generated_fixture()
        text = ('set(S "${CMAKE_BINARY_DIR}/romdata.c")\n'
                'add_custom_command(OUTPUT "${S}" COMMAND python '
                '"${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py" "${S}")\n'
                'file(STRINGS empty.txt S)\nadd_executable(host ${S})')
        selected, requests = self.selected(text)
        self.assertEqual((selected, requests), (set(), []))

    def test_actual_unselected_owner_does_not_clear_a_hal_reference(self):
        folder = self.port / 'unmatched'
        folder.mkdir()
        self.write(folder / 'ghost.cpp', 'extern "C" void func_02000000() {}')
        for mutation in ('file(STRINGS empty.txt S)',
                         'if(TRUE)\nfile(STRINGS empty.txt S)\nendif()',
                         'macro(clear out)\nset(${out} "")\nendmacro()\n'
                         'macro(wrap out)\nclear(${out})\nendmacro()\nwrap(S)'):
            with self.subTest(mutation=mutation):
                self.write(self.port / 'CMakeLists.txt', 'set(S unmatched/ghost.cpp)\n'
                           + mutation + '\nadd_executable(host ${S})')
                with mock.patch.object(P.SP, 'path_for', return_value=None):
                    count, failures = self.hal({'user.cpp': 'extern "C" void func_02000000();'})
                self.assertEqual(count, 1)
                self.assertEqual(len(failures), 1)
                self.assertIn('func_02000000', failures[0].message)

    def test_all_added_symbol_list_values_are_checked(self):
        for operation in ('APPEND X_SYMS missing', 'PREPEND X_SYMS missing',
                          'INSERT X_SYMS 0 missing'):
            for wrapper in ('{}', 'if(TRUE)\n{}\nendif()'):
                with self.subTest(operation=operation, wrapper=wrapper):
                    count, failures = self.cmake('set(X_SYMS real)\n' +
                                                wrapper.format('list(' + operation + ')'), ('real',))
                    self.assertEqual(count, 2)
                    self.assertEqual(len(failures), 1)
                    self.assertIn("'missing'", failures[0].message)

    def test_unknown_symbol_list_replacements_require_review(self):
        for operation in ('file(STRINGS symbols.txt X_SYMS)',
                          'string(REPLACE "real" "missing" X_SYMS "real")',
                          'list(TRANSFORM X_SYMS APPEND "missing")'):
            with self.subTest(operation=operation):
                _, failures = self.cmake('set(X_SYMS real)\n' + operation, ('real',))
                self.assertEqual(len(failures), 1)

    def test_empty_replacement_does_not_shift_output_argument_identity(self):
        count, failures = self.cmake('set(A first)\nstring(REGEX REPLACE ".*" "" B "")\n'
                                    'set(X_SYMS ${A})', ('first',))
        self.assertEqual((count, failures), (1, []))

    def test_named_source_list_requires_selected_generated_output_proof(self):
        self.generated_fixture()
        self.write(self.port / 'CMakeLists.txt', """
set(MODULES ov002)
set(ACTOR_OV_SYMS "")
foreach(mod IN LISTS MODULES)
  set(out "${CMAKE_BINARY_DIR}/${mod}.c")
  add_custom_command(OUTPUT "${out}" COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/ovdata.py" ${mod} "${out}" --from-list "${CMAKE_CURRENT_SOURCE_DIR}/${mod}_syms.txt" --pack)
  list(APPEND ACTOR_OV_SYMS "${out}")
endforeach()
add_executable(host ${ACTOR_OV_SYMS})
""")
        with mock.patch.object(P.SP, 'path_for', return_value=None):
            self.assertEqual(P.check_cmake_symbols(), (1, []))
            text = (self.port / 'CMakeLists.txt').read_text().replace('add_executable(host ${ACTOR_OV_SYMS})', '')
            self.write(self.port / 'CMakeLists.txt', text)
            self.assertEqual(len(P.check_cmake_symbols()[1]), 1)

    def test_unmodeled_final_source_replacement_blocks_ownership(self):
        for mutation in ('set_property(TARGET host PROPERTY SOURCES "")',
                         'set_target_properties(host PROPERTIES SOURCES "")',
                         'if(UNKNOWN)\nset_property(TARGET host PROPERTY SOURCES "")\nendif()',
                         'macro(clear target)\nset_property(TARGET ${target} PROPERTY SOURCES "")\n'
                         'endmacro()\nclear(host)'):
            with self.subTest(mutation=mutation):
                with self.assertRaisesRegex(ValueError, 'SOURCES rewrite'):
                    self.selected('add_executable(host unmatched/ghost.cpp)\n' + mutation)

    def test_changed_or_incomplete_tier_transfer_cannot_license_source_selection(self):
        for target in ('host', 'other'):
            for reinsertion in ('', 'target_sources(other PRIVATE $<TARGET_OBJECTS:shared>)'):
                with self.subTest(target=target, reinsertion=reinsertion):
                    text = ('add_executable(host unmatched/ghost.cpp)\nset(PORT_SHARE_TARGETS host)\n'
                            'set_property(TARGET ' + target + ' PROPERTY SOURCES "")\n' + reinsertion)
                    with self.assertRaisesRegex(ValueError, 'tier-sharing'):
                        self.selected(text)

    def test_pop_output_and_unknown_list_destinations_do_not_retain_old_owners(self):
        for mutation in ('set(A real.cpp)\nlist(POP_FRONT A S)',
                         'list(REMOVE_AT ${UNKNOWN} 0)'):
            with self.subTest(mutation=mutation):
                selected, _ = self.selected('set(S unmatched/ghost.cpp)\n' + mutation + '\nadd_executable(host ${S})')
                self.assertEqual(selected, set())

    def test_list_output_symbol_contract_is_not_silently_skipped(self):
        for operation in ('GET A 0 X_SYMS', 'POP_FRONT A X_SYMS'):
            with self.subTest(operation=operation):
                _, failures = self.cmake('set(A missing)\nlist(' + operation + ')')
                self.assertTrue(failures)

    def test_utf8_bom_does_not_hide_the_first_cmake_command(self):
        count, failures = self.cmake('\ufeffcmake_minimum_required(VERSION 3.20)\nset(X_SYMS real)', ('real',))
        self.assertEqual((count, failures), (1, []))

    def test_later_external_cmake_code_cannot_retain_target_owners(self):
        for command in ('include(other.cmake)', 'find_package(Other)', 'cmake_language(EVAL CODE "set_property(TARGET host PROPERTY SOURCES)")'):
            with self.subTest(command=command):
                with self.assertRaisesRegex(ValueError, 'rewrite existing target sources'):
                    self.selected('add_executable(host unmatched/ghost.cpp)\n' + command)

    def generated_fixture(self):
        (self.port / 'tools').mkdir(exist_ok=True)
        (self.repo / 'config' / 'arm9' / 'overlays' / 'ov002').mkdir(parents=True)
        self.write(self.port / 'tools' / 'romdata.py', 'BASE=0x02004000\nBSS_START=0x02005000\nTABLES=[]\nNAMED=["data_02004004"]\nCONTIG=[]\n')
        self.write(self.port / 'tools' / 'ovdata.py', '# fixture for the named-list input contract\n')
        self.write(self.repo / 'config' / 'arm9' / 'symbols.txt', 'data_02004004 kind:data addr:0x02004004\ndata_02004008 kind:data addr:0x02004008\n')
        self.write(self.repo / 'config' / 'arm9' / 'overlays' / 'ov002' / 'symbols.txt', 'data_ov002_02100000 kind:data addr:0x02100000\ndata_ov002_02100004 kind:data addr:0x02100004\n')
        self.write(self.port / 'ov002_syms.txt', 'data_ov002_02100000\n')

    def test_generated_owners_require_selected_list_and_exact_module(self):
        self.generated_fixture()
        command = ['python', str(self.port / 'tools' / 'ovdata.py'), 'ov002', str(self.repo / 'build' / 'ov002.c'), '--from-list', str(self.port / 'ov002_syms.txt'), '--pack']
        self.assertEqual(P._generated_data_owners([command]), {'data_ov002_02100000'})
        wrong = command[:]
        wrong[2] = 'ov003'
        self.assertEqual(P._generated_data_owners([wrong]), set())
        whole = command[:4] + ['--whole']
        self.assertEqual(P._generated_data_owners([whole]), set())
        self.write(self.port / 'ov002_syms.txt', '# data_ov002_02100004\ndata_ov002_02109999\n')
        self.assertEqual(P._generated_data_owners([command]), set())
        # Config-only and merely present list files cannot establish selection.
        self.assertEqual(P._generated_data_owners([]), set())

    def test_arm9_generation_uses_declared_inputs_not_every_config_row(self):
        self.generated_fixture()
        command = ['python', str(self.port / 'tools' / 'romdata.py'), str(self.repo / 'build' / 'romdata.c')]
        self.assertEqual(P._generated_data_owners([command]), {'data_02004004'})
        other = command[:]
        other[1] = str(self.port / 'tools' / 'other.py')
        self.assertEqual(P._generated_data_owners([other]), set())

    def test_unmatched_definitions_require_selected_source(self):
        folder = self.port / 'unmatched'
        folder.mkdir()
        self.write(folder / 'owner.cpp', 'extern "C" void func_02000000() {}')
        _, failures = self.hal({'user.cpp': 'extern "C" void func_02000000();'})
        self.assertEqual(len(failures), 1)
        self.write(self.port / 'CMakeLists.txt', 'add_executable(walk_window unmatched/owner.cpp)')
        _, failures = self.hal({})
        self.assertEqual(failures, [])
        for body in ('extern "C" void func_02000000();', 'extern "C" static void func_02000000() {}', '#if UNKNOWN\nextern "C" void func_02000000() {}\n#endif'):
            with self.subTest(body=body):
                self.write(folder / 'owner.cpp', body)
                _, failures = self.hal({})
                self.assertEqual(len(failures), 1)

    def test_alias_root_must_exist_and_cycles_and_conflicts_fail(self):
        alias = '#pragma comment(linker, "/alternatename:_func_02000000=_func_02000004")\n'
        with mock.patch.object(P.SP, 'path_for', return_value=None):
            self.assertEqual(P._resolved_aliases([alias], set()), set())
            self.assertEqual(P._resolved_aliases([alias], {'func_02000004'}), {'_func_02000000'})
            cycle = '#pragma comment(linker, "/alternatename:_func_02000004=_func_02000000")'
            self.assertEqual(P._resolved_aliases([alias + cycle], set()), set())
            conflict = '#pragma comment(linker, "/alternatename:_func_02000000=_func_02000008")'
            self.assertEqual(P._resolved_aliases([alias + conflict], {'func_02000004', 'func_02000008'}), set())
            self.assertEqual(P._resolved_aliases(['#if UNKNOWN\n' + alias + '#endif'], {'func_02000004'}), set())

    def test_alias_lhs_never_hides_a_missing_rhs(self):
        self.write(self.port / 'CMakeLists.txt', 'add_executable(walk_window hal/a.cpp)')
        _, failures = self.hal({'a.cpp': '#pragma comment(linker, "/alternatename:_data_02000000=_data_02000004")'})
        self.assertEqual(len(failures), 2)
        self.assertTrue(any('data_02000004' in f.message for f in failures))

    def test_storage_macro_requires_exact_body_and_active_unconditional_definition(self):
        definition = '#define STORAGE(sec, name, count, alignment) __pragma(section(sec, read, write)) extern "C" __declspec(allocate(sec)) __declspec(align(alignment)) unsigned char name[count]\n'
        use = 'STORAGE(".test", data_02000000, 4, 4) = {0};\n'
        self.assertEqual(set(P._host_c_definitions(definition + use + '#undef STORAGE')), {'data_02000000'})
        for text in (definition + '#undef STORAGE\n' + use,
                     '#if UNKNOWN\n' + definition + '#endif\n' + use,
                     definition + '#if UNKNOWN\n' + use + '#endif',
                     definition.replace('unsigned char', 'static unsigned char') + use,
                     definition + use.replace(', 4, 4', ', UNKNOWN, 4'),
                     definition + '#define STORAGE(...) ignored\n' + use):
            with self.subTest(text=text):
                self.assertEqual(P._host_c_definitions(text), {})

    def test_helper_outputs_invalidate_prior_list_values(self):
        text = 'function(clear_list)\nset(A "" PARENT_SCOPE)\nendfunction()\nset(A first)\nclear_list()\nset(X_SYMS ${A})'
        _, failures = self.cmake(text, ('first',))
        self.assertEqual(len(failures), 1)
        sources, _ = self.selected('macro(read_file output)\nfile(STRINGS missing ${output})\nendmacro()\nset(S unmatched/old.cpp)\nread_file(S)\nadd_executable(walk_window ${S})')
        self.assertEqual(sources, set())

    def test_generated_output_must_be_the_selected_declared_output(self):
        text = 'add_custom_command(OUTPUT expected.c COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py" other.c)\nadd_executable(walk_window expected.c)'
        _, requests = self.selected(text)
        self.assertEqual(requests, [])
        text = 'add_custom_command(OUTPUT expected.c COMMAND python "${CMAKE_CURRENT_SOURCE_DIR}/tools/romdata.py" expected.c COMMAND overwrite expected.c)\nadd_executable(walk_window expected.c)'
        _, requests = self.selected(text)
        self.assertEqual(requests, [])

    def test_conflicting_generation_rules_fail_closed(self):
        text = 'add_custom_command(OUTPUT same.c COMMAND first same.c)\nadd_custom_command(OUTPUT same.c COMMAND second same.c)\nadd_executable(walk_window same.c)'
        with self.assertRaisesRegex(ValueError, 'conflicting generators'):
            self.selected(text)

    def test_generator_mutations_do_not_leave_a_stale_literal_policy(self):
        self.generated_fixture()
        path = self.port / 'tools' / 'romdata.py'
        original = path.read_text()
        command = ['python', str(path), str(self.repo / 'build' / 'romdata.c')]
        for mutation in ('NAMED.append("data_02004008")', 'if unknown:\n NAMED=[]', 'NAMED[0]="data_02004008"', 'del NAMED', 'NAMED += ["data_02004008"]'):
            with self.subTest(mutation=mutation):
                self.write(path, original + '\n' + mutation + '\n')
                self.assertEqual(P._generated_data_owners([command]), set())

    def test_invalid_requested_symbol_invalidates_the_entire_generation(self):
        self.generated_fixture()
        command = ['python', str(self.port / 'tools' / 'ovdata.py'), 'ov002', str(self.repo / 'build' / 'ov002.c'), '--from-list', str(self.port / 'ov002_syms.txt')]
        for text in ('data_ov002_02100000 data_ov002_02109999', 'data_ov002_02100000 # inline comments are not supported by the emitter'):
            with self.subTest(text=text):
                self.write(self.port / 'ov002_syms.txt', text)
                self.assertEqual(P._generated_data_owners([command]), set())

    def test_malformed_table_and_contiguous_group_cannot_supply_owner(self):
        self.generated_fixture()
        path = self.port / 'tools' / 'romdata.py'
        original = path.read_text()
        command = ['python', str(path), str(self.repo / 'build' / 'romdata.c')]
        for replacement in ('TABLES=[(0x02004004,3,"int")]', 'TABLES=[(0x02004004,4,"unknown_type")]', 'CONTIG=[("group",0x02004000,0x0200400c,4)]', 'CONTIG=[("group",0x02004004,0x0200400b,4)]'):
            with self.subTest(replacement=replacement):
                key = replacement.partition('=')[0]
                self.write(path, original.replace(key+'=[]', replacement))
                self.assertEqual(P._generated_data_owners([command]), set())

    def test_generated_list_must_stay_inside_the_selected_port(self):
        self.generated_fixture()
        external = self.repo / 'outside.txt'
        self.write(external, 'data_ov002_02100000')
        command = ['python', str(self.port / 'tools' / 'ovdata.py'), 'ov002', str(self.repo / 'build' / 'ov002.c'), '--from-list', str(external)]
        self.assertEqual(P._generated_data_owners([command]), set())

    def test_alias_owner_must_come_from_selected_linker_input(self):
        self.write(self.repo / 'src' / 'func_02000004.cpp', 'void func_02000004() {}')
        path = self.repo / 'src' / 'func_02000004.cpp'
        with mock.patch.object(P.SP, 'path_for', side_effect=lambda n: path if n == 'func_02000004' else None):
            _, failures = self.hal({'a.cpp': '#pragma comment(linker, "/alternatename:_func_02000000=_func_02000004")'})
            self.assertEqual(len(failures), 2)
            self.write(self.port / 'CMakeLists.txt', 'add_executable(walk_window hal/a.cpp)')
            _, failures = self.hal({})
            # The LHS now has a selected alias, while the mocked absence of
            # the RHS from the original presence check remains independently red.
            self.assertEqual(len(failures), 1)
            self.assertIn('func_02000004', failures[0].message)

    def test_alias_decoration_is_not_erased_to_invent_an_owner(self):
        for spelling in ('?data_02000004@@3HA', '__imp__data_02000004', '_data_02000004@4'):
            self.assertIsNone(P._c_linker_identifier(spelling))
        self.assertEqual(P._c_linker_identifier('__ZN3OAM5TIMESE'), '_ZN3OAM5TIMESE')


    def test_aliases_in_macro_bodies_and_raw_strings_do_not_supply_owners(self):
        alias = '#pragma comment(linker, "/alternatename:_func_02000000=_func_02000004")'
        for text in ('#define NEVER_USED \\\n' + alias,
                     'const char *s = R"text(\n' + alias + '\n)text";',
                     '/*\n' + alias + '\n*/'):
            with self.subTest(text=text):
                with mock.patch.object(P.SP, 'path_for', return_value=None):
                    self.assertEqual(P._resolved_aliases([text], {'func_02000004'}), set())


if __name__ == '__main__':
    unittest.main()
