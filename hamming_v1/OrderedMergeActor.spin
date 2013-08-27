OBJ
   
  fifo_a : "LongFifo"
  fifo_b : "LongFifo"
  fifo_c : "LongFifo"
   
VAR
  byte cog
  long stack[50]
  byte running 
  
pub Start(base_a, base_b, base_c) : success
  stop
  fifo_a.SetBaseAddress(base_a)
  fifo_b.SetBaseAddress(base_b)
  fifo_c.SetBaseAddress(base_c)
  running := true
  success := (cog := cognew(Run, @stack) + 1)

pub Run | a, b, c, have_a, have_b, is_first, last_c  

  have_a   := false
  have_b   := false
  is_first := true
      
  repeat
      
    if (NOT have_a)
      if fifo_a.Pop
        a := fifo_a.LastPopped
        have_a := true
      
    if (NOT have_b)
      if fifo_b.Pop             
        b := fifo_b.LastPopped
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

    if c <> last_c OR is_first
      is_first := false
      fifo_c.Push(c) 
      last_c := c
      
  Shutdown

pri Shutdown
  fifo_c.EndFlow
  running := false 
  Stop

pub Stop
  if cog
    cogstop(cog~ - 1)

pub IsRunning
  return running