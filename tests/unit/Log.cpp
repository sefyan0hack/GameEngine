#include <gtest/gtest.h>
#include <core/Log.hpp>

TEST(Log, LoggerShouldEqDefaultLogger)
{
  EXPECT_EQ(debug::logger, debug::default_logger);
}

TEST(Log, UnimplimentedShouldabort)
{
  ASSERT_DEATH(debug::unimpl(), "");
}

TEST(Log, UnrechableShouldabort)
{
  
  ASSERT_DEATH(debug::unrechable(), "");
}