add s0 x0 x0  # 00000433
addi a0 x0 -1 # FFF00513
bne s0 s0 never_reach # 00841A63
addi s0 s0 -1 # FFF40413
lui s1 0 #end  # 000004B7
addi s1 s1 36 #end # 02448493
jr s1 # 00048067
never_reach: 
  addi s0, s0, 1 # 00140413
  j end  # 0040006F
end: 
  addi a0 a0 1 # 00150513

00000433
FFF00513
00841A63
FFF40413
000004B7
02448493
00048067
00140413
0040006F
00150513
