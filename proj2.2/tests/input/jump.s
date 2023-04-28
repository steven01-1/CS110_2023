jal ra label  # 00C000EF
addi s0 x0 -1  # FFF00413
jal x0 end  # 0080006F
label: jalr x0 ra 0  # 00008067
end: addi a0 x0 -1  # FFF00513

#0,3,1,2,4

00C000EF
FFF00413
0080006F
00008067
FFF00513
