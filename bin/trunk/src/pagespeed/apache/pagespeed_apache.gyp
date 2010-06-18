# Copyright 2010 Google Inc.
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
  'targets': [
    {
      'target_name': 'mod_pagespeed',
      'type': 'loadable_module',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/pagespeed/pagespeed_bin.gyp:pagespeed_chromium_lib',
        '<(DEPTH)/third_party/apache/httpd/httpd.gyp:include',
        '<(DEPTH)/third_party/libpagespeed/src/pagespeed/pagespeed.gyp:pagespeed',
        '<(DEPTH)/third_party/libpagespeed/src/pagespeed/formatters/formatters.gyp:pagespeed_formatters',
        '<(DEPTH)/third_party/libpagespeed/src/pagespeed/image_compression/image_compression.gyp:pagespeed_image_attributes_factory',
      ],
      'include_dirs': [
        '<(DEPTH)',
      ],
      'sources': [
        'mod_pagespeed.cc',
      ],
      'conditions': [['OS == "mac"', {
        'xcode_settings': {
          # We must null out these two variables when building this target,
          # because it is a loadable_module (-bundle).
          'DYLIB_COMPATIBILITY_VERSION':'',
          'DYLIB_CURRENT_VERSION':'',
        }
      }]],
    },
  ],
}
