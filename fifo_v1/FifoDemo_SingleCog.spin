
CON
   
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  LOAD = 10
  CAPACITY = 10
  
OBJ
   
  term  : "Parallax Serial Terminal"
  fifo  : "Fifo"

VAR
  long buffer[CAPACITY]

PUB Main | in, x, out

  term.Start(115_200)
  fifo.Create(@buffer, CAPACITY)

  in := term.CharIn 
  
  term.clear

  term.Str(string("Cleared screen"))
  term.NewLine
  
  repeat 

    term.Str(string("Waiting for keystroke"))
    term.NewLine
    in := term.CharIn
     
    repeat x from 1 to LOAD
      term.Str(string("Putting value to FIFO"))
      term.Newline
      fifo.Put(x)
      term.Str(string("Done"))
      term.Newline
     
    repeat x from 1 to LOAD
      term.Str(string("Taking value from FIFO"))
      term.Newline
      out := fifo.Take 
      term.Dec(out)
      term.NewLine

    term.NewLine
     
  fifo.Destroy