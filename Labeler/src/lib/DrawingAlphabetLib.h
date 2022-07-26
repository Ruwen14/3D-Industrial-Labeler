/*
 * DrawingAlphabetLib.h
 *
 * Created: 17.06.2022 15:46:26
 *  Author: Michele
 */ 


#ifndef DRAWINGALPHABETLIB_H_
#define DRAWINGALPHABETLIB_H_


#include "StepperMotionAxisController.h"
#include "LCD.h"



// Quality of Life Macros. pixel*controller->pixel_unit_mm ist innerhalb der Grenzen (Außer der Liftballon hätte 26cm durchmesser was nie der Fall ist)
#define vertical_up(pixel) \
SMAC_ADD_MOVE_X_max82(controller, pixel*controller->pixel_unit_mm, X_LEFT); \

#define vertical_down(pixel) \
SMAC_ADD_MOVE_X_max82(controller, pixel*controller->pixel_unit_mm, X_RIGHT); \


#define horizontal_left(pixel) \
SMAC_ADD_MOVE_Z_max233(controller, pixel*controller->pixel_unit_mm, Z_CLOCKWISE); \


#define horizontal_right(pixel) \
SMAC_ADD_MOVE_Z_max233(controller, pixel*controller->pixel_unit_mm, Z_ANTICLOCKWISE); \


#define diagonal_right_down(pixel) \
SMAC_ADD_MOVE_45DIAGONAL(controller, pixel*controller->pixel_unit_mm, Z_CLOCKWISE, X_RIGHT); \


#define diagonal_right_up(pixel) \
SMAC_ADD_MOVE_45DIAGONAL(controller, pixel*controller->pixel_unit_mm, Z_ANTICLOCKWISE, X_LEFT); \


#define diagonal_left_down(pixel) \
SMAC_ADD_MOVE_45DIAGONAL(controller, pixel*controller->pixel_unit_mm, Z_ANTICLOCKWISE, X_RIGHT); \


#define diagonal_left_up(pixel) \
SMAC_ADD_MOVE_45DIAGONAL(controller, pixel*controller->pixel_unit_mm, Z_CLOCKWISE, X_LEFT); \


#define pen_contact_y(pixel) \
SMAC_ADD_MOVE_Y_max82(controller, pixel*controller->pixel_unit_mm_y_up, Y_UP); \


#define pen_drop_y(pixel) \
SMAC_ADD_MOVE_Y_max82(controller, pixel*controller->pixel_unit_mm_y_down, Y_DOWN); \





// void drawMLetter(StepperMotionAxisController* controller);
// void drawBLetter(StepperMotionAxisController* controller);










void writeLetterA(StepperMotionAxisController* controller);
void writeLetterB(StepperMotionAxisController* controller);
void writeLetterC(StepperMotionAxisController* controller);
void writeLetterD(StepperMotionAxisController* controller);
void writeLetterE(StepperMotionAxisController* controller);
void writeLetterF(StepperMotionAxisController* controller);
void writeLetterG(StepperMotionAxisController* controller);
void writeLetterH(StepperMotionAxisController* controller);
void writeLetterI(StepperMotionAxisController* controller);
void writeLetterJ(StepperMotionAxisController* controller);
void writeLetterK(StepperMotionAxisController* controller);
void writeLetterL(StepperMotionAxisController* controller);
void writeLetterM(StepperMotionAxisController* controller);
void writeLetterN(StepperMotionAxisController* controller);
void writeLetterO(StepperMotionAxisController* controller);
void writeLetterP(StepperMotionAxisController* controller);
void writeLetterQ(StepperMotionAxisController* controller);
void writeLetterR(StepperMotionAxisController* controller);
void writeLetterS(StepperMotionAxisController* controller);
void writeLetterT(StepperMotionAxisController* controller);
void writeLetterU(StepperMotionAxisController* controller);
void writeLetterV(StepperMotionAxisController* controller);
void writeLetterW(StepperMotionAxisController* controller);
void writeLetterX(StepperMotionAxisController* controller);
void writeLetterY(StepperMotionAxisController* controller);
void writeLetterZ(StepperMotionAxisController* controller);

void writeDigitZero(StepperMotionAxisController* controller);
void writeDigitOne(StepperMotionAxisController* controller);
void writeDigitTwo(StepperMotionAxisController* controller);
void writeDigitThree(StepperMotionAxisController* controller);
void writeDigitFour(StepperMotionAxisController* controller);
void writeDigitFive(StepperMotionAxisController* controller);
void writeDigitSix(StepperMotionAxisController* controller);
void writeDigitSeven(StepperMotionAxisController* controller);
void writeDigitEight(StepperMotionAxisController* controller);
void writeDigitNine(StepperMotionAxisController* controller);
void writeComma(StepperMotionAxisController* controller);
void writeQuestionMark(StepperMotionAxisController* controller);
void writeExclamationMark(StepperMotionAxisController* controller);
void writeDot(StepperMotionAxisController* controller);
void writeSpace(StepperMotionAxisController* controller);

// 
// void drawNikolausFIFO(void)
// {
// 	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
// 	SMAC_ADD_MOVE_Z_max233(&controller, 20, Z_ANTICLOCKWISE);
// 	SMAC_ADD_MOVE_X_max82(&controller, 20, X_RIGHT);
// 	SMAC_ADD_MOVE_Z_max233(&controller, 20, Z_CLOCKWISE);
// 	SMAC_ADD_MOVE_X_max82(&controller, 20, X_LEFT);
// 	SMAC_ADD_MOVE_45DIAGONAL(&controller, 20, Z_ANTICLOCKWISE, X_RIGHT);
// 	SMAC_ADD_MOVE_45DIAGONAL(&controller, 10, Z_ANTICLOCKWISE, X_LEFT);
// 	SMAC_ADD_MOVE_45DIAGONAL(&controller, 10, Z_CLOCKWISE, X_LEFT);
// 	SMAC_ADD_MOVE_45DIAGONAL(&controller, 20, Z_CLOCKWISE, X_RIGHT);
// 	SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
// 
// }



uint8_t validate_input(char character);

uint8_t validate_input_stream(char* stream);

void SMAC_GO_BEGINNING_CYLINDER_ROW_2(StepperMotionAxisController* controller, uint8_t char_count);


void SMAC_dispatch_character_function(StepperMotionAxisController* controller, char character);




#endif /* DRAWINGALPHABETLIB_H_ */