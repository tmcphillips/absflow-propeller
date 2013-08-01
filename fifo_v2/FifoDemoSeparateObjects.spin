
CON
   
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  LOAD  = 100
  DEPTH = 9
  
OBJ
   
  term  : "Parallax Serial Terminal"
  fifo  : "Fifo"
  sndr  : "FifoSender"
  rcvr  : "FifoReceiver"

VAR

  long fifoStruct[fifo#STRUCT_SIZE]
  long fifoBuffer[DEPTH]
  long rcvBuffer[LOAD + 10]

PUB Main | in, x, out

  term.Start(115_200)
  in := term.CharIn 
  term.clear

  LONGFILL (@rcvBuffer, -1, LOAD)
  
  fifo.Initialize(@fifoStruct, @fifoBuffer, DEPTH)
  banner(string("Initialized fifo"))
  dump(@fifoStruct)
  
  rcvr.Start(@fifoStruct, @rcvBuffer, LOAD)
  sndr.Start(@fifoStruct, LOAD)

  waitcnt(cnt + clkfreq)

  rcvr.Stop
  sndr.Stop
  
  banner(string("Reading receive buffer"))

  repeat x from 0 to LOAD - 1
    term.Dec(long[@rcvBuffer][x])
    term.NewLine
 
  banner(string("Printing final fifo state"))
  dump(@fifoStruct)   
  fifo.Destroy



PUB println(s)
  term.Str(s)
  term.NewLine

PUB banner(s)
  term.Str(string("----------------------------",13))
  println(s)
  term.Str(string("----------------------------",13))


  
pub dump(f)
  term.Str(string("SEMID        = "))
  term.Dec(long[f][fifo#SEM_ID_OFFSET])
  term.NewLine

  term.Str(string("DEPTH        = "))
  term.Dec(long[f][fifo#DEPTH_OFFSET])
  term.NewLine

  term.Str(string("OCCUPANCY    = "))
  term.Dec(long[f][fifo#OCCUPANCY_OFFSET])
  term.NewLine

  term.Str(string("NEXT_WRITE   = "))
  term.Dec(long[f][fifo#NEXT_WRITE_OFFSET])
  term.NewLine

  term.Str(string("NEXT_READ    = "))
  term.Dec(long[f][fifo#NEXT_READ_OFFSET])
  term.NewLine

  term.Str(string("BUFFER_START = "))
  term.Dec(long[f][fifo#BUFFER_START_OFFSET])
  term.NewLine

  term.Str(string("BUFFER_END   = "))
  term.Dec(long[f][fifo#BUFFER_END_OFFSET])
  term.NewLine