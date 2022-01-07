/*	###### Copyright Information ######
	This is the program (2nd part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem X. 
	Written			Commented		Debugged		Commited
	 8 December 2021	12 December 2021	11~12 December 2021	12 December 2021
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp10 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	new_vertex_set:		Allocate space for a new vertex set. Return a pointer to the vertex set if succeed, and NULL otherwise. 
	free_vertex_set:	Free space of a vertex set; that is, memory that <pointers "vs"> and "vs->id" point to. 
	new_path:		Allocate space for a new path. Return a pointer to the path if succeed, and NULL otherwise. 
	free_path:		Free space of a path; that is, memory that <pointers "path"> and "path->id" point to. 
*/

#include <stdint.h>
#include <stdlib.h>
#include "mp10.h"
#define INIT_SIZE 8

vertex_set_t* new_vertex_set () {
	vertex_set_t* vs_attempt = malloc(sizeof(vertex_set_t));	// Try allocating "vs" struct
	if (!vs_attempt) return NULL;
	int32_t* id_attempt = malloc(sizeof(int32_t) * INIT_SIZE);	// Try allocating "vs->id" array
	if (!id_attempt) {
		free(vs_attempt);
		return NULL;
	}
	vs_attempt->id = id_attempt;					// Initialize fields
	vs_attempt->id_array_size = INIT_SIZE;
	vs_attempt->count = vs_attempt->minimap = 0;
	return vs_attempt;
}

void free_vertex_set (vertex_set_t* vs) {
	if (vs->id) free(vs->id);					// Free "vs->id" array
	free(vs);							// Free "vs" struct
}

path_t* new_path () {
	path_t* path_attempt = malloc(sizeof(path_t));			// Try allocating "path" struct
	if (!path_attempt) return NULL;
	int32_t* id_attempt = malloc(sizeof(int32_t));			// Try allocating "path->id" array
	if (!id_attempt) {
		free(path_attempt);
		return NULL;
	}
	path_attempt->id = id_attempt;					// Initialize fields
	path_attempt->n_vertices = 0;
	path_attempt->tot_dist = 0;
	path_attempt->minimap = 0;
	return path_attempt;
}

void free_path (path_t* path) {
	if (path->id) free(path->id);					// Free "path->id" array
	free(path);							// Free "path" struct
}
