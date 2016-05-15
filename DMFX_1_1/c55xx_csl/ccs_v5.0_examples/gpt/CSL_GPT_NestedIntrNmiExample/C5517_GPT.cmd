-stack    0x3000                /* PRIMARY STACK SIZE    */
-sysstack 0x2000                /* SECONDARY STACK SIZE  */
-heap     0x2000                /* HEAP AREA SIZE        */  

MEMORY
{
    MMR    (RW) : origin = 0000000h length = 0000C0h /* MMRs */
    VEC    (RX) : origin = 00000C0h length = 000300h /* on-chip ROM vectors */
    DATA   (RW) : origin = 0000400h length = 00FBFFh /* on-chip DARAM  */
    PROG   (RW) : origin = 0010000h length = 03FFFFh /* on-chip SARAM  */    
}

SECTIONS
{
   vectors(NOLOAD)
   vector    : > VEC    ALIGN = 256
  .text      : > PROG 
  .data      : > DATA  
  .cinit     : > PROG 
  .switch    : > PROG
  .stack     : > DATA 
  .sysstack  : > DATA 
  .bss       : > DATA
  .sysmem    : > PROG
  .const     : > DATA
  .cio	     : > PROG
}
