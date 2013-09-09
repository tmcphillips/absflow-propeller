OBJ
   
  _input_fifo   : "FifoQueue"
  _output_fifo  : "FifoQueue"
   
VAR
  byte cog
  long stack[50]
  byte running  
  
pub Start(input_fifo_base, output_fifo_base, cutoff, shutdownOnOverage) : success
  stop
  _input_fifo.Base(input_fifo_base)
  _output_fifo.Base(output_fifo_base)
  running := true
  success := (cog := cognew(Run(cutoff, shutdownOnOverage), @stack) + 1)

pri Run(cutoff, shutdownOnOverage) | value
  repeat
    
    if _input_fifo.Pop(@value)

      if value =< cutoff
        _output_fifo.Push(value)

      else
        if shutdownOnOverage
          Shutdown 

    else    
      Shutdown
      
pri Shutdown | discard
  _output_fifo.EndFlow
  repeat while _input_fifo.Pop(@discard)
  running := false    
  Stop
            
pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running