#include "include/tempus.hpp"

namespace Tempus{

    // Get time_point instance from Clock
    std::chrono::time_point<Clock> read_clock(){ return Clock::now(); }

    // read_clock + time_point_cast
    template<typename TICK>
    Time<TICK> get_time_point(){ return std::chrono::time_point_cast<TICK>(read_clock()); }

    // get_time_point + return ticks since epoch
    template<typename TICK>
    TICK get_time(){ return get_time_point<TICK>().time_since_epoch(); }

    // Get time as number of ticks since epoch
    template<typename TICK>
    typename TICK::rep time(){ return get_time<TICK>().count(); }

    template<typename TICK>
    std::string strtime(const char *spec){

        char tm[n_time_chars];
        std::string nstr;
        long n;
        std::time_t t;
        TICK prec(1);

        if(prec >= day){ return std::string(midnight); }

        // Duration since epoch
        TICK T = get_time<TICK>();

        // Compute fractional seconds (if required)
        if(prec >= sec){ n = 0; }
        else{ n = T.count() % TICK::period::den; }

        // Time in seconds since epoch
        if(prec != sec){ t = std::chrono::duration_cast<Second>(T).count(); }
        else{ t = T.count(); }

        // Write to tm according to spec
        if(prec < day){ std::strftime(tm, n_time_chars, spec, std::localtime(&t)); }
        else{ std::strftime(tm, n_time_chars, spec, std::gmtime(&t)); }

        // Append fractional seconds
        nstr = std::to_string(n);
        return std::string(tm) + "." + std::string(n_dec_chars - nstr.length(), '0') + nstr;
        
    }

    std::string strtime(unsigned long long t0, const char *spec){

        char tm[n_time_chars];
        std::string nstr;

        unsigned long n = t0 % std::nano::den;
        Nanosecond T(t0 - n);
        int64_t t = std::chrono::duration_cast<Second>(T).count();

        nstr = std::to_string(n);
        nstr = std::string(n_dec_chars - nstr.length(), '0') + nstr;

        std::strftime(tm, n_time_chars, spec, std::gmtime(&t));

        return std::string(tm) + "." + nstr;

    }

    std::string strdate(const char *spec){
        char dt[n_date_chars];
        std::time_t d = get_time<Second>().count();
        std::strftime(dt, n_date_chars, spec, std::gmtime(&d));
        return std::string(dt);
    }

    template<typename TICK>
    std::string strdatetime(const char * const *spec, char sep){ return strdate(spec[0]) + sep + strtime<TICK>(spec[1]); }
}


template Tempus::Time<Tempus::Nanosecond> Tempus::get_time_point();
template Tempus::Time<Tempus::Microsecond> Tempus::get_time_point();
template Tempus::Time<Tempus::Millisecond> Tempus::get_time_point();
template Tempus::Time<Tempus::Second> Tempus::get_time_point();
template Tempus::Time<Tempus::Minute> Tempus::get_time_point();
template Tempus::Time<Tempus::Hour> Tempus::get_time_point();
template Tempus::Time<Tempus::Day> Tempus::get_time_point();
template Tempus::Time<Tempus::Year> Tempus::get_time_point();

template Tempus::Nanosecond Tempus::get_time();
template Tempus::Microsecond Tempus::get_time();
template Tempus::Millisecond Tempus::get_time();
template Tempus::Second Tempus::get_time();
template Tempus::Minute Tempus::get_time();
template Tempus::Hour Tempus::get_time();
template Tempus::Day Tempus::get_time();
template Tempus::Year Tempus::get_time();

template typename Tempus::Nanosecond::rep Tempus::time<Tempus::Nanosecond>();
template typename Tempus::Microsecond::rep Tempus::time<Tempus::Microsecond>();
template typename Tempus::Millisecond::rep Tempus::time<Tempus::Millisecond>();
template typename Tempus::Second::rep Tempus::time<Tempus::Second>();
template typename Tempus::Minute::rep Tempus::time<Tempus::Minute>();
template typename Tempus::Hour::rep Tempus::time<Tempus::Hour>();
template typename Tempus::Day::rep Tempus::time<Tempus::Day>();
template typename Tempus::Year::rep Tempus::time<Tempus::Year>();

template std::string Tempus::strtime<Tempus::Nanosecond>(const char*);
template std::string Tempus::strtime<Tempus::Microsecond>(const char*);
template std::string Tempus::strtime<Tempus::Millisecond>(const char*);
template std::string Tempus::strtime<Tempus::Second>(const char*);
template std::string Tempus::strtime<Tempus::Minute>(const char*);
template std::string Tempus::strtime<Tempus::Hour>(const char*);
template std::string Tempus::strtime<Tempus::Day>(const char*);
template std::string Tempus::strtime<Tempus::Year>(const char*);

template std::string Tempus::strdatetime<Tempus::Nanosecond>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Microsecond>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Millisecond>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Second>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Minute>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Hour>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Day>(const char * const *, char);
template std::string Tempus::strdatetime<Tempus::Year>(const char * const *, char);
