OBJ
   
  _inputFifo  : "Fifo"
 
VAR
  byte cog
  long stack[50]
  long buffer
  
pub Start(fifoBase, bufferAddress) : success
  stop
  _inputFifo.SetBaseAddress(fifoBase)
  buffer := bufferAddress
  success := (cog := cognew(Run, @stack) + 1)

pub Run | i
  i := 0
  repeat 
    long[buffer][i++] := _inputFifo.Take 

pub Stop
  if cog
    cogstop(cog~ - 1)