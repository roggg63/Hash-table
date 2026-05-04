default rel

section .text
global my_strcmp

my_strcmp:
            vmovdqu ymm0, [rdi]
            vpcmpeqb ymm1, ymm0, [rsi]
            vpmovmskb eax, ymm1
            cmp eax, 0FFFFFFFFh
            jne .not_equal

            vzeroupper
            xor eax, eax
            ret

.not_equal:
            vzeroupper
            mov eax, 1
            ret
