
# --- Pseudo code --- #

# start_row = A/256 	
# end_row = B/256
# start_col = A%256     	
# end_col = C%256
# pixel_ofsset = 1
# row_number_in_triangle = 0
# while(row_number_in_triangle < end_row)
# 	while(j*(length of BC) >= (length of AB) - pixel offset)
#		print the i+pixel_offset
# 		pixel_offset++
#	row_number_in_triangle++

# ------------------- #


	# initialize the memory with the triangle's edges addresses
	.word 0x100 10   					# A = (1,10)
	.word 0x101 13066 					# B = (51,10) because 51*256 + 10 = 13066
	.word 0x102 13146					# C = (51,80)

	# load the addresses of the triangle's edges
	lw $a0, $zero, $imm, 0x100			# 0: get A from address 0x100
	lw $a1, $zero, $imm, 0x101			# 2: get B from address 0x101
	lw $a2, $zero, $imm, 0x102			# 2: get C from address 0x101

	add $s0, $a0, $zero, 0 		    	# we set $s0 to be the address of the first pixel to print in each line
	
iCondition:
	bge $imm, $s0, $a1, end 			# if (location of the first pixel in current line > B): the loop is done 

iLoop:
	# set the first pixel in line
	out $s0, $zero, $imm, 20			# update address of the pixel to be lighted
	add $t1, $zero, $imm, 0xff			# $t1 = 255
	out $t1, $zero, $imm, 21			# set pixel color to white
	add $t1, $zero, $imm, 1				# $t1 = 1
	out $t1, $zero, $imm, 22			# draw pixel
	
	add $s1, $zero, $imm, 1				# pixel_offset = 1
	sub $s2, $a2, $a1, 0				# $s2 = $a2 - $a1 = length of BC
	beq $imm, $zero, $zero, jCondition	# condition to see how many pixels should be printed in line

continue:
	add $s0, $s0, $imm, 256   			# $s0 = += 1 ($s0 is the first pixel to be printed in the current line)
	beq $imm, $zero, $zero, iCondition	# return to main loop
	

jCondition:
	sub $t0, $a1, $a0, 0				# 
	srl $t0, $t0, $imm, 8				# $t0 = ($a1-$a0)/256 = length of AB
	mul $t0, $t0, $s1, 0				# length of AB *= pixel_offset
	sub $t2, $s0, $a0, 0				#
	srl $t2, $t2, $imm, 8				# $t2 = row number when we count row 0 as the row the point A is at (row_number_in_triangle at pseudo code above)
	mul $t1, $s2, $t2					# $t1 = (length of BC) * row_number_in_triangle
	blt $imm, $t1, $t0, continue		# if (row_number_in_triangle * length of BC) < (length of AB): do not print any more pixels
	
jLoop:
	add $t0, $s0, $s1, 0				# $t0 = $s0 + pixel_offset
	out $t0, $zero, $imm, 20			# update address of the pixel to be lighted
	add $t1, $zero, $imm, 0xff			# $t1 = 255
	out $t1, $zero, $imm, 21			# set pixel color to white
	add $t1, $zero, $imm, 1				# $t1 = 1
	out $t1, $zero, $imm, 22			# draw pixel
	
	add $s1, $s1, $imm, 1				# pixel_offset += 1
	beq $imm, $zero, $zero, jCondition	# return to sub loop
	

end:
	halt $zero, $zero, $zero, 0			# halt



 