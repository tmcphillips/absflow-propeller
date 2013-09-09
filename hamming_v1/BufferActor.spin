OBJ
   
  _input_fifo   : "FifoQueue"
  _output_fifo  : "FifoQueue"
   
VAR
  long stack[50]
  byte cog
  byte running
  
pub Start(input_fifo_base, output_fifo_base) : success
  'stop
  _input_fifo.Base(input_fifo_base)
  _output_fifo.Base(output_fifo_base)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run | value
  repeat
                                                 
    if _input_fifo.Pop(@value)
      _output_fifo.Push(value)
      
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