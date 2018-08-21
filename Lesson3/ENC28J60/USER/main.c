#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "enc28j60.h"

 u8 my_macaddr[6] = {0x04, 0x02, 0x35, 0x00, 0x00, 0x00};

int main(void)
{ 
 
	delay_init(168);		  //��ʼ����ʱ����
    uart_init(115200);
	LED_Init();		        //��ʼ��LED�˿�
    enc28j60Init(my_macaddr);  //��ʼ��enc28j60
    
    while(1)
    {
        LED2 = 0;
        LED3 = 1;
        delay_ms(500);
        LED2 = 1;
        LED3 = 0;
        delay_ms(500);
     }
}


	

 



