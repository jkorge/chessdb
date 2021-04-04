#ifndef LOG_H
#define LOG_H

// IO headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

// Type structs
#include <string>
#include <map>

// OS Utils
#include <unistd.h>     // getpid
#include <filesystem>

// Custom headers
#include "tempus.hpp"

namespace logging{

    // Log level enum
    typedef enum LEVEL {
        ALL = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        NONE
    } LEVEL;

    // Map levels to strings for writing to ostreams
    const std::map<LEVEL, std::string> lstrings = {
        {ALL, "ALL"},
        {DEBUG, "DEBUG"},
        {INFO, "INFO"},
        {WARN, "WARN"},
        {ERROR, "ERROR"}
    };

    const char dt_spec[7] = "%Y%m%d";
    const char tm_spec[7] = "%H%M%S";
    const char * const dttm_spec[2] = {dt_spec, tm_spec};

    // logs/YYYYmmddTHHMMSS/
    const std::filesystem::path _log_dir = std::filesystem::path("logs/").append(Tempus::strdatetime(dttm_spec));

    class LoggerBase{

        // Container for log txt and call stack details
        struct loc{

            // Template type for _txt allows std::string and C-string/string-literal
            template<class Source>
            loc(
                const Source& _txt,
                const char *_dttm=Tempus::strdatetime().c_str(),
                const char *_func=__builtin_FUNCTION(),
                unsigned int _lineno=__builtin_LINE()
            ) noexcept : txt(_txt), dttm(_dttm), func(_func), lineno(_lineno) {}

            std::string txt;
            const std::string dttm, func;
            unsigned int lineno;
        };

        // separates level/time block from process info block in log text
        const std::string sep = "<->";

    public:
        LEVEL minl;
        std::string filename;

        LoggerBase(LEVEL lvl, short pid, std::filesystem::path _srcf, bool log_to_console=false, int cnt=0) : minl(lvl), _pid(pid), _log_to_console(log_to_console) {
            
            this->_srcfile = _srcf.string();
            _srcf = _srcf.replace_extension("").filename();
            _srcf += std::to_string(cnt);
            _srcf = _srcf.replace_extension(".log");
            
            // logs/YYYYmmddTHHMMSS/<srcfile_stem>.log
            if(this->minl != NONE){ this->mkdirs(_log_dir); }
            this->filename = (_log_dir / _srcf).string();

            // log "true/false" instead of "1/0"
            this->_f_log.setf(std::ios_base::boolalpha);

        }
        ~LoggerBase(){
            if(this->is_open()){ this->_os.flush(); }
            this->close();
        }

        template<typename... Ts>
        void logb(loc& fmt, LEVEL lvl, Ts&&... Args){

            if(sizeof...(Args)){
                std::stringstream tmp(fmt.txt, std::ios::out | std::ios::app);
                this->concat(tmp, Args...);
                fmt.txt = tmp.str();
            }

            this->format(fmt, lvl);
            this->out();
        }

        void open(){ this->_os.open(this->filename); }

        void close(){ if(this->is_open()){ this->_os.close(); } }

    private:

        std::string _srcfile;
        short _pid;
        bool _log_to_console;
        std::stringstream _f_log;
        std::ofstream _os;

        void mkdirs(const std::filesystem::path& d){ if(!std::filesystem::is_directory(d)){ std::filesystem::create_directories(d); } }

        void write(std::ostream& ostrm=std::cout){ ostrm << this->_f_log.str(); ostrm.flush(); }

        void format(loc& fmt, LEVEL lvl){

            // [Level] Timestamp <-> PID [File:Function:LineNo] <log_text>
            this->_f_log

            // [Level]
                            << "[" << std::setw(5) << std::left << lstrings.at(lvl) << "] "
            // Timestamp
                            << fmt.dttm << " "
            // "<->"
                            << this->sep << " "
            // PID
                            << std::setw(5) << std::right << this->_pid << " "
            // [File:Function:LineNo]
                            << "[" << this->_srcfile << ":" << fmt.func << ":" << fmt.lineno << "] "
            // <log_text>
                            << fmt.txt << std::endl;
        }

        void clear(){
            this->_f_log.str(std::string());
            this->_f_log.clear();
        }

        bool is_open(){ return this->_os.is_open(); }
        void out(){
            if(this->_os.is_open()){ this->write(this->_os); }      // File
            if(this->_log_to_console){ this->write(std::cout); }    // Console
        }

        void concat(std::stringstream &tmp){}

        template<typename T, typename... Ts>
        void concat(std::stringstream& tmp, T& val, Ts&&... Args){
            tmp << " " << val;
            if(sizeof...(Args)){ this->concat(tmp, Args...); }
        }

        friend class Logger;
    };




    /*
        LOGGER
            Writes to <_srcfile> log file (eg. Logger instance in `foo.cpp` writes to `foo.log`)
            Optional runtime arg `conlog` enables logging to stderr
    */
    class Logger : public LoggerBase{

        template<typename... Ts>
        void log(loc fmt, LEVEL lvl, Ts&&... Args){
            // Format and write
            this->logb(fmt, lvl, Args...);
            this->clear();
        }

    public:

        Logger(int cnt=0, LEVEL lvl=INFO, bool conlog=false, const char* _srcfile= __builtin_FILE()) : LoggerBase(lvl, getpid(), _srcfile, conlog, cnt){
            this->minl = lvl;
            if(this->minl != NONE){ this->open(); }
        }

        template<typename... Ts>
        void debug(loc fmt, Ts&&... Args){ if(DEBUG >= this->minl){ this->log(fmt, DEBUG, Args...); } }

        template<typename... Ts>
        void info(loc fmt, Ts&&... Args){ if(INFO >= this->minl){ this->log(fmt, INFO, Args...); } }

        template<typename... Ts>
        void warn(loc fmt, Ts&&... Args){ if(WARN >= this->minl){ this->log(fmt, WARN, Args...); } }

        template<typename... Ts>
        void error(loc fmt, Ts&&... Args){ if(ERROR >= this->minl){ this->log(fmt, ERROR, Args...); } }

    };
}

#endif