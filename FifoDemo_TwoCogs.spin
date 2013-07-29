
CON
   
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  LOAD = 100
  CAPACITY = 3
  
OBJ
   
  term  : "Parallax Serial Terminal"
  fifo  : "Fifo"

VAR
  long buffer[CAPACITY]
  long stack[400]

PUB Main | in, x, out

  term.Start(115_200)
  fifo.Create(@buffer, CAPACITY)

  in := term.CharIn 
  
  term.clear

  term.Str(string("Cleared screen"))
  term.NewLine

  term.Str(string("Waiting for keystroke"))
  term.NewLine
  
  in := term.CharIn

  cognew(sender, @stack[0])
  cognew(receiver, @stack[200])

  waitcnt(cnt + clkfreq)    
   
  fifo.Destroy


pub sender | x
  repeat x from 1 to LOAD
    fifo.Put(x)
    waitcnt(cnt + clkfreq)


pub receiver | x, out
  repeat x from 1 to LOAD
    term.Str(string("Taking value from FIFO"))
    term.Newline
    out := fifo.Take 
    term.Dec(out)
    term.NewLine