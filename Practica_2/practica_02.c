/******************************
 * 
 * PRÁCTICA 2_PIH Programació de Ports i práctica de: "do-while" "case" "if" i "for"
 * 
 *****************************/

#include <msp430x54xA.h>
#include <stdio.h>
#include "hal\hal_lcd.h"

char saludo[17]= "PRACT 2";
char cadena[17];
char borrado[] = "                 ";  
unsigned char contraste   = 0x64;
unsigned char iluminacion  = 30;
unsigned char linea=1;
char estado=0;
char estado_anterior=8;
long int i;


/**************************************************************************
 * BORRAR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a borrar
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void borrar(unsigned char Linea)
{
	halLcdPrintLine(borrado, Linea, OVERWRITE_TEXT); //incluimos una linea en blanco
}


/**************************************************************************
 * ESCRIBIR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a escribir
 * 					 String, cadena de caracteres que vamos a introducir
 * 
 * Sin datos de salida
 * 
 **************************************************************************/
  
void escribir(char String[], unsigned char Linea)  

{
	halLcdPrintLine(String, Linea, OVERWRITE_TEXT); //superponemos la nueva palabra introducida, haya o no algo.
}

/**************************************************************************
 * DELAY - A CONFIGURAR POR EL ALUMNO_ con bucle while
 * 
 * Datos de entrada: Tiempo de retraso. 1 segundo equivale a variable retraso 25000 (aprox)
 * 
 * Sin datos de salida
 * 
 **************************************************************************/


void delay (long int temps)
{
   volatile long int i=0;
	/***********************************************
   	 * A RELLENAR POR EL ALUMNO BLOQUE WHILE
   	 ***********************************************/	
   
   
   
   	 /***********************************************
   	 * HASTA AQUI BLOQUE WHILE
   	 ***********************************************/	
}


/**************************************************************************
 * INICIALIZACIÓN DE LOS BOTONES.
 * Configuramos botones y leds: 
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void init_botons(void)
{
  //Configuramos botones y leds:
  P1DIR |= 0x03;	//Puertos P1.0 y P1.1 como salidas (Leds)
  P1OUT |= 0x01;	//Inicializamos puerto P1.0 a 1, 
  P1OUT &= 0xFD;	// y P1.1 a 0, para leds en alternancia


  P2DIR &= ~0xC0;	//Puertos P2.6 y P2.7 como entradas (botones S1 y S2)
  P2SEL &= ~0xC0;	//Puertos P2.6 y P2.7 como I/O digitales,
  P2REN |= 0xC0;	//con resistencia activada
  P2OUT |= 0xC0;	// de pull-up
  P2IE |= 0xC0; 	//Interrupciones activadas en P2.6 y P2.7,
  P2IES &= ~0xC0;	// con transicion L->H
  
  
  //Configuramos el joystick:
  P2DIR &= ~0x3E;	//Puertos P2.1 a P2.5 como entradas (joystick)
  P2SEL &= ~0x3E;	//Puertos P2.1 y P2.5 como I/O digitales,
  P2REN |= 0x3E;	//con resistencia activada
  P2OUT |= 0x3E;	// de pull-up
  P2IE |= 0x3E; 	//Interrupciones activadas en P2.1 a P2.5,
  P2IES &= ~0x3E;	// con transicion L->H
  
}

/*****************************************************************************
 * CONFIGURACIÓN DEL PUERTO 4, PARA ILUMINAR LOS LEDS. A REALIZAR POR EL ALUMNO
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 *  
 ****************************************************************************/
 
void config_P4_LEDS (void)
{

}


/**************************************************************************
 * INICIALIZACIóN DE PANTALLA.
 * Inicializamos la pantallita LCD:
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void init_LCD(void)
{
      
  halLcdInit();  //Programa interno para iniciar la pantalla     
  halLcdBackLightInit(); // Inicio de Iluminación posterior de la pantalla
  halLcdSetBackLight(iluminacion); // Determinación de la Iluminación posterior de la pantalla
  halLcdSetContrast(contraste);//Establecimiento del contraste
  halLcdClearScreen();  //Limpiar (borrar) la pantalla
}



void main(void)
{	

  	WDTCTL = WDTPW+WDTHOLD;       	// Paramos el watchdog timer
  
  	init_botons();					// Iniciamos los botones y Leds.
    _enable_interrupt(); 				// Activamos las interrupciones a nivel global del chip
    init_LCD();						// Inicializamos la pantalla
  
  	escribir(saludo,linea); 			//escribimos saludo en la primera linea
  	linea++; 					//Aumentamos el valor de linea y con ello pasamos a la linea siguiente
  	
  	do
   	{
  		if (estado_anterior != estado)			// Dependiendo el valor del estado se encenderá un LED externo u otro.
  		{
  			sprintf(cadena," estado %d", estado); 	// Guardamos en cadena lo siguiente frase: estado "valor del estado"
  			escribir(cadena,linea); 			// Escribimos cadena
  			estado_anterior=estado; 			// Actualizamos el valor de estado_anterior, para que no esté siempre escribiendo.
  		}
  		P1OUT^= 0x03;					// Encender los LEDS con intermitencia
  		delay(25000);					// retraso de aprox 1 segundo
     }
  	while(1);
}


/**************************************************************************
 * MINIPROGRAMA DE LOS BOTONES:
 * Mediante este programa, se detectaró que botón se ha pulsado
 * 		 
 * Sin Datos de entrada
 * 
 * Sin datos de salida
 * 
 * Actualiza el valor estado
 * 
 **************************************************************************/
 
#pragma vector=PORT2_VECTOR  //interrupción de los botones. Actualiza el valor de la variable global estado.
__interrupt void Port2_ISR(void)
{
	P2IE &= 0xC0; 	//interrupciones botones S1 y S2 (P2.6 y P2.7) desactivadas
	P2IE &= 0x3E;   //interrupciones joystick (2.1-2.5) desactivadas
	
	/**********************************************************+
		A RELLENAR POR EL ALUMNO BLOQUE CASE 
	 
	Boton S1, estado =1 y leds ON
	Boton S2, estado =2 y leds uno ON y otro OFF 
	Joystick left, estado =3 y leds off
	Joystick right, estado =4 y leds off
	Joystick center, estado = 5 y leds uno OFF y otro ON
	Joystick up, estado =6 y leds off
	Joystick down, estado =7 y leds off
	 * *********************************************************/
	 



	
	/***********************************************
   	 * HASTA AQUI BLOQUE CASE
   	 ***********************************************/	
	
	P2IFG = 0;		//limpiamos todas las interrupciones
	P2IE |= 0xC0; 	//interrupciones botones S1 y S2 (P2.6 y P2.7) reactivadas
	P2IE |= 0x3E;  //interrupciones joystick (2.1-2.5) reactivadas
 return;
}
