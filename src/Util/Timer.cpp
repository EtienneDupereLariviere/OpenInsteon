/*
 * Timer.cpp
 *
 *  Created on: 2014-11-15
 *      Author: etienne
 */

#include "Timer.h"

using namespace boost;

Timer::Timer(asio::io_service& io_service, duration interval, handler_function handler)
		: impl (io_service)
		, interval (interval)
        , handler (handler)
{
	timerStart = false;
}

void Timer::start()
{
	if(timerStart)
		stop();

	timerStart = true;

	impl.expires_from_now(interval);
	impl.async_wait(bind(handler, asio::placeholders::error, ref(*this)));

	thread timerThread(bind(&asio::io_service::run, &impl.get_io_service()));
}

void Timer::continuePeriod()
{
	impl.expires_from_now(interval);
	impl.async_wait(bind(handler, asio::placeholders::error, ref(*this)));
}

void Timer::stop()
{
	impl.cancel();
	impl.get_io_service().reset();
	timerStart = false;
}


