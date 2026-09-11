"""Exercise the actual wide-conductor transcription gate and its failure paths."""
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import hostloop_transcription_check as H


class TranscriptionCheck(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.root = pathlib.Path(__file__).resolve().parents[2]
        cls.src = (cls.root / 'src/func_0203ea5c.c').read_text()
        cls.wide = (cls.root / 'port/hal/comms_conductor_wide.cpp').read_text()

    def reject(self, old, new):
        self.assertIn(old, self.wide)
        candidate = self.wide.replace(old, new, 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_real_candidate_matches_all_original_lines_and_externs(self):
        self.assertEqual(H.check_sources(self.src, self.wide),
                         {'lines': 406, 'externs': 60, 'diagnostic_helpers': 3})

    def test_cli_uses_its_actual_tree_even_from_another_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            result = subprocess.run([sys.executable, str(self.root / 'port/tools/hostloop_transcription_check.py')],
                                    cwd=directory, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn('406 lines', result.stdout)
        self.assertIn('60 symbols', result.stdout)

    def test_helper_comment_change_requires_a_fresh_source_pin_review(self):
        self.reject('++n;', '/* observational count */ ++n;')

    def test_indentation_and_line_endings_do_not_change_source_pins(self):
        helper = H.function(self.wide, 'void report_bound_expiry(')
        self.assertEqual(H.source_digest(helper),
                         H.source_digest('\r\n'.join('  ' + line for line in helper.split('\n'))))

    def test_split_increment_is_not_the_same_operator(self):
        for changed in ('+ +n;', '+/*separator*/+n;'):
            with self.subTest(changed=changed):
                self.reject('++n;', changed)

    def test_include_directive_logical_line_boundaries_are_preserved(self):
        self.reject('#include <chrono>\n#include <cstdio>', '#include <chrono> #include <cstdio>')


    def test_changed_wait_condition_is_rejected(self):
        self.reject('while ((sp8 == 0) && (sp4 != 0))', 'while ((sp8 == 0) && (sp4 > 1))')

    def test_changed_game_state_assignment_is_rejected(self):
        self.reject('data_020a0f04 = 0;', 'data_020a0f04 = 1;')

    def test_changed_game_callee_is_rejected(self):
        self.reject('data_020a0f10 = func_02040704(temp_r0_5);',
                    'data_020a0f10 = func_02040714(temp_r0_5);')

    def test_diagnostic_counter_cannot_become_a_state_write(self):
        self.reject('++vs7_turns;', '++data_020a0f04;')

    def test_diagnostic_counter_cannot_move_outside_the_loop(self):
        candidate = self.wide.replace('++vs7_turns;', '', 1)
        candidate = candidate.replace('while ((sp8 == 0) && (sp4 != 0))',
                                      '++vs7_turns;\n    while ((sp8 == 0) && (sp4 != 0))', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_duplicate_instrument_is_not_erased(self):
        self.reject('++vs7_with_data;', '++vs7_with_data;\n            ++vs7_with_data;')

    def test_changed_report_argument_is_rejected(self):
        self.reject('report_bound_expiry(vs7_bound, vs7_turns, vs7_with_data,',
                    'report_bound_expiry(vs7_bound = 0, vs7_turns, vs7_with_data,')

    def test_missing_report_is_rejected(self):
        self.reject('report_bound_expiry(vs7_bound, vs7_turns, vs7_with_data,\n                        host_ms() - vs7_t0);', '')

    def test_warning_prefix_is_not_a_blanket_exception(self):
        self.reject('warn_info_mode_armed_wide();', 'warn_info_mode_armed_wide_mutates_state();')

    def test_warning_cannot_move_to_another_game_branch(self):
        candidate = self.wide.replace('warn_info_mode_armed_wide();', '', 1)
        candidate = candidate.replace('data_020a0f30 = 1;',
                                      'warn_info_mode_armed_wide();\n        data_020a0f30 = 1;', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_local_silencer_cannot_move_into_a_game_branch(self):
        candidate = self.wide.replace('(void)temp_r0_7;', '', 1)
        candidate = candidate.replace('data_020a0f30 = 1;',
                                      '(void)temp_r0_7;\n        data_020a0f30 = 1;', 1)
        with self.assertRaises(ValueError):
            H.check_sources(self.src, candidate)

    def test_extern_width_change_is_rejected(self):
        self.reject('extern u8 data_020a0f04;', 'extern u16 data_020a0f04;')

    def test_logging_helper_cannot_acquire_a_game_state_write(self):
        self.reject('std::fflush(stderr);', 'data_020a0f04 = 1;')

    def test_clock_helper_cannot_acquire_a_game_state_write(self):
        self.reject('return (unsigned)std::chrono::duration_cast',
                    'data_020a0f04 = 1;\n    return (unsigned)std::chrono::duration_cast')

    def test_warning_helper_cannot_acquire_a_game_state_write(self):
        self.reject('said = true;', 'said = true;\n    data_020a0f04 = 1;')

    def test_continued_comment_cannot_hide_a_real_statement(self):
        self.reject('data_020a0f30 = 1;', '// removed by line splicing \\\n        data_020a0f30 = 1;')

    def test_only_seven_reviewed_record_walks_may_be_widened(self):
        for old, new in (('} while (var_r5_2 < 4);', '} while (var_r5_2 < kRecs);'),
                         ('} while (spC < 4);', '} while (spC < kRecs);'),
                         ('sp18 = 4;', 'sp18 = kRecs;')):
            with self.subTest(old=old):
                self.reject(old, new)

    def test_each_authorized_widening_is_required(self):
        for line in ('} while ((s32) var_r7 < kRecs);', 'if (var_r2 < kRecs) {',
                     'if (var_r5_3 < kRecs) {', 'if (var_r3_2 < kRecs) {',
                     '} while (var_r0 < kRecs);', 'if (var_r2_3 < kRecs) {',
                     'if (var_r2_4 < kRecs) {'):
            with self.subTest(line=line):
                self.reject(line, line.replace('kRecs', '4'))

    def test_disabled_old_helper_cannot_hide_an_active_changed_definition(self):
        helper = H.function(self.wide, 'void warn_info_mode_armed_wide(void)')
        self.reject(helper, '#if 0\n' + helper + '\n#endif\n'
                    'void warn_info_mode_armed_wide (void) { data_020a0f04 = 1; }')

    def test_new_callee_macro_or_context_directive_is_rejected(self):
        for directive in ('#define func_02040704(x) ((u8)1)', '#if 0', '#pragma pack(1)'):
            with self.subTest(directive=directive):
                self.reject('void conductor_wide(void)', directive + '\nvoid conductor_wide(void)')

    def test_copy_callee_contract_is_compared_by_its_real_name(self):
        self.assertIn('CpuCopy8', H.decls(self.src))
        self.assertEqual(len(H.decls(self.src)), 60)
        self.reject('extern void CpuCopy8(void *dst, void *src, s32 len);',
                    'extern void CpuCopy8(void *dst, void *src, s16 len);')

    def test_missing_or_extra_named_game_extern_is_rejected(self):
        self.reject('extern void CpuCopy8(void *dst, void *src, s32 len);', '')
        self.reject('extern void CpuCopy8(void *dst, void *src, s32 len);',
                    'extern void CpuCopy8(void *dst, void *src, s32 len);\nextern int NewGameState;')

    def test_dispatch_or_record_definition_changes_are_rejected(self):
        self.reject('enum { kRecs = kPortMaxPlayers };', 'enum { kRecs = 4 };')
        self.reject('u8 unk12[0x10];', 'u8 unk12[0x11];')

    def test_original_source_context_also_requires_review(self):
        candidate = self.src.replace('s32 len);', 's16 len);', 1)
        with self.assertRaises(ValueError):
            H.check_sources(candidate, self.wide)

    def test_missing_or_repeated_function_fails_closed(self):
        for candidate in (self.wide.replace('void conductor_wide(void)', 'void other(void)', 1),
                          self.wide + '\nvoid conductor_wide(void) {}'):
            with self.subTest(candidate=candidate[-70:]):
                with self.assertRaises(ValueError):
                    H.check_sources(self.src, candidate)


if __name__ == '__main__':
    unittest.main()
