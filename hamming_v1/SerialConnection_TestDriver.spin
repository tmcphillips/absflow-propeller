CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  term : "SerialConnection"

VAR
  long buffer[200]
   
PUB Main | c, x, y
 
  term.Start(115_200)

  dira[23..16] := 255
  debugChar(255)
      
  repeat

    c := term.CharIn
    debugChar(c)
  
    case c
                        
      "A":
        c := term.CharIn
        debugChar(c)
        term.Char(c)

      "S":
        term.ReadString(@buffer)
        term.WriteString(@buffer)

      "L":
        term.ReadLong(@x)
        debugLong(x)
        term.WriteLong(x)

      "T":
        term.ReadLong(@x)
        debugLong(x)
        term.ReadLong(@y)
        debugLong(y)
        term.WriteLong(x)
        term.WriteLong(y)


PRI debugChar(c)
 dira[23..16] := 255
 outa[23..16] := c

PRI debugLong(l)
  dira[23..16] := 255
  outa[23..16] := l
     