# -*- coding: utf-8 -*-
import logging
import os
import posixpath
import re
import shutil
import time
import urllib.parse

from zipfile import ZipFile

import jieba3

from sphinx.search import SearchLanguage

logging.basicConfig(level=logging.INFO)

# 构建执行时声明的环境变量
use_doc_reading = os.getenv('USE_DOC_READING', 'yes')  # 文档捉虫
repo_namespace = os.getenv('REPO_NAMESPACE', 'HiSpark')  # 仓命名空间
repo_path = os.getenv('REPO_PATH')  # 仓路径
repo_host = os.getenv('REPO_HOST', 'gitee.com')  # 仓域名
repo_branch = os.getenv('REPO_BRANCH', 'master')  # 构建分支
cur_path = os.path.dirname(__file__)

# 流水线配置
url_root_prefix = os.getenv('OBS_ROOT_KEY', 'repos')  # OBS根路径
gitee_view_mode = os.getenv('GITEE_VIEW_MODE', 'blob')

# sphinx变量---------------------
project = 'HiSpark编程指南'
author = repo_namespace
release = repo_branch

extensions = [
    'myst_parser',
    'sphinx_rtd_theme',
    'sphinx_copybutton'
]
templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'zh_CN'

html_favicon = '../_static/img/favicon.ico'
html_static_path = ['../_static']
html_theme = "sphinx_rtd_theme"
html_theme_options = {
    'logo_only': False,
    'prev_next_buttons_location': 'bottom',
}

myst_enable_extensions = ["colon_fence"]
source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown'
}

html_css_files = [
    'css/custom.css',
]

html_context = {
    # 自定义模板变量1
    'repo_host': repo_host,
    'repo_namespace': repo_namespace,
    'repo_path': repo_path,
    'gitee_pageview_mode': gitee_view_mode,
    'repo_branch': repo_branch,
    'versions_url': f'/{url_root_prefix.strip("/")}/version_ctrl/hispark_version.js',
    'url_root_prefix': url_root_prefix,
    'repo_type': 'gitee' if 'gitee' in repo_host else 'inner',
    'use_doc_reading': use_doc_reading,

    # sphinx模板变量
    'logo_url': 'img/hisilicon.svg',
    'show_copyright': False,
    'show_sphinx': False,
}


a_tag_re_matcher = re.compile(r'<a name=[\\]?"[^"]+[\\]?"></a>')
img_tag_re_matcher = re.compile(r'<img\b[^>]{0,200}src="([^"]+)"[^>]{0,200}>')
md_link_matcher = re.compile(r'\[[^\]]+\]\([^\)]+\.md[^\)]{0,200}\)|<a href="[^>]+\.md">[^<]+</a>')


# 适配处理相关函数----------------------
def builder_inited(app):
    if app.builder.name == 'latex':
        return

    # 构建前追加jinjia2模板变量
    html_context[
        'zip_filename'] = f'{repo_path}-{app.config.language}-{release}.zip'

    # 追加模板变量过滤器
    env = app.builder.templates.environment
    env.filters['toctree_handler'] = lambda content: a_tag_re_matcher.sub('', content)


def remove_md_href(content):
    for md_link in md_link_matcher.findall(content):
        if md_link.startswith('<a'):
            text = md_link.split('>')[1].split('<')[0]
        else:
            text = md_link.split(']')[0].lstrip('[')
        content = content.replace(md_link, text)
    return content


def handle_zh_images(app, page_name, template_name, context, doctree):
    """适配中文名称的图片"""
    if 'body' not in context or not isinstance(context['body'], str):
        return
    body_lines = context['body'].splitlines()
    # imagedir: 构建的图片存放路径
    # imgpath: img标签请求路由前缀
    out_dir_img_path = os.path.join(app.outdir, app.builder.imagedir)
    if not os.path.isdir(out_dir_img_path):
        os.makedirs(out_dir_img_path, mode=0o700, exist_ok=True)
    body_change = False
    for index, html_line in enumerate(body_lines):
        temp_line = html_line
        for img_src_val in img_tag_re_matcher.findall(html_line):

            if img_src_val.startswith((app.builder.imgpath, 'http')):
                continue
            # 解决sphinx无法正确处理存在中文的图片名称
            unquote_path = urllib.parse.unquote(img_src_val)  # 恢复中文
            relative_path, filename = os.path.split(unquote_path)

            if relative_path.startswith('figures'):
                img_relative_path = os.path.join(
                    os.path.split(page_name)[0], relative_path)
            else:
                img_relative_path = relative_path

            src_img_file = os.path.join(app.builder.srcdir, img_relative_path, filename)
            dst_file_path = os.path.join(out_dir_img_path, filename)
            if not os.path.isfile(dst_file_path):
                shutil.copy(src_img_file, dst_file_path)
            # 仅图片名称无路径表示图片与conf.py同级目录
            if not relative_path:
                temp_line = temp_line.replace(
                    img_src_val,
                    posixpath.join(app.builder.imgpath, img_src_val))
            else:
                posix_img_path = posixpath.join(app.builder.imgpath,
                                                urllib.parse.quote(filename))
                temp_line = temp_line.replace(img_src_val, posix_img_path)
            body_change = True
        if body_change:
            body_lines[index] = temp_line
    if body_change:
        context['body'] = '\n'.join(body_lines)


def after_build(app, exception):
    """去除菜单栏点击滚动事件"""
    static_js_path = os.path.join(app.outdir, '_static', 'js')
    if not os.path.isdir(static_js_path):
        return
    theme_js_path = os.path.join(static_js_path, 'theme.js')
    if not os.path.isfile(theme_js_path):
        return
    with open(theme_js_path, encoding='utf-8') as fr:
        origin_js_content = fr.read()
    with open(theme_js_path, 'w', encoding='utf-8') as fw:
        fw.write(origin_js_content.replace(
            't[0].scrollIntoView()', '')
        )

    # 清除搜索索引标题中的a标签
    search_index_js_path = os.path.join(app.outdir, 'searchindex.js')
    if os.path.isfile(search_index_js_path):
        with open(search_index_js_path, encoding='utf-8') as fr:
            search_index_js_content = fr.read()
        with open(search_index_js_path, 'w', encoding='utf-8') as fw:
            fw.write(
                a_tag_re_matcher.sub('', search_index_js_content)
            )

    # 打包构建产物
    start_time = time.time()
    filepath = os.path.join(cur_path, html_context['zip_filename'])
    if os.path.isfile(filepath):
        os.remove(filepath)
    logging.info('pack zip file starting: %s', filepath)
    with ZipFile(filepath, mode='w') as zf:
        for root, _, files in os.walk(app.outdir):
            logging.info('packing dir path: %s, files: %s', root, len(files))
            for fn in files:
                real_path = os.path.join(root, fn)
                arc_name = os.path.relpath(real_path, start=app.outdir)
                zf.write(real_path, arcname=arc_name)
    end_time = time.time()
    shutil.move(filepath, os.path.join(app.outdir, html_context['zip_filename']))
    logging.info('pack html zip take seconds: %s', end_time - start_time)


def source_read_handler(app, docname, content):
    for index, text in enumerate(content):
        content[index] = remove_md_href(text)


class ChineseSearch(SearchLanguage):
    lang = 'zh'
    jieba = jieba3.jieba3()

    def split(self, input_content):
        return list(set(self.jieba.cut_text(input_content)))  # 搜索引擎模式分词


def setup(app):
    app.connect('builder-inited', builder_inited)
    app.connect('source-read', source_read_handler)
    app.connect('html-page-context', handle_zh_images)
    app.connect('build-finished', after_build)
    app.add_search_language(ChineseSearch)


# latex构建相关
latex_table_style = ['standard', 'nocolorrows']
latex_elements = {
    'papersise': 'a4paper',
    'sphinxsetup': 'verbatimforcewraps=true',  # 代码块内容强制换行
    'figure_align': 'H',  # 禁用图片浮动
    'preamble': r"""
    \usepackage{longtable} % 长表格包
    \usepackage{array}
    \setcounter{tocdetph}{2}  % 目录层级
    \setcounter{secnumdepth}{6}  % 章节标题层级
    """
}

