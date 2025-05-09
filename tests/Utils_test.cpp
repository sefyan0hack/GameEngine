#include <gtest/gtest.h>
#include <core/Utils.hpp>
#include <fstream>
#include <chrono>
#include <future>
#include <thread>

using namespace std;
using namespace chrono;
using namespace chrono_literals;

TEST(load_file_async, exist) {
    auto filename = "Hello.txt";
    ofstream file { filename };
    if(!file.is_open()){
        SUCCEED();
    }
    
    auto r = load_file_async(filename).get();
    EXPECT_EQ(r.has_value(), true);
}

TEST(load_file_async, not_exist) {
    auto r = load_file_async("filenotexist.ext").get();
    EXPECT_EQ(r.has_value(), false);
}

TEST(split, string) {
    auto r = split("hi my name sofyane", " ");
    decltype(r) expect = {"hi", "my", "name", "sofyane"};
    EXPECT_EQ(r, expect);
}

TEST(split, empty_string) {
    auto r = split("", " ");
    decltype(r) expect;
    EXPECT_EQ(r, expect);
}

TEST(split, empty_null) {
    auto r = split((const char*)nullptr, " ");
    decltype(r) expect;
    EXPECT_EQ(r, expect);
}

TEST(replace, with_char) {
    auto r = replace("fuzz", 'f', 'b');
    EXPECT_EQ(r, "buzz");
}

TEST(replace, with_null_char) {
    auto r = replace("fuzz", 'f', '\0');
    EXPECT_EQ(r, "fuzz");
}

TEST(replace, with_not_exist_char) {
    auto r = replace("fuzz", 'a', 'b');
    EXPECT_EQ(r, "fuzz");
}

TEST(replace, c_null) {
    auto r = replace("fuzz", '\0', 'b');
    EXPECT_EQ(r, "fuzz");
}


TEST(SetTimeout, BasicFunctionality) {
    auto promise = make_shared<std::promise<void>>();
    auto future = promise->get_future();

    setTimeOut(100, [promise]() {
        promise->set_value();
    });

    auto status = future.wait_for(150ms);
    EXPECT_EQ(status, future_status::ready);
}

TEST(SetTimeout, DelayIsAtLeastSpecified) {
    auto promise = make_shared<std::promise<void>>();
    auto future = promise->get_future();
    auto start = steady_clock::now();
    steady_clock::time_point end;

    setTimeOut(100, [promise, &end]() {
        end = steady_clock::now();
        promise->set_value();
    });

    future.wait();
    auto duration = duration_cast<milliseconds>(end - start);
    ASSERT_GE(duration.count(), 100);
}

TEST(SetTimeout, ValueArguments) {
    auto promise = make_shared<std::promise<int>>();
    auto future = promise->get_future();

    setTimeOut(100, [promise](int x) {
        promise->set_value(x);
    }, 42);

    EXPECT_EQ(future.get(), 42);
}


TEST(SetTimeout, ReferenceArgument) {
    auto promise = make_shared<std::promise<void>>();
    auto future = promise->get_future();
    int value = 0;

    setTimeOut(100, [promise, &value](int& x) {
        x = 42;
        promise->set_value();
    }, ref(value));

    future.wait();
    EXPECT_EQ(value, 42);
}

TEST(SetTimeout, MoveOnlyArgument) {
    auto promise = make_shared<std::promise<void>>();
    auto future = promise->get_future();

    setTimeOut(100, [promise](unique_ptr<int> ptr) {
        EXPECT_EQ(*ptr, 42);
        promise->set_value();
    }, make_unique<int>(42));

    future.wait();
}

TEST(SetTimeout, ZeroDelay) {
    auto promise = make_shared<std::promise<void>>();
    auto future = promise->get_future();

    setTimeOut(0, [promise]() {
        promise->set_value();
    });

    auto status = future.wait_for(50ms);
    EXPECT_EQ(status, future_status::ready);
}

TEST(SetTimeout, MultipleTimeouts) {
    auto p1 = make_shared<std::promise<void>>();
    auto p2 = make_shared<std::promise<void>>();
    auto f1 = p1->get_future();
    auto f2 = p2->get_future();
    atomic<int> count{0};

    setTimeOut(100, [p1, &count]() {
        count++;
        p1->set_value();
    });
    setTimeOut(100, [p2, &count]() {
        count++;
        p2->set_value();
    });

    f1.wait();
    f2.wait();
    EXPECT_EQ(count.load(), 2);
}