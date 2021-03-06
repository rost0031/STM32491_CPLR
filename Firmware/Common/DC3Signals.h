/**
 * @file 	DC3Signals.h
 * @brief   Contains all the signals used by the state machines
 *
 * @date   	06/03/2014
 * @author 	Harry Rostovtsev
 * @email  	rost0031@gmail.com
 * Copyright (C) 2014 Harry Rostovtsev. All rights reserved.
 *
 * @addtogroup groupApp
 * @{
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DC3SIGNALS_H_
#define DC3SIGNALS_H_

/* Includes ------------------------------------------------------------------*/
#include "qep_port.h"
#include "DC3CommApi.h"
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/**
 * @brief Event type for transferring large data.
 */
typedef struct LrgDataEvtTag {
/* protected: */
    QEvt       super;
    DC3MsgRoute_t src;                                   /**< Source of the data */
    DC3MsgRoute_t dst;                              /**< Destination of the data */
    uint16_t   dataLen;                    /**< Length of the data in dataBuf */
    uint8_t    dataBuf[DC3_MAX_MSG_LEN];       /**< Buffer that holds the data */
} LrgDataEvt;

/**
 * @enum First signal
 * Q_USER_SIG is the first signal that user applications are allowed to use.
 * Should always be at the very top of the very first enum list
 */
enum FirstSignal {
   FIRST_SIG = Q_USER_SIG
};

/**
 * @enum Signals used by CommMgr
 */
enum CommSignals {
   ETH_UDP_SEND_OUT_SIG = FIRST_SIG, /** This signal must start at the previous category max signal */
   ETH_UDP_RECEIVED_SIG,
   SER_RECEIVED_SIG,
   CLI_RECEIVED_SIG,
   CLI_SEND_DATA_SIG,
   COMM_MGR_TIMEOUT_SIG,
   COMM_OP_TIMEOUT_SIG,
   MSG_PROCESS_SIG,
   BOOT_APPL_SIG,
   BOOT_RESET_SIG,
   COMM_MAX_SIG,
};

/**
 * @enum Signals used by SerialMgr
 */
enum SerialMgrSignals {
   UART_DMA_START_SIG = COMM_MAX_SIG, /** This signal must start at the previous category max signal */
   UART_DMA_DONE_SIG,
   UART_DMA_TIMEOUT_SIG,
   UART_DMA_MAX_SIG
};

/**
 * @enum Signals used by I2CMgr
 */
enum I2CMgrSignals {
   I2C_READ_START_SIG = UART_DMA_MAX_SIG, /** This signal must start at the previous category max signal */
   I2C_READ_DONE_SIG,
   I2C_WRITE_START_SIG,
   I2C_WRITE_DONE_SIG,
   I2C_TIMEOUT_SIG,
   I2C_CHECK_EV_SIG,
   I2C_DMA_TIMEOUT_SIG,
   I2C_EV_MASTER_MODE_SELECT_SIG,
   I2C_EV_MASTER_TX_MODE_SELECTED_SIG,
   I2C_SENT_MSB_ADDR_SIG,
   I2C_SENT_LSB_ADDR_SIG,
   I2C_RECOVERY_TIMEOUT_SIG,
   I2C_MAX_SIG
};


/**
 * @enum Signals used by I2CBusMgr
 */
enum I2CBusMgrSignals {
   I2C_BUS_CHECK_FREE_SIG = I2C_MAX_SIG, /** This signal must start at the previous category max signal */
   I2C_BUS_GLOBAL_TOUT_SIG,
   I2C_BUS_OP_TOUT_SIG,
   I2C_BUS_SETTLE_TIMER_SIG,
   I2C_BUS_START_BIT_SIG,
   I2C_BUS_STOP_BIT_SIG,
   I2C_BUS_SELECT_MASTER_SIG,
   I2C_BUS_SET_DIR_TX,
   I2C_BUS_SET_DIR_RX,
   I2C_BUS_SEND_7BIT_ADDR_SIG,
   I2C_BUS_SEND_10BIT_ADDR_SIG,
   I2C_BUS_SEND_DEV_ADDR_SIG,
   I2C_BUS_READ_MEM_SIG,
   I2C_BUS_WRITE_MEM_SIG,
   I2C_BUS_DMA_DONE_SIG,
   I2C_BUS_ACK_EN_SIG,
   I2C_BUS_ACK_DIS_SIG,
   I2C_BUS_DONE_SIG,
   I2C_BUS_MAX_SIG
};

/**
 * @enum Signals used by I2C1DevMgr
 */
enum I2C1DevMgrSignals {
   I2C1_DEV_TIMEOUT_SIG = I2C_BUS_MAX_SIG, /** This signal must start at the previous category max signal */
   I2C1_DEV_OP_TIMEOUT_SIG,
   I2C1_DEV_RAW_MEM_READ_SIG,
   I2C1_DEV_RAW_MEM_WRITE_SIG,
   I2C1_DEV_POST_WRITE_TIMER_SIG,
   I2C1_DEV_READ_DONE_SIG,
   I2C1_DEV_WRITE_DONE_SIG,
   I2C1_DEV_PRIVATE_SIG,
   I2C1_DEV_MAX_SIG
};

/**
 * @enum Signals used by FlashMgr
 */
enum FlashMgrSignals {
   FLASH_OP_START_SIG = I2C1_DEV_MAX_SIG, /** This signal must start at the previous category max signal */
   FLASH_DATA_SIG,
   FLASH_DONE_SIG,
   FLASH_ERROR_SIG,
   FLASH_TIMEOUT_SIG,
   FLASH_OP_TIMEOUT_SIG,
   FLASH_OP_DONE_SIG,
   FLASH_FW_DONE_SIG,
   FLASH_NEXT_STEP_SIG,
   RAM_TIMEOUT_SIG,
   RAM_OP_START_SIG,
   RAM_TEST_START_SIG,
   RAM_TEST_DONE_SIG,
   FLASH_MAX_SIG
};

/**
 * @enum Signals used by SysMgr
 */
enum SysMgrSignals {
   DB_GET_ELEM_SIG = FLASH_MAX_SIG, /** This signal must start at the previous category max signal */
   DB_SET_ELEM_SIG,
   DB_GET_ELEM_DONE_SIG,
   DB_SET_ELEM_DONE_SIG,
   DB_ERROR_SIG,
   DB_READ_SIG,
   DB_WRITE_SIG,
   DB_OP_DONE_SIG,
   DB_ACCESS_TIMEOUT_SIG,
   DB_FULL_RESET_SIG,
   DB_INTRNL_FULL_RESET_SIG,
   DB_INTRNL_CHK_ELEM_SIG,
   DB_FLASH_READ_DONE_SIG,
   SYS_MGR_TIMEOUT_SIG,
   DBG_MENU_SIG,
   DBG_LOG_SIG,
   SYS_MGR_MAX_SIG
};

/**
 * @enum Signals used to communicate to CPLR task
 */
enum CplrSignals {
   /* Signals that use the EthEvt type event tag - start */
   CPLR_ETH_SYS_TEST_SIG = SYS_MGR_MAX_SIG, /** This signal must start at the previous category max signal */

   /* ... insert signals here */
   /* Signals that use the EthEvt type event tag - end */

   CPLR_MAX_SIG
};

/* INSERT NEW SIGNAL CATEGORIES BEFORE HERE...POINT MAX_SHARED_SIG TO LAST SIGNAL */

/**
 * @enum Final signal.
 * @note MAX_SHARED_SIG should be updated to point to the very last signal
 * before it.
 */
enum FinalSignal {
   MAX_SHARED_SIG = CPLR_MAX_SIG,  /**< Last published shared signal - should always be at the bottom of this list */
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/**
 * @}
 * end addtogroup groupApp
 */

#endif                                                       /* DC3SIGNALS_H_ */
/******** Copyright (C) 2014 Harry Rostovtsev. All rights reserved *****END OF FILE****/
