OBJ
   
  _input_fifo     : "LongFifo"
  _output_1_fifo  : "LongFifo"
  _output_2_fifo  : "LongFifo"
  _output_3_fifo  : "LongFifo"
  _output_4_fifo  : "LongFifo"
       
VAR
  byte cog
  long stack[50]
  byte running
  
pub Start(input_base, output_1_base, output_2_base, output_3_base, output_4_base) : success
  stop
  _input_fifo.Base(input_base)
  _output_1_fifo.Base(output_1_base)
  _output_2_fifo.Base(output_2_base)
  _output_3_fifo.Base(output_3_base)
  _output_4_fifo.Base(output_4_base)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run | value
  repeat

    if _input_fifo.Pop(@value)            
      _output_1_fifo.Push(value) 
      _output_2_fifo.Push(value) 
      _output_3_fifo.Push(value)
      _output_4_fifo.Push(value)

    else
      Shutdown

pri Shutdown
  _output_1_fifo.EndFlow
  _output_2_fifo.EndFlow 
  _output_3_fifo.EndFlow
  _output_4_fifo.EndFlow
  running := false
  Stop
          
pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running