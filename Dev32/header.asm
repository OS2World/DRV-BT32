; $Id: header.asm,v 1.15 2003/07/22 22:05:10 smilcke Exp $

;
; header.asm
; Autor:               Stefan Milcke
; Erstellt am:         29.03.2003
; Letzte Aenderung am: 22.07.2003
;
       .386p
       include seg32.inc

;##############################################################################
;* DATA16
;##############################################################################
DATA16 segment
       public  device_header
       public  help_header
       public  DriverFileName

;******************************************************************************
; device header                                                               *
;******************************************************************************
help_header    dw OFFSET DATA16:device_header  ; Pointer to next device header
               dw SEG DATA16:device_header
               dw 1000100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:help_stub_strategy    ; Strategy routine entry point
               dw 0				    ; IDC routine entry point
               db 'BT32HLP$'                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b

device_header  dw offset DATA16:bttv1_header
               dw SEG DATA16:bttv1_header
;device_header  dd -1
               dw 1001100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:device_stub_strategy  ; Strategy routine entry point
               dw 0                            ; IDC routine entry point
               db 'BT32$   '                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b
bttv1_header   dw offset DATA16:bttv2_header
               dw SEG DATA16:bttv2_header
               dw 1101100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:stream_stub_strategy1  ; Strategy routine entry point
               dw offset CODE16:stream_stub_idc1       ; IDC routine entry point
               db 'STBTTV1$'                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b
bttv2_header   dw offset DATA16:bttv3_header
               dw SEG DATA16:bttv3_header
               dw 1101100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:stream_stub_strategy2  ; Strategy routine entry point
               dw offset CODE16:stream_stub_idc2       ; IDC routine entry point
               db 'STBTTV2$'                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b
bttv3_header   dw offset DATA16:bttv4_header
               dw SEG DATA16:bttv4_header
               dw 1101100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:stream_stub_strategy3  ; Strategy routine entry point
               dw offset CODE16:stream_stub_idc3       ; IDC routine entry point
               db 'STBTTV3$'                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b
bttv4_header   dd -1
               dw 1101100110000000b            ; Device attributes
;                 ||||| +-+   ||||
;                 ||||| | |   |||+------------------ STDIN
;                 ||||| | |   ||+------------------- STDOUT
;                 ||||| | |   |+-------------------- NULL
;                 ||||| | |   +--------------------- CLOCK
;                 ||||| | |
;                 ||||| | +------------------------+ (001) OS/2
;                 ||||| |                          | (010) DosDevIOCtl2 + SHUTDOWN
;                 ||||| +--------------------------+ (011) Capability bit strip
;                 |||||
;                 ||||+----------------------------- OPEN/CLOSE (char) or Removable (blk)
;                 |||+------------------------------ Sharing support
;                 ||+------------------------------- IBM
;                 |+-------------------------------- IDC entry point
;                 +--------------------------------- char/block device driver

               dw offset CODE16:stream_stub_strategy4  ; Strategy routine entry point
               dw offset CODE16:stream_stub_idc4       ; IDC routine entry point
               db 'STBTTV4$'                   ; Device name
               db 8 dup (0)                    ; Reserved
               dw 0000000000010011b            ; Level 3 device driver capabilities
;                            |||||
;                            ||||+------------------ DosDevIOCtl2 + Shutdown
;                            |||+------------------- More than 16 MB support
;                            ||+-------------------- Parallel port driver
;                            |+--------------------- Adapter device driver
;                            +---------------------- InitComplete
               dw 0000000000000000b

DriverFileName db "BT32HLP$", 0

DATA16 ends

CODE16 segment
       assume cs:CODE16, ds:DATA16
       extrn   help_stub_strategy : far
       extrn   device_stub_strategy : far
       extrn   stream_stub_strategy1 : far
       extrn   stream_stub_idc1 : far
       extrn   stream_stub_strategy2 : far
       extrn   stream_stub_idc2 : far
       extrn   stream_stub_strategy3 : far
       extrn   stream_stub_idc3 : far
       extrn   stream_stub_strategy4 : far
       extrn   stream_stub_idc4 : far
CODE16 ends

end

