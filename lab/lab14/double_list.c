#include <stdio.h>
#include <stdlib.h>

#include "double_list.h"

void
dl_init (double_list_t* head)
{
    head->prev = head->next = head;
}

void 
dl_insert (double_list_t* head, double_list_t* elt)
{
    elt->next = head->next;
    elt->prev = head;
    head->next->prev = elt;
    head->next = elt;
}

void 
dl_remove (double_list_t* elt)
{
    elt->prev->next = elt->next;
    elt->next->prev = elt->prev;
}

void*
dl_execute_on_all (double_list_t* head, dl_execute_func_t func, void* arg)
{
    double_list_t* dl;
    double_list_t* remove;
    dl_execute_response_t result;

    for (dl = head->next; head != dl; dl = dl->next) {
        switch ((result = func (dl, arg))) {
	    case DL_REMOVE_AND_STOP:
	    	dl_remove (dl);
	        // Fall through to next case to return dl.
	    case DL_STOP_AND_RETURN:
	    	return dl;
	    case DL_REMOVE_AND_CONTINUE:
	    case DL_FREE_AND_CONTINUE:
		remove = dl;
		// After removal, dl's predecessor's next will point
		// to dl's next before removal, so update in loop
		// will operate correctly.
	    	dl = dl->prev;
		dl_remove (remove);
		if (DL_FREE_AND_CONTINUE == result) {
		    free (remove);
		}
		break;
	    default: // DL_CONTINUE case
		break;
	}
    }
    return NULL;
}

void*
dl_first (double_list_t* head)
{
    return (head == head->next ? NULL : head->next);
}

