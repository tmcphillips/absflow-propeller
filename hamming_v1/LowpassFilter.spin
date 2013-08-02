OBJ
   
  _inputFifo   : "Fifo"
  _outputFifo  : "Fifo"
   
VAR
  byte cog
  long stack[50]
  
pub Start(inputBase, outputBase, cutoff) : success
  stop
  _inputFifo.SetBaseAddress(inputBase)
  _outputFifo.SetBaseAddress(outputBase)
  success := (cog := cognew(Run(cutoff), @stack) + 1)

pub Run(cutoff) | inputValue
  repeat
    inputValue := _inputFifo.Take
    if inputValue < cutoff
      _outputFifo.Put(inputValue) 

pub Stop
  if cog
    cogstop(cog~ - 1)