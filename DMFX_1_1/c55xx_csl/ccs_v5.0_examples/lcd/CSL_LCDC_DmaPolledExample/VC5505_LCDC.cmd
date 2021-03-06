
MEMORY
{
    MMR     (RW) : origin = 0000000h length = 000200h /* MMRs */
    DARAM_0 (RW) : origin = 0000200h length = 005E00h /* on-chip DARAM 0 */
    DARAM_1 (RW) : origin = 0006000h length = 002000h /* on-chip DARAM 1 */
    SARAM_0 (RW) : origin = 0008000h length = 018000h /* on-chip SARAM 0 */
    SARAM_1 (RW) : origin = 0020000h length = 017E00h /* on-chip SARAM 1 */
    VECS    (RX) : origin = 0037E00h length = 000080h /* on-chip ROM vectors */
}
 
SECTIONS
{
  .intvec    : > MMR ALIGN = 256
  .text      : > SARAM_0 
  .data      : > DARAM_0  
  .cinit     : > SARAM_0 
  .switch    : > SARAM_0
  .stack     : > DARAM_0 
  .sysstack  : > DARAM_0 
  .bss       : > DARAM_0
  .const     : > SARAM_1
  .sysmem    : > DARAM_0 
  .cio	     : > DARAM_0
  .buffer1   : > DARAM_1
  .buffer2   : > SARAM_1
  vectors (NOLOAD) 
}