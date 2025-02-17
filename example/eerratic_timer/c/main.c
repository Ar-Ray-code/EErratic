/*
 * Copyright (c) 2024 Ar-Ray-code
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "eerratic_timer.h"

bool flag = false;

uint32_t get_current_time_impl(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void sleep_ms_impl(uint32_t ms) {
    usleep(ms * 1000);
}

bool is_event_set_impl(void) {
    return flag;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *thread_func(void *arg) {
    printf("Thread started\n");
    sleep(5);
    pthread_mutex_lock(&mutex);
    flag = true;
    pthread_mutex_unlock(&mutex);
    return NULL;
}


int main(void)
{
    const uint32_t loop_start_time = get_current_time_impl();
    const uint32_t loop_expected_elapsed_time = 7000;
    pthread_t th;

    timer_utils_t timer_func_base = {
        .get_current_time_func = get_current_time_impl,
        .sleep_func = sleep_ms_impl,
        .is_event_set_func = is_event_set_impl
    };

    timer_utils_t timer_step_0 = timer_func_base;
    timer_utils_t timer_step_1 = timer_func_base;
    timer_utils_t timer_step_2 = timer_func_base;

    timer_step_0.expected_elapsed_time = 5500;
    timer_step_1.expected_elapsed_time = 500;
    timer_step_2.expected_elapsed_time = loop_expected_elapsed_time;

    pthread_create(&th, NULL, thread_func, NULL);
    pthread_detach(th);
    ERROR_CODE ret_0 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_0, WAIT_EVENT);
    printf("Elapsed time: %u ms\tresult: %s\n", timer_step_0.elapsed_time, ret_0 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED");

    flag = false;
    pthread_create(&th, NULL, thread_func, NULL);
    pthread_detach(th);
    ERROR_CODE ret_1 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_1, WAIT_EVENT);
    printf("Elapsed time: %u ms\tresult: %s\n", timer_step_1.elapsed_time, ret_1 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED");

    ERROR_CODE ret_2 = eerratic_sleep(loop_start_time, loop_expected_elapsed_time, &timer_step_2, SLEEP_REMAINING_TIME);
    printf("Elapsed time: %u ms\tresult: %s\n", timer_step_2.elapsed_time, ret_2 == ERROR_CODE_OK ? "\u2705 SUCCESS" : "\u274C FAILED");
    printf("----------------\n");
    printf("\u23F1 Total elapsed time: %u ms\n", get_current_time_impl() - loop_start_time);

    return 0;
}