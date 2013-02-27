/******************************
 * 
 * PRÁCTICA 2_PIH Programació de Ports i práctica de: "do-while" "case" "if" i "for"
 * 
 *****************************/

#include <msp430x54xA.h>
#include <stdio.h>

#include <hal_common.h>
#include <hal_buttons.h>
#include <hal_lcd.h>
#include <hal_led.h>

char saludo[17]= "PRACTICA 2";
char cadena[17];
char borrado[] = "                 ";  
unsigned char contraste   = 0x64;
unsigned char iluminacion = 30;
unsigned char linea = 1;
char estado = 0;
char estado_anterior = 8;
unsigned char bitled = BIT0;
long int i;


/**************************************************************************
 * BORRAR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a borrar
 * 
 * Sin datos de salida
 * 
 **************************************************************************/
void clearLine(unsigned char line)
{
    halLcdPrintLine(borrado, line, OVERWRITE_TEXT); //incluimos una linea en blanco
}


/**************************************************************************
 * ESCRIBIR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a escribir
 *           String, cadena de caracteres que vamos a introducir
 * 
 * Sin datos de salida
 * 
 **************************************************************************/
void write(char string[], unsigned char line)
{
    halLcdPrintLine(string, line, OVERWRITE_TEXT); //superponemos la nueva palabra introducida, haya o no algo.
}

/**************************************************************************
 * DELAY - A CONFIGURAR POR EL ALUMNO_ con bucle while
 * 
 * Datos de entrada: Tiempo de retraso. 1 segundo equivale a variable retraso 25000 (aprox)
 * 
 * Sin datos de salida
 * 
 **************************************************************************/


void delay(long unsigned int seconds)
{
    // Preferable to have it as unsigned, as we would have more
    // numbers if any person wishes to stall for more time...
    volatile long unsigned int ticks = 25000 * seconds;
    while ( ticks-- );
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
    /*/Configuramos botones y leds:
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
    P2IES &= ~0x3E; // con transicion L->H */
    
    halLed_sx_initialize();
    halLed_sx_setLed(LED_S1, ON);
    halLed_sx_setLed(LED_S2, OFF);

    halButtons_initialize();
    halButtons_setInterruptions(BUTTON_ALL, ON);

    halJoystick_initialize();
    halJoystick_setInterruptions(JOYSTICK_ALL, ON);
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
    halLed_rx_initialize();
    halLed_rx_setLed(LED_RX_ALL, OFF);
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
    halLcdInit();                    // Programa interno para iniciar la pantalla
    halLcdBackLightInit();           // Inicio de Iluminación posterior de la pantalla
    halLcdSetBackLight(iluminacion); // Determinación de la Iluminación posterior de la pantalla
    halLcdSetContrast(contraste);    //Establecimiento del contraste
    halLcdClearScreen();             //Limpiar (borrar) la pantalla
}



void main(void)
{
    WDTCTL = WDTPW+WDTHOLD; // Paramos el watchdog timer

    init_botons();          // Iniciamos los botones y Leds.

    config_P4_LEDS();       // Iniciamos los LEDS del puerto 4

    _enable_interrupt();    // Activamos las interrupciones a nivel global del chip
    init_LCD();             // Inicializamos la pantalla

    write(saludo,linea);    //escribimos saludo en la primera linea
    linea++;                //Aumentamos el valor de linea y con ello pasamos a la linea siguiente
    
    do
    {
        if (estado_anterior != estado)          // Dependiendo el valor del estado se encenderá un LED externo u otro.
        {
            //clearLine(linea);
            sprintf(cadena," estado %02d", estado);   // Guardamos en cadena lo siguiente frase: estado "valor del estado"
            write(cadena, linea);              // Escribimos cadena
            estado_anterior = estado;           // Actualizamos el valor de estado_anterior, para que no esté siempre escribiendo.
        }

        halLed_rx_setLed(LED_RX_ALL, OFF);
        halLed_rx_setLed(bitled, ON);
        if ( estado <= 2 && estado == 5 )
            halLed_sx_toggleLed(LED_SX_ALL); //P1OUT^= 0x03; // Encender los LEDS con intermitencia
        delay(2); // retraso de aprox 1 segundo

        if ( bitled == LED_R8 )
            bitled = BIT0;
        else
            bitled = bitled << 1;

    }
    while(1);
}

/**************************************************************************
 * MINIPROGRAMA DE LOS BOTONES:
 * Mediante este programa, se detectará que botón se ha pulsado
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
    halButtons_setInterruptions(BUTTON_ALL, ON);    // P2IE &= 0xC0;   //interrupciones botones S1 y S2 (P2.6 y P2.7) desactivadas
    halJoystick_setInterruptions(JOYSTICK_ALL, ON); // P2IE &= 0x3E;   //interrupciones joystick (2.1-2.5) desactivadas

    /**********************************************************+
        A RELLENAR POR EL ALUMNO BLOQUE CASE 
     
        Boton S1, estado =1 y leds ON
        Boton S2, estado =2 y leds uno ON y otro OFF
        Joystick left, estado =3 y leds off
        Joystick right, estado =4 y leds off
        Joystick center, estado = 5 y leds uno OFF y otro ON
        Joystick up, estado =6 y leds off
        Joystick down, estado =7 y leds off
     ***********************************************************/

    switch ( P2IFG )
    {
        case BUTTON_S1:
            halLed_sx_setLed(LED_SX_ALL, OFF);
            estado = 1;
            break;
        case BUTTON_S2:
            halLed_sx_setLed(LED_S1, ON);
            halLed_sx_setLed(LED_S2, OFF);
            estado = 2;
            break;
        case JOYSTICK_LEFT:
            halLed_sx_setLed(LED_SX_ALL, OFF);
            estado = 3;
            break;
        case JOYSTICK_RIGHT:
            halLed_sx_setLed(LED_SX_ALL, OFF);
            estado = 4;
            break;
        case JOYSTICK_CENTER:
            halLed_sx_setLed(LED_S1, OFF);
            halLed_sx_setLed(LED_S2, ON);
            estado = 5;
            break;
        case JOYSTICK_UP:
            halLed_sx_setLed(LED_SX_ALL, OFF);
            estado = 6;
            break;
        case JOYSTICK_DOWN:
            halLed_sx_setLed(LED_SX_ALL, OFF);
            estado = 7;
            break;
    }

    /***********************************************
     * HASTA AQUI BLOQUE CASE
     ***********************************************/
    
    //P2IFG = 0;    //limpiamos todas las interrupciones
    //P2IE |= 0xC0;   //interrupciones botones S1 y S2 (P2.6 y P2.7) reactivadas
    //P2IE |= 0x3E;  //interrupciones joystick (2.1-2.5) reactivadas

    halButtons_setInterruptions(BUTTON_ALL, OFF);
    halJoystick_setInterruptions(JOYSTICK_ALL, OFF);
 return;
}
