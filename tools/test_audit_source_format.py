"""No-ROM regressions for audit isolation, phase coverage and safety deferrals."""
import contextlib
import hashlib
import io
import pathlib
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import audit_source_format as A


class Audit(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        (self.root / 'tools').mkdir()
        for name in ('source_style.py', 'delaunder.py', 'msvc_arms.py', 'build_pin.py', 'rombuild.py'):
            (self.root / 'tools' / name).write_text('# test implementation')
        (self.root / 'src').mkdir()
        (self.root / 'include').mkdir()
        (self.root / '.clang-format').write_text('BasedOnStyle: LLVM')
        self.source = self.root / 'src/probe.c'
        self.header = self.root / 'include/probe.h'
        self.source.write_text('#include "probe.h"\nint probe(){return VALUE;}\n')
        self.header.write_text('#define VALUE 1\n')
        self.calls = []

    def compile(self, rel, mirror, output):
        self.calls.append((mirror / rel, output.name, (mirror / rel).read_bytes(),
                           (mirror / 'include/probe.h').read_bytes()))
        # Mock compilation only: tests must not claim a compiler or byte proof.
        return {'status': 'compiled', 'sha256': 'same-test-hash'}

    def run_audit(self, **kwargs):
        def git(cmd, **opts):
            if 'ls-files' in cmd:
                return b'src/probe.c\0include/probe.h\0'
            return 'test-commit\n'
        with patch.object(A, 'ROOT', self.root), \
             patch.object(A.rombuild, 'enrolled', return_value=['src/probe.c']), \
             patch.object(A.source_style, 'formatter_version'), \
             patch.object(A.subprocess, 'check_output', side_effect=git), \
             patch.object(A, 'compile_input', side_effect=self.compile), \
             patch.object(A, 'format_input', side_effect=lambda exe, cfg, rel, raw: raw.replace(b'1', b' 1 ')), \
             contextlib.redirect_stdout(io.StringIO()):
            return A.audit(self.root / 'build/audit', 'formatter', jobs=2, **kwargs)

    def test_identical_paths_isolated_inputs_and_all_three_phases(self):
        original = self.source.read_bytes(), self.header.read_bytes()
        report = self.run_audit()
        self.assertTrue(report['complete'])
        self.assertEqual(3, len(self.calls))
        self.assertEqual(1, len({str(c[0]) for c in self.calls}))
        self.assertNotEqual(self.source, self.calls[0][0])
        self.assertEqual(self.calls[0][3], self.calls[1][3])
        self.assertNotEqual(self.calls[1][3], self.calls[2][3])
        self.assertEqual(original, (self.source.read_bytes(), self.header.read_bytes()))
        self.assertEqual({'identical': 1}, report['summary']['formatted_sources_and_headers'])
        self.assertEqual(hashlib.sha256(original[0]).hexdigest(), report['input_sha256']['src/probe.c'])

    def test_sensitive_source_is_retained_and_reported(self):
        self.source.write_text('int probe(){return __LINE__;}\n')
        report = self.run_audit()
        self.assertIn('src/probe.c', report['deferred'])
        self.assertEqual(1, len({c[2] for c in self.calls}))
        self.assertEqual(0, report['source_changed_count'])

    def test_token_change_is_deferred(self):
        self.source.write_text('int probe(){return 1;}\n')
        with patch.object(A.source_style, 'tokens', side_effect=lambda text: [text]):
            report = self.run_audit()
        self.assertIn('token spelling/order', report['deferred']['src/probe.c'][0])
        self.assertEqual(1, len({c[2] for c in self.calls}))

    def test_failed_control_cannot_be_an_invariance_pass(self):
        def fail_control(rel, mirror, output):
            return {'status': 'compile_error'} if output.name == 'control' else {'status': 'compiled', 'sha256': 'x'}
        self.compile = fail_control
        report = self.run_audit()
        self.assertEqual({'unverified_control': 1}, report['summary']['formatted_sources'])

    def test_output_escape_existing_directory_and_invalid_selection_rejected(self):
        with patch.object(A, 'ROOT', self.root), patch.object(A.source_style, 'formatter_version'), \
             patch.object(A.rombuild, 'enrolled', return_value=['src/probe.c']):
            for output in (self.root, self.root / 'src/new', self.root / 'build/../include/new'):
                with self.subTest(output=output), self.assertRaises(ValueError):
                    A.audit(output, 'formatter')
            for args in ({'sources': ['src/unclaimed.c']}, {'sources': []}, {'jobs': 0}, {'mode': 'unknown'}):
                with self.subTest(args=args), self.assertRaises(ValueError):
                    A.audit(self.root / 'build/new', 'formatter', **args)

    def test_include_and_pragma_order_signature_ignores_trailing_comments(self):
        before = b'//cpp\n#include "z.h" // first\r\n#include "a.h"\n#pragma defer_codegen on\n// @symbol probe\n'
        after = before.replace(b' // first', b'  // first').replace(b'\r\n', b'\n')
        self.assertEqual(A.order_signature(before), A.order_signature(after))
        self.assertNotEqual(A.order_signature(before), A.order_signature(before.replace(b'z.h', b'b.h')))


if __name__ == '__main__':
    unittest.main()
