
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

OBJ
   
  term   : "SerialConnection"
  hamming: "HammingDataflow"

PUB Main | max_output, in, x, out, i, value

  term.Start(115_200)
  in := term.CharIn 
  term.clear

  repeat

    term.Str(string("Enter maximum hamming number:"))
    term.NewLine
    max_output := term.DecIn
    
    term.Str(string("Starting hamming dataflow..."))
    term.NewLine
    hamming.Initialize(100)
    hamming.Begin(max_output)
     
    repeat while hamming.ReadOutput(@value)
      term.Dec(value)
      term.NewLine
     
    term.Str(string("...dataflow complete."))
    term.NewLine
     
    hamming.Wait
     
    term.Str(string("All actors have stopped."))
    term.NewLine
     