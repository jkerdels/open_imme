#ifndef SOUND_H
#define SOUND_H

void imme_sound_init(void);


// plays a breathing sound
void imme_breathe(void);

// has to be called regularly
void imme_sound_trigger(void);

#endif
