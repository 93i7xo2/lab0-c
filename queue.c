#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->count = 0;
        q->last_element = q->head;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Free queue structure */
    if (!q)
        return;

    while (q->head) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    /* Create a new element. */
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    int len = strlen(s);
    if (!newh)
        return false;
    newh->value = (char *) malloc(len + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }
    strncpy(newh->value, (const char *) s, len);
    newh->value[len] = '\0';

    /* Attach new element at the head of linked list. */
    newh->next = q->head;
    q->head = newh;
    q->count++;

    /* Mark it as last element if it's a first element. */
    if (!newh->next)
        q->last_element = newh;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    /* Create a new element. */
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    int len = strlen(s);
    if (!newh)
        return false;
    newh->value = (char *) malloc(len + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }
    strncpy(newh->value, (const char *) s, len);
    newh->value[len] = '\0';
    newh->next = NULL;

    /* Insert element at tail of queue, with time complexity of O(1) */
    if (!q->last_element) {
        q->head = q->last_element = newh;
    } else {
        q->last_element->next = newh;
        q->last_element = newh;
    }
    q->count++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;
    if (sp) {
        int realBufsize = strlen(q->head->value) < bufsize - 1
                              ? strlen(q->head->value)
                              : bufsize - 1;
        memcpy((void *) sp, (const void *) q->head->value, realBufsize);
        *(sp + realBufsize) = '\0';
    }
    list_ele_t *target = q->head;
    /* May be bufsize is greater than actual size of value */
    q->head = q->head->next;
    q->count--;
    free(target->value);
    free(target);

    /* Mark last element as NULL if queue is empty */
    if (!q->head)
        q->last_element = NULL;

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* With time complexity of O(1) */
    return q ? q->count : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q == NULL)
        return;

    list_ele_t *prev = NULL;
    list_ele_t *temp;
    list_ele_t *current = q->last_element = q->head;
    while (current != NULL) {
        temp = current->next;
        current->next = prev;
        prev = current;
        current = temp;
    }
    q->head = prev;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == NULL || q->count == 1)
        return;
    q->head = mergesort(q->head, comparator);

    list_ele_t *end = q->head;
    while (end->next) {
        end = end->next;
    }
    q->last_element = end;
}

/*
 * Sort elements in linked list
 * This function is called by `q_sort`
 */
list_ele_t *mergesort(list_ele_t *start,
                      int (*compar)(const void *, const void *))
{
    if (!start || !start->next) {
        return start;
    }

    // Split the list into two lists
    list_ele_t *slow = start, *fast = start;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_ele_t *left = start, *right = slow->next;
    slow->next = NULL;

    left = mergesort(left, compar);
    right = mergesort(right, compar);

    for (list_ele_t *merge = NULL; left || right;) {
        if (!right || (left && (*compar)(left, right) < 0)) {
            list_ele_t *next = left->next;
            if (!merge) {
                start = merge = left;
            } else {
                merge->next = left;
                merge = left;
            }
            left = next;
        } else {
            list_ele_t *next = right->next;
            if (!merge) {
                start = merge = right;
            } else {
                merge->next = right;
                merge = right;
            }
            right = next;
        }
        merge->next = NULL;
    }
    return start;
}

int comparator(const void *a, const void *b)
{
    return strcmp(((list_ele_t *) a)->value, ((list_ele_t *) b)->value);
}