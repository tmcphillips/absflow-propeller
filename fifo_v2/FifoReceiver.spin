OBJ
   
  fifo  : "Fifo"
 
VAR
  byte cog
  long stack[50]
  long buffer
  
pub Start(fifoBase, bufferAddress, count) : success
  stop
  fifo.SetBaseAddress(fifoBase)
  buffer := bufferAddress
  success := (cog := cognew(ReceiveData(count), @stack) + 1)

pub ReceiveData(count) | i
  repeat i from 0 to count - 1 
    long[buffer][i] := fifo.Take
    waitcnt(cnt + clkfreq/(count + 1)) 

pub Stop
  if cog
    cogstop(cog~ - 1)