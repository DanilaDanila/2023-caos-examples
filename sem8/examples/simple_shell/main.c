#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char *envp[5] = {NULL,};

#ifdef ALLOWENV
char *find_env(char *name) {
    char **env_value;
    int eq_pos = -1;
    for (env_value = envp; /* true */; env_value += 1) {
        if (*env_value == NULL) {
            break;
        }

        eq_pos = strstr(*env_value, "=") - *env_value;
        (*env_value)[eq_pos] = '\0';

        if (strcmp(name, *env_value) == 0) {
            break;
        }

        (*env_value)[eq_pos] = '=';
    }

    if (*env_value != NULL) {
        (*env_value)[eq_pos] = '=';
        return *env_value + eq_pos + 1;
    }

    return *env_value;
}
#endif //ALLOWENV


/* В целом это всё можно было бы захардкодить как-то так
 * 
 * char *envp[] = {
 *      "SHELL=caos_shell",
 *      "?=00000000",
 *      "PATH=/bin/:",
 *      NULL
 * };
 *
 * И я так в начале даже сдалал)))
 * Но я хотел прям там же менять значение `$?`
 * Но тут такое дело... константы располагаются в .rodata
 *
 * + Помогает ли АКОС писать программы на C - наверное
 *
 * + Помогает ли АКОС с умным лицом объяснить, почему
 *   ты ловишь SegFault - определенно да))
 *
 * Чтоб не колдовать с флагами, просто сделаем всё через strcpy
 * (я правда хотел найти способ проще, но не справился)
 */
void init_envp() {
    for (int i = 0; i < 5; ++i) {
        envp[i] = (char*)calloc(1024, sizeof(char));
    }

    strcpy(envp[0], "SHELL=caos_shell");
    strcpy(envp[1], "?=любой exit - success, кроме kernel panic!)");
    strcpy(envp[2], "LOGNAME=caos_seminarist");
}

void finilize_envp() {
    // finilize envp
    for (int i = 0; i < 5; ++i) {
        free(envp[i]);
    }
}


/* А вот эта функция будет создавать дочерний процесс
 * и в нём запускать переданную команду
 *
 * Ну и с умным видом (через perror) сообщать, если такой команды нет
 */
int try_run(char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        printf("COMMAND OUTPUT: ----\\\n");
        execvpe(argv[0], argv, envp);
        perror(argv[0]);
        _exit(0);
    }

    int status;
    waitpid(pid, &status, 0);
    printf("--------------------/\n");
    
    // yeah!!! let's do some magic with numbers!))))
    // просто `$?` идёт под индексом '1' (ибо я так захотел)
    // и там строка вида '?=smth'
    // и вот нам нужно заменить 'smth' на новый код возврата
    sprintf(envp[1] + 2, "%d", WEXITSTATUS(status));
    return 1;
}

int main() {
    // по названию очевидно
    init_envp();

    // А вот тут будем хранить аргументы
    // и да, тут есть допущение, что все они не длиннее 256
    char *argv[256] = {NULL,};

    // Тут будем хранить (динамически) введённую строку
    char *input = NULL;
    size_t input_cap = 0;
    for (;;) {
        // печатаем приглашение командной строки
        // (умное слово - PROMPT)
        printf("$> ");
        ssize_t line_len = getline(
                &input,
                &input_cap,
                stdin
        );

        // если файл закрылся / кончился
        // то выходим
        if (line_len < 0) {
            printf("\n`stdin` was closed\nexiting...\n");
            break;
        }

        // argv[0] - это путь до исполняемой программы
        argv[0] = input;
        int argv_i = 1;

        // а дальше проходимся в цикле и делаем что-то типа
        //
        // input().split(" ")
        //
        // (сори, python головного мозга)
        //
        // заметь, что нам тут не надо выделять память заново
        // мы меняем только указатели
        for (ssize_t i = 0; i < line_len; ++i) {
            // так получилось, что getline вводит и '\n'
            if (input[i] == '\n') {
                input[i] = '\0';
                break;
            }

            if (input[i] == ' ') {
                input[i] = '\0';
                argv[argv_i++] = input + i + 1;
            }
        }
        // ну и это нам нужно, чтоб не тащить аргументы
        // предыдущих команд
        //
        // echo 1 2 3 4 5 6 7 8
        // echo a b              <-- вот тут мы не должны печатать лишнего
        argv[argv_i] = NULL;

        #ifdef ALLOWENV
        for (int i = 1; i < argv_i; ++i) {
            if (argv[i][0] == '$') {
                argv[i] = find_env(argv[i] + 1);
            }
        }
        #endif // ALLOWENV

        #ifdef DEBUGINFO
        printf("\n----- DEBUG INFO -----\n");
        for (int i = 0; i < argv_i; ++i) {
            printf("argv[%d] = \"%s\"\n", i, argv[i]);
        }
        printf("argv[%d] = 0x%d\n", argv_i, argv[argv_i]);
        printf("----- DEBUG INFO -----\n\n");
        #endif // DEBUGINFO

        // название так-себе)))
        // команда всегда возвращает 1 (успех)
        // но идея понятна
        try_run(argv);
    }

    // по названию тоже очевидно
    finilize_envp();

    // getline эту штуку аллоцирует
    free(input);
    return 0;
}

