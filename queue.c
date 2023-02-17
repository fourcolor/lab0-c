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
    if (!head)
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *e = container_of(node, element_t, list);
        free(e->value);
        free(e);
    }
    free(head);
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
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
