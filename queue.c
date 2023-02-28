#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, l) {
        element_t *e = container_of(node, element_t, list);
        free(e->value);
        free(e);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return 0;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return 0;
    e->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!e->value) {
        free(e);
        return 0;
    }
    memcpy(e->value, s, strlen(s) + 1);
    list_add(&e->list, head);
    return 1;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return 0;
    element_t *e = malloc(sizeof(element_t));
    if (!e)
        return 0;
    e->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!e->value) {
        free(e);
        return 0;
    }
    memcpy(e->value, s, strlen(s) + 1);
    list_add_tail(&e->list, head);
    return 1;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp && bufsize) {
        size_t len =
            bufsize > strlen(e->value) + 1 ? strlen(e->value) + 1 : bufsize;
        memcpy(sp, e->value, len);
        sp[len - 1] = '\0';
    }
    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *e = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp && bufsize) {
        size_t len =
            bufsize > strlen(e->value) + 1 ? strlen(e->value) + 1 : bufsize;
        memcpy(sp, e->value, len);
        sp[len - 1] = '\0';
    }
    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head) || head->next->next == head)
        return false;
    struct list_head *node, *mid = head->next;
    list_for_each (node, head) {
        if (node->next != head) {
            node = node->next;
            mid = mid->next;
        }
    }
    list_del(mid);
    element_t *e = list_entry(mid, element_t, list);
    free(e->value);
    free(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return 0;
    element_t *e, *safe;
    list_for_each_entry_safe (e, safe, head, list) {
        if (&safe->list != head && strcmp(safe->value, e->value) == 0) {
            do {
                safe = list_entry(safe->list.next, element_t, list);
            } while (&safe->list != head && strcmp(safe->value, e->value) == 0);
            e->list.prev->next = &safe->list;
            safe->list.prev = e->list.prev;
            while (e != safe) {
                element_t *tmp = list_entry(e->list.next, element_t, list);
                q_release_element(e);
                e = tmp;
            }
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *node = head->next, *adj_node = head->next->next;
    while (node != head && adj_node != head) {
        node->next = adj_node->next;
        node->next->prev = node;
        adj_node->prev = node->prev;
        adj_node->prev->next = adj_node;
        adj_node->next = node;
        node->prev = adj_node;
        node = node->next;
        adj_node = node->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *prev = head->prev, *next, *cur = head;
    list_for_each (next, head) {
        cur->next = prev;
        cur->prev = next;
        prev = cur;
        cur = next;
    }
    cur->next = prev;
    cur->prev = next;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    int counter = 0;
    struct list_head *node, *safe, sub_list_head, *tmp_head = head;
    list_for_each_safe (node, safe, head) {
        counter++;
        if (counter == k) {
            list_cut_position(&sub_list_head, tmp_head, node);
            q_reverse(&sub_list_head);
            list_splice_init(&sub_list_head, tmp_head);
            counter = 0;
            tmp_head = safe->prev;
        }
    }
}

struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; l1 && l2; *node = (*node)->next) {
        char *v1 = list_entry(l1, element_t, list)->value,
             *v2 = list_entry(l2, element_t, list)->value;
        node = (strcasecmp(v1, v2) <= 0) ? &l1 : &l2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    if (l1 == NULL)
        *ptr = l2;
    else
        *ptr = l1;
    return head;
}

struct list_head *find_mid(struct list_head *head)
{
    if (!head)
        return NULL;
    struct list_head *node, *mid = head;
    for (node = head; node != NULL; node = node->next) {
        if (node->next != NULL) {
            node = node->next;
            mid = mid->next;
        }
    }
    return mid;
}

struct list_head *merge_sort(struct list_head *head)
{
    if (!head)
        return NULL;
    struct list_head *mid = find_mid(head), *ll = head, *rl = mid;
    if (mid == head)
        return head;
    mid->prev->next = NULL;
    ll = merge_sort(ll);
    rl = merge_sort(rl);
    return merge(ll, rl);
}
/* *
 * Sort elements of queue in ascending order
 */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *single_list_head = head->next;
    head->prev->next = NULL;
    struct list_head *sorted = merge_sort(single_list_head);
    struct list_head *ptr, *next_ptr;
    for (ptr = sorted, next_ptr = sorted->next; next_ptr != NULL;
         ptr = next_ptr, next_ptr = next_ptr->next) {
        next_ptr->prev = ptr;
    }
    head->next = sorted;
    sorted->prev = head;
    head->prev = ptr;
    ptr->next = head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *node, *safe;
    char *max_val = NULL;
    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        if (max_val == NULL) {
            max_val = list_entry(node, element_t, list)->value;
            continue;
        }
        int ret = strcmp(max_val, list_entry(node, element_t, list)->value);
        if (ret < 0) {
            max_val = list_entry(node, element_t, list)->value;
        } else {
            list_del(node);
            element_t *e = list_entry(node, element_t, list);
            free(e->value);
            free(e);
        }
    }
    return q_size(head);
}

struct list_head *merge_two_list(struct list_head *l1, struct list_head *l2)
{
    struct list_head *f1 = l1;
    struct list_head *f2 = l2->next;
    while (f1->next != l1 && !list_empty(l2)) {
        char *s1 = list_entry(f1->next, element_t, list)->value;
        char *s2 = list_entry(l2->next, element_t, list)->value;
        if (strcmp(s1, s2) > 0) {
            list_del(f2);
            list_add(f2, f1);
            f2 = l2->next;
        }
        f1 = f1->next;
    }
    if (!list_empty(l2)) {
        list_splice_tail_init(l2, l1);
    }

    return l1;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;
    struct list_head *node, *back = head->prev;
    while (head->next != back) {
        for (node = (head)->next; node != (back); node = node->next) {
            queue_contex_t *q1 = list_entry(node, queue_contex_t, chain);
            queue_contex_t *q2 = list_entry(back, queue_contex_t, chain);
            if (q1 == q2)
                break;
            q1->q = merge_two_list(q1->q, q2->q);
            back = back->prev;
            if (back == node)
                break;
        }
    }
    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}
