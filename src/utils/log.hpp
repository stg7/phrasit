/**
    part of phrasit

    \author stg7

    \brief logging module

    \date 17.08.2014

    Copyright 2015 Steve Göring
**/

#ifndef LOG_HEADER_HPP_
#define LOG_HEADER_HPP_

#include <iostream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>

#define FILENAME ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') : __FILE__ - 1) + 1)
// colored quick messages
#define RED(msg) LOG(lerror, msg)
#define GREEN(msg) LOG(lnew, msg)
#define YELLOW(msg) LOG(lwarning, msg)
#define PURPLE(msg) LOG(linfo, msg)
#define BLUE(msg) LOG(ldebug, msg)
#define CYAN(msg) LOG(lnone, msg)

#define LOGINFO(msg) LOGEX(linfo, "INFO ", msg)
#define LOGMSG(msg) LOGEX(lnew, "MSG  ", msg)
#define LOGERROR(msg) LOGEX(lerror, "ERROR", msg)
#define LOGWARN(msg) LOGEX(lwarning, "WARN ", msg)
#define LOGDEBUG(msg) LOGEX(ldebug, "DEBUG", msg)


// LOG macro
#define LOG(level, msg) {                                                    \
    phrasit::utils::Log::getInstance().log(level, FILENAME, __LINE__)        \
        << msg                                                               \
        << " \033[0m"                                                        \
        << std::endl;                                                        \
}
#define LOGEX(level, type, msg) {                                            \
    phrasit::utils::Log::getInstance().log(level, FILENAME, __LINE__, type)  \
        << msg                                                               \
        << std::setw(20) << std::right                                       \
        << " \033[0m"                                                        \
        << std::endl;                                                        \
}

#define DISABLE_LOG() {                             \
    phrasit::utils::Log::getInstance().disable();   \
}
#define ENABLE_LOG() {                              \
    phrasit::utils::Log::getInstance().enable();    \
}
// logging level
enum LogLevel {
    lnew = 0,
    lerror = 1,
    lwarning = 2,
    linfo = 3,
    ldebug = 4,
    lnone = 5
};

namespace phrasit {
    namespace utils {
        class Log {
         private:
            std::ostream* _output;
            std::stringstream null_stream;

            std::map<int, std::string> _colorValues;
            bool _enabled = true;

            Log() {
                _output = &std::cout;

                // define color values
                _colorValues[lnew] = "\033[1;92m";  // green
                _colorValues[lerror] = "\033[1;91m";  // red
                _colorValues[lwarning] = "\033[1;93m";  // yellow
                _colorValues[linfo] = "\033[1;95m";  // purple
                _colorValues[ldebug] = "\033[1;94m";  // blue
                _colorValues[lnone] = "\033[1;96m";  // cyan
            }

            Log(const Log&);
            virtual ~Log() { }

         public:
            static Log& getInstance() {
                static Log log;
                return log;
            }
            void enable() {
                _enabled = true;
            }

            void disable() {
                _enabled = false;
            }

            std::ostream& log(int level, const char* file, int line, const char* type = "") {
                if (!_enabled) {
                    null_stream.str("");
                    return null_stream;
                }

                *_output << std::left << _colorValues[level]
                        << "["
                        << type << "] "
                        << file << "@" << std::setw(3)
                        << std::right
                        << line
                        << std::setw(28 - strlen(file) - strlen(type) - 4) << " "
                        << ": "  << " "
                        << _colorValues[level] << std::left
                        << std::left;
                return *_output;
            }
        };
    }
}
#endif
