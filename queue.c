#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));

    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *cur, *safe;

    list_for_each_safe (cur, safe, head) {
        list_del(cur);
        element_t *node = list_entry(cur, element_t, list);
        free(node->value);
        free(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);

    int len = strlen(s) + 1;
    new->value = malloc(sizeof(char) * len);
    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, len);
    list_add(&new->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    INIT_LIST_HEAD(&new->list);

    int len = strlen(s) + 1;
    new->value = malloc(sizeof(char) * len);
    if (!new->value) {
        free(new);
        return false;
    }

    strncpy(new->value, s, len);
    list_add_tail(&new->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_first_entry(head, element_t, list);

    if (sp && node->value) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&node->list);

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_last_entry(head, element_t, list);

    if (sp && node->value) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(&node->list);

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;

    return len;
}

void q_delete_node(element_t *node)
{
    list_del(&node->list);
    if (node->value)
        free(node->value);
    free(node);
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head;
         fast = fast->next->next, slow = slow->next)
        ;

    q_delete_node(list_entry(slow, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    bool found_dup = false;
    element_t *node = NULL, *safe = NULL;

    list_for_each_entry_safe (node, safe, head, list) {
        if (!node->value || !safe->value)
            continue;

        if (&safe->list != head && strcmp(node->value, safe->value) == 0) {
            found_dup = true;
            q_delete_node(node);
        } else if (found_dup) {
            found_dup = false;
            q_delete_node(node);
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    struct list_head *cur = head->next;

    while (cur != head && cur->next != head) {
        list_move(cur, cur->next);
        cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *safe;

    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    struct list_head *node = head->next, *next = head, *start = head;
    int len = q_size(head);

    for (int i = 0; i + k < len; i += k) {
        for (int j = 0; j < k; j++) {
            next = node->next;
            list_move(node, start);
            node = next;
        }
        start = next->prev;
    }
}

/* Merge two lists in ascending order*/
struct list_head *merge_two(struct list_head *l1,
                            struct list_head *l2,
                            bool descend)
{
    struct list_head merged;
    INIT_LIST_HEAD(&merged);

    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *node_1 = list_first_entry(l1, element_t, list);
        element_t *node_2 = list_first_entry(l2, element_t, list);

        if ((descend && strcmp(node_1->value, node_2->value) > 0) ||
            (!descend && strcmp(node_1->value, node_2->value) <= 0)) {
            list_del(&node_1->list);
            list_add_tail(&node_1->list, &merged);
        } else {
            list_del(&node_2->list);
            list_add_tail(&node_2->list, &merged);
        }
    }

    if (!list_empty(l1)) {
        list_splice(&merged, l1);
        return l1;
    } else {
        list_splice(&merged, l2);
        return l2;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head l1, l2;
    INIT_LIST_HEAD(&l1);
    INIT_LIST_HEAD(&l2);

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_cut_position(&l1, head, slow->prev);
    list_splice_tail_init(head, &l2);

    q_sort(&l1, descend);
    q_sort(&l2, descend);

    INIT_LIST_HEAD(head);
    struct list_head *sorted = merge_two(&l1, &l2, descend);
    list_splice_tail(sorted, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;

    int count = 0;
    const char *biggest = NULL;
    struct list_head *node = NULL, *safe = NULL;

    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        element_t *cur = list_entry(node, element_t, list);
        if (!biggest || strcmp(biggest, cur->value) >= 0) {
            biggest = cur->value;
            count++;
        } else
            q_delete_node(cur);
    }

    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;

    int count = 0;
    const char *biggest = NULL;
    struct list_head *node = NULL, *safe = NULL;

    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        element_t *cur = list_entry(node, element_t, list);
        if (!biggest || strcmp(biggest, cur->value) <= 0) {
            biggest = cur->value;
            count++;
        } else
            q_delete_node(cur);
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
