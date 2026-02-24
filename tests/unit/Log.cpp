#include <gtest/gtest.h>
#include <core/Log.hpp>

TEST(Log, LoggerShouldEqDefaultLogger)
{
  EXPECT_EQ(debug::logger, debug::default_logger);
}

TEST(Log, UnimplimentedShouldabort)
{
  #if defined(GTEST_HAS_DEATH_TEST) && GTEST_HAS_DEATH_TEST
      ASSERT_DEATH(debug::unimpl(), ".*");
  #else
      GTEST_SKIP_("Death tests not supported on this platform (Emscripten)");
  #endif
}

TEST(Log, UnrechableShouldabort)
{
  #if defined(GTEST_HAS_DEATH_TEST) && GTEST_HAS_DEATH_TEST
      ASSERT_DEATH(debug::unrechable(), ".*");
  #else
      GTEST_SKIP_("Death tests not supported on this platform (Emscripten)");
  #endif
}