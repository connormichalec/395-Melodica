/*
 * audio.h
 *
 *  Created on: Mar 31, 2025
 *      Author: Connor Michalec
 */


#ifndef INC_AUDIO_H_
#define INC_AUDIO_H_

void audio_signal_init(void * _htim8, void * _hdac);

/**
 * Main loop for audio signal generation.
 */
void audio_signal_loop();



#endif /* INC_AUDIO_H_ */
