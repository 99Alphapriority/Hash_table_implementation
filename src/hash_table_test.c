#include<stdio.h>
#include<string.h>
#include<stdio.h>

#include "hash_table.h"

#define mu_assert(message, test)	do {if ((!test)) return message;} while(0)
#define mu_run_test(test)		do{char *message = test(); test_run++; \
						if(message) return message;} while(0)
#define strings_equal(a,b)		(strcmp(a,b) == 0)

int test_run = 0;

/*****************************
testcases related to insertion
*****************************/

static char* test_insert()
{
	ht_hash_table *table = ht_new();
	ht_item *item = NULL;

	ht_insert(table, "k", "v");

	//check number of items in the table
	int count = 0;

	for(int i = 0; i<table->size; i++)
	{
		if(NULL != table->items[i])
		{
			count++;
			item = table->items[i];
		}
	}
	//There should only be 1 item.
	mu_assert("ERROR!! More than 1 item found in the hashtable\n",(1 == count));
	mu_assert("ERROR!! Key does not match\n",strings_equal("k", item->key));
	mu_assert("ERROR!! Value does not match\n",strings_equal("v", item->value));

	//Test case passed
	ht_del_hash_table(table);

	return 0;
}

static char* test_insert_lots_of_items()
{
	ht_hash_table* table = ht_new();

	for(int i = 0; i< 20000; i++)
	{
		char key[10];
		snprintf(key, 10, "%d", i);
		ht_insert(table, key, "value");
	}
	return 0;
}

static char* test_insert_with_duplicate_key()
{
	ht_hash_table *table = ht_new();
	ht_insert(table, "key", "value1");
	ht_insert(table, "key", "value2");

	mu_assert("ERROR!! Value not updated; Instead created a new entry\n",(1 == table->count));

	return 0;
}

/******************************
Test cases related to searching
******************************/

static char* test_search_with_invalid_key()
{
	ht_hash_table* table = ht_new();
	ht_insert(table, "key", "value");

	char *value = ht_search(table, "invalid key");

	mu_assert("ERROR!! Found value against INVALID KEY\n",(NULL == value));

	ht_del_hash_table(table);
	return 0;
}

static char* test_search_with_valid_key()
{
	ht_hash_table* table = ht_new();
	ht_insert(table, "key", "value");

	char *value = ht_search(table, "key");

	mu_assert("ERROR!! Unexpected value found\n",strings_equal(value, "value"));

	ht_del_hash_table(table);
	return 0;
}

static char* test_search_with_colliding_keys()
{
	ht_hash_table* table = ht_new();

	ht_insert(table,"bz", "value1");
	ht_insert(table, "4", "value2");

	char *val1 = ht_search(table, "bz");
	char *val2 = ht_search(table, "4");

	mu_assert("ERROR!! value of first insert is not correct\n",strings_equal("value1",val1));
	mu_assert("ERROR!! value of second insert is not correct\n",strings_equal("value2",val2));

	ht_del_hash_table(table);
	return 0;
}

/*****************************
Test cases related to deletion
*****************************/

static char* test_delete()
{
	ht_hash_table *table = ht_new();
	ht_insert(table, "k", "v");
	ht_delete(table, "k");

	char *value = ht_search(table, "k");

	mu_assert("ERROR!! The item should have been deleted\n",(NULL == value));

	ht_del_hash_table(table);
	return 0;
}

/*****************************
Test cases related to resizing
*****************************/

static char* test_resize_up()
{
	//Smallest table size is 53. Table resizes when a load of 0.7 is hit.
	//This should happen on the 38th insert.

	ht_hash_table *table = ht_new();

	for(int i = 0; i < 38; i++)
	{
		char key[10];
		snprintf(key, 10, "%d", i);
		ht_insert(table, key, "value");
	}

	mu_assert("ERROR!! Table size should be 53\n",(53 == table->size));

	ht_insert(table, "new key", "value");
	mu_assert("ERROR!! Table size should be 101\n",(101 == table->size));

	ht_del_hash_table(table);
	return 0;
}

static char* test_resize_down()
{
	ht_hash_table *table = ht_new();

	for(int i = 0; i< 39; i++)
	{
		char key[10];
		snprintf(key, 10, "%d", i);
		ht_insert(table, key, "vaue");
	}

	mu_assert("ERROR!! Table size should be 101\n",(101 == table->size));

	//Table should resize down when the load hits 0.1.
	for(int i = 0; i<30; i++)
	{
		char key[10];
		snprintf(key, 10, "%d", i);
		ht_delete(table, key);
	}

	mu_assert("ERROR!! Table size should be 53\n",(53 == table->size));
	
	ht_del_hash_table(table);
	return 0;
}

/*******************
Test cases completed
*******************/

static char* all_tests()
{
	mu_run_test(test_insert);
	mu_run_test(test_insert_lots_of_items);
	mu_run_test(test_insert_with_duplicate_key);
	mu_run_test(test_search_with_invalid_key);
	mu_run_test(test_search_with_valid_key);
	mu_run_test(test_search_with_colliding_keys);
	mu_run_test(test_delete);
	mu_run_test(test_resize_up);
	mu_run_test(test_resize_down);
	return 0;
}

int main()
{
	char *result = all_tests();

	if(0 != result)
		fprintf(stderr, "%s\n", result);
	else
		fprintf(stderr, "All tests passed!!!\n");
	printf("%d tests run\n",test_run);
	return (0 != result);
}
