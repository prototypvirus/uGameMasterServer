//
// Created by symbx on 09.09.17.
//

#ifndef UGAMEMASTERSERVER_LOGGER_H
#define UGAMEMASTERSERVER_LOGGER_H

#include <string>
#include <fstream>

namespace uGame {
    class Logger {
    public:
        enum Severity {
            INFO,
            WARNING,
            ERROR,
            DEBUG
        };

        static void init(const std::string &appDir);

        static void clean();

        static void log(const std::string &msg, Logger::Severity severity);

        static void info(const std::string &msg);

        static void warn(const std::string &msg);

        static void err(const std::string &msg);

        static void dbg(const std::string &msg);

    protected:
        static std::ofstream *_file;

        static void write(const char* msg, unsigned long len);
    };

#define L_INFO Logger::info
#define L_WARN Logger::warn
#define L_ERR Logger::err
#define L_DBG Logger::dbg
}

#endif //UGAMEMASTERSERVER_LOGGER_H
