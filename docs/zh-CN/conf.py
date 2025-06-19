# -*- coding: utf-8 -*-
try:
    from common_conf import *
except ImportError:
    import os
    import sys
    sys.path.insert(0, os.path.abspath('..'))
    from common_conf import *


html_search_language = 'zh'
html_search_options = {
    'type': 'json',
    'lang': 'zh',
    'tokenizer': {
        'zh': lambda s: [c for c in s]  # 按字符索引中文
    }
}
