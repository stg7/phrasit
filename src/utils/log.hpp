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

#define LOGINFO(msg) LOGEX(linfo, "[INFO ] ", msg)
#define LOGMSG(msg) LOGEX(lnone, "[MSG  ] ", msg)
#define LOGERROR(msg) LOGEX(lerror, "[ERROR] ", msg)
#define LOGWARN(msg) LOGEX(lwarning, "[WARN ] ", msg)
#define LOGDEBUG(msg) LOGEX(ldebug, "[DEBUG] ", msg)


// LOG makro
#define LOG(level, msg) {                                                \
    phrasit::utils::Log::getInstance().log(level, FILENAME, __LINE__) \
        << msg                                                           \
        << " \033[0m"                                                    \
        << std::endl;                                                    \
}
#define LOGEX(level, type, msg) {                                        \
    phrasit::utils::Log::getInstance().log(level, FILENAME, __LINE__) \
        << type                                                          \
        << msg                                                           \
        << " \033[0m"                                                    \
        << std::endl;                                                    \
}

#define DISABLE_LOG() {                             \
    phrasit::utils::Log::getInstance().disable();\
}
#define ENABLE_LOG() {                              \
    phrasit::utils::Log::getInstance().enable(); \
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
                _colorValues[lnew] = "\033[0;32m";  // green
                _colorValues[lerror] = "\033[0;31m";  // red
                _colorValues[lwarning] = "\033[0;33m";  // yellow
                _colorValues[linfo] = "\033[0;35m";  // purple
                _colorValues[ldebug] = "\033[0;34m";  // blue
                _colorValues[lnone] = "\033[1;36m";  // cyan
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

            std::ostream& log(int level, const char* file, int line) {
                if (!_enabled) {
                    null_stream.str("");  // TODO: do it in a better way
                    return null_stream;
                }

                *_output << std::left << _colorValues[lnone]
                        << "[" <<  file << "@" << std::setw(3)
                        << std::right
                        << line
                        << std::setw(17 - strlen(file) - 4) << " "
                        << "] " << "\033[0m" << " "
                        << _colorValues[level] << std::left
                        << std::left;
                return *_output;
            }
        };
    }
}
#endif
