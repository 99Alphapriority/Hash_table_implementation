#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "hash_table.h"
#include "prime.h"

#define HT_PRIME_1	151
#define HT_PRIME_2	163
#define HT_INITIAL_BASE_SIZE	50

static ht_item HT_DELETED_ITEM = {NULL, NULL};

/************************************************************************
Function Name:	ht_new_item
Arguments:	key, value
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

	if(NULL == item)
	{
		fprintf(stderr,"[%s:%d] Memory allocation failed for a new item\n",__func__,__LINE__);
		abort();
	}

	item->key = strdup(k);
	item->value = strdup(v);

	return item;
}

/************************************************************************
Function Name:	ht_new_sized
Arguments:	integer baseSize
Description:
		Initialises a new hash table. Initialise the array of
		ht_item with calloc. A NULL entry in the array indicates
		that the bucket is empty.
Return value:	A (ht_hash_table)pointer to a chunk of memory.
************************************************************************/
static ht_hash_table* ht_new_sized(const int baseSize)
{
	ht_hash_table *table = (ht_hash_table*)malloc(sizeof(ht_hash_table));

	if(NULL == table)
	{
		fprintf(stderr,"[%s:%d] Memory allocation failed for a new hash table\n",__func__, __LINE__);
		abort();
	}
	
	table->baseSize = baseSize;
	table->size = nextPrime(table->baseSize);
	table->count = 0;
	table->items = (ht_item**)calloc((size_t)table->size, sizeof(ht_item*));

	return table;
}

/************************************************************************
Function Name:	ht_new
Arguments:	NONE
Description:
		Initialises a new hash table
Retrun value:	a pointer to a chunk of memory storing the hash table
************************************************************************/
ht_hash_table* ht_new()
{
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

/************************************************************************
Function Name:	ht_del_item
Arguments:	item pointer
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
Arguments:	table pointer
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

/************************************************************************
Function Name:	ht_hash
Arguments:	key string, int a, size of bucket array
Description:
		Function that takes a string as an input and performs
		a hash function on it. The result is a large integer
		that is reduced by dividing it by the size of the bucket
		and returning the remainder.
Return value:	int index
************************************************************************/
static int ht_hash(const char* s, const int a, const int m)
{
	long hash = 0;
	const int len_s = strlen(s);

	for(int i =0; i<len_s; i++)
	{
		hash+=(long)pow(a,(len_s - (i+1))) * s[i];
		hash %= m;
	}

	return (int)hash;
}

/***********************************************************************
Function Name:	ht_get_hash
Arguments:	key string, size of bucket array, int attempt
Description:
		If different inputs result to same index after hashing,
		performing Double hashing to chose a new index.
Return value:	int index
***********************************************************************/
static int ht_get_hash(const char* s, const int m, const int attempt)
{
	const int hash_a = ht_hash(s, HT_PRIME_1, m);
	const int hash_b = ht_hash(s, HT_PRIME_2, m);

	return (hash_a + (attempt * (hash_b + 1)))%m;
}

/**********************************************************************
Function Name:	ht_resize
Arguments:	table pointer, integer baseSize
Description:	
		Initialise a new hash table with desired size. All non
		NULL or deleted items are inserted into the new table.
		Swap the attributes of the new and old hash table
		before deleting the old table.
Return value:	void
**********************************************************************/
static void ht_resize(ht_hash_table* table, const int baseSize)
{
	/*
	check to ensure no attempting to reduce the size below its 
	minimum
	*/
	if(HT_INITIAL_BASE_SIZE > baseSize)
		return;
	
	ht_hash_table* newTable = ht_new_sized(baseSize);

	for(int i = 0; i < table->size; i++)
	{
		ht_item *item = table->items[i];

		if((NULL != item) && (item != &HT_DELETED_ITEM))
		{
			ht_insert(newTable, item->key, item->value);
		}
	}


	//Swapp the content of new table and old table
	table->baseSize = newTable->baseSize;
	table->count = newTable->count;

	const int tmpSize = table->size;
	table->size = newTable->size;
	newTable->size = tmpSize;

	ht_item** tmpItems = table->items;
	table->items = newTable->items;
	newTable->items = tmpItems;

	//delete the new table
	ht_del_hash_table(newTable);
}

/**********************************************************************
Function Name:	ht_resize_up
Arguments:	table pointer
Description:
		Up sizes the hash table by doubling the existing size
Return value:	void
**********************************************************************/
static void ht_resize_up(ht_hash_table *table)
{
	const int newSize = table->baseSize * 2;
	ht_resize(table, newSize);
}

/**********************************************************************
Function Name:	ht_resize_down
Arguments:	table pointer
Description:
		Down sizes the hash table by dividing it by 2
Retrun value: void
**********************************************************************/
static void ht_resize_down(ht_hash_table *table)
{
	const int newSize = table->baseSize / 2;
	ht_resize(table, newSize);
}

/**********************************************************************
Function Name:	ht_insert
Arguments:	table pointer, key string , value string
Description:
		Uses the hash function to find an empty bucket and 
		insert the item. After insertion increment the table
		count variable.
Return value:	void
**********************************************************************/
void ht_insert(ht_hash_table *table, const char* key, const char* value)
{
	//check if resizing is required
	const int load = table->count * 100 / table->size;	
	//multiplied by 100 to avoid floating points
	if(70 < load)
		ht_resize_up(table);
	
	ht_item *item = ht_new_item(key, value);
	int index = ht_get_hash(item->key, table->size, 0);
	ht_item* currItem = table->items[index];
	int i = 1;	//number of attempts
	
	/*
	if the bucket is not empty then perform hashing again by 
	incrementing the attempt variable untill an empty bucket is 
	found
	*/

	while((NULL != currItem) && (currItem != &HT_DELETED_ITEM))
	{	
		/*
		check if the key already exists; replace the item
		with new item havinf updated value
		*/
		if(strcmp(currItem->key, key))
		{
			ht_del_item(currItem);
			table->items[index] = item;
			return;
		}
		index = ht_get_hash(item->key, table->size, i);
		currItem = table->items[index];
		i++;
	}

	table->items[index] = item;
	table->count++;
}

/********************************************************************
Function Name:	ht_search
Arguments:	table pointer, key string
Description:
		Uses hash function to find the bucket index and
		compares the key attribute. If key doesn't match
		run hash function again until the key is found.
		If the key is not found return NULL.
Retrun value:	value string
********************************************************************/
char* ht_search(ht_hash_table *table, const char* key)
{
	int index = ht_get_hash(key, table->size, 0);
	ht_item* item = table->items[index];
	int i = 1;	//number of attempts

	while(NULL != item)
	{
		if(&HT_DELETED_ITEM != item)
		{
			if(0 == strcmp(item->key, key))
				return item->value;
		}
		index = ht_get_hash(key, table->size, i);

		//check to ensure that index is not out of bound
		if(table->size < index)
			return NULL;

		item = table->items[index];
		i++;
	}

	return NULL;
}

/********************************************************************
Function Name:	ht_delete
Arguments:	table pointer, key string
Description:
		Function for deleting the item from the hash table.
		Instead of actually deleting it is replaced by a
		global sentinel item.
Return value:	void
********************************************************************/
void ht_delete(ht_hash_table *table, const char* key)
{
	//check if resizing is required
	const int load = table->count * 100 / table->size;
	//multiplied by 100 to avoid floating point
	if(10 > load)
		ht_resize_down(table);

	int index = ht_get_hash(key, table->size, 0);
	ht_item *item = table->items[index];
	int i = 1;

	while(NULL != item)
	{
		if(&HT_DELETED_ITEM != item)
		{
			if(0 == strcmp(key, item->key))
			{
				ht_del_item(item);
				table->items[index] = &HT_DELETED_ITEM;
			}
		}
		index = ht_get_hash(key, table->size, i);
		
		//check to ensure that index is not out of bound
		if(table->size < index)
			fprintf(stderr,"index calculated is out of bound!!!!\n");

		item = table->items[index];
		i++;
	}

	table->count--;
}

int main()
{
	ht_hash_table* ht = ht_new();
	fprintf(stderr,"New hash_table malloced at address: 0x%x\n",ht);
	ht_del_hash_table(ht);
	return 0;
}

