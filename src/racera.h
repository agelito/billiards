#ifndef RACERA_H_INCLUDED
#define RACERA_H_INCLUDED

#define UNUSED(variable) (void)(variable)

#define invalid_code *((int*)0) = 0
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#endif // RACERA_H_INCLUDED
