	.word 0x100 8               # values to calculate
	.word 0x101 3 
	
	add $sp,$zero,$imm,0		#
	add $sp,$zero,$imm,100		# initialize stack pointer
	lw $a3, $zero, $imm, 0x100	# take n from memmory
	lw $a4, $zero, $imm, 0x101	# take k from memmory
	jal $ra, $imm, $zero, BINOM	# calculate binom(n,k)
	sw $v0, $zero, $imm, 0x102	# memmory[0x102]=$v0 -- > result
	halt $zero, $zero, $zero, 0	# halt program
	
BINOM:

	add $sp, $sp, $imm, -4		# prepare stack to save 4 values
	sw $a3, $sp, $imm, 0		# save n of current recursive step 
	sw $a4, $sp, $imm, 1		# save k of current recursive step
	sw $ra, $sp, $imm, 2		# save return address
	sw $s0, $sp, $imm, 3		# save $s0 for temporary result

	
	
	add $t1, $zero, $imm, 0		# $t1 = 0
	beq $imm, $a4, $t1, RETURN	# jump to RETURN if k == 0
	beq $imm, $a4, $a3, RETURN	# jump to RETURN if n == k
	beq $imm, $zero, $zero, RECURSIVE_STEP	# call RECURSIVE_STEP
	
RECURSIVE_STEP: 

	sub $a3, $a3, $imm, 1		# calculate n - 1
	jal $ra, $imm, $zero, BINOM	# calculate binom(n-1, k)
	add $s0, $v0, $imm, 0		# $s0 = binom(n-1, k)
	lw $a3, $sp, $imm, 0		# get n
	lw $a4, $sp, $imm, 1		# get k 
	sub $a3, $a3, $imm, 1		# calculate n - 1
	sub $a4, $a4, $imm, 1		# calculate k - 1
	jal $ra, $imm, $zero, BINOM	# calculate binom(n-1, k-1)
	add $v0, $v0, $s0, 0		# $v0 =  binom(n-1, k-1) + binom(n-1, k)
	lw $s0, $sp, $imm, 3		# restore temporary result
	lw $ra, $sp, $imm, 2		# restore return address
	lw $a4, $sp, $imm, 1		# restore k of last recursive step
	lw $a3, $sp, $imm, 0		# restore n of last recursive step
	
	
EXIT:	

	add $sp, $sp, $imm, 4		# pull 4 values from stack
	beq $ra, $zero, $zero, 0	# return

RETURN:

	add $v0, $imm, $zero, 1		# else $v0 = 1
	beq $imm, $zero, $zero, EXIT