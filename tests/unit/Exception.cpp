#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <core/Exception.hpp>

using namespace std;

TEST(Exception, ThrowingException)
{
    EXPECT_THROW({throw Exception("error");}, Exception);
}

TEST(Exception, ExceptionHasErrorMessage)
{
    try {
        throw Exception("error");
    } catch (const Exception& e) {
        EXPECT_THAT(e.what(), testing::HasSubstr("error"));
    }
}