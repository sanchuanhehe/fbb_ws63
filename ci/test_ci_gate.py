#!/usr/bin/env python3
"""
ci_gate.py 测试用例

运行方式（从项目根目录）:
    python ci/test_ci_gate.py
"""

import unittest
from unittest.mock import patch
import sys
import os

# --- 导入 ci_gate 模块 ---
import importlib.util
_ci_dir = os.path.dirname(os.path.abspath(__file__))
spec = importlib.util.spec_from_file_location(
    "ci_gate",
    os.path.join(_ci_dir, "ci_gate.py")
)
gate = importlib.util.module_from_spec(spec)
spec.loader.exec_module(gate)

# --- 测试数据 ---
# file_path 格式与 os.walk("./") 产生的一致，带 ./ 前缀
VENDOR_LED_FILE = 'vendor/HiHope_NearLink_DK_WS63E_V03/demo/led/led.c'
VENDOR_BEEP_FILE = 'vendor/BearPi-Pico_H3863/demo/beep/beep.c'
SRC_BLINKY_FILE = 'src/application/samples/peripheral/blinky/blinky.c'
SRC_UART_H_FILE = 'src/application/samples/peripheral/uart/uart.h'
README_FILE = 'README.md'
GITIGNORE_FILE = '.gitignore'
CMAKE_FILE = 'src/application/samples/CMakeLists.txt'

SRC_MOCK_ENTRY = {
    'file_path': './ci/build_config.json',
    'build_target': 'ws63-liteos-app',
    'sample_name': 'sdk-full',
    'platform': 'WS63',
    'build_defs': [],
}

MOCK_ENTRIES = [
    {
        'file_path': './vendor/HiHope_NearLink_DK_WS63E_V03/build_config.json',
        'build_target': 'ws63-liteos-app',
        'sample_name': 'demo-led',
        'platform': 'WS63',
        'build_defs': [],
    },
    {
        'file_path': './vendor/HiHope_NearLink_DK_WS63E_V03/build_config.json',
        'build_target': 'ws63-liteos-app',
        'sample_name': 'demo-beep',
        'platform': 'WS63',
        'build_defs': ['CONFIG_SAMPLE_BEEP=y'],
    },
    {
        'file_path': './vendor/BearPi-Pico_H3863/build_config.json',
        'build_target': 'ws63-liteos-app',
        'sample_name': 'demo-beep',
        'platform': 'WS63',
        'build_defs': [],
    },
]

class TestCheckChangesAndGetFolders(unittest.TestCase):
    """check_changes_and_get_folders 的 6 种场景"""

    def setUp(self):
        gate.has_src_changes = False

    # ── 场景1: 仅修改 src/ 下的 .c/.h ──
    def test_only_src_changes(self):
        """仅src修改 → has_src_changes=True, changed_folders为空set"""
        result = gate.check_changes_and_get_folders([SRC_BLINKY_FILE, SRC_UART_H_FILE])
        self.assertTrue(gate.has_src_changes)
        self.assertEqual(result, set())

    # ── 场景2: 仅修改 vendor/ 下的 .c/.h ──
    def test_only_vendor_changes(self):
        """仅vendor修改 → has_src_changes=False, 提取vendor条目"""
        result = gate.check_changes_and_get_folders([VENDOR_LED_FILE])
        self.assertFalse(gate.has_src_changes)
        self.assertIn('HiHope_NearLink_DK_WS63E_V03+demo+led', result)

    # ── 场景3: 同时修改 src/ 和 vendor/ ──
    def test_both_src_and_vendor(self):
        """同时修改 → has_src_changes=True 且 vendor条目仍被提取"""
        result = gate.check_changes_and_get_folders([SRC_BLINKY_FILE, VENDOR_LED_FILE])
        self.assertTrue(gate.has_src_changes)
        self.assertIn('HiHope_NearLink_DK_WS63E_V03+demo+led', result)

    # ── 场景4: 只有文档/git配置文件修改 ──
    def test_non_build_files_skipped(self):
        """仅文档(.md)和git配置 → 返回None, has_src_changes保持False"""
        result = gate.check_changes_and_get_folders([README_FILE, GITIGNORE_FILE])
        self.assertIsNone(result)
        self.assertFalse(gate.has_src_changes)

    # ── 场景4b: CMakeLists.txt等构建系统文件修改会触发构建 ──
    def test_build_system_files_trigger_build(self):
        """CMakeLists.txt变更 → has_src_changes=True, 返回空set触发全量编译"""
        result = gate.check_changes_and_get_folders([CMAKE_FILE])
        self.assertTrue(gate.has_src_changes)
        self.assertEqual(result, set())

    # ── 场景5: ci_gate.py 被修改 ──
    def test_ci_gate_modified(self):
        """修改ci_gate.py → 先跑测试, has_src_changes=True, 返回空set触发全量编译"""
        with patch.object(gate, 'run_ci_gate_tests'):
            result = gate.check_changes_and_get_folders(['ci/ci_gate.py'])
        self.assertTrue(gate.has_src_changes)
        self.assertEqual(result, set())

    # ── 边界: src 路径不足4级的文件不会被提取到 changed_folders ──
    def test_src_shallow_path(self):
        """src/下浅层.c文件 → has_src_changes=True但changed_folders为空"""
        result = gate.check_changes_and_get_folders(['src/main.c'])
        self.assertTrue(gate.has_src_changes)
        self.assertEqual(result, set())


class TestExtractExactMatch(unittest.TestCase):
    """extract_exact_match 匹配逻辑"""

    def test_match_found(self):
        """vendor变更匹配到对应build_config条目"""
        match_list = {'HiHope_NearLink_DK_WS63E_V03+demo+led'}
        result = gate.extract_exact_match(MOCK_ENTRIES, match_list)
        self.assertEqual(len(result), 1)
        self.assertEqual(result[0]['sample_name'], 'demo-led')

    def test_multiple_matches(self):
        """多个vendor变更匹配多个条目"""
        match_list = {
            'HiHope_NearLink_DK_WS63E_V03+demo+led',
            'HiHope_NearLink_DK_WS63E_V03+demo+beep',
        }
        result = gate.extract_exact_match(MOCK_ENTRIES, match_list)
        self.assertEqual(len(result), 2)

    def test_no_match_exits(self):
        """无匹配条目 → SystemExit(0)"""
        with self.assertRaises(SystemExit) as cm:
            gate.extract_exact_match(MOCK_ENTRIES, {'UnknownCompany+unknown+sample'})
        self.assertEqual(cm.exception.code, 0)

    def test_empty_match_list(self):
        """空match_list → SystemExit(0)"""
        with self.assertRaises(SystemExit) as cm:
            gate.extract_exact_match(MOCK_ENTRIES, set())
        self.assertEqual(cm.exception.code, 0)


class TestProcessBuildInfoFiles(unittest.TestCase):
    """build_config.json 解析（依赖实际文件）"""

    @classmethod
    def setUpClass(cls):
        cls._orig_cwd = os.getcwd()
        os.chdir(os.path.dirname(_ci_dir))  # 切到项目根目录

    @classmethod
    def tearDownClass(cls):
        os.chdir(cls._orig_cwd)

    def test_returns_list(self):
        result = gate.process_build_info_files()
        self.assertIsInstance(result, list)

    def test_entries_have_required_keys(self):
        result = gate.process_build_info_files()
        for entry in result:
            for key in ('file_path', 'build_target', 'sample_name', 'platform', 'build_defs'):
                self.assertIn(key, entry, f"缺少 key: {key}")

    def test_build_defs_is_always_list(self):
        result = gate.process_build_info_files()
        for entry in result:
            self.assertIsInstance(entry['build_defs'], list)

    def test_sample_name_uses_dash_not_slash(self):
        result = gate.process_build_info_files()
        for entry in result:
            self.assertNotIn('/', entry['sample_name'])

    def test_build_defs_no_comma(self):
        result = gate.process_build_info_files()
        for entry in result:
            for d in entry['build_defs']:
                self.assertNotIn(',', d)


class TestMainFlow(unittest.TestCase):
    """main() 编排——验证三种场景走对路径"""

    def setUp(self):
        gate.has_src_changes = False
        gate.global_combined = ''

    def test_only_src_calls_compile_directly(self):
        """仅src修改 → 读取ci/build_config.json编译SDK，不调extract_exact_match"""
        gate.has_src_changes = True
        with patch.object(gate, 'compare_with_remote_master', return_value=set()):
            with patch.object(gate, '_load_src_build_info', return_value=[SRC_MOCK_ENTRY]):
                with patch.object(gate, 'compile_sdk_and_save_log') as mock_compile:
                    with patch.object(gate, 'extract_exact_match') as mock_extract:
                        with patch.object(gate, 'process_build_info_files'):
                            gate.main()

        mock_compile.assert_called_once_with('ws63-liteos-app')
        mock_extract.assert_not_called()
        self.assertEqual(gate.global_combined, 'ws63-liteos-app_sdk-full_WS63')

    def test_only_vendor_goes_prepare_compile_cleanup_loop(self):
        """仅vendor修改 → extract + prepare/compile/cleanup循环"""
        gate.has_src_changes = False
        matched = [MOCK_ENTRIES[0]]
        with patch.object(gate, 'compare_with_remote_master', return_value={'some+folder'}):
            with patch.object(gate, 'process_build_info_files', return_value=MOCK_ENTRIES):
                with patch.object(gate, 'extract_exact_match', return_value=matched) as mock_extract:
                    with patch.object(gate, 'sample_build_prepare_one') as mock_prepare:
                        with patch.object(gate, 'compile_sdk_and_save_log') as mock_compile:
                            with patch.object(gate, 'sample_build_cleanup_one') as mock_cleanup:
                                gate.main()

        mock_extract.assert_called_once()
        mock_prepare.assert_called_once_with(matched[0])
        mock_compile.assert_called_once_with(matched[0]['build_target'])
        mock_cleanup.assert_called_once_with(matched[0])

    def test_both_src_and_vendor_goes_vendor_path(self):
        """src+vendor同时修改 → 走vendor路径（check_list非空）"""
        gate.has_src_changes = True
        matched = [MOCK_ENTRIES[0], MOCK_ENTRIES[1]]
        with patch.object(gate, 'compare_with_remote_master',
                          return_value={'HiHope_NearLink_DK_WS63E_V03+demo+led'}):
            with patch.object(gate, 'process_build_info_files', return_value=MOCK_ENTRIES):
                with patch.object(gate, 'extract_exact_match', return_value=matched):
                    with patch.object(gate, 'sample_build_prepare_one') as mock_prepare:
                        with patch.object(gate, 'compile_sdk_and_save_log') as mock_compile:
                            with patch.object(gate, 'sample_build_cleanup_one') as mock_cleanup:
                                gate.main()

        self.assertEqual(mock_prepare.call_count, 2)
        self.assertEqual(mock_compile.call_count, 2)
        self.assertEqual(mock_cleanup.call_count, 2)


class TestBuildDefsHelpers(unittest.TestCase):
    """_apply_build_defs / _remove_build_defs"""

    def setUp(self):
        self.insert_calls = []
        self.delete_calls = []

    def _mock_insert(self, file_path, target_line, content):
        self.insert_calls.append((file_path, content))

    def _mock_delete(self, file_path, content):
        self.delete_calls.append((file_path, content))

    def test_menuconfig_def(self):
        """=y 宏 → 写入 menuconfig"""
        with patch.object(gate, 'insert_content_before_line', self._mock_insert):
            gate._apply_build_defs(['CONFIG_X=y'])
        self.assertEqual(len(self.insert_calls), 1)
        self.assertIn('CONFIG_X=y', self.insert_calls[0][1])

    def test_menuconfig_def_with_spaces(self):
        """= y 宏 → 写入 menuconfig"""
        with patch.object(gate, 'insert_content_before_line', self._mock_insert):
            gate._apply_build_defs(['CONFIG_X = y'])
        self.assertEqual(len(self.insert_calls), 1)

    def test_errcode_def(self):
        """非 =y 宏 → 写入 errcode.h"""
        with patch.object(gate, 'insert_content_before_line', self._mock_insert):
            gate._apply_build_defs(['CONFIG_VAL=10'])
        self.assertEqual(len(self.insert_calls), 1)
        self.assertIn('#define CONFIG_VAL 10', self.insert_calls[0][1])

    def test_mixed_defs(self):
        """混合宏：menuconfig + errcode 各一个"""
        with patch.object(gate, 'insert_content_before_line', self._mock_insert):
            gate._apply_build_defs(['CONFIG_A=y', 'CONFIG_B=5'])
        self.assertEqual(len(self.insert_calls), 2)
        self.assertIn('CONFIG_A=y', self.insert_calls[0][1])
        self.assertIn('#define CONFIG_B 5', self.insert_calls[1][1])

    def test_remove_defs(self):
        """清除宏"""
        with patch.object(gate, 'delete_specific_content', self._mock_delete):
            gate._remove_build_defs(['CONFIG_A=y', 'CONFIG_B=5'])
        self.assertEqual(len(self.delete_calls), 2)


if __name__ == '__main__':
    unittest.main(verbosity=2)
