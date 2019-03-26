#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "enc28j60.h"
#include "timer.h"
#include "lwip_demo.h"
#include "oled.h"

int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
    uart_init(115200);
	LED_Init();		        //��ʼ��LED�˿�
    TIM3_Int_Init(2000-1, 84-1);  // 20ms�ж�һ��
    OLED_Init();
    OLED_ShowString(0,0, "  LwIP DEMO", 16);
    lwip_demo();
    
    return 0;
}


	

 



