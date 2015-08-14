#ifndef FUNC_H
#define FUNC_H
#include "record_Save_Process_PlayHeader.h"
#include <assert.h>


int audioPlay( void* data );
int Display( void* data );
void DrawWaves();
void DrawWaveOnTop();
void DrawWavesOnButton();
void SetPoints();
void IdelFunction();
void DisplayFunction();
void ReshapeFunction( int w, int h );
void InitializeGraphics();
void KeyboradFunction( unsigned char key, int x, int y );


#endif