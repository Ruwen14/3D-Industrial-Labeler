/*
 * StepperMotionAxisController.h
 *
 * Created: 13.06.2022 20:53:52
 *  Author: ruwen
 */ 


#ifndef STEPPERMOTIONAXISCONTROLLER_H_
#define STEPPERMOTIONAXISCONTROLLER_H_

#define FIFO_BUFFER_SIZE 256 // muss 2^n betragen (8, 16, 32, 64 ...)
#define FIFO_BUFFER_MASK (FIFO_BUFFER_SIZE-1)


#include "constants.h"
#include "movement.h"
#include "ADCLaser.h"
#include <avr/io.h>
#include <avr/interrupt.h>


// Befehlstypen einer Motionsequenz
typedef enum
{
	AXIS_MOVE_LEFT,
	AXIS_MOVE_RIGHT,
	AXIS_MOVE_UP,
	AXIS_MOVE_DOWN,

	AXIS_ROTATE_CLOCKWISE,
	AXIS_ROTATE_ANTICLOCKWISE,
	
	AXIS_ROTATE_CLOCKWISE_DEGREE,
	AXIS_ROTATE_ANTICLOCKWISE_DEGREE,
	

	AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE,
	AXIS_MOVE_LEFT_ROTATE_CLOCKWISE,
	AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE,
	AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE,

	AXIS_MOVE_UP_ROTATE_CLOCKWISE,
	AXIS_MOVE_DOWN_ROTATE_CLOCKWISE,
	AXIS_MOVE_UP_ROTATE_ANTICLOCKWISE,
	AXIS_MOVE_DOWN_ROTATE_ANTICLOCKWISE,
	
	AXIS_MOVE_UP_SUB_MM,
	AXIS_MOVE_DOWN_SUB_MM,
	
	AXIS_MOVE_MEAS_RANGE,
	AXIS_MOVE_DRAWING_LEVEL,
	
	AXIS_MEAS_RADIUS_MEDIAN,
	AXIS_MEAS_DIST_ROW_1,
	AXIS_MEAS_DIST_ROW_2,
	AXIS_MEAS_BALLOON_SHELL_CENTER,
	AXIS_ADD_MEAS_POINT_ROW_1,
	AXIS_ADD_MEAS_POINT_ROW_2,
	AXIS_ADD_VERIFY_DIST_CYL,
	
} AxisCmdFlag;



// Jegliche Bewegung wird als MotionSequenz gepuffert. Sie besteht einmal aus dem Verfahrweg und dem Befehlstyp, welcher später interpretiert wird.
typedef struct {
	uint8_t move_XY;
	uint8_t move_Z;
	AxisCmdFlag cmd;
} MotionSequence;

// Gibt den Status einer FIFO-Buffer Operation an
typedef enum
{
	BUFFER_FULL,
	BUFFER_EMPTY,
	BUFFER_HAS_CAPACITY
} FIFOSeqBufferOptSuccess;

// First-In-First-Out Pufferspeicher für Fahrsequenzen
typedef volatile struct {
	MotionSequence sequences[FIFO_BUFFER_SIZE];
	uint8_t readIndex; // zeigt auf das Feld mit dem ältesten Inhalt
	uint8_t writeIndex; // zeigt immer auf leeres Feld
} FIFOSeqBuffer;

uint8_t FIFOSeqBuffer_empty(FIFOSeqBuffer* buf);

FIFOSeqBufferOptSuccess FIFOSeqBuffer_push(FIFOSeqBuffer* buf, MotionSequence seq);

FIFOSeqBufferOptSuccess FIFOSeqBuffer_pop(FIFOSeqBuffer* buf, MotionSequence* seq);

void FIFOSeqBuffer_delete(FIFOSeqBuffer* buf);

typedef enum
{
	X_RIGHT,
	X_LEFT
} XDir;

typedef enum
{
	Y_UP,
	Y_DOWN
} YDir;

typedef enum
{
	Z_CLOCKWISE,
	Z_ANTICLOCKWISE
} ZDir;

// Derzeitiger Status des SMAC. Hilfreich, wenn abgefragt werden soll ob eine Ansammlung von Fahrtsequenzen fertig ist.
// Weiterhin nützlich, um ATOMAREN Zugriff auf Membervariablen zu garantieren
typedef enum
{
	STATE_IDLE,
	STATE_PROCESSING,
} ControllerState;


// Derzeit eingespannter Objekttypus
typedef enum
{
	OBJECT_CYLINDER,
	OBJECT_BALLOON,
} ObjectType;

// Derzeit gewählter Subobjekttypus des Ballons
typedef enum
{
	NONE,
	BALLOON_SINGLE_ROW,
	BALLOON_DOUBLE_ROW	
}BalloonSubType;


// StepperMotionAxisController - kurz SMAC
// Sofern Variablen zwischen Main-Thread & Interrupt geteilt werden, sind diese 8-Bit für Atomaren Zugriff. 
// 16-Bit & Größer Variablen werden stets nur innerhalb der ISR modifiziert
typedef volatile struct
{
	// Indikator ob gescanntes Objekt valide ist. Sofern 0, fährt der Schlitten nach wieder nach Hause
	// da ein falsches Objekt vorliegt.
	uint8_t is_object_valid;
	
	// Speichere 16-Bit Positionen der Achsen als 8-Bit, um
	// Atomaren Zugriff der UART-Sende-ISR zu garantieren.
	uint8_t position_x_lower_byte;
	uint8_t position_x_upper_byte;
	uint8_t position_y_lower_byte;
	uint8_t position_y_upper_byte;
	uint8_t position_z_lower_byte;
	uint8_t position_z_upper_byte;
	
	// Umrechnungsfaktor eines (Schriftgrößen)-Pixels in mm
	uint8_t pixel_unit_mm;
	uint8_t pixel_unit_mm_y_up;
	uint8_t pixel_unit_mm_y_down;
	
	// Median-Radius des eingespannten Objektes
	uint8_t object_radius_mm;

	// Kalibrierungsfaktor der Z-Achse
	uint8_t steps_per_mm_z;

	// Mitte des ggf. eingespannten Ballones. Bei Walze = 0;
	uint16_t balloon_shell_center_mm;

	// Temporäre Z-Stepperfrequenz, um diagonales Fahren zu ermöglichen.
	uint16_t temp_ocra_diagonal;

	// Hilfsvariable, für Validierung einer Walze
	uint16_t cylinder_add_verify_dist;
	
	// Zustelldistanzen des Stiftes für die einzelnen Buchstaben
	uint16_t meas_distances_row_1[32];
	uint16_t meas_distances_row_2[32];

	// Status des SMAC
	ControllerState state;

	// Derzeit eingespannter Objekttyp
	ObjectType objtype;

	// ggf. gewählter Subtyp des Objekttyps Ballon
	BalloonSubType balloonsubtype;

	// FIFO-Buffer für Fahrtsequenzen
	FIFOSeqBuffer sequencebuffer;

	
} StepperMotionAxisController;

void SMAC_init_XYZ(void);

void SMAC_reset_positions(StepperMotionAxisController*c);

void SMAC_run_Z_pwm(void);

void SMAC_disable_Z_pwm(void);

void SMAC_run_XY_pwm(void);

void SMAC_disable_XY_pwm(void);


// INTERNAL-USAGE
// Berechnet den Output-Compare-Value für einen One-Shot-Interrupt mit einer Auflösung von 1 ms,
// der die Stepper-PWM nach verfahrenen Weg stoppt. Maximalwert von 233 [mm] darf nicht überschritten werden,
// da sonst der Output-Compare-Value (16 bit) überläuft.
uint16_t SMAC_calc_one_shot_timer_Z_max233(uint8_t move_mm, uint8_t steps_per_mm_z);

// INTERNAL-USAGE
// Berechnet den Output-Compare-Value für einen One-Shot-Interrupt mit einer Auflösung von 1 ms,
// der die Stepper-PWM nach verfahrenen Weg stoppt. Maximalwert von 82 [mm] darf nicht überschritten werden,
// da sonst der Output-Compare-Value (16 bit) überläuft.
uint16_t SMAC_calc_one_shot_timer_XY_max82(uint8_t move_mm);



uint16_t SMAC_calc_one_shot_timer_XY_10th_mm_max820(uint16_t move_10thmm);


uint16_t SMAC_calc_one_shot_timer_Z_max233_degree(uint8_t degree);



// INTERNAL-USAGE
// Dieser "One-Shot-Timer" wird genutzt sofern nur eine Bahn X, Y oder Z gefahren wird.
// Bei parallelem Verfahren wird 'start_multi_one_shot_timer' genutzt.
void SMAC_start_one_shot_timer(uint16_t ocrna_);

// INTERNAL-USAGE
// Disabled den 'Single'-One-Shot-Interrupt. Wird in der ISR aufgerufen
void SMAC_disable_one_shot_timer(void);

// INTERNAL-USAGE
// Da nur noch einer der insgesamt 4x 16Bit Timer übrig ist, müssen wir diesen für das
// zeitgleiche Verfahren der XY / Z - Achse über die Register OCR5A / OCR5B aufteilen
// Der "One-Shot-Timer" generiert hierbei 2 COMP-Interrupts. Einmal bei OCR5B und einmal bei
// OCR5A. Da bei OCR5A der Timer TCNT5 zurückgesetzt wird muss OCR5B < OCR5A sein !. (S. 146)
void SMAC_start_multi_one_shot_timer(uint16_t ocrna_, uint16_t ocrnb_);

// INTERNAL-USAGE
// Disabled den 'Multi'-One-Shot-Interrupt. Wird in der ISR aufgerufen
void SMAC_disable_multi_one_shot_timer(void);

void SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c);

void SMAC_END_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c);

void SMAC_ADD_MOVE_X_max82(StepperMotionAxisController* c, uint8_t move_mm, XDir dir);






void SMAC_ADD_MOVE_Y_max82(StepperMotionAxisController* c, uint8_t move_mm, YDir dir);

void SMAC_ADD_MOVE_Z_max233(StepperMotionAxisController* c, uint8_t move_mm, ZDir dir);

void SMAC_ADD_MOVE_Z_DEGREE_max233(StepperMotionAxisController* c, uint8_t degree, ZDir dir);

void SMAC_ADD_MOVE_45DIAGONAL(StepperMotionAxisController* c, uint8_t zx_mm, ZDir zdir, XDir xdir);

void SMAC_ADD_MOVE_Y_sub_mm_max255(StepperMotionAxisController* c, uint8_t y_10th_mm , YDir dir);

void SMAC_ADD_MOVE_X_BIG(StepperMotionAxisController* c, uint8_t move_mm, XDir dir);

void SMAC_ADD_MOVE_Y_FLOAT_MM(StepperMotionAxisController*c, uint16_t move, YDir dir);

void SMAC_ADD_VERIFY_DIST_CYL(StepperMotionAxisController* c);

void SMAC_ADD_MOVE_Y_DRAWING_LEVEL(StepperMotionAxisController* c);

void SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(StepperMotionAxisController* c);

void SMAC_ADD_MEASURMENT_POINT_ROW_1(StepperMotionAxisController* c);

void SMAC_ADD_MEASURMENT_POINT_ROW_2(StepperMotionAxisController* c);


void SMAC_ADD_FIND_BALLOON_SHELL_CENTER_STEP(StepperMotionAxisController* c);

void SMAC_start_new_motion_sequence(StepperMotionAxisController* c, MotionSequence* seq);

void SMAC_return_home(StepperMotionAxisController* c);


void SMAC_ADD_GO_MEASUREMENT_RANGE(StepperMotionAxisController*c);

void GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(StepperMotionAxisController* c);

void GO_MEASUREMENT_RADII_BALLOON_SINGLE_ROW(StepperMotionAxisController* c);

void GO_MEASUREMENT_RADII_BALLOON_DOUBLE_ROW(StepperMotionAxisController* c);

void GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER(StepperMotionAxisController* c);

void SMAC_GO_BEGINNING_BALLOON_ROW_2_DOUBLE_ROW(StepperMotionAxisController* controller, uint8_t char_count);


uint8_t calc_steps_per_mm_Z(uint16_t radius_mm);

uint16_t calc_temp_freq_Z_for_diagonal_move(uint8_t steps_per_mm_Z);

uint8_t calc_pixel_unit_width_mm(uint8_t radius_obj_mm);

uint16_t balloon_perimeter(uint8_t radius);

void SMAC_init(StepperMotionAxisController*c, uint8_t object_radius_z_mm_, ObjectType type);







#endif /* STEPPERMOTIONAXISCONTROLLER_H_ */