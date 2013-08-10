
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  LOAD          = 50
  FIFO_DEPTH    = 9
  FACTOR        = 5
  OUTPUT_MAX    = 100
  
  
OBJ
   
  term  : "Parallax Serial Terminal"

  fifo[9]       : "Fifo"

  merge         : "Merge"
  times_2       : "ConstantMultiplier"
  times_3       : "ConstantMultiplier"
  times_5       : "ConstantMultiplier"
  filter        : "LowpassFilter"
  fork          : "Fork"
  

VAR

  long fifo_0_struct[fifo#STRUCT_SIZE]
  long fifo_0_buffer[FIFO_DEPTH]

  long fifo_1_struct[fifo#STRUCT_SIZE]
  long fifo_1_buffer[FIFO_DEPTH]

  long fifo_2_struct[fifo#STRUCT_SIZE]
  long fifo_2_buffer[FIFO_DEPTH]

  long fifo_3_struct[fifo#STRUCT_SIZE]
  long fifo_3_buffer[FIFO_DEPTH]

  long fifo_4_struct[fifo#STRUCT_SIZE]
  long fifo_4_buffer[FIFO_DEPTH]
   
  long fifo_5_struct[fifo#STRUCT_SIZE]
  long fifo_5_buffer[FIFO_DEPTH]
  
  long fifo_6_struct[fifo#STRUCT_SIZE]
  long fifo_6_buffer[FIFO_DEPTH]

  long fifo_7_struct[fifo#STRUCT_SIZE]
  long fifo_7_buffer[FIFO_DEPTH]

  long fifo_8_struct[fifo#STRUCT_SIZE]
  long fifo_8_buffer[FIFO_DEPTH]

PUB Main | in, x, out, i

  term.Start(115_200)
  in := term.CharIn 
  term.clear
  
  fifo[0].Initialize(@fifo_0_struct, @fifo_0_buffer, FIFO_DEPTH)
  fifo[1].Initialize(@fifo_1_struct, @fifo_1_buffer, FIFO_DEPTH)
  fifo[2].Initialize(@fifo_2_struct, @fifo_2_buffer, FIFO_DEPTH)
  fifo[3].Initialize(@fifo_3_struct, @fifo_3_buffer, FIFO_DEPTH)
  fifo[4].Initialize(@fifo_4_struct, @fifo_4_buffer, FIFO_DEPTH)
  fifo[5].Initialize(@fifo_5_struct, @fifo_5_buffer, FIFO_DEPTH)
  fifo[6].Initialize(@fifo_6_struct, @fifo_6_buffer, FIFO_DEPTH)
  fifo[7].Initialize(@fifo_7_struct, @fifo_7_buffer, FIFO_DEPTH)
  fifo[8].Initialize(@fifo_8_struct, @fifo_8_buffer, FIFO_DEPTH)
        
  fork.Start(@fifo_0_struct, @fifo_1_struct, @fifo_2_struct, @fifo_3_struct, @fifo_4_struct)
  times_2.Start(@fifo_1_struct, @fifo_5_struct, 2)
  times_3.Start(@fifo_2_struct, @fifo_6_struct, 3)
  times_5.Start(@fifo_3_struct, @fifo_7_struct, 5)
  merge.Start(@fifo_5_struct, @fifo_6_struct, @fifo_7_struct, @fifo_8_struct)
  filter.Start(@fifo_8_struct, @fifo_0_struct, OUTPUT_MAX)

  fifo[0].Put(1)

  repeat
  
    if fifo[4].Take
      term.Dec(fifo[4].LastTaken)
      term.NewLine
    else
      quit

  println(string("done"))
  
  waitcnt(cnt + clkfreq)
  
  repeat i from 0 to 8
    term.Dec(i)
    term.Str(string(" "))
    term.Dec(fifo[i].FlowEnded)
    term.NewLine  

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