
CON
   
  _clkmode      = xtal1 + pll16x
  _xinfreq      = 5_000_000

  LOAD          = 50
  FIFO_DEPTH    = 9
  FACTOR        = 5
  OUTPUT_MAX    = 100
  
  
OBJ
   
  term  : "Parallax Serial Terminal"

  fifo[9]       : "LongFifo"

  filtered_fork : "FilterThenForkActor"
  times_2       : "ConstantMultiplierActor"
  times_3       : "ConstantMultiplierActor"
  times_5       : "ConstantMultiplierActor"
  merge_2_3     : "OrderedMergeActor"
  merge_2_3_5   : "OrderedMergeActor"
  

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
  
  fifo[0].Initialize(@fifo_0_struct, @fifo_0_buffer, FIFO_DEPTH, 0)
  fifo[1].Initialize(@fifo_1_struct, @fifo_1_buffer, FIFO_DEPTH, 0)
  fifo[2].Initialize(@fifo_2_struct, @fifo_2_buffer, FIFO_DEPTH, 0)
  fifo[3].Initialize(@fifo_3_struct, @fifo_3_buffer, FIFO_DEPTH, 0)
  fifo[4].Initialize(@fifo_4_struct, @fifo_4_buffer, FIFO_DEPTH, 0)
  fifo[5].Initialize(@fifo_5_struct, @fifo_5_buffer, FIFO_DEPTH, 0)
  fifo[6].Initialize(@fifo_6_struct, @fifo_6_buffer, FIFO_DEPTH, 0)
  fifo[7].Initialize(@fifo_7_struct, @fifo_7_buffer, FIFO_DEPTH, 0)
  fifo[8].Initialize(@fifo_8_struct, @fifo_8_buffer, FIFO_DEPTH, 0)
        
  filtered_fork.Start(@fifo_0_struct, @fifo_1_struct, @fifo_2_struct, @fifo_3_struct, @fifo_4_struct, OUTPUT_MAX, 1)
  times_2.Start(@fifo_1_struct, @fifo_5_struct, 2)
  times_3.Start(@fifo_2_struct, @fifo_6_struct, 3)
  times_5.Start(@fifo_3_struct, @fifo_7_struct, 5)
  merge_2_3.Start(@fifo_5_struct, @fifo_6_struct, @fifo_8_struct)
  merge_2_3_5.Start(@fifo_7_struct, @fifo_8_struct, @fifo_0_struct)

  fifo[0].Put(1)

  repeat
  
    if fifo[4].Take
      term.Dec(fifo[4].LastTaken)
      term.NewLine
    else
      quit

  term.Str(string("done"))
  term.NewLine
  
  repeat while filtered_fork.IsRunning
  repeat while times_2.IsRunning
  repeat while times_3.IsRunning
  repeat while times_5.IsRunning
  repeat while merge_2_3.IsRunning
  repeat while merge_2_3_5.IsRunning
  
  repeat i from 0 to 8
    term.Dec(i)
    term.Str(string(" "))
    term.Dec(fifo[i].FlowEnded)
    term.NewLine  
