// Copyright 2022 TF.Text Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tensorflow_text/core/kernels/sentence_breaking_utils.h"

#include <memory>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "icu4c/source/common/unicode/uchar.h"
#include "icu4c/source/common/unicode/ucnv.h"
#include "icu4c/source/common/unicode/ucnv_err.h"
#include "icu4c/source/common/unicode/umachine.h"
#include "icu4c/source/common/unicode/uniset.h"
#include "icu4c/source/common/unicode/unistr.h"
#include "icu4c/source/common/unicode/uset.h"
#include "icu4c/source/common/unicode/utypes.h"

namespace tensorflow {
namespace text {
namespace {

class SentenceBreakingUtilsTest {
 protected:
  UConverter* GetUConverter() {
    constexpr char name[] = "UTF-8";
    UErrorCode status = U_ZERO_ERROR;
    UConverter* converter = ucnv_open(name, &status);
    if (U_FAILURE(status)) {
      if (converter) {
        ucnv_close(converter);
      }
      return nullptr;
    }
    return converter;
  }
};

class SentenceBreakingUtilsParamTest : public SentenceBreakingUtilsTest,
                                       public ::testing::TestWithParam<UChar> {
 protected:
  void SetUp() override {
    converter_ = SentenceBreakingUtilsTest::GetUConverter();
    ASSERT_NE(converter_, nullptr);
  }

  void TearDown() override { ucnv_close(converter_); }

  std::string StringFromUnicodeChar(UChar32 input) {
    std::string result;
    icu::UnicodeString test_unicode_string(input);
    test_unicode_string.toUTF8String(result);
    return result;
  }

  UConverter* converter_;
};

class IsTerminalPuncParamTest : public SentenceBreakingUtilsParamTest {};

class IsTerminalPuncTest : public SentenceBreakingUtilsTest,
                           public ::testing::Test {};

const UChar is_terminal_punc_test_cases[] = {
    0x055C,  // Armenian exclamation mark
    0x055E,  // Armenian question mark
    0x0589,  // Armenian full stop
    0x061F,  // Arabic question mark
    0x06D4,  // Arabic full stop
    0x0700,  // Syriabc end of paragraph
    0x0701,  // Syriac supralinear full stop
    0x0702,  // Syriac sublinear full stop
    0x1362,  // Ethiopic full stop
    0x1367,  // Ethiopic question mark
    0x1368,  // Ethiopic paragraph separator
    0x104A,  // Myanmar sign little section
    0x104B,  // Myanmar sign section
    0x166E,  // Canadian syllabics full stop
    0x17d4,  // Khmer sign khan
    0x1803,  // Mongolian full stop
    0x1809,  // Mongolian Manchu full stop
    0x1944,  // Limbu exclamation mark
    0x1945,  // Limbu question mark
    0x203C,  // double exclamation mark
    0x203D,  // interrobang
    0x2047,  // double question mark
    0x2048,  // question exclamation mark
    0x2049,  // exclamation question mark
    0x3002,  // ideographic full stop
    0x037E,  // Greek question mark
    0xFE52,  // small full stop
    0xFE56,  // small question mark
    0xFE57,  // small exclamation mark
    0xFF01,  // fullwidth exclamation mark
    0xFF0E,  // fullwidth full stop
    0xFF1F,  // fullwidth question mark
    0xFF61,  // halfwidth ideographic full stop
    0x2026,  // ellipsis
    0x0964,
    0x0965,  // Devanagari danda..Devanagari double
};

TEST_P(IsTerminalPuncParamTest, IsTerminalPunc) {
  UnicodeUtil util(converter_);
  std::string test_string = StringFromUnicodeChar(GetParam());
  bool result = false;
  EXPECT_TRUE(util.IsTerminalPunc(test_string, &result).ok());
  EXPECT_TRUE(result);
}

INSTANTIATE_TEST_SUITE_P(IsTerminalPuncTest, IsTerminalPuncParamTest,
                         ::testing::ValuesIn(is_terminal_punc_test_cases));

TEST_F(IsTerminalPuncTest, IsMultiCharEllipseTerminalPunc) {
  UConverter* converter = SentenceBreakingUtilsTest::GetUConverter();
  ASSERT_NE(converter, nullptr);
  UnicodeUtil util(converter);
  std::string test_string = "...";
  bool result;
  EXPECT_TRUE(util.IsTerminalPunc(test_string, &result).ok());
  EXPECT_TRUE(result);
  ucnv_close(converter);
}

TEST_F(IsTerminalPuncTest, TestMultiUnicodeChars) {
  UConverter* converter = SentenceBreakingUtilsTest::GetUConverter();
  ASSERT_NE(converter, nullptr);
  UnicodeUtil util(converter);
  std::string test_string = "never gonna let you decode";
  bool result;
  EXPECT_TRUE(util.IsTerminalPunc(test_string, &result).ok());
  EXPECT_FALSE(result);
  ucnv_close(converter);
}

TEST_F(IsTerminalPuncTest, TestInvalidConverter) {
  UErrorCode status = U_ZERO_ERROR;
  UConverter* converter = ucnv_open("cant find me", &status);
  UnicodeUtil util(converter);
  std::string test_string = ".";
  bool result;
  EXPECT_FALSE(util.IsTerminalPunc(test_string, &result).ok());
  ucnv_close(converter);
}

class ClosePuncParamTest : public SentenceBreakingUtilsParamTest {};

const UChar close_punc_test_cases[] = {
    0x29,   0x5D, 0x3E, 0x7D,
    0x207E,  // superscript right parenthesis
    0x208E,  // subscript right parenthesis
    0x27E7,  // mathematical right white square bracket
    0x27E9,  // mathematical right angle bracket
    0x27EB,  // mathematical right double angle bracket
    0x2984,  // right white curly bracket
    0x2986,  // right white parenthesis
    0x2988,  // Z notation right image bracket
    0x298A,  // Z notation right binding bracket
    0x298C,  // right square bracket with underbar
    0x298E,  // right square bracket with tick in top corner
    0x2990,  // right square bracket with tick in bottom corner
    0x2992,  // right angle bracket with dot
    0x2994,  // right arc greater-than bracket
    0x2996,  // double right arc less-than bracket
    0x2998,  // right black tortoise shell bracket
    0x29D9,  // right wiggly fence
    0x29DB,  // right double wiggly fence
    0x29FD,  // right-pointing curved angle bracket
    0x3009,  // CJK right angle bracket
    0x300B,  // CJK right double angle bracket
    0x3011,  // CJK right black lenticular bracket
    0x3015,  // CJK right tortoise shell bracket
    0x3017,  // CJK right white lenticular bracket
    0x3019,  // CJK right white tortoise shell bracket
    0x301B,  // CJK right white square bracket
    0xFD3F,  // Ornate right parenthesis
    0xFE5A,  // small right parenthesis
    0xFE5C,  // small right curly bracket
    0xFF09,  // fullwidth right parenthesis
    0xFF3D,  // fullwidth right square bracket
    0xFF5D,  // fullwidth right curly bracket
    0x27,   0x60, 0x22,
    0xFF07,  // fullwidth apostrophe
    0xFF02,  // fullwidth quotation mark
    0x2019,  // right single quotation mark (English, others)
    0x201D,  // right double quotation mark (English, others)
    0x2018,  // left single quotation mark (Czech, German, Slovak)
    0x201C,  // left double quotation mark (Czech, German, Slovak)
    0x203A,  // single right-pointing angle quotation mark (French, others)
    0x00BB,  // right-pointing double angle quotation mark (French, others)
    0x2039,  // single left-pointing angle quotation mark (Slovenian, others)
    0x00AB,  // left-pointing double angle quotation mark (Slovenian, others)
    0x300D,  // right corner bracket (East Asian languages)
    0xfe42,  // presentation form for vertical right corner bracket
    0xFF63,  // halfwidth right corner bracket (East Asian languages)
    0x300F,  // right white corner bracket (East Asian languages)
    0xfe44,  // presentation form for vertical right white corner bracket
    0x301F,  // low double prime quotation mark (East Asian languages)
    0x301E,  // close double prime (East Asian languages written horizontally)
};

TEST_P(ClosePuncParamTest, IsClosePunc) {
  UnicodeUtil util(converter_);
  std::string test_string = StringFromUnicodeChar(GetParam());
  bool result = false;
  EXPECT_TRUE(util.IsClosePunc(test_string, &result).ok());
  EXPECT_TRUE(result);
}

INSTANTIATE_TEST_SUITE_P(IsClosePuncParamTest, ClosePuncParamTest,
                         ::testing::ValuesIn(close_punc_test_cases));

class OpenParenParamTest : public SentenceBreakingUtilsParamTest {};

const UChar open_paren_test_cases[] = {
    '(',    '[', '<', '{',
    0x207D,  // superscript left parenthesis
    0x208D,  // subscript left parenthesis
    0x27E6,  // mathematical left white square bracket
    0x27E8,  // mathematical left angle bracket
    0x27EA,  // mathematical left double angle bracket
    0x2983,  // left white curly bracket
    0x2985,  // left white parenthesis
    0x2987,  // Z notation left image bracket
    0x2989,  // Z notation left binding bracket
    0x298B,  // left square bracket with underbar
    0x298D,  // left square bracket with tick in top corner
    0x298F,  // left square bracket with tick in bottom corner
    0x2991,  // left angle bracket with dot
    0x2993,  // left arc less-than bracket
    0x2995,  // double left arc greater-than bracket
    0x2997,  // left black tortoise shell bracket
    0x29D8,  // left wiggly fence
    0x29DA,  // left double wiggly fence
    0x29FC,  // left-pointing curved angle bracket
    0x3008,  // CJK left angle bracket
    0x300A,  // CJK left double angle bracket
    0x3010,  // CJK left black lenticular bracket
    0x3014,  // CJK left tortoise shell bracket
    0x3016,  // CJK left white lenticular bracket
    0x3018,  // CJK left white tortoise shell bracket
    0x301A,  // CJK left white square bracket
    0xFD3E,  // Ornate left parenthesis
    0xFE59,  // small left parenthesis
    0xFE5B,  // small left curly bracket
    0xFF08,  // fullwidth left parenthesis
    0xFF3B,  // fullwidth left square bracket
    0xFF5B,  // fullwidth left curly bracket
};

TEST_P(OpenParenParamTest, IsOpenParen) {
  UnicodeUtil util(converter_);
  std::string test_string = StringFromUnicodeChar(GetParam());
  bool result = false;
  EXPECT_TRUE(util.IsOpenParen(test_string, &result).ok());
  EXPECT_TRUE(result);
}

INSTANTIATE_TEST_SUITE_P(IsOpenParenParamTest, OpenParenParamTest,
                         ::testing::ValuesIn(open_paren_test_cases));

class CloseParenParamTest : public SentenceBreakingUtilsParamTest {};

const UChar close_paren_test_cases[] = {
    ')',    ']', '>', '}',
    0x207E,  // superscript right parenthesis
    0x208E,  // subscript right parenthesis
    0x27E7,  // mathematical right white square bracket
    0x27E9,  // mathematical right angle bracket
    0x27EB,  // mathematical right double angle bracket
    0x2984,  // right white curly bracket
    0x2986,  // right white parenthesis
    0x2988,  // Z notation right image bracket
    0x298A,  // Z notation right binding bracket
    0x298C,  // right square bracket with underbar
    0x298E,  // right square bracket with tick in top corner
    0x2990,  // right square bracket with tick in bottom corner
    0x2992,  // right angle bracket with dot
    0x2994,  // right arc greater-than bracket
    0x2996,  // double right arc less-than bracket
    0x2998,  // right black tortoise shell bracket
    0x29D9,  // right wiggly fence
    0x29DB,  // right double wiggly fence
    0x29FD,  // right-pointing curved angle bracket
    0x3009,  // CJK right angle bracket
    0x300B,  // CJK right double angle bracket
    0x3011,  // CJK right black lenticular bracket
    0x3015,  // CJK right tortoise shell bracket
    0x3017,  // CJK right white lenticular bracket
    0x3019,  // CJK right white tortoise shell bracket
    0x301B,  // CJK right white square bracket
    0xFD3F,  // Ornate right parenthesis
    0xFE5A,  // small right parenthesis
    0xFE5C,  // small right curly bracket
    0xFF09,  // fullwidth right parenthesis
    0xFF3D,  // fullwidth right square bracket
    0xFF5D,  // fullwidth right curly bracket
};

TEST_P(CloseParenParamTest, IsCloseParen) {
  UnicodeUtil util(converter_);
  std::string test_string = StringFromUnicodeChar(GetParam());
  bool result = false;
  EXPECT_TRUE(util.IsCloseParen(test_string, &result).ok());
  EXPECT_TRUE(result);
}

INSTANTIATE_TEST_SUITE_P(IsCloseParenParamTest, CloseParenParamTest,
                         ::testing::ValuesIn(close_paren_test_cases));

class IsPunctuationWordParamTest : public SentenceBreakingUtilsParamTest {};

const UChar punc_word_test_cases[] = {
    '(',    '[',    '<',    '{',
    0x207D,  // superscript left parenthesis
    0x208D,  // subscript left parenthesis
    0x27E6,  // mathematical left white square bracket
    0x27E8,  // mathematical left angle bracket
    0x27EA,  // mathematical left double angle bracket
    0x2983,  // left white curly bracket
    0x2985,  // left white parenthesis
    0x2987,  // Z notation left image bracket
    0x2989,  // Z notation left binding bracket
    0x298B,  // left square bracket with underbar
    0x298D,  // left square bracket with tick in top corner
    0x298F,  // left square bracket with tick in bottom corner
    0x2991,  // left angle bracket with dot
    0x2993,  // left arc less-than bracket
    0x2995,  // double left arc greater-than bracket
    0x2997,  // left black tortoise shell bracket
    0x29D8,  // left wiggly fence
    0x29DA,  // left double wiggly fence
    0x29FC,  // left-pointing curved angle bracket
    0x3008,  // CJK left angle bracket
    0x300A,  // CJK left double angle bracket
    0x3010,  // CJK left black lenticular bracket
    0x3014,  // CJK left tortoise shell bracket
    0x3016,  // CJK left white lenticular bracket
    0x3018,  // CJK left white tortoise shell bracket
    0x301A,  // CJK left white square bracket
    0xFD3E,  // Ornate left parenthesis
    0xFE59,  // small left parenthesis
    0xFE5B,  // small left curly bracket
    0xFF08,  // fullwidth left parenthesis
    0xFF3B,  // fullwidth left square bracket
    0xFF5B,  // fullwidth left curly bracket
    '"',    '\'',   '`',
    0xFF07,  // fullwidth apostrophe
    0xFF02,  // fullwidth quotation mark
    0x2018,  // left single quotation mark (English, others)
    0x201C,  // left double quotation mark (English, others)
    0x201B,  // single high-reveresed-9 quotation mark (PropList.txt)
    0x201A,  // single low-9 quotation mark (Czech, German, Slovak)
    0x201E,  // double low-9 quotation mark (Czech, German, Slovak)
    0x201F,  // double high-reversed-9 quotation mark (PropList.txt)
    0x2019,  // right single quotation mark (Danish, Finnish, Swedish, Norw.)
    0x201D,  // right double quotation mark (Danish, Finnish, Swedish, Norw.)
    0x2039,  // single left-pointing angle quotation mark (French, others)
    0x00AB,  // left-pointing double angle quotation mark (French, others)
    0x203A,  // single right-pointing angle quotation mark (Slovenian, others)
    0x00BB,  // right-pointing double angle quotation mark (Slovenian, others)
    0x300C,  // left corner bracket (East Asian languages)
    0xFE41,  // presentation form for vertical left corner bracket
    0xFF62,  // halfwidth left corner bracket (East Asian languages)
    0x300E,  // left white corner bracket (East Asian languages)
    0xFE43,  // presentation form for vertical left white corner bracket
    0x301D,  // reversed double prime quotation mark (East Asian langs, horiz.)
    ')',    ']',    '>',    '}',
    0x207E,  // superscript right parenthesis
    0x208E,  // subscript right parenthesis
    0x27E7,  // mathematical right white square bracket
    0x27E9,  // mathematical right angle bracket
    0x27EB,  // mathematical right double angle bracket
    0x2984,  // right white curly bracket
    0x2986,  // right white parenthesis
    0x2988,  // Z notation right image bracket
    0x298A,  // Z notation right binding bracket
    0x298C,  // right square bracket with underbar
    0x298E,  // right square bracket with tick in top corner
    0x2990,  // right square bracket with tick in bottom corner
    0x2992,  // right angle bracket with dot
    0x2994,  // right arc greater-than bracket
    0x2996,  // double right arc less-than bracket
    0x2998,  // right black tortoise shell bracket
    0x29D9,  // right wiggly fence
    0x29DB,  // right double wiggly fence
    0x29FD,  // right-pointing curved angle bracket
    0x3009,  // CJK right angle bracket
    0x300B,  // CJK right double angle bracket
    0x3011,  // CJK right black lenticular bracket
    0x3015,  // CJK right tortoise shell bracket
    0x3017,  // CJK right white lenticular bracket
    0x3019,  // CJK right white tortoise shell bracket
    0x301B,  // CJK right white square bracket
    0xFD3F,  // Ornate right parenthesis
    0xFE5A,  // small right parenthesis
    0xFE5C,  // small right curly bracket
    0xFF09,  // fullwidth right parenthesis
    0xFF3D,  // fullwidth right square bracket
    0xFF5D,  // fullwidth right curly bracket
    '\'',   '"',    '`',
    0xFF07,  // fullwidth apostrophe
    0xFF02,  // fullwidth quotation mark
    0x2019,  // right single quotation mark (English, others)
    0x201D,  // right double quotation mark (English, others)
    0x2018,  // left single quotation mark (Czech, German, Slovak)
    0x201C,  // left double quotation mark (Czech, German, Slovak)
    0x203A,  // single right-pointing angle quotation mark (French, others)
    0x00BB,  // right-pointing double angle quotation mark (French, others)
    0x2039,  // single left-pointing angle quotation mark (Slovenian, others)
    0x00AB,  // left-pointing double angle quotation mark (Slovenian, others)
    0x300D,  // right corner bracket (East Asian languages)
    0xfe42,  // presentation form for vertical right corner bracket
    0xFF63,  // halfwidth right corner bracket (East Asian languages)
    0x300F,  // right white corner bracket (East Asian languages)
    0xfe44,  // presentation form for vertical right white corner bracket
    0x301F,  // low double prime quotation mark (East Asian languages)
    0x301E,  // close double prime (East Asian languages written horizontally)
    0x00A1,  // Spanish inverted exclamation mark
    0x00BF,  // Spanish inverted question mark
    '.',    '!',    '?',
    0x055C,  // Armenian exclamation mark
    0x055E,  // Armenian question mark
    0x0589,  // Armenian full stop
    0x061F,  // Arabic question mark
    0x06D4,  // Arabic full stop
    0x0700,  // Syriac end of paragraph
    0x0701,  // Syriac supralinear full stop
    0x0702,  // Syriac sublinear full stop
    0x0964,  // Devanagari danda..Devanagari double danda
    0x0965,
    0x1362,  // Ethiopic full stop
    0x1367,  // Ethiopic question mark
    0x1368,  // Ethiopic paragraph separator
    0x104A,  // Myanmar sign little section
    0x104B,  // Myanmar sign section
    0x166E,  // Canadian syllabics full stop
    0x17d4,  // Khmer sign khan
    0x1803,  // Mongolian full stop
    0x1809,  // Mongolian Manchu full stop
    0x1944,  // Limbu exclamation mark
    0x1945,  // Limbu question mark
    0x203C,  // double exclamation mark
    0x203D,  // interrobang
    0x2047,  // double question mark
    0x2048,  // question exclamation mark
    0x2049,  // exclamation question mark
    0x3002,  // ideographic full stop
    0x037E,  // Greek question mark
    0xFE52,  // small full stop
    0xFE56,  // small question mark
    0xFE57,  // small exclamation mark
    0xFF01,  // fullwidth exclamation mark
    0xFF0E,  // fullwidth full stop
    0xFF1F,  // fullwidth question mark
    0xFF61,  // halfwidth ideographic full stop
    0x2026,  // ellipsis
    0x30fb,  // Katakana middle dot
    0xff65,  // halfwidth Katakana middle dot
    0x2040,  // character tie
    '-',    '~',
    0x058a,  // Armenian hyphen
    0x1806,  // Mongolian todo soft hyphen
    0x2010,  // hyphen..horizontal bar
    0x2011, 0x2012, 0x2013, 0x2014, 0x2015,
    0x2053,  // swung dash -- from Table 6-3 of Unicode book
    0x207b,  // superscript minus
    0x208b,  // subscript minus
    0x2212,  // minus sign
    0x301c,  // wave dash
    0x3030,  // wavy dash
    0xfe31,  // presentation form for vertical em dash..en dash
    0xfe32,
    0xfe58,  // small em dash
    0xfe63,  // small hyphen-minus
    0xff0d,  // fullwidth hyphen-minus
    ',',    ':',    ';',
    0x00b7,  // middle dot
    0x0387,  // Greek ano teleia
    0x05c3,  // Hebrew punctuation sof pasuq
    0x060c,  // Arabic comma
    0x061b,  // Arabic semicolon
    0x066b,  // Arabic decimal separator
    0x066c,  // Arabic thousands separator
    0x0703,  // Syriac contraction and others
    0x0704, 0x0705, 0x0706, 0x0707, 0x0708, 0x0709, 0x70a,
    0x070c,  // Syric harklean metobelus
    0x0e5a,  // Thai character angkhankhu
    0x0e5b,  // Thai character khomut
    0x0f08,  // Tibetan mark sbrul shad
    0x0f0d,  // Tibetan mark shad..Tibetan mark rgya gram shad
    0x0f0e, 0x0f0f, 0x0f10, 0x0f11, 0x0f12,
    0x1361,  // Ethiopic wordspace
    0x1363,  // other Ethiopic chars
    0x1364, 0x1365, 0x1366,
    0x166d,  // Canadian syllabics chi sign
    0x16eb,  // Runic single punctuation..Runic cross punctuation
    0x16ed,
    0x17d5,  // Khmer sign camnuc pii huuh and other
    0x17d6,
    0x17da,  // Khmer sign koomut
    0x1802,  // Mongolian comma
    0x1804,  // Mongolian four dots and other
    0x1805,
    0x1808,  // Mongolian manchu comma
    0x3001,  // ideographic comma
    0xfe50,  // small comma and others
    0xfe51,
    0xfe54,  // small semicolon and other
    0xfe55,
    0xff0c,  // fullwidth comma
    0xff0e,  // fullwidth stop..fullwidth solidus
    0xff0f,
    0xff1a,  // fullwidth colon..fullwidth semicolon
    0xff1b,
    0xff64,  // halfwidth ideographic comma
    0x2016,  // double vertical line
    0x2032, 0x2033,
    0x2034,  // prime..triple prime
    0xfe61,  // small asterisk
    0xfe68,  // small reverse solidus
    0xff3c,  // fullwidth reverse solidus
};

TEST_P(IsPunctuationWordParamTest, IsPunctuation) {
  UnicodeUtil util(converter_);
  std::string test_string = StringFromUnicodeChar(GetParam());
  bool result = false;
  EXPECT_TRUE(util.IsPunctuationWord(test_string, &result).ok());
  EXPECT_TRUE(result);
}

INSTANTIATE_TEST_SUITE_P(IsPuncWordParamTest, IsPunctuationWordParamTest,
                         ::testing::ValuesIn(punc_word_test_cases));

class IsEllipsisTest : public SentenceBreakingUtilsTest,
                       public ::testing::Test {
 protected:
  void SetUp() override {
    converter_ = SentenceBreakingUtilsTest::GetUConverter();
  }

  void TearDown() override { ucnv_close(converter_); }

  UConverter* converter_;
};

TEST_F(IsEllipsisTest, IsEllipsis) {
  UnicodeUtil util(converter_);
  bool result = false;
  EXPECT_TRUE(util.IsEllipsis("...", &result).ok());
  EXPECT_TRUE(result);

  EXPECT_TRUE(util.IsEllipsis("…", &result).ok());
  EXPECT_TRUE(result);

  EXPECT_TRUE(util.IsEllipsis("@", &result).ok());
  EXPECT_FALSE(result);
}

}  // namespace
}  // namespace text
}  // namespace tensorflow
