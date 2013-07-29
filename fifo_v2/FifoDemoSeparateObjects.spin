
CON
   
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  LOAD = 10
  CAPACITY = 5
  
OBJ
   
  term  : "Parallax Serial Terminal"
  fifo  : "Fifo"
  sndr  : "FifoSender"
  rcvr  : "FifoReceiver"

VAR

  long fifoStruct[fifo#STRUCT_SIZE]
  long fifoBuffer[CAPACITY]
  long rcvBuffer[LOAD]

PUB Main | in, x, out

  term.Start(115_200)
  in := term.CharIn 
  term.clear
  term.Str(string("Waiting for keystroke", 13))

  fifo.Construct(@fifoStruct, @fifoBuffer, CAPACITY)
  dump(@fifoStruct)

  term.Str(string("Starting sender", 13))
  sndr.Start(@fifoStruct, LOAD)
    
  term.Str(string("Starting receiver", 13))
  rcvr.Start(@fifoStruct, @rcvBuffer, LOAD)

  term.Str(string("Waiting 2 seconds", 13))
  waitcnt(cnt + clkfreq * 2)

  term.Str(string("Dumping received data", 13))
  repeat x from 0 to 9
    term.Dec(long[@rcvBuffer][x])
    term.NewLine
 
  dump(@fifoStruct)   
  fifo.Destroy

pub dump(f)
  term.Str(string("SEMID    = "))
  term.Dec(long[f][0])
  term.NewLine

  term.Str(string("CAPACITY = "))
  term.Dec(long[f][1])
  term.NewLine

  term.Str(string("SIZE     = "))
  term.Dec(long[f][2])
  term.NewLine

  term.Str(string("HEAD     = "))
  term.Dec(long[f][3])
  term.NewLine

  term.Str(string("TAIL     = "))
  term.Dec(long[f][4])
  term.NewLine

