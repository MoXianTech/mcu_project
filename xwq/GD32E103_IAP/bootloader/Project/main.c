#include "gd32e10x.h"
#include "main.h"
#include "systick.h"

//mlib
#include "mculib_usart0.h"
#include "mlib_simplefifo.h"

//ebus
#include "eBus_mcuboot.h"
#include "eBus_port.h"



//user_application
#include "io.h"


uint16_t boot_wait=0;

void usart0_receive_back(uint8_t value)
{
    simplefifo_buff_write(value);
}

typedef void (*MCURESET)(void);
void load_appx(uint32 appaddx)
{
    MCURESET reset;
    //校验，从要跳转的地址中取出数据，然后校验地址是否在正常范围中
    if(((*(uint32*)appaddx)&0x2FFE0000)==0x20000000)
    {
        reset = (MCURESET)*(uint32*)(appaddx+4);
        __set_MSP(*(uint32*)appaddx);
        reset();
    }
}

int main(void)
{
    systick_config();
    io_init();
    ledG_on();
    eBus_init();
    mculib_usart0_init(115200,usart0_receive_back);
	
    while(1)
    {
        //上电后等待3000ms，如果在此期间收到进入boot命令，则进入boot，等待升级
        eBus_run(1);
        delay_1ms(1);
        if(eBus_mcuboot_state_get()==EBUS_MCUBOOT_STATE_ENUM_ENABLE)
        {
            boot_wait=1;
            break;
        }
        boot_wait++;
        if(boot_wait>3000)
        {
            boot_wait=0;
            break;
        }
    }
    
    if((eBus_mcuboot_state_get()!=EBUS_MCUBOOT_STATE_ENUM_ENABLE)&&(eBus_flash_param_update_flag_get()==0))//执行APP程序跳转
    {
        __set_PRIMASK(1);//关闭所有中断
        load_appx(0x8002000);
        __enable_irq();//如果跳转失败，需要打开中断
    }
    simplefifo_buff_cleare_all();
    //停留在当前BOOT中
    while(1)
    {
        boot_wait++;
        if(boot_wait>=100)
        {
            boot_wait=0; 
            ledG_not();
        }
        eBus_run(1);
        delay_1ms(1);

    }
}




