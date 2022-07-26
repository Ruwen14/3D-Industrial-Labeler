/*
 * StepperMotionAxisController.c
 *
 * Created: 13.06.2022 21:20:12
 *  Author: ruwen
 */ 

#include "StepperMotionAxisController.h"
#include "uart.h"


// Addition von einer 16-Bit Variable über lower/upper - Byte (8-Bit) Repräsentation
static void _add_16reg_as_8reg(volatile uint8_t* lower_byte, volatile uint8_t* upper_byte, uint8_t data)
{
	uint16_t temp = (uint16_t)((*upper_byte << 8) | (*lower_byte & 0xff)) + data;
	*lower_byte = temp & 0xff;
	*upper_byte = (temp >> 8) & 0xff;
}

// Subtraktion von einer 16-Bit Variable über lower/upper - Byte (8-Bit) Repräsentation
static void _subtr_16reg_as_8reg(volatile uint8_t* lower_byte, volatile uint8_t* upper_byte, uint8_t data)
{
	uint16_t temp = (uint16_t)((*upper_byte << 8) | (*lower_byte & 0xff)) - data;
	*lower_byte = temp & 0xff;
	*upper_byte = (temp >> 8) & 0xff;
}

// Addition von einer 16-Bit Variable über lower/upper - Byte (8-Bit) Repräsentation
// Bei Z-Achse ist zu beachten, dass es zu einem Überlauf von 360° auf 1° bzw.  Unterlauf von 0° auf 359° kommt
static void _add_16reg_as_8reg_deg(volatile uint8_t* lower_byte, volatile uint8_t* upper_byte, uint8_t data)
{
	uint16_t current = (uint16_t)((*upper_byte << 8) | (*lower_byte & 0xff));
	uint16_t dif = 360 - current;

	if (dif >= data)	 {
		current += data;
	}

	else {
		// Beginne wieder bei 0°
		current = (current + data) - 360;
	}

	*lower_byte = current & 0xff;
	*upper_byte = (current >> 8) & 0xff;
}

// Subtraktion von einer 16-Bit Variable über lower/upper - Byte (8-Bit) Repräsentation
// Bei Z-Achse ist zu beachten, dass es zu einem Überlauf von 360° auf 1° bzw.  Unterlauf von 0° auf 359° kommt
static void _subtr_16reg_as_8reg_deg(volatile uint8_t* lower_byte, volatile uint8_t* upper_byte, uint8_t data)
{
	uint16_t current = (uint16_t)((*upper_byte << 8) | (*lower_byte & 0xff));
	int16_t dif = current - data;
	
	if (dif <= 0) {
		current = dif + 360;
	}
	
	else {
		current -= data;
	}
	
	*lower_byte = current & 0xff;
	*upper_byte = (current >> 8) & 0xff;
}


// Umrechnung von Bogenmaß zu Grad bei Walze / Ballon
static uint8_t rad2deg(uint8_t rad, uint8_t radius)
{
	uint16_t temp1 = (rad*10)+5;
	uint32_t temp2 = (uint32_t)temp1 * THREE_SIXTY_DIV_TWO_PI;

	return 	temp2 / (uint16_t)(10*radius);
}




// Signalisiert, dass Fahrtsequenzbuffer leer ist.
uint8_t FIFOSeqBuffer_empty(FIFOSeqBuffer* buf)
{
	if (buf->readIndex == buf->writeIndex)
		return 1;
	return 0;
}

// Fügt dem Puffer eine Fahrtsequenz hinzu. Gibt den aktuellen Status des Buffers zurück
FIFOSeqBufferOptSuccess FIFOSeqBuffer_push(FIFOSeqBuffer* buf, MotionSequence seq)
{
	uint8_t next = ((buf->writeIndex + 1) & FIFO_BUFFER_MASK);

	if (buf->readIndex == next)
		return BUFFER_FULL; // voll

	buf->sequences[buf->writeIndex] = seq;
	// 	// buf->data[buf->write & BUFFER_MASK] = byte; // absolut Sicher
	buf->writeIndex = next;

	return BUFFER_HAS_CAPACITY;
}

// Entfernt eine Fahrsequenz aus dem Buffer. Gibt den aktuellen Status des Buffers zurück
FIFOSeqBufferOptSuccess FIFOSeqBuffer_pop(FIFOSeqBuffer* buf, MotionSequence* seq)
{
	if (buf->readIndex == buf->writeIndex)
		return BUFFER_EMPTY;

	*seq = buf->sequences[buf->readIndex];

	buf->readIndex = (buf->readIndex + 1) & FIFO_BUFFER_MASK;

	return BUFFER_HAS_CAPACITY;
}

// Entleert den Buffer komplett.
void FIFOSeqBuffer_delete(FIFOSeqBuffer* buf)
{
	MotionSequence seq;
	
	while(FIFOSeqBuffer_pop(buf, &seq) == BUFFER_HAS_CAPACITY)
	{
	}
}


void SMAC_init_XYZ(void)
{
	// ZXY-STEPPER-PORTS
	// Ausgang für Z-Achse konfigurieren
	BitSet(DDRH, Z_TAKT);
	BitSet(DDRL, Z_DIR);
	BitSet(DDRL, Z_ACTIVE);
	
	// Z-Schrittmotor aktivieren
	Z_SET_ACTIVE;
	
	// Ausgang für X-Achse konfigurieren
	BitSet(DDRE, X_TAKT);
	BitSet(DDRA, X_DIR);
	BitSet(DDRA, X_ACTIVE);
	
	// Ausgang für Y-Achse konfigurieren
	BitSet(DDRB, Y_TAKT);
	BitSet(DDRA, Y_DIR);
	BitSet(DDRA, Y_ACTIVE);
	
	// XY-Schrittmotoren aktivieren
	XY_SET_ACTIVE;
	
	// Z-Stepper 500 Hz
	OCR4A = 249;
	
	
	// X-Stepper 500 Hz
	OCR3A = 249;
	
	// Y-Stepper 500 Hz
	OCR1A = 249;
}

void SMAC_reset_positions(StepperMotionAxisController*c)
{
	c->position_x_lower_byte = 0;
	c->position_x_upper_byte = 0;
	
	c->position_y_lower_byte = 0;
	c->position_y_upper_byte = 0;
	
	c->position_z_lower_byte = 0;
	c->position_z_upper_byte = 0;
}

// Schaltet die Hardware-PWM der Z-Achse an
void SMAC_run_Z_pwm(void)
{
	// WGM42 CTC Mode | CS41 + CS40 -> prescaler 64
	TCCR4B = (1<<WGM42) | (1<<CS41) | (1<<CS40);

	// Reset Counter
	TCNT4 = 0;

	// Toggle den Output Pin OC4B = PH4 = Z_TAKT
	TCCR4A |= (1<<COM4B0);
}

// Schaltet die Hardware-PWM der Z-Achse aus
void SMAC_disable_Z_pwm(void)
{
	// No Source (Timer/Counter stopped)
	TCCR4B = 0;

	// Wahrscheinlich nicht benötigt ??? ToDo: Ruwen
	TCCR4A = 0;
}

// Schaltet die Hardware-PWM der XY-Achse an
void SMAC_run_XY_pwm(void)
{
	// X-Achse
	// --------------------------------------------
	// WGM32 CTC Mode | CS31 + CS30 prescaler 64
	TCCR3B = (1<<WGM32) | (1<<CS31) | (1<<CS30);
	
	
	// Y-Achse
	// --------------------------------------------
	// WGM12 CTC Mode | CS11 + CS10 prescaler 64
	TCCR1B = (1<<WGM12) | (1<<CS11) | (1<<CS10);
	
	
	TCNT3 = 0; // X-Reset Counter
	TCNT1 = 0; // Y-Reset Counter
	
	
	
	// X-Achse
	// --------------------------------------------
	// Toggle den Output Pin OC3B = PE4 = X_TAKT
	TCCR3A = (1<<COM3B0);
	
	
	// Y-Achse
	// --------------------------------------------
	// Toggle den Output Pin OC1B = PB6 = Y_TAKT
	TCCR1A = (1<<COM1B0);
}

// Schaltet die Hardware-PWM der XY-Achse aus
void SMAC_disable_XY_pwm(void)
{
	TCCR3B = 0;
	TCCR1B = 0;
	
	TCCR3A = 0;
	TCCR1A = 0;
}

// Errechnet den OCRNA-Wert des One-Shot-Timers (Z-Achse) aus, damit dieser nach genau (move_mm) ausläuft.
uint16_t SMAC_calc_one_shot_timer_Z_max233(uint8_t move_mm, uint8_t steps_per_mm_z)
{
	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * steps_per_mm_z * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000; // (16 MHz / N=1024) = 15625

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}

// Errechnet den OCRNA-Wert des One-Shot-Timers (XY-Achse) aus, damit dieser nach genau (move_mm) ausläuft.
uint16_t SMAC_calc_one_shot_timer_XY_max82(uint8_t move_mm)
{
	// Folgende Operationen sind bestimmt auch als Shifting-Operationen durchzuführen.
	// Da diese Funktion jedoch vor der PWM-Generationen/One-Shot-Start aufgerufen wird
	// und somit nicht zeitkritisch ist, habe ich mir mal die Mühe ersparrt :). Ruwen

	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint32_t T_interrupt_period = move_mm * STEPS_PER_MM_X * pulse_period_ms;

	uint32_t OCRNA_ = (T_interrupt_period * 15625) - 1000; // (16 MHz / N=1024) = 15625

	uint16_t OCRNA_rescaled_SI = OCRNA_ / 1000;

	return OCRNA_rescaled_SI;
}

// Errechnet den OCRNA-Wert des One-Shot-Timers (XY-Achse) aus, damit dieser nach genau (move_10thmm) ausläuft.
// Die Genauigkeit dieser Funktion kann genutzt werden, um mit einer Genauigkeit von 1/10 mm zu fahren
uint16_t SMAC_calc_one_shot_timer_XY_10th_mm_max820(uint16_t move_10thmm)
{
// Folgende Operationen sind bestimmt auch als Shifting-Operationen durchzuführen.
// Da diese Funktion jedoch vor der PWM-Generationen/One-Shot-Start aufgerufen wird
// und somit nicht zeitkritisch ist, habe ich mir mal die Mühe ersparrt :). Ruwen

	uint8_t pulse_period_ms = 2;

	// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
	uint64_t T_interrupt_period = (move_10thmm * STEPS_PER_MM_X * pulse_period_ms);

	uint64_t OCRNA_ = (T_interrupt_period * 15625) - 1000; // (16 MHz / N=1024) = 15625

	uint64_t OCRNA_rescaled_SI = (OCRNA_ / 1000);

	return OCRNA_rescaled_SI/10;	
}

// Errechnet den OCRNA-Wert des One-Shot-Timers (Z-Achse) aus, damit dieser nach genau (degree) ausläuft.
// Diese Funktion ermäglicht es die Z-Achse grad-genau zu drehen. Wir müssen hier mit 8.89 steps/deg rechnen, damit
// wir wirklich exakt 360° Fahren können!
uint16_t SMAC_calc_one_shot_timer_Z_max233_degree(uint8_t degree)
{
		uint8_t pulse_period_ms = 2;

		uint32_t temp = degree * pulse_period_ms;
		// Als uint32_t damit temporäres Ergebnis (T_interrupt_period * 15625) nicht überläuft
		uint64_t T_interrupt_period = temp * 889; // entspricht 8.89 steps/deg -> scaled

		uint64_t OCRNA_ = (T_interrupt_period * 15625) - 1000; // (16 MHz / N=1024) = 15625
		uint64_t divider = 100000;

		uint16_t OCRNA_rescaled_SI = OCRNA_ / divider;

		return OCRNA_rescaled_SI;
}

// Triggered den One-Shot-Timer
void SMAC_start_one_shot_timer(uint16_t ocrna_)
{
	// OC5A Timer-Register CTC
	OCR5A = ocrna_;
	
	// Reset da in *disable* potenziell cleared.
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5A1 + COM5A0 for disconnecting OC5A. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5A1) | (1<<COM5A0));
	
	// Enables Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 |= (1<<OCIE5A);
	
// 	sei();
}

// Schaltet den One-Shot-Timer aus
void SMAC_disable_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}

// Schaltet den Multi-One-Shot-Timer aus. 
// Um 45° diagonal fahren zu können, müssen wir X und Z-Achse gleichzeitig betakten. Entsprechend
// muss das Unteregister B hinzugenommen werden.
void SMAC_start_multi_one_shot_timer(uint16_t ocrna_, uint16_t ocrnb_)
{
	// Wir teilen das OC5A/B Timer-Register für CTC
	OCR5A = ocrna_;
	OCR5B = ocrnb_;
	
	
	// Reset da in *disable* potenziell cleared.
	// WGM52 CTC Mode | (1<<CS52) + (1<<CS50) prescaler 1024
	TCCR5B = (1<<WGM52) | (1<<CS52) | (1<<CS50);
	
	// Clear COM5B1 + COM5B0 for disconnecting OC5B. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5B1) | (1<<COM5B0));
	// Clear COM5A1 + COM5A0 for disconnecting OC5A. Usable for Normal port operation (p. 155)
	TCCR5A &= ~((1<<COM5A1) | (1<<COM5A0));
	
	// Reset da in *disable* potenziell cleared.
	// Enables Output Compare Match Interrupt 'TIMER5_COMPB_vect'
	TIMSK5 |= (1<<OCIE5B);
	// Enables Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 |= (1<<OCIE5A);
	
// 	sei();
}

// Schaltet den Multi-One-Shot-Timer aus.
void SMAC_disable_multi_one_shot_timer(void)
{
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPB_vect'
	TIMSK5 &= ~(1 << OCIE5B);
	// DISABLES Output Compare Match Interrupt 'TIMER5_COMPA_vect'
	TIMSK5 &= ~(1 << OCIE5A);
	
	// No clock source. (Timer / Counter stopped)
	TCCR5B &= ~((1 << CS51) | (1 << CS52) | (1 << CS50));
}

// Eine Sequenzabfolge kann aber muss nicht mit dieser Anweisung beginnen. Derzeit tut Sie nix. Dies kann sich 
// Aber in der Zukunft ändern!
void SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c)
{

}

// Signalisiert das ENDE einer Sequenzabfolge. Diese Funktion stößt das entleeren des Fahrsequenzpuffers an. 
// Daraus folgt eine Kaskadierung bis der Puffer leer ist.
void SMAC_END_DECLARE_MOTION_SEQUENCE(StepperMotionAxisController* c)
{
	if (!(FIFOSeqBuffer_empty(&c->sequencebuffer)))
	{
		MotionSequence seq;
		FIFOSeqBuffer_pop(&c->sequencebuffer, &seq);
		SMAC_start_new_motion_sequence(c, &seq);
	}
}

// Fügt eine Fahrtsequenz in X-Richtung hinzu. Bei direkten Aufruf darf move_mm -> 82 nicht überschreiten, da sonst der berechnete 
// One-Shot-Timer überlauft (>16Bit). Intern wird sichergestellt, das es dazu jedoch nicht kommt.
void SMAC_ADD_MOVE_X_max82(StepperMotionAxisController* c, uint8_t move_mm, XDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == X_LEFT)
		newCmd = AXIS_MOVE_LEFT;
	else
		newCmd = AXIS_MOVE_RIGHT;
		
	// Puffer die Sequenz für später
	MotionSequence seq = {move_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Fügt eine Fahrtsequenz in Y-Richtung hinzu. Bei direkten Aufruf darf move_mm -> 82 nicht überschreiten, da sonst der berechnete
// One-Shot-Timer überlauft (>16Bit). Intern wird sichergestellt, das es dazu jedoch nicht kommt.
void SMAC_ADD_MOVE_Y_max82(StepperMotionAxisController* c, uint8_t move_mm, YDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Y_UP)
		newCmd = AXIS_MOVE_UP;
	else
		newCmd = AXIS_MOVE_DOWN;
		
	// Puffer die Sequenz für später
	MotionSequence seq = {move_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}


// Fügt eine Fahrtsequenz in Z-Richtung hinzu. Bei direkten Aufruf darf move_mm -> 233 nicht überschreiten, da sonst der berechnete
// One-Shot-Timer überlauft (>16Bit). Intern wird sichergestellt, das es dazu jedoch nicht kommt.
void SMAC_ADD_MOVE_Z_max233(StepperMotionAxisController* c, uint8_t move_mm, ZDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Z_CLOCKWISE)
		newCmd = AXIS_ROTATE_CLOCKWISE;
	else
		newCmd = AXIS_ROTATE_ANTICLOCKWISE;
	
	// Puffer die Sequenz für später
	MotionSequence seq = {0, move_mm, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}	


// Fügt eine Fahrtsequenz in Z-Richtung in der Einheit grad hinzu. Bei direkten Aufruf darf degree -> 233! nicht überschreiten, da sonst der berechnete
// One-Shot-Timer überlauft (>16Bit). Intern wird sichergestellt, das es dazu jedoch nicht kommt.
void SMAC_ADD_MOVE_Z_DEGREE_max233(StepperMotionAxisController* c, uint8_t degree, ZDir dir)
{
	
	AxisCmdFlag newCmd;
	if (dir == Z_CLOCKWISE)
		newCmd = AXIS_ROTATE_CLOCKWISE_DEGREE;
	else
		newCmd = AXIS_ROTATE_ANTICLOCKWISE_DEGREE;
	
	// Puffer die Sequenz für später
	MotionSequence seq = {0, degree, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Fügt eine Fahrtsequenz in der Diagonale hinzu. Dies wird aus einem koordinierten Zusammenspiel des X und Z-Steppers erreicht .
// Es gelten die Überlaufbedingung der X/Z-Achse (siehe oben).
void SMAC_ADD_MOVE_45DIAGONAL(StepperMotionAxisController* c, uint8_t zx_mm, ZDir zdir, XDir xdir)
{
	AxisCmdFlag newCmd;
	if (zdir == Z_CLOCKWISE)
	{
		if (xdir == X_LEFT)
			newCmd = AXIS_MOVE_LEFT_ROTATE_CLOCKWISE;
		else
			newCmd = AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE;
	}
	
	else
	{
		if (xdir == X_LEFT)
			newCmd = AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE;
		else
			newCmd = AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE;
	}
	
	// Puffer die Sequenz für später
	MotionSequence seq = {zx_mm, zx_mm, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}


// Fügt eine Fahrtsequenz in Y-Richtung in der Einheit 1/10mm hinzu. Der Wert 1 enstpricht 0.1 mm, 10 entsprechend 1 mm, usw...
// Bei direkten Aufruf darf y_10th_mm -> 255! nicht überschreiten, da sonst der berechnete
// One-Shot-Timer überlauft (>16Bit). Intern wird sichergestellt, das es dazu jedoch nicht kommt.
// Diese Funktion wird benötigt, um ein genaues Zustellen des Stiftes zu gewährleisten
void SMAC_ADD_MOVE_Y_sub_mm_max255(StepperMotionAxisController* c, uint8_t y_10th_mm , YDir dir)
{
	AxisCmdFlag newCmd;
	if (dir == Y_UP)
		newCmd = AXIS_MOVE_UP_SUB_MM;
	else
		newCmd = AXIS_MOVE_DOWN_SUB_MM;

	// 5 entspricht 0.5 in diesem Fall
	MotionSequence seq = {y_10th_mm, 0, newCmd};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Hilfsfunktion, um einen Messung + Fahrt zu erzeugen. Wird genutzt um ein extra Snapshot zu machen, um Objekte zu validieren.
void SMAC_ADD_VERIFY_DIST_CYL(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_ADD_VERIFY_DIST_CYL};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Puffert eine Sequenz, die bei Ausführung eine eine Y-Bewegung des Stiftes bis zum Malpunkt/Kontaktpunkt generiert.
void SMAC_ADD_MOVE_Y_DRAWING_LEVEL(StepperMotionAxisController* c)
{
	MotionSequence seq = {0, 0, AXIS_MOVE_DRAWING_LEVEL};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Puffert eine Sequenz, die eine Radiusmessung durchführt und 24° Z-Drehung generiert.
// Bei 15ten Aufruf hat sich die Z-Achse 360° gedreht, und es wird automatisch die Pixelgröße / Steps_per_mm_Z und 
// der Radius berechntet
// MÜNDET am ENDE in -> _trigger_meas_radius_step
void SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_RADIUS_MEDIAN};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}


// Puffert eine Sequenz, die eine Distanzmessung durchführt und eine Z-Drehung generiert die der Breite eines Buchstabens
// Entspricht. Die Distanz wird in meas_distances_row_1[] gespeichert.
void SMAC_ADD_MEASURMENT_POINT_ROW_1(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_ADD_MEAS_POINT_ROW_1};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Puffert eine Sequenz, die eine Distanzmessung durchführt und eine Z-Drehung generiert die der Breite eines Buchstabens
// Entspricht. Die Distanz wird in meas_distances_row_2[] gespeichert.
void SMAC_ADD_MEASURMENT_POINT_ROW_2(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_ADD_MEAS_POINT_ROW_2};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Puffert eine Sequenz, die eine Distanzmessung durchführt und eine Z-Drehung in 1mm generiert. Nach 'BALLOON_SHELL_CENTER_FIND_STEPS'-Aufrufen wird
// das Minimum berechnet und als balloon_shell_center_mm abgespeichert.
// Wird genutzt um die Mitte des Ballons zu finden.
void SMAC_ADD_FIND_BALLOON_SHELL_CENTER_STEP(StepperMotionAxisController* c)
{
	MotionSequence seq = {0,0, AXIS_MEAS_BALLOON_SHELL_CENTER};
	FIFOSeqBuffer_push(&c->sequencebuffer, seq);
}

// Triggert eine Bewegung in X-Richtung
static void _trigger_motion_X(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

// Triggert eine Bewegung in Y-Richtung
static void _trigger_motion_Y(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

// Triggert eine Bewegung in Y-Richtung, Genaugigkeit 1/10 mm
static void _trigger_motion_Y_sub_mm(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_10th_mm_max820(seq->move_XY);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
}

// Triggert eine Bewegung in Z-Richtung
static void _trigger_motion_Z(StepperMotionAxisController* c, MotionSequence* seq)
{

	uint16_t oneshot_val = SMAC_calc_one_shot_timer_Z_max233(seq->move_Z, c->steps_per_mm_z);
	
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_Z_pwm();	
	
}

// Triggert eine Bewegung in Z-Richtung, Einheit Grad
static void _trigger_motion_Z_degree(StepperMotionAxisController*c , MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_Z_max233_degree(seq->move_Z);
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_Z_pwm();
}


// Triggert eine Diagonalbewegung. Zusammenspiel aus X/Z-Stepper
static void _trigger_motion_diagonal(StepperMotionAxisController*c, MotionSequence* seq)
{
	uint16_t oneshot_val = SMAC_calc_one_shot_timer_XY_max82(seq->move_XY);
	
	// Passe Frequenz des Z-Steppers temporär, so an, dass gleicher Weg in gleicher Zeit wie X verfahren wird
	OCR4A = c->temp_ocra_diagonal;
	SMAC_start_one_shot_timer(oneshot_val);
	SMAC_run_XY_pwm();
	SMAC_run_Z_pwm();
}

// Triggert eine Bewegung zum Malpunkt des Stiftes
static void _trigger_motion_drawing_level(StepperMotionAxisController*c, MotionSequence* seq)
{
	uint16_t y_dist  = ADC_Laser_read_median();
	uint8_t mm = y_dist / 10;
	uint8_t tenth_mm = (y_dist - mm * 10)+7;

	_add_16reg_as_8reg(&c->position_y_lower_byte, &c->position_y_upper_byte, mm);
	
	uint16_t oneshot_val = 0;
	if (mm != 0)
	{
		oneshot_val = oneshot_val + SMAC_calc_one_shot_timer_XY_max82(mm);
	}
	
	if (tenth_mm != 0)
	{
		oneshot_val = oneshot_val + SMAC_calc_one_shot_timer_XY_10th_mm_max820(tenth_mm);
	}
	
	if (oneshot_val > 0)
	{
		SMAC_start_one_shot_timer(oneshot_val);
		SMAC_run_XY_pwm();
	}
}

static uint8_t check_object_cylinder_valid(uint16_t dist_val1, uint16_t dist_val2)
{
	
	// Kein Objekt, Laser misst 0
	if ((dist_val1 == 0) && (dist_val2 == 0))
		return 0;
	
	uint16_t delta;
	if (dist_val1 > dist_val2)
		delta = dist_val1 -  dist_val2;
	
	else
		delta = dist_val2 - dist_val1;
	
	
	// Achtung wir haben hier anscheinend einen Ballon!
	// Dies ist an dem zu großen Delta zu erkennen.
	if (delta >= CYLINDER_OBJECT_CHECK_DELTA)
		return 0;
	
	// Jawohl! Wir haben eine Walze. Gut!
	else
		return 1;

}

// Generiere Einzel-Radius-Messung
// Ausmessen Walze/Ballon. Nach 15-Bewegungen, wird Pixelgröße, Radius, steps_per_mm_z usw. berechnet, da insg. 360° (15*24°) 
// gefahren worden sind.
static void _trigger_meas_radius_step(StepperMotionAxisController*c , MotionSequence* seq)
{
	static uint16_t laser_dist_buffer[MEDIAN_FILTER_SIZE];
	static uint8_t index = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_dist_buffer[index] = laser_dist;
	
	index++;
	
	
	if (index == MEDIAN_FILTER_SIZE)
	{
		uint16_t median_dist = median_filter(laser_dist_buffer);
		c->object_radius_mm  = calc_radius(median_dist);
		//c->pixel_unit_mm = calc_pixel_unit_width_mm(c->object_radius_mm);
		
		if (c->objtype == OBJECT_BALLOON)
		{
			if (c->balloonsubtype == BALLOON_SINGLE_ROW)
			{
				c->pixel_unit_mm = 3;
			}
			else
			{
				c->pixel_unit_mm = 2;
			}
			c->steps_per_mm_z = calc_steps_per_mm_Z(c->object_radius_mm);
		
		
		}
		else
		{
			c->pixel_unit_mm = calc_pixel_unit_width_mm(c->object_radius_mm);
			c->pixel_unit_mm_y_up = c->pixel_unit_mm;
			c->pixel_unit_mm_y_down = c->pixel_unit_mm;
			c->steps_per_mm_z = calc_steps_per_mm_Z(c->object_radius_mm);
			c->temp_ocra_diagonal = calc_temp_freq_Z_for_diagonal_move(c->steps_per_mm_z);
		}
		
		
		// Überprüfe ob richtiges Objekt
		if (c->objtype == OBJECT_CYLINDER)
		{
			c->is_object_valid = check_object_cylinder_valid(laser_dist_buffer[0], c->cylinder_add_verify_dist);
		}
		
		index = 0;
	}

	// -> 15*24° = 360°
	MotionSequence seq_temp = {0, 24, AXIS_ROTATE_ANTICLOCKWISE_DEGREE};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}



static uint8_t check_object_balloon_valid(uint16_t dist_val1, uint16_t dist_val2)
{
	
	// Kein Objekt, Laser misst 0
	if ((dist_val1 == 0) && (dist_val2 == 0))
		return 0;
			
	uint16_t delta;
	if (dist_val1 > dist_val2)
		delta = dist_val1 -  dist_val2;
	
	else
		delta = dist_val2 - dist_val1;
	
	
	// Achtung wir haben hier anscheinend eine Walze!
	// Dies ist an dem zu kleinen Delta zu erkennen.
	if (delta <= BALLOON_OBJECT_CHECK_DELTA)
		return 0;
	
	// Jawohl! Wir haben einen Ballon. Gut!
	else
		return 1;

}

// Generiere Einzel-Finde-Mitte-Ballon-Messung
static void _trigger_find_balloon_shell_center_step(StepperMotionAxisController* c , MotionSequence* seq)
{
	static uint16_t laser_distances[BALLOON_SHELL_CENTER_FIND_STEPS];
	static uint8_t array_index = 0;
	static uint8_t min_step_occurance = 0;
	
	uint16_t laser_dist = ADC_Laser_read_median();
	laser_distances[array_index] = laser_dist;
	array_index++;
	
	if (array_index == BALLOON_SHELL_CENTER_FIND_STEPS)
	{
		uint16_t min = laser_distances[0];
		
		for (uint8_t i = 0; i < BALLOON_SHELL_CENTER_FIND_STEPS; i++)
		{
			if (laser_distances[i] < min)
			{
				min = laser_distances[i];
				min_step_occurance = i;
			}
			
		}
		
		// Überprüfe valides Objekt
		c->is_object_valid = check_object_balloon_valid(
			laser_distances[BALLOON_OBJECT_CHECK_INDEX_LOWER], laser_distances[BALLOON_OBJECT_CHECK_INDEX_UPPER]
			);
		
		
		c->balloon_shell_center_mm = min_step_occurance * 1;
		array_index = 0;
	}
	
	// Fahr 1mm
	MotionSequence seq_temp = {1, 0, AXIS_MOVE_RIGHT};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}


// Generiere Buchstaben-Abstand-Messung OBERE KANTE
// Mache Messung pro Buchstabe. Obere Kante
static void _trigger_add_measurment_point_row_1_step(StepperMotionAxisController* c, MotionSequence* seq)
{
	static uint8_t index = 0;
	uint16_t laser_dist = ADC_Laser_read_median();
	c->meas_distances_row_1[index] = laser_dist+20;
	
	index++;
	
	if (index == 32)
	{
		index = 0;
	}
	
	MotionSequence seq_temp = {0, 5*c->pixel_unit_mm, AXIS_ROTATE_ANTICLOCKWISE};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}

// Generiere Buchstaben-Abstand-Messung UNTERE KANTE
// Mache Messung pro Buchstabe. Untere Kante
static void _trigger_add_measurment_point_row_2_step(StepperMotionAxisController* c, MotionSequence* seq)
{
	static uint8_t index_2 = 0;
	uint16_t laser_dist = ADC_Laser_read_median();
	c->meas_distances_row_2[index_2] = laser_dist+20;
	
	index_2++;
	
	if (index_2 == 32)
	{
		index_2 = 0;
	}
	
	MotionSequence seq_temp = {0, 5*c->pixel_unit_mm, AXIS_ROTATE_ANTICLOCKWISE};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}

// Zusätzlicher Messpunkt für Walzenvalidierung
static void _trigger_add_additionl_meas_cyl(StepperMotionAxisController* c, MotionSequence* seq)
{
	uint16_t laser_dist = ADC_Laser_read_median();
	c->cylinder_add_verify_dist = laser_dist;
	
	MotionSequence seq_temp = {30, 0, AXIS_MOVE_RIGHT};
	SMAC_start_new_motion_sequence(c, &seq_temp);
}


// Hier werden die Befehlstypen interpretiert und es werden die entsprechenden Bewegungen getriggered
void SMAC_start_new_motion_sequence(StepperMotionAxisController* c, MotionSequence* seq)
{
	// Signalisiert, dass SMAC nun arbeitet
	c->state = STATE_PROCESSING;
	// Wird wahrscheinlich zu einem Jump-Table optimiert.
	// Performance entsprechend O(1) -> const.
	// Der Übergang zweier Fahrtsequenzen ist nicht spürbar	
	switch (seq->cmd)
	{
		// Generiere einfache X-Bewegung
		case AXIS_MOVE_LEFT:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			// Aktualisiere X-Position
			_subtr_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);

			_trigger_motion_X(c, seq);
			break;
		
		// Generiere einfache X-Bewegung
		case AXIS_MOVE_RIGHT:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			
			// Aktualisiere X-Position
			_add_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);
			

			_trigger_motion_X(c, seq);
			break;

		// Generiere einfache Y-Bewegung
		case AXIS_MOVE_UP:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			// Aktualisiere Y-Position
			_add_16reg_as_8reg(&c->position_y_lower_byte, &c->position_y_upper_byte, seq->move_XY);
			
			
			_trigger_motion_Y(c, seq);
			break;

		// Generiere einfache Y-Bewegung
		case AXIS_MOVE_DOWN:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);

			// Aktualisiere Y-Position
			_subtr_16reg_as_8reg(&c->position_y_lower_byte, &c->position_y_upper_byte, seq->move_XY);
			
			_trigger_motion_Y(c, seq);
			break;

		// Generiere einfache Z-Bewegung
		case AXIS_ROTATE_CLOCKWISE:
			BitClear(PORTL, Z_DIR);
			
			// Aktualisiere Z-Position
			_subtr_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));
			
			_trigger_motion_Z(c, seq);
			break;

		
		// Generiere einfache Z-Bewegung
		case AXIS_ROTATE_ANTICLOCKWISE:
			BitSet(PORTL, Z_DIR);

			// Aktualisiere Z-Position
			_add_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));

			
			_trigger_motion_Z(c, seq);
			break;
			
		// Generiere einfache Z-Bewegung IN GRAD
		case AXIS_ROTATE_CLOCKWISE_DEGREE:
			BitClear(PORTL, Z_DIR);
			
			// Aktualisiere Z-Position
			_subtr_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, seq->move_Z);
			

			_trigger_motion_Z_degree(c, seq);
			break;
			
		// Generiere einfache Z-Bewegung IN GRAD
		case AXIS_ROTATE_ANTICLOCKWISE_DEGREE:
			BitSet(PORTL, Z_DIR);
			
			// Aktualisiere Z-Position
			_add_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, seq->move_Z);
			

			_trigger_motion_Z_degree(c, seq);
			
			break;
		
		// Generiere Diagonal Bewegung
		case AXIS_MOVE_RIGHT_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			
			// Aktualisiere X-Position
			_add_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);
			// Aktualisiere Z-Position
			_subtr_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));
			

			_trigger_motion_diagonal(c, seq);
			break;

		// Generiere Diagonal Bewegung
		case AXIS_MOVE_LEFT_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			
			// Aktualisiere X-Position
			_subtr_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);
			// Aktualisiere Z-Position
			_subtr_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));


			_trigger_motion_diagonal(c, seq);
			break;

		// Generiere Diagonal Bewegung
		case AXIS_MOVE_RIGHT_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			
			// Aktualisiere X-Position
			_add_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);
			// Aktualisiere Z-Position
			_add_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));
			
			
			_trigger_motion_diagonal(c, seq);
			
			break;

		// Generiere Diagonal Bewegung
		case AXIS_MOVE_LEFT_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			
			// Aktualisiere X-Position
			_subtr_16reg_as_8reg(&c->position_x_lower_byte, &c->position_x_upper_byte, seq->move_XY);
			// Aktualisiere Z-Position
			_add_16reg_as_8reg_deg(&c->position_z_lower_byte, &c->position_z_upper_byte, rad2deg(seq->move_Z, c->object_radius_mm));


			_trigger_motion_diagonal(c, seq);
			break;
		
		
		case AXIS_MOVE_UP_ROTATE_CLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_CLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitClear(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_UP_ROTATE_ANTICLOCKWISE:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;

		case AXIS_MOVE_DOWN_ROTATE_ANTICLOCKWISE:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			BitSet(PORTL, Z_DIR);
			break;
			
		// Generiere Y-Bewegung mit der Genauigkeit 1/10 mm
		case AXIS_MOVE_DOWN_SUB_MM:
			BitClear(PORTA, Y_DIR);
			BitSet(PORTA, X_DIR);
			
			_trigger_motion_Y_sub_mm(c, seq);
			break;
			
		// Generiere Y-Bewegung mit der Genauigkeit 1/10 mm
		case AXIS_MOVE_UP_SUB_MM:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			_trigger_motion_Y_sub_mm(c, seq);
			break;
			
		// Generiere Y-Bewegung bis Malpunkt
		case AXIS_MOVE_DRAWING_LEVEL:
			BitSet(PORTA, Y_DIR);
			BitClear(PORTA, X_DIR);
			
			// Position wird innerhalb _trigger_motion_drawing_level getracked.
			
			_trigger_motion_drawing_level(c, seq);
	
			break;
			
		
		// Generiere Einzel-Radius-Messung
		case AXIS_MEAS_RADIUS_MEDIAN:
		
			// Nutzt obige Funktionen, die automatisch Positionen tracken.
			
			_trigger_meas_radius_step(c, seq);
		
			break;	

		// Generiere Einzel-Finde-Mitte-Ballon-Messung
		case AXIS_MEAS_BALLOON_SHELL_CENTER:
		
			// Nutzt obige Funktionen, die automatisch Positionen tracken.
			
			_trigger_find_balloon_shell_center_step(c, seq);
			break;
		
		// Generiere Buchstaben-Abstand-Messung OBERE KANTE
		case AXIS_ADD_MEAS_POINT_ROW_1:	
		
			// Nutzt obige Funktionen, die automatisch Positionen tracken.
			
			_trigger_add_measurment_point_row_1_step(c, seq);
			break;
			
		// Generiere Buchstaben-Abstand-Messung UNTERE KANTE
		case AXIS_ADD_MEAS_POINT_ROW_2:
		
			// Nutzt obige Funktionen, die automatisch Positionen tracken.
			
			_trigger_add_measurment_point_row_2_step(c, seq);
			break;
		
		// Generiere zus. Abstandmessung für Zylinder-Validierung
		case AXIS_ADD_VERIFY_DIST_CYL:
		
			// Nutzt obige Funktionen, die automatisch Positionen tracken.
			
		
			_trigger_add_additionl_meas_cyl(c, seq);
			break;
			

		default:
		break;
	}
}


// Schlitten soll heimkehren
void SMAC_return_home(StepperMotionAxisController* c)
{
	// Stop derzeitige Sequenzen
	SMAC_disable_XY_pwm();
	SMAC_disable_Z_pwm();
	SMAC_disable_multi_one_shot_timer();
	
	// Entleert Fahrsequenzbuffer
	FIFOSeqBuffer_delete(&c->sequencebuffer);
	c->state = STATE_PROCESSING;
	
	// Zugegebenermaßen ist das benutzen einer While-Schleife nicht perfekt und
	// widerspricht dem Design-Ansatz des Totzeitfreien Fahrens über Buffer des restlichen Codes
	// Diese Whileschleife positioniert den Motor allerdings nur um wenige Zehntel-mm vom Endlagenschalter weg und wird
	// nur einmalig am Start des Programmes ausgeführt. Die dadurch eingeführte Totzeit von wenigen millisekunden ist für uns somit vertretbar und
	// vereinfacht die Initiale-Positioniertung des Motors ERHEBLICH!
	while(!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		BitSet(PORTA, Y_DIR);
		BitSet(PORTA, X_DIR);
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
		
	}
	
	// Zugegebenermaßen ist das benutzen einer While-Schleife nicht perfekt und
	// widerspricht dem Design-Ansatz des Totzeitfreien Fahrens über Buffer des restlichen Codes
	// Diese Whileschleife positioniert den Motor allerdings nur um wenige Zehntel-mm vom Endlagenschalter weg und wird
	// nur einmalig am Start des Programmes ausgeführt. Die dadurch eingeführte Totzeit von wenigen millisekunden ist für uns somit vertretbar und
	// vereinfacht die Initiale-Positioniertung des Motors ERHEBLICH!
	while(!IsBitSet(PIND, Y_BOTTOM_LIM_PIN))
	{
		BitSet(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
		XY_RISING_EDGE;
		_delay_us(1000);
		XY_FALLING_EDGE;
		_delay_us(1000);
	}
	
	
	// Fahr in Home-Position
	BitClear(PORTA, Y_DIR);
	BitSet(PORTA, X_DIR);
	SMAC_run_XY_pwm();
	
}

// Sequenzen, Fahre in Messposition
void SMAC_ADD_GO_MEASUREMENT_RANGE(StepperMotionAxisController*c)
{
		if (c->objtype == OBJECT_BALLOON)
		{
			SMAC_ADD_MOVE_X_max82(c, 77, X_RIGHT);
		}
		else
		{
			SMAC_ADD_MOVE_X_max82(c, 50, X_RIGHT);
			SMAC_ADD_MOVE_X_max82(c, 35, X_RIGHT);
		}
	
	
		// Gehe 190,7mm hoch. Hierfür ist der Laser optimal kalibriert
		SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
		SMAC_ADD_MOVE_Y_max82(c, 80, Y_UP);
		SMAC_ADD_MOVE_Y_max82(c, 30, Y_UP);
		SMAC_ADD_MOVE_Y_sub_mm_max255(c, 7, Y_UP);
}


// Generiere Sequenzen um Ballon-Mitte zu finden
void GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	
	// Füge 150x1mm Abtastpunkte hinzu, um Mitte zu finden
	SMAC_ADD_GO_MEASUREMENT_RANGE(c);
	for (uint8_t i = 0; i < BALLOON_SHELL_CENTER_FIND_STEPS; i++)
	{
		SMAC_ADD_FIND_BALLOON_SHELL_CENTER_STEP(c);
	}
	
	SMAC_ADD_MOVE_X_BIG(c, BALLOON_SHELL_CENTER_FIND_STEPS*1, X_LEFT);
	
	// Trigger Sequenzen
	SMAC_END_DECLARE_MOTION_SEQUENCE(c);
}

// Generiere Sequenzen 1x32 Modi-Ballon auszumessen
void GO_MEASUREMENT_RADII_BALLOON_SINGLE_ROW(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	// Generiere Sequenz Fahre Ballon Mitte die zuvor ausgemessen wurde.
	// Kann mehr als 80 mm sein, deswegen move_BIG
	SMAC_ADD_MOVE_X_BIG(c, c->balloon_shell_center_mm, X_RIGHT);

	// Generiere Sequenzen, um (Median)-Radius zu messen
	for (uint8_t i = 0; i < 15; i++)
	{
		SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	}
	
	// 32 Messpunkte für Buchstaben Obere Kante
	SMAC_ADD_MOVE_X_max82(c, 4*c->pixel_unit_mm, X_LEFT);
	for (uint8_t j = 0; j < 32; j++)
	{
		SMAC_ADD_MEASURMENT_POINT_ROW_1(c);
	}
	
	// Seqeuenz für Ausgangsposition
	for (uint8_t l = 0; l < 32; l++)
	{
		SMAC_ADD_MOVE_Z_max233(c, 5*c->pixel_unit_mm, Z_CLOCKWISE);
	}
	
	// Da hier die Gefahr besteht, dass wir mehr als 80mm verfahren.
	SMAC_ADD_MOVE_X_BIG(c, 8*c->pixel_unit_mm, X_RIGHT); // Fahre Reihe 2
	
	// 32 Messpunkte für Buchstaben Untere Kante
	for (uint8_t k = 0; k < 32; k++)
	{
		SMAC_ADD_MEASURMENT_POINT_ROW_2(c);
	}
	
	// Seqeuenz für Ausgangsposition
	for (uint8_t n = 0; n < 32; n++)
	{
		SMAC_ADD_MOVE_Z_max233(c, 5*c->pixel_unit_mm, Z_CLOCKWISE);
	}
}

// Generiere Sequenzen 2x16 Modi-Ballon auszumessen
void GO_MEASUREMENT_RADII_BALLOON_DOUBLE_ROW(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);
	// Sequenz für Fahre Ballon Mitte die zuvor ausgemessen wurde.
	// Kann mehr als 80 mm sein, deswegen move_BIG
	SMAC_ADD_MOVE_X_BIG(c, c->balloon_shell_center_mm, X_RIGHT);

	// Generiere Sequenzen, um (Median)-Radius zu finden
	for (uint8_t i = 0; i < 15; i++)
	{
		SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	}
	
	// Fahre Reihe 1
	SMAC_ADD_MOVE_X_max82(c, 8*c->pixel_unit_mm, X_LEFT);
	SMAC_ADD_MOVE_X_max82(c, 1, X_LEFT);

	// 16 Messpunkte für die 16 möglichen Buchstaben der Reihe 1
	for (uint8_t j = 0; j < 16; j++)
	{
		SMAC_ADD_MEASURMENT_POINT_ROW_1(c);
	}
	
	// Seqeuenz für Ausgangsposition
	for (uint8_t l = 0; l < 16; l++)
	{
		SMAC_ADD_MOVE_Z_max233(c, 5*c->pixel_unit_mm, Z_CLOCKWISE);
	}
	
	// Da hier die Gefahr besteht, dass wir mehr als 80mm verfahren.
	SMAC_ADD_MOVE_X_BIG(c, (16*c->pixel_unit_mm)+2, X_RIGHT);

	// 16 Messpunkte für die 16 möglichen Buchstaben der Reihe 2
	for (uint8_t k = 0; k < 16; k++)
	{
		SMAC_ADD_MEASURMENT_POINT_ROW_1(c);
	}
	
	// Seqeuenz für Ausgangsposition
	for (uint8_t n = 0; n < 16; n++)
	{
		SMAC_ADD_MOVE_Z_max233(c, 5*c->pixel_unit_mm, Z_CLOCKWISE);
	}
	
	// Sequenz, um auf Buchstabe 1, Reihe 1 zu fahren
	SMAC_ADD_MOVE_X_max82(c, (8*c->pixel_unit_mm)+2, X_LEFT);
}

// Generiere Sequenzen, um Radius der Walze zu messen
void GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER(StepperMotionAxisController* c)
{
	SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(c);

	// Sequenzen, um in Messbereich zu fahren
	SMAC_ADD_GO_MEASUREMENT_RANGE(c);

	// Sequenz, um einen kurzen Snapshot aufzunehmen, wichtig für spätere Validierung der Walze
	SMAC_ADD_VERIFY_DIST_CYL(c);

	// Generiere Sequenzen, um (Median)-Radius zu finden.
	for (uint8_t i = 0; i < 15; i++)
	{
		SMAC_ADD_MEAS_RADIUS_MEDIAN_STEP(c);
	}

	// trigger
	SMAC_END_DECLARE_MOTION_SEQUENCE(c);
}

// Generiere Sequenzen, um in Reihe 2 zu Fahren. Entweder nach 16 Buchstaben oder nach forcierten Newline
void SMAC_GO_BEGINNING_BALLOON_ROW_2_DOUBLE_ROW(StepperMotionAxisController* controller, uint8_t char_count)
{
	uint16_t left_to_drive = (char_count*5)*controller->pixel_unit_mm;
	while(left_to_drive!=0)
	{
		if (left_to_drive >= 40)
		{
			SMAC_ADD_MOVE_Z_max233(controller, 40, Z_CLOCKWISE);
			left_to_drive -= 40;
		}
		else
		{
			SMAC_ADD_MOVE_Z_max233(controller, left_to_drive, Z_CLOCKWISE);
			left_to_drive -= left_to_drive;
		}
		
	}
	SMAC_ADD_MOVE_X_max82(controller, (8*controller->pixel_unit_mm)+2, X_RIGHT);
}

// Hilfsfunktion, um variable Fahrsequenzen zu puffern, bei welcher unbekannt ist ob sie größer 8cm ist.
void SMAC_ADD_MOVE_X_BIG(StepperMotionAxisController* c, uint8_t move_mm, XDir dir)
{
	while(move_mm != 0)
	{
		if (move_mm >= 80)
		{
			
			SMAC_ADD_MOVE_X_max82(c, 80, dir);
			move_mm -= 80;
		}
		else
		{
			SMAC_ADD_MOVE_X_max82(c, move_mm, dir);
			move_mm -= move_mm;
		}
	}
	
}


// Hilfsfunktion, um Fahrsequenzen zu erzeugen, die mm und 1/10 mm fahren
// Bsp. 164 entspricht 16.4 mm 
void SMAC_ADD_MOVE_Y_FLOAT_MM(StepperMotionAxisController*c, uint16_t move, YDir dir)
{
	uint8_t mm = move / 10;
	uint8_t tenth_mm = move - mm * 10;
	
	if (mm != 0)
	{
		SMAC_ADD_MOVE_Y_max82(c, mm, dir);
	}
	
	if (tenth_mm != 0)
	{
		SMAC_ADD_MOVE_Y_sub_mm_max255(c, tenth_mm, dir);
	}
}

// Berechne aus dem radius, den Kalibrieruingsfaktor steps_per_mm_z für Z-Drehungen
uint8_t calc_steps_per_mm_Z(uint16_t radius_mm)
{
	
	uint16_t presc = Z_STEPS_PER_MM_PRESCALER;
	uint32_t scaled_radius = 10000 / radius_mm;
	uint32_t temp = (uint32_t)(presc * scaled_radius);
	uint16_t downscaled = (uint16_t)(temp / 1000);
	uint8_t res = (downscaled + 5) / 10; // round
	return res;
}

// Berechne aus dem Kalibrieruingsfaktor steps_per_mm_z die tempöräre Stepperfrequenz der Z-Achse, um Diagonale Fahrsequenzen zu ermöglichen
uint16_t calc_temp_freq_Z_for_diagonal_move(uint8_t steps_per_mm_Z)
{
	uint16_t freq = (uint32_t)((uint32_t)(FREQ_STEPPER_X * STEPS_PER_MM_X) * 10)  / steps_per_mm_Z;
	return (freq + 5) / 10;
}

// Berechne den Umrechnungsfaktor von pixeln zu mm, aus Radius
uint8_t calc_pixel_unit_width_mm(uint8_t radius_obj_mm)
{
	uint16_t upscaled_res= PIXEL_CALC_PRESC_UPSCALED_1000 * radius_obj_mm;
	// Always round down
	uint8_t res_floored = upscaled_res / 1000;
	return res_floored;
}

// Berechne Ballon-Umfang aus radius. Scaled für bessere Genaugikeit
uint16_t balloon_perimeter(uint8_t radius)
{
	uint32_t temp = TWO_PI_SCALED * radius;
	return temp / 100;
}

// Initialisiert den SMAC
void SMAC_init(StepperMotionAxisController*c, uint8_t object_radius_z_mm_, ObjectType type)
{
	c->object_radius_mm = object_radius_z_mm_;
	c->pixel_unit_mm = calc_pixel_unit_width_mm(object_radius_z_mm_)-1;
	c->steps_per_mm_z = calc_steps_per_mm_Z(object_radius_z_mm_);
	c->state = STATE_IDLE;
	c->objtype = type;
	
	if (type == OBJECT_BALLOON )
	{
		c->pixel_unit_mm = 2;
	}
	SMAC_init_XYZ();
}
