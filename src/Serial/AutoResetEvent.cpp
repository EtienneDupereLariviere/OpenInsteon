/*
 * AutoResetEvent.cpp
 *
 *  Created on: 2014-11-09
 *      Author: Étienne Dupéré Larivière
 */

#include "AutoResetEvent.h"
#include <errno.h>


AutoResetEvent::AutoResetEvent(bool initial)
{
  this->flag = initial;
  pthread_mutex_init(&protect, NULL);
  pthread_cond_init(&signal, NULL);
}

void AutoResetEvent::set()
{
  pthread_mutex_lock(&protect);
  flag = true;
  pthread_mutex_unlock(&protect);
  pthread_cond_signal(&signal);
}

void AutoResetEvent::reset()
{
  pthread_mutex_lock(&protect);
  flag = false;
  pthread_mutex_unlock(&protect);
}

bool AutoResetEvent::waitOne(int timeout)
{
  pthread_mutex_lock(&protect);

  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += (timeout / 1000);
  ts.tv_nsec += (timeout % 1000) * 1000000;

  int rc = 0;
  while( !flag && rc != ETIMEDOUT)
	  rc = pthread_cond_timedwait(&signal, &protect, &ts);

  flag = false; // waiting resets the flag
  pthread_mutex_unlock(&protect);

  if(rc == ETIMEDOUT)
	  return false;
  else
	  return true;
}

AutoResetEvent::~AutoResetEvent()
{
  pthread_mutex_destroy(&protect);
  pthread_cond_destroy(&signal);
}

