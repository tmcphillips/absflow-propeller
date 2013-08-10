CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  pst : "Parallax Serial Terminal"

VAR
  long buffer[200]
   
PUB EchoTerminal
 
  pst.Start(115_200)
  
  repeat
     pst.StrIn(@buffer)
     pst.Str(@buffer)
     pst.Char(13)
