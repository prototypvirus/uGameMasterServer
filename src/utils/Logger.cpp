//
// Created by symbx on 09.09.17.
//

#include <iostream>
#include "utils/Logger.h"

namespace uGame {
    std::ofstream *Logger::_file;

    void Logger::init(const std::string &appDir) {
#ifdef _LOG2FILE
        std::string app(appDir);
        app.append("/latest.log");
        remove(app.c_str());
        Logger::_file = new std::ofstream(app);
#endif
    }

    void Logger::clean() {
#ifdef _LOG2FILE
        Logger::_file->close();
        delete Logger::_file;
#endif
    }

    void Logger::log(const std::string &msg, Logger::Severity severity) {
        switch (severity) {
            case INFO:
                write("[INFO] ", 7);
                std::cout << "[INFO] ";
                break;
            case WARNING:
                write("[WARNING] ", 10);
                std::cout << "[WARNING] ";
                break;
            case ERROR:
                write("[ERROR] ", 8);
                std::cout << "[ERROR] ";
                break;
            case DEBUG:
                write("[DEBUG] ", 8);
                std::cout << "[DEBUG] ";
                break;
        }
        write(msg.c_str(), msg.length());
        write("\r\n", 2);
        std::cout << msg << std::endl;
    }

    void Logger::info(const std::string &msg) {
        Logger::log(msg, INFO);
    }

    void Logger::warn(const std::string &msg) {
        Logger::log(msg, WARNING);
    }

    void Logger::err(const std::string &msg) {
        Logger::log(msg, ERROR);
    }

    void Logger::dbg(const std::string &msg) {
#ifdef _DEBUG
        Logger::log(msg, DEBUG);
#endif
    }

    void Logger::write(const char* msg, unsigned long len) {
#ifdef _LOG2FILE
        if(Logger::_file != NULL && Logger::_file->is_open())
            Logger::_file->write(msg, len);
#endif
    }
}