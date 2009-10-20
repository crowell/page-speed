# Copyright 2009 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'includes': [
    '../third_party/WebKit/WebCore/WebCore.gypi',
  ],
  'variables': {
    'package_root': '..'
  },
  'targets': [
    {
      'target_name': 'pagespeed_webkit',
      'type': '<(library)',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/third_party/WebKit/WebCore/WebCore.gyp/WebCore.gyp:webcore',
        '<(DEPTH)/third_party/pagespeed/src/pagespeed/pagespeed.gyp:pagespeed',
        '<(DEPTH)/third_party/pagespeed/src/pagespeed/pagespeed.gyp:pagespeed_output_pb',
      ],
      'sources': [
        '<(package_root)/pagespeed_webkit/remove_unused_css.cc',
        '<(package_root)/pagespeed_webkit/webkit_css_style_info.cc',
        '<(package_root)/pagespeed_webkit/webkit_rule_provider.cc',
      ],
      'include_dirs': [
        '<(package_root)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(package_root)',
        ],
      },
      'export_dependent_settings': [
        '<(DEPTH)/base/base.gyp:base',
      ]
    },
  ],
}
