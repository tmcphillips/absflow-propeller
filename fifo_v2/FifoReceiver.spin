OBJ
   
  fifo  : "Fifo"
 
VAR
  byte cog
  long stack[400]
  long data
  long count
  
pub Start(fifoStruct, d, c) : success

  fifo.Initialize(fifoStruct)
  success := (cog := cognew(ReceiveData, @stack) + 1)
  data := d
  count := c
  

pub ReceiveData | x, out
  repeat x from 0 to count - 1
    long[data][x] := fifo.Take
