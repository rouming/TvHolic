#ifndef __LIST_H__
#define __LIST_H__

#include "Std.h"

/*
  Simply doubly linked list implementation.
  Thanks to linux kernel
*/

struct cd_list
{
	struct cd_list *next, *prev;
};

#define LIST_INIT(name) { &(name), &(name) }
#define CD_LIST_HEAD(name) \
	    struct cd_list name = CD_LIST_INIT(name)

#define cd_list_entry(ptr, type, member) \
	__container_of(ptr, type, member)

#define cd_list_first_entry(ptr, type, member) \
	cd_list_entry((ptr)->next, type, member)

#define cd_list_last_entry(ptr, type, member) \
	cd_list_entry((ptr)->prev, type, member)

static __inline void cd_list_init(struct cd_list *ptr)
{
	ptr->next = ptr;
	ptr->prev = ptr;
}

static __inline void __cd_list_add(struct cd_list *newl, struct cd_list *prev, struct cd_list *next)
{
	next->prev = newl;
	newl->next = next;
	newl->prev = prev;
	prev->next = newl;
}

/* add element to list head */
static __inline void cd_list_add(struct cd_list *newl, struct cd_list *head)
{
	__cd_list_add(newl, head, head->next);
}

/* add element to list tail */
static __inline void cd_list_add_tail(struct cd_list *newl, struct cd_list *head)
{
	__cd_list_add(newl, head->prev, head);
}

#define CD_LIST_POISON_NEXT  ((void *)0x00112233)
#define CD_LIST_POISON_PREV  ((void *)0x00445566)

static __inline void __cd_list_del(struct cd_list *prev, struct cd_list *next)
{
	next->prev = prev;
	prev->next = next;
}

static __inline void cd_list_del(struct cd_list *elem)
{
	__cd_list_del(elem->prev, elem->next);

	elem->next = (struct cd_list *)CD_LIST_POISON_NEXT;
	elem->prev = (struct cd_list *)CD_LIST_POISON_PREV;
}

static __inline void cd_list_del_init(struct cd_list *elem)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_init(elem);
}

static __inline int cd_list_empty(const struct cd_list* head)
{
	return (head->next == head);
}

static __inline void cd_list_move(struct cd_list *elem, struct cd_list *head)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_add(elem, head);
}

static __inline void cd_list_move_tail(struct cd_list *elem, struct cd_list *head)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_add_tail(elem, head);
}

static __inline void cd_list_splice(struct cd_list *list, struct cd_list *head)
{
	if (!cd_list_empty(list)) {
		struct cd_list *first = list->next;
		struct cd_list *last = list->prev;
		struct cd_list *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

#define cd_list_for_each_entry(typeof_pos, pos, head, member) \
	for (pos = cd_list_entry((head)->next, typeof_pos, member); \
			&pos->member != (head); \
			pos = cd_list_entry(pos->member.next, typeof_pos, member))

#define cd_list_for_each_entry_reverse(typeof_pos, pos, head, member) \
	for (pos = cd_list_entry((head)->prev, typeof_pos, member); \
			&pos->member != (head); \
			pos = cd_list_entry(pos->member.prev, typeof_pos, member))

#define cd_list_for_each_entry_safe(typeof_pos, pos, n, head, member) \
	for (pos = cd_list_entry((head)->next, typeof_pos, member), \
			n = cd_list_entry(pos->member.next, typeof_pos, member); \
			&pos->member != (head); \
			pos = n, n = cd_list_entry(n->member.next, typeof_pos, member))

#define cd_list_for_each_entry_from_safe(typeof_pos, pos, n, head, member) \
	for (n = cd_list_entry(pos->member.next, typeof_pos, member); \
			&pos->member != (head); \
			pos = n, n = cd_list_entry(n->member.next, typeof_pos, member))

#define cd_list_for_each_entry_reverse_safe(typeof_pos, pos, p, head, member) \
	for (pos = cd_list_entry((head)->prev, typeof_pos, member),	\
			p = cd_list_entry(pos->member.prev, typeof_pos, member); \
			&pos->member != (head); \
			pos = p, p = cd_list_entry(pos->member.prev, typeof_pos, member))

#endif /* __LIST_H__ */
