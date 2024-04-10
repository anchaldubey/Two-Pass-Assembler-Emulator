; Name: Anchal Dubey
; Roll No. 2201CS15
; Sum of first n natural numbers



call func
HALT

func:       stl 0       ; store return address (from A) into stack
            adj -1      ; adjust stack pointer
    loop:   ldc n
            ldnl 0      ; load num into A
            ldc total
            ldnl 0      ; load sum into A, B = num
            add         ; A = B + A
            ldc total
            stnl 0      ; store new value of sum
            ldc n
            ldnl 0      ; load num into A again
            adc -1      ; decrement A
            brz done    ; if A == 0, stop sum calculation
            ldc n
            stnl 0      ; otherwise update num value
            br loop     ; repeat calculation
    done:   
            adj 1       ; if done, adjust stack pointer
            ldl 0       ; load calling address address to A
            return      ; return to calling address

n: data 10       ; number upto which sum is calculated
total: data 0         ; sum is stored here