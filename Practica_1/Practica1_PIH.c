/*
 * programa práctica 1 de PIH.
 * fichero practica1_PIH.c
 */

#include <msp430x54xA.h>
#include <stdio.h>
#include "hal_lcd.h"


typedef unsigned char byte;
#define chartest 0x55                  //tren de bits 01010101.

volatile unsigned long i,j;            // Volatile to prevent optimization
volatile char txchar,rxchar;
unsigned char contraste   = 0x64;
unsigned char iluminacion  = 30;
char textstyle = 2, linea = 1;
char saludo[] = "Practica 1";
char cadena[17];
char borrar[] = "                 ";   //17 espacios para borrar una linea entera
int estado_botones = 0;
byte bID = 1;

void init_botons(void)
{
	//Configuramos botones y leds:
	P1DIR |= 0x03;  //Puertos P1.0 y P1.1 como salidas (Leds)
	P1OUT |= 0x01;  //Inicializamos puerto P1.0 a 1, 
	P1OUT &= 0xFD;  // y P1.1 a 0, para leds en alternancia


	P2DIR &= ~0xC0; //Puertos P2.6 y P2.7 como entradas (botones S1 y S2)
	P2SEL &= ~0xC0; //Puertos P2.6 y P2.7 como I/O digitales,
	P2REN |= 0xC0;  //con resistencia activada
	P2OUT |= 0xC0;  // de pull-up
	P2IE |= 0xC0;   //Interrupciones activadas en P2.6 y P2.7,
	P2IES &= ~0xC0; // con transicion L->H
	
	
	//Configuramos el joystick:
	P2DIR &= ~0x3E; //Puertos P2.1 a P2.5 como entradas (joystick)
	P2SEL &= ~0x3E; //Puertos P2.1 y P2.5 como I/O digitales,
	P2REN |= 0x3E;  //con resistencia activada
	P2OUT |= 0x3E;  // de pull-up
	P2IE |= 0x3E;   //Interrupciones activadas en P2.1 a P2.5,
	P2IES &= ~0x3E; // con transicion L->H
	
}

void init_LCD(void)
{
	//Inicializamos la pantallita LCD:    
	halLcdInit();       
	halLcdBackLightInit();  
	halLcdSetBackLight(iluminacion);
	halLcdSetContrast(contraste);
	halLcdClearScreen(); 
}


void main(void)
{
	WDTCTL = WDTPW+WDTHOLD;       // Paramos el watchdog timer

	init_botons();                // Iniciamos los botones y Leds.
	_enable_interrupt();          // Activamos las interrupciones a nivel global del chip
	init_LCD();                   // Inicializamos la pantalla

	halLcdPrintLine( saludo,linea,textstyle);
	linea++;
	sprintf(cadena,"bID = %d",bID); 
	halLcdPrintLine( cadena,linea,textstyle);
	

	do
	{
		P1OUT ^= 0x03; 
		i = 25000;
		do
		{
			i--;
		}
		while (i != 0);
	}
	while(1);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
	//Inicializamos los leds para que parpadeen en fase (S1) o alternando (S2):
	P2IE &= 0xC0;   //interrupciones botones S1 y S2 (P2.6 y P2.7) desactivadas
	P2IE &= 0x3E;   //interrupciones joystick (2.1-2.5) desactivadas
	switch (P2IFG)
	{
		case 0x40: //Boton S1
			txchar=chartest;
			P1OUT |= 0x03; //Leds parpadean a la vez
			estado_botones=1;
			break;
		case 0x80: //Boton S2
			txchar=~chartest;
			P1OUT |= 0x01; //Leds parpadean en alternancia
			estado_botones=2;
			break;
		case 0x02: // joystick left
			if (bID > 0x00) bID--;
			else bID = 0xFE;
			break;
		case 0x04: // joystick right
			if (bID < 0xFE) bID++;
			else bID = 0;
			break;
		case 0x08: // joystick center
			P1OUT |= 0x01;P1OUT &= 0xFD;//Leds parpadean alternando
			break;
		case 0x10: // joystick up
			if (bID < 0xF0) bID+=0x0F;
			else bID = 0;
			break;
		case 0x20: // joystick down
			if (bID > 0x0F) bID-=0x0F;
			else bID = 0xFE;
			break;

		default: break;
	}
	
	sprintf(cadena,"bID = %d",bID);
	halLcdPrintLine( cadena,linea,OVERWRITE_TEXT );
	
	P2IFG = 0;      //limpiamos todas las interrupciones
	P2IE |= 0xC0;   //interrupciones botones S1 y S2 (P2.6 y P2.7) reactivadas
	P2IE |= 0x3E;   //interrupciones joystick (2.1-2.5) reactivadas
	return;
}

