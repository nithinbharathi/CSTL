#include<stdio.h>
#include<stdlib.h>

#define SEED 1000000007

typedef struct entry entry;
typedef struct hash_table hash_table;

void add(hash_table** hash_table, int key, int value);

struct entry{
  int key;
  int value;
  entry* next;
};

struct hash_table{
  entry** table;
  size_t cnt;
  size_t size;
  float fill_factor;
};

size_t get_size(size_t size){
  if(size == 0)
	return 1;

  size--;

  size |= size>>1;
  size |= size>>2;
  size |= size>>4;
  size |= size>>8;
  size |= size>>16;

  return size+1;

}

hash_table* init(size_t size){
    hash_table* table = (hash_table*)malloc(sizeof(hash_table));

	table->cnt = 0;
	table->fill_factor = 0.5;
	table->size = get_size(size);
	table->table = (entry**)malloc(table->size * sizeof(entry*));

	for (size_t i = 0; i < table->size; i++) {
	  table->table[i] = NULL;
	}

	return table;
}

void trace(hash_table* hash_table){
  for(size_t index = 0; index<hash_table->size;index++){
	entry* head = hash_table->table[index];
	if(head != NULL){
	  printf("BUCKET: %zu\nVALUES: ",index);
	  	while(head != NULL){
		  printf("%d\t",head->value);
		  head = head->next;
		}
		printf("\n");
	}	
  }
}

entry* create_entry(int key, int value){
  entry* item = (entry*)malloc(sizeof(entry));
  
  item->key = key;
  item->value = value;
  item->next = NULL;

  return item;
}

// simplified implementation of MurmurHash3
int hash(int key){
    uint32_t h = SEED ^ 4;
    h ^= key;
    h *= 0x5bd1e995;
    h ^= h >> 15;
    h *= 0x5bd1e995;
    h ^= h >> 13;
	
    return h;
}

void free_table(hash_table* hash_table){
  for(size_t index = 0; index < hash_table->size; index++){
	entry* head = hash_table->table[index];
	while(head != NULL){
	  entry* temp = head;
	  head = head->next;
	  free(temp);
	}
	free(hash_table->table);
	free(hash_table);
  }
}

hash_table* resize(hash_table* table){
  size_t new_size = table->size*2;
  hash_table* new_table = init(new_size);
  
  for(size_t index = 0; index<table->size;index++){
	entry* head = table->table[index];
	while(head != NULL){
	  add(&new_table,head->key,head->value);
	  head = head->next;
	}
  }
  
  free_table(table);
  
  return new_table;
  
}

int filled_percentage(hash_table* hash_table){
  return ((float)hash_table->cnt/hash_table->size)*100;
}

void add(hash_table** table, int key, int value){
  hash_table* original_table = *table;
  if (filled_percentage(original_table) >= original_table->fill_factor*100)
	*table = resize(original_table);
  
  int hash_value = hash(key);
  int index = hash_value%(*table)->size;
  entry* head = (*table)->table[index];
  entry* entry = create_entry(key, value);
 
  if(head == NULL){
	(*table)->table[index] = entry;
  }else{
	while(head->next != NULL){
	  head = head->next;
	}
	head->next = entry;
  }

  (*table)->cnt++;
}

int main(){
  hash_table* table = init(3);
  
  printf("the size of table  is %zu\n",table->size);
  
  add(&table, 1, 500);
  add(&table,2,600);
  trace(table);
  printf("the percentage of buckets filled is %d\n",filled_percentage(table));

  add(&table,5,1000);
  add(&table,6,2000);


  printf("the size of table  is %zu\n",table->size);
  trace(table);
  printf("the percentage of buckets filled is %d\n",filled_percentage(table));


}
