OBJ
   
  _input_1   : "Fifo"
  _input_2   : "Fifo"
  _input_3   : "Fifo"
  _output   : "Fifo"
   
VAR
  byte cog
  long stack[50]
  
pub Start(input_1_base, input_2_base, input_3_base, output_base) : success
  stop
  _input_1.SetBaseAddress(input_1_base)
  _input_2.SetBaseAddress(input_2_base)
  _input_3.SetBaseAddress(input_3_base)
  _output.SetBaseAddress(output_base)
  success := (cog := cognew(Run, @stack) + 1)

pub Run | ok

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

  _output.EndFlow
  Stop

pub Stop
  if cog
    cogstop(cog~ - 1) 