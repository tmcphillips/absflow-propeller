OBJ
   
  fifo  : "Fifo"
 
VAR
  byte cog
  long stack[400]
  long count


pub Start(fifoStruct, c) : success

  fifo.Initialize(fifoStruct)
  success := (cog := cognew(SendData, @stack) + 1)
  count := c
  
pub SendData | x
  repeat x from 1 to count
    fifo.Put(x)               
    waitcnt(cnt + clkfreq/5)