/*
 * Timer.h
 *
 *  Created on: 2014-11-15
 *      Author: etienne
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

class Timer {
public:
	typedef boost::asio::deadline_timer timer;
	typedef timer::duration_type duration;
	typedef boost::function<void (boost::system::error_code, Timer&)> handler_function;

	Timer(boost::asio::io_service& io_service, duration interval, handler_function handler);

	void start();
	void continuePeriod();
	void stop();
private:
	timer impl;
	duration interval;
	handler_function handler;
	bool timerStart;
};

#endif /* TIMER_H_ */
