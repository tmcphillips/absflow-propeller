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

pub Run(multiplier) | product
  repeat

    if _inputFifo.Take 
      product := _inputFifo.LastTaken * multiplier
      _outputFifo.Put(product)   

    else                  
      _outputFifo.EndFlow 
      Stop

pub Stop
  if cog
    cogstop(cog~ - 1)