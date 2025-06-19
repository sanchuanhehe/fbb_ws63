# -*- coding: utf-8 -*-
import os


current_dir = os.path.dirname(__file__)

index_filename = 'index.rst'


top_dirs = {
    'software': '软件资料'
}


template = """{title}
{symbol}

.. toctree::
   :maxdepth: 2

{doc_tree}
"""


def write_file(filepath, content):
    if os.path.isfile(filepath):
        os.remove(filepath)
    with open(filepath, 'w', encoding='utf-8') as fw:
        fw.write(content)


def find_md_name(file_list):
    for f in file_list:
        if f.endswith('.md'):
            return f.replace('.md', '')
    return ''


def main():
    for language in ('en', 'zh-CN'):
        language_dir = os.path.join(current_dir, language)
        if not os.path.isdir(language_dir):
            continue
        top_index = os.path.join(language_dir, index_filename)
        title = 'WS63指导资料'
        symbol = len(title) * 2 * '='
        lines = [f'   {key}/index\n' for key in top_dirs]
        doc_tree = ''.join(lines)
        top_index_content = template.format(
            title=title, symbol=symbol, doc_tree=doc_tree
        )
        write_file(top_index, top_index_content)
        for doc_dir, sub_title in top_dirs.items():
            doc_dir_path = os.path.join(language_dir, doc_dir)
            sub_index = os.path.join(doc_dir_path, index_filename)
            sub_symbol = len(sub_title) * 3 * '='
            sub_lines = [f'   {key}/index\n' for key in os.listdir(doc_dir_path) if key != 'index.rst']
            sub_doc_tree = ''.join(sub_lines)
            sub_index_content = template.format(title=sub_title, symbol=sub_symbol, doc_tree=sub_doc_tree)
            write_file(sub_index, sub_index_content)
            for ff in os.listdir(doc_dir_path):
                md_dir = os.path.join(doc_dir_path, ff)
                if os.path.isfile(md_dir):
                    continue
                sub2_title = find_md_name(os.listdir(md_dir))
                if not sub2_title:
                    continue
                sub2_index = os.path.join(md_dir, index_filename)
                sub2_symbol = len(sub2_title) * 3 * '='
                sub2_doc_tree = f'   {sub2_title}'
                sub2_index_content = template.format(title=sub2_title, symbol=sub2_symbol, doc_tree=sub2_doc_tree)
                write_file(sub2_index, sub2_index_content)


if __name__ == '__main__':
    main()