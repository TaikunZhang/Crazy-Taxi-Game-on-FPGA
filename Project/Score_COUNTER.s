 .text                   // executable code follows
          .global _count          
		  
_count: ldr r5,=0xff200020   //hex address
		ldr r6,=200000000   //load 200 mill
		ldr r9,=0xfffec600
		str r6,[r9]  //str 200 mill in load register 
		mov r6,#3
		str r6,[r9,#0x8] //load A bit and E bit
		LADR r7,#0     //count register
		mov r12,#0   //state register
		mov r10,#0xf  //overwrite
		
start_count:	
				cmp r7,#99
				moveq r7,#0
				add r7,#1
				strb r0,[r5,#1]
				
				ldr r4,[r9,#0xc]
				and r4,#1
				cmp r4,#1
				bne delay
				str r4,[r9,#0xc]
				b start_count