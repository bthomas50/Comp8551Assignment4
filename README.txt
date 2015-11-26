##### Comp 8551 Assignment 4 #####
##################################
############ Authors #############
##################################
Brian Thomas A00861166
Matt Ku      A00795612
##################################
############### Q1 ###############
##################################
The NEON intrinsics code runs about 100 times faster than the C++ code.

See <./Part_1/SomeNEONTests> XCode project.
##################################
############### Q2 ###############
##################################
The NEON inline assembly code runs about twice as fast as the NEON intrinsics code, and about 200 times faster than the C++ code. 
The difference between the inline assembly and the intrinsics is likely due to the intrinsics storing data on the stack, while the inline assembly works entirely in registers.

See <./Part_1/SomeNEONTests> XCode project.
##################################
############### Q3 ###############
##################################
We used inline assembly for this question. We found that the inline assembly implementation ran about twice as fast as the GLKMatrixMultiply() function. The small magnitude of this performance difference is because the GLKMatrixMultiply() function uses NEON intrinsics if they are available. The improvement, similar to Q2, likely comes from storing the data in registers instead of the stack.

See <./Part_1/Assignment1> XCode project.
The timing (rolling average) is written to the console.
##################################
############### Q4 ###############
##################################
We implemented the blending in an unmanaged C++ dll. 
The UI is a C# app.
It will crash if the "kernel", or "blend" image in our app, is larger (in either dimension) than the original image.
Because the original and kernel images can be of different sizes, we blend the kernel image onto the top-left subimage of the original image. This way, the formula:
   pBlendImg[i][j] = pOrigImg[i][j] * blendFac + pKernelImg[i][j] * (1 - blendFac);
is well-defined for i := [0, pKernelImg.width) 
                and j := [0, pKernelImg.height).

See <./Part_2/8551_asgn4> visual studio solution.
Please make sure to run "Build Solution" before running the app, to make sure the dll gets built.
##################################
############### Q5 ###############
##################################
We used inline MMX and SSE rather than intrinsics for this question.

See <./Part_2/8551_asgn4> visual studio solution.
Please make sure to run "Build Solution" before running the app, to make sure the dll gets built.
##################################
############### Q6 ###############
##################################
C++ time: 4400 - 4770 microseconds
MMX time: 689 - 800 microseconds
SSE time: 740 - 800 microseconds

MMX - 45 lines
SSE - 43 lines

MMX seems to be slightly faster.
Both give significant performance increases
over C++ code.

MMX seems to have a few extra
instructions that may not be necessary,
yet it runs faster.

See <./Part_2/8551_asgn4> visual studio solution.
Please make sure to run "Build Solution" before running the app, to make sure the dll gets built.
##################################
############### Q7 ###############
##################################
When calling the pass-by-value function, the compiled code does a MOVL -12(%rbp), %eax  instruction to get the value of x1.
When calling the pass-by-reference function, the compiled code does a LEAQ -16(%rbp), %eax  instruction to get the address of x2 (which is a QWORD on my 64-bit system)

See <./Part_3/q7.cpp> for c++ source and <./Part_3/q7.s> for the assembly code.
##################################
############### Q8 ###############
##################################
The compiled code does a call _Z7operatev  (_Z7operatev is the mangled name of the function operate()).
Instead of calling the inline function, the compiler substitutes the instructions:
	MOVL	$3, -8(%rbp)
	MOVL	$4, -4(%rbp)
	MOVL	-8(%rbp), %eax
	IMULL	-4(%rbp), %eax
(put the values on the stack, load them from the stack, do the multiplication and store the result in %eax), which is what the function did on the inside. It was able to skip the CALL, PUSHQ, POPQ, RET instructions that the operate() function had.

See <./Part_3/q8.cpp> for c++ source and <./Part_3/q8.s> for the assembly code.
##################################
