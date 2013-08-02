OBJ
   
  _inputFifo   : "Fifo"
  _outputFifo  : "Fifo"
   
VAR
  byte cog
  long stack[50]
  
pub Start(inputBase, outputBase, seed, sendSeed) : success
  stop
  _inputFifo.SetBaseAddress(inputBase)
  _outputFifo.SetBaseAddress(outputBase)
  success := (cog := cognew(Run(seed, sendSeed), @stack) + 1)

pub Run(seed, sendSeed) | inputValue
  if sendSeed
    _outputFifo.Put(seed)
  repeat
    inputValue := _inputFifo.Take
    _outputFifo.Put(inputValue) 

pub Stop
  if cog