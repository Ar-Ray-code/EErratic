/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

extern "C" {
#include "eerratic_timer.h"
}

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
    std::cout << "Thread started" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    flag.store(true);
}

int main() {
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

    flag.store(false);
    {
        std::thread t(thread_func);
        t.detach();
        ERROR_CODE ret_0 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_0, WAIT_EVENT);
        std::cout << "Elapsed time: " << timer_step_0.elapsed_time << " ms\tresult: " 
                  << (ret_0 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    flag.store(false);
    {
        std::thread t(thread_func);
        t.detach();
        ERROR_CODE ret_1 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_1, WAIT_EVENT);
        std::cout << "Elapsed time: " << timer_step_1.elapsed_time << " ms\tresult: " 
                  << (ret_1 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    {
        ERROR_CODE ret_2 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_2, SLEEP_REMAINING_TIME);
        std::cout << "Elapsed time: " << timer_step_2.elapsed_time << " ms\tresult: " 
                  << (ret_2 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    std::cout << "----------------" << std::endl;
    std::cout << "\u23F1 Total elapsed time: " 
              << (get_current_time_impl() - loop_start_time) << " ms" << std::endl;

    return 0;
}
