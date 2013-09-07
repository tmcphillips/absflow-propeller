OBJ
   
  input_fifo   : "LongFifo"
  output_fifo  : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running  
  
pub Start(input_base, output_base, multiplier) : success
  stop
  input_fifo.Base(input_base)
  output_fifo.Base(output_base)
  running := true
  success := (cog := cognew(Run(multiplier), @stack) + 1)

pub Run(multiplier) | input_value
  repeat

    if input_fifo.Pop(@input_value) 
      output_fifo.Push(input_value * multiplier)   

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