; $Id: end.asm,v 1.19 2004/02/16 21:24:58 smilcke Exp $

;
; end.asm for BT8X8 32bit device driver
; Autor:               Stefan Milcke
; Erstellt am:         26.11.2001
; Letzte Aenderung am: 15.02.2004
	.386p
       include seg32.inc

DATA16 segment
       public __OffFinalDS16
__OffFinalDS16 label byte
DATA16 ends

CODE16 segment
       public __OffFinalCS16
__OffFinalCS16 label byte
CODE16 ends

TEXT32 segment
       public __OffFinalCS32
__OffFinalCS32 label byte
TEXT32 ends

DATA32END segment
       public __OffFinalDS32
__OffFinalDS32 label byte
DATA32END ends

end
