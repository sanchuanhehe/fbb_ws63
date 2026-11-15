# ci_gate.py —— CI 门禁脚本

## 核心逻辑

`check_changes_and_get_folders()` 分析 git diff 产生的变更文件列表, 采用黑名单模式过滤文件（仅排除文档、图片、git配置等明确不影响构建的文件）, 场景矩阵:

| 场景                               | has_src_changes | 返回的 set          |
|------------------------------------|-----------------|---------------------|
| 仅 vendor/ 下构建相关文件变更       | False           | 非空 (vendor keys)  |
| 仅 src/    下构建相关文件变更       | True            | 空 set              |
| vendor/ + src/ 同时变更            | True            | 非空 (vendor keys)  |
| 无构建相关文件变更                  | False           | None                |
| ci/ci_gate.py 自身变更             | True            | 空 set              |

`main()` 根据返回决定具体行为:

1. **返回 None** — 无构建相关文件变更 (仅变更了文档/图片/git配置等黑名单文件), 直接跳过。

2. **返回空 set 且 has_src_changes == True** — 仅 src/ 变更 (或 ci_gate.py 自身变更), 直接全量编译整个 SDK:
   ```
   python build.py -c ws63-liteos-app
   ```
   编译日志输出到 `archives/build-ws63-liteos-app.log`。

3. **返回非空 set** (无论 has_src_changes 是否为 True) — vendor/ 下有变更, 逐个编译受影响的 sample:
   - `process_build_info_files()` 解析所有 `vendor/*/build_config.json`
   - `extract_exact_match()` 将变更的 vendor key 与 JSON 条目精确匹配
   - 对每条匹配到的 entry 循环: prepare → compile → cleanup

## ci_gate.py 自身变更的特殊处理

当 `ci/ci_gate.py` 被修改时, 先运行自身测试用例再编译:

```
python ci/test_ci_gate.py
```

全部 24 个测试用例通过后, 才允许继续编译。任何一个测试失败都会直接拦截。

## 文件过滤策略（黑名单模式）

采用黑名单模式判断文件是否为"构建相关":

- **黑名单（明确跳过）**: `.md`, `.rst`, `.png`, `.jpg`, `.jpeg`, `.gif`, `.svg`, `.ico`, `.bmp`, `.gitignore`, `.gitattributes`, `.gitmodules`, `README`, `CHANGELOG`, `LICENSE`, `NOTICE`
- **其余所有文件** — 包括 `.c`, `.h`, `.s`, `.S`, `.cpp`, `CMakeLists.txt`, `Kconfig`, `Makefile`, `.py`, `.cmake`, `.mk`, `.lds`, `.config`, `.cfg`, `.json`, `.xml`, `.sh`, `.a`, `.bin` 等 — 均视为可能影响构建，变更即触发编译。

## 运行方式

- 本地测试: `python ci/test_ci_gate.py`
- CI 执行:   `python ci/ci_gate.py`

## 数据流

```
git diff origin/master...HEAD
    │
    ▼
compare_with_remote_master()
    │
    ▼
check_changes_and_get_folders()
    │
    ├── 仅 src 变更  ──→ compile_sdk_and_save_log('ws63-liteos-app')
    │
    └── vendor 变更  ──→ process_build_info_files()
                              │
                              ▼
                        extract_exact_match()
                              │
                              ▼
                        for each matched entry:
                            sample_build_prepare_one()
                            compile_sdk_and_save_log()
                            sample_build_cleanup_one()
```

## 关键文件路径

| 文件 | 路径 |
|------|------|
| 本脚本 | `ci/ci_gate.py` |
| 测试用例 | `ci/test_ci_gate.py` |
| 构建入口 | `src/build.py` |
| menuconfig | `src/build/config/target_config/ws63/menuconfig/acore/ws63_liteos_app.config` |
| errcode.h | `src/include/errcode.h` |
| samples CMakeLists | `src/application/samples/CMakeLists.txt` |
| 构建日志/产物 | `archives/` |
