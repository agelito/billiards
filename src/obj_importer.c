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

    int position_count = 0;
    int normal_count = 0;
    int face_count = 0;

    while(*at)
    {
	if(*at == '#')
	{ }
	else if(match_string("v ", at))
	{
	    position_count++;
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
    platform_log("%s\n position: %d\n normal: %d\n face: %d\n", path,
	    position_count, normal_count, face_count);

    at = begin;

    vector3* positions = (vector3*)malloc(sizeof(vector3) * position_count);
    vector3* normals = (vector3*)malloc(sizeof(vector3) * normal_count);
    
    int* positions_index = (int*)malloc(sizeof(int) * face_count * 3);
    int* normals_index = (int*)malloc(sizeof(int) * face_count * 3);

    int read_position_count = 0;
    int read_normal_count = 0;
    int read_face_count = 0;

    while(*at)
    {
	if(*at == '#')
	{ }
	else if(match_string("v ", at))
	{
	    vector3* position = (positions + read_position_count++);
	    int v = sscanf(at, "v %f %f %f", &position->x, &position->y, &position->z);
	    assert(v == 3);
	}
	else if(match_string("vn ", at))
	{
	    vector3* normal = (normals + read_normal_count++);
	    int v = sscanf(at, "vn %f %f %f", &normal->x, &normal->y, &normal->z);
	    assert(v == 3);
	}
	else if(match_string("f ", at))
	{
	    int* pi = (positions_index + read_face_count * 3);
	    int* ni = (normals_index + read_face_count * 3);
	    
	    int p0, p1, p2;
	    int n0, n1, n2;
	    int v = sscanf(at, "f %d//%d %d//%d %d//%d", &p0, &n0, &p1, &n1, &p2, &n2);
	    assert(v == 6);
	    
	    *(pi + 0) = p0; *(pi + 1) = p1; *(pi + 2) = p2;
	    *(ni + 0) = n0; *(ni + 1) = n1; *(ni + 2) = n2;

	    read_face_count++;
	}

	at = advance_line(at);
    }

    assert(position_count == read_position_count);
    assert(normal_count == read_normal_count);
    assert(face_count == read_face_count);
    platform_free_file(&file);

    int vertex_count = (face_count * 3);
    result.vertex_count = vertex_count;
    result.vertices.positions = (vector3*)malloc(sizeof(vector3) * vertex_count);
    result.vertices.normals = (vector3*)malloc(sizeof(vector3) * vertex_count);

    int vertex_index = 0;
    
    int n;
    for_range(n, face_count)
    {
	int* positions_face = (positions_index + n * 3);
	
	int pi0 = *(positions_face + 0) - 1;
	int pi1 = *(positions_face + 2) - 1;
	int pi2 = *(positions_face + 1) - 1;

	vector3* p0 = (result.vertices.positions + vertex_index + 0);
	vector3* p1 = (result.vertices.positions + vertex_index + 1);
	vector3* p2 = (result.vertices.positions + vertex_index + 2);

	*p0 = *(positions + pi0); *p1 = *(positions + pi1); *p2 = *(positions + pi2);
	

	int* normals_face = (normals_index + n * 3);

	int ni0 = *(normals_face + 0) - 1;
	int ni1 = *(normals_face + 2) - 1;
	int ni2 = *(normals_face + 1) - 1;

	vector3* n0 = (result.vertices.normals + vertex_index + 0);
	vector3* n1 = (result.vertices.normals + vertex_index + 1);
	vector3* n2 = (result.vertices.normals + vertex_index + 2);
	
	*n0 = *(normals + ni0); *n1 = *(normals + ni1); *n2 = *(normals + ni2);
	
	vertex_index += 3;
    }

    free(positions);
    free(normals);
    
    free(positions_index);
    free(normals_index);
    
    return result;
}
