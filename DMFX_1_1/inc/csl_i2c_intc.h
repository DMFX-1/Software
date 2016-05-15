/*
 * csl_i2c_intc.h
 *
 *  Created on: Dec 12, 2015
 *      Author: Diego de la Cruz
 */

#ifndef CSL_I2C_INTC_H_
#define CSL_I2C_INTC_H_
#include "csl_types.h"

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_PAGE_SIZE        (32)
#define CSL_I2C_DATA_SIZE        (FXCMD_SIZE)	//(7)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_BUS_FREQ         (400)		// I2C bus clock frequency in kHz <=400
#define CSL_I2C_SYS_CLK          (100)
#define	CSL_I2C_ICCLK_VAL	     ((CSL_I2C_MODULE_CLOCK_FREQ*500)/CSL_I2C_BUS_FREQ-5)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)

#ifdef DMFX_PROC2 	//DMFX2 Master Tx Device
	#define I2C_MASTER_TX
	#define CSL_I2C_OWN_ADDR         (0x2E)
	#define CSL_I2C_DEVICE_ADDR		 (0x2F)
	CSL_Status CSL_i2cIntcTest(void);
	void i2c_isr(void);
	void CSL_i2cAlCallback(void);
	void CSL_i2cNackCallback(void);
	void CSL_i2cArdyCallback(void);
	void CSL_i2cRxCallback(void);
	void CSL_i2cTxCallback(void);
	void CSL_i2cScdCallback(void);
	void CSL_i2cAasCallback(void);
	void I2c_Intc_MasterTx(void);
	CSL_Status I2c_Intc_Init(void);
	CSL_Status I2c_Intc_reInit(void);
	CSL_Status I2c_Intc_MasterTx_Test(void);
	CSL_Status I2C_Intc_Deinit(void);
	void Int0_ISR(void);
#endif

#ifdef DMFX_PROC1 	//DMFX1 Slave Rx Device
	#define I2C_SLAVE_RX
	#define CSL_I2C_OWN_ADDR         (0x2F)
	#define CSL_I2C_DEVICE_ADDR		 (0x2E)
	CSL_Status CSL_i2cIntcTest(void);
	void i2c_isr(void);
	void CSL_i2cAlCallback(void);
	void CSL_i2cNackCallback(void);
	void CSL_i2cArdyCallback(void);
	void CSL_i2cRxCallback(void);
	void CSL_i2cTxCallback(void);
	void CSL_i2cScdCallback(void);
	void CSL_i2cAasCallback(void);
	void I2c_Intc_SlaveRx(void);
	CSL_Status I2c_Intc_Init(void);
	CSL_Status I2c_Intc_reInit(void);
	CSL_Status I2c_Intc_Deinit(void);
	CSL_Status I2c_Codec_Init(void);
	CSL_Status DRC();
	void AGC();
	CSL_Status Codec_Write(Uint16 regAddr, Uint16  regValue);
	CSL_Status Codec_Read(Uint16  regAddr, Uint16 *regValue);
	void Int0_ISR(void);
#endif

#endif /* CSL_I2C_INTC_H_ */
