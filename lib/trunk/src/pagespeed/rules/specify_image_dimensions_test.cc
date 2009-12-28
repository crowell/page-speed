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

#include "base/scoped_ptr.h"
#include "base/stl_util-inl.h"  // for STLDeleteContainerPointers
#include "pagespeed/core/dom.h"
#include "pagespeed/core/pagespeed_input.h"
#include "pagespeed/proto/pagespeed_output.pb.h"
#include "pagespeed/rules/specify_image_dimensions.h"
#include "testing/gtest/include/gtest/gtest.h"

using pagespeed::rules::SpecifyImageDimensions;
using pagespeed::PagespeedInput;
using pagespeed::Result;
using pagespeed::Results;

namespace {

class MockDocument : public pagespeed::DomDocument {
 public:
  MockDocument() {}
  virtual ~MockDocument() {
    STLDeleteContainerPointers(elements_.begin(), elements_.end());
  }

  virtual void Traverse(pagespeed::DomElementVisitor* visitor) const {
    for (std::vector<pagespeed::DomElement*>::const_iterator
             iter = elements_.begin(),
             end = elements_.end();
         iter != end;
         ++iter) {
      visitor->Visit(**iter);
    }
  }

  void AddElement(pagespeed::DomElement* element) {
    elements_.push_back(element);
  }

 private:
  std::vector<pagespeed::DomElement*> elements_;

  DISALLOW_COPY_AND_ASSIGN(MockDocument);
};

class MockElement : public pagespeed::DomElement {
 public:
  MockElement(pagespeed::DomDocument* content,
              const std::string& tagname,
              const std::string& resource_url,
              const std::map<std::string, std::string>& attributes,
              const std::map<std::string, std::string>& css_properties)
      : content_(content),
        tagname_(tagname),
        resource_url_(resource_url),
        attributes_(attributes),
        css_properties_(css_properties) {
  }

  virtual pagespeed::DomDocument* GetContentDocument() const {
    return content_;
  }

  virtual std::string GetTagName() const {
    return tagname_;
  }

  virtual bool GetResourceUrl(std::string* src) const {
    if (resource_url_.empty()) return false;
    *src = resource_url_;
    return true;
  }

  virtual bool GetAttributeByName(const std::string& name,
                                  std::string* attr_value) const {
    std::map<std::string, std::string>::const_iterator entry =
        attributes_.find(name);
    if (entry != attributes_.end()) {
      *attr_value = entry->second;
      return true;
    } else {
      return false;
    }
  }

  virtual bool GetCSSPropertyByName(const std::string& name,
                                    std::string* property_value) const {
    std::map<std::string, std::string>::const_iterator entry =
        css_properties_.find(name);
    if (entry != css_properties_.end()) {
      *property_value = entry->second;
      return true;
    } else {
      return false;
    }
  }

 private:
  pagespeed::DomDocument* content_;
  std::string tagname_;
  std::string resource_url_;
  std::map<std::string, std::string> attributes_;
  std::map<std::string, std::string> css_properties_;

  DISALLOW_COPY_AND_ASSIGN(MockElement);
};

class SpecifyImageDimensionsTest : public ::testing::Test {
 protected:
  void CheckNoViolations(MockDocument* document) {
    CheckExpectedViolations(document, std::vector<std::string>());
  }

  void CheckOneViolation(MockDocument* document,
                         const std::string& violation_url) {
    std::vector<std::string> expected;
    expected.push_back(violation_url);
    CheckExpectedViolations(document, expected);
  }

  void CheckTwoViolations(MockDocument* document,
                          const std::string& violation_url1,
                          const std::string& violation_url2) {
    std::vector<std::string> expected;
    expected.push_back(violation_url1);
    expected.push_back(violation_url2);
    CheckExpectedViolations(document, expected);
  }

 private:
  void CheckExpectedViolations(MockDocument* document,
                               const std::vector<std::string>& expected) {
    PagespeedInput input;
    input.AcquireDomDocument(document);

    SpecifyImageDimensions dimensions_rule;

    Results results;
    ASSERT_TRUE(dimensions_rule.AppendResults(input, &results));
    ASSERT_EQ(results.results_size(), expected.size());

    for (int idx = 0; idx < expected.size(); ++idx) {
      const Result& result = results.results(idx);
      ASSERT_EQ(result.resource_urls_size(), 1);
      EXPECT_EQ(expected[idx], result.resource_urls(0));
    }
  }
};

TEST_F(SpecifyImageDimensionsTest, EmptyDom) {
  MockDocument* doc = new MockDocument;
  CheckNoViolations(doc);
}

TEST_F(SpecifyImageDimensionsTest, DimensionsSpecified) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  attributes["width"] = "23";
  attributes["height"] = "42";
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/image.png",
                                  attributes,
                                  css_properties));
  CheckNoViolations(doc);
}

TEST_F(SpecifyImageDimensionsTest, DimensionsSpecifiedInCss) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  css_properties["width"] = "23";
  css_properties["height"] = "42";
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/image.png",
                                  attributes,
                                  css_properties));
  CheckNoViolations(doc);
}

TEST_F(SpecifyImageDimensionsTest, NoHeight) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  attributes["width"] = "23";
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/image.png",
                                  attributes,
                                  css_properties));
  CheckOneViolation(doc, "http://test.com/image.png");
}

TEST_F(SpecifyImageDimensionsTest, NoWidth) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  attributes["height"] = "42";
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/image.png",
                                  attributes,
                                  css_properties));
  CheckOneViolation(doc, "http://test.com/image.png");
}

TEST_F(SpecifyImageDimensionsTest, NoDimensions) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/image.png",
                                  attributes,
                                  css_properties));
  CheckOneViolation(doc, "http://test.com/image.png");
}

// Same test as above, only no resource URL specified. Now we expect
// no violation since a resource URL is required in order to trigger a
// violation.
TEST_F(SpecifyImageDimensionsTest, NoViolationMissingResourceUrl) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "",
                                  attributes,
                                  css_properties));
  CheckNoViolations(doc);
}

TEST_F(SpecifyImageDimensionsTest, NoDimensionsInIFrame) {
  MockDocument* iframe_doc = new MockDocument;

  std::map<std::string, std::string> attributes, css_properties;
  iframe_doc->AddElement(
      new MockElement(NULL,
                      "IMG",
                      "http://test.com/image.png",
                      attributes,
                      css_properties));

  MockDocument* doc = new MockDocument;
  doc->AddElement(
      new MockElement(iframe_doc,
                      "IFRAME",
                      "",
                      std::map<std::string, std::string>(),
                      std::map<std::string, std::string>()));

  CheckOneViolation(doc, "http://test.com/image.png");
}

TEST_F(SpecifyImageDimensionsTest, MultipleViolations) {
  MockDocument* doc = new MockDocument;

  std::map<std::string, std::string> css_properties;
  std::map<std::string, std::string> attributesA;
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/imageA.png",
                                  attributesA,
                                  css_properties));

  std::map<std::string, std::string> attributesB;
  doc->AddElement(new MockElement(NULL,
                                  "IMG",
                                  "http://test.com/imageB.png",
                                  attributesB,
                                  css_properties));
  CheckTwoViolations(doc,
                     "http://test.com/imageA.png",
                     "http://test.com/imageB.png");
}

}  // namespace
