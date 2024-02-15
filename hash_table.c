#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "hash_table.h"

/************************************************************************
Function Name:	ht_new_item
Arguments:		key, value
Description:
				Initialisation function for ht_item structure pointer.
				Allocates a chunk of memory of size of an ht_item, and 
				saves a copy of the strings key and value in the new 
				chunk of memory.
Return value:	A (ht_item)pointer to a chunk of memory.
************************************************************************/
static ht_item* ht_new_item(const char* k, const char* v)
{
	ht_item* item = (ht_item*)malloc(sizeof(ht_item));
	item->key = strdup(k);
	item->value = strdup(v);

	return item;
}

/************************************************************************
Function Name:	ht_new
Arguments:		void
Description:
				Initialises a new nash table. Initialise the array of
				ht_item with calloc. A NULL entry in the array indicates
				that the bucket is empty.
Return value:	A (ht_hash_table)pointer to a chunk of memory.
************************************************************************/
ht_hash_table* ht_new(void)
{
	ht_hash_table *table = (ht_hash_table*)malloc(sizeof(ht_hash_table));

	table->size = 53;
	table->count = 0;
	table->items = (ht_item**)calloc((size_t)table->size, sizeof(ht_item*));

	return table;
}

/************************************************************************
Function Name:	ht_del_item
Arguments:		item pointer
Description:
				Function to free the memory allocated to the item pointer
Return value:	void
************************************************************************/
static void ht_del_item(ht_item* item)
{
	free(item->key);
	free(item->value);
	free(item);
}

/************************************************************************
Function Name: 	ht_del_hash_table
Arguments:		table pointer
Description:
				Function to free the memory allocated to the ht_items 
				array and the hash table itself.
Return value:	void
************************************************************************/
void ht_del_hash_table(ht_hash_table* table)
{
	for(int i = 0; i<table->size; i++)
	{
		ht_item *item = table->items[i];

		if(NULL != item)
			ht_del_item(item);
	}
	free(table->items);
	free(table);
}

int main()
{
	ht_hash_table* ht = ht_new();
	fprintf(stderr,"New hash_table malloced at address: 0x%x\n",ht);
	ht_del_hash_table(ht);
	return 0;
}
