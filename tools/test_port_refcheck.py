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


if __name__ == '__main__':
    unittest.main()
