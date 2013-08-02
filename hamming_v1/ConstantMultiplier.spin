OBJ
   
  _inputFifo   : "Fifo"
  _outputFifo  : "Fifo"
   
VAR
  byte cog
  long stack[50]
  
pub Start(inputBase, outputBase, multiplier) : success
  stop
  _inputFifo.SetBaseAddress(inputBase)
  _outputFifo.SetBaseAddress(outputBase)
  success := (cog := cognew(Run(multiplier), @stack) + 1)

pub Run(multiplier) | inputValue, outputValue
  repeat
    inputValue := _inputfifo.Take
    outputValue := inputValue * multiplier
    _outputFifo.Put(outputValue) 

pub Stop
  if cog
    cogstop(cog~ - 1)