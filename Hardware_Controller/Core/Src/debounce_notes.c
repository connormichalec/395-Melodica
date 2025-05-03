/*
 * debounce_notes.c
 *
 *  Created on: May 3, 2025
 *      Author: Connor Michalec
 */

#include <string.h>
#include <stdint.h>

static uint32_t timer;
static uint8_t note_timer[128];

static int note_delay = 5;

void note_debouncer_init() {
	memset(note_timer, 0, sizeof(note_timer));
	timer = 0;
}

void debounce_notes_tick() {
	// Only update every 10 ticks for performance reasons
	timer++;
	if(timer%10==0) {
		for(int i =0; i<128;i++) {
			// Go thru all notes:
			if(note_timer[i] > 0)
				note_timer[i]--;
		}
	}
}

int note_debouncer(uint8_t note) {
	if(note_timer[note] == 0) {
		// Should toggle:
		note_timer[note] = note_delay;
		return 1;
	}
	// Still on button cooldown for that note:
	return 0;
}
