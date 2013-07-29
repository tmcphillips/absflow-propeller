'' File: PusbuttonLedTest.spin
'' Test program for propeller quick start

CON

  _clkmode      = xtal1 + pll16x                        ' Feedback and PLL multiplier
  _xinfreq      = 5_000_000                             ' External oscillator = 5 MHz

  LEDs_START    = 16                                    ' Start of I/O pin group for on/off signals
  LEDs_END      = 23                                    ' End of I/O pin group for on/off signals
  PUSHBUTTON    = 0                                    ' Pushbutton input pin

PUB ButtonBlinkSpeed | pressed

  '' Sends on/off (3.3 V / 0 V) signals at approximately 2 Hz.

  dira[LEDs_START..LEDs_END]~~                          ' Set entire pin group to output

  repeat
    ! outa[LEDs_START..LEDs_END]                        ' Endless loop
    
    if ResistiveTouchButtonIsPressed(PUSHBUTTON)        ' If pushbutton pressed
      waitcnt(clkfreq / 4 + cnt)                        ' Wait 1/4 second -> 2 Hz
    else                                                ' If pushbutton not pressed
      waitcnt(clkfreq / 20 + cnt)                       ' Wait 1/20 second -> 10 Hz

PUB ResistiveTouchButtonIsPressed(pin)
    dira[pin] := outa[pin] := 1
    dira[pin] := 0 
    waitcnt(clkfreq / 1000 + cnt)
    return NOT ina[pin]