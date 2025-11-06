/*
 * IODevices.h
 *
 *  Created on: Nov 6, 2025
 *      Author: connor15
 */

#ifndef INC_IODEVICES_H_
#define INC_IODEVICES_H_

#include "IOManager.h"

void testIORegister();

int testIOPollFunction(io_abs* io);

unsigned int testIOGetValue(io_abs* io);


#endif /* INC_IODEVICES_H_ */
