#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

/* Пример игрушечный и довольно плохой
 * Но тут отражено всё, что вам может понадобиться в домашке
 *
 * Есть воркер `do_work`, который каждый раз, когда просыпается,
 * выставляет новый next_out, сигнализирует об этом, и идёт дальше спать...
 *
 * Соответственно есть `main`, который запрашивает новое число у `do_work`
 * Можно пофантазировать и сказать, что мы выполняем поиск делителей числа...
 * И `do_work` отдает нам эти делители 'по востребованию'
 */

// mutex, condvar и штука для возвращения значений
typedef struct {
  pthread_mutex_t *mutex;
  pthread_cond_t *condvar;

  int64_t *next_out;
} gen_args_t;


void *do_work(void *args) {
  gen_args_t *params = (gen_args_t*)args;

  for (int64_t i=0; ; ++i) {
    // захватываем mutex (нам ещё с ним спать)
    pthread_mutex_lock(params->mutex);
    *params->next_out = i; // имитаця важной деятельности

    // тут сигнализируем, что пора просыпаться
    pthread_cond_signal(params->condvar);

    while (*params->next_out != -1) // до тех пока у нас не забрали результат
        pthread_cond_wait(params->condvar, params->mutex); // мы спим

    // отпускаем mutex, чтоб не создавать проблем
    // после завершения работы 
    // отпускаем его тут, а не выше, т.к. выше нам с ним спать ложиться)))
    pthread_mutex_unlock(params->mutex);
  }
}


int main(int argc, char **argv) {
  // тут вроде ничего хитрого...
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

  int64_t next_out;

  gen_args_t thread_args = {
      .mutex = &mutex,
      .condvar = &condvar,
      .next_out = &next_out,
  };

  pthread_t worker;
  pthread_create(&worker, NULL, do_work, &thread_args);

  next_out = -1;
  const int N = 10;
  for (int32_t count = 0; count < N; ++count) {
    // захватываем mutex
    pthread_mutex_lock(&mutex);
      
    // если нам пока не обновили значения, то ложимся спать
    while (next_out == -1)
      pthread_cond_wait(&condvar, &mutex); // а тут mutex НЕ захвачен
    // mutex тут ЗАХВАЧЕН

    // забираем что-то ценное
    int64_t cur_out = next_out;

    // говорим, что мы это забрали
    next_out = -1;
    pthread_mutex_unlock(&mutex); // тут можем отпустить mutex
  
    // сигнализируем, что нам есть, чего ждать
    if (count != N - 1)
      pthread_cond_signal(&condvar);

    // печатаем результат
    printf("%lld\n", cur_out);
  }

  pthread_cancel(worker);

  pthread_cond_destroy(&condvar);
  pthread_mutex_destroy(&mutex);
  return 0;
}

