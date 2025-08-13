/**
* @file                hal_storage.c
* @brief
* @author              
* @version             V0.1.0
* @date                2019.2.22
* @details
* @copyright           (C) COPYRIGHT 2019 SHENZHEN CHIPSEA TECHNOLOGIES CO.,LTD.ALL RIGHTS RESERVED
* @note
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HAL_STORAGE_H
#define HAL_STORAGE_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define FT_STORAGE_SYSTEM_PARAM_BLOCK_OFFSET        0
#define FT_STORAGE_SYSTEM_PARAM_BLOCK_SIZE          2
#define FT_STORAGE_USER_PARAM_BLOCK_OFFSET          (FT_STORAGE_SYSTEM_PARAM_BLOCK_OFFSET+FT_STORAGE_SYSTEM_PARAM_BLOCK_SIZE)
#define FT_STORAGE_USER_PARAM_BLOCK_SIZE            2
#define FT_STORAGE_FACTORY_PARAM_BLOCK_OFFSET       (FT_STORAGE_USER_PARAM_BLOCK_OFFSET+FT_STORAGE_USER_PARAM_BLOCK_SIZE)
#define FT_STORAGE_FACTORY_PARAM_BLOCK_SIZE         2

#define PROFILE_BLOCK_SEC_START                     (112)       // 56K = 112block
#define PROFILE_BLOCK_SEC_SIZE                      (1 << 9)    //(1 << 10) = *512
#define PROFILE_BLOCK_HEADER_SIZE                   sizeof(TypeDef_ProfileBlockHeader)
#define PROFILE_BLOCK_MAGICCODE_SIZE                sizeof(unsigned int)
#define PROFILE_BLOCK_DATACRC32_SIZE                sizeof(unsigned int)
#define PROFILE_BLOCK_DATA_SIZE                     (PROFILE_BLOCK_SEC_SIZE - PROFILE_BLOCK_HEADER_SIZE)

#define BLOCK_MIN           PBlock1
#define BLOCK_MAX           PBlock11

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    PBlock1 = 0x00,
    PBlock2,
    PBlock3,
    PBlock4,
    PBlock5,
    PBlock6,
    PBlock7,
    PBlock8,
    PBlock9,
    PBlock10,
    PBlock11,
} TypeDef_BlockNum;

typedef enum
{
    HAL_OK       = 0x00,
    HAL_ERROR    = 0x01,
    HAL_BUSY     = 0x02,
    HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;

typedef struct
{
    unsigned int MagicCode;
    unsigned int DataCRC32;
    unsigned int Flag;
} TypeDef_ProfileBlockHeader;

typedef struct
{
    TypeDef_ProfileBlockHeader PBHeader;
    unsigned char PBData[PROFILE_BLOCK_DATA_SIZE];
} TypeDef_ProfileBlock;

//!< ISP Control Register (ISPCON) definitions
#define FMC_ISPFF             ((uint32_t)0x00000040)
#define FMC_ISPEN             ((uint32_t)0x00000001)
#define FMC_EXIT_STANDBY      ((uint32_t)0x00000080)
#define FMC_LDUEN             ((uint32_t)0x00000020)
#define FMC_CFGUEN            ((uint32_t)0x00000010)
#define FMC_APUEN             ((uint32_t)0x00000008)
#define FMC_CLK_HIRC          ((uint32_t)0x00000100)
//!<#define FMC_CLK_HIRC          ((unsigned int)0x00000500)
//!< ICP Control Register (ICPCON) definitions
#define FMC_ICPEN             ((uint32_t)0x00000001)

//!< ISP Command (ISPCMD) definitions
#define FMC_STANDBY             ((uint32_t)0x00000030)
#define FMC_READ                ((uint32_t)0x00000000)
#define FMC_PROGRAM             ((uint32_t)0x00000021)
#define FMC_PAGE_ERASE          ((uint32_t)0x00000022)
#define FMC_READ_CID            ((uint32_t)0x0000000B)
#define FMC_READ_DID            ((uint32_t)0x00000004)
#define FMC_PAGE_SWITCH         ((uint32_t)0x0000002e)

//!< ISP Trigger Control Register (ISPTRG) definitions
#define FMC_ISPGO               ((uint32_t)0x00000001)

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
HAL_StatusTypeDef   hal_storage_init(void);
HAL_StatusTypeDef   hal_storage_write(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf);
HAL_StatusTypeDef   hal_storage_read(unsigned char ucBlock, unsigned short usOffset, unsigned short usLength, void *pBuf);
/* Peripheral Control functions ----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* HAL_PROFILE_H */


/************************ (C) COPYRIGHT Chipsea Tech *****END OF FILE****/
