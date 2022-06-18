/*
 * DrawingAlphabetLib.c
 *
 * Created: 17.06.2022 15:46:42
 *  Author: ruwen
 */ 



#include "DrawingAlphabetLib.h"





void drawMLetter(StepperMotionAxisController* controller)
{
	// Fahrt zur Start Linie
	SMAC_ADD_MOVE_Z_max233(controller, 3, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(controller, 2, Y_UP);
	
	
	SMAC_ADD_MOVE_X_max82(controller, 28, X_LEFT);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 7, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 7, Z_ANTICLOCKWISE, X_LEFT);
	SMAC_ADD_MOVE_X_max82(controller, 28, X_RIGHT);
	SMAC_ADD_MOVE_Y_max82(controller, 2, Y_DOWN);
	
	
	// Fahrt zur Referenzlinie
	
	SMAC_ADD_MOVE_Z_max233(controller, 3, Z_ANTICLOCKWISE);
	
}


void drawBLetter(StepperMotionAxisController* controller)
{
	// Fahrt zur Start Linie
	SMAC_ADD_MOVE_Z_max233(controller, 3, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(controller, 2, Y_UP);
	
	
	SMAC_ADD_MOVE_X_max82(controller, 28, X_LEFT);
	SMAC_ADD_MOVE_Z_max233(controller, 12, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 2, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(controller, 10, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 2, Z_CLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 2, Z_ANTICLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_X_max82(controller, 10, X_RIGHT);
	SMAC_ADD_MOVE_45DIAGONAL(controller, 2, Z_CLOCKWISE, X_RIGHT);
	SMAC_ADD_MOVE_Z_max233(controller, 12, Z_CLOCKWISE);
	SMAC_ADD_MOVE_X_max82(controller, 14, X_LEFT);
	SMAC_ADD_MOVE_Z_max233(controller, 12, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_Y_max82(controller, 2, Y_DOWN);
	

	// FAhrt zur refernzlinie
	SMAC_ADD_MOVE_Z_max233(controller, 5, Z_ANTICLOCKWISE);
	SMAC_ADD_MOVE_X_max82(controller, 14, X_RIGHT);
	
}