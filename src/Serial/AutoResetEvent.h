/*
 * AutoResetEvent.h
 *
 *  Created on: 2014-11-09
 *      Author: Étienne Dupéré Larivière
 */

#ifndef AUTORESETEVENT_H_
#define AUTORESETEVENT_H_

#include <pthread.h>
#include <stdio.h>

class AutoResetEvent
{
public:
   AutoResetEvent(bool initial);
  ~AutoResetEvent();

  void set();
  void reset();
  bool waitOne(int timeout);

private:
  bool flag;
  timespec ts;
  pthread_mutex_t protect;
  pthread_cond_t signal;
};

#endif /* AUTORESETEVENT_H_ */
