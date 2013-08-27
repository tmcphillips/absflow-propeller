OBJ
   
  _input_1   : "LongFifo"
  _input_2   : "LongFifo"
  _output    : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running 
  byte first
  long last
  long a
  long b
  long c
  byte have_a
  byte have_b 
  
pub Start(input_1_base, input_2_base, output_base) : success
  stop
  _input_1.SetBaseAddress(input_1_base)
  _input_2.SetBaseAddress(input_2_base)
  _output.SetBaseAddress(output_base)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run  

  first := true
  have_a := false
  have_b := false
      
  repeat
      
    ifnot have_a
      if _input_1.Take
        a := _input_1.LastTaken
        have_a := true

    ifnot have_b
      if _input_2.Take
        b := _input_2.LastTaken
        have_b := true

    ifnot have_a OR have_b
      quit

    ifnot have_a
      c := b
      have_b := false

    elseifnot have_b
      c := a
      have_a := false

    elseif a < b
      c := a
      have_a := false

    elseif b < a
      c := b
      have_b := false

    elseif a == b
      c := a
      have_a := false
      have_b := false

    if c <> last OR first
      first := false
      _output.Put(c) 
      last := c
      
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