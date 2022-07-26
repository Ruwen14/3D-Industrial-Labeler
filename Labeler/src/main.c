/*
 * Labeler.c
 *
 * Created: 18.05.2022 12:12:03
 * Author : ruwen
 */ 


#include "lib/movement.h"
#include "lib/constants.h"
#include "lib/uart.h"
#include "lib/limit_switches.h"
#include "lib/ADCLaser.h"
#include "lib/LCD.h"
#include "lib/StepperMotionAxisController.h"
#include "lib/DrawingAlphabetLib.h"
#include "lib/controll_buttons.h"

#include <avr/interrupt.h>

// SMAC. Koordinator f�r die Motoransteuerung. Ich bin mir ziemlich sicher, dass volatile hier n�tig ist, da die ISR den controller modifizieren kann (Ruwen).
volatile StepperMotionAxisController controller = {1, 0, 0, 0, 0, 0, 0, 3, 3, 3, 55, 9, 0, 640, 0, {}, {}, STATE_IDLE, OBJECT_CYLINDER , NONE,{}};

int main(void)
{
	uart_init();
	lcd_init();
	SMAC_init(&controller, 55, OBJECT_CYLINDER);
	ADC_Laser_init();
	limit_swiches_init();
	controll_buttons_init();
	
	// Koordiniert die UART-Nutzereingabe. Signalisiert, wenn UART-Nutzereingabe vollst�ndig
	UserInputHandler input_handler = {0, 16, INPUT_INCOMPLETED, {}};

	// FLAGS, um ENDLOSVERHALTEN des Programmes zu erm�glichen
	uint8_t was_object_type_choosen = 0;
	uint8_t is_object_type_question_published_on_lcd = 0;
	uint8_t is_balloon_subtype_question_published_on_lcd = 0;
	uint8_t was_button_released = 0;
	uint8_t is_input_published_on_lcd = 0;
	uint8_t is_drawing = 0;
	uint8_t was_instructed_home = 0;
	uint8_t character_counter = 0;
	uint8_t was_instructed_meas_radius = 0;
	uint8_t was_instructed_find_center_point_balloon = 0;
	uint8_t was_instructed_meas_radius_cylinder = 0;
	uint8_t was_instructed_measure_balloon_radii = 0;
	uint8_t measured_distance_prev = 0;
	uint8_t uart_graph_started = 0;
	uint8_t is_program_resetted = 1;

	while(1)
	{	
		// Startet das Programm neu
		// Tritt ein:
		//		1)  wenn ein kompletter Schreibzyklus (32-Zeichen) durchgef�hrt wurden ist.
		//		2)  wenn eingespanntes und ausgew�hltes Objekt nicht zusammenpassen
		if (!is_program_resetted)
		{
			// Reset FLAGS
			was_object_type_choosen = 0;
			is_object_type_question_published_on_lcd = 0;
			is_balloon_subtype_question_published_on_lcd = 0;
			was_button_released = 0;
			is_input_published_on_lcd = 0;
			is_drawing = 0;
			was_instructed_home = 0;
			was_instructed_meas_radius = 0;
			was_instructed_find_center_point_balloon = 0;
			was_instructed_meas_radius_cylinder = 0;
			was_instructed_measure_balloon_radii = 0;
			measured_distance_prev = 0;
			uart_graph_started = 0;
			
			// Standardtyp->Walze, da wir sonst direkt in ein Untermen� hereinspringen
			controller.objtype = OBJECT_CYLINDER;
			
			// Resete den Handler, der f�r die UART-Nutzereingabe zust�ndig ist.
			UserInputHandler_reset_input(&input_handler);
		
			// Entleere den derzeitigen Fahrtsequenzpuffer. Rein Sicherheitshalber. Er sollte an dieser Stelle immer leer sein
			FIFOSeqBuffer_delete(&controller.sequencebuffer); 
			
			// Resete die getrackten Positionen die auf dem DebugGraph ausgegeben werden.
			SMAC_reset_positions(&controller);
			
			// GGf. restlichen Sendepuffer leeren
			EMPTY_UART
			
			// String-Buffer des InputHandlers Nullen
			RESET_INPUT_BUF
			
			
			is_program_resetted = 1; // Programm ist resettet.
		}
		
		// Garantiert das der Schlitter IMMER in der Referenzposition (HOME) startet. 
		if (!was_instructed_home)
		{
			lcd_reset();
			
			// Bei falsch eingespannten Objekt, wird Heimgefahren. Gebe dem Nutzer hier einen Tipp, warum abgebrochen wird.
			if (controller.is_object_valid)
			{
				lcd_text("Return Home...");
			}
			else
			{
				lcd_text("INVALID OBJECT! Return Home...");
			}
			
			SMAC_return_home(&controller);
			character_counter = 0;
			was_instructed_home = 1;
		}
		
	
		if (!was_object_type_choosen)
		{
			
			// Garantiert ATOMAREN Zugriff. Ich darf den SMAC (mit ggf. 16Bit vars) nur modifizeren wenn der One-Shot-Timer (TIMER5_COMPA_vect) aus ist!
			if (controller.state == STATE_PROCESSING) // 8-Bit Operation. Intrinsisch Atomar
			{
				continue;
			}
			
			// Einmaliges Anzeigen. St�ndiges updaten des LCD mit gleichen Text ist sinnlos & teuer. 
			if (!is_object_type_question_published_on_lcd)
			{
				DISABLE_UART_ISR; // Schalte PositionsGraph bis Malen aus. Nutzer soll im HTerm nicht mit den Achs-Positionen bombadiert werden
				lcd_reset();
				lcd_text("^CYLI | BALLOON^CHOOSE OBJECT...");
				is_object_type_question_published_on_lcd = 1;
			}
			
			// Nutzer w�hlt Objekttypus BALLON
			if (IS_VERIFY_BUTTON_PRESSED || controller.objtype == OBJECT_BALLOON) 
			{
				controller.objtype = OBJECT_BALLOON;
				controller.pixel_unit_mm_y_up = 8;    // Default-Wert. Wird Sp�ter f�r jeden Buchstaben just-in-time geupdated.
				controller.pixel_unit_mm_y_down = 15; // Standard-Wegfahrweg des Stiftes. Gro� genug damit Stift nie Ballon streift
				
				// Einmaliges Anzeigen. St�ndiges updaten des LCD mit gleichen Text ist sinnlos & teuer.
				if (!is_balloon_subtype_question_published_on_lcd)
				{
					lcd_reset();
					lcd_text("HOW MANY ROWS?  ^1x32   |  2x16^");
					is_balloon_subtype_question_published_on_lcd = 1;
					
				}
				
				// Knopf entprellen f�r Arme
				if (!IS_VERIFY_BUTTON_PRESSED && was_button_released!=100)
				{
					was_button_released++;
					continue;
				}
				
				// Ist Knopf entprellt?
				if (was_button_released==100)
				{
					
					// Nutzer w�hlt Ballon-Sub-Modi 2x16 Buchstaben	
					if (IS_VERIFY_BUTTON_PRESSED)
					{
						controller.balloonsubtype = BALLOON_DOUBLE_ROW;
						was_object_type_choosen = 1;
						_DR_SETUP
						lcd_reset();
						lcd_text("CHOSEN 2x16,    AWAIT INPUT...");
					}

					// Nutzer w�hlt Ballon-Sub-Modi 1x32 Buchstaben
					else if (IS_CANCEL_BUTTON_PRESSED)
					{
						controller.balloonsubtype = BALLOON_SINGLE_ROW;
						was_object_type_choosen = 1;
						_SR_SETUP
						lcd_reset();
						lcd_text("CHOSEN 1x32,    AWAIT INPUT...");
					}
				}
				
				
			}
			
			// Nutzer w�hlt Objekttypus Walze
			else if (IS_CANCEL_BUTTON_PRESSED)
			{
				controller.objtype = OBJECT_CYLINDER;
				was_object_type_choosen = 1;
				lcd_reset();
				lcd_text("AWAIT INPUT...");
			}
			continue;
		}
		
		// Blockiere registrierung der UART-Nutzereingabe, BIS Objekttyp gew�hlt wurde.
		if (!was_object_type_choosen)
		{
			continue;
		}
		
		if ((!is_drawing) && was_instructed_home)
		{
			// Pollt regelm��ig die UART-Nutzereingabe (HTerm), bis der Buffer (32 Zeichen) voll ist. 
			// Oder ein Nullterminator registriert wird.
			UserInputHandler_poll_input(&input_handler);

			// Ist Nutzereingabe abgeschlossen?
			if (input_handler.status == INPUT_COMPLETED)
			{
				// �bertr�gt die UART-Nutzereingabe auf das LCD. Macht dies nur einmalig pro Nutzereingabe, um
				// redundante Aufrufe zu eliminieren.
				if (!is_input_published_on_lcd)
				{
					lcd_reset();
					lcd_text(input_handler.input_buffer);
					is_input_published_on_lcd = 1;
				}
			
				// Schreibprozess wird auf Knopfdruck (Gr�n) gestartet.
				// �berpr�ft die Eingabe (HTerm) auf unterst�tzte Zeichen. Sofern invalide Zeichen -> wird Fehlermeldung ausgegeben und der Nutzer
				// muss eine neue Eingabe t�tigen. Ist die Eingabe valide ist, wird in den Draw-Modus geswitched (is_drawing = 1) und das eigentliche 
				// Schreiben beginnt.
				if (IS_VERIFY_BUTTON_PRESSED)
				{
					// Eingabe-Validierung
					uint8_t valid = validate_input_stream(input_handler.input_buffer);
				
					if (valid)
					{
						// Ready to Start Drawing
						is_drawing = 1;
						
						// Startet erst hier den DebugGraph der Positionen. Zuvor ist er ausgeschaltet, da der Nutzer sonst im HTerm bombadiert wird.
						if (!uart_graph_started)
						{
							uart_interrupt_init();
							uart_graph_started = 1;
						}
						
					}
			
				}
					
				// Resettet die UART-Nutzereingabe. Der Nutzer kann nun erneut eine Eingabe (HTerm) t�tigen -> Solange bis UART-Nutzereingabe valide!
				else if (IS_CANCEL_BUTTON_PRESSED)
				{
					UserInputHandler_reset_input(&input_handler);
					is_input_published_on_lcd = 0;
					lcd_reset();
					lcd_text("AWAIT INPUT...");
				}
			}
		
		}
		
		// Nutzereingabe vollst�ndig und valide.
		else if(is_drawing)
		{

			// Mit dieser Abfrage garantieren wir ATOMAREN-Zugriff, da die modifizierende ISR bei (true) ausgeschalten ist.
			// Gleichzeitig ist der SMAC im IDLE-Modus wenn die gepufferten Abfolgen [BEGIN_...(), ..., SMAC_END_DECLARE_MOTION_SEQUENCE...()] der vorherigen
			// Sequenzen abgearbeitet wurden, sonst nicht.
			// Mit dieser Abfrage kann also eine Art State-Machine realisiert werden.
			if (!(controller.state == STATE_PROCESSING))
			{	
				// Puffer Sequenzen, um das Eingespannte Objekt auszumessen & in Schreibposition zu gehen. Der gesamte Prozess findet vor dem eigentlichen Schreiben statt.
				if (!was_instructed_meas_radius)
				{
					// Bei Walze
					if (controller.objtype == OBJECT_CYLINDER)
					{
						// Puffer Sequenzen um den (Median)-Radius der Walze auszumessen.
						if (!was_instructed_meas_radius_cylinder)
						{
							GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER(&controller);
							was_instructed_meas_radius_cylinder = 1;
						}
						
						// In diese Abfrage kann erst reingesprungen werden wenn der SMAC idled --> Merke State-Machine
						else if (was_instructed_meas_radius_cylinder)
						{
							// GO_MEASURMENT_RANGE_AND_FIND_RADIUS_CYLINDER hat bereits herausgefunden ob Objekt valide ist.
							// Ist es nicht valide, so resette Programm und fahre in Heimposition.
							if (!controller.is_object_valid)
							{
								is_program_resetted = 0;
								continue;
							}
							
							// Puffer Fahresequenz Stift in Schreibposition
							SMAC_ADD_MOVE_X_max82(&controller, OFFSET_PEN_LASER, X_RIGHT);
							SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
							SMAC_ADD_MOVE_Y_DRAWING_LEVEL(&controller);
							SMAC_ADD_MOVE_Y_max82(&controller, controller.pixel_unit_mm_y_down, Y_DOWN);
							SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
							was_instructed_meas_radius = 1;
						}
					}
					
					// Bei Ballon
					else if (controller.objtype == OBJECT_BALLOON)
					{
						// Puffer Sequenzen, um Mittelpunkt des Ballones zu finden.
						if (!was_instructed_find_center_point_balloon)
						{
							GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER(&controller);
							was_instructed_find_center_point_balloon = 1;
							
						}
						
						// In diese Abfrage kann erst reingesprungen werden wenn der SMAC idled --> Merke State-Machine
						else if (!(was_instructed_measure_balloon_radii) && was_instructed_find_center_point_balloon)
						{
							// GO_MEASURMENT_RANGE_AND_FIND_BALLOON_CENTER hat bereits herausgefunden ob Objekt valide ist.
							// Ist es nicht valide, so resette Programm und fahre in Heimposition.
							if (!controller.is_object_valid)
							{
								is_program_resetted = 0;
								continue;
							}
							
							// Bei 1x32 Modi. Puffer die Sequenzen um den Ballon auszumessen.
							if (controller.balloonsubtype == BALLOON_SINGLE_ROW)
							{
								GO_MEASUREMENT_RADII_BALLOON_SINGLE_ROW(&controller);
							}
							
							// Bei 2x16 Modi. Puffer die Sequenzen um den Ballon auszumessen.
							else if (controller.balloonsubtype == BALLOON_DOUBLE_ROW)
							{
								GO_MEASUREMENT_RADII_BALLOON_DOUBLE_ROW(&controller);
							}
							
							
							// Puffer Fahresequenz Stift in Schreibposition
							SMAC_ADD_MOVE_X_max82(&controller, OFFSET_PEN_LASER, X_RIGHT);
							SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
							was_instructed_measure_balloon_radii = 1;
						}
						
						// In diese Abfrage kann erst reingesprungen werden wenn der SMAC idled --> Merke State-Machine
						else if (was_instructed_measure_balloon_radii)
						{	
							// Berechne temp�r�re Z-Stepperfrequenz f�r diagonales Fahren. An dieser Stelle ist Radius & Co bereichts bekannt.
							controller.temp_ocra_diagonal = calc_temp_freq_Z_for_diagonal_move(controller.steps_per_mm_z);
								
							SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
							was_instructed_meas_radius = 1; // Alles Ready f�rs Schreiben
						}
						
					}
			
				}
				
				// Dieses Statement tritt nur ein, wenn das Objekt ERFOLGREICH ausgemessen wurde und der Schlitten sich in Schreibposition befindet.
				else
				{
					// Holt iterativ die Buchstaben der einstigen Nutzereingabe
					char character = input_handler.input_buffer[character_counter];
					
					// Puffer Malsequenzen nur wenn kein Nullterminator entdeckt wird, da sonst Ende.
					if (!(character == '\0'))
					{
						
						// Spezialisierung bei Ballon. Hier m�ssen wir zuvor den Zustellweg des Stiftes aus den Messungen ermitteln
						if (controller.objtype == OBJECT_BALLOON)
						{
							uint8_t measured_dist = 0;
							uint8_t contact_dist = 0;
							
							if (controller.balloonsubtype == BALLOON_SINGLE_ROW)
							{
								// Vergleiche Distanz Ober-/Untere Kante pro Buchstabe, und nimmt die die gr��er ist.
								if (controller.meas_distances_row_1[character_counter] > controller.meas_distances_row_2[character_counter])
									measured_dist = (controller.meas_distances_row_1[character_counter] + 5) / 10;
								else
									measured_dist = (controller.meas_distances_row_2[character_counter] + 5 ) / 10;
							}
							
							
							else if (controller.balloonsubtype == BALLOON_DOUBLE_ROW)
							{		
								// Nimmt die Distanz die f�r jeden Buchstabe gemessen wurde
							
								// Haben die 16-Buchstaben pro Reihe, einfach in das erste Array gef�llt, da der Index-Zugriff somit weniger Kopfschmerz bereitet.
								measured_dist = (controller.meas_distances_row_1[character_counter] + 5) / 10;
							}
							
							// Beim allerersten Buchstabe, m�ssen wir einfach nur die gemessene Distanz nehmen
							if (character_counter == 0)
							{
								contact_dist = measured_dist;
							}
							
							// Ansonsten m�ssen wir nat�rlich die gemessene Distanz des vorherigen Buchstabens miteinbeziehen
							// um den derzeitigen Zustellweg des Buchstabens zu ermitteln.
							else
							{
								int16_t temp = measured_dist - measured_distance_prev; // Zeitliche Atomarer-Zugriff
								contact_dist = temp + controller.pixel_unit_mm_y_down;
							}
							
							
							// Aktualisiere die Variable f�r den Zustellweg des Stiftes
							measured_distance_prev = measured_dist;
							controller.pixel_unit_mm_y_up = contact_dist;
						}
										
						// Hier werden die eigentlichen Malsequenzen f�r eines jeden Buchstabes gepuffert.
						SMAC_BEGIN_DECLARE_MOTION_SEQUENCE(&controller);
						SMAC_dispatch_character_function(&controller, character); // Dispatch Malfunktion.
 					
						
						if (controller.balloonsubtype == BALLOON_DOUBLE_ROW)
						{
							// Sofern wir an der Grenze zur zweiten Reihe sind (entweder forciert durch # oder nat�rlich nach 16 Zeichen)
							// puffern wir noch eine Bewegung in die Zweite Reihe nach.
							if (character_counter == input_handler.second_row_at - 1)
							{
								SMAC_GO_BEGINNING_BALLOON_ROW_2_DOUBLE_ROW(&controller, character_counter+1);
								
								character_counter = 15;
							}
						}
					 						
						if (controller.objtype == OBJECT_CYLINDER)
						{

							// Sofern wir an der Grenze zur zweiten Reihe sind (entweder forciert durch # oder nat�rlich nach 16 Zeichen)
							// puffern wir noch eine Bewegung in die Zweite Reihe nach.
							if (character_counter == input_handler.second_row_at - 1)
							{
								SMAC_GO_BEGINNING_CYLINDER_ROW_2(&controller, character_counter+1);
						
								character_counter = 15;

							}
						}
						
						// ERST HIER STOSSEN wir die Entleerung des Puffers und somit das eigentliche MALEN erst an. Alles davor war rein Puffern von Fahrtsequenzen
						// Letztentlich Puffern wir also pro Buchstabe. Und puffern erst nach wenn der Buchstabe gemalt wurde.
						SMAC_END_DECLARE_MOTION_SEQUENCE(&controller);
					}
					
					// Nullterminator wurde detektiert und signalisiert somit den letzten Buchstabe. Das Programm kann nun neugestartet werden.
					else
					{
						// Force reset
						is_program_resetted = 0;
						
					}
					++character_counter; 
				
				}
				
			}
		}
		
	}
}





// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES BEGIN
// ***********************************************************************************************************************

// ISR - "ONE-SHOT" Timer
// Verantwortlich f�r das Ausschalten der Stepper-PWM's nach dem Prinzip des One-Shot-Timer's. Koordiniert ggf. n�chste Fahrsequenz,
// sofern welche gepuffert sind.
ISR(TIMER5_COMPA_vect)
{
	// F�r eine ISR ist dies zugegebenermassen vergleichsweise viel Quellcode. Es sei aber bedacht, dass diese
	// ISR den �bergang zweier Fahrtsequenzen koordiniert. Dies ist weder zeitkritisch und wird auch nicht h�ufig (ISR-typisch) aufgerufen.
	
	// Sofort disablen --> Maximale Genauigkeit & um ggf. Rekursion und damit Stack-Overflow zu verhindern
	SMAC_disable_XY_pwm();
	SMAC_disable_Z_pwm();
	SMAC_disable_multi_one_shot_timer();
	OCR4A = 249; // Resettet Z-Stepperfrequenz, sofern wir diagonal gefahren sind.

	
	// Sto�e schnell noch n�chste Fahrtsequenz an. 
	if (!(FIFOSeqBuffer_empty(&controller.sequencebuffer)))
	{
		MotionSequence seq;
		FIFOSeqBuffer_pop(&controller.sequencebuffer, &seq);
		SMAC_start_new_motion_sequence(&controller, &seq);
	}
	else
	{
		// Ansonsten signalisiere das der SMAC nicht mehr arbeitet und die ISR aus ist. Kann genutzt werden um ATOMAREN Zugriff
		// zu koordinieren und State-Maschines zu realisieren.
		controller.state = STATE_IDLE;
	}

}


// ISR's f�r Endlagenschalter. 
// Werden unteranderem f�r die Heimkehrsequenz genutzt.

// Endlagenschalter Unten.
ISR(INT3_vect)
{
	
	// Invertiert denken, da Taster gedr�ckt wenn fallende Flanke!
	if (!IsBitSet(PIND, Y_BOTTOM_LIM_PIN))
	{
		// Switche in Linksbewegung des Motors f�r Heimkehrsequenz.
		// Keine Sorge der Endlagenschalter Link wird dann alles ausschalten
		TCNT3 = 0; // X-Reset Counter
		TCNT1 = 0; // Y-Reset Counter
		BitClear(PORTA, Y_DIR);
		BitClear(PORTA, X_DIR);
		controller.state = STATE_PROCESSING;
		
	}
	
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den sonst Interrupt 2x statt 1x triggern;
	prevent_bounce();
}

// Endlagenschalter Links
ISR(INT2_vect)
{
	
	// Invertiert denken, da Taster gedr�ckt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_LEFT_LIM_PIN))
	{
		// Schalte Motor+PWM+Timer+Buffer(einfach alles) aus, da wir an der Referenzposition sind.
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer); // L�sche die ggf. noch �brigen Sequenzen
		controller.state = STATE_IDLE;
		
	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den sonst Interrupt 2x statt 1x triggern;
	prevent_bounce();
	
	
}

// Endlagenschalter Rechts
ISR(INT0_vect)
{
	// Invertiert denken, da Taster gedr�ckt wenn fallende Flanke!
	if (!IsBitSet(PIND, X_RIGHT_LIM_PIN))
	{
		// Schalte Motor+PWM+Timer+Buffer(einfach alles) aus.
		// Dieser Fall tritt nur dann auf, wenn etwas m�chtig schief gelaufen ist under Schlitten am rechten Anschlag ist.
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer);
		controller.state = STATE_PROCESSING;

		
	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den sonst Interrupt 2x statt 1x triggern;
	prevent_bounce();
	
}
	
// Endlagenschalter oben
ISR(INT1_vect)
{
	
	// Invertiert denken, da Taster gedr�ckt wenn fallende Flanke!
	if (!IsBitSet(PIND, Y_TOP_LIM_PIN))
	{
		// Schalte Motor+PWM+Timer+Buffer(einfach alles) aus.
		// Dieser Fall tritt nur dann auf, wenn etwas m�chtig schief gelaufen ist under Schlitten am obigen Anschlag ist.
		SMAC_disable_XY_pwm();
		SMAC_disable_Z_pwm();
		SMAC_disable_multi_one_shot_timer();
		FIFOSeqBuffer_delete(&controller.sequencebuffer);
		controller.state = STATE_PROCESSING;

	}
	// Leider brauchen wir diese Funktion weil die Endlagenschalter prallen und den sonst Interrupt 2x statt 1x triggern;
	prevent_bounce();
}



// ISR f�r Positionsgraphen �ber DebugTerminal.
// Diese ISR erlaub nested Interrupts f�r z.B. den One-Shot-Timer, da dieser AUF KEINEN FALL verz�gert werden darf.
// Weiterhin d�rfen auch hier die Endlagenschalter-ISR zwischenspringen, da diese ggf. mechanische Sch�den verhindern.
ISR(TIMER0_COMPA_vect)
{
	static uint8_t update_counter = 0;
	
	// Vorsichtshalber: Verhindert einen Stack-Overflow durch Rekursion
	DISABLE_UART_ISR;

	// Erlaube nested Interrupts! --> TIMER5_COMPA_vect darf nicht warten!!!
	sei();

	update_counter++;

	// Update des Graphen alle ~ 21fps(3*16ms) --> Zum Vergleich, Kinofilm l�uft mit 24fps
	if (update_counter == 3)
	{
		// ToDo an RUWEN (PRIO NIEDRIG): While(SendeBufferFull...) ggf. durch Chaining von Transmit Complete Interrupt ersetzen: (It occures when the previous transmit completed).
		uart_send(controller.position_z_lower_byte);
		uart_send(controller.position_z_upper_byte);

		uart_send(controller.position_x_lower_byte);
		uart_send(controller.position_x_upper_byte);
		
		uart_send(controller.position_y_lower_byte);
		uart_send(controller.position_y_upper_byte);

		update_counter = 0;
	}
	
	// Disable nested Interrupt, da wir nun die UART_ISR reaktivieren
	cli();
	ENABLE_UART_ISR;
	
	// Quelle: https://www.mikrocontroller.net/articles/AVR-GCC-Tutorial#Unterbrechbare_Interruptroutinen
}

// ***********************************************************************************************************************
// INTERRUPT SERVICE ROUTINES END
// ***********************************************************************************************************************




