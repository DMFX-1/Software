******************************************************************************
* iic.inc                                                                    *
*                                                                            *
* This is proprietary information, not to be published -- TI INTERNAL DATA   *
* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.          *
*                                                                            *
* With new address for MD100/DA225                                           *
*                                                                            *
******************************************************************************

*********************************************************************
* Constants                                                         *
*********************************************************************


IIC_MAX_POLL    .set 6000h    ; The max number of times to poll for status
ICRRDY_FLAG     .set 0008h
ICXRDY_FLAG     .set 0010h
ICBB_FLAG       .set 1000h
ICNACK_FLAG     .set 0002h

*********************************************************************
*     I2C Peripheral registers                                      *
*********************************************************************

ICOAR           .set 1A00h        ; I2C Own Address register
ICIMR           .set 1A04h        ; I2C Interrupt Mask/Status register
ICSTR           .set 1A08h        ; I2C Interrupt Flag register
ICCLKL          .set 1A0Ch        ; I2C Clock Divider Low register
ICCLKH          .set 1A10h        ; I2C Clock Divider High register
ICCNT           .set 1A14h        ; I2C Data Count register
ICDRR           .set 1A18h        ; I2C Data Receive register
ICSAR           .set 1A1Ch        ; I2C Slave Address register
ICDXR           .set 1A20h        ; I2C Data Transmit register
ICMDR           .set 1A24h        ; I2C Mode register
ICIVR           .set 1A28h        ; I2C Interrupt Vector register
ICGPIO          .set 1A2Ch        ; I2C GPIO register
ICPSC           .set 1A30h        ; I2C Prescaler register
;ICXSR                             ; I2C Data Transmit Shift Register
;ICRSR                             ; I2C Data Receive Shift Register
