/*
**************************************************************************************************************
*                 Test program for the mitm�t
*
* filename:	avr_C_sampl.c
* created:	T. Daboczi, February 2016. 
*
* prerequisites:
*			- WinAVR 20071221 is installed in the root of drive C (C:\WinAVR-20071221)
*
* function:	- LED1..3 shows the status of the buttons
*			- 7 segment display shows the temperature, read every ~1 sec in IT routine
*
**************************************************************************************************************
*/


#include "mcu_avr_atmega128_api.h" 	// MCU API   
#include "dpy_trm_s01.h"			// DPY API
#include "time.h"
#include <stdlib.h>

unsigned char	led_counter=0;
volatile unsigned char	disp_flag=0;
unsigned char	err;
float			temp_sensor;
unsigned char	but1, but2, but3;

/********  function prototypes  ***************************/
void Timer0_Init(void);
ISR(SIG_OVERFLOW0);
int main(void);

/********  Timer0 overflow IT Service Routine  ***************************/
ISR(SIG_OVERFLOW0) // Timer0 overflow
{
   led_counter++; 
   if (led_counter<15) DPY_TRM_S01__LED_4_OFF();
   else if (led_counter<30) DPY_TRM_S01__LED_4_ON();
   else {
		err=dpy_trm_s01__Temp_ReadTEMP(&temp_sensor,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF,TMP75_JUMPER_OFF);
							/* Reads the temperature sensor */
		disp_flag=1;
		led_counter=0;
		}
}

/* �sszehasonl�t� f�ggv�ny */
void compare(int calc, int random){
	if(calc < random){	//ha a tippelt sz�m kisebb a random sz�mn�l
		while(1){		//v�gtelen�tett ciklusban menjen a villog�s egy interruptig.
			DPY_TRM_S01__LED_4_ON();	//ha kisebb akkor 4-es LED-et bekapcsoljuk
			_delay_ms(200);				//200ms-ig hagyjuk �gni
			DPY_TRM_S01__LED_4_OFF();	//kikapcsoljuk a 4-es LED-et
			_delay_ms(200);				//200ms-ig nem vil�g�t. �gy v�gtelen�tett ciklusban villog.
			if(!DPY_TRM_S01__BUTTON_1_GET_STATE() || 
			   !DPY_TRM_S01__BUTTON_2_GET_STATE() ||
			   !DPY_TRM_S01__BUTTON_3_GET_STATE()){		//ha ak�rmelyik gombot lenyomjuk, akkor kil�p�nk a v�gtelen�tett ciklusb�l
				   DPY_TRM_S01__LED_4_OFF();
				   break;
			   }
		}
	}
	/* ugyanazon gondolatmenettel mint az elobb */
	if(calc > random){
		while(1){
			DPY_TRM_S01__LED_1_ON();
			_delay_ms(200);
			DPY_TRM_S01__LED_1_OFF();
			_delay_ms(200);
			if(!DPY_TRM_S01__BUTTON_1_GET_STATE() || 
			   !DPY_TRM_S01__BUTTON_2_GET_STATE() ||
			   !DPY_TRM_S01__BUTTON_3_GET_STATE()){
				   DPY_TRM_S01__LED_1_OFF();
				   break;
			   }
		}
	}
	/* ha eltal�ltuk a sz�mot akkor minden LED villogjon */
	/* ugyan �gy mint eddig, csak az �sszes LED-et �ll�tjuk */
	if(calc == random){
		while(1){
			DPY_TRM_S01__LED_1_ON();
			DPY_TRM_S01__LED_2_ON();
			DPY_TRM_S01__LED_3_ON();
			DPY_TRM_S01__LED_4_ON();
			_delay_ms(200);
			DPY_TRM_S01__LED_1_OFF();
			DPY_TRM_S01__LED_2_OFF();
			DPY_TRM_S01__LED_3_OFF();
			DPY_TRM_S01__LED_4_OFF();
			_delay_ms(200);
			if(!DPY_TRM_S01__BUTTON_1_GET_STATE() || 
			   !DPY_TRM_S01__BUTTON_2_GET_STATE() ||
			   !DPY_TRM_S01__BUTTON_3_GET_STATE()){
				   DPY_TRM_S01__LED_1_OFF();
				   DPY_TRM_S01__LED_2_OFF();
				   DPY_TRM_S01__LED_3_OFF();
				   DPY_TRM_S01__LED_4_OFF();
				   break;
			   }
		}
	}
}

/* Random sz�m gener�l� f�ggv�ny */
/* http://stackoverflow.com/questions/9571738/picking-random-number-between-two-points-in-c */
int RandRange(int Min, int Max)
{
    int diff = Max-Min;
    return (int) (((double)(diff+1)/RAND_MAX) * rand() + Min);
}


/********  main program  ***************************/
int main (void) {
  srand(time(NULL));
  int randNum=rand() % 1000;			//a gener�lt random sz�m 0-999 k�z�tt a feladatnak megfeleloen
  
  int tip=0;			//a kezdeti tipp�nk defini�l�sa

  int i;				//seg�d iter�tor
  
  int shiftReg[124];	//124 bites shift register
  
  int delay = 100;			//main-ben l�vo ciklus k�sleltet�se.
  int cnt1=0;
  int cnt3=0;
  dpy_trm_s01__Init();	//gombok, LED-ek megh�v�sa
  
  /*shiftregisztert kinull�zok*/
  for(i=0;i<124;i++){
	  shiftReg[i]=0;
  }
  
  while(1) {
	/* ha a jobb oldali gombot nyomjuk akkor n�velj�k az �rt�ket */
    if(!DPY_TRM_S01__BUTTON_1_GET_STATE()){
		if(cnt1 > 15)
			tip=tip+5;
		else if(cnt1 > 10)
			tip=tip+2;
		else tip++;
		cnt1++;
		//shifting(shiftReg,1);
		//delay=delayChange(shiftReg, delay);
	}
	else cnt1=0;
	/* k�z�pso gomb megnyom�s�val �sszehasonl�tjuk a tippelt sz�mot a random sz�mmal */
	if(!DPY_TRM_S01__BUTTON_2_GET_STATE()){
		compare(tip,randNum);
		delay=100;
	}
	/* ha a bal oldali gombot nyomjuk akkor cs�kkentj�k az �rt�ket */
	if(!DPY_TRM_S01__BUTTON_3_GET_STATE()){
		if(cnt3 > 15)
			tip=tip-5;
		else if(cnt3 > 10)
			tip=tip-2;
		else tip--;
		cnt3++;
		//shifting(shiftReg,2);
		//delay=delayChange(shiftReg, delay);
	}
	else cnt3=0;
	dpy_trm_s01__7seq_write_number((float)randNum, 0);	//minden ciklusban kiiratjuk a h�tszegmens kijelzore a tippet.
	_delay_ms(delay);
  }
  return 0;
}


/********  Timer0 initialisation  ***************************/
void Timer0_Init(void)
{
   TCCR0=0x07;			// Set TIMER0 prescaler to CLK/1024                 
   TCNT0=0;				// Set the counter initial value                    
   TIMSK=_BV(TOIE0);	// Enable TIMER0 overflow interrupt                 
}

