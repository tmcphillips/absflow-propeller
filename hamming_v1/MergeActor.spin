OBJ
   
  _input_1   : "LongFifo"
  _input_2   : "LongFifo"
  _input_3   : "LongFifo"
  _output    : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running 
  
pub Start(input_1_base, input_2_base, input_3_base, output_base) : success
  stop
  _input_1.SetBaseAddress(input_1_base)
  _input_2.SetBaseAddress(input_2_base)
  _input_3.SetBaseAddress(input_3_base)
  _output.SetBaseAddress(output_base)
  success := (cog := cognew(Run, @stack) + 1)

pub Run | ok
  running := true
  repeat
    ok := false

    if _input_1.Take
      _output.Put(_input_1.LastTaken)
      ok := true
      
    if _input_2.Take
      _output.Put(_input_2.LastTaken)
      ok := true

    if _input_3.Take
      _output.Put(_input_3.LastTaken)
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