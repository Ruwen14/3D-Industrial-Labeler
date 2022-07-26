/*
 * DrawingAlphabetLib.c
 *
 * Created: 17.06.2022 15:46:42
 *  Author: Michele
 */ 


#include "DrawingAlphabetLib.h"


void writeLetterA(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe A
	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	vertical_up(7);
	diagonal_right_up(1);
	horizontal_right(2);
	diagonal_left_down(1);
	vertical_down(7);
	vertical_up(4);
	horizontal_left(4);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(4);
}

void writeLetterB(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe B
	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(3);
	diagonal_left_down(1);
	vertical_down(2);
	diagonal_right_down(1);
	horizontal_left(3);
	horizontal_right(3);
	diagonal_left_down(1);
	vertical_down(2);
	diagonal_right_down(1);
	horizontal_left(3);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(5);

}

void writeLetterC(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe C
	horizontal_right(4);
	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	horizontal_left(3);
	diagonal_left_up(1);
	vertical_up(6);
	diagonal_right_up(1);
	horizontal_right(3);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterD(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe D
	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(3);
	diagonal_left_down(1);
	vertical_down(6);
	diagonal_right_down(1);
	horizontal_left(3);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
}

void writeLetterE(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe E
	horizontal_right(4);
	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	horizontal_left(4);
	vertical_up(4);
	horizontal_right(2);
	horizontal_left(2);
	vertical_up(4);
	horizontal_right(4);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}




void writeLetterF(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe F
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(4);
	horizontal_right(2);
	horizontal_left(2);
	vertical_up(4);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterG(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe G
	horizontal_right(2);
	vertical_up(4);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(2);
	vertical_down(3);
	diagonal_right_down(1);
	horizontal_left(2);
	diagonal_left_up(1);
	vertical_up(6);
	diagonal_right_up(1);
	horizontal_right(2);
	diagonal_left_down(1);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(7);
}

void writeLetterH(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe H
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	vertical_down(4);
	horizontal_right(4);
	vertical_up(4);
	vertical_down(8);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeLetterI(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe I
		
	horizontal_right(1);	
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	horizontal_right(2);
	
	horizontal_left(1);	
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	
	horizontal_left(1);
	
	horizontal_right(2);
	

	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	vertical_down(8);
	horizontal_right(2);
}

void writeLetterJ(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe J
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_down(7);
	diagonal_right_down(1);
	horizontal_left(2);
	diagonal_left_up(1);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(1);

}

void writeLetterK(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe K
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	vertical_down(4);
	diagonal_right_up(4);
	diagonal_right_down(4);
	diagonal_left_down(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeLetterL(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe L
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(8);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeLetterM(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe M
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	diagonal_left_down(2);
	diagonal_right_up(2);
	vertical_down(8);
		
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeLetterN(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe N

	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	diagonal_left_down(4);
	vertical_up(4);
	vertical_down(8);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
		
}

void writeLetterO(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe O
	vertical_up(1);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(6);
	diagonal_right_up(1);
	horizontal_right(2);
	diagonal_left_down(1);
	vertical_down(6);
	diagonal_right_down(1);
	horizontal_left(2);
	diagonal_left_up(1);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(1);
}

void writeLetterP(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe P
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(3);
	diagonal_left_down(1);
	vertical_down(2);
	diagonal_right_down(1);
	horizontal_left(3);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(4);
}

void writeLetterQ(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe Q
	vertical_up(1);
		
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(6);
	diagonal_right_up(1);
	horizontal_right(2);
	diagonal_left_down(1);
	vertical_down(7);
	diagonal_left_up(2);
	diagonal_left_down(2);
	horizontal_left(3);
	diagonal_left_up(1);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(1);
}

void writeLetterR(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe R
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(3);
	diagonal_left_down(1);
	vertical_down(2);
	diagonal_right_down(1);
	horizontal_left(3);
	diagonal_left_down(4);

		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);					//Fahrt auf Referenzlinie
}

void writeLetterS(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe S
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(3);
	diagonal_right_up(1);
	vertical_up(2);
	diagonal_left_up(1);
	horizontal_left(2);
	diagonal_left_up(1);
	vertical_up(2);
	diagonal_right_up(1);
	horizontal_right(3);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterT(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe T
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
// 	pen_contact_y(1);
	pen_contact_y(1);

		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	horizontal_left(2);
	vertical_down(8);
		
	//Absetzen in y
// 	pen_drop_y(1);
	pen_drop_y(1);
	
		
	//Fahrt zum Referenzpunkt
	horizontal_right(3);
}

void writeLetterU(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe U
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(8);
	horizontal_right(4);
	vertical_up(8);
		
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterV(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe V
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(6);
	diagonal_left_down(2);
	diagonal_right_up(2);
	vertical_up(6)
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterW(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe W
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(8);
	diagonal_right_up(2);
	diagonal_left_down(2);
	vertical_up(8);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}

void writeLetterX(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe X
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(2);
	diagonal_right_up(4);
	vertical_up(2);
	vertical_down(2);
	diagonal_right_down(2);
	diagonal_left_up(2);
	vertical_up(2);
	vertical_down(2);
	diagonal_left_down(4);
	vertical_down(2);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeLetterY(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe Y
	horizontal_right(2);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(4);
	diagonal_left_up(2);
	vertical_up(2);
	vertical_down(2);
	diagonal_left_down(2);
	diagonal_right_up(2);
	vertical_up(2);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);
}


void writeLetterZ(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe Z
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_down(2);
	diagonal_right_down(2);
	horizontal_left(2);
	horizontal_right(4);
	horizontal_left(2);
	diagonal_right_down(2);
	vertical_down(2);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeDigitZero(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe Z
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
	
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(4);
	vertical_down(8);
	horizontal_left(4);
	
	//Absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(5);


}

void writeDigitOne(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
	vertical_up(4);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	diagonal_right_up(4);
	vertical_down(8);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
		
}

void writeDigitTwo(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_down(4);
	horizontal_left(4);
	vertical_down(4);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeDigitThree(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_up(4);
	horizontal_left(4);
	horizontal_right(4);
	vertical_up(4);
	horizontal_left(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(8);
}

void writeDigitFour(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(4);
	horizontal_right(4);
	vertical_up(4);
	vertical_down(8);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
}

void writeDigitFive(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(3);
	diagonal_right_up(1);
	vertical_up(2);
	diagonal_left_up(1);
	horizontal_left(3);
	vertical_up(4);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(8);

}

void writeDigitSix(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe A
	horizontal_right(4);
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_left(3);
	diagonal_right_down(1);
	vertical_down(6);
	diagonal_left_down(1);
	horizontal_right(2);
	diagonal_right_up(1);
	vertical_up(2);
	diagonal_left_up(1);
	horizontal_left(3);
		
		
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(5);
	vertical_down(4);
}

void writeDigitSeven(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Dusche
	vertical_up(6);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(2);
	horizontal_right(4);
	vertical_down(8);
	vertical_up(4);
	horizontal_left(2);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(3);
	vertical_down(4);
}

void writeDigitEight(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_up(8);
	horizontal_right(4);
	vertical_down(8);
	horizontal_left(4);
	vertical_up(4);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(4);
}

void writeDigitNine(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_up(8);
	horizontal_left(4);
	vertical_down(4);
	horizontal_right(4);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(1);
	vertical_down(4);
}

void writeComma(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt
	horizontal_right(1);	
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	diagonal_right_up(2);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Fahrt zum Referenzpunkt
	horizontal_right(2);
	vertical_down(2);
		
}

void writeQuestionMark(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe A
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
	vertical_down(4);
	horizontal_left(2);
	vertical_down(2);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Zustellung Punkt
	vertical_down(2);
		
	//ZUstellung
	pen_contact_y(1);
		
	//absetzen in y
	pen_drop_y(1);
	
	//Fahrt zum Referenzpunkt
	horizontal_right(3);
}

void writeExclamationMark(StepperMotionAxisController* controller)
{
	//Zustellung Startpunk
	horizontal_right(2);
	vertical_up(8);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
	vertical_down(6);
		
	//Absetzen in y
	pen_drop_y(1);
		
	//Zustellung Punkt
	vertical_down(2);
		
	//ZUstellung
	pen_contact_y(1);
		
	//absetzen in y
	pen_drop_y(1);
		
		
	//Fahrt zum Referenzpunkt
	horizontal_right(3);
}

void writeDot(StepperMotionAxisController* controller)
{
	//Zustellung Startpunkt Buchstabe A
	horizontal_right(2);
		
	//Zustellung in y - Papierkontakt
	pen_contact_y(1);
		
	//Zeichensequenz in Pixeln
		
	//absetzen in y
	pen_drop_y(1);

		
	//Fahrt zum Referenzpunkt
	horizontal_right(3);			//Fahrt auf Referenzlinie
}

void writeSpace(StepperMotionAxisController* controller)
{
	//keine Zustellung Startpunkt
		
	//Zeichensequenz in Pixeln
	horizontal_right(4);
		
	//keine Fahrt zur Referenz
	horizontal_right(1);
}


uint8_t validate_input(char character)
{
	uint8_t valid = 1;
	switch(character)
	{
		case 'a':
		case 'A':
		break;
		
		case 'b':
		case 'B':
		break;
		
		case 'c':
		case 'C':
		break;
		
		case 'd':
		case 'D':
		break;
		
		case 'e':
		case 'E':
		break;
		
		case 'f':
		case 'F':
		break;
		
		case 'g':
		case 'G':
		break;
		
		case 'h':
		case 'H':
		break;
		
		case 'i':
		case 'I':
		break;
		
		case 'j':
		case 'J':
		break;
		
		case 'k':
		case 'K':
		break;
		
		case 'l':
		case 'L':
		break;
		
		case 'm':
		case 'M':
		break;
		
		case 'n':
		case 'N':
		break;
		
		case 'o':
		case 'O':
		break;
		
		case 'p':
		case 'P':
		break;
		
		case 'q':
		case 'Q':
		break;
		
		case 'r':
		case 'R':
		break;
		
		case 's':
		case 'S':
		break;
		
		case 't':
		case 'T':
		break;
		
		case 'u':
		case 'U':
		break;
		
		case 'v':
		case 'V':
		break;
		
		case 'w':
		case 'W':
		break;
		
		case 'x':
		case 'X':
		break;
		
		case 'y':
		case 'Y':
		break;
		
		case 'z':
		case 'Z':
		break;
		
		case '0':
		break;
		
		case '1':
		break;
		
		case '2':
		break;
		
		case '3':
		break;
		
		case '4':
		break;
		
		case '5':
		break;
		
		case '6':
		break;
		
		case '7':
		break;
		
		case '8':
		break;
		
		case '9':
		break;
		
		case ' ':
		break;
		
		case '.':
		break;
		
		case ',':
		break;
		
		case '?':
		break;
		
		case '!':
		break;
		
		default :
		valid = 0;
		break;
		
		
		//gibt Fehlermeldung aus? Z.B. ö, ä, ....
	}
	return valid;




}

uint8_t validate_input_stream(char* stream)
{
	while (*stream)
	{
		if (!validate_input(*stream))
		{
			char error_message_part1[2];
			error_message_part1[0] = *stream;
			error_message_part1[1] = 0x00;
			lcd_reset();
			lcd_text(error_message_part1);
			lcd_text(": not supported   [RED=RETRY]");
			return 0;
		}
		stream++;
	}
	return 1;
}

void SMAC_GO_BEGINNING_CYLINDER_ROW_2(StepperMotionAxisController* controller, uint8_t char_count)
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
	SMAC_ADD_MOVE_X_max82(controller, 10*controller->pixel_unit_mm, X_RIGHT);
	
	
// 	for (uint8_t i = 0; i < temp/40; i++)
// 	{
// 		SMAC_ADD_MOVE_Z_max233(controller, 40, Z_CLOCKWISE);
// 	}
// 	SMAC_ADD_MOVE_X_max82(controller, 9*controller->pixel_unit_mm, X_RIGHT);

}

void SMAC_dispatch_character_function(StepperMotionAxisController* controller, char character)
{

	switch(character)
	{
		case 'a':
		case 'A':
		writeLetterA(controller);
		break;
			
		case 'b':
		case 'B':
		writeLetterB(controller);
		break;
			
		case 'c':
		case 'C':
		writeLetterC(controller);
		break;
			
		case 'd':
		case 'D':
		writeLetterD(controller);
		break;
			
		case 'e':
		case 'E':
		writeLetterE(controller);
		break;
			
		case 'f':
		case 'F':
		writeLetterF(controller);
		break;
			
		case 'g':
		case 'G':
		writeLetterG(controller);
		break;
			
		case 'h':
		case 'H':
		writeLetterH(controller);
		break;
			
		case 'i':
		case 'I':
		writeLetterI(controller);
		break;
			
		case 'j':
		case 'J':
		writeLetterJ(controller);
		break;
			
		case 'k':
		case 'K':
		writeLetterK(controller);
		break;
			
		case 'l':
		case 'L':
		writeLetterL(controller);
		break;
			
		case 'm':
		case 'M':
		writeLetterM(controller);
		break;
			
		case 'n':
		case 'N':
		writeLetterN(controller);
		break;
			
		case 'o':	
		case 'O':
		writeLetterO(controller);
		break;
			
		case 'p':
		case 'P':
		writeLetterP(controller);
		break;
			
		case 'q':
		case 'Q':
		writeLetterQ(controller);
		break;
			
		case 'r':
		case 'R':
		writeLetterR(controller);
		break;
			
		case 's':
		case 'S':
		writeLetterS(controller);
		break;
			
		case 't':
		case 'T':
		writeLetterT(controller);
		break;
			
		case 'u':
		case 'U':
		writeLetterU(controller);
		break;
			
		case 'v':
		case 'V':
		writeLetterV(controller);
		break;
			
		case 'w':
		case 'W':
		writeLetterW(controller);
		break;
			
		case 'x':
		case 'X':
		writeLetterX(controller);
		break;
			
		case 'y':
		case 'Y':
		writeLetterY(controller);
		break;
			
		case 'z':
		case 'Z':
		writeLetterZ(controller);
		break;
			
		case '0':
		writeDigitZero(controller);
		break;
			
		case '1':
		writeDigitOne(controller);
		break;
			
		case '2':
		writeDigitTwo(controller);
		break;
			
		case '3':
		writeDigitThree(controller);
		break;
			
		case '4':
		writeDigitFour(controller);
		break;
			
		case '5':
		writeDigitFive(controller);
		break;
			
		case '6':
		writeDigitSix(controller);
		break;
			
		case '7':
		writeDigitSeven(controller);
		break;
			
		case '8':
		writeDigitEight(controller);
		break;
			
		case '9':
		writeDigitNine(controller);
		break;
			
		case ' ':
		writeSpace(controller);
		break;
			
		case '.':
		writeDot(controller);
		break;
			
		case ',':
		writeComma(controller);
		break;
			
		case '?':
		writeQuestionMark(controller);
		break;
			
		case '!':
		writeExclamationMark(controller);
		break;
			
		default:
		// Kann nie Eintreten, weil wir zuvor UART-Nutzereingabe validiert haben.
		break;
		
	}
		
}






/*
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
	//SMAC_ADD_MOVE_Z_max233(controller, 3, Z_ANTICLOCKWISE);
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
*/
