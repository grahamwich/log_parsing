// Copyright 2022 Graham Stelzer

#include <iostream>
#include <fstream>

// necessary to add header into file after data is computed
#include <vector>

// from stdin_boost example
#include <regex>

// from datetime example
#include <string>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
using std::cout;
using std::cin;
using std::endl;
using std::string;
using boost::gregorian::date;
using boost::gregorian::from_simple_string;
using boost::gregorian::date_period;
using boost::gregorian::date_duration;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::posix_time::hours;
using boost::posix_time::minutes;
using boost::posix_time::seconds;
using boost::posix_time::milliseconds;


time_duration CalcBootTime(
    std::string date1, std::string date2, std::string time1, std::string time2);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    string fileName = argv[1];

    // take in log file
    // create regex to test for start of log, search for log.c.166
    static const std::regex regStart("(log.c.166)");
    static const std::regex regSuccess(
        "oejs.AbstractConnector:Started SelectChannelConnector");

    std::string currLine = "";

    int startCount = 0;
    int lineCount = 0;
    int startLineNum;

    // use regex to find the date and time and convert to string
    std::regex regDate("(\\d{4}-\\d{2}-\\d{2})");
    std::regex regTime("(\\d{2}:\\d{2}:\\d{2})");
    // create strings to store the date and time
    std::string startDateStr = "";
    std::string startTimeStr = "";
    std::string successDateStr = "";
    std::string successTimeStr = "";
    // smatch - https://cplusplus.com/reference/regex/match_results/
    std::smatch match;

    // use counts for header:
    int bootCountTotal = 0;
    int completedCountTotal = 0;

    // output file
    std::ofstream outputFile(fileName + ".rpt");
    // to add the header file,
    // store each string a vector then write the vector to the file
    std::vector<std::string> header;
    std::ifstream logFile(fileName);

    if (logFile.is_open() && outputFile.is_open()) {
        while (getline(logFile, currLine)) {
            lineCount++;
            if (std::regex_search(currLine, regStart)) {
                startCount++;
                bootCountTotal++;

                if (startCount > 1) {
                    startCount = 1;
                    header.push_back("**** Incomplete boot **** \n");
                }

                startLineNum = lineCount;

                if (std::regex_search(currLine, match, regDate)) {
                    startDateStr = match[0];
                }
                if (std::regex_search(currLine, match, regTime)) {
                    startTimeStr = match[1];
                }

                header.push_back("=== Device boot ===");
                header.push_back(
                    std::to_string(startLineNum) +
                    "(device5_intouch.log): " +
                    startDateStr + " " + startTimeStr +
                    " Boot Start");
            }
            if (std::regex_search(currLine, regSuccess)) {
                completedCountTotal++;
                startCount = 0;

                if (std::regex_search(currLine, match, regDate)) {
                    successDateStr = match[0];
                }
                if (std::regex_search(currLine, match, regTime)) {
                    successTimeStr = match[1];
                }

                header.push_back(
                    std::to_string(lineCount) +
                    "(device5_intouch.log): " +
                    successDateStr + " " + successTimeStr +
                    " Boot Completed");

                time_duration bootTime =
                    CalcBootTime(startDateStr, successDateStr,
                                startTimeStr, successTimeStr);
                header.push_back("\tBoot time: " +
                    std::to_string(bootTime.total_milliseconds()) +
                        "ms\n");
            }
        }
    }

    // if reach EOF before finding success
    if (startCount == 1) {
        header.push_back("**** Incomplete boot **** \n");
    }

    // write header to file
    outputFile << "Device Boot Report" << endl << endl;
    outputFile << "Intouch log file: " << fileName << endl;
    outputFile << "Lines Scanned: " << lineCount << endl << endl;
    outputFile << "Device boot count: initiated = " <<
        bootCountTotal << ", completed: " <<
        completedCountTotal << endl << endl << endl;

    // write vector to file:
    for (auto s : header) {
        outputFile << s << endl;
    }

    // close files
    logFile.close();
    outputFile.close();

    return 0;
}

time_duration CalcBootTime(
    std::string date1, std::string date2,
    std::string time1, std::string time2) {
    date d1 = from_simple_string(date1);
    date d2 = from_simple_string(date2);

    ptime t1(d1, time_duration(hours(std::stoi(time1.substr(0, 2))) +
        minutes(std::stoi(time1.substr(3, 2))) +
        seconds(std::stoi(time1.substr(6, 2)))));
    ptime t2(d2, time_duration(hours(std::stoi(time2.substr(0, 2))) +
        minutes(std::stoi(time2.substr(3, 2))) +
        seconds(std::stoi(time2.substr(6, 2)))));
    return t2 - t1;
}
