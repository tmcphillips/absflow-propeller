OBJ
   
  _input_fifo   : "LongFifo"
  _output_fifo  : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running  
  
pub Start(input_fifo_base, output_fifo_base, cutoff, shutdownOnOverage) : success
  stop
  _input_fifo.SetBaseAddress(input_fifo_base)
  _output_fifo.SetBaseAddress(output_fifo_base)
  running := true
  success := (cog := cognew(Run(cutoff, shutdownOnOverage), @stack) + 1)

pri Run(cutoff, shutdownOnOverage) | input_value
  repeat
    
    if _input_fifo.Take
      input_value := _input_fifo.LastTaken

      if input_value =< cutoff
        _output_fifo.Put(input_value)

      else
        if shutdownOnOverage
          Shutdown 

    else    
      Shutdown
      
pri Shutdown
  _output_fifo.EndFlow
  repeat while _input_fifo.Take
  running := false    
  Stop
            
pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running