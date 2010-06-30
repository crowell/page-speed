// Copyright 2009 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>

#include "pagespeed/cssmin/cssmin.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char* kBeforeMinification =
    "/* This is a CSS file.  Hooray. */\n"
    "\n"
    "BODY {\n"
    "  border: 5px solid blue;\n"
    "  color: red /*two comments*/ /*in a row*/;\n"
    "}\n"
    "\n"
    "DIV.bg1  {\n"
    "  background-image : url( 'www.example.com/bg1.png' ) ;\n"
    " } \n"
    "DIV.bg2 {\n"
    "  background-image : url(\"www.example.com/bg2.png\");  \n"
    "}\n";

const char* kAfterMinification =
    "BODY{border:5px solid blue;color:red;}\n"
    "DIV.bg1{background-image:url('www.example.com/bg1.png');}\n"
    "DIV.bg2{background-image:url(\"www.example.com/bg2.png\");}\n";


class CssminTest : public testing::Test {
 protected:
  void CheckMinification(const std::string& before, const std::string& after) {
    std::string output;
    ASSERT_TRUE(pagespeed::cssmin::MinifyCss(before, &output));
    ASSERT_EQ(after, output);

    int minified_size = -1;
    ASSERT_TRUE(pagespeed::cssmin::GetMinifiedCssSize(before, &minified_size));
    ASSERT_EQ(static_cast<int>(after.size()), minified_size);
  }
};

TEST_F(CssminTest, Basic) {
  CheckMinification(kBeforeMinification, kAfterMinification);
}

TEST_F(CssminTest, AlreadyMinified) {
  CheckMinification(kAfterMinification, kAfterMinification);
}

TEST_F(CssminTest, RunawayComment) {
  CheckMinification("BODY { color: red; } /* unclosed comment...*",
                    "BODY{color:red;}\n");
}

TEST_F(CssminTest, RunawayString) {
  CheckMinification("DIV { background-image: url('ain\\'t   no  /*end*/ quote",
                    "DIV{background-image:url('ain\\'t   no  /*end*/ quote");
}

}  // namespace
