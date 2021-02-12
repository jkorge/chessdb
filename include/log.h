#ifndef LOG_H
#define LOG_H

// IO headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

// Types
#include <string>
#include <map>

// OS Utils
#include <unistd.h>     // getpid
#include <filesystem>   // log filename/directory creation

// Custom headers
#include "tempus.h"

// Stringify Macros
#define STFY(s) #s
#define XSTFY(s) STFY(s)

/*
    Define project's __NAME__ as "root" if not defined by preprocessor directive already
    Run `g++ -Dname=project_name ...` to use "project_name" instead of "root"
    NB: Uses "root" in reference to RootLogger - the only object that uses this macro
*/

#ifndef project
    #define __NAME__ STFY(root)
#else
    #define __NAME__ XSTFY(project)
#endif

/*
    Default root logger to NOT create log file
    Run `g++ -Drflog ...` to set root logger's `log_to_file` ctor param to true
    NB: __RFLOG__=true will create file for root logger; false will not
*/

#ifndef rflog
    #define __RFLOG__ false
#else
    #define __RFLOG__ true
#endif

/*
    Default root logger to NOT log to console
    Run `g++ -Dconlog ...` to set root logger's `log_to_console` ctor param (delegated to LoggerBase) to true
    NB: __CONLOG__=true will cause all logs to be sent to stdout; false will not
*/

#ifndef conlog
    #define __CONLOG__ false
#else
    #define __CONLOG__ true
#endif

/*
    Set default log level. Root logger won't log anything less than dlvl
        e.g. dlvl=WARN => only WARNs and ERRORs printed to console (and root.log if __RFLOG__)
    Run `g++ -Ddlvl=DEBUG ...` to set default log level to DEBUG. Requires one of DEBUG, INFO, WARN, ERROR
    NB: Unrecognized values of dlvl will set default to DEBUG
*/

#ifndef dlvl
    #warning "Setting default log level to NONE"
    #define __DLVL__ NONE
#elif (dlvl != DEBUG) && (dlvl != INFO) && (dlvl != WARN) && (dlvl != ERROR)
    #warning "Unrecognized log level specified. Defaulting to NONE"
    #define __DLVL__ NONE
#else
    // #pragma message "Setting default log level to " XSTFY(dlvl)
    #define __DLVL__ dlvl
#endif

/*
  **************************************************************************************************
  **************************************************************************************************
  **************************************************************************************************
*/

void mkdirs(const std::filesystem::path& d){ if(!std::filesystem::is_directory(d)){ std::filesystem::create_directories(d); } }

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
std::map<LEVEL, std::string> lstrings{
    {ALL, "ALL"},
    {DEBUG, "DEBUG"},
    {INFO, "INFO"},
    {WARN, "WARN"},
    {ERROR, "ERROR"}
};

const char dt_spec[7] = "%Y%m%d";
const char tm_spec[7] = "%H%M%S";
const char *dttm_spec[2] = {dt_spec, tm_spec};

// logs/YYYYmmddTHHMMSS/
const std::filesystem::path _log_dir = std::filesystem::path("logs/").append(Tempus::strdatetime(dttm_spec));


/*
    LOGGERBASE
        Contains core functionality for logging
*/
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
    LEVEL minl;                                                             // Will only write logs of minl or greater
    std::string filename;                                                   // Log file name

    LoggerBase(LEVEL, short, std::filesystem::path, bool=false, int=0);     // Constructor - parses log filenames and creates log directories (if not already created)
    ~LoggerBase();                                                          // Destructor - closes file stream

    template<typename... Ts>
    void logb(loc&, LEVEL, Ts&&...);                                        // Buffer and write to output stream(s)
    void open();                                                            // Open handle to log file
    void close();                                                           // Close handle to log file

private:

    std::string _srcfile;                                           // File name of src code which constructs this object
    short _pid;                                                     // PID of calling process
    bool _log_to_console;                                           // Whether this instance should output to console
    std::stringstream _f_log;                                       // String stream for buffering/formatting log text
    std::ofstream _os;                                              // ostream to log file

    void write(std::ostream& = std::cout);                          // Write to stream (with operator<<)
    void format(loc&, LEVEL);                                       // Format text and save to _f_log
    void clear();                                                   // Empty _f_log and clear _f_log's state flags
    bool is_open();                                                 // Check if underlying file is open (calls this->_os.is_open)
    void out();                                                     // Calls this->write for all applicable ostreams (this->_os and/or std::cout)

    template<typename T, typename... Ts>
    void concat(std::stringstream&, T&, Ts&&...);                   // Adds data from parameter pack to text
    void concat(std::stringstream&);                                // Base function for recursive `concat` - does nothing

    friend class RootLogger;
    friend class Logger;
};

LoggerBase::LoggerBase(LEVEL lvl, short pid, std::filesystem::path _srcf, bool log_to_console, int cnt) : minl(lvl), _pid(pid), _log_to_console(log_to_console) {
    
    this->_srcfile = _srcf.string();
    // _srcf = _srcf.replace_extension(".log").filename();
    _srcf = _srcf.replace_extension("").filename();
    _srcf += std::to_string(cnt);
    _srcf = _srcf.replace_extension(".log");
    
    // logs/YYYYmmddTHHMMSS/<srcfile_stem>.log
    if(this->minl != NONE){ mkdirs(_log_dir); }
    this->filename = (_log_dir / _srcf).string();

    // log "true/false" instead of "1/0"
    this->_f_log.setf(std::ios_base::boolalpha);

}

LoggerBase::~LoggerBase(){
    if(this->is_open()){ this->_os.flush(); }
    this->close();
}

void LoggerBase::write(std::ostream& ostrm){ ostrm << this->_f_log.str(); ostrm.flush(); }

void LoggerBase::format(loc& fmt, LEVEL lvl){

    // [Level] Timestamp <-> PID [File:Function:LineNo] <log_text>
    this->_f_log

    // [Level]
                    << "[" << std::setw(5) << std::left << lstrings[lvl] << "] "
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

void LoggerBase::clear(){
    this->_f_log.str(std::string());
    this->_f_log.clear();
}

template<typename... Ts>
void LoggerBase::logb(loc& fmt, LEVEL lvl, Ts&&... Args){

    if(sizeof...(Args)){
        std::stringstream tmp(fmt.txt, std::ios::out | std::ios::app);
        this->concat(tmp, Args...);
        fmt.txt = tmp.str();
    }

    this->format(fmt, lvl);
    this->out();
}

void LoggerBase::out(){
    if(this->_os.is_open()){ this->write(this->_os); }      // File
    if(this->_log_to_console){ this->write(std::cout); }    // Console
}

void LoggerBase::open(){ this->_os.open(this->filename); }

void LoggerBase::close(){ if(this->is_open()){ this->_os.close(); } }

bool LoggerBase::is_open(){ return this->_os.is_open(); }

void LoggerBase::concat(std::stringstream &tmp){}

template<typename T, typename... Ts>
void LoggerBase::concat(std::stringstream& tmp, T& val, Ts&&... Args){
    tmp << " " << val;
    if(sizeof...(Args)){ this->concat(tmp, Args...); }
}


/*
    ROOTLOGGER
        Writes to console and, optionally, <program_name> log file
*/

class RootLogger : public LoggerBase{
public:
    RootLogger(LEVEL lvl=ALL, bool log_to_file=false) : LoggerBase(lvl, getpid(), __NAME__, __CONLOG__) {
        this->minl = lvl;
        if(log_to_file){ this->open(); }
    };

private:
    friend class Logger;
    void log(std::stringstream& lstream){
        this->_f_log.swap(lstream);
        this->out();
        this->clear();
    }
};


/*
    LOGGER
        Writes to <_srcfile> log file (eg. Logger instance in `foo.cpp` writes to `foo.log`)
        All logs are sent to RootLogger member first
*/

class Logger : public LoggerBase{
    
    static RootLogger rlog;

    template<typename... Ts>
    void log(loc fmt, LEVEL lvl, Ts&&... Args){
        // Format and write
        this->logb(fmt, lvl, Args...);
        // Send to root logger
        this->rlog.log(this->_f_log);
        this->clear();
    }

public:

    Logger(int cnt=0, LEVEL lvl=__DLVL__, const char *_srcfile=__builtin_FILE()) : LoggerBase(lvl, getpid(), _srcfile, false, cnt){
        this->minl = lvl;
        if(this->minl != NONE){ this->open(); }
    }

    template<typename... Ts>
    void debug(loc, Ts&&...);

    template<typename... Ts>
    void info(loc, Ts&&...);

    template<typename... Ts>
    void warn(loc, Ts&&...);

    template<typename... Ts>
    void error(loc, Ts&&...);

};

RootLogger Logger::rlog(__DLVL__, __RFLOG__);

template<typename... Ts>
inline void Logger::debug(loc fmt, Ts&&... Args){ if(DEBUG >= this->minl){ this->log(fmt, DEBUG, Args...); } }

template<typename... Ts>
inline void Logger::info(loc fmt, Ts&&... Args){ if(INFO >= this->minl){ this->log(fmt, INFO, Args...); } }

template<typename... Ts>
inline void Logger::warn(loc fmt, Ts&&... Args){ if(WARN >= this->minl){ this->log(fmt, WARN, Args...); } }

template<typename... Ts>
inline void Logger::error(loc fmt, Ts&&... Args){ if(ERROR >= this->minl){ this->log(fmt, ERROR, Args...); } }

#endif
