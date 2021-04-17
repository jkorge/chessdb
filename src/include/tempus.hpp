#ifndef TEMPUS_H
#define TEMPUS_H

#include <cstdio>
#include <ctime>
#include <string>
#include <ratio>
#include <chrono>

using namespace std::chrono_literals;

namespace Tempus{

    typedef std::chrono::system_clock Clock;

    template<typename LONGTYPE, typename TICKSIZE>
    using Tick = std::chrono::duration<LONGTYPE, TICKSIZE>;

    template<typename TICK>
    using Time = std::chrono::time_point<Clock, TICK>;

    typedef std::chrono::duration<uint64_t, std::nano> Nanosecond;
    typedef std::chrono::duration<int32_t, std::micro> Microsecond;
    typedef std::chrono::duration<int32_t, std::milli> Millisecond;
    typedef std::chrono::duration<int32_t, std::ratio<1> > Second;
    typedef std::chrono::duration<int32_t, std::ratio<60> > Minute;
    typedef std::chrono::duration<int32_t, std::ratio<3600> > Hour;
    typedef std::chrono::duration<int32_t, std::ratio<86400> > Day;
    typedef std::chrono::duration<int32_t, std::ratio<31556952> > Year;

    /*
        DURATION INSTANCES
            Used as precision specifiers (`prec`)
    */

    const Nanosecond nsec(1);
    const Microsecond usec(1);
    const Millisecond msec(1);
    const Second sec(1);
    const Minute min(1);
    const Hour hour(1);
    const Day day(1);
    const Year year(1);

    /*
        UNIT CONVERSIONS
    */

    // Fractional seconds to seconds
    constexpr uint32_t nsecs_per_sec = std::nano::den;
    constexpr uint32_t usecs_per_sec = std::micro::den;
    constexpr uint32_t msecs_per_sec = std::milli::den;

    // Seconds => Minutes => Hours => Days => Year
    constexpr uint32_t secs_per_min = 60;
    constexpr uint32_t mins_per_hour = 60;
    constexpr uint32_t hours_per_day = 24;
    constexpr uint32_t days_per_year = 365;

    // Nanoseconds to larger units
    constexpr uint32_t nsecs_per_usec = usecs_per_sec / nsecs_per_sec;
    constexpr uint32_t nsecs_per_msec = msecs_per_sec / nsecs_per_sec;
    constexpr uint32_t nsecs_per_min = nsecs_per_sec * secs_per_min;
    constexpr uint32_t nsecs_per_hour = nsecs_per_min * mins_per_hour;
    constexpr uint32_t nsecs_per_day = nsecs_per_hour * hours_per_day;
    constexpr uint32_t nsecs_per_year = nsecs_per_day * days_per_year;

    // Microseconds to larger units
    constexpr uint32_t usecs_per_msec = msecs_per_sec / nsecs_per_sec;
    constexpr uint32_t usecs_per_min = usecs_per_sec * secs_per_min;
    constexpr uint32_t usecs_per_hour = usecs_per_min * mins_per_hour;
    constexpr uint32_t usecs_per_day = usecs_per_hour * hours_per_day;
    constexpr uint32_t usecs_per_year = usecs_per_day * days_per_year;

    // Milliseconds to larger units
    constexpr uint32_t msecs_per_min = msecs_per_sec * secs_per_min;
    constexpr uint32_t msecs_per_hour = msecs_per_min * mins_per_hour;
    constexpr uint32_t msecs_per_day = msecs_per_hour * hours_per_day;
    constexpr uint32_t msecs_per_year = msecs_per_day * days_per_year;

    // Seconds to larger units
    constexpr uint32_t secs_per_hour = secs_per_min * mins_per_hour;
    constexpr uint32_t secs_per_day = secs_per_hour * hours_per_day;
    constexpr uint32_t secs_per_year = secs_per_day * days_per_year;

    // Minutes to larger units
    constexpr uint32_t mins_per_day = mins_per_hour * hours_per_day;
    constexpr uint32_t mins_per_year = mins_per_day * days_per_year;

    // Hours to larger units
    constexpr uint32_t hours_per_year = hours_per_day * days_per_year;

    /*
        FORMAT SPECS
    */

    const char dt_spec[9] = "%Y-%m-%d";
    const char tm_spec[9] = "%H:%M:%S";
    const char * const dttm_spec[2] = {dt_spec, tm_spec};
    const short int n_time_chars = 9;      // Including terminating null char ('\0'); Not including fractional seconds
    const short int n_dec_chars = 9;       // Number of decimal digits to include
    const short int n_date_chars = 11;
    const char midnight[19] = {"00:00:00.000000000"};

    /*
        FUNCS
    */

    // Get time_point instance from Clock
    std::chrono::time_point<Clock> read_clock();

    // read_clock + time_point_cast
    template<typename TICK=Nanosecond>
    Time<TICK> get_time_point();

    // read_clock + duration_cast
    template<typename TICK=Nanosecond>
    TICK get_time();

    // Get time as number of ticks since epoch
    template<typename TICK=Nanosecond>
    typename TICK::rep time();

    template<typename TICK=Nanosecond>
    std::string strtime(const char *spec=tm_spec);

    std::string strtime(uint64_t t0, const char *spec=tm_spec);

    std::string strdate(const char *spec=dt_spec);

    template<typename TICK=Nanosecond>
    std::string strdatetime(const char * const *spec=dttm_spec, char sep='T');

}

#endif