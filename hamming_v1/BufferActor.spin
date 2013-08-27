OBJ
   
  _input_fifo   : "LongFifo"
  _output_fifo  : "LongFifo"
   
VAR
  long stack[50]
  byte cog
  byte running
  
pub Start(input_fifo_base, output_fifo_base) : success
  'stop
  _input_fifo.SetBaseAddress(input_fifo_base)
  _output_fifo.SetBaseAddress(output_fifo_base)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run | inputValue
  repeat
                                                 
    if _input_fifo.Take
      _output_fifo.Put(_input_fifo.LastTaken)
      
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