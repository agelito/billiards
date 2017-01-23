// obj_importer.c

static inline int
match_string(char* a, char* b)
{
    int match = 1;
    char test;
    while((test = *a++))
    {
	if(test != *b++)
	{
	    match = 0;
	    break;
	}
    }
    
    return match;
}

static inline char*
advance_line(char* at)
{
    char test;
    while((test = *at++) && test != '\n');
    return at;
}

static mesh_data
obj_load_from_file(char* path)
{
    mesh_data result = (mesh_data){0};
    
    read_file file = platform_read_file(path, 1);
    assert(file.size != 0);

    char* begin = (char*)file.data;
    char* at = begin;

    int vertex_count = 0;
    int normal_count = 0;
    int face_count = 0;

    while(*at)
    {
	if(*at == '#')
	{ }
	else if(match_string("v ", at))
	{
	    vertex_count++;
	}
	else if(match_string("vn ", at))
	{
	    normal_count++;
	}
	else if(match_string("f ", at))
	{
	    face_count++;
	}

	at = advance_line(at);
    }
    fprintf(stdout, "%s\n vertex: %d\n normal: %d\n face: %d\n", path,
	    vertex_count, normal_count, face_count);

    at = begin;

    vector3* vertices = (vector3*)malloc(sizeof(vector3) * vertex_count);
    int* faces = (int*)malloc(sizeof(int) * face_count * 3);

    int read_vertex_count = 0;
    int read_face_count = 0;

    while(*at)
    {
	if(*at == '#')
	{ }
	else if(match_string("v ", at))
	{
	    vector3* vertex = (vertices + read_vertex_count++);
	    int v = sscanf(at, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
	    assert(v == 3);
	}
	else if(match_string("f ", at))
	{
	    int* face = (faces + read_face_count * 3);
	    
	    int v0, v1, v2;
	    int n0, n1, n2;
	    int v = sscanf(at, "f %d//%d %d//%d %d//%d", &v0, &n0, &v1, &n1, &v2, &n2);
	    assert(v == 6);
	    
	    *(face + 0) = v0;
	    *(face + 1) = v1;
	    *(face + 2) = v2;

	    read_face_count++;
	}

	at = advance_line(at);
    }

    assert(vertex_count == read_vertex_count);
    assert(face_count == read_face_count);
    platform_free_file(&file);

    vertex_count = (face_count * 3);
    result.vertex_count = vertex_count;
    result.vertices = (vertex*)malloc(sizeof(vertex) * vertex_count);

    int vertex_index = 0;
    
    int n;
    for_range(n, face_count)
    {
	int* face = (faces + n * 3);
	int f0 = *(face + 0) - 1;
	int f1 = *(face + 2) - 1;
	int f2 = *(face + 1) - 1;
	
	vector3* p0 = (vertices + f0);
	vector3* p1 = (vertices + f1);
	vector3* p2 = (vertices + f2);

	vertex* v0 = (result.vertices + vertex_index++);
	vertex* v1 = (result.vertices + vertex_index++);
	vertex* v2 = (result.vertices + vertex_index++);

	v0->p = *p0;
	v0->c = (color){1.0f, 1.0f, 1.0f};
	v0->uv = (vector2){{{0.0f, 0.0f}}};
	
	v1->p = *p1;
	v1->c = (color){1.0f, 1.0f, 1.0f};
	v1->uv = (vector2){{{0.0f, 0.0f}}};
	
	v2->p = *p2;
	v2->c = (color){1.0f, 1.0f, 1.0f};
	v2->uv = (vector2){{{0.0f, 0.0f}}};
    }
    
    free(vertices);
    free(faces);
    
    return result;
}
