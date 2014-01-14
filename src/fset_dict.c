#include <stdlib.h>

#include "fset_dict.h"

fset_node_t *_fset_node_create(const wchar_t *name){
    fset_node_t *node = malloc(sizeof(fset_node_t));
    node->data = fset_create();
    node->next = NULL;
    wcsncpy(node->name, name, 31);
    return node;
}

void _fset_node_delete(fset_node_t *node){
    fset_delete(node->data);
    free(node);
}

fset_dict_t *fset_dict_create(void){
    fset_dict_t *temp = malloc(sizeof(fset_dict_t));
    temp->head = NULL;
    return temp;
}

fset_t *fset_dict_get(fset_dict_t *dict, const wchar_t *name){
    if(dict->head == NULL){
        dict->head = _fset_node_create(name);
        return dict->head->data;
    }

    fset_node_t *iter = dict->head;
    
    while(iter != NULL){
        if(wcscmp(name, iter->name) == 0){
            return iter->data;
        }else if(iter->next == NULL){
            iter->next = _fset_node_create(name);
            return iter->next->data;
        }else{
            iter = iter->next;
        }
    }

    return NULL; // Should not reach this unless shenanigans are afoot.
}

void fset_dict_delete(fset_dict_t *dict){
    fset_node_t *iter = dict->head;
    fset_node_t *next = NULL;
    
    while(iter != NULL){
        next = iter->next;
        _fset_node_delete(iter);
        iter = next;
    }
    
    free(dict);
}

int32_t fset_dict_length(fset_dict_t *dict){
    fset_node_t *iter = dict->head;

    int32_t count = 0;
    while(iter != NULL){
        count += 1;
        iter = iter->next;
    }
    return count;
}

