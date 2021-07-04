sys_exit:		equ				60

				section			.text
				global			_start

buf_size:		equ				8192
_start:
				xor				rbx, rbx
				sub				rsp, buf_size
				mov				rsi, rsp

				mov				r10, 1

read_again:
				xor				rax, rax
				xor				rdi, rdi
				mov				rdx, buf_size
				syscall

				test			rax, rax
				jz				quit
				js				read_error

				xor				rcx, rcx
check_char:
				cmp				rcx, rax
				je				read_again
				mov				r11b, byte [rsi + rcx] 
				mov				r9, r11
				call			func
				cmp				r10, r12
				jng				skip
				inc				rbx
skip:
				mov				r10, r12
				inc				rcx
				jmp				check_char

quit:
				mov				rax, rbx
				call			print_int

				mov				rax, sys_exit
				xor				rdi, rdi
				syscall
			  
	 
func:
				cmp				r11b, 0x20
				je				ret_true
		
				cmp				r11b, 0x0d
				jg				ret_false
				cmp				r11b, 0x09
				jl				ret_false
		
ret_true:
				mov				r12, 1
				ret
ret_false:
				mov				r12, 0
				ret
		
; cmp r11 == 32 || (r11 < 14 && r11 > 8) 
; rax -- number to print
print_int:
				mov				rsi, rsp
				mov				rbx, 10

				dec				rsi
				mov				byte [rsi], 0x0a

next_char:
				xor				rdx, rdx
				div				rbx
				add				dl, '0'
				dec				rsi
				mov				[rsi], dl
				test			rax, rax
				jnz				next_char

				mov				rax, 1
				mov				rdi, 1
				mov				rdx, rsp
				sub				rdx, rsi
				syscall

				ret

read_error:
				mov				rax, 1
				mov				rdi, 2
				mov				rsi, read_error_msg
				mov				rdx, read_error_len
				syscall

				mov				rax, sys_exit
				mov				rdi, 1
				syscall

				section			.rodata

read_error_msg: db				"read failure", 0x0a
read_error_len: equ				$ - read_error_msg
