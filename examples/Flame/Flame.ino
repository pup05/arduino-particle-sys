/*
 * Copyright (C) 2013 Gilad Dayagi.  All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

/*
  * An example for the Arduino particle system library
  * A wandering particle that emits colorful smoke
  *
  * Note: this example uses the colorduino library becuse that is what I had,
  * but any device that supports setting a pixel to an RGB value can be used
  */

//'r' for rainbowduino, 'c' for colorduino, 'm' for SmartMatrix
#define BOARD 'm'

#if BOARD == 'c'
#include <Colorduino.h>
#endif
#if BOARD == 'r'
#include <Rainbowduino.h>
#endif
#if BOARD == 'm'
#include <SmartMatrix.h>
#include <FastLED.h>
SmartMatrix matrix;
#endif

#include "ParticleSys.h"
#include "Particle_Std.h"
#include "Particle_Fixed.h"
#include "Emitter_Fountain.h"
#include "PartMatrix.h"

const byte numParticles = 50;
boolean pulseOn = false;

Particle_Std particles[numParticles];
Particle_Fixed source;
Emitter_Fountain emitter(0, 0, 5, &source);
ParticleSys pSys(numParticles, particles, &emitter);
PartMatrix pMatrix;

CRGB *leds;

/**
 * Render the particles into a low-resolution matrix
 */
void drawMatrix(){
    pMatrix.reset();
    pMatrix.render(particles, numParticles);
    //update the actual LED matrix
    for (byte y = 0; y < PS_PIXELS_Y; y++) {
        for (byte x = 0; x < PS_PIXELS_X; x++) {
#if BOARD == 'c'
            Colorduino.SetPixel(x, y, pMatrix.matrix[x][y].r, pMatrix.matrix[x][y].g, pMatrix.matrix[x][y].b);
#endif
#if BOARD == 'r'
            Rb.setPixelXY(PS_PIXELS_Y - y - 1, x, pMatrix.matrix[x][y].r, pMatrix.matrix[x][y].g, pMatrix.matrix[x][y].b);
#endif
#if BOARD == 'm'
            ColorRGB color = pMatrix.matrix[x][y];
            leds[XY(x, PS_PIXELS_Y - y - 1)] = CRGB(color.r, color.g, color.b);
#endif
        }
    }
}

// translates from x, y into an index into the LED array
int XY(int x, int y) {
    if (y >= MATRIX_HEIGHT) { y = MATRIX_HEIGHT - 1; }
    if (y < 0) { y = 0; }
    if (x >= MATRIX_WIDTH) { x = MATRIX_WIDTH - 1; }
    if (x < 0) { x = 0; }

    return (y * MATRIX_WIDTH) + x;
}

void setup()
{
#if BOARD == 'c'    
    Colorduino.Init(); // initialize the board

    // compensate for relative intensity differences in R/G/B brightness
    // array of 6-bit base values for RGB (0~63)
    // whiteBalVal[0]=red
    // whiteBalVal[1]=green
    // whiteBalVal[2]=blue
    byte whiteBalVal[3] = {
        36,63,7  }; // for LEDSEE 6x6cm round matrix
    Colorduino.SetWhiteBal(whiteBalVal);
#endif
#if BOARD == 'r'  
    Rb.init();
#endif   
#if BOARD == 'm'
    matrix.begin();
    matrix.setColorCorrection(cc24);
    matrix.setBrightness(255);
    matrix.fillScreen(CRGB::Black);
#endif

    randomSeed(analogRead(0));

    //source.vx = 3;
    //source.vy = 1;
    source.x = 112;
    source.y = 1;
    Emitter_Fountain::minLife = 20;
    Emitter_Fountain::maxLife = 80;
    Particle_Std::ay = 1;
    //PartMatrix::isOverflow = false;

    //init all pixels to zero
    pMatrix.reset();
}

void loop()
{
#if BOARD == 'm'
    leds = matrix.backBuffer();
#endif

    pSys.update();
    drawMatrix();
#if BOARD == 'c'    
    Colorduino.FlipPage();
#endif
#if BOARD == 'm'
    matrix.swapBuffers();
#endif
    LEDS.delay(40);
    //delay(40);
}


