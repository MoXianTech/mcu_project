#include "board.h"
#include "bsp_led.h"
#include "bsp_moto.h"
#include "bsp_encoder.h"
#include "bsp_airsac.h"
#include "bsp_power_check.h"
#include "bsp_switch.h"
#include "PID_moto_control.h"
#include "priv_malloc.h"

uint64_t time_ms = 0;
int16_t wzh_speed_moto = 20;

#if 1
int main(void)
{
    uint16_t key_word[4] = {0};
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);  // 优先级分组

    board_init();
    /* 等待上电稳定 */
    delay_ms(2000);

    bsp_led_init();

    moto_init(10,800);
    encoder_init();
    power_check_init();

    switch_thread_init();
    PID_thread_init();
    usart_thread_init();
    airsac_thread_init();

    set_airsac_mode(POWER_ON);
    printf("start sys !!\n");
    while(1)
    {
        time_ms++;
        if(time_ms % 500 == 0)
        {
            //bsp_led_toggle(LED1);
            //set_moto_target(MOTO_4, wzh_speed_moto);
        }

        if(time_ms % 100 == 0)
        {
            switch_thread_run();
            usart_thread_run();
            airsac_thread_run();
            power_check_handle();
        }

        if (time_ms % 20 == 0)
        {
            //printf("%lld\n", time_ms);
            moto_PID_thread_run();
        }
        delay_ms(1);
    }
}

#else

/* FMC sector information */
typedef struct
{
    uint32_t sector_name;                                         /*!< the name of the sector */
    uint32_t sector_num;                                          /*!< the number of the sector */
    uint32_t sector_size;                                         /*!< the size of the sector */
    uint32_t sector_start_addr;                                   /*!< the start address of the sector */
    uint32_t sector_end_addr;                                     /*!< the end address of the sector */
} fmc_sector_info_struct;

/* sector size */
#define SIZE_16KB                  ((uint32_t)0x00004000U)        /*!< size of 16KB*/
#define SIZE_64KB                  ((uint32_t)0x00010000U)        /*!< size of 64KB*/
#define SIZE_128KB                 ((uint32_t)0x00020000U)        /*!< size of 128KB*/
#define SIZE_256KB                 ((uint32_t)0x00040000U)        /*!< size of 256KB*/

/* FMC BANK address */
#define FMC_START_ADDRESS          FLASH_BASE                               /*!< FMC start address */
#define FMC_BANK0_START_ADDRESS    FMC_START_ADDRESS                        /*!< FMC BANK0 start address */
#define FMC_BANK1_START_ADDRESS    ((uint32_t)0x08100000U)                  /*!< FMC BANK1 start address */
#define FMC_SIZE                   (*(uint16_t *)0x1FFF7A22U)               /*!< FMC SIZE */
#define FMC_END_ADDRESS            (FLASH_BASE + (FMC_SIZE * 1024) - 1)     /*!< FMC end address */
#define FMC_MAX_END_ADDRESS        ((uint32_t)0x08300000U)                  /*!< FMC maximum end address */

/* FMC error message */
#define FMC_WRONG_SECTOR_NAME      ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector name*/
#define FMC_WRONG_SECTOR_NUM       ((uint32_t)0xFFFFFFFFU)        /*!< wrong sector number*/
#define FMC_INVALID_SIZE           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector size*/
#define FMC_INVALID_ADDR           ((uint32_t)0xFFFFFFFFU)        /*!< invalid sector address*/

/*!
  \brief      get the sector number, size and range of the given address
  \param[in]  address: The flash address
  \param[out] none
  \retval     fmc_sector_info_struct: The information of a sector
  */
fmc_sector_info_struct fmc_sector_info_get(uint32_t addr)
{
    fmc_sector_info_struct sector_info;
    uint32_t temp = 0x00000000U;
    if((FMC_START_ADDRESS <= addr)&&(FMC_END_ADDRESS >= addr)) {
        if ((FMC_BANK1_START_ADDRESS > addr)) {
            /* bank0 area */
            temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (uint32_t)temp;
                sector_info.sector_num = CTL_SN(temp);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000004U;
                sector_info.sector_num = CTL_SN(4);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08010000U;
                sector_info.sector_end_addr = 0x0801FFFFU;
            } else {
                temp = (addr - FMC_BANK0_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 4);
                sector_info.sector_num = CTL_SN(temp + 4);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            }
        } else {
            /* bank1 area */
            temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_16KB;
            if (4U > temp) {
                sector_info.sector_name = (uint32_t)(temp + 12);
                sector_info.sector_num = CTL_SN(temp + 16);
                sector_info.sector_size = SIZE_16KB;
                sector_info.sector_start_addr = FMC_BANK0_START_ADDRESS + (SIZE_16KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_16KB - 1;
            } else if (8U > temp) {
                sector_info.sector_name = 0x00000010;
                sector_info.sector_num = CTL_SN(20);
                sector_info.sector_size = SIZE_64KB;
                sector_info.sector_start_addr = 0x08110000U;
                sector_info.sector_end_addr = 0x0811FFFFU;
            } else if (64U > temp){
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_128KB;
                sector_info.sector_name = (uint32_t)(temp + 16);
                sector_info.sector_num = CTL_SN(temp + 20);
                sector_info.sector_size = SIZE_128KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_128KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_128KB - 1;
            } else {
                temp = (addr - FMC_BANK1_START_ADDRESS) / SIZE_256KB;
                sector_info.sector_name = (uint32_t)(temp + 20);
                sector_info.sector_num = CTL_SN(temp + 8);
                sector_info.sector_size = SIZE_256KB;
                sector_info.sector_start_addr = FMC_BANK1_START_ADDRESS + (SIZE_256KB * temp);
                sector_info.sector_end_addr = sector_info.sector_start_addr + SIZE_256KB - 1;
            }
        }
    } else {
        /* invalid address */
        sector_info.sector_name = FMC_WRONG_SECTOR_NAME;
        sector_info.sector_num = FMC_WRONG_SECTOR_NUM;
        sector_info.sector_size = FMC_INVALID_SIZE;
        sector_info.sector_start_addr = FMC_INVALID_ADDR;
        sector_info.sector_end_addr = FMC_INVALID_ADDR;
    }
    return sector_info;
}

/*!
  \brief      get the sector number by a given sector name
  \param[in]  address: a given sector name
  \param[out] none
  \retval     uint32_t: sector number
  */
uint32_t sector_name_to_number(uint32_t sector_name)
{
    if(11 >= sector_name){
        return CTL_SN(sector_name);
    }else if(23 >= sector_name){
        return CTL_SN(sector_name + 4);
    }else if(27 >= sector_name){
        return CTL_SN(sector_name - 12);
    }else{
        while(1);
    }
}

/*!
  \brief      write 8 bit length data to a given address
  \param[in]  address: a given address(0x08000000~0x082FFFFF)
  \param[in]  length: data length
  \param[in]  data_8: data pointer
  \param[out] none
  \retval     none
  */
void fmc_write_8bit_data(uint32_t address, uint16_t length, int8_t* data_8)
{
    fmc_sector_info_struct start_sector_info;
    fmc_sector_info_struct end_sector_info;
    uint32_t sector_num,i;

    printf("\r\nFMC half_word program operation:\n");
    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);
    /* get the information of the start and end sectors */
    start_sector_info = fmc_sector_info_get(address);
    end_sector_info = fmc_sector_info_get(address + 2*length);
    /* erase sector */
    for(i = start_sector_info.sector_name; i <= end_sector_info.sector_name; i++){
        sector_num = sector_name_to_number(i);
        if(FMC_READY != fmc_sector_erase(sector_num)){
            while(1);
        }
    }

    /* write data_8 to the corresponding address */
    for(i=0; i<length; i++){
        if(FMC_READY == fmc_byte_program(address, data_8[i])){
            address++;
        }else{
            while(1);
        }
    }
    /* lock the flash program erase controller */
    fmc_lock();
    printf("\r\nWrite complete!\n");
    printf("\r\n");
}

/*!
  \brief      read 8 bit length data to a given address
  \param[in]  address: a given address(0x08000000~0x082FFFFF)
  \param[in]  length: data length
  \param[in]  data_8: data pointer
  \param[out] none
  \retval     none
  */
void fmc_read_8bit_data(uint32_t address, uint16_t length, int8_t* data_8)
{
    uint8_t i;
    for(i=0; i<length; i++){
        data_8[i] = *(__IO int8_t*)address;
        address++;
    }
}

#define WRITE_ADDRESS_BYTE          ((uint32_t)0x0807f800)
int8_t data_8_1[10] = {0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D,0x3D};
int8_t data_8_2[10];

int main(void)
{
    void *mem_ptr = NULL;
    uint32_t mem_addr = 0;
    board_init();
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOC);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART5);

    /* configure the USART0 TX pin and USART0 RX pin */
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_6);
    gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_7);

    /* configure USART0 TX as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    /* configure USART0 RX as alternate function push-pull */
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    /* USART configure */
    usart_deinit(USART5);
    usart_baudrate_set(USART5, 460800U);
    usart_receive_config(USART5, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART5, USART_TRANSMIT_ENABLE);
    usart_enable(USART5);

    while(1)
    {
        delay_ms(1000);
        printf("wzh\n");
        /* write 8 bit length data to a given address */
//        fmc_write_8bit_data(WRITE_ADDRESS_BYTE, 10, data_8_1);
        /* read 8 bit length data from a given address */
        fmc_read_8bit_data(WRITE_ADDRESS_BYTE, 10, data_8_2);

    }
}


#endif
