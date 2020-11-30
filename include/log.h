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
    Set root logger to log to console AND file by default
    Run `g++ -Drflog ...` to set root logger's `log_to_file` ctor param to true
    NB: __RFLOG__=true will create file for root logger; false will not
*/

#ifndef rflog
    #define __RFLOG__ false
#else
    #define __RFLOG__ true
#endif

/*
    Set default log level. Root logger won't log anything less than dlvl
        e.g. dlvl=WARN => only WARNs and ERRORs printed to console (and root.log if __RFLOG__)
    Run `g++ -Ddlvl=DEBUG ...` to set default log level to DEBUG. Requires one of DEBUG, INFO, WARN, ERROR
    NB: Unrecognized values of dlvl will set default to DEBUG
*/

#ifndef dlvl
    #define __DLVL__ DEBUG
#elif (dlvl != DEBUG) && (dlvl != INFO) && (dlvl != WARN) && (dlvl != ERROR)
    #warning "Unrecognized log level specified. Defaulting to DEBUG"
    #define __DLVL__ DEBUG
#else
    #define __DLVL__ dlvl
#endif

/*
  **************************************************************************************************
  **************************************************************************************************
  **************************************************************************************************
*/

namespace fs = std::filesystem;

// Log level enum
typedef enum LEVEL {
    ALL = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR
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

/*
    LOGGERBASE
        Contains core functionality for logging
*/
class LoggerBase{
protected:
    str _srcfile;                                      // File name of src code which calls this object

public:
    typedef std::string str;
    LEVEL minl;                                        // Will only write logs of minl or greater
    str filename;                                      // Log file name

    LoggerBase(short, fs::path, bool=false);           // Constructor - parses log filenames and creates log directories (if not already created)
    ~LoggerBase();                                     // Destructor - closes file stream

    template<typename... Ts>
    void logb(str, str, LEVEL, str, short, Ts...);     // Buffer and write to output stream(s). Clears buffer
    void open();                                       // Open handle to log file
    void close();                                      // Close handle to log file

private:
    static const str sep;                              // Separator string for log prefix (separates level/time block from process info block)
    static const fs::path _log_dir;
    short _pid;                                        // PID of calling process
    bool _log_to_console;                              // Whether this instance should output to console
    std::stringstream _f_log;                          // String stream for buffering/formatting log text
    std::ofstream _os;                                 // ostream to log file

    void write(std::ostream& = std::cout);             // Write to stream (with operator<<)
    void format(str, str, LEVEL, str, short);          // Format text and save to _f_log
    void clear();                                      // Empty _f_log and clear _f_log's state flags
    bool is_open();                                    // Check if underlying file is open (calls this->_os.is_open)
    void out();                                        // Calls this->write for all applicable ostreams

    template<typename T, typename... Ts>
    void concat(std::stringstream&, T, Ts...);         // Adds data from parameter pack to text
    void concat(std::stringstream&);                   // Base function for recursive `concat`

    friend class RootLogger;
    friend class Logger;
};


const str LoggerBase::sep("<->");
const fs::path LoggerBase::_log_dir = fs::path("logs/").append(Tempus::strdatetime(dttm_spec));

LoggerBase::LoggerBase(short pid, fs::path _srcf, bool log_to_console) : _pid(pid), _log_to_console(log_to_console) {
    
    // Save _srcfile path and convert to log filename
    this->_srcfile = _srcf.string();
    _srcf = _srcf.replace_extension(".log").filename();
    
    // logs/YYYYmmddHHMMSS/
    if(not fs::is_directory(this->_log_dir)){ fs::create_directories(this->_log_dir); }

    // Replace _srcfile extension with .log, remove path, and append to _log_dir
    this->filename = (this->_log_dir / _srcf).string();

    // Set _f_log to format bool values (ie. log "true/false" instead of "1/0")
    this->_f_log.setf(std::ios_base::boolalpha);
}

LoggerBase::~LoggerBase(){
    if(this->is_open()){ this->_os.flush(); }
    this->close();
}

void LoggerBase::write(std::ostream& ostrm){ ostrm << this->_f_log.str(); ostrm.flush(); }

void LoggerBase::format(str txt, str dttm, LEVEL lvl, str function, short line){

    // [Level] Timestamp <-> PID [File:Function:LineNo] <log_text>
    this->_f_log

    // [Level]
                    << "[" << std::setw(5) << std::left << lstrings[lvl] << "] "
    // Timestamp
                    << dttm << " "
    // "<->"
                    << this->sep << " "
    // PID
                    << std::setw(5) << std::right << this->_pid << " "
    // [File:Function:LineNo]
                    << "[" << this->_srcfile << ":" << function << ":" << line << "] "
    // <log_text>
                    << txt << std::endl;
}

void LoggerBase::clear(){
    this->_f_log.str(str());
    this->_f_log.clear();
}

template<typename... Ts>
void LoggerBase::logb(str txt, str dttm, LEVEL lvl, str function, short line, Ts... Args){

    if(sizeof...(Args)){
        std::stringstream tmp(txt, std::ios::out | std::ios::app);
        this->concat(tmp, Args...);
        txt = tmp.str();
    }
    // Format log text
    this->format(txt, dttm, lvl, function, line);

    // Write to output stream(s)
    this->out();
}

void LoggerBase::out(){
    // File
    if(this->_os.is_open()){ this->write(this->_os); }
    // Console
    if(this->_log_to_console){ this->write(std::cout); }
}

void LoggerBase::open(){ this->_os.open(this->filename); }

void LoggerBase::close(){ if(this->is_open()){ this->_os.close(); } }

bool LoggerBase::is_open(){ return this->_os.is_open(); }

void LoggerBase::concat(std::stringstream &tmp){}

template<typename T, typename... Ts>
void LoggerBase::concat(std::stringstream &tmp, T val, Ts... Args){
    tmp << " " << val;
    if(sizeof...(Args)){ this->concat(tmp, Args...); }
}


/*
    ROOTLOGGER
        Writes to console and, optionally, <program_name> log file
*/

class RootLogger : public LoggerBase{
public:
    RootLogger(bool log_to_file=false) : LoggerBase(getpid(), __NAME__, true) {
        this->minl = ALL;
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
        Writes to <_srcfile> log file (eg. Logger instance in `dep.cpp` writes to `dep.log`)
        All logs are sent to RootLogger member first
*/

class Logger : public LoggerBase{
    
    // Single RootLogger instance shared by all instances of Logger
    static RootLogger rlog;

public:

    Logger(str _srcfile, LEVEL lvl=__DLVL__) : LoggerBase(getpid(), _srcfile, false){
        this->minl = lvl;
        this->open();
    }

    template<typename... Ts>
    void log(str txt, str dttm, LEVEL lvl, str function, short line, Ts... Args){
        // Format and write
        this->logb(txt, dttm, lvl, function, line, Args...);
        // Send to root logger
        if(lvl >= this->minl){ this->rlog.log(this->_f_log); }
        this->clear();
    }

};

RootLogger Logger::rlog(__RFLOG__);


/*
    LOGGERWRAPPER
        Container for Logger class which collects calling-code's info (function name, line number) for use in BaseLogger::logb
        *** #define directives below class declaration make this work ***
*/

class LoggerWrapper{

    Logger _log;

public:

    str _function, _dttm;
    short _line;
    LEVEL _lvl;

    LoggerWrapper(str _srcfile, LEVEL lvl=__DLVL__) : _log(_srcfile, lvl) {}

    LoggerWrapper& logw(str dttm, str function, short line, LEVEL lvl){
        this->_dttm = dttm;
        this->_function = function;
        this->_line = line;
        this->_lvl = lvl;
        return *this;
    }

    template<typename... Ts>
    void operator()(str txt, Ts... Args){ this->_log.log(txt, this->_dttm, this->_lvl, this->_function, this->_line, Args...); }
};

// Replace `Logger` token with `LoggerWrapper`
#undef Logger
#define Logger LoggerWrapper

// Replace `debug`, `info`, etc. names with call to logw using function and line macros
#define debug logw(Tempus::strdatetime(), __FUNCTION__, __LINE__, DEBUG)
#define info logw(Tempus::strdatetime(), __FUNCTION__, __LINE__, INFO)
#define warn logw(Tempus::strdatetime(), __FUNCTION__, __LINE__, WARN)
#define error logw(Tempus::strdatetime(), __FUNCTION__, __LINE__, ERROR)




/*
BREAKDOWN OF INSTANTIATION AND USAGE OF LOGGER CLASS:

    Logger log(__FILE__);
        => `Logger` gets replaced with `LoggerWrapper`

    log.info("some text");
        => log.logw(Tempus::strdatetime(), __FUNCTION__, __LINE__, INFO)("some text")
            => log("some text")
                => log._log.log("some text", INFO, <retval of __FUNCTION__>, <retval of __LINE__>)
                    => logb("some text", INFO, <retval of __FUNCTION__>, <retval of __LINE__>)
                    => rlog.log(this->_f_log) // swaps this loggers sstream with root logger sstream
                    


TODO:
Logger log => LoggerWrapper log(__FILE__)
    ie. Seek to instantiate logger without having to pass __FILE__ macro

*/

#endif
