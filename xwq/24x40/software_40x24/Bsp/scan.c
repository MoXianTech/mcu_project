#include "scan.h"

void scan_y(Hc4051AdcValue *adc_raw_value,uint8_t count_y)
{
    static uint8_t rankB_Y[24] = {7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,23,22,21,20,19,18,17,16};
    math_resi_cal_t math_resi_cal = {0};
    if (1)
    {

    REF_OUT(1);
    ic_74hc595_clean();
    ReadAdcValue(adc_raw_value, count_y, REFER_RESISTACE);

    REF_OUT(0);
    ic_74hc595_io_write(rankB_Y[count_y]);
    ReadAdcValue(adc_raw_value, count_y, RAW_RESISTACE);

    math_resi_cal.sensor_adc_value = adc_raw_value->sensorB_raw_value;
    math_resi_cal.resi_adc_value = adc_raw_value->sensorB_ref_value;
    math_resi_cal.math_number = AX_NUM;

    math_resi_cali_once(&math_resi_cal,
                        adc_raw_value->sensorB_resi_output,
                        AX_NUM,
                        SCAN_LOW_LEVEL);
    }

    math_display_resi(adc_raw_value->sensorB_resi_output,
                      adc_raw_value->sensorB_voltage[count_y],
                      AX_NUM,
                      RESI_BACKWARDS);

}

