import subprocess
import json
import os
import sys
import time
import stat
import shutil
import hashlib
from typing import List, Dict, Union, Set, Optional

#ws63编译配置
BUILD_INFO_FILENAME = 'build_config.json'
ws63_cmakelist_search_string = 'build_component()'
ws63_compile_cmakelistfile = 'src/application/samples/CMakeLists.txt'
ws63_menuconfig_file = 'src/build/config/target_config/ws63/menuconfig/acore/ws63_liteos_app.config'
# 编译sample的路径
ws63_compile_sample_directory = 'src/application/samples'
# 定义要执行的脚本文件和目录
build_directory_path = 'src'
script_to_execute = 'build.py'
# log复制
ws63_output_source_directory = 'output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg'
ws63_log_image_target_directory = 'archives'
global_combined = ''
ws63_error_h = 'src/include/errcode.h'
ws63_error_search_string = '#define ERRCODE_SUCC                                        0UL'
DEFAULT_BUILD_TIMEOUT = 60 * 5

def get_local_branches() -> List[str]:
    """获取所有本地分支列表"""
    result = subprocess.run(
        ["git", "branch", "--format=%(refname:short)"],
        capture_output=True,
        text=True
    )
    return result.stdout.strip().split("\n") if result.stdout else []

def check_changes_and_get_folders(changed_files: List[str]) -> Optional[Set[str]]:
    """
    检查变更文件并返回受影响的文件夹集合
    返回None表示不需要构建或存在非法修改
    """
    # 检查是否有C/H文件修改
    has_c_or_h_files = any(f.endswith(('.c', '.h')) for f in changed_files)
    
    if not has_c_or_h_files:
        print("Not need build, only non-source files modified")
        return None
    
    # 检查是否修改了src目录或构建脚本
    for file_path in changed_files:
        if '/' in file_path:
            src_folder_name = file_path.split('/')[0]
            if '"src' in src_folder_name or 'src' in src_folder_name:
                print(f"invalid modify, not allow modify src dir and build script")
                sys.exit(0)
            # Check if it's modifying 'build_sample.py' file
        if 'build_sample.py' in file_path:
            print(f"invalid modify, not allow modify build_sample.py")
            sys.exit(0)
    # 提取三级文件夹结构
    changed_folders = set()
    for file_path in changed_files:
        if file_path.endswith(('.c', '.h')):
            parts = file_path.split('/')
            if len(parts) >= 4:  # 确保路径深度足够
                folder_name = '+'.join(parts[1:4])  # 取第2-4级目录
                changed_folders.add(folder_name)
    
    print(f"[Changed folders]: {changed_folders}")
    return changed_folders

def compare_with_remote_master() -> Dict[str, Union[List[str], str, Set[str]]]:
    """
    比较所有本地分支与远程master分支的差异
    返回字典格式: {
        分支名: {
            'files': 差异文件列表,
            'folders': 受影响的文件夹集合(仅C/H文件),
            'error': 错误信息(如果有)
        }
    }
    """
    local_branches = get_local_branches()
    remote_master = "origin/master"
    branch_diff = {}
    
    print(f"Comparing {len(local_branches)} local branches with {remote_master}\n")
    
    for branch in local_branches:
        print(f"🔍 Branch [{branch}] vs {remote_master}:")
        branch_info = {}
        
        # 获取差异文件列表
        diff_result = subprocess.run(
            ["git", "diff", "--name-only", remote_master, branch],
            capture_output=True,
            text=True
        )
        
        if diff_result.returncode != 0:
            error_msg = f"Error: {diff_result.stderr.strip()}"
            print(f"{error_msg}")
            branch_info['error'] = error_msg
            branch_diff[branch] = branch_info
            continue
            
        changed_files = diff_result.stdout.strip().split("\n") if diff_result.stdout else []
        branch_info['files'] = changed_files
        
        if not changed_files:
            print("    Identical to remote master")
        else:
            print(f"{len(changed_files)} changed files:")
            for file in changed_files:
                print(f"- {file}")
            
            # 检查变更并获取文件夹信息
            changed_folders = check_changes_and_get_folders(changed_files)
    return changed_folders


# 获取代码仓所有build_info.json文件内容，并拼接在一起
def process_build_info_files():
    print(f"start process_build_info_files")
    result_list = []
    # 遍历指定目录及其子目录下的所有文件和文件夹
    for root, dirs, files in os.walk("./"):
        for file in files:
            if file == BUILD_INFO_FILENAME:
                file_path = os.path.join(root, file)
                print(file_path)
                # 读取JSON文件内容
                with open(file_path, 'r') as f:
                    try:
                        data = json.load(f)
                        for item in data:
                            # 提取需要的字段值
                            build_target = item.get('buildTarget', '')
                            relative_path = item.get('relativePath', '').replace('/','-')
                            chip_name = item.get('chip', '')
                            # 组合成一个字符串并添加到结果列表
                            if item.get('buildDef', ''):
                                build_def = item.get('buildDef', '')
                                combined_value = f"{file_path}+{build_target}+{relative_path}+{chip_name}+{build_def}"
                            else:
                                combined_value = f"{file_path}+{build_target}+{relative_path}+{chip_name}"
                            result_list.append(combined_value)
                    except json.JSONDecodeError:
                        print(f"Error decoding JSON in file: {file_path}")
                        sys.exit(-1)
    return result_list

# 对比git和json内容，一致则代表提交，不一样代表之前提交
def extract_exact_match(input_list, match_list):
    print(f"start extract_exact_match")
    print(f"[extract_exact_match] input_list: {input_list}")
    print(f"[extract_exact_match] match_list: {match_list}")
    exact_matches = []
    try:
        for string in input_list:
            # 使用 split 方法获取第二个 + 和第三个 + 之间的内容
            parts = string.split('+')
            if len(parts) >= 4:
                sample_company_name = parts[0].split('/')[2]
                sample_name_field = parts[2].replace('-','+')
                combined_string = sample_company_name + "+" + sample_name_field
                if combined_string in match_list:
                    exact_matches.append(string)
        print(f"[extract_exact_match] exact_matches: {exact_matches}")
        if not exact_matches:
            print(f"build-config has not been synchronously updated")
            exit(0)  # 退出并返回非零状
        else:
            print("exact_matche 列表不为空")
            return exact_matches
    except TypeError as e:
        print(f"Error: {e}")
    # 在捕获到异常后，可以选择退出程序
        exit(0)  # 退出并返回非零状

# 编译完成后复制log，镜像等到指定目录
def move_file(source_file, new_filename):
    print(f"start move_file")
    target_file = os.path.join(f'../{ws63_log_image_target_directory}', f'{new_filename}.fwpkg')
    os.chmod(source_file, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    try:
        # 移动并重命名文件
        shutil.move(source_file, target_file)
        os.chmod(target_file, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
        print(f"文件 {source_file} 移动并重命名为 {target_file} 成功。")
    except FileNotFoundError:
        print(f"找不到文件 {source_file}")
    except PermissionError:
        print(f"权限错误，无法移动文件。")
    except Exception as e:
        print(f"发生错误：{str(e)}")

#  编译案例
def compile_sdk_and_save_log(build_target_name):
    print(f"start compile_sdk_and_save_log")
    current_dir = os.getcwd()
    print(f"Current working directory: {current_dir}")
    # 创建目录archives
    if not os.path.exists("./archives"):
        os.mkdir("./archives")
    # 切换到目标目录src
    os.chdir(build_directory_path)
    log_path = os.path.join('..', 'archives', f'build-{global_combined}.log')
    writer = os.fdopen(os.open(
        log_path,
        os.O_WRONLY | os.O_CREAT | os.O_TRUNC,
        stat.S_IWUSR | stat.S_IRUSR,
    ), 'wb')
    reader = os.fdopen(os.open(
        log_path,
        os.O_RDONLY,
        stat.S_IWUSR | stat.S_IRUSR,
    ), 'rb')
    os.chmod(log_path, stat.S_IRWXU | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    args = ['-c', build_target_name]
    try:
        # 打开日志文件准备写入
        # 使用 subprocess.Popen() 执行命令，并将标准输出和标准错误重定向到日志文件
        process = subprocess.Popen(['python', script_to_execute] + args, text=False, stdout=writer, stderr=writer)
        # 等待进程结束
        start = time.time()
        while True:
            timeout = (time.time() - start) > DEFAULT_BUILD_TIMEOUT

            line = reader.readline()

            if line == b'':
                if process.poll() is not None:
                    break

                time.sleep(2)
                if not timeout:
                    continue
                else:
                    process.kill()
                    raise Exception(f"build exit cause: timeout")

            try:
                outs = line.decode('utf-8', errors='strict').rstrip()
            except UnicodeDecodeError:
                outs = line.decode('gbk', errors='replace').rstrip()
            if not outs:
                if not timeout:
                    continue
                else:
                    process.kill()
                    raise Exception(f"build timeout")
            print(outs)
        # 检查进程的返回码
        if process.returncode == 0:
            writer.write(b"Finished: SUCCESS")
            print(f"SDK compilation succeeded. Log saved to '{log_path}'.")
        else:
            print(f"SDK compilation failed. Check '{log_path}' for details.")
            writer.write(b"Finished: FAILURE")
            # 输出标准错误信息到控制台
            print(process.stderr.read().decode('utf-8'))

    except Exception as e:
        print(f"An error occurred: {str(e)}")
    if writer:
        writer.close()
    if reader:
        reader.close()
    move_file(ws63_output_source_directory, global_combined)


# 删除指定cmakelist里面内容
def delete_specific_content(file_path, content_to_delete):
    print(f"start delete_specific_content")
    # 读取文件内容
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # 处理文件内容，删除指定内容
    modified_lines = [line for line in lines if content_to_delete not in line]

    # 将修改后的内容写回文件
    with open(file_path, 'w') as file:
        file.writelines(modified_lines)


# 如果代码编译cmakelist文件中通过宏控制编译哪些文件，则需要用宏重新组合编译配置信息
def sample_meunconfig_modify(input_list):
    print(f"start sample_meunconfig_modify{input_list}")
    # 初始化结果列表
    result_list = []
    combined_string = ""
    for string in input_list:
        parts = string.split('+')
        sample_file_path = parts[0]
        build_target = parts[1]
        sample_name = parts[2]
        platform = parts[3]
        if len(parts) == 5:
            def_name = parts[4]
            # 根据逗号分割字符串
            def_parts = def_name.split(',')
            # 遍历分割后的每一个部分，添加到结果列表中,宏的个数，决定编译的次数
            if len(def_parts) == 1 and def_parts[0] == def_name:
                result_list.append(sample_file_path+"+"+build_target+"+"+sample_name+"+"+platform+"+"+def_name)
            else:
                for idx, part in enumerate(def_parts):
                    if idx == 0:
                        combined_string = part
                    else:
                        combined_string += "+" + part  # 在每个部分之前添加加号
                result_list.append(sample_file_path+"+"+build_target+"+"+sample_name+"+"+platform+"+"+combined_string)
        # 如果没有逗号，则整个字符串保存到结果列表
        elif len(parts) == 4:
            result_list.append(sample_file_path+"+"+build_target+"+"+sample_name+"+"+platform)
    return result_list

def insert_content_before_line(file_path, target_line, content_to_insert):
    print(f"start insert_content_before_line")
    found_target_line = False
    try:
        # 以读写模式打开文件
        with open(file_path, 'r') as file:
            lines = file.readlines()
            # 查找目标字符串并在其上一行添加列表中第一个内容
            for i in range(len(lines)):
                if target_line in lines[i]:
                    lines.insert(i, content_to_insert)
                    found_target_line = True
                    print(f"成功在文件 {file_path} 中的目标行 {target_line} 的上一行插入内容{content_to_insert}。")
                    break  # 找到目标字符串后添加内容并退出循环

            # 如果没找到目标字符串则直接在该文件最后一行加入内容
            if not found_target_line:
                lines.append(content_to_insert)
                print(f"成功在文件 {file_path} 中的最后一行插入内容{content_to_insert}。")

            with open(file_path, 'w') as file:
                file.writelines(lines)
    except FileNotFoundError:
        print(f"文件 {file_path} 未找到。")
        

# 定义一个自定义的复制函数
def custom_copytree(src, dst):
    # 拼接目标路径
    dst_path = os.path.join(dst, os.path.basename(src))

    # 如果目标路径存在，则先删除目标路径
    if os.path.exists(dst_path):
        shutil.rmtree(dst_path)
    
    # 执行复制操作
    shutil.copytree(src, dst_path)

# sample编译前需要添加到编译工具链
def sample_build_prepare(input_list):
    print(f"start sample_build_prepare")
    print(f"[sample_build_prepare] input_list: {input_list}")
    global global_combined
    for string in input_list:
        parts = string.split('+')
        sample_file_path = parts[0]
        build_target = parts[1]
        sample_name = parts[2]
        platfor_name = parts[3]
        if len(parts) == 5:
            def_name = parts[4]
            if '=y' in def_name or '= y' in def_name:
                insert_content_before_line(ws63_menuconfig_file, ws63_cmakelist_search_string, f'{def_name}\n')
            else:
                def_name_cleaned = def_name.replace('=', ' ')
                insert_content_before_line(ws63_error_h, ws63_error_search_string, f'#define {def_name_cleaned}\n')
        elif len(parts) > 5:
             for i in range(len(parts) - 4):
                def_name = parts[4 + i]
                if '=y' in def_name or '= y' in def_name:
                    insert_content_before_line(ws63_menuconfig_file, ws63_cmakelist_search_string, f'{def_name}\n')
                else:
                    def_name_cleaned = def_name.replace('=', ' ')
                    insert_content_before_line(ws63_error_h, ws63_error_search_string, f'#define {def_name_cleaned}\n')
        remaining_parts = parts[1:4]
        global_combined = '_'.join(remaining_parts)
        if len(parts) >= 5:
            global_combined += '_'
            build_def_sha = hashlib.sha256('-'.join(parts[4:]).encode('utf-8',errors='ignore'))
            global_combined += build_def_sha.hexdigest()[:32]
        # 获取sample目录路径，并复制到指定目录
        # 截取目标内容
        target_string = sample_file_path.split('/build_config.json')[0] + '/'
        samples = sample_name.replace('-','/')
        source_directory = target_string + samples
        print(source_directory)
        print(f"[sample_build_prepare] source_directory: {source_directory}")
        try:
            if os.path.exists(ws63_compile_sample_directory + '/' + sample_name.split("-")[1]):
                shutil.rmtree(ws63_compile_sample_directory + '/' + sample_name.split("-")[1])
            # 使用shutil.copytree()函数复制整个目录树
            shutil.copytree(source_directory, os.path.join(ws63_compile_sample_directory, os.path.basename(source_directory)))
            print(f"Directory '{source_directory}' successfully copied to '{ws63_compile_sample_directory}'")
        except shutil.Error as e:
            print(f"Error: {e}")
        except OSError as e:
            print(f"Error: {e.strerror}")
        delete_specific_content(ws63_compile_cmakelistfile, f'add_subdirectory_if_exist({sample_name.split("-")[1]})')
        insert_content_before_line(ws63_compile_cmakelistfile, ws63_cmakelist_search_string, f'add_subdirectory_if_exist({sample_name.split("-")[1]})\n')
        compile_sdk_and_save_log(build_target)
        current_dir = os.getcwd()
        # 切换到上一层目录
        parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
        os.chdir(parent_dir)
        try:
            shutil.rmtree(ws63_compile_sample_directory + '/' + sample_name.split("-")[1])
            print(f"Directory {ws63_compile_sample_directory + '/' + sample_name} successfully removed recursively.")
        except OSError as e:
            print(f"Error: {ws63_compile_sample_directory} : {e.strerror}")
        delete_specific_content(ws63_compile_cmakelistfile, f'add_subdirectory_if_exist({sample_name.split("-")[1]})')
        if len(parts) == 5:
            def_name = parts[4]
            if '=y' in def_name or '= y' in def_name:
                delete_specific_content(ws63_menuconfig_file, f'{def_name}')
            else:
                def_name_cleaned = def_name.replace('=', ' ')
                delete_specific_content(ws63_error_h, f'#define {def_name_cleaned}')
        elif len(parts) > 5:
             for i in range(len(parts) - 4):
                def_name = parts[4 + i]
                if '=y' in def_name or '= y' in def_name:
                    delete_specific_content(ws63_menuconfig_file, f'{def_name}')
                else:
                    def_name_cleaned = def_name.replace('=', ' ')
                    delete_specific_content(ws63_error_h, f'#define {def_name_cleaned}')

def main():
    print(f"start main")
    check_list = get_changed_folders_in_vendor()
    input_list = process_build_info_files()
    sample_name = extract_exact_match(input_list, check_list)
    result_list = sample_meunconfig_modify(sample_name)
    sample_build_prepare(result_list)
    print(f"all build step execute end")

if __name__ == '__main__':
    sys.exit(main())