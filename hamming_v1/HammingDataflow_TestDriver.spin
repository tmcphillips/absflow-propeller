
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000
    
OBJ
   
  term    : "SerialConnection"
  hamming : "HammingDataflow"


PUB Main | i, f, fifo_depths, max_output_value, value

  term.Start(115_200)

  repeat 

    case term.CharIn
    
      "I":  'Initialize input and output fifos with requested size
        term.ReadLong(@fifo_depths)
        hamming.Initialize(fifo_depths) 
    
      "S" :   
        term.ReadLong(@max_output_value)
        hamming.Begin(max_output_value)
                       
      "R" :
        term.WriteLong(hamming.ReadOutput(@value))
        term.WriteLong(value)
       
      "W":
        hamming.Wait
         
      "Q":  'Query fifo data structure f
        term.ReadLong(@f)
        term.ReadLong(@i)
        term.WriteLong(hamming.Peek(f,i)) 
       