#ifndef _ENCODER_H
#define _ENCODER_H

#include <stdint.h>
#include <stdbool.h>

#define PI (3.141592653589793)

void enc_init(void);
float enc1_read(void);
float enc2_read(void);
void enc_reset(void);
float enc_angles(int encoder);

#endif
