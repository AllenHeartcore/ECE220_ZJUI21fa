#if !defined(DOUBLE_LIST_H)
#define DOUBLE_LIST_H

typedef struct double_list_t double_list_t;
struct double_list_t {
    double_list_t* prev;
    double_list_t* next;
};

typedef enum {
    DL_CONTINUE,		// continue with next element
    DL_STOP_AND_RETURN,		// stop iteration
    DL_REMOVE_AND_CONTINUE,	// remove current element and continue
    DL_REMOVE_AND_STOP,		// remove current element and stop
    DL_FREE_AND_CONTINUE	// remove current element, free it, and continue
} dl_execute_response_t;

typedef dl_execute_response_t (*dl_execute_func_t) (void* dl, void* arg);

extern void dl_init (double_list_t* head);
extern void dl_insert (double_list_t* head, double_list_t* elt);
extern void dl_remove (double_list_t* elt);
extern void* dl_execute_on_all 
	(double_list_t* head, dl_execute_func_t func, void* arg);
extern void* dl_first (double_list_t* head);

#endif // DOUBLE_LIST_H
