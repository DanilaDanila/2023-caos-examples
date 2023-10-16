.intel_syntax noprefix

.global arrsum

// int (int N, int *arr)
// edi := N
// rsi := arr
arrsum:
    mov ebx, [rsi]     // сохраняем первый элемент массива
    .loop:
        add rsi, 4     // сдвиг на 1 элемент
        sub edi, 1     // уменьшаем N
        jz .loop_end   // if N == 0 { goto .loop_end }
        mov edx, [rsi] // загружаем следующий элемент
        add ebx, edx   // прибавляем его к сумме
        jmp .loop      // goto .loop
    .loop_end:
    mov eax, ebx       // ret_value := sum
    ret                // return ret_value

