#include <gtest/gtest.h>
#include <core/Log.hpp>

TEST(Log, LoggerShouldEqDefaultLogger)
{
  EXPECT_EQ(logg::logger, logg::default_logger);
}

TEST(Log, UnimplimentedShouldabort)
{
  #if defined(GTEST_HAS_DEATH_TEST) && GTEST_HAS_DEATH_TEST
      ASSERT_DEATH(logg::unimpl(), ".*");
  #else
      GTEST_SKIP_("Death tests not supported on this platform (Emscripten)");
  #endif
}

TEST(Log, UnrechableShouldabort)
{
  #if defined(GTEST_HAS_DEATH_TEST) && GTEST_HAS_DEATH_TEST
      ASSERT_DEATH(logg::unrechable(), ".*");
  #else
      GTEST_SKIP_("Death tests not supported on this platform (Emscripten)");
  #endif
}