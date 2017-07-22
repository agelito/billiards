// hash_map.c

typedef struct hash_map hash_map;

struct hash_map
{
    int element_count;
    int element_capacity;

    int* keys;
    int* values;
};

hash_map
hash_map_create(int capacity)
{
    hash_map created = (hash_map){0};
    created.element_capacity = capacity;

    created.keys = (int*)malloc(capacity * sizeof(int));
    created.values = (int*)malloc(capacity * sizeof(int));

    return created;
}

void
hash_map_delete(hash_map* map)
{
    free(map->keys);
    free(map->values);

    *map = (hash_map){0};
}

int
hash_map_contains(hash_map* map, int key)
{
    int i, contains = 0;
    for_range(i, map->element_count)
    {
	if(*(map->keys + i) != key)
	    continue;

	contains = 1;
	break;
    }

    return contains;
}

int
hash_map_get_value(hash_map* map, int key)
{
    int i, value = 0;
    for_range(i, map->element_count)
    {
	if(*(map->keys + i) != key)
	    continue;

	value = *(map->values + i);
	break;
    }

    return value;
}

void
hash_map_insert(hash_map* map, int key, int value)
{
    if(map->element_count >= map->element_capacity)
	return;

    *(map->keys + map->element_count) = key;
    *(map->values + map->element_count) = value;

    ++map->element_count;
}
