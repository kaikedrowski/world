#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#if defined(_WIN32)
#include <conio.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <direct.h>
#endif

inline std::string getCurrentDateTime(std::string s)
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    if (s == "now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if (s == "date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return std::string(buf);
};

inline void Logger(std::string logMsg)
{

    int check;
    std::string path = "logs";

#if defined(_WIN32)
    check = _mkdir(path.c_str());
#else
    check = mkdir(path.c_str(), 0777); // notice that 777 is different than 0777
#endif

    // check if directory is created or not
    if (check)
    {
        printf("Unable to create directory, may already exist\n");
    }

    std::string filePath = ".\\logs\\log_" + getCurrentDateTime("date") + ".txt";
    std::string now = getCurrentDateTime("now");
    std::ofstream ofs(filePath.c_str(), std::ios::out | std::ios::app);
    if (!ofs)
    {
        std::string err = strerror(errno);
        std::cerr << "Cannot open the output file." << strerror(errno) << std::endl;
        // exit(-1);
    }
    ofs << now << '\t' << logMsg << '\n';
    ofs.close();
}

#endif /* LOGGER_H */