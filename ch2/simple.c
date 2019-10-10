/**
 * simple.c
 *
 * A simple kernel module. 
 * 
 * To compile, run makefile by entering "make"
 *
 * Operating System Concepts - 10th Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/types.h>
#include <linux/slab.h>

struct birthday{
	int day;
	int month;
	int year;
	struct list_head list;
};

//extern birthday_list;
static LIST_HEAD(birthday_list);

void insert_person(int year, int month, int day){
	
	struct birthday *person;
	person = kmalloc(sizeof(*person), GFP_KERNEL);
	person->day = day;
	person->month = month;
	person->year = year;
	INIT_LIST_HEAD(&person->list);

	list_add_tail(&person->list, &birthday_list);
}

/* This function is called when the module is loaded. */
int simple_init(void)
{
	printk(KERN_INFO "Loading Module\n");

	insert_person(1998,5,8);
	insert_person(1923,7,23);
	insert_person(2020,10,1);
	insert_person(1721,9,17);
	insert_person(1999,7,10);

/* Traversal the birthday struct */


	struct birthday *ptr = NULL;
	list_for_each_entry(ptr, &birthday_list, list){
		printk(KERN_INFO "%d %d %d\n",ptr->year,ptr->month,ptr->day);
	}

	return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");

	struct birthday *ptr, *next;
	list_for_each_entry_safe(ptr, next, &birthday_list, list){
		list_del(&ptr->list);
		kfree(ptr);
	}
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

