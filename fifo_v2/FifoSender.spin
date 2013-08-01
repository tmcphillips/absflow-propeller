OBJ
   
  fifo  : "Fifo"
 
VAR
  byte cog
  long stack[50]
  long count


pub Start(fifoBase, c) : success
  stop
  fifo.SetBaseAddress(fifoBase)
  count := c
  success := (cog := cognew(SendData, @stack) + 1)

pub SendData | x
  repeat x from 1 to count
    fifo.Put(x)               
    'waitcnt(cnt + clkfreq/(count + 1))

pub Stop
  if cog
    cogstop(cog~ - 1)
    