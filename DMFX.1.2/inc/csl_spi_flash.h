/*
 * csl_spi_flash.h
 *
 *  Created on: Nov 29, 2015
 *      Author: Diego de la Cruz
 */

#ifndef CSL_SPI_FLASH_H_
#define CSL_SPI_FLASH_H_
#include "csl_general.h"

#define CSL_TEST_FAILED         (1)
/**< Error Status to indicate Test has Failed */
#define CSL_TEST_PASSED         (0)
/**< Error Status to indicate Test has Passed */

#define	CSL_SPI_BUF_LEN			(64)
/**< SPI Buffer length used for reading and writing */

#define	SPI_CLK_DIV				(25)
/**< SPI Clock Divisor */

#define	SPI_FRAME_LENGTH		(1)
/**< SPI Frame length */

/**
 * \brief API to test SPI read\write API by writing to SPI Flash/EEPROM
 */
Int16 spi_sample(void);

#if (defined (C5535_EZDSP) || defined (CHIP_C5517))
/**
 * \brief API to test SPI read\write API by writing to SPI Flash
 */
Int16 spi_flash_sample(void);
#else
/**
 * \brief API to test SPI read\write API by writing to EEPROM
 */
Int16 spi_eeprom_sample(void);
#endif

void csl_spi_test(void);

#endif /* CSL_SPI_FLASH_H_ */
