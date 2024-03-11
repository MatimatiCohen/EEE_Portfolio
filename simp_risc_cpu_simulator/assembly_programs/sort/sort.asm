.word 0x100 -1
.word 0x101 3
.word 0x102 40
.word 0x103 -4
.word 0x104 6
.word 0x105 52
.word 0x106 5
.word 0x107 11
.word 0x108 -10
.word 0x109 17
.word 0x10A 0xff
.word 0x10B 10
.word 0x10C 0
.word 0x10D 7
.word 0x10E -200
.word 0x10F 111


	add $a0, $zero, $imm, 0xf				#a0 = N-1 = f  when N is number of elements
	add $s0, $zero, $imm, 0					# i=0				
	beq $imm, $zero, $zero, I_CONDITION 	#for i in 0 to N-1

I_LOOP: 	
								
	add $s1, $zero,$imm, 0		# j=0    
	beq $imm, $zero, $zero, J_CONDITION		#for j in 0 to N-1-i
	

J_LOOP:						

	lw $t0, $s1, $imm, 0x100    	# t0 = ARR[j]
	lw $t1, $s1, $imm, 0x101   	 	# t1 = ARR[j+1]
	bgt $imm, $t0, $t1, SWAP		#if (ARR[j] > ARR[j+1]) -> SWAP

BACK:

	 add $s1, $s1, $imm, 1 			# j=j+1
	 

J_CONDITION: 

	sub $t2, $a0, $s0				#t2 = j = N-1-i
	blt $imm, $s1, $t2, J_LOOP		# if j<0xf-i
	add $s0, $s0, $imm, 1 			# i=i+1
	
I_CONDITION:

	blt $imm, $s0, $a0, I_LOOP		# if i<0xf
	halt $zero, $zero, $zero, 0		# halt	
	
SWAP:	
									#sawping $t0 with $t1
	sw $t1, $s1, $imm, 0x100   		# ARR[j] = t1
	sw $t0, $s1, $imm, 0x101   		#  ARR[j+1] = t0

	add $t2, $t0, $zero				# tmp = t0
	add $t0, $t1, $zero				# t0 = t1
	add $t1, $t2, $zero				# t1 = tmp	
	beq $imm, $zero, $zero, BACK	#jump BACK



