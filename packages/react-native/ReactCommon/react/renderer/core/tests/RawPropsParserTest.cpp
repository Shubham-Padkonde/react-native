/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <array>
#include <string>

#include <gtest/gtest.h>
#include <react/renderer/core/RawProps.h>
#include <react/renderer/core/RawPropsParser.h>

using namespace facebook;
using namespace facebook::react;

namespace {

struct KeyParts {
  const char* prefix;
  const char* suffix;
};

constexpr std::array<KeyParts, 5> keyParts{{
    {"border", nullptr},
    {nullptr, "End"},
    {"border", "End"},
    {"accessibilityBorderEnd", nullptr},
    {nullptr, "AccessibilityProperty"},
}};

class ComposedKeyProps : public Props {
 public:
  ComposedKeyProps() = default;
  ComposedKeyProps(
      const PropsParserContext&,
      const ComposedKeyProps&,
      const RawProps& rawProps) {
    for (const auto& parts : keyParts) {
      rawProps.at("Width", parts.prefix, parts.suffix);
      rawProps.at("Color", parts.prefix, parts.suffix);
      // Repeated accesses during preparation must refer to the same key.
      rawProps.at("Width", parts.prefix, parts.suffix);
    }
  }
};

class TemporaryKeyProps : public Props {
 public:
  TemporaryKeyProps() = default;
  TemporaryKeyProps(
      const PropsParserContext&,
      const TemporaryKeyProps&,
      const RawProps& rawProps) {
    for (int i = 0; i < 32; ++i) {
      auto key = "prop" + std::to_string(i);
      rawProps.at(key.c_str());
    }
  }
};

} // namespace

TEST(RawPropsParserTest, ownsComposedKeys) {
  RawPropsParser parser;
  parser.prepare<ComposedKeyProps>();

  auto values = folly::dynamic::object();
  for (const auto& parts : keyParts) {
    const std::string prefix = parts.prefix ? parts.prefix : "";
    const std::string suffix = parts.suffix ? parts.suffix : "";
    values[prefix + "Width" + suffix] = 17;
    values[prefix + "Color" + suffix] = 42;
  }
  RawProps rawProps(values);
  rawProps.parse(parser);

  for (const auto& parts : keyParts) {
    // Reverse order and repeat a key to exercise cursor wraparound.
    const auto* color = rawProps.at("Color", parts.prefix, parts.suffix);
    ASSERT_NE(color, nullptr);
    EXPECT_EQ(static_cast<int>(*color), 42);
    const auto* width = rawProps.at("Width", parts.prefix, parts.suffix);
    ASSERT_NE(width, nullptr);
    EXPECT_EQ(static_cast<int>(*width), 17);
    EXPECT_EQ(rawProps.at("Width", parts.prefix, parts.suffix), width);
  }
}

TEST(RawPropsParserTest, ownsTemporaryNamesAcrossStorageGrowth) {
  RawPropsParser parser;
  parser.prepare<TemporaryKeyProps>();

  auto values = folly::dynamic::object();
  for (int i = 0; i < 32; ++i) {
    values["prop" + std::to_string(i)] = i;
  }
  RawProps rawProps(values);
  rawProps.parse(parser);

  for (int i = 31; i >= 0; --i) {
    auto key = "prop" + std::to_string(i);
    const auto* value = rawProps.at(key.c_str());
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(static_cast<int>(*value), i);
  }
}
