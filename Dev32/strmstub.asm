; $Id: strmstub.asm,v 1.5 2004/02/16 21:24:59 smilcke Exp $

;
; strmstub.asm
; Autor:               Stefan Milcke
; Erstellt am:         27.06.2003
; Letzte Aenderung am: 15.02.2004
;
       .386p

       INCL_DOS        equ 1
       INCL_DOSERRORS  equ 1
       include os2.inc
       include seg32.inc
       include r0thunk.inc
       include rp.inc
       include rm.inc
       include devhlp.inc
       include infoseg.inc
       include lxbases.inc
       include lxbasem.inc
       include lxapihlp.inc
INCL_DEF = 1
       include basemaca.inc

; Status word masks
STERR      EQU 8000H       ; Bit 15 - Error
STINTER    EQU 0400H       ; Bit 10 - Interim character
STBUI      EQU 0200H       ; Bit  9 - Busy
STDON      EQU 0100H       ; Bit  8 - Done
STECODE    EQU 00FFH       ; Error code

;##############################################################################
;* DATA16
;##############################################################################
DATA16 segment
       extrn __OffFinalDS16 : byte
DATA16 ends

;##############################################################################
;* CODE16
;##############################################################################
CODE16 segment
       assume cs:CODE16, ds:DATA16
       extrn __OffFinalCS16 : byte

;******************************************************************************
;* Strategy routines                                                          *
;******************************************************************************
;---------------------------- stream_stub_strategy1 ----------------------------
       ALIGN   2
       public  stream_stub_strategy1
stream_stub_strategy1 proc far
       mov     ecx,1
       jmp     stream_stub_strategy
stream_stub_strategy1 endp
;---------------------------- stream_stub_strategy2 ----------------------------
       ALIGN   2
       public  stream_stub_strategy2
stream_stub_strategy2 proc far
       mov     ecx,2
       jmp     stream_stub_strategy
stream_stub_strategy2 endp
;---------------------------- stream_stub_strategy3 ----------------------------
       ALIGN   2
       public  stream_stub_strategy3
stream_stub_strategy3 proc far
       mov     ecx,3
       jmp     stream_stub_strategy
stream_stub_strategy3 endp
;---------------------------- stream_stub_strategy4 ----------------------------
       ALIGN   2
       public  stream_stub_strategy4
stream_stub_strategy4 proc far
       mov     ecx,4
       jmp     stream_stub_strategy
stream_stub_strategy4 endp

;----------------------------- stram_stub_strategy -----------------------------
       ALIGN   2
       public  stream_stub_strategy
stream_stub_strategy proc far
       enter   0,0
       and     sp,0fffch   ; align stack
       mov     ax,DATA16
       mov     ds,ax
       movzx   eax,byte ptr es:[bx].reqCommand
       cmp     eax,0       ; Init
       jz      short stream_stub_init
       push    bx
       push    es
       mov     ax,bx
       xor     ebx,ebx
       mov     bx,ax
       mov     ax,es
       mov     fs,ax   ; fs:ebx = req. packet
       call    far ptr FLAT:STREAM_STRATEGY_
       pop     es
       pop     bx
       mov     word ptr es:[bx].reqStatus,ax ; status code
       leave
       ret
stream_stub_strategy endp

stream_stub_init proc far
       mov     ax,STDON
       mov     word ptr es:[bx].reqStatus, ax  ; status code (ret by device_init)
       mov     word ptr es:[bx].o_codeend,offset CODE16:__OffFinalCS16
       mov     word ptr es:[bx].o_dataend,offset DATA16:__OffFinalDS16
       leave
       ret
stream_stub_init endp

;------------------------------- stream_stub_idc1 ------------------------------
       ALIGN   2
       public  stream_stub_idc1
stream_stub_idc1 proc far
       mov     ecx,1
       jmp     stream_stub_idc
stream_stub_idc1 endp
;------------------------------- stream_stub_idc2 ------------------------------
       ALIGN   2
       public  stream_stub_idc2
stream_stub_idc2 proc far
       mov     ecx,2
       jmp     stream_stub_idc
stream_stub_idc2 endp
;------------------------------- stream_stub_idc3 ------------------------------
       ALIGN   2
       public  stream_stub_idc3
stream_stub_idc3 proc far
       mov     ecx,3
       jmp     stream_stub_idc
stream_stub_idc3 endp
;------------------------------- stream_stub_idc4 ------------------------------
       ALIGN   2
       public  stream_stub_idc4
stream_stub_idc4 proc far
       mov     ecx,4
       jmp     stream_stub_idc
stream_stub_idc4 endp

;------------------------------- stream_stub_idc -------------------------------
; bp+6 -> Request packet offset
; bp+8 -> Request packet selector
       ALIGN 2
       public  stream_stub_idc
stream_stub_idc proc far
       enter   0,0
       and     sp,0fffch
       push    esi
       push    ds
       push    fs
       push    DATA16
       pop     ds
       mov     ax,word ptr [bp+8]  ; selector
       xor     esi,esi
       mov     si,word ptr [bp+6] ; offset
       mov     dl,5bh
       call    dword ptr ds:DevHelpInit
       jc      short @stream_stub_idc_err
       mov     ebx,eax
       call    far ptr FLAT:STREAM_IDC_
       pop     fs
       pop     ds
       pop     esi
       mov     dx,ax
       shr     eax,16
       xchg    ax,dx
       leave
       retf
@stream_stub_idc_err:
       pop     fs
       pop     ds
       pop     esi
       mov     ax,1
       leave
       retf
stream_stub_idc endp

thunk3216_SHDENTRYPOINT proc far
       sub     esp,ecx
       sub     esp,10
       lea     edi,[esp]
       push    ecx
       mov     esi,ebx
@@:
       mov     al,[esi]
       mov     ss:[edi],al
       inc     esi
       inc     edi
       dec     ecx
       jnz     @B
       mov     ss:[edi],edx
       lea     esi,[esp]
       push    ss
       push    si
;       int 3
       call    dword ptr ss:[edi]
       pop     si
       pop     ss
       nop
       nop
       pop     ecx
       add     esp,10
       add     esp,ecx
       jmp     far ptr FLAT:thunk1632_SHDENTRYPOINT
thunk3216_SHDENTRYPOINT endp

CODE16 ends

;*******************************************************************************
;* CODE32                                                                      *
;*******************************************************************************
CODE32 segment
       ASSUME CS:FLAT, DS:FLAT, ES:FLAT
       ALIGN 4
STREAM_STRATEGY_ proc far
       public  STREAM_STRATEGY_
       extrn   STREAM_STRATEGY_ENTRY : NEAR
       mov     eax,DOS32FLATDS
       mov     ds,eax
       mov     es,eax
       cmp     dword ptr LX_FixSelDPLPtr,0
       jne     @F
       call    CODE32:init_lxapi_asm32
       cmp     dword ptr LX_FixSelDPLPtr,0
       je      @STREAM_STRATEGY_ERROR
@@:    ThunkStackTo32
       cmp     eax,0
       jne     @STREAM_STRATEGY_ERROR
       call    STREAM_STRATEGY_ENTRY
       ThunkStackTo16
@STREAM_STRATEGY_ERROR:
       retf
STREAM_STRATEGY_ endp

;******************************************************************************
;* IDC routine                                                                *
;******************************************************************************
;in: eax = fileno (moved to ebx)
;    fs:ebx = ULONG packet
;return value in eax
STREAM_IDC_ proc far
       public  STREAM_IDC_
       extrn   STREAM_IDC_ENTRY : NEAR
       push    es
       push    fs
       push    gs
       push    ebx
       mov     eax,DOS32FLATDS
       mov     ds,eax
       mov     es,eax
       ThunkStackTo32
       cmp     eax,0
       jne     @STREAM_IDC_ERROR
       call    STREAM_IDC_ENTRY
       ThunkStackTo16
@STREAM_IDC_ERROR:
       pop     ebx
       pop     gs
       pop     fs
       pop     es
@@:
       retf
STREAM_IDC_ endp

; In: edx = PSHDFN pfn
;     ebx = lin ptr to data
;     ecx = len of data
CALLSHDENTRYPOINT32_ proc near
       public CALLSHDENTRYPOINT32_
       ThunkStackTo16_Int
       jmp     far ptr CODE16:thunk3216_SHDENTRYPOINT
thunk1632_SHDENTRYPOINT:
       ThunkStackTo32_Int
       ret
CALLSHDENTRYPOINT32_ endp

CODE32 ends
end

