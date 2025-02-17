/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EERRATIC_TIMER_CLASS_HPP
#define EERRATIC_TIMER_CLASS_HPP

#include "eerratic_timer.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>


class EEerraticTimer {
public:
    struct StepConfig {
        uint32_t expectedElapsedTime;
        is_event_set_func_t isEventSetFunc;
        sleep_type_t sleepType;
    };

    EEerraticTimer(uint32_t, get_current_time_func_t, sleep_func_t);
    void addStep(int, uint32_t, is_event_set_func_t, sleep_type_t);
    void resetLoop();
    ERROR_CODE executeSleep(int);
    uint32_t getLastElapsedTime() const;
    uint32_t getLoopStartTime() const;

private:
    timer_utils_t m_timerUtils{};
    std::unordered_map<int, StepConfig> m_steps;
    uint32_t m_loopExpectedElapsedTime = 0;
    uint32_t m_loopStartTime = 0;
};

#endif // EERRATIC_TIMER_CLASS_HPP
