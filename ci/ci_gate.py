#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
# ============================================================================
"""
CI 门禁脚本, 根据变更来源决定编译范围:

    仅 vendor/ 下构建相关文件变更 → 逐一编译受影响的 sample
    仅 src/    下构建相关文件变更 → 全量编译 SDK
    两者同时变更                  → 逐一编译受影响的 sample
    无构建相关文件变更            → 跳过
    ci_gate.py 自身变更           → 先跑测试用例, 再全量编译

文件过滤采用黑名单模式: 只排除明确不会影响构建的文档(.md/.rst)、图片(.png/.jpg等)
和 git 配置文件(.gitignore等), 其余所有文件变更均视为可能影响构建。
"""

import subprocess
import json
import os
import sys
import time
import stat
import shutil
import hashlib
import threading
from typing import List, Set, Optional

# ============================================================
# 常量定义
# ============================================================
BUILD_INFO_FILENAME = 'build_config.json'
CMAKE_SEARCH_STRING = 'build_component()'
SAMPLE_CMAKELISTS_FILE = 'src/application/samples/CMakeLists.txt'
MENUCONFIG_FILE = 'src/build/config/target_config/ws63/menuconfig/acore/ws63_liteos_app.config'
SAMPLE_DIRECTORY = 'src/application/samples'
BUILD_DIRECTORY = 'src'
BUILD_SCRIPT = 'build.py'
OUTPUT_FWPKG_PATH = 'output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg'
ARCHIVE_DIRECTORY = 'archives'
ERRCODE_H_FILE = 'src/include/errcode.h'
ERRCODE_SEARCH_STRING = '#define ERRCODE_SUCC                                        0UL'
DEFAULT_BUILD_TIMEOUT = 60 * 5

# 明确不会影响构建结果的文件（黑名单）
# 不在黑名单中的文件一律视为可能影响构建
NON_BUILD_EXTENSIONS = (
    '.md', '.rst',                                     # 文档
    '.png', '.jpg', '.jpeg', '.gif', '.svg', '.ico',   # 图片
    '.bmp',
)

NON_BUILD_BASENAMES = (
    '.gitignore',
    '.gitattributes',
    '.gitmodules',
    'README',
    'CHANGELOG',
    'LICENSE',
    'NOTICE',
)

# ============================================================
# 全局状态
# ============================================================
has_src_changes = False
global_combined = ''


# ============================================================
# 日志输出辅助函数
# ============================================================

def _phase(title: str) -> None:
    """打印阶段标题"""
    print(f"\n{'=' * 68}")
    print(f"  [{title}]")
    print(f"{'=' * 68}")


def _step(title: str) -> None:
    """打印步骤标题"""
    print(f"\n  >> {title}")


def _info(key: str, value) -> None:
    """打印键值对信息"""
    print(f"     {key}: {value}")


def _ok(msg: str) -> None:
    """打印成功信息"""
    print(f"  [OK] {msg}")


def _fail(msg: str) -> None:
    """打印失败信息"""
    print(f"  [FAIL] {msg}")


def _warn(msg: str) -> None:
    """打印警告信息"""
    print(f"  [WARN] {msg}")


def _line(msg: str = "") -> None:
    """打印一条普通信息"""
    print(f"    {msg}")


def _git_branch_name() -> str:
    """获取当前分支名称"""
    result = subprocess.run(
        ["git", "rev-parse", "--abbrev-ref", "HEAD"],
        capture_output=True, text=True
    )
    return result.stdout.strip() if result.returncode == 0 else "unknown"


def _is_build_relevant(file_path: str) -> bool:
    """
    判断文件是否可能影响构建结果（黑名单模式）。

    只排除明确不会影响构建的文件（文档、图片、git 配置等），
    其余文件一律视为可能影响构建。
    """
    normalized = file_path.replace('\\', '/')
    basename = os.path.basename(normalized)

    if basename.endswith(NON_BUILD_EXTENSIONS):
        return False

    if basename in NON_BUILD_BASENAMES:
        return False

    return True


# ============================================================
# git 差异分析
# ============================================================

def compare_with_remote_master() -> Set[str]:
    """
    比较当前分支与 origin/master 的差异文件，
    调用 check_changes_and_get_folders 提取受影响的 vendor/sample key 集合。
    """
    result = subprocess.run(
        ["git", "diff", "--name-only", "origin/master", "HEAD"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        _fail(f"git diff 失败: {result.stderr}")
        sys.exit(-1)

    changed_files = result.stdout.strip().split("\n") if result.stdout else []
    _info("当前分支", _git_branch_name())
    _info("对比基线", "origin/master")
    _info(f"变更文件数", len(changed_files))
    if changed_files:
        for f in changed_files:
            normalized = f.replace('\\', '/')
            print(f"      - {normalized}")

    return check_changes_and_get_folders(changed_files)


def check_changes_and_get_folders(changed_files: List[str]) -> Optional[Set[str]]:
    """
    分析变更文件列表（黑名单模式: 仅排除文档/图片/git配置, 其余均视为构建相关）。

    - 如果只有非构建相关文件变更 → 返回 None（无需编译）
    - 如果变更了 ci/ci_gate.py 本身 → 先跑测试用例, 然后设置 has_src_changes = True 触发全量编译
    - src/ 下的构建相关文件变更 → 设置 has_src_changes = True
    - vendor/ 下的构建相关文件变更 → 提取 "{vendor_name}+{subdir}+{sample}" key 加入返回集合

    返回: None（无需编译）或 Set[str]（受影响的 vendor key 集合，可能为空）
    """
    global has_src_changes
    has_src_changes = False

    # ci_gate.py 自身被修改 → 先跑测试用例，全部通过后走全量编译验证
    ci_gate_modified = False
    for file_path in changed_files:
        normalized = file_path.replace('\\', '/')
        if normalized in ('ci/ci_gate.py', 'ci_gate.py'):
            has_src_changes = True
            ci_gate_modified = True
            _step("检测到 ci_gate.py 自身变更, 先运行门禁测试用例")
            run_ci_gate_tests()

    # 仅保留会影响构建结果的文件
    build_files = [f for f in changed_files if _is_build_relevant(f)]
    _info("构建相关变更文件数", len(build_files))
    if build_files:
        for f in build_files:
            print(f"      - {f.replace(chr(92), '/')}")

    if not build_files:
        if ci_gate_modified:
            _info("变更来源", "仅 ci_gate.py (自身)")
            _info("编译策略", "全量编译 SDK")
            return set()
        _info("结论", "无构建相关文件变更, 跳过编译")
        return None

    changed_folders = set()
    for file_path in build_files:
        file_path = file_path.replace('\\', '/')

        if file_path.startswith('src/'):
            has_src_changes = True

        if file_path.startswith('vendor/'):
            parts = file_path.split('/')
            if len(parts) >= 4:
                folder_name = '+'.join(parts[1:4])
                changed_folders.add(folder_name)

    _info("has_src_changes", has_src_changes)
    _info("受影响的 vendor key", changed_folders if changed_folders else "(无)")
    return changed_folders


# ============================================================
# build_config.json 解析
# ============================================================

def process_build_info_files() -> List[dict]:
    """
    遍历所有 vendor/*/build_config.json，返回展平后的条目列表。

    每条目包含: file_path, build_target, sample_name, platform, build_defs
    """
    result_list = []

    for root, dirs, files in os.walk("./"):
        for file in files:
            if file == BUILD_INFO_FILENAME:
                file_path = os.path.join(root, file).replace('\\', '/')

                with open(file_path, 'r', encoding='utf-8') as f:
                    try:
                        data = json.load(f)
                        for item in data:
                            build_target = item.get('buildTarget', '')
                            relative_path = item.get('relativePath', '').replace('/', '-')
                            chip_name = item.get('chip', '')

                            build_defs = []
                            if item.get('buildDef', ''):
                                build_defs = [
                                    d.strip() for d in item['buildDef'].split(',')
                                    if d.strip()
                                ]

                            entry = {
                                'file_path': file_path,
                                'build_target': build_target,
                                'sample_name': relative_path,
                                'platform': chip_name,
                                'build_defs': build_defs,
                            }
                            result_list.append(entry)
                    except json.JSONDecodeError:
                        _fail(f"JSON 解析失败: {file_path}")
                        sys.exit(-1)

    _info("解析到的 build_config 文件数", len(result_list))
    return result_list


def extract_exact_match(entries: List[dict], match_list: Set[str]) -> List[dict]:
    """
    将 git 差异提取出的 vendor key 集合与 build_config 条目进行精确匹配。

    匹配规则:
      对每条 entry, 从 file_path 提取 vendor_name,
      将 sample_name 的 '-' 转换为 '+' 后拼接:
        "{vendor_name}+{sample_key}"
      若该 key 在 match_list 中 → 匹配成功。

    若 match_list 为空或无任何匹配 → sys.exit(0)
    """
    if not match_list:
        _warn("match_list 为空, 无法匹配")
        sys.exit(0)

    exact_matches = []
    for entry in entries:
        file_path = entry['file_path']
        parts = file_path.split('/')
        if len(parts) >= 3:
            vendor_name = parts[2]  # ./vendor/{vendor_name}/build_config.json
        else:
            continue

        sample_key = entry['sample_name'].replace('-', '+')
        combined_key = vendor_name + '+' + sample_key

        if combined_key in match_list:
            exact_matches.append(entry)

    _info("匹配到的条目数", len(exact_matches))
    for m in exact_matches:
        _line(f"vendor={m['file_path'].split('/')[2]}, "
              f"sample={m['sample_name']}, "
              f"target={m['build_target']}, "
              f"defs={m['build_defs']}")

    if not exact_matches:
        _fail("变更文件与 build_config.json 未匹配, 请确认 build_config 是否同步更新")
        sys.exit(0)

    return exact_matches


# ============================================================
# 文件操作工具
# ============================================================

def run_ci_gate_tests() -> None:
    """运行 CI 门禁自身测试用例，全部通过才允许继续编译"""
    test_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_ci_gate.py')
    _info("测试脚本", test_path)

    result = subprocess.run(
        ["python", test_path],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        _fail("门禁测试用例未全部通过!")
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
        sys.exit(result.returncode)
    _ok("门禁测试用例全部通过")


def insert_content_before_line(file_path: str, target_line: str, content_to_insert: str) -> None:
    """在文件的 target_line 之前插入 content_to_insert；找不到则追加到末尾"""
    found_target_line = False
    content_stripped = content_to_insert.strip()
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
            for i in range(len(lines)):
                if target_line in lines[i]:
                    lines.insert(i, content_to_insert)
                    found_target_line = True
                    break

            if not found_target_line:
                lines.append(content_to_insert)

            with open(file_path, 'w', encoding='utf-8') as file:
                file.writelines(lines)
    except FileNotFoundError:
        _warn(f"文件不存在, 跳过写入: {file_path}")


def delete_specific_content(file_path: str, content_to_delete: str) -> None:
    """删除文件中所有包含 content_to_delete 的行"""
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    modified_lines = [line for line in lines if content_to_delete not in line]

    with open(file_path, 'w', encoding='utf-8') as file:
        file.writelines(modified_lines)


def move_file(source_file: str, new_filename: str) -> None:
    """将构建产物移动到 archives/ 目录"""
    target_file = os.path.join(f'../{ARCHIVE_DIRECTORY}', f'{new_filename}.fwpkg')
    os.chmod(source_file, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    try:
        shutil.move(source_file, target_file)
        os.chmod(target_file, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    except FileNotFoundError:
        _warn(f"产物文件不存在: {source_file}")
    except PermissionError:
        _warn(f"权限不足, 无法移动产物: {source_file}")
    except Exception as e:
        _warn(f"产物移动失败: {str(e)}")


# ============================================================
# 构建宏处理
# ============================================================

def _apply_build_defs(build_defs: List[str]) -> None:
    """
    应用构建宏:
      - =y 或 = y 类型 → 写入 menuconfig 文件
      - 其他类型 (=value) → 写入 errcode.h 作为 #define
    """
    for defn in build_defs:
        if '=y' in defn or '= y' in defn:
            insert_content_before_line(MENUCONFIG_FILE, CMAKE_SEARCH_STRING, f'{defn}\n')
        else:
            def_name_cleaned = defn.replace('=', ' ')
            insert_content_before_line(ERRCODE_H_FILE, ERRCODE_SEARCH_STRING,
                                       f'#define {def_name_cleaned}\n')


def _remove_build_defs(build_defs: List[str]) -> None:
    """
    移除构建宏（与 _apply_build_defs 对应）:
      - =y / = y 类型 → 从 menuconfig 文件中删除
      - 其他类型 → 从 errcode.h 中删除对应的 #define
    """
    for defn in build_defs:
        if '=y' in defn or '= y' in defn:
            delete_specific_content(MENUCONFIG_FILE, defn)
        else:
            def_name_cleaned = defn.replace('=', ' ')
            delete_specific_content(ERRCODE_H_FILE, f'#define {def_name_cleaned}')


# ============================================================
# 单个 sample 的 prepare / cleanup
# ============================================================

def _build_artifact_name(build_target: str, sample_name: str,
                         platform: str, build_defs: List[str]) -> str:
    """生成构建产物名称"""
    name = f'{build_target}_{sample_name}_{platform}'
    if build_defs:
        name += '_'
        sha = hashlib.sha256('-'.join(build_defs).encode('utf-8', errors='ignore'))
        name += sha.hexdigest()[:32]
    return name


def _load_src_build_info() -> List[dict]:
    """读取 ci/build_config.json 获取 src 侧构建目标"""
    config_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                               'build_config.json')
    _info("SRC 构建配置", config_path)
    with open(config_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    result = []
    for item in data:
        build_defs = []
        if item.get('buildDef', ''):
            build_defs = [d.strip() for d in item['buildDef'].split(',') if d.strip()]

        entry = {
            'file_path': config_path.replace('\\', '/'),
            'build_target': item.get('buildTarget', ''),
            'sample_name': item.get('relativePath', '').replace('/', '-'),
            'platform': item.get('chip', ''),
            'build_defs': build_defs,
        }
        result.append(entry)

    _info("SRC 构建条目数", len(result))
    for e in result:
        _line(f"target={e['build_target']}, name={e['sample_name']}, "
              f"platform={e['platform']}, defs={e['build_defs']}")
    return result


def _is_src_entry(entry: dict) -> bool:
    """判断是否为 src 侧构建条目（非 vendor sample）"""
    return '/ci/build_config.json' in entry.get('file_path', '')


def sample_build_prepare_one(entry: dict) -> None:
    """
    为单个 sample 做编译前准备:
      1. 应用构建宏 (menuconfig / errcode.h)
      2. 复制 sample 源码到 src/application/samples/
      3. 在 CMakeLists.txt 中添加 add_subdirectory_if_exist
    """
    global global_combined

    file_path = entry['file_path']
    build_target = entry['build_target']
    sample_name = entry['sample_name']
    platform = entry['platform']
    build_defs = entry.get('build_defs', [])

    is_src = _is_src_entry(entry)
    if is_src:
        _step(f"准备编译 [SRC] {sample_name}")
        source = entry.get('description', '')
        if source:
            _info("说明", source)
    else:
        vendor_name = file_path.split('/')[2]
        _step(f"准备编译 [{vendor_name}] {sample_name}")

    # 生成 global_combined 标识
    global_combined = _build_artifact_name(build_target, sample_name, platform, build_defs)
    _info("构建标识", global_combined)

    # 应用构建宏
    if build_defs:
        _info("构建宏", build_defs)
        _apply_build_defs(build_defs)
    else:
        _info("构建宏", "(无)")

    if is_src:
        # src 侧构建无需复制 sample 源码和修改 CMakeLists
        return

    # 复制 sample 源码
    target_string = file_path.rsplit('/build_config.json', 1)[0] + '/'
    samples = sample_name.replace('-', '/')
    source_directory = target_string + samples
    sample_basename = os.path.basename(source_directory)
    dest_path = os.path.join(SAMPLE_DIRECTORY, sample_basename)

    _info("源码目录", source_directory)
    _info("目标目录", dest_path)

    try:
        if os.path.exists(dest_path):
            shutil.rmtree(dest_path)
        shutil.copytree(source_directory, dest_path)
    except shutil.Error as e:
        _warn(f"复制源码出错: {e}")
    except OSError as e:
        _warn(f"复制源码出错: {e.strerror}")

    # 添加到 CMakeLists.txt
    sample_dir_name = sample_name.split('-')[-1]
    delete_specific_content(SAMPLE_CMAKELISTS_FILE,
                            f'add_subdirectory_if_exist({sample_dir_name})')
    insert_content_before_line(SAMPLE_CMAKELISTS_FILE, CMAKE_SEARCH_STRING,
                               f'add_subdirectory_if_exist({sample_dir_name})\n')
    _info("CMakeLists", f"已添加 add_subdirectory_if_exist({sample_dir_name})")


def sample_build_cleanup_one(entry: dict) -> None:
    """
    单个 sample 编译后清理:
      1. 删除 sample 源码目录
      2. 从 CMakeLists.txt 中移除 add_subdirectory_if_exist
      3. 移除构建宏
    """
    sample_name = entry['sample_name']
    build_defs = entry.get('build_defs', [])
    is_src = _is_src_entry(entry)

    if is_src:
        _step(f"清理 [SRC] {sample_name}")
    else:
        vendor_name = entry['file_path'].split('/')[2]
        _step(f"清理 [{vendor_name}] {sample_name}")

    if is_src:
        # src 侧构建无需清理 sample 目录和 CMakeLists
        if build_defs:
            _remove_build_defs(build_defs)
            _line(f"已移除构建宏: {build_defs}")
        return

    sample_dir_name = sample_name.split('-')[-1]

    # 删除 sample 目录
    try:
        sample_path = os.path.join(SAMPLE_DIRECTORY, sample_dir_name)
        if os.path.exists(sample_path):
            shutil.rmtree(sample_path)
            _line(f"已删除目录: {sample_path}")
    except OSError as e:
        _warn(f"删除目录失败 {sample_dir_name}: {e.strerror}")

    # 从 CMakeLists.txt 中移除
    delete_specific_content(SAMPLE_CMAKELISTS_FILE,
                            f'add_subdirectory_if_exist({sample_dir_name})')
    _line(f"已从 CMakeLists.txt 移除 add_subdirectory_if_exist({sample_dir_name})")

    # 移除构建宏
    if build_defs:
        _remove_build_defs(build_defs)
        _line(f"已移除构建宏: {build_defs}")


# ============================================================
# 编译
# ============================================================

def compile_sdk_and_save_log(build_target_name: str) -> None:
    """
    在 src/ 目录下执行编译，将日志保存到 archives/，
    构建产物 .fwpkg 移动到 archives/。
    """
    _phase("执行编译")
    _info("构建目标", build_target_name)
    _info("构建标识", global_combined)
    _info("工作目录", os.path.abspath(BUILD_DIRECTORY))

    start_time = time.time()

    # 确保 archives 目录存在
    if not os.path.exists("./archives"):
        os.mkdir("./archives")

    os.chdir(BUILD_DIRECTORY)
    log_path = os.path.join('..', 'archives', f'build-{global_combined}.log')
    _info("日志文件", os.path.abspath(log_path))

    writer = None
    stdout_pipe = None
    output_thread = None
    process = None

    args = ['-c', build_target_name]

    def _mirror_build_output(stream, log_file) -> None:
        while True:
            line = stream.readline()
            if line == b'':
                break

            log_file.write(line)
            log_file.flush()

            try:
                outs = line.decode('utf-8', errors='strict')
            except UnicodeDecodeError:
                outs = line.decode('gbk', errors='replace')
            sys.stdout.write(outs)
            sys.stdout.flush()

    try:
        writer = os.fdopen(os.open(
            log_path,
            os.O_WRONLY | os.O_CREAT | os.O_TRUNC,
            stat.S_IWUSR | stat.S_IRUSR,
        ), 'wb')
        os.chmod(log_path, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)

        process = subprocess.Popen(
            [sys.executable, '-u', BUILD_SCRIPT] + args,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        stdout_pipe = process.stdout
        if stdout_pipe is None:
            raise Exception("无法获取构建输出")

        output_thread = threading.Thread(
            target=_mirror_build_output,
            args=(stdout_pipe, writer),
            daemon=True,
        )
        output_thread.start()
        process.wait(timeout=DEFAULT_BUILD_TIMEOUT)
        output_thread.join()

        elapsed = time.time() - start_time
        _info("构建耗时", f"{elapsed:.1f}s")

        if process.returncode == 0:
            writer.write(b"Finished: SUCCESS")
            _ok(f"编译成功, 日志已保存至: {log_path}")
        else:
            writer.write(b"Finished: FAILURE")
            _fail(f"编译失败! 请检查日志: {log_path}")

    except subprocess.TimeoutExpired:
        if process is not None and process.poll() is None:
            process.kill()
            process.wait()
        if output_thread is not None:
            output_thread.join()
        _fail("构建超时")
    except Exception as e:
        if process is not None and process.poll() is None:
            process.kill()
            process.wait()
        if output_thread is not None:
            output_thread.join()
        _fail(f"编译异常: {str(e)}")
    finally:
        if stdout_pipe is not None:
            stdout_pipe.close()
        if writer:
            writer.close()

    move_file(OUTPUT_FWPKG_PATH, global_combined)


# ============================================================
# 主入口
# ============================================================

def main() -> None:
    """
    门禁主流程:
      1. git diff 获取变更文件，分析受影响范围
      2. 仅 src/ 变更 → 全量编译 SDK
      3. 有 vendor/ 变更（不论是否有 src 变更）→ 匹配 build_config，逐个编译 sample
    """
    global global_combined

    # -------- 阶段1: 分析变更 --------
    _phase("阶段1: 分析变更")

    _info("当前分支", _git_branch_name())
    _info("对比基线", "origin/master")

    check_list = compare_with_remote_master()

    if check_list is None:
        _ok("无 C/H 文件变更, 门禁通过 (无需编译)")
        sys.exit(0)

    # -------- 阶段2: 决策 & 执行 --------

    if has_src_changes and not check_list:
        # 仅 src/ 变更 → 从 ci/build_config.json 读取构建目标，全量编译 SDK
        _phase("阶段2: 全量编译 SDK (仅 src/ 变更)")
        src_entries = _load_src_build_info()
        for entry in src_entries:
            sample_build_prepare_one(entry)
            compile_sdk_and_save_log(entry['build_target'])
            current_dir = os.getcwd()
            parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
            os.chdir(parent_dir)
            sample_build_cleanup_one(entry)

    else:
        # vendor 有变更 → 匹配 build_config，逐个编译 sample
        _phase("阶段2: 匹配构建配置")
        entries = process_build_info_files()
        matched = extract_exact_match(entries, check_list)

        _phase("阶段3: 逐项编译 sample")
        total = len(matched)
        for idx, entry in enumerate(matched, 1):
            _info(f"[{idx}/{total}] 开始处理", f"{entry['file_path'].split('/')[2]} / {entry['sample_name']}")
            sample_build_prepare_one(entry)
            compile_sdk_and_save_log(entry['build_target'])
            # compile_sdk_and_save_log 内会 chdir 到 src/，回到项目根目录
            current_dir = os.getcwd()
            parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
            os.chdir(parent_dir)
            sample_build_cleanup_one(entry)

    # -------- 阶段4: 结果汇总 --------
    _phase("门禁完成")
    _ok("所有编译步骤已执行完毕")

    # 汇总产物
    if os.path.exists(ARCHIVE_DIRECTORY):
        artifacts = os.listdir(ARCHIVE_DIRECTORY)
        if artifacts:
            _info("产物目录", os.path.abspath(ARCHIVE_DIRECTORY))
            for a in artifacts:
                _line(a)


if __name__ == '__main__':
    sys.exit(main())
