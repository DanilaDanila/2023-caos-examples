

struct list_head_t {
    struct list_head_t *next;

    int value;
}

list_head_t *my_list = init_list();


mutex.lock();
list_push(my_list, 123);
mutex.unlock();


CAS

// global
list_head_t *my_list = ...


// local
list_head_t *list_top = my_list;
list_

// CAS -> True
// CAS -> False
//
// compare_exchange_weak()

// append-only list
// persistent
whie (CAS(&(list_top->next), NULL, new_element) != True) {
    list_top = list_top -> next;
}




