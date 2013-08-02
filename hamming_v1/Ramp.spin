OBJ
   
  _outputFifo  : "Fifo"
 
VAR
  byte cog
  long stack[50]
  long _first
  long _last


pub Start(fifoBase, first, last) : success
  stop
  _outputFifo.SetBaseAddress(fifoBase)
  _first := first
  _last := last
  success := (cog := cognew(Run, @stack) + 1)

pub Run | x
  repeat x from _first to _last
    _outputFifo.Put(x)               

pub Stop
  if cog
    cogstop(cog~ - 1)
    