---
marp: true
paginate: true
---

<style>
img[alt~="center"] {
  display: block;
  margin: 0 auto;
}
.columns {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 1rem;
}
.label {
  font-size: 20px !important;
}
</style>

<script src="https://unpkg.com/mermaid@8.1.0/dist/mermaid.min.js"></script>
<script>mermaid.initialize({startOnLoad:true});</script>


# Семинар 1
- компиляция, gdb, gcc, sanitizers, strace

---

## Вводная часть

---

### Про меня
- Манаков Данила
- МФТИ
- работаю в Тинькофф
- можно писать в тг @DanilaManakov в любое время
- не факт, что я вам отвечу, но писать можно

---

### Чат групп(ы?)

<Даня, не забудь втавить сюда QR-код>

---

### Как закрыть курс

---

### 1. Инструменты
- Linux (хотяб Ubuntu)
  + лучше не WSL
  + да, можно виртуалку
- bash / zsh
- gcc + gas

---

### 2. Оценка

1. делаете задачи
2. защищаете задачи
3. оценка ставится по сумме баллов

---

## ~~Вводная часть~~
## Сборка проекта

---

### Код проекта $\rightarrow$ _???_ $\rightarrow$ EFL

<div class="columns">
<div>

```c
#include <stdio.h>

int main() {
    printf("Hello, world!"\n);
    return 0;
}
```

</div>
<div>

```bash
$ file a.out
a.out: ELF 64-bit LSB pie executable, ...
```
</div>
</div>

---

### Этапы сборки проекта

В процессе сборки проекта последовательно запускаются:

1. Препроцессор
   - из кода на С делает код на С
2. Компилятор
   - из кода на С делает код на ASM
3. Ассемблер
   - из кода на ASM делает op-коды
4. Линкер
   - собирает разные .o файлы вместе
   - настраивает адресацию функций
   - настраивает адресацию библиотек

---

### 1. Препроцессор

- выполняет простые операции с текстом (вырезать + вставить)
- обрабатывает инструкции, которые начинаются с #
  - #define
  - #include
  - #ifndef
  - #ifdef
  - #else
  - #end
  - ...

---

### 1. Препроцессор

```c
#include <stdio.h>

// у нас так НЕ ПРИНЯТО
// за такое будем бить
#define PI 3.1415926  // use `enum` instead

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))

int main() {
    // ...
}
```

---

### 1. Препроцессор

<div class="columns">
<div>

```c
// somelib.h
void sml() {
  // ...
}
```

```c
// foo.h
#include "somelib.h"

void foo() {
  sml();
}
```

```c
// bar.h
#include "somelib.h"

void bar() {
  sml();
}
```
</div>
<div>

```c
#include "foo.h"
#include "bar.h"

int main() {
  foo();
  bar();
}
```

<div class="mermaid">
  graph TD;
      somelib.h-->foo.h;
      somelib.h-->bar.h;
      foo.h-->main.c;
      bar.h-->main.c;
</div>
</div>
</div>

---

### 1. Препроцессор

Скомпилировать такое не выйдет :(

```bash
$ gcc main.c
In file included from bar.h:1,
                 from main.c:2:
somelib.h:1:6: error: redefinition of ‘sml’
    1 | void sml() {
      |      ^~~
In file included from foo.h:1,
                 from main.c:1:
somelib.h:1:6: note: previous definition of ‘sml’ with type ‘void()’
    1 | void sml() {
      |
```

---

### 1. Препроцессор

Потому что на самом деле там не ромбик, а такая штука

Хотя мы очень хотим, чтобы был ромбик

<div class="columns">
<div>
<div class="mermaid">
  graph TD;
      somelib.h-->foo.h;
      somеlib.h-->bar.h;
      foo.h-->main.c;
      bar.h-->main.c;
</div>
</div>
<div>
<div class="mermaid">
  graph TD;
      somelib.h-->foo.h;
      somelib.h-->bar.h;
      foo.h-->main.c;
      bar.h-->main.c;
</div>
</div>
</div>

---

### 1. Препроцессор

Решается это как-то так


<div class="columns">
<div>

```c
// somelib.h
#ifndef SOMELIB_H
#define SOMELIB_H

void sml() {
  // ...
}

#endif // SOMELIB_H
```
</div>
<div>

```c
// foo.h
#ifndef FOO_H
#define FOO_H

#include "somelib.h"

void foo() {
  sml();
}

#endif // FOO_H
```
</div>
</div>

(аналогично `bar.h`)

---

### 1. Препроцессор

и такое чудо даже сработает

```c
// gcc -E main.c
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "foo.h" 1
# 1 "somelib.h" 1



void sml() {
}
# 2 "foo.h" 2

void foo() {
    sml();
}
# 2 "main.c" 2
# 1 "bar.h" 1


void bar() {
    sml();
}
# 3 "main.c" 2

int main() {
    foo();
    bar();
}
```

---

### 1. Препроцессор

Ну или чуть менее жизненный пример (в таком виде не компилируется)

<div class="columns">
<div>

```c
// foo.h
#include "bar.h"

void external_foo() { /* ... */ }

void foo() { external_bar(); }
```

```c
// bar.h
#include "foo.h"

void external_bar() { /* ... */ }

void bar() { external_foo(); }
```
</div>
<div>

```c
#include "foo.h"
#include "bar.h"

int main() {
  foo();
  bar();
}
```


<div class="mermaid">
  graph TD;
      foo.h-->bar.h;
      bar.h-->foo.h;
      foo.h-->main.c;
      bar.h-->main.c;
</div>
</div>
</div>

---

### 1. Препроцессор

На самом деле чаще встречается что-то такое

<div class="columns">
<div>

```c
// foobar.h
#ifndef FOOBAR_H
#define FOOBAR_H

void foobar() {
    // ...
}

#endif // FOOBAR_H
```
</div>
<div>

```c
// main.c
#include "foobar.h"
#define PI 3.1415926

int main() {
    foobar();
    printf("%lf\n", PI);
    return 0;
}
```
</div>
</div>

```bash
$ gcc -E main.c > main.i
```

---

### 1. Препроцессор

<div class="columns">
<div>

```c
// foobar.h
#ifndef FOOBAR_H
#define FOOBAR_H

void foobar() {
    // ...
}

#endif // FOOBAR_H
```

```c
// main.c
#include "foobar.h"
#define PI 3.1415926

int main() {
    foobar();
    printf("%lf\n", PI);
    return 0;
}
```
</div>
<div>

```c
// gcc -E main.c
// и да, ниже валидный код на C
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "foobar.h" 1
// (письмена выше понимать не надо)


void foobar() {
    // ...
}
# 2 "main.c" 2


int main() {
    foobar();
    printf("%lf\n", 3.1415926);
    return 0;
}
```
</div>
</div>

---

### 2. Компилятор

- это тот, который из кода на C делает код на языке ассемблера

---

### 2. Компилятор

<div class="columns">
<div>

```c
// gcc -E main.c
// и да, ниже валидный код на C
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "foobar.h" 1
// (письмена выше понимать не надо)


void foobar() {
    // ...
}
# 2 "main.c" 2


int main() {
    foobar();
    printf("%lf\n", 3.1415926);
    return 0;
}
```
</div>

<div>

```x86asm
        .file   "main.c"
        .intel_syntax noprefix
        .text
        .globl  foobar
        .type   foobar, @function
foobar:
.LFB0:
        .cfi_startproc
        push    rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        mov     rbp, rsp
        .cfi_def_cfa_register 6
        nop
        pop     rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE0:
        .size   foobar, .-foobar
        .section        .rodata
.LC1:
        .string "%lf\n"
        .text
        .globl  main
        .type   main, @function
main:
.LFB1:
        .cfi_startproc
        push    rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        mov     rbp, rsp
        .cfi_def_cfa_register 6
        mov     eax, 0
        call    foobar
        mov     rax, QWORD PTR .LC0[rip]
        movq    xmm0, rax
        lea     rax, .LC1[rip]
        mov     rdi, rax
        mov     eax, 1
        call    printf@PLT
        mov     eax, 0
        pop     rbp
        .cfi_def_cfa 7, 8
        ret
        .cfi_endproc
.LFE1:
        .size   main, .-main
        .section        .rodata
        .align 8
.LC0:
        .long   1293080650
        .long   1074340347
        .ident  "GCC: (GNU) 12.2.1 20230201"
        .section        .note.GNU-stack,"",@progbits
```
</div>
</div>

```bash
$ gcc -S -masm=intel main.i
```

---

### 2. Компилятор

<div class="columns">
<div>

```c
// gcc -E main.c
// и да, ниже валидный код на C
# 0 "main.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "main.c"
# 1 "foobar.h" 1
// (письмена выше понимать не надо)


void foobar() {
    // ...
}
# 2 "main.c" 2


int main() {
    foobar();
    printf("%lf\n", 3.1415926);
    return 0;
}
```
</div>

<div>

```x86asm
# ...
foobar:
.LFB0:
        push    rbp
        mov     rbp, rsp
        nop
        pop     rbp
        ret
# ...
main:
.LFB1:
        push    rbp
        mov     rbp, rsp
        mov     eax, 0
        call    foobar
        mov     rax, QWORD PTR .LC0[rip]
        movq    xmm0, rax
        lea     rax, .LC1[rip]
        mov     rdi, rax
        mov     eax, 1
        call    printf@PLT
        mov     eax, 0
        pop     rbp
        ret
# ...
```
</div>
</div>

```bash
$ gcc -S -masm=intel main.i
```

---

### 3. Ассемблер

- `ассемблер` - это программа
- `язык ассемблера` - это 'язык'
- `ассемблер` переводит код на `языке ассемблера` в op-коды

---

### 3. Ассемблер

Продолжаем издевательства

```bash
$ gcc -c main.s
```

Получаем ELF-файлик (не совсем правда)

```bash
$ file main.o
main.o: ELF 64-bit LSB relocatable, x86-64, version 1 (SYSV), not stripped
```

- файлик содержит op-коды
- но не содержит необходимой 'обертки' для запуска
- (в MS-DOS такое уже можно было запускать)

---

### 3. Ассемблер

Можно посмотреть, что там внутри этого файла

```sh
$ objdump -d -M intel main.o

main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <foobar>:
   0:   55                      push   rbp
   1:   48 89 e5                mov    rbp,rsp
   4:   90                      nop
   5:   5d                      pop    rbp
   6:   c3                      ret

0000000000000007 <main>:
   7:   55                      push   rbp
   8:   48 89 e5                mov    rbp,rsp
   b:   b8 00 00 00 00          mov    eax,0x0
  10:   e8 00 00 00 00          call   15 <main+0xe>
  15:   48 8b 05 00 00 00 00    mov    rax,QWORD PTR [rip+0x0]        # 1c <main+0x15>
  1c:   66 48 0f 6e c0          movq   xmm0,rax
  21:   48 8d 05 00 00 00 00    lea    rax,[rip+0x0]        # 28 <main+0x21>
  28:   48 89 c7                mov    rdi,rax
  2b:   b8 01 00 00 00          mov    eax,0x1
  30:   e8 00 00 00 00          call   35 <main+0x2e>
  35:   b8 00 00 00 00          mov    eax,0x0
  3a:   5d                      pop    rbp
  3b:   c3                      ret
```

---

### 4. Линкер

- собирает несколько .o файлов вместе
- высчитывает ссылки на функции
- 'линкует' динамические библиотеки

---

### 4. Линкер

Дальше запускаем простую команду

`$ ld /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/x86_64-linux-gnu/crtn.o /usr/lib/x86_64-linux-gnu/crt1.o -lc main.o -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o main_ELF_executable`

---

### 4. Линкер

Дальше запускаем простую команду

~~`$ ld /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/x86_64-linux-gnu/crtn.o /usr/lib/x86_64-linux-gnu/crt1.o -lc main.o -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o main_ELF_executable`~~

```bash
$ gcc main.o
$ ./a.out
3.141593
```

---

### 4. Линкер

Для сравнения посмотрим на оба файлика

<div class="columns":>
<div>

```bash
$ objdump -d -M intel main.o

main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <foobar>:
   0:   55                      push   rbp
   1:   48 89 e5                mov    rbp,rsp
   4:   90                      nop
   5:   5d                      pop    rbp
   6:   c3                      ret

0000000000000007 <main>:
   7:   55                      push   rbp
   8:   48 89 e5                mov    rbp,rsp
   b:   b8 00 00 00 00          mov    eax,0x0
  10:   e8 00 00 00 00          call   15 <main+0xe>
  15:   48 8b 05 00 00 00 00    mov    rax,QWORD PTR [rip+0x0]        # 1c <main+0x15>
  1c:   66 48 0f 6e c0          movq   xmm0,rax
  21:   48 8d 05 00 00 00 00    lea    rax,[rip+0x0]        # 28 <main+0x21>
  28:   48 89 c7                mov    rdi,rax
  2b:   b8 01 00 00 00          mov    eax,0x1
  30:   e8 00 00 00 00          call   35 <main+0x2e>
  35:   b8 00 00 00 00          mov    eax,0x0
  3a:   5d                      pop    rbp
  3b:   c3                      ret
```
</div>
<div>

```bash
$ objdump -d a.out 

a.out:     file format elf64-x86-64


Disassembly of section .init:

0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    1004:       48 83 ec 08             sub    $0x8,%rsp
    1008:       48 8b 05 c1 2f 00 00    mov    0x2fc1(%rip),%rax        # 3fd0 <__gmon_start__@Base>
    100f:       48 85 c0                test   %rax,%rax
    1012:       74 02                   je     1016 <_init+0x16>
    1014:       ff d0                   call   *%rax
    1016:       48 83 c4 08             add    $0x8,%rsp
    101a:       c3                      ret

Disassembly of section .plt:

0000000000001020 <printf@plt-0x10>:
    1020:       ff 35 ca 2f 00 00       push   0x2fca(%rip)        # 3ff0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:       ff 25 cc 2f 00 00       jmp    *0x2fcc(%rip)        # 3ff8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:       0f 1f 40 00             nopl   0x0(%rax)

0000000000001030 <printf@plt>:
    1030:       ff 25 ca 2f 00 00       jmp    *0x2fca(%rip)        # 4000 <printf@GLIBC_2.2.5>
    1036:       68 00 00 00 00          push   $0x0
    103b:       e9 e0 ff ff ff          jmp    1020 <_init+0x20>

Disassembly of section .text:

0000000000001040 <_start>:
    1040:       f3 0f 1e fa             endbr64
    1044:       31 ed                   xor    %ebp,%ebp
    1046:       49 89 d1                mov    %rdx,%r9
    1049:       5e                      pop    %rsi
    104a:       48 89 e2                mov    %rsp,%rdx
    104d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
    1051:       50                      push   %rax
    1052:       54                      push   %rsp
    1053:       45 31 c0                xor    %r8d,%r8d
    1056:       31 c9                   xor    %ecx,%ecx
    1058:       48 8d 3d e1 00 00 00    lea    0xe1(%rip),%rdi        # 1140 <main>
    105f:       ff 15 5b 2f 00 00       call   *0x2f5b(%rip)        # 3fc0 <__libc_start_main@GLIBC_2.34>
    1065:       f4                      hlt
    1066:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
    106d:       00 00 00 
    1070:       48 8d 3d a1 2f 00 00    lea    0x2fa1(%rip),%rdi        # 4018 <__TMC_END__>
    1077:       48 8d 05 9a 2f 00 00    lea    0x2f9a(%rip),%rax        # 4018 <__TMC_END__>
    107e:       48 39 f8                cmp    %rdi,%rax
    1081:       74 15                   je     1098 <_start+0x58>
    1083:       48 8b 05 3e 2f 00 00    mov    0x2f3e(%rip),%rax        # 3fc8 <_ITM_deregisterTMCloneTable@Base>
    108a:       48 85 c0                test   %rax,%rax
    108d:       74 09                   je     1098 <_start+0x58>
    108f:       ff e0                   jmp    *%rax
    1091:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    1098:       c3                      ret
    1099:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10a0:       48 8d 3d 71 2f 00 00    lea    0x2f71(%rip),%rdi        # 4018 <__TMC_END__>
    10a7:       48 8d 35 6a 2f 00 00    lea    0x2f6a(%rip),%rsi        # 4018 <__TMC_END__>
    10ae:       48 29 fe                sub    %rdi,%rsi
    10b1:       48 89 f0                mov    %rsi,%rax
    10b4:       48 c1 ee 3f             shr    $0x3f,%rsi
    10b8:       48 c1 f8 03             sar    $0x3,%rax
    10bc:       48 01 c6                add    %rax,%rsi
    10bf:       48 d1 fe                sar    %rsi
    10c2:       74 14                   je     10d8 <_start+0x98>
    10c4:       48 8b 05 0d 2f 00 00    mov    0x2f0d(%rip),%rax        # 3fd8 <_ITM_registerTMCloneTable@Base>
    10cb:       48 85 c0                test   %rax,%rax
    10ce:       74 08                   je     10d8 <_start+0x98>
    10d0:       ff e0                   jmp    *%rax
    10d2:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
    10d8:       c3                      ret
    10d9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10e0:       f3 0f 1e fa             endbr64
    10e4:       80 3d 2d 2f 00 00 00    cmpb   $0x0,0x2f2d(%rip)        # 4018 <__TMC_END__>
    10eb:       75 33                   jne    1120 <_start+0xe0>
    10ed:       55                      push   %rbp
    10ee:       48 83 3d ea 2e 00 00    cmpq   $0x0,0x2eea(%rip)        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    10f5:       00 
    10f6:       48 89 e5                mov    %rsp,%rbp
    10f9:       74 0d                   je     1108 <_start+0xc8>
    10fb:       48 8b 3d 0e 2f 00 00    mov    0x2f0e(%rip),%rdi        # 4010 <__dso_handle>
    1102:       ff 15 d8 2e 00 00       call   *0x2ed8(%rip)        # 3fe0 <__cxa_finalize@GLIBC_2.2.5>
    1108:       e8 63 ff ff ff          call   1070 <_start+0x30>
    110d:       c6 05 04 2f 00 00 01    movb   $0x1,0x2f04(%rip)        # 4018 <__TMC_END__>
    1114:       5d                      pop    %rbp
    1115:       c3                      ret
    1116:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
    111d:       00 00 00 
    1120:       c3                      ret
    1121:       66 66 2e 0f 1f 84 00    data16 cs nopw 0x0(%rax,%rax,1)
    1128:       00 00 00 00 
    112c:       0f 1f 40 00             nopl   0x0(%rax)
    1130:       f3 0f 1e fa             endbr64
    1134:       e9 67 ff ff ff          jmp    10a0 <_start+0x60>

0000000000001139 <foobar>:
    1139:       55                      push   %rbp
    113a:       48 89 e5                mov    %rsp,%rbp
    113d:       90                      nop
    113e:       5d                      pop    %rbp
    113f:       c3                      ret

0000000000001140 <main>:
    1140:       55                      push   %rbp
    1141:       48 89 e5                mov    %rsp,%rbp
    1144:       b8 00 00 00 00          mov    $0x0,%eax
    1149:       e8 eb ff ff ff          call   1139 <foobar>
    114e:       48 8b 05 bb 0e 00 00    mov    0xebb(%rip),%rax        # 2010 <_IO_stdin_used+0x10>
    1155:       66 48 0f 6e c0          movq   %rax,%xmm0
    115a:       48 8d 05 a7 0e 00 00    lea    0xea7(%rip),%rax        # 2008 <_IO_stdin_used+0x8>
    1161:       48 89 c7                mov    %rax,%rdi
    1164:       b8 01 00 00 00          mov    $0x1,%eax
    1169:       e8 c2 fe ff ff          call   1030 <printf@plt>
    116e:       b8 00 00 00 00          mov    $0x0,%eax
    1173:       5d                      pop    %rbp
    1174:       c3                      ret

Disassembly of section .fini:

0000000000001178 <_fini>:
    1178:       f3 0f 1e fa             endbr64
    117c:       48 83 ec 08             sub    $0x8,%rsp
    1180:       48 83 c4 08             add    $0x8,%rsp
    1184:       c3                      ret
```
</div>
</div>

---

### 4. Линкер

Там есть уже знакомые нам куски


```bash
0000000000001139 <foobar>:
    1139:       55                      push   %rbp
    113a:       48 89 e5                mov    %rsp,%rbp
    113d:       90                      nop
    113e:       5d                      pop    %rbp
    113f:       c3                      ret
```

```bash
0000000000001140 <main>:
    1140:       55                      push   %rbp
    1141:       48 89 e5                mov    %rsp,%rbp
    1144:       b8 00 00 00 00          mov    $0x0,%eax
    1149:       e8 eb ff ff ff          call   1139 <foobar>
    114e:       48 8b 05 bb 0e 00 00    mov    0xebb(%rip),%rax        # 2010 <_IO_stdin_used+0x10>
    1155:       66 48 0f 6e c0          movq   %rax,%xmm0
    115a:       48 8d 05 a7 0e 00 00    lea    0xea7(%rip),%rax        # 2008 <_IO_stdin_used+0x8>
    1161:       48 89 c7                mov    %rax,%rdi
    1164:       b8 01 00 00 00          mov    $0x1,%eax
    1169:       e8 c2 fe ff ff          call   1030 <printf@plt>
    116e:       b8 00 00 00 00          mov    $0x0,%eax
    1173:       5d                      pop    %rbp
    1174:       c3                      ret
```

---

### 4. Линкер

А есть что-то дикое (часть из этого коснёмся в будущем)
```bash
0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    # ...

0000000000001020 <printf@plt-0x10>:
    1020:       ff 35 ca 2f 00 00       push   0x2fca(%rip)        # 3ff0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:       ff 25 cc 2f 00 00       jmp    *0x2fcc(%rip)        # 3ff8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:       0f 1f 40 00             nopl   0x0(%rax)

0000000000001030 <printf@plt>:
    1030:       ff 25 ca 2f 00 00       jmp    *0x2fca(%rip)        # 4000 <printf@GLIBC_2.2.5>
    1036:       68 00 00 00 00          push   $0x0
    103b:       e9 e0 ff ff ff          jmp    1020 <_init+0x20>


0000000000001040 <_start>:
    1040:       f3 0f 1e fa             endbr64
    1044:       31 ed                   xor    %ebp,%ebp
    1046:       49 89 d1                mov    %rdx,%r9
    1049:       5e                      pop    %rsi
    104a:       48 89 e2                mov    %rsp,%rdx
    104d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
    1051:       50                      push   %rax
    1052:       54                      push   %rsp
    # ...

0000000000001178 <_fini>:
    1178:       f3 0f 1e fa             endbr64
    117c:       48 83 ec 08             sub    $0x8,%rsp
    1180:       48 83 c4 08             add    $0x8,%rsp
    1184:       c3                      ret
```

---

### Подытог процесса сборки

1. Препроцессор
```bash
gcc -E main.c > main.i
```
2. Компилятор
```bash
gcc -S -masm=intel main.i
```
3. Ассемблер
```bash
gcc -c main.s
```
4. Линкер
```bash
gcc main.o
```

---

### Подытог процесса сборки

Хотя в жизни мы с вами будем делать как-то так

```bash
gcc main.c -o hw1_ex1
```

ну или так

```bash
gcc main.c
```

---

### Подытог процесса сборки

А почему везде `gcc`?

```bash
# GCC - GNU C Compiler
# GCC - GNU Compiler Collection
# C / C++
# ASM
# Fortran
# Pascal
# ...

# gnu c compiler
gcc -E main.c > main.i

# gnu c compiler (again)
gcc -S -masm=intel main.i

# gnu assembler
gcc -c main.s

# ld - The GNU linker
gcc main.o
```

---

## GDB
- gnu debugger

---

### Шпаргалка по основным командам

- `r` / `run`
- `b` / `break`
- `c` / `continue`
- `n` / `next`
- `s` / `step`
- `layout src` / `layout asm` / `layout regs`
- `print`
