/*
 * Log.h
 *
 *  Created on: 2014-10-29
 *      Author: etienne
 */

#ifndef LOG_H_
#define LOG_H_

#include <string>

using namespace std;


enum Severity_level
{
	trace,
	debug,
    info,
    warning,
    error,
    critical
};

class Log
{
public:
	static void open();
	static void writeLine(Severity_level level, string message);

private:
};

#endif /* LOG_H_ */
