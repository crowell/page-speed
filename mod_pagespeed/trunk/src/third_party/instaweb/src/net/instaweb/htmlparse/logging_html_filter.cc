// Copyright 2010 Google Inc. All Rights Reserved.
// Author: jmaessen@google.com (Jan Maessen)

#include "public/logging_html_filter.h"
#include "public/html_element.h"
#include "public/statistics_log.h"

namespace {
// Printable names of the statistics.
// Must match up with enum Statistic in logging_html_filter.h;
// is this bad for maintenance?
const char* kStatisticNames[] = {
  "explicit_close", "implicit_close", "brief_close", "closed", "unclosed",
  "spurious_close", "tags", "cdata", "comments", "directives", "documents",
  "IE_directives",
};
}

namespace net_instaweb {

LoggingFilter::LoggingFilter() {
  Reset();
}

void LoggingFilter::StartDocument() {
  ++stats_[NUM_DOCUMENTS];
}

void LoggingFilter::EndDocument() {
}

void LoggingFilter::StartElement(HtmlElement* element) {
  // Does EndElement get called for singleton elements?
  ++stats_[NUM_UNCLOSED];
  ++stats_[NUM_TAGS];
}

void LoggingFilter::EndElement(HtmlElement* element) {
  // Figure out what's up with the element (implicitly vs explicitly closed)
  switch (element->close_style()) {
    case HtmlElement::EXPLICIT_CLOSE: {
      --stats_[NUM_UNCLOSED];
      ++stats_[NUM_CLOSED];
      ++stats_[NUM_EXPLICIT_CLOSED];
      break;
    }
    case HtmlElement::IMPLICIT_CLOSE: {
      --stats_[NUM_UNCLOSED];
      ++stats_[NUM_CLOSED];
      ++stats_[NUM_IMPLICIT_CLOSED];
      break;
    }
    case HtmlElement::BRIEF_CLOSE: {
      --stats_[NUM_UNCLOSED];
      ++stats_[NUM_CLOSED];
      ++stats_[NUM_BRIEF_CLOSED];
      break;
    }
    case HtmlElement::UNCLOSED: {
      // We assumed unmatchedness at StartElement, so do nothing.
      break;
    }
    case HtmlElement::AUTO_CLOSE: {
      // Another form of unmated tag, again do nothing.
      break;
    }
  }
}

void LoggingFilter::Cdata(const std::string& cdata) {
  ++stats_[NUM_CDATA];
}

void LoggingFilter::Comment(const std::string& comment) {
  ++stats_[NUM_COMMENTS];
}

void LoggingFilter::IEDirective(const std::string& directive) {
  ++stats_[NUM_IE_DIRECTIVES];
}

void LoggingFilter::Characters(const std::string& characters) {
}

void LoggingFilter::IgnorableWhitespace(const std::string& whitespace) {
}

void LoggingFilter::Directive(const std::string& directive) {
  ++stats_[NUM_DIRECTIVES];
}

void LoggingFilter::Flush() {
}

// Logging, diffing, and aggregation
void LoggingFilter::LogStatistics(StatisticsLog *statistics_log) const {
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    statistics_log->LogStat(kStatisticNames[statistic], stats_[statistic]);
  }
}

bool LoggingFilter::Equals(const LoggingFilter &that) const {
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    if (this->stats_[statistic] != that.stats_[statistic])
      return false;
  }
  return true;
}

void LoggingFilter::LogDifferences(
    const LoggingFilter &that, StatisticsLog *statistics_log) const {
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    if (this->stats_[statistic] != that.stats_[statistic]) {
      statistics_log->LogDifference(
          kStatisticNames[statistic],
          this->stats_[statistic], that.stats_[statistic]);
    }
  }
}

void LoggingFilter::Aggregate(const LoggingFilter &that) {
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    this->stats_[statistic] += that.stats_[statistic];
  }
}

void LoggingFilter::AggregateDifferences(const LoggingFilter &first,
                                         const LoggingFilter &second) {
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    this->stats_[statistic] +=
        first.stats_[statistic] - second.stats_[statistic];
  }
}

void LoggingFilter::Reset() {
  // Cleaner than memset?
  for (int statistic = MIN_STAT; statistic < MAX_STAT; ++statistic) {
    stats_[statistic] = 0;
  }
}
}
