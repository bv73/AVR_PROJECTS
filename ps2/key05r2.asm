.pagewidth !160
*****************************************************
*                                                   *
*   101 Key, IBM Keyboard Decoder for 68HC705J1A.   *
*                                                   *
*      Craig Peacock (cpeacock@senet.com.au)        *
*                    6th May 1998                   *
*                                                   *
*        Includes ALTDEC & CTRLHEX Routines         *
*                                                   *
*****************************************************

; Port A

nreset  equ     5       ; Reset Output (Open Drain)
clk     equ     6       ; Clock I/O
data    equ     7       ; Data I/O

; Port B

TXD     equ     5       ; Transmit Data

; Equates for LED Byte

pscrlck equ     4       ; If true, Scroll Lock Pressed
pnumlck equ     3       ; If true, Num Lock Pressed
caplock equ     2       ; If true, Caps Lock is On (Active)
numlock equ     1       ; If true, Num Lock is On (Active)
scrlock equ     0       ; If true, Scroll Lock is On (Active)

; Equates for Status Flag, Byte

rctrl   equ     7       ; If true, Right Ctrl Pressed
lctrl   equ     6       ; If true, Left Ctrl Pressed
ralt    equ     5       ; If true, Right Alt Pressed
lalt    equ     4       ; If true, Left Alt Pressed

caploc  equ     2       ; If true, Caps Lock Pressed
rshift  equ     1       ; If true, Right Shift Key Pressed
lshift  equ     0       ; If true, Left Shift Key Pressed

        org     ram

byte    rmb     1       ; Used to hold byte, during Trans & Rec
status  rmb     1       ; Status Flags
LED     rmb     1       ; LED Flags
asc     rmb     3       ; Used for altdec & ctrlhex
PAR     rmb     1       ; Used during receive for Parity Tracking

        org     $7F8

        dw      start   ; Timer Interrupt Vector
        dw      start   ; IRQ Vector
        dw      start   ; Software Interrupt Vector
        dw      start   ; Reset Vector

        org     $7F1    ; Mask Option Register (MOR)
        db      $20     ; Osc Internal Res Enabled

        org     rom

start   lda     #%00000000      ;PORTA - Inputs
        sta     ddra            ;Set Data Direction Register
        sta     pdra            ;Enable Pull Downs
        sta     PORTA           ;Set nReset, Clk & Data low

        lda     #%00100111      ;PORTB - I/O
        sta     ddrb            ;Set Data Direction Register
        sta     pdrb            ;Dis/enable Pull Downs
        lda	#%00100000	;Transmit Idle
	sta	PORTB

        jmp     rstflag         ;No Attempt to Reset Keyboard made
                                ;as keyboard would still be in POST
                                ;or BAT Tests, if power applied at
                                ;the same time than the HC705.

*****************************************************
*                                                   *
* reset - Sends a Reset Command to the Keyboard.    *
*         Not a very good effort to reset keyboard, *
*         as it doesn't check for ACK or BAT        *
*         Completion Code. I.e. Reset may not of    *
*         even Worked!                              *
*                                                   *
*****************************************************

reset   lda     #$FF            ;Reset Keyboard
        sta     byte
        jsr     transmit

*****************************************************
*                                                   *
* rstflag - Resets Status and LED Flags. Used when  *
*           a successful Bat Completion code is     *
*           sent to sync keyboard's LED's to 705's  *
*           status register                         *
*                                                   *
*****************************************************

rstflag clr     status
        clr     LED

*****************************************************
*                                                   *
* main - Main Keyboard Decoding Routine. Once key   *
*        been decoded, program should return here   *
*                                                   *
*****************************************************

main    jsr     Receive ;Get's a Single Byte from the Keyboard.
        lda     byte

        cmp     #$F0    ;A Key has been Released
        bne     main1
        jmp     release

main1   cmp     #$AA    ;Successful Completion of BAT
        bne     main2
        jmp     rstflag

main2   cmp     #$E0    ;Extended Keys
        bne     main3
        jmp     extend

main3   cmp     #$12    ;Left Shift Key Pressed
        bne     main4
        bset    lshift,status

main4   cmp     #$59    ;Right Shift Key Pressed
        bne     main5
        bset    rshift,status

main5   cmp     #$14    ;Left Ctrl
        bne     main6
        bset    lctrl,status
        jmp     clrasc

main6   cmp     #$11    ;Left Alt
        bne     main7
        bset    lalt,status
        jmp     clrasc

main7   cmp     #$58    ;Caps Lock Pressed
        bne     main8
        brclr   caploc,status,caps
        bset    caploc,status

main8   cmp     #$7E    ;Scroll Lock Pressed
        bne     main9
        brclr   pscrlck,status,scrl
        bset    pscrlck,status

main9   cmp     #$77    ;Num Lock Pressed
        bne     main10
        brclr   pnumlck,status,nums
        bset    pnumlck,status

main10  cmp     #$8F    ;Last Value in Look-Up Table
        blo     main11
        jmp     main    ;Out of Bounds

main11  tax
        brset   caplock,LED,caps_on
        brset   rshift,status,shifton
        brset   lshift,status,shifton

cancel  lda     noshift,x               ;Load Lower Case Values
        jmp     main12

caps_on brset   rshift,status,cancel    ;If ShiftLock & Shift, Cancel
        brset   lshift,status,cancel

shifton lda     shift,x                 ;Load Upper Case Values

main12  beq     return                  ;Scan Code not in Lookup Table.

        tax
        lda     status
        and     #$30                    ;Either Alt Key Pressed
        beq     main13
        txa
        jmp     altdec

main13  lda     status
        and     #$C0                    ;Either CTRL Key Pressed
        beq     main14
        txa
        jmp     ctrlhex

main14  txa
        sta     byte
        jsr     RS232T                  ;Send to RS232

return  jmp     main

*****************************************************
*                                                   *
* caps - Toggle Status of Caps lock and Echo to     *
*        Keyboard                                   *
*                                                   *
*****************************************************

caps    bset    caploc,status   ; Set caploc flag to prevent routine being
                                ; called again
        lda     LED
        eor     #$04            ; Toggle Shift Lock Flag
        sta     LED
        jmp     LEDshow

*****************************************************
*                                                   *
* nums - Toggle Status of Nums lock and Echo to     *
*        Keyboard                                   *
*                                                   *
*****************************************************

nums    bset    pnumlck,status

        lda     LED
        eor     #$02
        sta     LED
        jmp     LEDshow

*****************************************************
*                                                   *
* scrl - Toggle Status of Scroll lock and Echo to   *
*        Keyboard                                   *
*                                                   *
*****************************************************

scrl    bset    pscrlck,status

        lda     LED
        eor     #$01
        sta     LED
        jmp     LEDshow

*****************************************************
*                                                   *
* extend - An Extended Key has been Pressed         *
*                                                   *
*****************************************************

extend  jsr     Receive ;Get Next byte
        lda     byte

        cmp     #$F0    ;An Extended Key Has been Released
        bne     extend1
        jmp     rel_ext

extend1 cmp     #$11    ;Right Alt Pressed
        bne     extend2
        bset    ralt,status
        jmp     clrasc

extend2 cmp     #$14    ;Right Ctrl Pressed
        bne     extend3
        bset    rctrl,status
        jmp     clrasc

extend3 cmp     #$71    ;Delete
        bne     extend4
        lda     status
        and     #$C0    ;Either Alt Key Pressed?
        beq     extend4
        lda     status
        and     #$30    ;Either Ctrl Key Pressed?
        beq     extend4
        bset    nreset,DDRA
        lda     #$FF
        jsr     delay
        bclr    nreset,DDRA
        jmp     reset

extend4 cmp     #$5A    ;Enter Key on Num Keypad
        bne     extend5
        lda     #$0D
        sta     byte
        jsr     RS232T

extend5 cmp     #$4A    ; '/' Key on Num Keypad
        bne     extend6
        lda     #'/'
        sta     byte
        jsr     RS232T

extend6 jmp     main    ;Return to main

*****************************************************
*                                                   *
* release - A Key has been Released                 *
*                                                   *
*****************************************************

release jsr     Receive ;Release - Next Byte Garbage in many cases
        lda     byte

releas3 cmp     #$12    ;Left Shift Key Released
        bne     releas4
        bclr    lshift,status

releas4 cmp     #$59    ;Right Shift Key Released
        bne     releas5
        bclr    rshift,status

releas5 cmp     #$14    ;Left Ctrl Released
        bne     releas6
        bclr    lctrl,status
        jmp     ctrl_re

releas6 cmp     #$11    ;Left Alt Released
        bne     releas7
        bclr    lalt,status
        jmp     alt_rel

releas7 cmp     #$58     ;Caps Lock Released
        bne     releas8
        bclr    caploc,status

releas8 cmp     #$7E     ;Scroll Lock Released
        bne     releas9
        bclr    pscrlck,status

releas9 cmp     #$77     ;Num Lock Released
        bne     relea10
        bclr    pnumlck,status

relea10 jmp     main     ;Return to Main

*****************************************************
*                                                   *
* rel_ext - An Extended Key has been Released       *
*                                                   *
*****************************************************

rel_ext jsr     Receive  ;Get Next byte
        lda     byte

        cmp     #$11    ;Right Alt Released
        bne     rel_ex2
        bclr    ralt,status
        jmp     alt_rel

rel_ex2 cmp     #$14    ;Right Ctrl Released
        bne     rel_ex3
        bclr    rctrl,status
        jmp     ctrl_re

rel_ex3 jmp     main    ;Return to main

*****************************************************
*                                                   *
* LEDshow - Copies the 3 LSB of the LED register to *
*           keyboard for the keyboards Status LED's *
*           E.g. Num Lock, Caps Lock, Scroll Lock   *
*           Also makes their status present on      *
*           PORTB                                   *
*                                                   *
*****************************************************

LEDshow lda     LED
        ora     #$20    ;Set TXD High
        and     #$27
        sta     PORTB   ;Made Status Avalible at PORTB
        lda     #$ED
        sta     byte
        jsr     transmit
        jsr     Receive
        lda     LED
        and     #$07
        sta     byte
        jsr     transmit
        jmp     main

*****************************************************
*                                                   *
* Transmit - Send Data stored at Byte to the        *
*            Keyboard. Result                       *
*                                                   *
*****************************************************

transmit
        ldx     #$08                    ;8 Data Bits
        bset    clk,DDRA                ;Set Clock Low
        lda     #$13                    ;Delay 64uS
        jsr     delay
        clra                            ;Clear Parity Register
        bset    data,DDRA               ;Set Data Low
        bclr    clk,DDRA                ;Release Clock Line
        jsr     highlow

loop    ror     byte
        bcs     mark

space   bset    data,DDRA               ;Clear Bit
        jmp     next

mark    bclr    data,DDRA               ;Clear Bit
        inca                            ;Parity Calculation

next    jsr     highlow                 ;Wait for high to low transition

        decx
        bne     loop

        and     #$01
        bne     clr_par
set_par bclr    data,DDRA
        jmp     tr_ackn
clr_par bset    data,DDRA
tr_ackn jsr     highlow

        bclr    data,DDRA               ;Release Data Line
        jsr     highlow
        brset   data,PORTA,error        ;Check for Ack
        brclr   clk,PORTA,*             ;Wait for idle line

        bset    clk,DDRA                ;Prevent Keyboard from sending data
                                        ;(Clear to Send)
        rts

error   lda     #$FF      ;Reset
        sta     byte
        jsr     transmit
        rts

*****************************************************
*                                                   *
* Receive - Get a Byte from the Keyboard. Result    *
*           stored in byte.                         *
*                                                   *
*****************************************************

Receive ldx     #08                     ;Number of Bits
        clr     PAR                     ;Clear Parity Register
        bclr    clk,DDRA                ;Clear to Send

        brset   clk,PORTA,*             ;wait for idle Clock
        brset   data,PORTA,Receive      ;False Start Bit, Restart

Recdata ror     byte
        jsr     highlow                 ;Wait for high to low Transision
        brset   data,PORTA,Recset

        bclr    7,byte
        jmp     Recnext

Recset  bset    7,byte
        inc     PAR

Recnext decx
        bne     Recdata                 ;Loop until 8 bits been received

        jsr     highlow

        lda     PORTA                   ;Read Parity Bit
        rola                            ;Place in LSbit
        rola
        eor     PAR                     ;Parity Bit Detection
        and     #$01
        beq     r_error

        jsr     highlow
        brclr   data,PORTA,r_error      ;Stop Bit Detection

        bset    clk,DDRA                ;Prevent Keyboard from sending data
                                        ;(Clear to Send)
        rts

r_error lda     #$FE                    ;Resend
        sta     byte
        jsr     Transmit
        jmp     Receive                 ;Try again

*****************************************************
*                                                   *
* highlow - Waits for next High to Low Transistion  *
*           on the Clock Line                       *
*                                                   *
*****************************************************


highlow brclr   clk,PORTA,*         ;Loop until Clk High
        brset   clk,PORTA,*         ;Loop until Clk Low
        rts

*****************************************************
*                                                   *
* ctrlhex & althex - Make sure keys pressed are     *
* valid. If not, don't store them. Also converts    *
* ASCII to binary and stores them in the ASCII      *
* Storage Location                                  *
*                                                   *
*****************************************************

ctrlhex cmp     #'a'            ;Convert 'a' - 'f' to binary
        blo     he1_ran
        cmp     #'f'
        bhi     outrang
        sub     #$57
        jmp     store

he1_ran cmp     #'A'            ;Convert 'A' - 'F' to binary
        blo     altdec
        cmp     #'F'
        bhi     outrang
        sub     #$37
        jmp     store

altdec  cmp     #'0'            ;Convert '0' - '9' to binary
        blo     outrang
        cmp     #'9'
        bhi     outrang
        sub     #$30

store   ldx     asc+1           ;Shift Bytes Left
        stx     asc+0
        ldx     asc+2
        stx     asc+1
        sta     asc+2           ;Store as Binary
outrang jmp     main

*****************************************************
*                                                   *
* alt_rel Alt Released. (Decimal Enter Routine)     *
*         Once both the ALT keys have been released *
*         a calculation must be made to convert the *
*         bytes found in ASCII Storage to binary    *
*         for transmission.                         *
*                                                   *
*****************************************************

alt_rel lda     status          ;Decimal Calculation
        and     #$30
        bne     complet         ;One of the Alt Keys Still Pressed

        lda     asc
        ldx     #$64    ;x 100
        mul
        sta     byte

        lda     asc+1
        ldx     #$0A    ;x 10
        mul
        add     asc+2   ;Add Units
        add     byte    ;Add hundreds
        sta     byte

        jsr     RS232T  ;Transmit number

        jmp     main    ;Return to Main.

*****************************************************
*                                                   *
* ctrl_re Ctrl Released.(Hexadecimal Enter Routine) *
*         Once both the CTRL keys have been released*
*         a calculation must be made to convert the *
*         bytes found in ASCII Storage to binary    *
*         for transmission.                         *
*                                                   *
*****************************************************

ctrl_re lda     status
        and     #$C0
        bne     complet         ;One of the Ctrl Keys Still Pressed

        lda     asc+1
        lsla
        lsla
        lsla
        lsla
        add     asc+2
        sta     byte

        jsr     RS232T          ;Transmit Number

complet jmp     main            ;Return to Main

*****************************************************
*                                                   *
* clrasc - Clear ASCII Storage Locations (3 Bytes)  *
*        - These storage bytes are used for the     *
*          ALTDEC & CTRLHEX Routines.               *
*                                                   *
*****************************************************

clrasc  clr     asc+0
        clr     asc+1
        clr     asc+2
        jmp     main

*****************************************************
*                                                   *
* RS-232 NRZ 8N1 Transmit Routine.                  *
*                                                   *
* Uses a 4.00 Mhz Crystal (2 Mhz Bus Speed)         *
* to obtain a transmission speed of 9600 BPS        *
*                                                   *
*****************************************************

RS232T  ldx     #8        ; Number of Bits (8)
        bclr    TXD,PORTB ; Start Bit (0)
        lda     #$1D      ; 29 Cycles 6[29] + 6
        jsr     delay
        brn     *
        nop
        nop
nextbit ror     byte
        bcs     rsmark
        bclr    TXD,PORTB ; Space (Logic 0)
        jmp     rsnext
rsmark  bset    TXD,PORTB ; Mark (Logic 1)
        brn     *
rsnext  lda     #$1C      ; 28 Cycles 6[28] + 6
        jsr     delay
        nop
        nop
        decx
        bne     nextbit
        brn     *
        brn     *
        nop
        bset    TXD,PORTB ; Stop Bit (Logic 1)
        lda     #$1F      ; 31 Cycles 6[31] + 6
        jsr     delay
        rts

delay   deca              ; Delay = 6[A] + 6
        bne     delay
        rts

*****************************************************
*                                                   *
*   No-Shift - Lookup Table when Shift not Pressed  *
*                                                   *
*****************************************************

noshift fcb      $00     ; 00
        fcb      $00     ; 01  F9
        fcb      $00     ; 02
        fcb      $00     ; 03  F5
        fcb      $00     ; 04  F3
        fcb      $00     ; 05  F1
        fcb      $00     ; 06  F2
        fcb      $00     ; 07  F12
        fcb      $00     ; 08
        fcb      $00     ; 09  F10
        fcb      $00     ; 0A  F8
        fcb      $00     ; 0B  F6
        fcb      $00     ; 0C  F4
        fcb      $09     ; 0D  TAB
        fcb      '`'     ; 0E  ` or ~
        fcb      $00     ; 0F

        fcb      $00     ; 10
        fcb      $00     ; 11  Left ALT
        fcb      $00     ; 12  Left SHIFT
        fcb      $00     ; 13
        fcb      $00     ; 14  Left Ctrl
        fcb      'q'     ; 15  Q
        fcb      '1'     ; 16  1 or !
        fcb      $00     ; 17
        fcb      $00     ; 18
        fcb      $00     ; 19
        fcb      'z'     ; 1A  Z
        fcb      's'     ; 1B  S
        fcb      'a'     ; 1C  A
        fcb      'w'     ; 1D  W
        fcb      '2'     ; 1E  2 or @
        fcb      $00     ; 1F

        fcb      $00     ; 20
        fcb      'c'     ; 21  C
        fcb      'x'     ; 22  X
        fcb      'd'     ; 23  D
        fcb      'e'     ; 24  E
        fcb      '4'     ; 25  4 or $
        fcb      '3'     ; 26  3 or #
        fcb      $00     ; 27
        fcb      $00     ; 28
        fcb      ' '     ; 29  Space
        fcb      'v'     ; 2A  V
        fcb      'f'     ; 2B  F
        fcb      't'     ; 2C  T
        fcb      'r'     ; 2D  R
        fcb      '5'     ; 2E  5 or %
        fcb      $00     ; 2F

        fcb      $00     ; 30
        fcb      'n'     ; 31  N
        fcb      'b'     ; 32  B
        fcb      'h'     ; 33  H
        fcb      'g'     ; 34  G
        fcb      'y'     ; 35  Y
        fcb      '6'     ; 36  6 or ^
        fcb      $00     ; 37
        fcb      $00     ; 38
        fcb      $00     ; 39
        fcb      'm'     ; 3A  M
        fcb      'j'     ; 3B  J
        fcb      'u'     ; 3C  U
        fcb      '7'     ; 3D  7 or &
        fcb      '8'     ; 3E  8 or *
        fcb      $00     ; 3F

        fcb      $00     ; 40
        fcb      ','     ; 41  , or <
        fcb      'k'     ; 42  K
        fcb      'i'     ; 43  I
        fcb      'o'     ; 44  O
        fcb      '0'     ; 45  0 or )
        fcb      '9'     ; 46  9 or (
        fcb      $00     ; 47
        fcb      $00     ; 48
        fcb      '.'     ; 49  . or >
        fcb      '/'     ; 4A  / or ?
        fcb      'l'     ; 4B  L
        fcb      ';'     ; 4C  ; or :
        fcb      'p'     ; 4D  P
        fcb      '-'     ; 4E  - or _
        fcb      $00     ; 4F

        fcb      $00     ; 50
        fcb      $00     ; 51
        fcb      $27     ; 52  ' or "
        fcb      $00     ; 53
        fcb      '['     ; 54  [ or {
        fcb      '='     ; 55  = OR +
        fcb      $00     ; 56
        fcb      $00     ; 57
        fcb      $00     ; 58  Caps Lock
        fcb      $00     ; 59  Right Shift
        fcb      $0D     ; 5A  Enter
        fcb      ']'     ; 5B  ] or }
        fcb      $00     ; 5C
        fcb      '\'     ; 5D  \ or |
        fcb      $00     ; 5E
        fcb      $00     ; 5F

        fcb      $00     ; 60
        fcb      $00     ; 61
        fcb      $00     ; 62
        fcb      $00     ; 63
        fcb      $00     ; 64
        fcb      $00     ; 65
        fcb      $08     ; 66  Backspace
        fcb      $00     ; 67
        fcb      $00     ; 68
        fcb      '1'     ; 69  NUM - 1 or END
        fcb      $00     ; 6A
        fcb      '4'     ; 6B  NUM - 4 or LEFT
        fcb      '7'     ; 6C  NUM - 7 or HOME
        fcb      $00     ; 6D
        fcb      $00     ; 6E
        fcb      $00     ; 6F

        fcb      '0'     ; 70  NUM - 0 or INS
        fcb      '.'     ; 71  NUM - . or DEL
        fcb      '2'     ; 72  NUM - 2 or DOWN
        fcb      '5'     ; 73  NUM - 5
        fcb      '6'     ; 74  NUM - 6 or RIGHT
        fcb      '8'     ; 75  NUM - 8 or UP
        fcb      $1B     ; 76  ESC
        fcb      $00     ; 77  NUM LOCK
        fcb      $00     ; 78  F11
        fcb      '+'     ; 79  NUM - + (Plus)
        fcb      '3'     ; 7A  NUM 3 or PAGE DOWN
        fcb      '-'     ; 7B  NUM - - (Minus)
        fcb      '*'     ; 7C  NUM - *
        fcb      '9'     ; 7D  NUM - 9 or PAGE UP
        fcb      $00     ; 7E  SCROLL LOCK
        fcb      $00     ; 7F

        fcb      $00     ; 80
        fcb      $00     ; 81
        fcb      $00     ; 82
        fcb      $00     ; 83  F7
        fcb      $00     ; 84
        fcb      $00     ; 85
        fcb      $00     ; 86
        fcb      $00     ; 87
        fcb      $00     ; 88
        fcb      $00     ; 89
        fcb      $00     ; 8A
        fcb      $00     ; 8B
        fcb      $00     ; 8C
        fcb      $00     ; 8D
        fcb      $00     ; 8E
        fcb      $00     ; 8F

*****************************************************
*                                                   *
*   Shift - Lookup Table Used when Shift Pressed    *
*                                                   *
*****************************************************

shift   fcb      $00     ; 00
        fcb      $00     ; 01  F9
        fcb      $00     ; 02
        fcb      $00     ; 03  F5
        fcb      $00     ; 04  F3
        fcb      $00     ; 05  F1
        fcb      $00     ; 06  F2
        fcb      $00     ; 07  F12
        fcb      $00     ; 08
        fcb      $00     ; 09  F10
        fcb      $00     ; 0A  F8
        fcb      $00     ; 0B  F6
        fcb      $00     ; 0C  F4
        fcb      $09     ; 0D  TAB
        fcb      '~'     ; 0E  ` or ~
        fcb      $00     ; 0F

        fcb      $00     ; 10
        fcb      $00     ; 11  Left ALT
        fcb      $00     ; 12  Left SHIFT
        fcb      $00     ; 13
        fcb      $00     ; 14  Left Ctrl
        fcb      'Q'     ; 15  Q
        fcb      '!'     ; 16  1 or !
        fcb      $00     ; 17
        fcb      $00     ; 18
        fcb      $00     ; 19
        fcb      'Z'     ; 1A  Z
        fcb      'S'     ; 1B  S
        fcb      'A'     ; 1C  A
        fcb      'W'     ; 1D  W
        fcb      '@'     ; 1E  2 or @
        fcb      $00     ; 1F

        fcb      $00     ; 20
        fcb      'C'     ; 21  C
        fcb      'X'     ; 22  X
        fcb      'D'     ; 23  D
        fcb      'E'     ; 24  E
        fcb      '$'     ; 25  4 or $
        fcb      '#'     ; 26  3 or #
        fcb      $00     ; 27
        fcb      $00     ; 28
        fcb      ' '     ; 29  Space
        fcb      'V'     ; 2A  V
        fcb      'F'     ; 2B  F
        fcb      'T'     ; 2C  T
        fcb      'R'     ; 2D  R
        fcb      '%'     ; 2E  5 or %
        fcb      $00     ; 2F

        fcb      $00     ; 30
        fcb      'N'     ; 31  N
        fcb      'B'     ; 32  B
        fcb      'H'     ; 33  H
        fcb      'G'     ; 34  G
        fcb      'Y'     ; 35  Y
        fcb      '^'     ; 36  6 or ^
        fcb      $00     ; 37
        fcb      $00     ; 38
        fcb      $00     ; 39
        fcb      'M'     ; 3A  M
        fcb      'J'     ; 3B  J
        fcb      'U'     ; 3C  U
        fcb      '&'     ; 3D  7 or &
        fcb      '*'     ; 3E  8 or *
        fcb      $00     ; 3F

        fcb      $00     ; 40
        fcb      '<'     ; 41  , or <
        fcb      'K'     ; 42  K
        fcb      'I'     ; 43  I
        fcb      'O'     ; 44  O
        fcb      ')'     ; 45  0 or )
        fcb      '('     ; 46  9 or (
        fcb      $00     ; 47
        fcb      $00     ; 48
        fcb      '>'     ; 49  > or .
        fcb      '?'     ; 4A  / or ?
        fcb      'L'     ; 4B  L
        fcb      ':'     ; 4C  ; or :
        fcb      'P'     ; 4D  P
        fcb      '_'     ; 4E  - or _
        fcb      $00     ; 4F

        fcb      $00     ; 50
        fcb      $00     ; 51
        fcb      $22     ; 52  ' or "
        fcb      $00     ; 53
        fcb      '{'     ; 54  [ or {
        fcb      '+'     ; 55  = OR +
        fcb      $00     ; 56
        fcb      $00     ; 57
        fcb      $00     ; 58  Caps Lock
        fcb      $00     ; 59  Right Shift
        fcb      $0D     ; 5A  Enter
        fcb      '}'     ; 5B  ] or }
        fcb      $00     ; 5C
        fcb      '|'     ; 5D  \ or |
        fcb      $00     ; 5E
        fcb      $00     ; 5F

        fcb      $00     ; 60
        fcb      $00     ; 61
        fcb      $00     ; 62
        fcb      $00     ; 63
        fcb      $00     ; 64
        fcb      $00     ; 65
        fcb      $08     ; 66  Backspace
        fcb      $00     ; 67
        fcb      $00     ; 68
        fcb      '1'     ; 69  NUM - 1 or END
        fcb      $00     ; 6A
        fcb      '4'     ; 6B  NUM - 4 or LEFT
        fcb      '7'     ; 6C  NUM - 7 or HOME
        fcb      $00     ; 6D
        fcb      $00     ; 6E
        fcb      $00     ; 6F

        fcb      '0'     ; 70  NUM - 0 or INS
        fcb      '.'     ; 71  NUM - . or DEL
        fcb      '2'     ; 72  NUM - 2 or DOWN
        fcb      '5'     ; 73  NUM - 5
        fcb      '6'     ; 74  NUM - 6 or RIGHT
        fcb      '8'     ; 75  NUM - 8 or UP
        fcb      $1B     ; 76  ESC
        fcb      $00     ; 77  NUM LOCK
        fcb      $00     ; 78  F11
        fcb      '+'     ; 79  NUM - + (Plus)
        fcb      '3'     ; 7A  NUM 3 or PAGE DOWN
        fcb      '-'     ; 7B  NUM - - (Minus)
        fcb      '*'     ; 7C  NUM - *
        fcb      '9'     ; 7D  NUM - 9 or PAGE UP
        fcb      $00     ; 7E  SCROLL LOCK
        fcb      $00     ; 7F

        fcb      $00     ; 80
        fcb      $00     ; 81
        fcb      $00     ; 82
        fcb      $00     ; 83  F7
        fcb      $00     ; 84
        fcb      $00     ; 85
        fcb      $00     ; 86
        fcb      $00     ; 87
        fcb      $00     ; 88
        fcb      $00     ; 89
        fcb      $00     ; 8A
        fcb      $00     ; 8B
        fcb      $00     ; 8C
        fcb      $00     ; 8D
        fcb      $00     ; 8E
        fcb      $00     ; 8F

        end



