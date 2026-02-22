#include <gtest/gtest.h>
#include <core/SysInfo.hpp>


TEST(SysInfo, CeckbuildInfo)
{
    os::build_info();
}

TEST(SysInfo, CeckOsInfo)
{
    os::name();
    os::arch();
    os::bits();
}