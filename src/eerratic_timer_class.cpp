/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "eerratic_timer_class.hpp"


EEerraticTimer::EEerraticTimer(uint32_t loopExpectedElapsedTime,
            get_current_time_func_t getTimeFunc,
            sleep_func_t sleepFunc)
    : m_loopExpectedElapsedTime(loopExpectedElapsedTime)
{
    if (!getTimeFunc) {
        throw std::invalid_argument("get_current_time_func is null");
    }
    m_timerUtils.get_current_time_func = getTimeFunc;
    m_timerUtils.sleep_func = sleepFunc;
}

void EEerraticTimer::addStep(int id,
            uint32_t expectedElapsedTime,
            is_event_set_func_t isEventSetFunc,
            sleep_type_t sleepType)
{
    m_steps[id] = { expectedElapsedTime, isEventSetFunc, sleepType };
}

void EEerraticTimer::resetLoop() {
    m_loopStartTime = m_timerUtils.get_current_time_func();
}

ERROR_CODE EEerraticTimer::executeSleep(int id) {
    auto it = m_steps.find(id);
    if (it == m_steps.end()) {
        return ERROR_CODE_INVALID_PARAMETER;
    }
    m_timerUtils.expected_elapsed_time = it->second.expectedElapsedTime;
    m_timerUtils.is_event_set_func = it->second.isEventSetFunc;
    return eerratic_sleep(m_loopStartTime, m_loopExpectedElapsedTime, &m_timerUtils, it->second.sleepType);
}

uint32_t EEerraticTimer::getLastElapsedTime() const {
    return m_timerUtils.elapsed_time;
}

uint32_t EEerraticTimer::getLoopStartTime() const {
    return m_loopStartTime;
}
