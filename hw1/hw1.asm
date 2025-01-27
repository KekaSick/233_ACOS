    .data
.macro newline
    li a0, '\n'
    li a7, 11
    ecall
.end_macro 

.macro printStr(%x)
        la a0, %x      # Load the address of the string into a0
        li a7, 4      # Syscall for printing a string in RISC-V
        ecall
.end_macro 

const_1.:
    .asciz "1. "
const_2.:
    .asciz "2. "
const_3.:
    .asciz "3. "
const_4.:
    .asciz "4. "
const_5.:
    .asciz "5. "
const_6.:
    .asciz "6. "
const_7.:
    .asciz "7. "
const_8.:
    .asciz "8. "
const_9.:
    .asciz "9. "
const_10.:
    .asciz "10. "
const_11.:
    .asciz "11. "
const_12.:
    .asciz "12. "
const_13.:
    .asciz "13. "
const_14.:
    .asciz "14. "
const_15.:
    .asciz "15. "
const_16.:
    .asciz "16. "


    
    .text
main:
# x: t0 = readInt()
    li a7, 5
    ecall
    mv t0, a0
    
# y: t0 = readInt()
    li a7, 5
    ecall
    mv t1, a0


# 1.
printStr(const_1.)

# (x+5)
    li t2, 5
    add t3, t0, t2
# (y-7)   
    li t2, 7
    sub t4, t1, t2

# (x+5)-(y-7)        
    sub t4, t3, t4
    
# print (x+5)-(y-7)
    mv a0, t4
    li a7, 1
    ecall
    
newline
    
# 2.
printStr(const_2.)
# (x>>2)
    srli t3, t0, 2
    
# (y-1)
    li t2, 1
    sub t4, t1, t2
    
# ((y-1)<<3)
    slli t4, t4, 3
    
# (x>>2)+((y-1)<<3)
    add t4, t3, t4
    
# print (x>>2)+((y-1)<<3)
    mv a0, t4
    li a7, 1
    ecall
    
newline
    
# 3.
printStr(const_3.)
# (x<<y)
    sll t2, t0, t1

# (x<<y) - 10
    li t3, 10
    sub t2, t2, t3
  
# print (x<<y) - 10 
    mv a0, t2
    li a7, 1
    ecall   
    
newline
    
 # 4.
printStr(const_4.) 
 # (x>>y)
    sra t2, t0, t1
    
 # (x>>y) + 10
    li t3, 10
    add t3, t2, t3
    
 # print (x>>y) + 10
    mv a0, t3
    li a7, 1
    ecall   
    
newline
 
 # 5. 
 printStr(const_5.)
 # (x << 2)
    slli t2, t0, 2
    
 # (x << 2) - y
    sub t2, t2, t1
    
 # (x << 2) - y + 5
    li t3, 5
    add t2, t2, t3
    
 # ((x << 2) - y + 5) >> 1
    srai t2, t2, 1
 
 # print ((x << 2) - y + 5) >> 1
    mv a0, t2
    li a7, 1
    ecall   
    
newline  
    
 # 6.
 printStr(const_6.)
 # x * 6 = (x << 2) + (x << 1)
    slli t2, t0, 2    # x << 2 (x * 4)
    slli t3, t0, 1    # x << 1 (x * 2)
    add t4, t2, t3    # (x << 2) + (x << 1) = x * 6

# y * 3 = (y << 1) + y
    slli t2, t1, 1    # y << 1 (y * 2)
    add t5, t2, t1    # (y << 1) + y = y * 3

# x * 6 - y * 3
    sub t6, t4, t5    # (x * 6) - (y * 3)

# print (x * 6 - y * 3)
    mv a0, t6         
    li a7, 1          
    ecall

newline
 
 # 7.
 printStr(const_7.)   
 # 2*x*x
    li t2, 2
    mul t2, t0, t2
    mul t2, t0, t2
    
 # 3*y
    li t3, 3
    mul t3, t3, t1
    
 # 2*x*x - 3*y
    sub t3, t2, t3
    
 # 2*x*x - 3*y + 4
    li t4, 4
    add t4, t3, t4
    
 # print 2*x*x - 3*y + 4   
    mv a0, t4         
    li a7, 1          
    ecall

newline 
    
 # 8. 
 printStr(const_8.)
 # (x+5)
    li t2, 5
    add t2, t0, t2
    
 # (x+5)/y
    div t2, t2, t1
    
 # (y-1)
    li t3, 1
    sub t3, t1, t3
    
 # 10/(y-1)  
    li t4, 10
    div t4, t4, t3
    
 # (x+5)/y + 10/(y-1)  
    add t4, t2, t4
 
 # print (x+5)/y + 10/(y-1)
    mv a0, t4         
    li a7, 1          
    ecall

newline
    
 # 9.
 printStr(const_9.)
 # (x/y)
    div t2, t0, t1

 # (x/y)*y
    mul t2, t2, t1    
    
 # x%y
    rem t3, t0, t1
    
 # (x/y)*y + x%y
    add t2, t2, t3
    
 # print (x/y)*y + x%y
    mv a0, t2         
    li a7, 1          
    ecall
 
newline
    
 # 10
 printStr(const_10.)
    li t2, -1
    slli t2, t2, 2
    
 # x & (-1<<2)
    and t2, t0, t2
    
 # print x & (-1<<2)
    mv a0, t2         
    li a7, 1          
    ecall
 
newline

 # 11 
 printStr(const_11.)
    li t2, -1
    srli t2, t2, 30   
    
 # x | (-1 >> 30)
    or t2, t0, t2
    
  # print x & (-1 >> 30)
    mv a0, t2         
    li a7, 1          
    ecall
    
newline
    
 # 12
 printStr(const_12.)
 # mask 
    li t2, 1
    sll t2, t2, t1
    
 # apply mask
    or t2, t0, t2
    
 # print 
 
    mv a0, t2         
    li a7, 1          
    ecall  
    
newline
    
 # 13
 printStr(const_13.)
 # mask 
    li t2, 1
    sll t2, t2, t1
    
 # invert mask 
    not t2, t2
 # apply mask
    and t2, t0, t2
    
 # print 
    mv a0, t2         
    li a7, 1          
    ecall  
    
newline
    
 # 14
 printStr(const_14.)
    slt t3, t1, t0      # (y < x)
    beqz t3, else       # if t3 == 0 (x <= y), to к else
    li t2, 0            # if x > y, t2 = 0
    j end1               # skip else
else:
    li t2, 1            # if x <= y, t2 = 1
end1:
 
 # print 
    mv a0, t2         
    li a7, 1          
    ecall  
          
newline
    
 # 15
 printStr(const_15.)
    addi t2, t1, 3     
    beq t2, t0, equal      
    li t3, 1            
    j end2             
equal:
    li t3, 0
end2:

  # print 
    mv a0, t3         
    li a7, 1          
    ecall  
    
newline
    
  # 16
 printStr(const_16.)
    li t3, -5           # -5
    bgt t0, t3, check_y # if x > -5, then check y
    li t2, 0            # false
    j end               

check_y:
    li t4, 5            # 5
    blt t1, t4, success # if y < 5, true
    li t2, 0            # false
    j end               

success:
    li t2, 1            #  x > -5 && y < 5 true

end:

  # print 
    mv a0, t2         
    li a7, 1          
    ecall  
    
newline
