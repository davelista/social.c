/**
 * Author: Davide Lista
 *
 * Version:: 1.1
 *
 * STRUTTURA: HT PER LE ENTITÀ E LE RELAZIONI
 *
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_LENGTH 50
#define ARG_LENGTH 50

/**
* DATA STRUCTURES
*/

#define RED 'R'
#define BLACK 'B'

/******************************************** STRING RB TREE *********************************************************/


/**
* rb-tree to manage strings
*/


struct string_rb_node{
    char name[ARG_LENGTH]; //key
    char node_color;
    struct string_rb_node* left;
    struct string_rb_node* right;
    struct string_rb_node* parent;
};

struct string_rb_tree {
    struct string_rb_node *root;
    struct string_rb_node *nil;
};

/************************************************ INT RB TREE**********************************************************/


/**
* int rb-tree to see which every number relations of entity destination
*/

struct num_relations_rb_node{
    int key;
    char node_color;
    int num_of_child;
    struct num_relations_rb_node* left;
    struct num_relations_rb_node* right;
    struct num_relations_rb_node* parent;
    struct string_rb_tree* entity_dest_tree;
};

struct num_relations_rb_tree {
    struct num_relations_rb_node *root;
    struct num_relations_rb_node *nil;
};

/******************************************** RHT = RelationsHashTable ************************************************/
#define RHT_KEY_T uint8_t
#define RHT_SIZE (1 << (sizeof(RHT_KEY_T) * 8)) // sizeof returns the value in BYTES of the size (READ THE FUCKING DOCUMENTATION)

// Element of the hashtable
struct rht_entry {

    uint8_t valid; //boolean to see if is valid
    char relation_name[ARG_LENGTH]; // name of the relation
    uint8_t has_collision; //boolean to see if there is a collision
    RHT_KEY_T collision_key;// key of the probable collision
    struct num_relations_rb_tree* num_rb_tree;
};

static struct rht_entry relations[RHT_SIZE];

static struct string_rb_tree* relations_alphabetized_tree; //albero delle relazioni in ordine alfabetico
/**
 * struct to connect relations index with a entity rb
 */
struct rel_entity{
    uint32_t counter_relations;
    struct string_rb_tree* entity_tree;
};

/******************************************** EHT = EntitiesHashTable *************************************************/

#define EHT_KEY_T uint32_t
#define EHT_SIZE 50000

// Element of the hashtable
struct eht_entry {
    int valid;
    char entity_name[ARG_LENGTH]; // name of the relation
    struct rel_entity* is_dest[RHT_SIZE]; //entity_tree is a orig_entity tree
    struct rel_entity* is_orig[RHT_SIZE]; //entity_tree is a dest_entity tree
    uint8_t has_collision; //boolean to see if there is a collision
    RHT_KEY_T collision_key;// key of the probable collision
};

static struct eht_entry entities[EHT_SIZE];

/******************************************* FUNCTIONS OF ENTITY RB-TREE **********************************************/

int string_rb_init_node(struct string_rb_node* n, char* name){
    strcpy(n->name, name);
}

int string_rb_init_tree(struct string_rb_tree **T) {
    (*T) = (struct string_rb_tree *) malloc(sizeof(struct string_rb_tree));
    (*T)->nil = (struct string_rb_node *) malloc(sizeof(struct string_rb_node));
    (*T)->nil->node_color = BLACK;
    (*T)->nil->left = NULL;
    (*T)->nil->right = NULL;
    (*T)->nil->parent = NULL;
    (*T)->root = (*T)->nil;
    return 1;
}

int string_rb_left_rotate(struct string_rb_tree *T, struct string_rb_node *x) {
    struct string_rb_node *y = x->right;

    x->right = y->left;

    if(y->left != T->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == T->nil) {
        T->root = y;
    } else if(x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;

    x->parent = y;
}

int string_rb_right_rotate(struct string_rb_tree *T, struct string_rb_node *x) {
    struct string_rb_node *y = x->left;

    x->left = y->right;

    if(y->right != T->nil) {
        y->right->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == T->nil) {
        T->root = y;
    } else if(x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }

    y->right = x;

    x->parent = y;
}

int string_rb_insert_fixup(struct string_rb_tree *T, struct string_rb_node *z) {
    struct string_rb_node *y;

    while(z->parent->node_color == RED) {
        if(z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;

            if(y->node_color == RED) {
                z->parent->node_color = BLACK;
                y->node_color = BLACK;
                z->parent->parent->node_color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    string_rb_left_rotate(T, z);
                }

                z->parent->node_color = BLACK;
                z->parent->parent->node_color = RED;
                string_rb_right_rotate(T, z->parent->parent);
            }
        } else {
            y = z->parent->parent->left;

            if(y->node_color == RED) {
                z->parent->node_color = BLACK;
                y->node_color = BLACK;
                z->parent->parent->node_color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    string_rb_right_rotate(T, z);
                }

                z->parent->node_color = BLACK;
                z->parent->parent->node_color = RED;
                string_rb_left_rotate(T, z->parent->parent);
            }

        }
    }

    T->root->node_color = BLACK;
}

int string_rb_insert(struct string_rb_tree *T, struct string_rb_node *z) {
    struct string_rb_node *y = T->nil;
    struct string_rb_node *x = T->root;

    while(x != T->nil) {
        y = x;
        if(strcmp(z->name, x->name) < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if(y == T->nil) {
        T->root = z;
    } else if (strcmp(z->name, y->name) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = T->nil;
    z->right = T->nil;
    z->node_color = RED;

    string_rb_insert_fixup(T, z);
}

struct string_rb_node* string_tree_minimum(struct string_rb_tree* T, struct string_rb_node* x){
    while (x->left != T->nil){
        x = x->left;
    }
    return x;
}

struct string_rb_node* string_tree_maximum(struct string_rb_tree* T, struct string_rb_node* x){
    while (x->right != T->nil){
        x = x->right;
    }
    return x;
}

struct string_rb_node* string_tree_successor(struct string_rb_tree* T, struct string_rb_node* x){
    struct string_rb_node* y;

    if (x->right != T->nil){
        x = string_tree_minimum(T, x->right);
        return x;
    }
    y = x->parent;
    while (y != T->nil && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;

}

void string_rb_transplant(struct string_rb_tree *T, struct string_rb_node *u, struct string_rb_node *v) { //ok
    if (u->parent == T->nil) { //ok
        T->root = v;//ok

    } else if (u == u->parent->left) {//ok
        u->parent->left = v;//ok
    } else {
        u->parent->right = v; //ok
    }
    v->parent = u->parent;//ok
}

void string_rb_delete_fixup(struct string_rb_tree* T, struct string_rb_node* x) {

    struct string_rb_node *w;

    while (x != T->root && x->node_color == BLACK) {//ok
        if (x == x->parent->left) {//ok
            w = x->parent->right;//ok
            if (w->node_color == RED) {//ok
                w->node_color = BLACK;//ok
                x->parent->node_color = RED;//ok
                string_rb_left_rotate(T, x->parent);//ok
                w = x->parent->right;//ok
            }
            if (w->left->node_color == BLACK && w->right->node_color == BLACK) {//ok
                w->node_color = RED;//ok
                x = x->parent;//ok
            } else {//ok
                if (w->right->node_color == BLACK) {//ok
                    w->left->node_color = BLACK;//ok
                    w->node_color = RED;//ok
                    string_rb_right_rotate(T, w);//ok
                    w = x->parent->right;//ok
                }
                w->node_color = x->parent->node_color;//ok
                x->parent->node_color = BLACK;//ok
                w->right->node_color = BLACK;//ok
                string_rb_left_rotate(T, x->parent);//ok
                x = T->root;//ok
            }
        } else {//ok
            w = x->parent->left;//ok
            if (w->node_color == RED) {//ok
                w->node_color = BLACK;//ok
                x->parent->node_color = RED;//ok
                string_rb_right_rotate(T, x->parent);//ok
                w = x->parent->left;//ok
            }
            if (w->right->node_color == BLACK && w->left->node_color == BLACK) {//ok
                w->node_color = RED;//ok
                x = x->parent;//ok
            } else {//ok
                if (w->left->node_color == BLACK) {//ok
                    w->right->node_color = BLACK;//ok
                    w->node_color = RED;//ok
                    string_rb_left_rotate(T, w);//ok
                    w = x->parent->left;//ok
                }
                w->node_color = x->parent->node_color;//ok
                x->parent->node_color = BLACK;//ok
                w->left->node_color = BLACK;//ok
                string_rb_right_rotate(T, x->parent);//ok
                x = T->root;
            }
        }
    }
    x->node_color = BLACK;
}

void string_rb_delete(struct string_rb_tree* T, struct string_rb_node* z){
    struct string_rb_node* y = z;//ok
    struct string_rb_node* x;//ok
    char y_original_color = y->node_color; //ok

    if (z->left == T->nil){ //ok
        x = z->right; //ok
        string_rb_transplant(T, z, z->right); //ok
    }
    else if(z->right == T->nil){//ok
        x = z->left;//ok
        string_rb_transplant(T, z, z->left);//ok
    }
    else {
        y = string_tree_minimum(T, z->right);//ok
        y_original_color = y->node_color;//ok
        x = y->right;//ok
        if(y->parent == z){//ok
            x->parent = y;//ok
        } else {
            string_rb_transplant(T, y, y->right);//ok
            y->right = z->right;//ok
            y->right->parent = y;//ok
        }
        string_rb_transplant(T, z, y);//ok
        y->left = z->left;//ok
        y->left->parent = y;//ok
        y->node_color = z->node_color;//ok
    }
    if (y_original_color == BLACK){//ok
        string_rb_delete_fixup(T, x);//ok
    }
    free(z);
}

struct string_rb_node* string_search_in_rb(struct string_rb_tree* T, char* name){

    struct string_rb_node *x = T->root;

    while (x != T->nil)
    {
        if (strcmp(name, x->name) == 0){
            return x;
        } else if( strcmp(name, x->name) > 0){
            x = x->right;
        } else {
            x = x->left;
        }
    }
    return NULL;
}

void string_rb_print_from_node(struct string_rb_tree* T, struct string_rb_node* root){
    struct string_rb_node* min;
    struct string_rb_node* max;
    min = string_tree_minimum(T, root);
    max = string_tree_maximum(T, root);

    while(min != max){
        fputs(min->name, stdout);
        min = string_tree_successor(T, min);
        fputs(" ", stdout);
    }
    fputs(max->name, stdout);
    fputs(" ", stdout);

}

/****************************************** FUNCTIONS OF NUM RELATIONS RB-TREE ****************************************/

int num_rel_rb_init_node(struct num_relations_rb_node* n, int key){
    n->key = key;
    n->num_of_child = 0;
    //printf("\n\nnum_rel_rb_init_node: HO INIZIALIZZATO L'ALBERO DELLE ENTITÀ");
    string_rb_init_tree(&n->entity_dest_tree);
}

int num_rel_rb_init_tree(struct num_relations_rb_tree **T) {
    (*T) = (struct num_relations_rb_tree *) malloc(sizeof(struct num_relations_rb_tree));
    (*T)->nil = (struct num_relations_rb_node *) malloc(sizeof(struct num_relations_rb_node));
    (*T)->nil->key = -1;
    (*T)->nil->node_color = BLACK;
    (*T)->nil->left = NULL;
    (*T)->nil->right = NULL;
    (*T)->nil->parent = NULL;
    (*T)->root = (*T)->nil;
    return 1;
}

int num_rel_rb_left_rotate(struct num_relations_rb_tree *T, struct num_relations_rb_node *x) {
    struct num_relations_rb_node *y = x->right;

    x->right = y->left;

    if(y->left != T->nil) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == T->nil) {
        T->root = y;
    } else if(x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    y->left = x;

    x->parent = y;
}

int num_rel_rb_right_rotate(struct num_relations_rb_tree *T, struct num_relations_rb_node *x) {
    struct num_relations_rb_node *y = x->left;

    x->left = y->right;

    if(y->right != T->nil) {
        y->right->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == T->nil) {
        T->root = y;
    } else if(x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }

    y->right = x;
    x->parent = y;
}

int num_rel_rb_insert_fixup(struct num_relations_rb_tree *T, struct num_relations_rb_node *z) {
    struct num_relations_rb_node *y;

    while(z->parent->node_color == RED) {
        if(z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;

            if(y->node_color == RED) {
                z->parent->node_color = BLACK;
                y->node_color = BLACK;
                z->parent->parent->node_color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    num_rel_rb_left_rotate(T, z);
                }

                z->parent->node_color = BLACK;
                z->parent->parent->node_color = RED;
                num_rel_rb_right_rotate(T, z->parent->parent);
            }
        } else {
            y = z->parent->parent->left;

            if(y->node_color == RED) {
                z->parent->node_color = BLACK;
                y->node_color = BLACK;
                z->parent->parent->node_color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    num_rel_rb_right_rotate(T, z);
                }

                z->parent->node_color = BLACK;
                z->parent->parent->node_color = RED;
                num_rel_rb_left_rotate(T, z->parent->parent);
            }

        }
    }

    T->root->node_color = BLACK;
}

int num_rel_rb_insert(struct num_relations_rb_tree *T, struct num_relations_rb_node *z) {
    struct num_relations_rb_node *y = T->nil;
    struct num_relations_rb_node *x = T->root;

    while(x != T->nil) {
        y = x;
        if(z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;
    if(y == T->nil) {
        T->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = T->nil;
    z->right = T->nil;
    z->node_color = RED;

    num_rel_rb_insert_fixup(T, z);
}

struct num_relations_rb_node* num_rel_tree_minimum(struct num_relations_rb_tree* T, struct num_relations_rb_node* x){
    if(T->root == T->nil){
        return NULL;
    }
    while (x->left != T->nil){
        x = x->left;
    }
    return x;
}

struct num_relations_rb_node* num_rel_tree_maximum(struct num_relations_rb_tree* T, struct num_relations_rb_node* x){
    if(T->root == T->nil){
        return NULL;
    }

    while (x->right != T->nil){
        x = x->right;
    }
    return x;
}

/*struct num_relations_rb_node* num_rel_tree_successor(struct num_relations_rb_tree* T, struct num_relations_rb_node* x){
    struct num_relations_rb_node* y;

    if (x->right != T->nil){
        x = num_rel_tree_minimum(T, x->right);
        return x;
    }
    y = x->parent;
    while (y != T->nil && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;

}*/

void num_rel_rb_transplant(struct num_relations_rb_tree *T, struct num_relations_rb_node *u, struct num_relations_rb_node *v) { //ok
    if (u->parent == T->nil) { //ok
        T->root = v;//ok

    } else if (u == u->parent->left) {//ok
        u->parent->left = v;//ok
    } else {
        u->parent->right = v; //ok
    }
    v->parent = u->parent;//ok
}

void num_rel_rb_delete_fixup(struct num_relations_rb_tree* T, struct num_relations_rb_node* x) {

    struct num_relations_rb_node *w;

    while (x != T->root && x->node_color == BLACK) {//ok
        if (x == x->parent->left) {//ok
            w = x->parent->right;//ok
            if (w->node_color == RED) {//ok
                w->node_color = BLACK;//ok
                x->parent->node_color = RED;//ok
                num_rel_rb_left_rotate(T, x->parent);//ok
                w = x->parent->right;//ok
            }
            if (w->left->node_color == BLACK && w->right->node_color == BLACK) {//ok
                w->node_color = RED;//ok
                x = x->parent;//ok
            } else {//ok
                if (w->right->node_color == BLACK) {//ok
                    w->left->node_color = BLACK;//ok
                    w->node_color = RED;//ok
                    num_rel_rb_right_rotate(T, w);//ok
                    w = x->parent->right;//ok
                }
                w->node_color = x->parent->node_color;//ok
                x->parent->node_color = BLACK;//ok
                w->right->node_color = BLACK;//ok
                num_rel_rb_left_rotate(T, x->parent);//ok
                x = T->root;//ok
            }
        } else {//ok
            w = x->parent->left;//ok
            if (w->node_color == RED) {//ok
                w->node_color = BLACK;//ok
                x->parent->node_color = RED;//ok
                num_rel_rb_right_rotate(T, x->parent);//ok
                w = x->parent->left;//ok
            }
            if (w->right->node_color == BLACK && w->left->node_color == BLACK) {//ok
                w->node_color = RED;//ok
                x = x->parent;//ok
            } else {//ok
                if (w->left->node_color == BLACK) {//ok
                    w->right->node_color = BLACK;//ok
                    w->node_color = RED;//ok
                    num_rel_rb_left_rotate(T, w);//ok
                    w = x->parent->left;//ok
                }
                w->node_color = x->parent->node_color;//ok
                x->parent->node_color = BLACK;//ok
                w->left->node_color = BLACK;//ok
                num_rel_rb_right_rotate(T, x->parent);//ok
                x = T->root;
            }
        }
    }
    x->node_color = BLACK;
}

void num_rel_rb_delete(struct num_relations_rb_tree* T, struct num_relations_rb_node* z){
    struct num_relations_rb_node* y = z;//ok
    struct num_relations_rb_node* x;//ok
    char y_original_color = y->node_color; //ok

    if (z->left == T->nil){ //ok
        x = z->right; //ok
        num_rel_rb_transplant(T, z, z->right); //ok
    }
    else if(z->right == T->nil){//ok
        x = z->left;//ok
        num_rel_rb_transplant(T, z, z->left);//ok
    }
    else {
        y = num_rel_tree_minimum(T, z->right);//ok
        y_original_color = y->node_color;//ok
        x = y->right;//ok
        if(y->parent == z){//ok
            x->parent = y;//ok
        } else {
            num_rel_rb_transplant(T, y, y->right);//ok
            y->right = z->right;//ok
            y->right->parent = y;//ok
        }
        num_rel_rb_transplant(T, z, y);//ok
        y->left = z->left;//ok
        y->left->parent = y;//ok
        y->node_color = z->node_color;//ok
    }
    if (y_original_color == BLACK){//ok
        num_rel_rb_delete_fixup(T, x);//ok
    }
    free(z);
}

struct num_relations_rb_node* num_rel_search_in_rb(struct num_relations_rb_tree* T, int key){

    struct num_relations_rb_node *x = T->root;

    while (x != T->nil)
    {
        if (key == x->key){
            return x;
        } else if(key > x->key){
            x = x->right;
        } else {
            x = x->left;
        }
    }
    return NULL;
}

/*
 * FUNCTIONS TO CONTROL
 *
 * struct num_relations_rb_node* search_max_node_in_num_rel_rb(struct num_relations_rb_tree* T){
    struct num_relations_rb_node *x = T->root;
    while (x != T->nil)
    {
        if (x->right != T->nil){
            x = x->right;
        } else
            return x;
    }
}

void num_rel_rb_print_from_node(struct num_relations_rb_tree* T, struct num_relations_rb_node* root){
    struct num_relations_rb_node* min;
    struct num_relations_rb_node* max;
    min = num_rel_tree_minimum(T, root);
    max = num_rel_tree_maximum(T, root);

    while(min != max){
        printf("%d",min->key);
        min = num_rel_tree_successor(T, min);
    }
    printf("%d",max->key);
}*/

/************************************************* RHT FUNCTIONS ******************************************************/

static RHT_KEY_T rht_str_hash(const char *z) {
    RHT_KEY_T h = 0;
    unsigned char c;
    while( (c = (unsigned char)*z++)!=0 ){
        h += c;
        h *= 0x9e3779b1;
    }
    return h;
}

int rht_init_entry(struct rht_entry *entry, uint8_t valid) {
    entry->valid = valid;
    entry->relation_name[0] = '\0';
    entry->collision_key = 0;
    entry->has_collision = 0;
}

int rht_init_table(struct rht_entry *table, int size) {
    int i = 0;
    for(i = 0; i < size; i++) {
        rht_init_entry(&table[i], 0);
    }
    return 1;
}

int rht_search_first_free(struct rht_entry *table, int size, RHT_KEY_T *key, RHT_KEY_T current_key) {
    int i = 0;
    for(i = size/2; i < size; i++) {
        if(table[i].valid == 0 && i != current_key) {
            *key = i;
            return 1;
        }
    }

    for(i = 0; i < size/2; i++) {
        if(table[i].valid == 0 && i != current_key) {
            *key = i;
            return 1;
        }
    }

    // No free slot found.
    return 0;
}

/**
* 1: OK
* -1: name already present
* -2: full table
*/
int rht_insert( struct rht_entry *table, int size, char *relation_name) {
    RHT_KEY_T hash = rht_str_hash(relation_name);
    struct string_rb_node* new_rel_node = (struct string_rb_node*) malloc(sizeof(struct string_rb_node));

    if (table[hash].valid == 0){
        table[hash].valid = 1;
        table[hash].has_collision = 0;
        strcpy(table[hash].relation_name, relation_name);
        num_rel_rb_init_tree(&table[hash].num_rb_tree);//inizializzo l'albero num_rb_tree

        string_rb_init_node(new_rel_node, relation_name);
        string_rb_insert(relations_alphabetized_tree, new_rel_node); //inserisco la rel anche nell'albero ordinato delle relazioni
        return 1;
    } else {

        if(strcmp(relation_name, table[hash].relation_name) == 0) {
            // A relation with the same name is already present in the table
            return -1;
        }
        while (table[hash].has_collision) {
            if (strcmp(relation_name, table[hash].relation_name) == 0) {
                // A relation with the same name is already present in the table
                return -1;
            }
            hash = table[hash].collision_key;
        }
        RHT_KEY_T new_index;

        if(rht_search_first_free(table, size, &new_index, hash) == 0) {
            return -2;
        }

        table[hash].collision_key = new_index;

        table[new_index].valid = 1;
        table[new_index].has_collision = 0;
        strcpy(table[new_index].relation_name, relation_name);
        num_rel_rb_init_tree(&table[new_index].num_rb_tree); //inizializzo l'albero num_rb_tree

        string_rb_init_node(new_rel_node, relation_name);
        string_rb_insert(relations_alphabetized_tree, new_rel_node);

        return 1;
    }
}

/**
* 1: OK
* -1: KO
*/

int rht_find_key(struct rht_entry *table, char *name, RHT_KEY_T *key) {
    RHT_KEY_T hash = rht_str_hash(name);

    while (strcmp(name, table[hash].relation_name) != 0) {
        if (!table[hash].valid) {
            return -1;
        }
        hash = table[hash].collision_key;
    }

    *key = hash;
    return 1;
}

int rht_find(struct rht_entry *table, char *name) {
    RHT_KEY_T key;

    if(rht_find_key(table, name, &key)==1) {
        return 1;
    }else{
        return -1;
    }

}

void rht_print_table(struct rht_entry *table, int size) {
    int i = 0;
    for(i = 0; i < size; i++) {
        printf("%d\t%s\n", i, table[i].relation_name);
    }
}

/************************************************* EHT FUNCTIONS ******************************************************/

static EHT_KEY_T eht_str_hash(const char *z) {
    EHT_KEY_T h = 0;
    unsigned char c;
    while( (c = (unsigned char)*z++)!=0 ){
        h += c;
        h *= 0x9e3779b1;
    }
    return h%EHT_SIZE;
}

int eht_search_first_free(struct eht_entry *table, int size, EHT_KEY_T *key, EHT_KEY_T current_key) {
    int i = 0;
    for(i = size/2; i < size; i++) {
        if(table[i].entity_name[0] == '\0' && i != current_key) {
            *key = i;
            return 1;
        }
    }

    for(i = 0; i < size/2; i++) {
        if(table[i].entity_name[0] == '\0' && i != current_key) {
            *key = i;
            return 1;
        }
    }

    // No free slot found.
    return 0;
}

/**
* 1: OK
* -1: name already present
* -2: full table
*/
int eht_insert( struct eht_entry *table, int size, char *entity_name) {
    EHT_KEY_T hash = eht_str_hash(entity_name);

    if (table[hash].entity_name[0] == '\0'){

        table[hash].has_collision = 0;
        table[hash].valid = 1;
        strcpy(table[hash].entity_name, entity_name);
        for(int i = 0; i < RHT_SIZE; i++){ //RHT PERCHÉ counter_relations HA LUNGHEZZA RHT
            table[hash].is_dest[i] = (struct rel_entity*)malloc(sizeof(struct rel_entity));
            table[hash].is_orig[i] = (struct rel_entity*)malloc(sizeof(struct rel_entity));
            table[hash].is_dest[i]->counter_relations=0;
            table[hash].is_orig[i]->counter_relations=0;
        }

        return 1;
    } else {

        if(strcmp(entity_name, table[hash].entity_name) == 0) {
            // A relation with the same name is already present in the table
            if(table[hash].valid == 1){
                return -1;
            } else{
                table[hash].valid = 1;
                return 1;
            }

        }
        while (table[hash].has_collision) {
            if (strcmp(entity_name, table[hash].entity_name) == 0) {
                // A relation with the same name is already present in the table
                return -1;
            }
            hash = table[hash].collision_key;
        }
        EHT_KEY_T new_index;

        if(eht_search_first_free(table, size, &new_index, hash) == 0) {
            return -2;
        }

        table[hash].collision_key = new_index;
        table[new_index].has_collision = 0;
        table[new_index].valid = 1;
        strcpy(table[new_index].entity_name, entity_name);

        for(int i = 0; i < RHT_SIZE; i++){
            table[new_index].is_dest[i]->counter_relations=0;
            table[new_index].is_orig[i]->counter_relations=0;
        }
        return 1;
    }
}

/**
* 1: OK
* -1: KO
 * 2 IS_DELETE
*/
#define IS_DELETE 2
int eht_find_key(struct eht_entry *table, char *name, EHT_KEY_T *key) {
    EHT_KEY_T hash = eht_str_hash(name);

    while (strcmp(name, table[hash].entity_name) != 0) {
        if (table[hash].entity_name[0] == '\0') {
            return -1;
        }
        hash = table[hash].collision_key;
    }

    if (table[hash].valid == 0) {
        return IS_DELETE;
    }
    *key = hash;
    return 1;
}

int eht_find(struct eht_entry *table, char *name) {
    EHT_KEY_T key;

    if(eht_find_key(table, name, &key)==1) {
        return 1;
    }else{
        return -1;
    }

}

void eht_print_table(struct eht_entry *table, int size) {
    int i = 0;
    for(i = 0; i < size; i++) {
        if(table[i].entity_name[0] != '\0'){
            printf("%d|\t%s\t|%d\t|%d\n", i, table[i].entity_name, table[i].has_collision, table[i].collision_key);
        }
    }
}

/************************************************* COMMAND PARSER *****************************************************/

enum available_command {
    ADD_ENTITY,
    DELETE_ENTITY,
    ADD_RELATION,
    DELETE_RELATION,
    REPORT,
    END,
    UNKNOWN,
};

struct parsed_command {
    enum available_command command;
    char first_arg[ARG_LENGTH];
    char second_arg[ARG_LENGTH];
    char third_arg[ARG_LENGTH];
};

void init_parsed_command(struct parsed_command *pc) {
    pc->command = UNKNOWN;
    pc->first_arg[0] = '\0';
    pc->second_arg[0] = '\0';
    pc->third_arg[0] = '\0';
}

int addrel(char* e_orig, char* e_dest, char* rel){

    if(eht_find(entities, e_orig) == 1 && eht_find(entities, e_dest) == 1) {//se esistono le entità
        if (rht_find(relations, rel) != 1) {//se non esiste la relazione
            rht_insert(relations, RHT_SIZE, rel); //la inserisco sia nella hash table che nell'albero delle relazioni
        }

        EHT_KEY_T hash_e1, hash_e2;
        RHT_KEY_T hash_rel;

        /*mi salvo gli indirizzi hash*/
        rht_find_key(relations, rel, &hash_rel);
        eht_find_key(entities, e_orig, &hash_e1);
        eht_find_key(entities, e_dest, &hash_e2);
        if(relations[hash_rel].valid == 0){
            struct string_rb_node* rel_to_add_tree = (struct string_rb_node*) malloc(sizeof(struct string_rb_node));
            string_rb_init_node(rel_to_add_tree, rel);
            string_rb_insert(relations_alphabetized_tree, rel_to_add_tree);
            relations[hash_rel].valid = 1;
        }

        struct string_rb_node* exist_dest, *exist_orig;
        struct string_rb_node *dest_to_insert_in_orig, *orig_to_insert_in_dest;
        struct string_rb_node *dest_to_insert_in_rel, *dest_to_delete_in_rel;
        struct num_relations_rb_node *node_old_counter, *node_new_counter;

        if (entities[hash_e1].is_orig[hash_rel]->counter_relations != 0) {//se esiste quella relazione per e1
            exist_dest = string_search_in_rb(entities[hash_e1].is_orig[hash_rel]->entity_tree, e_dest);
            if (exist_dest != NULL) {//se esiste già una relazione di quel tipo con e2 allora esco
                return 0;
            }
        } else {//altrimenti
            string_rb_init_tree(
                    &entities[hash_e1].is_orig[hash_rel]->entity_tree); //inizializzo l'albero delle entità destinatarie
        }

        dest_to_insert_in_orig = (struct string_rb_node *) malloc((sizeof(struct string_rb_node))); //allooco memoria
        string_rb_init_node(dest_to_insert_in_orig, e_dest); //inizializzo il nodo
        string_rb_insert(entities[hash_e1].is_orig[hash_rel]->entity_tree, dest_to_insert_in_orig);//lo inserisco in e1
        entities[hash_e1].is_orig[hash_rel]->counter_relations++;//aumento il contatore

        if (entities[hash_e2].is_dest[hash_rel]->counter_relations != 0) { //se esiste già una relazione di quel tipo
            node_old_counter = num_rel_search_in_rb(relations[hash_rel].num_rb_tree, entities[hash_e2].is_dest[hash_rel]->counter_relations);
            dest_to_delete_in_rel = string_search_in_rb(node_old_counter->entity_dest_tree, e_dest);
            if (dest_to_delete_in_rel != NULL) {//controllo altrimenti la delete impazzisce se è null
                string_rb_delete(node_old_counter->entity_dest_tree, dest_to_delete_in_rel);
                node_old_counter->num_of_child--;//ho abbassato di uno il numero delle entità all'interno di quel nodo
            }
        } else {
            string_rb_init_tree(&entities[hash_e2].is_dest[hash_rel]->entity_tree);//altrimenti inizializzo l'albero
        }

        orig_to_insert_in_dest = (struct string_rb_node *) malloc(sizeof(struct string_rb_node));
        string_rb_init_node(orig_to_insert_in_dest, e_orig);
        string_rb_insert(entities[hash_e2].is_dest[hash_rel]->entity_tree, orig_to_insert_in_dest);//inserisco e1
        entities[hash_e2].is_dest[hash_rel]->counter_relations++;//aggiorno il numero di relazione

        dest_to_insert_in_rel = (struct string_rb_node *) malloc(sizeof(struct string_rb_node));
        string_rb_init_node(dest_to_insert_in_rel, e_dest); //inizializzo il nodo da inserire all'interno dell'albero int
        node_new_counter = num_rel_search_in_rb(relations[hash_rel].num_rb_tree, entities[hash_e2].is_dest[hash_rel]->counter_relations); //cerco il nodo col nuovo counter
        if (node_new_counter == NULL) { //se non esiste
            node_new_counter = (struct num_relations_rb_node *) malloc(sizeof(struct num_relations_rb_node));
            num_rel_rb_init_node(node_new_counter, entities[hash_e2].is_dest[hash_rel]->counter_relations);//inizializzo
            string_rb_insert(node_new_counter->entity_dest_tree, dest_to_insert_in_rel);//inserisco l'entità
            node_new_counter->num_of_child++;//aggiorno il num dei figli
            num_rel_rb_insert(relations[hash_rel].num_rb_tree, node_new_counter);//inserisco il nodo dentro l'albero int
        } else {//altrimenti
            string_rb_insert(node_new_counter->entity_dest_tree, dest_to_insert_in_rel);//inserisco l'entità
            node_new_counter->num_of_child++;//aggiorno il num dei figli
        }
    } else return 0;

}

int delrel(char* e_orig, char* e_dest, char* rel){
    if(rht_find(relations, rel) == 1 && eht_find(entities, e_orig) == 1 && eht_find(entities, e_dest) == 1){//se non esistono uno dei 3 valori esco

        EHT_KEY_T hash_e1, hash_e2;
        RHT_KEY_T hash_rel;

        /*mi salvo gli indirizzi hash*/
        rht_find_key(relations, rel, &hash_rel);
        eht_find_key(entities, e_orig, &hash_e1);
        eht_find_key(entities, e_dest, &hash_e2);

        if(relations[hash_rel].valid != 0){

            struct string_rb_node* dest_to_del_from_orig, *orig_to_del_from_dest;
            struct string_rb_node* dest_to_del_from_rel, *new_dest_to_add_from_rel;
            struct string_rb_node* rel_to_delete_from_tree;
            struct num_relations_rb_node *node_old_counter, *node_new_counter;
            if(entities[hash_e1].is_orig[hash_rel]->counter_relations != 0 && entities[hash_e2].is_dest[hash_rel]->counter_relations != 0){

                dest_to_del_from_orig = string_search_in_rb(entities[hash_e1].is_orig[hash_rel]->entity_tree, e_dest);//e2 in e1
                orig_to_del_from_dest = string_search_in_rb(entities[hash_e2].is_dest[hash_rel]->entity_tree, e_orig);//e1 in e2

                if(dest_to_del_from_orig != NULL && orig_to_del_from_dest != NULL) { //se e1 e e2 sono legate dalla relazione

                    string_rb_delete(entities[hash_e1].is_orig[hash_rel]->entity_tree, dest_to_del_from_orig);//elimino e2 da e1
                    entities[hash_e1].is_orig[hash_rel]->counter_relations--;//decremento il contatore

                    node_old_counter = num_rel_search_in_rb(relations[hash_rel].num_rb_tree,  entities[hash_e2].is_dest[hash_rel]->counter_relations);//cerco il nodo rispettivo del contatore da aggiornare

                    dest_to_del_from_rel = string_search_in_rb(node_old_counter->entity_dest_tree, e_dest);
                    if(dest_to_del_from_rel != NULL){//DOVREBBE SEMPRE ESSERE VERA
                        string_rb_delete(node_old_counter->entity_dest_tree, dest_to_del_from_rel);//elimino e2
                        node_old_counter->num_of_child--;
                        if(node_old_counter->entity_dest_tree->root == node_old_counter->entity_dest_tree->nil){//se il nodo non ha più figli
                            num_rel_rb_delete(relations[hash_rel].num_rb_tree, node_old_counter);//lo elimino
                        }
                        string_rb_delete(entities[hash_e2].is_dest[hash_rel]->entity_tree, orig_to_del_from_dest); //elimino e1 da e2
                        entities[hash_e2].is_dest[hash_rel]->counter_relations--;//decremento il contatore

                        if(entities[hash_e2].is_dest[hash_rel]->counter_relations == 0){
                            if(relations[hash_rel].num_rb_tree->root == relations[hash_rel].num_rb_tree->nil){//se la rel non ha entità dest (root = nil)
                                rel_to_delete_from_tree = string_search_in_rb(relations_alphabetized_tree, rel);//lo cerco nell'albero delle rel ordinate
                                string_rb_delete(relations_alphabetized_tree, rel_to_delete_from_tree);//lo elimino
                                relations[hash_rel].valid = 0;

                                return 1;
                            }
                        } else{
                            node_new_counter = num_rel_search_in_rb(relations[hash_rel].num_rb_tree, entities[hash_e2].is_dest[hash_rel]->counter_relations);
                            if(node_new_counter == NULL){
                                node_new_counter = (struct num_relations_rb_node*) malloc(sizeof(struct num_relations_rb_node));
                                num_rel_rb_init_node(node_new_counter, entities[hash_e2].is_dest[hash_rel]->counter_relations);
                                num_rel_rb_insert(relations[hash_rel].num_rb_tree, node_new_counter);
                            }
                            new_dest_to_add_from_rel = (struct string_rb_node*) malloc(sizeof(struct string_rb_node));
                            string_rb_init_node(new_dest_to_add_from_rel, e_dest);
                            string_rb_insert(node_new_counter->entity_dest_tree, new_dest_to_add_from_rel);
                            node_new_counter->num_of_child++;
                            return 1;
                        }
                    } else return 0;

                } else return 0;
            } else return 0;

        } else return 0;

    }else return 0;
}

int delent(char* entity){
    if(eht_find(entities, entity) == 1){
        char e_dest[ARG_LENGTH];
        char e_orig[ARG_LENGTH];
        EHT_KEY_T hash_e1;

        eht_find_key(entities, entity, &hash_e1);
        for(int i = 0; i < RHT_SIZE; i++){
            while(entities[hash_e1].is_orig[i]->counter_relations != 0){
                strcpy(e_dest, entities[hash_e1].is_orig[i]->entity_tree->root->name);
                delrel(entity, e_dest, relations[i].relation_name);
                e_dest[0] = '\0';

            }
            while(entities[hash_e1].is_dest[i]->counter_relations != 0){
                strcpy(e_orig, entities[hash_e1].is_dest[i]->entity_tree->root->name);
                delrel(e_orig, entity, relations[i].relation_name);
                e_orig[0] = '\0';
            }
        }
        entities[hash_e1].valid = 0;
    }else return 0;
}

struct parsed_command parse_command_line(const char *line) {
    struct parsed_command pc;
    struct string_rb_node* e1, *e2;
    char command_str[COMMAND_LENGTH];

    init_parsed_command(&pc);

    sscanf(line, "%s %s %s %s", command_str, pc.first_arg, pc.second_arg, pc.third_arg);

    /*ADDREL*/
    if(strcmp(command_str, "addent") == 0) {
        if(pc.first_arg[0] != '\0') {
            eht_insert(entities, EHT_SIZE, pc.first_arg);
            pc.command = ADD_ENTITY;

            return pc;
        }
    }
        /*DELENT*/
    else if(strcmp(command_str, "delent") == 0) {
        if(pc.first_arg[0] != '\0') {
            delent(pc.first_arg);
            pc.command = DELETE_ENTITY;
            return pc;
        }
    }
        /*ADDREL*/
    else if(strcmp(command_str, "addrel") == 0) {
        if( pc.first_arg[0] != '\0' && pc.second_arg[0] != '\0' && pc.third_arg[0] != '\0') {
            addrel(pc.first_arg, pc.second_arg, pc.third_arg);
            pc.command = ADD_RELATION;
            return pc;
        }
    }
        /*DELREL*/
    else if(strcmp(command_str, "delrel") == 0) {
        if( pc.first_arg[0] != '\0' && pc.second_arg[0] != '\0' && pc.third_arg[0] != '\0') {
            delrel(pc.first_arg, pc.second_arg, pc.third_arg);
            pc.command = DELETE_RELATION;
            return pc;
        }

    }
        /*REPORT*/
    else if(strcmp(command_str, "report") == 0) {

        if(relations_alphabetized_tree->root == relations_alphabetized_tree->nil){
            fputs("none", stdout);
            fputs("\n", stdout);
            return pc;
        } else{
            struct string_rb_node* min = string_tree_minimum(relations_alphabetized_tree, relations_alphabetized_tree->root);
            struct string_rb_node* max = string_tree_maximum(relations_alphabetized_tree, relations_alphabetized_tree->root);
            struct num_relations_rb_node* max_number;
            RHT_KEY_T hash_rel;
            while (min != max){
                rht_find_key(relations, min->name, &hash_rel);
                max_number = num_rel_tree_maximum(relations[hash_rel].num_rb_tree, relations[hash_rel].num_rb_tree->root);
                fputs(min->name, stdout);
                fputs(" ", stdout);
                string_rb_print_from_node(max_number->entity_dest_tree, max_number->entity_dest_tree->root);
                printf("%d; ", max_number->key);
                min = string_tree_successor(relations_alphabetized_tree, min);

            }
            rht_find_key(relations, min->name, &hash_rel);
            max_number = num_rel_tree_maximum(relations[hash_rel].num_rb_tree, relations[hash_rel].num_rb_tree->root);
            //printf("IL NODO PIÙ GRANDE È %d", max_number->key);
            fputs(min->name, stdout);
            fputs(" ", stdout);
            if(max_number->entity_dest_tree->root != max_number->entity_dest_tree->nil && max_number->key > 0){
                string_rb_print_from_node(max_number->entity_dest_tree, max_number->entity_dest_tree->root);
                printf("%d;", max_number->key);
                fputs("\n", stdout);
            }
            pc.command = REPORT;
            return pc;
        }



    }
        /*END*/
    else if(strcmp(command_str, "end") == 0) {
        pc.command = END;
        return pc;
    }

    init_parsed_command(&pc);
    return pc;
}



#define COMMAND_LINE_LENGTH (COMMAND_LENGTH+3*ARG_LENGTH+4)
int main() {
    string_rb_init_tree(&relations_alphabetized_tree); //inizializzo l'albero delle relazioni in ordine alfabetico

    rht_init_table(relations, RHT_SIZE);

    char line_read[COMMAND_LINE_LENGTH];
    line_read[0] = '\0';
    struct parsed_command current_command;
    init_parsed_command(&current_command);
    while(current_command.command != END){
        fgets(line_read, COMMAND_LINE_LENGTH, stdin);
        current_command = parse_command_line(line_read);
    }
    //printf("%lu", sizeof(entities));
    return 0;
}