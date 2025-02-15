/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>
#include "eerratic_timer.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

std::atomic<bool> flag{false};

uint32_t get_current_time_impl() {
    using namespace std::chrono;
    return static_cast<uint32_t>(
        duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()
    );
}

void sleep_ms_impl(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool is_event_set_impl() {
    return flag.load();
}

void thread_func() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    flag.store(true);
}

TEST(eerratic_timer, test_eerratic_timer) {
    const uint32_t loop_start_time = get_current_time_impl();
    const uint32_t loop_expected_elapsed_time = 7000;

    timer_utils_t timer_func_base;
    timer_func_base.get_current_time_func = get_current_time_impl;
    timer_func_base.sleep_func = sleep_ms_impl;
    timer_func_base.is_event_set_func = is_event_set_impl;

    timer_utils_t timer_step_0 = timer_func_base;
    timer_utils_t timer_step_1 = timer_func_base;
    timer_utils_t timer_step_2 = timer_func_base;

    timer_step_0.expected_elapsed_time = 5500;
    timer_step_1.expected_elapsed_time = 500;
    timer_step_2.expected_elapsed_time = loop_expected_elapsed_time;

    std::thread th(thread_func);
    th.detach();
    ERROR_CODE ret_0 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_0, WAIT_EVENT);
    EXPECT_EQ(ret_0, ERROR_CODE_OK);
    EXPECT_NEAR(timer_step_0.elapsed_time, 5000, 2);

    flag = false;
    std::thread th2(thread_func);
    th2.detach();
    ERROR_CODE ret_1 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_1, WAIT_EVENT);
    EXPECT_EQ(ret_1, ERROR_CODE_TIMEOUT);
    EXPECT_NEAR(timer_step_1.elapsed_time, 500, 2);

    ERROR_CODE ret_2 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_2, SLEEP_REMAINING_TIME);
    EXPECT_EQ(ret_2, ERROR_CODE_OK);

    uint32_t elapsed_time = get_current_time_impl() - loop_start_time;
    EXPECT_NEAR(elapsed_time, loop_expected_elapsed_time, 2);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}