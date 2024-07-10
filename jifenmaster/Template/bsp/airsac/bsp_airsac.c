#include "bsp_airsac.h"
#include "board.h"
#include "string.h"
#include "bsp_switch.h"

lin_pack_stu lin_pack = {0};

static const uint8_t air_mode_data[AIR_MODE_MAX_NUM][LIN_DATA_SIZE] = {
    0X80,0X00,0XFC,0XA4,0X00,0X10,0XFF,//POWER_ON
    0X80,0X00,0XFC,0XA4,0X00,0X14,0XFF,//UP_CHARGE
    0X80,0X00,0XFC,0XA4,0X00,0X18,0XFF,//DOWN_CHARGE
    0X80,0X00,0XFC,0XA4,0X00,0X11,0XFF,//ALL_CHARGE
    0X80,0X00,0XFC,0XA4,0X00,0X12,0XFF,//ALL_DISCHARGE
    0XC2,0X00,0XFC,0XA4,0X00,0X10,0XFF,//MASSAGE
};


void airsac_thread_init(void)
{
    lin_pack.sync = 0x55;
    lin_pack.pid 	= 0x03;

    lin_init();
}



void set_airsac_mode(air_mode_enum mode)
{
    if(mode < AIR_MODE_MAX_NUM)
    {
        switch((uint8_t)mode)
        {
            case POWER_ON:			memcpy(&lin_pack.data, &air_mode_data[POWER_ON], LIN_DATA_SIZE);    break;
            case UP_CHARGE:			memcpy(&lin_pack.data, &air_mode_data[UP_CHARGE], LIN_DATA_SIZE);   break;
            case DOWN_CHARGE: 	    memcpy(&lin_pack.data, &air_mode_data[DOWN_CHARGE], LIN_DATA_SIZE);break;
            case ALL_CHARGE:  	    memcpy(&lin_pack.data, &air_mode_data[ALL_CHARGE], LIN_DATA_SIZE);break;
            case ALL_DISCHARGE:	    memcpy(&lin_pack.data, &air_mode_data[ALL_DISCHARGE], LIN_DATA_SIZE);break;
            case MASSAGE:			memcpy(&lin_pack.data, &air_mode_data[MASSAGE], LIN_DATA_SIZE);break;
        }

        lin_pack.checksum = lin_checksum_calculate(lin_pack.pid, lin_pack.data, LIN_DATA_SIZE);

        lin_send_pack(&lin_pack);
    }
}


void airsac_thread_run(void)
{
    uint16_t key_value = get_switch_value(SWITCH_4);

    if (key_value > 200) set_airsac_mode(POWER_ON);

    if (key_value > 4000)   set_airsac_mode(UP_CHARGE);
    else if (key_value > 3400) set_airsac_mode(DOWN_CHARGE);
    else if (key_value > 2600) set_airsac_mode(ALL_CHARGE);
    else if (key_value > 1900) set_airsac_mode(ALL_DISCHARGE);

}


