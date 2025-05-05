/*
 * debounce_notes.h
 *
 *  Created on: May 3, 2025
 *      Author: Connor Michalec
 */

/**
 * Initialize debouncer
 */
void note_debouncer_init();

/**
 * Should be called by a timer every midi update
 */
void debounce_notes_tick();

/**
 * Returns true if provided button should be handled or if it should be ignored since it there was a very recent button press.
 */
int note_debouncer(uint8_t note);
