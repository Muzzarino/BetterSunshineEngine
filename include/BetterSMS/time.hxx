#pragma once

#include <Dolphin/OS.h>

namespace BetterSMS {
    namespace Time {
        const char *buildDate();
        const char *buildTime();
        OSTime ostime();
        void calendarTime(OSCalendarTime &result);
        void date(char *dst);
        void time(char *dst);
        u16 nanosecond();
        u16 microsecond();
        u16 millisecond();
        u8 second();
        u8 minute();
        u8 hour();
        u8 day();
        u8 month();
        u8 year();
        void calendarToDate(char *dst, const OSCalendarTime &calendar);
        void calendarToTime(char *dst, const OSCalendarTime &calendar);
    }  // namespace Time
}  // namespace BetterSMS::Time