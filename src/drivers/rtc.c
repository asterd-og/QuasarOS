#include <drivers/rtc.h>

#define RTC_PORT 0x70
#define RTC_PORT_REPLY 0x71

// RTC credits: ilobilo (https://github.com/ilobilo/)

u64 bcd_to_bin(u64 value) {
    return (value >> 4) * 10 + (value & 15);
}

u64 rtc_year() {
    outb(RTC_PORT, 0x09);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_month() {
    outb(RTC_PORT, 0x08);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_day() {
    outb(RTC_PORT, 0x07);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_hour() {
    outb(RTC_PORT, 0x04);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_minute() {
    outb(RTC_PORT, 0x02);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_second() {
    outb(RTC_PORT, 0x00);
    return bcd_to_bin(inb(RTC_PORT_REPLY));
}

u64 rtc_time() {
    return rtc_hour() * 3600 + rtc_minute() * 60 + rtc_second();
}

void rtc_sleep(u64 seconds) {
    u64 last_sec = rtc_time() + seconds;
    while (last_sec != rtc_time());
}