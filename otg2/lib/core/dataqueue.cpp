/*
 * Copyright 2004-2010 WINLAB, Rutgers University, USA
 * Copyright 2007-2013 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "dataqueue.h"

DataQueue::DataQueue()
{
  current_ = new data;
  current_->next_ = 0;
  base_ = current_;
}

DataQueue::~DataQueue()
{
  data *databuf;
  //clear all data
  while((databuf = pop()) != 0 )
    delete databuf->payload_;
  delete current_;
}


struct data*
DataQueue::pop(){
  struct data     *temp;

  //check for existence of data
  if(base_->next_ == 0)
    return 0;                       //no data is on stack
  temp = base_;
  base_ = base_->next_;
  return temp;
}

void
DataQueue::push(char *newdata, short len)
{
  struct data *temp = new data;
  //Add new payload to waiting structure...
  current_->payload_ = (char *) malloc(len);
  memcpy(current_->payload_, newdata, len*sizeof(char));
  current_->len_ = len;
  //Add new structure to wait for new data
  current_->next_ = temp;
  current_ = temp;
  current_->next_ = 0;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
