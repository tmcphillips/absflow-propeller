OBJ
   
  _input_fifo   : "LongFifo"
  _output_fifo  : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running  
  
pub Start(input_fifo_base, output_fifo_base, multiplier) : success
  stop
  _input_fifo.SetBaseAddress(input_fifo_base)
  _output_fifo.SetBaseAddress(output_fifo_base)
  success := (cog := cognew(Run(multiplier), @stack) + 1)

pub Run(multiplier) | product
  running := true
  repeat

    if _input_fifo.Take 
      product := _input_fifo.LastTaken * multiplier
      _output_fifo.Put(product)   

    else                  
      Shutdown
      
pri Shutdown
  _output_fifo.EndFlow
  running := false
  Stop

pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running