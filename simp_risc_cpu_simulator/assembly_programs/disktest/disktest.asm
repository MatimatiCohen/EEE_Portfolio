
#	for (i=0;i<128;i++){
#	  sector8[i]=sector0[i]+...+sector7[i]
#	}


add $a0, $zero, $imm, 7 					# a0 = num_of_sectors = 7 (0 to 6)
add $a1, $zero, $imm, 128					# a1 = num_of_words_in_sector = 128 (0 to 127)
add $s0, $zero, $imm, 1						# s0 = 1 disk is busy and also  can be useful to read command from Hard Disk
add $s1 ,$zero,$imm, 2						# s1=2 write command to hard disk
add $s2 ,$zero, $imm, 0x100					# $s2 = read buffer - the buffer address to read to from sectors 0-7
add $a3 , $zero , $imm, 0x200				# $a3= sum buffer - the buffer address to sum the words

out $s0, $zero, $imm, 1			 			# enable irq1
add $t2, $zero, $imm, DISKTEST				# $t2 = address of DISKTEST
out $t2, $zero, $imm, 6						# set irqhandler as DISKTEST
out $s0, $zero, $imm, 4		 				#  irq1status=1 for interrupt begin
halt $zero, $zero, $zero, 0					# halt	


DISKTEST:

out $zero, $zero, $imm, 4		 			#  irq1status=0 from now the simulator controls it.
add $t0, $zero, $imm, 0   					# j=0
beq $imm, $zero, $zero, SECTORֹ_LOOP  		# for j = 0 to num_of_sectors

READ_LOOP:

add $t1, $zero, $imm, 0						# i=0 (each read loop i from 0 to 127)	
add $v0 ,$zero,$imm, 0						# v0=result=0
	
WAIT_FOR_DISK_STATUS:	

in $t2, $zero, $imm, 17						#t2 = IO_REG[diskstatus]
bne $imm, $t2, $zero, WAIT_FOR_DISK_STATUS	#waiting for diskstatus to be 0	


out $t0, $zero, $imm, 15					#IO_REG[DISK_SECTOR]=j
out $s2,$zero, $imm, 16						# IO_REG[BUFFER]= read_address in main memmory
out $s0, $zero, $imm,14						#IO_REG[DISK_CMD] = 1 read from sector command 
beq $imm, $zero, $zero, WAIT_FOR_DATA 		# waiting for the reading to be done (IRQ1STATUS==1)

WAIT_FOR_DATA:

in $t2, $zero, $imm, 4						# $t2 = IO_REG[IRQ1STATUS] 
beq $imm, $t2, $zero, WAIT_FOR_DATA 		# wait while data isn't ready yet (1024 clock cycles)

out $zero,$zero,$imm, 4						# update irq1status to 0

SUM_WORDS:

lw $t2,$s2,$t1,0							# get word from address in memmory $t2=main_memmory[address1 + i]
lw $v0,$a3,$t1,0							# $v0 = main_memmory[address2+i]
add $v0, $v0,$t2,0							# $v0= $v0 + $t2
sw $v0,$a3, $t1,0							# main_memmory[address2+i]=$v0
add $t1,$t1,$imm,1							# i++	
blt $imm, $t1, $a1, SUM_WORDS				# if i<num_of_words_in_sector
add $t0,$t0,$imm,1							# j++

SECTORֹ_LOOP:

blt $imm, $t0, $a0, READ_LOOP				# if j<num_of_sectors
out $zero,$zero,$imm, 4						# update irq1status to 0
out $a0, $zero, $imm, 15					#IO_REG[DISK_SECTOR]=7
out $a3,$zero, $imm, 16						# IO_REG[BUFFER]= write_address from main memmory
out $s1, $zero, $imm,14						#IO_REG[DISK_CMD] = 2 write to sector command 



WAIT_FOR_DISK_STATUS1:	

in $t2, $zero, $imm, 17						#t2 = IO_REG[diskstatus]
bne $imm, $t2, $zero, WAIT_FOR_DISK_STATUS1	#waiting for diskstatus to be 0	

WAIT_FOR_WRITE:

in $t2, $zero, $imm, 4						# $t2 = IO_REG[IRQ1STATUS] 
beq $imm, $t2, $zero, WAIT_FOR_WRITE 		# wait while data isn't ready yet (1024 clock cycles)


out $zero,$zero,$imm, 4						# update irq1status to 0
reti $zero, $zero, $zero, 0					# return from interrupt
