OBJ
   
  _input_1   : "FifoQueue"
  _input_2   : "FifoQueue"
  _output    : "FifoQueue"
   
VAR
  byte cog
  long stack[50]
  byte running 
  
pub Start(input_1_base, input_2_base, output_base) : success
  stop
  _input_1.Base(input_1_base)
  _input_2.Base(input_2_base)
  _output.Base(output_base)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run | ok, value
  repeat
    ok := false

    if _input_1.Pop(@value)
      _output.Push(value)
      ok := true
      
    if _input_2.Pop(@value)
      _output.Push(value)
      ok := true

  while ok
  Shutdown

pri Shutdown
  _output.EndFlow
  running := false 
  Stop

pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running