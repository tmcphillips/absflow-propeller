OBJ
   
  input_fifo   : "LongFifo"
  output_fifo  : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running  
  
pub Start(input_base, output_base, multiplier) : success
  stop
  input_fifo.SetBaseAddress(input_base)
  output_fifo.SetBaseAddress(output_base)
  running := true
  success := (cog := cognew(Run(multiplier), @stack) + 1)

pub Run(multiplier) | product
  repeat

    if input_fifo.Pop 
      output_fifo.Push(input_fifo.LastPopped * multiplier)   

    else                  
      Shutdown
      
pri Shutdown
  output_fifo.EndFlow
  running := false
  Stop

pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running