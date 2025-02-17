/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "eerratic_timer_class.hpp"


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
    std::this_thread::sleep_for(std::chrono::seconds(2));
    flag.store(true);
}

int main() {
    const uint32_t loopExpectedElapsedTime = 7000;
    EEerraticTimer timer(loopExpectedElapsedTime, get_current_time_impl, sleep_ms_impl);

    timer.addStep(0, 2500, is_event_set_impl, WAIT_EVENT);
    timer.addStep(1, 2500, is_event_set_impl, WAIT_TIME_AND_EVENT);
    timer.addStep(2, 500,  is_event_set_impl, WAIT_EVENT);
    timer.addStep(3, loopExpectedElapsedTime, NULL, SLEEP_REMAINING_TIME);
    timer.resetLoop();

    flag.store(false);
    {
        std::thread t(thread_func);
        t.detach();
        ERROR_CODE ret0 = timer.executeStep(0);
        std::cout << "Step 0 elapsed time: " << timer.getLastElapsedTime() << " ms\tresult: " 
                  << (ret0 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    flag.store(false);
    {
        std::thread t(thread_func);
        t.detach();
        ERROR_CODE ret = timer.executeStep(1);
        std::cout << "Step 1 elapsed time: " << timer.getLastElapsedTime() << " ms\tresult: " 
                  << (ret == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    flag.store(false);
    {
        std::thread t(thread_func);
        t.detach();
        ERROR_CODE ret = timer.executeStep(2);
        std::cout << "Step 2 elapsed time: " << timer.getLastElapsedTime() << " ms\tresult: " 
                  << (ret == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    {
        ERROR_CODE ret = timer.executeStep(3);
        std::cout << "Step 3 elapsed time: " << timer.getLastElapsedTime() << " ms\tresult: " 
                  << (ret == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED") << std::endl;
    }

    std::cout << "----------------" << std::endl;
    std::cout << "\u23F1 Total elapsed time: " 
              << (get_current_time_impl() - timer.getLoopStartTime()) << " ms" << std::endl;

    return 0;
}
