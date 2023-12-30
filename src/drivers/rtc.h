#pragma once

#include <types.h>
#include <arch/x86_64/io.h>

u64 rtc_year();
u64 rtc_month();
u64 rtc_day();
u64 rtc_hour();
u64 rtc_minute();
u64 rtc_second();
u64 rtc_time();
void rtc_sleep(u64 seconds);