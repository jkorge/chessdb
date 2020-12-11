#ifndef TEMPUS_H
#define TEMPUS_H

#include <cstdio>
#include <ctime>
#include <ratio>
#include <string>
#include <chrono>

typedef unsigned long ulong;
typedef unsigned long long ullong;
typedef std::string str;

using Clock = std::chrono::system_clock;

template<typename LONGTYPE, typename TICKSIZE>
using Tick = std::chrono::duration<LONGTYPE, TICKSIZE>;

template<typename TICK>
using Time = std::chrono::time_point<Clock, TICK>;


using Nanosecond = std::chrono::duration<ullong, std::nano>;
using Microsecond = std::chrono::duration<long, std::micro>;
using Millisecond = std::chrono::duration<long, std::milli>;
using Second = std::chrono::duration<long, std::ratio<1> >;
using Minute = std::chrono::duration<long, std::ratio<60> >;
using Hour = std::chrono::duration<long, std::ratio<3600> >;
using Day = std::chrono::duration<long, std::ratio<86400> >;
using Year = std::chrono::duration<long, std::ratio<31556952> >;


namespace Tempus{

    using namespace std::chrono_literals;

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
    constexpr ulong nsecs_per_sec = std::nano::den;
    constexpr ulong usecs_per_sec = std::micro::den;
    constexpr ulong msecs_per_sec = std::milli::den;

    // Seconds => Minutes => Hours => Days => Year
    constexpr ulong secs_per_min = 60;
    constexpr ulong mins_per_hour = 60;
    constexpr ulong hours_per_day = 24;
    constexpr ulong days_per_year = 365;

    // Nanoseconds to larger units
    constexpr ulong nsecs_per_usec = usecs_per_sec / nsecs_per_sec;
    constexpr ulong nsecs_per_msec = msecs_per_sec / nsecs_per_sec;
    constexpr ulong nsecs_per_min = nsecs_per_sec * secs_per_min;
    constexpr ulong nsecs_per_hour = nsecs_per_min * mins_per_hour;
    constexpr ulong nsecs_per_day = nsecs_per_hour * hours_per_day;
    constexpr ulong nsecs_per_year = nsecs_per_day * days_per_year;

    // Microseconds to larger units
    constexpr ulong usecs_per_msec = msecs_per_sec / nsecs_per_sec;
    constexpr ulong usecs_per_min = usecs_per_sec * secs_per_min;
    constexpr ulong usecs_per_hour = usecs_per_min * mins_per_hour;
    constexpr ulong usecs_per_day = usecs_per_hour * hours_per_day;
    constexpr ulong usecs_per_year = usecs_per_day * days_per_year;

    // Milliseconds to larger units
    constexpr ulong msecs_per_min = msecs_per_sec * secs_per_min;
    constexpr ulong msecs_per_hour = msecs_per_min * mins_per_hour;
    constexpr ulong msecs_per_day = msecs_per_hour * hours_per_day;
    constexpr ulong msecs_per_year = msecs_per_day * days_per_year;

    // Seconds to larger units
    constexpr ulong secs_per_hour = secs_per_min * mins_per_hour;
    constexpr ulong secs_per_day = secs_per_hour * hours_per_day;
    constexpr ulong secs_per_year = secs_per_day * days_per_year;

    // Minutes to larger units
    constexpr ulong mins_per_day = mins_per_hour * hours_per_day;
    constexpr ulong mins_per_year = mins_per_day * days_per_year;

    // Days to larger units
    constexpr ulong hours_per_year = hours_per_day * days_per_year;

    /*
        FORMAT SPECS
    */

    const char dt_spec[9] = "%Y-%m-%d";
    const char tm_spec[9] = "%H:%M:%S";
    const char *dttm_spec[2] = {dt_spec, tm_spec};
    const short int n_time_chars = 9;      // Including terminating (null) char: '\0'; Not including fractional seconds
    const short int n_dec_chars = 9;       // Number of decimal digits to include
    const short int n_date_chars = 11;
    const char midnight[19] = {"00:00:00.000000000"};

    /*
        FUNCS
    */

    // Get time_point instance from Clock
    std::chrono::time_point<Clock> read_clock(){ return Clock::now(); }

    // Cast time_point `tp` to a time_point with a different duration
    template<
        typename FROMTICK,
        typename TOLONGTYPE,
        typename TOTICKSIZE
    >
    Time<Tick<TOLONGTYPE, TOTICKSIZE> > time_point_cast(
        Time<FROMTICK> tp,
        Tick<TOLONGTYPE, TOTICKSIZE> to_duration
    ){ return std::chrono::time_point_cast<Tick<TOLONGTYPE, TOTICKSIZE> >(tp); }

    // read_clock + time_point_cast
    template<typename TICK>
    Time<TICK> get_time_point(TICK prec){ return time_point_cast(read_clock(), prec); }

    // Cast duration to a different tick size
    template<
        typename FROMLONGTYPE,
        typename TOLONGTYPE,
        typename FROMTICKSIZE,
        typename TOTICKSIZE
    >
    Tick<TOLONGTYPE, TOTICKSIZE> duration_cast(
        Tick<FROMLONGTYPE, FROMTICKSIZE> elapsed,
        Tick<TOLONGTYPE, TOTICKSIZE> to_duration
    ){ return std::chrono::duration_cast<Tick<TOLONGTYPE, TOTICKSIZE> >(elapsed); }

    // read_clock + duration_cast
    template<typename LONGTYPE, typename TICKSIZE>
    Tick<LONGTYPE, TICKSIZE> get_time(Tick<LONGTYPE, TICKSIZE> prec){ return get_time_point(prec).time_since_epoch(); }

    // Get time as number of ticks since epoch, where tick size is determined by the `period` member of input `prec`
    template<typename LONGTYPE, typename TICKSIZE>
    LONGTYPE time(Tick<LONGTYPE, TICKSIZE> prec){ return get_time(prec).count(); }

    ullong time(){ return time(nsec); }

    template<typename LONGTYPE, typename TICKSIZE>
    str strtime(Tick<LONGTYPE, TICKSIZE> prec, const char *spec=tm_spec){

        char tm[n_time_chars];
        str nstr;
        long n, t;

        if(prec >= day){ return str(midnight); }

        // Duration since epoch
        Tick<LONGTYPE, TICKSIZE> T = get_time(prec);

        // Compute fractional seconds (if required)
        if(prec >= sec){ n = 0; }
        else{ n = T.count() % TICKSIZE::den; }

        // Time in seconds since epoch
        if(prec != sec){ t = duration_cast(T, sec).count(); }
        else{ t = T.count(); }

        // Write to tm according to spec
        if(prec < day){ std::strftime(tm, n_time_chars, spec, std::localtime(&t)); }
        else{ std::strftime(tm, n_time_chars, spec, std::gmtime(&t)); }

        // Append fractional seconds
        nstr = std::to_string(n);
        return str(tm) + "." + str(n_dec_chars - nstr.length(), '0') + nstr;
        
    }

    str strtime(ullong t0, const char *spec=tm_spec){

        char tm[n_time_chars];
        str nstr;

        ulong n = t0 % std::nano::den;
        Nanosecond T(t0 - n);
        long t = duration_cast(T, sec).count();

        nstr = std::to_string(n);
        nstr = str(n_dec_chars - nstr.length(), '0') + nstr;

        std::strftime(tm, n_time_chars, spec, std::gmtime(&t));

        return str(tm) + "." + nstr;

    }

    str strtime(const char *spec=tm_spec){ return strtime(nsec, spec); }

    template<typename LONGTYPE, typename TICKSIZE>
    str strdate(Tick<LONGTYPE, TICKSIZE> prec, const char *spec=dt_spec){
        char dt[n_date_chars];
        long d;

        // Duration since epoch
        // Expand precision to seconds and count
        d = duration_cast(get_time(prec), sec).count();
        // Write to dt according to dt_spec
        std::strftime(dt, n_date_chars, spec, std::gmtime(&d));
        return str(dt);
    }

    str strdate(const char *spec=dt_spec){ return strdate(day); }

    template<typename LONGTYPE, typename TICKSIZE>
    str strdatetime(Tick<LONGTYPE, TICKSIZE> prec, const char **spec, char sep='T'){ return strdate(prec, spec[0]) + sep + strtime(prec, spec[1]); }

    str strdatetime(const char **spec=dttm_spec, char sep='T'){ return strdatetime(nsec, spec, sep); }

}

#endif