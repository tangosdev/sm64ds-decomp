"""Executable regression tests: scanner limits, Git scope, and pinned formatter."""
import contextlib
import io
import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import source_style as S

ROOT = pathlib.Path(__file__).resolve().parent.parent
FIXTURES = ROOT / "tools/style_fixtures"
FORMAT = os.environ.get("CLANG_FORMAT") or shutil.which("clang-format")
TIDY = os.environ.get("CLANG_TIDY") or shutil.which("clang-tidy")


class Patterns(unittest.TestCase):
    def test_good_c_and_cpp(self):
        for name in ("good.c", "good.cpp"):
            self.assertEqual([], S.source_findings((FIXTURES / name).read_text()))

    def test_bad_fixture_exercises_every_rule(self):
        found = S.source_findings((FIXTURES / "bad.cpp").read_text())
        self.assertEqual(set(S.RULES), {f.rule for f in found})
        self.assertEqual(2, sum(f.rule == "raw-offset" for f in found))

    def test_comments_and_literals_are_not_findings(self):
        text = '// obj->vtable[3]; _ZN5Actor4TickEv; nullptr\n'
        text += 'const char* s = "unk_74; (int*)(self + 0x74)";\n'
        self.assertEqual([], S.source_findings(text))

    def test_host_bridge_is_not_a_rom_binding(self):
        text = '#ifdef _MSC_VER\nvoid _ZN5Actor4TickEv();\n#else\nvoid Tick();\n#endif\n'
        self.assertEqual([], S.source_findings(text))

    def test_real_mmio_and_callbacks_are_allowed(self):
        text = '*(volatile unsigned*)0x04000208 = 0;\n(*callback)(actor);\n'
        self.assertEqual([], S.source_findings(text))

    def test_old_debt_survives_comment_and_whitespace_edits(self):
        before = '*(int*)(self + 0x74) = 1;\n'
        after = '// explanation\n*(int*) ( self+0x74 ) = 1;\n'
        self.assertEqual([], S.new_findings(before, after))

    def test_new_offset_cannot_replace_an_old_finding(self):
        found = S.new_findings('*(int*)(self + 0x74)=1;', '*(int*)(self + 0x80)=1;')
        self.assertEqual(1, len(found))
        self.assertIn('0x80', found[0].match)

    def test_duplicate_occurrence_is_a_regression(self):
        text = '_ZN5Actor4TickEv(actor);\n'
        self.assertEqual(1, len(S.new_findings(text, text + text)))

    def test_deletion_does_not_add_debt(self):
        self.assertEqual([], S.new_findings('p->vtable[3](p);', 'Tick();'))

    def test_line_numbers_survive_masking(self):
        text = '/*\nnullptr\n*/\nvoid f() { nullptr; }\n'
        self.assertEqual(4, S.source_findings(text)[0].line)

    def test_line_endings_are_not_source_changes(self):
        self.assertEqual([], S.added_ranges('int x;\r\n', 'int x;\n'))

    def test_changed_ranges_ignore_deletion_only(self):
        self.assertEqual([], S.added_ranges('a\nb\n', 'a\n'))
        self.assertEqual([(2, 2)], S.added_ranges('a\nb\n', 'a\nc\n'))


class Exceptions(unittest.TestCase):
    def row(self, **changes):
        row = dict(path='src/a.cpp', rule='handspelled-member', match='_ZN1A1fEv',
                   count=1, reason='Measured destructor ABI bridge', evidence='issue/123#experiment')
        row.update(changes)
        return row

    def load(self, rows):
        with tempfile.TemporaryDirectory() as td:
            p = pathlib.Path(td) / 'exceptions.json'
            p.write_text(json.dumps(dict(schema=1, exceptions=rows)))
            return S.load_exceptions(p)

    def test_exception_is_exact_and_counted(self):
        budget = self.load([self.row()])
        findings = S.source_findings('_ZN1A1fEv(); _ZN1A1fEv(); _ZN1B1fEv();')
        self.assertEqual(2, len(S.apply_exceptions('src/a.cpp', findings, budget)))
        self.assertEqual(3, len(S.apply_exceptions('src/b.cpp', findings, budget)))

    def test_exception_is_a_total_cap_across_successive_baselines(self):
        budget = self.load([self.row()])
        one = '_ZN1A1fEv();\n'
        first = S.apply_exceptions('src/a.cpp', S.new_findings('', one), budget, '')
        second = S.apply_exceptions('src/a.cpp', S.new_findings(one, one + one), budget, one)
        third = S.apply_exceptions('src/a.cpp', S.new_findings(one + one, one * 3), budget, one + one)
        self.assertEqual([], first)
        self.assertEqual(1, len(second))
        self.assertEqual(1, len(third))

    def test_empty_reason_evidence_and_wildcards_fail(self):
        for changes in ({'reason': ''}, {'evidence': ''}, {'rule': '*'}, {'path': '../a.cpp'},
                        {'path': 'src/*.cpp'}, {'count': 0}, {'count': True}):
            with self.subTest(changes=changes), self.assertRaises(S.StyleError):
                self.load([self.row(**changes)])

    def test_duplicate_and_unknown_keys_fail(self):
        with self.assertRaises(S.StyleError):
            self.load([self.row(), self.row()])
        with self.assertRaises(S.StyleError):
            self.load([self.row(extra='not supported')])

    def test_missing_config_does_not_skip(self):
        with self.assertRaises(S.StyleError):
            S.load_exceptions(ROOT / 'build/definitely-missing-style-exceptions.json')


class GitScope(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.run_git('init', '-q')
        self.run_git('config', 'user.name', 'Style Test')
        self.run_git('config', 'user.email', 'style-test@example.invalid')
        self.run_git('config', 'core.autocrlf', 'false')
        (self.root / 'src').mkdir()
        (self.root / 'config').mkdir()
        (self.root / 'config/source-style-exceptions.json').write_text('{"schema":1,"exceptions":[]}')
        (self.root / 'src/old.cpp').write_text('//cpp\nvoid f(){_ZN1A1fEv();}\n')
        self.run_git('add', '.')
        self.run_git('commit', '-qm', 'base')
        self.base = self.run_git('rev-parse', 'HEAD').strip()

    def run_git(self, *args):
        return subprocess.check_output(['git', '-C', str(self.root), *args], text=True)

    def check(self, base=None):
        return S.check(self.root, base or self.base, None, None, patterns_only=True)

    def test_staged_unstaged_and_untracked_inputs(self):
        (self.root / 'src/old.cpp').write_text('//cpp\nvoid f(){_ZN1A1fEv(); _ZN1B1fEv();}\n')
        self.run_git('add', 'src/old.cpp')
        (self.root / 'src/new.c').write_text('void f(){nullptr;}\n')
        report = self.check()
        self.assertEqual(2, report['files_checked'])
        self.assertEqual(2, len(report['findings']))
        (self.root / 'src/old.cpp').write_text('//cpp\nvoid f(){_ZN1A1fEv(); _ZN1C1fEv();}\n')
        self.assertTrue(any(f['match'] == '_ZN1C1fEv' for f in self.check()['findings']))

    def test_rename_keeps_debt_but_new_copy_does_not(self):
        self.run_git('mv', 'src/old.cpp', 'src/renamed.cpp')
        self.assertEqual([], self.check()['findings'])
        for path in ('src/renamed.cpp', str(self.root / 'src/renamed.cpp')):
            report = S.check(self.root, self.base, [path], None, patterns_only=True)
            self.assertEqual([], report['findings'])
        shutil.copyfile(self.root / 'src/renamed.cpp', self.root / 'src/copied.cpp')
        self.assertEqual(1, len(self.check()['findings']))

    def test_baselined_occurrence_consumes_exception_cap(self):
        row = dict(path='src/old.cpp', rule='handspelled-member', match='_ZN1A1fEv',
                   count=1, reason='Measured ABI bridge', evidence='issue/123#experiment')
        (self.root / 'config/source-style-exceptions.json').write_text(
            json.dumps(dict(schema=1, exceptions=[row])))
        (self.root / 'src/old.cpp').write_text('//cpp\nvoid f(){_ZN1A1fEv(); _ZN1A1fEv();}\n')
        self.assertEqual(1, len(self.check()['findings']))

    def test_deleted_file_is_not_opened(self):
        self.run_git('rm', '-q', 'src/old.cpp')
        self.assertEqual(0, self.check()['files_checked'])

    def test_bad_base_is_fatal(self):
        with self.assertRaises(S.StyleError):
            self.check('missing-base-ref')

    def test_merge_base_excludes_changes_on_the_other_branch(self):
        self.run_git('branch', 'pr-base')
        self.run_git('checkout', '-qb', 'other')
        (self.root / 'src/other.cpp').write_text('void f(){nullptr;}\n')
        self.run_git('add', '.')
        self.run_git('commit', '-qm', 'other branch')
        self.run_git('branch', '-f', 'pr-base', 'HEAD')
        self.run_git('checkout', '-q', '--detach', self.base)
        self.assertEqual(0, self.check('pr-base')['files_checked'])

    def test_explicit_missing_file_and_path_escape_are_errors(self):
        for path in ('src/missing.cpp', '../escape.cpp'):
            with self.subTest(path=path), self.assertRaises((OSError, S.StyleError)):
                S.check(self.root, None, [path], None, patterns_only=True)

    def test_formatter_required_even_with_zero_inputs(self):
        with self.assertRaises(S.StyleError):
            S.check(self.root, self.base, None, None)


class Formatter(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not FORMAT:
            raise RuntimeError('Install tools/requirements-style.txt; formatter fixtures cannot be skipped')
        S.formatter_version(FORMAT)

    def test_exact_fixture_and_idempotence(self):
        raw = (FIXTURES / 'format-input.cpp').read_bytes()
        expected = (FIXTURES / 'format-expected.cpp').read_bytes().replace(b'\r\n', b'\n')
        def fmt(text):
            return subprocess.check_output([FORMAT, f'--style=file:{ROOT / ".clang-format"}',
                                            '--assume-filename=fixture.cpp'], input=text).replace(b'\r\n', b'\n')
        self.assertEqual(expected, fmt(raw))
        self.assertEqual(expected, fmt(expected))
        self.assertTrue(expected.startswith(b'//cpp\n'))
        self.assertLess(expected.index(b'z_fixture.h'), expected.index(b'a_fixture.h'))

    def test_good_files_are_formatted(self):
        for name in ('good.cpp', 'good.c', 'format-expected.cpp'):
            text = (FIXTURES / name).read_text()
            self.assertEqual([], S.format_findings(FORMAT, ROOT / '.clang-format', name,
                                                   text, [(1, len(text.splitlines()))]))

    def test_new_bad_formatting_is_reported(self):
        text = 'void f(){return;}\n'
        self.assertTrue(S.format_findings(FORMAT, ROOT / '.clang-format', 'a.cpp', text, [(1, 1)]))

    def test_new_line_indentation_uses_preceding_newline_replacement(self):
        for before, after, line in (
                ('void f()\n{\n}\n', 'void f()\n{\nx();\n}\n', 3),
                ('void f()\n{\n    x();\n}\n', 'void f()\n{\n    x();\ny();\n}\n', 4)):
            findings = S.format_findings(FORMAT, ROOT / '.clang-format', 'a.cpp', after,
                                        S.added_ranges(before, after))
            self.assertTrue(findings)
            self.assertTrue(any(f.line == line for f in findings))

    def test_old_unformatted_line_is_not_a_new_error(self):
        before = 'void old(){return;}\n\nint x;\n'
        after = before + 'int y;\n'
        self.assertEqual([], S.format_findings(FORMAT, ROOT / '.clang-format', 'a.cpp', after,
                                               S.added_ranges(before, after)))

    def test_utf8_offsets_are_bytes(self):
        text = '// Unicode: \u00e9\nvoid f(){return;}\n'
        findings = S.format_findings(FORMAT, ROOT / '.clang-format', 'a.cpp', text, [(2, 2)])
        self.assertTrue(findings)
        self.assertTrue(all(f.line == 2 for f in findings))

    def test_wrong_version_is_fatal(self):
        for version in ('18.1.0', '19.1.7.1'):
            result = subprocess.CompletedProcess([], 0, stdout='clang-format version ' + version)
            with self.subTest(version=version), patch.object(S.subprocess, 'run', return_value=result):
                with self.assertRaises(S.StyleError):
                    S.formatter_version('formatter')

    def test_invalid_xml_and_incomplete_formatting_are_fatal(self):
        for data in (b'invalid', b'<replacements incomplete_format="true"/>'):
            result = subprocess.CompletedProcess([], 0, stdout=data, stderr=b'')
            with self.subTest(data=data), patch.object(S.subprocess, 'run', return_value=result):
                with self.assertRaises(S.StyleError):
                    S.format_findings('formatter', ROOT / '.clang-format', 'a.cpp', 'int x;', [(1, 1)])

    def test_requirements_and_checker_pins_agree(self):
        self.assertIn('clang-format==' + S.FORMAT_VERSION,
                      (ROOT / 'tools/requirements-style.txt').read_text().splitlines())


class FormatSafety(unittest.TestCase):
    def test_sensitive_contexts_are_explicit(self):
        examples = [
            'asm void Copy() { mov r0,r1\n bx lr\n}',
            'int line() { return __LINE__; }',
            '#define WHERE() __LINE__\nint f(){return WHERE();}',
            '%:define SPELL(x) %:x\n',
            '??=define SPELL(x) ??=x\n',
            '#line 40 "source"\nint x;',
            '#define SPELL(x) #x\nconst char* s=SPELL(a+b);',
            '#define SPELL(x) \\\n #x\n',
        ]
        for text in examples:
            with self.subTest(text=text):
                self.assertTrue(S.direct_format_risks(text))
        self.assertEqual([], S.direct_format_risks('// asm and __LINE__ are discussed\nconst char* p="asm";'))
        self.assertTrue(S.direct_format_risks('#define JOIN(a,b) a##b\n'))

    def test_formatter_off_comments_are_explicit_deferrals(self):
        for directive in ('// clang-format off', '/* clang-format off */',
                          '// clang-format off: preserve measured layout'):
            text = directive + '\nvoid f(){return;}\n// clang-format on\n'
            with self.subTest(directive=directive):
                self.assertIn('clang-format off suppresses formatting coverage',
                              S.direct_format_risks(text))
        self.assertEqual([], S.direct_format_risks(
            'const char *s = "// clang-format off";\n'
            '// do not use clang-format off\n'))

    def test_formatter_off_retains_pattern_checks(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / 'src').mkdir()
            (root / 'config').mkdir()
            (root / 'config/source-style-exceptions.json').write_text(
                '{"schema":1,"exceptions":[]}')
            (root / 'src/a.cpp').write_text(
                '// clang-format off\nvoid f(){nullptr;}\n')
            with patch.object(S, 'format_findings', side_effect=AssertionError('suppressed formatter call')):
                report = S.check(root, None, ['src/a.cpp'], FORMAT)
            self.assertEqual(1, len(report['format_deferred']))
            self.assertEqual(['modern-nullptr'], [f['rule'] for f in report['findings']])

    def test_token_spelling_order_and_literal_boundaries(self):
        self.assertEqual(S.tokens('int /* note */ x = 0x1p-2;'), S.tokens('int x=0x1p-2;'))
        self.assertNotEqual(S.tokens('double x=1e-5;'), S.tokens('double x=1e -5;'))
        self.assertNotEqual(S.tokens('"abcdef"'), S.tokens('"abc" "def"'))
        self.assertNotEqual(S.tokens('int z; int a;'), S.tokens('int a; int z;'))

    def test_header_macro_and_reverse_consumer_protection(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / 'src').mkdir(); (root / 'include').mkdir()
            (root / 'include/macros.h').write_text('#define SPELL(x) #x\n')
            (root / 'include/shared.h').write_text('const char* s=SPELL(a+b);\n')
            (root / 'src/use.cpp').write_text('#include "macros.h"\n#include "shared.h"\n')
            guard = S.FormatSafety(root)
            self.assertTrue(any('stringifying' in r for r in guard.reasons(
                'src/use.cpp', (root / 'src/use.cpp').read_text())))
            self.assertTrue(any('stringifying' in r for r in guard.reasons(
                'include/shared.h', (root / 'include/shared.h').read_text())))

    def test_computed_and_external_includes_defer(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir()
            for text in ('#include HEADER\n', '#include <missing.h>\n'):
                self.assertTrue(S.FormatSafety(root).reasons('src/use.cpp', text))

    def test_header_defers_for_unknown_computed_reverse_consumer(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir(); (root / 'include').mkdir()
            (root / 'src/use.cpp').write_text('#define SPELL(x) #x\n#define H "shared.h"\n#include H\n')
            (root / 'include/shared.h').write_text('const char* s=SPELL(a+b);\n')
            reasons = S.FormatSafety(root).reasons('include/shared.h', (root / 'include/shared.h').read_text())
            self.assertTrue(any('unresolved computed include' in r for r in reasons))
            self.assertTrue(any('stringifying' in r for r in reasons))

    def test_reverse_graph_walks_nonstandard_include_suffix(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir(); (root / 'include').mkdir()
            (root / 'src/use.cpp').write_text('#define SPELL(x) #x\n#include "bridge.inc"\n')
            (root / 'include/bridge.inc').write_text('#include "shared.h"\n')
            (root / 'include/shared.h').write_text('const char* s=SPELL(a+b);\n')
            reasons = S.FormatSafety(root).reasons('include/shared.h', (root / 'include/shared.h').read_text())
            self.assertTrue(any('stringifying' in r for r in reasons))

    def test_include_comments_splicing_and_angle_search_order(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir(); (root / 'include').mkdir()
            (root / 'src/macros.h').write_text('int innocent;\n')
            (root / 'include/macros.h').write_text('#define SPELL(x) #x\n')
            for text in ('#include"macros.h"\n', '#/**/include/**/<macros.h>\n',
                         '#inc' + '\\' + '\n' + 'lude <macros.h>\n'):
                # Quoted includes search beside the source before the shared directory.
                reasons = S.FormatSafety(root).reasons('src/use.cpp', text)
                self.assertEqual(not text.startswith('#include"'), bool(reasons), text)

    def test_mwcc_include_graph_ignores_host_only_standard_header(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir()
            text = '#ifdef _MSC_VER\n#include <missing-host.h>\n#endif\nint x;\n'
            self.assertEqual([], S.FormatSafety(root).reasons('src/use.cpp', text))

    def test_assembly_deferred_without_calling_formatter(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td); (root / 'src').mkdir(); (root / 'config').mkdir()
            (root / 'config/source-style-exceptions.json').write_text('{"schema":1,"exceptions":[]}')
            (root / 'src/a.c').write_text('asm void f() { mov r0,r1\n bx lr\n}\n')
            with patch.object(S, 'formatter_version'), patch.object(S, 'format_findings', side_effect=AssertionError('unsafe formatter call')):
                r = S.check(root, None, ['src/a.c'], 'formatter')
            self.assertEqual([], r['findings'])
            self.assertEqual(1, len(r['format_deferred']))

    def test_xml_token_rewrite_is_not_a_formatting_requirement(self):
        xml = b'<replacements><replacement offset="4" length="1">z</replacement></replacements>'
        result = subprocess.CompletedProcess([], 0, stdout=xml, stderr=b'')
        with patch.object(S.subprocess, 'run', return_value=result):
            with self.assertRaises(S.UnsafeFormat):
                S.format_findings('formatter', ROOT / '.clang-format', 'x.cpp', 'int x;', [(1, 1)])

    def test_real_formatter_preserves_class_and_header_order(self):
        for filename in ('order.cpp', 'order.h'):
            text = (FIXTURES / filename).read_text()
            formatted = subprocess.check_output([FORMAT, '--style=file:' + str(ROOT / '.clang-format'),
                                                 '--assume-filename=' + filename], input=text.encode()).decode()
            self.assertEqual(S.tokens(text), S.tokens(formatted))
        header = (FIXTURES / 'order.h').read_text()
        self.assertLess(header.index('Zeta'), header.index('Alpha'))
        self.assertLess(header.index('int mSecond'), header.index('short mFirst'))

    def test_long_string_and_macro_body_are_preserved(self):
        text = '#define TIGHT(x) ((x)+1)\nconst char* s="' + 'a' * 160 + '";\n'
        formatted = subprocess.check_output([FORMAT, '--style=file:' + str(ROOT / '.clang-format'),
                                             '--assume-filename=probe.cpp'], input=text.encode()).decode()
        self.assertEqual(S.tokens(text), S.tokens(formatted))
        self.assertIn('#define TIGHT(x) ((x)+1)', formatted)


class AdvisoryTidy(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if not TIDY:
            raise RuntimeError('Install tools/requirements-style.txt; advisory config fixtures cannot be skipped')

    def test_only_explicit_checks_enabled(self):
        p = subprocess.run([TIDY, '--list-checks', '--config-file=' + str(ROOT / '.clang-tidy')],
                           capture_output=True, text=True)
        self.assertEqual(0, p.returncode, p.stderr)
        checks = {line.strip() for line in p.stdout.splitlines() if line.strip().startswith(('bugprone-', 'modernize-', 'clang-analyzer-'))}
        self.assertEqual({'bugprone-sizeof-expression', 'bugprone-suspicious-memset-usage'}, checks)

    def test_clean_cpp03_arm_fixture_parses(self):
        p = subprocess.run([TIDY, str(FIXTURES / 'good.cpp'),
                            '--config-file=' + str(ROOT / '.clang-tidy'), '--',
                            '-x', 'c++', '-std=c++03', '-target', 'arm-none-eabi', '-mcpu=arm946e-s'],
                           capture_output=True, text=True)
        self.assertEqual(0, p.returncode, p.stdout + p.stderr)
        self.assertNotIn('warning:', p.stdout + p.stderr)


class CommandLine(unittest.TestCase):
    def test_bad_fixture_returns_nonzero_and_records_scope(self):
        with contextlib.redirect_stdout(io.StringIO()) as output:
            code = S.main(['--files', 'tools/style_fixtures/bad.cpp', '--patterns-only'])
        self.assertEqual(1, code)
        self.assertIn('PATTERNS ONLY', output.getvalue())
        self.assertIn('6 finding(s)', output.getvalue())


if __name__ == '__main__':
    unittest.main()
