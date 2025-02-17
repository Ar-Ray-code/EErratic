/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EERRATIC_TIMER_H
#define EERRATIC_TIMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum ERROR_CODE
{
    ERROR_CODE_OK = 0,
    ERROR_CODE_TIMEOUT = -1,
    ERROR_CODE_NULL_POINTER = -2,
    ERROR_CODE_TOTAL_TIMEOUT = -3,
    ERROR_CODE_INVALID_PARAMETER = -4,
    ERROR_CODE_UNKNOWN = -5
} ERROR_CODE;

typedef uint32_t (*get_current_time_func_t)(void);
typedef void (*sleep_func_t)(uint32_t);
typedef bool (*is_event_set_func_t)(void);


typedef struct
{
    uint32_t elapsed_time; 
    uint32_t expected_elapsed_time;
    get_current_time_func_t get_current_time_func;
    is_event_set_func_t is_event_set_func;
    sleep_func_t sleep_func;
} timer_utils_t;

typedef enum {
    WAIT_EVENT,
    WAIT_TIME_AND_EVENT,
    SLEEP_REMAINING_TIME
} sleep_type_t;


/**
 * @brief Check if the timer is expired
 * 
 * @param loop_start_time The start time of the loop
 * @param loop_expected_elapsed_time The expected elapsed time of the loop
 * @param start_time The start time of the timer
 * @param expected_elapsed_time The expected elapsed time of the timer
 * @param get_current_time_func The function to get the current time
 * @return ERROR_CODE 
 */
inline static ERROR_CODE is_timer_expired(
    const uint32_t loop_start_time,
    const uint32_t loop_expected_elapsed_time,
    const uint32_t start_time,
    const uint32_t expected_elapsed_time,
    get_current_time_func_t get_current_time_func)
{
    if (get_current_time_func == NULL)
    {
        return ERROR_CODE_NULL_POINTER;
    }

    if (get_current_time_func() - loop_start_time >= loop_expected_elapsed_time) {
        return ERROR_CODE_TOTAL_TIMEOUT;
    } else if (get_current_time_func() - start_time >= expected_elapsed_time) {
        return ERROR_CODE_TIMEOUT;
    } else {
        return ERROR_CODE_OK;
    }
}

/**
 * @brief Sleep the remaining time
 * 
 * @param loop_start_time The start time of the loop
 * @param loop_expected_elapsed_time The expected elapsed time of the loop
 * @param expected_elapsed_time The expected elapsed time of the timer
 * @param elapsed_time The elapsed time
 * @param get_current_time_func The function to get the current time
 * @param sleep_func The function to sleep
 */
static inline void sleep_remaining_time(
    const uint32_t loop_start_time,
    const uint32_t loop_expected_elapsed_time,
    const uint32_t expected_elapsed_time,
    uint32_t* elapsed_time,
    get_current_time_func_t get_current_time_func,
    sleep_func_t sleep_func)
{
    if (get_current_time_func == NULL || sleep_func == NULL)
    {
        return;
    }

    uint32_t current_time = get_current_time_func();
    
    if (current_time - loop_start_time >= loop_expected_elapsed_time) {
        return;
    }

    uint32_t remaining_time = expected_elapsed_time;
    remaining_time = (remaining_time > loop_expected_elapsed_time - (current_time - loop_start_time)) ? loop_expected_elapsed_time - (current_time - loop_start_time) : remaining_time;

    if (remaining_time > 0) {
        sleep_func(remaining_time);
    }

    if (elapsed_time != NULL) {
        *elapsed_time = get_current_time_func() - current_time;
    }
    return;
}


/**
 * @brief Wait for the timeout or the event
 * 
 * @param loop_start_time The start time of the loop
 * @param loop_expected_elapsed_time The expected elapsed time of the loop
 * @param expected_elapsed_time The expected elapsed time of the timer
 * @param elapsed_time The elapsed time
 * @param get_current_time_func The function to get the current time
 * @param is_event_set_func The function to check if the event is set
 * @return ERROR_CODE 
 */
static inline ERROR_CODE wait_timeout_or_event(
    const uint32_t loop_start_time,
    const uint32_t loop_expected_elapsed_time,
    const uint32_t expected_elapsed_time,
    uint32_t* elapsed_time,
    get_current_time_func_t get_current_time_func,
    is_event_set_func_t is_event_set_func)
{
    if (get_current_time_func == NULL || is_event_set_func == NULL)
    {
        return ERROR_CODE_NULL_POINTER;
    }

    uint32_t start_time = get_current_time_func();
    while (!is_event_set_func())
    {
        ERROR_CODE error_code = is_timer_expired(loop_start_time, loop_expected_elapsed_time, start_time, expected_elapsed_time, get_current_time_func);
        if (error_code != ERROR_CODE_OK)
        {
            if (elapsed_time != NULL)
            {
                *elapsed_time = get_current_time_func() - start_time;
            }
            return ERROR_CODE_TIMEOUT;
        }
    }

    if (elapsed_time != NULL)
    {
        *elapsed_time = get_current_time_func() - start_time;
    }
    return ERROR_CODE_OK;
}

/**
 * @brief Wait for the timeout and the event
 * 
 * @param loop_start_time The start time of the loop
 * @param loop_expected_elapsed_time The expected elapsed time of the loop
 * @param expected_elapsed_time The expected elapsed time of the timer
 * @param elapsed_time The elapsed time
 * @param get_current_time_func The function to get the current time
 * @param is_event_set_func The function to check if the event is set
 * @param sleep_func The function to sleep
 * @return ERROR_CODE 
 */
static inline ERROR_CODE wait_time_and_event(
    const uint32_t loop_start_time,
    const uint32_t loop_expected_elapsed_time,
    const uint32_t expected_elapsed_time,
    uint32_t* elapsed_time,
    get_current_time_func_t get_current_time_func,
    is_event_set_func_t is_event_set_func,
    sleep_func_t sleep_func)
{
    if (get_current_time_func == NULL || is_event_set_func == NULL || sleep_func == NULL)
    {
        return ERROR_CODE_NULL_POINTER;
    }

    uint32_t start_time = get_current_time_func();

    while (!is_event_set_func())
    {
        if (is_timer_expired(loop_start_time, loop_expected_elapsed_time, start_time, expected_elapsed_time, get_current_time_func) != ERROR_CODE_OK)
        {
            if (elapsed_time != NULL)
            {
                *elapsed_time = get_current_time_func() - start_time;
            }
            return ERROR_CODE_TIMEOUT;
        }
    }

    uint32_t remaining_time = expected_elapsed_time - (get_current_time_func() - start_time);
    sleep_remaining_time(loop_start_time, loop_expected_elapsed_time, remaining_time, NULL, get_current_time_func, sleep_func);
    if (elapsed_time != NULL)
    {
        *elapsed_time = get_current_time_func() - start_time;
    }
    return ERROR_CODE_OK;
}

/**
 * @brief Sleep eerratic
 * 
 * @param loop_start_time The start time of the loop
 * @param loop_expected_elapsed_time The expected elapsed time of the loop
 * @param timer_utils The timer utils
 * @param sleep_type The sleep type
 * @return ERROR_CODE 
 */
static inline ERROR_CODE eerratic_sleep(
    const uint32_t loop_start_time,
    const uint32_t loop_expected_elapsed_time,
    timer_utils_t* timer_utils,
    sleep_type_t sleep_type)
{
    if (timer_utils->get_current_time_func == NULL)
    {
        return ERROR_CODE_NULL_POINTER;
    }

    switch (sleep_type)
    {
    case WAIT_EVENT:
        return wait_timeout_or_event(
            loop_start_time,
            loop_expected_elapsed_time,
            timer_utils->expected_elapsed_time,
            &timer_utils->elapsed_time,
            timer_utils->get_current_time_func,
            timer_utils->is_event_set_func);
    case WAIT_TIME_AND_EVENT:
        return wait_time_and_event(
            loop_start_time,
            loop_expected_elapsed_time,
            timer_utils->expected_elapsed_time,
            &timer_utils->elapsed_time,
            timer_utils->get_current_time_func,
            timer_utils->is_event_set_func,
            timer_utils->sleep_func);
    case SLEEP_REMAINING_TIME:
        sleep_remaining_time(
            loop_start_time,
            loop_expected_elapsed_time,
            timer_utils->expected_elapsed_time,
            &timer_utils->elapsed_time,
            timer_utils->get_current_time_func,
            timer_utils->sleep_func);
        return ERROR_CODE_OK;
    default:
        return ERROR_CODE_INVALID_PARAMETER;
    }
    return ERROR_CODE_UNKNOWN;
}

#ifdef __cplusplus
}
#endif

#endif // EERRATIC_TIMER_H
