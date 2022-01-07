/*	###### Copyright Information ######
	This is the program (3rd part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem X. 
	Written			Commented		Debugged		Commited
	 9 December 2021	12 December 2021	11 December 2021	12 December 2021
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp10 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	traverse_subtree:		[HELPER] Traverse the subtree of a <pyramid tree "p"> starting from a <root node "root">. The minimap of 
						 a leaf node in <graph "g"> is labelled with <marker "marker"> indicating its ancestor. 
	mark_vertex_minimap:		Mark each vertex in a <graph "g"> with a bit indicating which node in the fourth level of a 
						 <pyramid tree "p"> is the ancestor of the pyramid tree node corresponding to the vertex. 
						 >>> Return 1 on success. <<<
	build_vertex_set_minimap:	Calculate the "minimap" of <vertex set "vs"> based on the "mm_bits" of the recorded nodes in <graph "g">. 
	build_path_minimap:		Calculate the "minimap" of <path "p"> based on the "mm_bits" of the recorded nodes in <graph "g">. 
	merge_vertex_sets:		Find the intersection of <two vertex sets "v1" and "v2"> and writes the results into a new <vertex set "vint">. 
						 >>> Return 1 if the resulting intersection is non-empty, and 0 otherwise. <<<
*/

#include <stdint.h>
#include <stdio.h>
#include "mp10.h"

void traverse_subtree (graph_t* g, pyr_tree_t* p, int32_t root, int32_t marker) {
	if (4 * root + 1 >= p->n_nodes) g->vertex[p->node[root].id].mm_bit = marker;					// Stop at leaf nodes, otherwise 
	else {														// continue searching downward
		if (4 * root + 1 < p->n_nodes) traverse_subtree(g, p, 4 * root + 1, marker);
		if (4 * root + 2 < p->n_nodes) traverse_subtree(g, p, 4 * root + 2, marker);
		if (4 * root + 3 < p->n_nodes) traverse_subtree(g, p, 4 * root + 3, marker);
		if (4 * root + 4 < p->n_nodes) traverse_subtree(g, p, 4 * root + 4, marker);
	}
}

int32_t mark_vertex_minimap (graph_t* g, pyr_tree_t* p) {
	if (p->n_nodes <= 64) for (int32_t marker = 0; marker < 64; marker++) g->vertex[p->node[marker].id].mm_bit = marker;	// Small pyr. tree case
	else for (int32_t marker = 0; marker < 64; marker++) traverse_subtree(g, p, marker + 21, marker);		// Large pyr. tree case
	return 1;
}

void build_vertex_set_minimap (graph_t* g, vertex_set_t* vs) {
	for (int32_t iter_set = 0; iter_set < vs->count; iter_set++)
		vs->minimap = vs->minimap | (1ULL << g->vertex[vs->id[iter_set]].mm_bit);				// OR thru mm_bits in "vs->id"
}

void build_path_minimap (graph_t* g, path_t* p) {
	for (int32_t iter_set = 0; iter_set < p->n_vertices; iter_set++)
		p->minimap = p->minimap | (1ULL << g->vertex[p->id[iter_set]].mm_bit);					// OR thru mm_bits in "p->id"
}

int32_t merge_vertex_sets (const vertex_set_t* v1, const vertex_set_t* v2, vertex_set_t* vint) {
	int32_t search_ptr = 0;			// Pointer to the rightmost value in "v2" not exceeding the value in "v1" currently being examined
	vint->count = 0;												// Clear previous data
	for (int32_t iter_set = 0; iter_set < v1->count; iter_set++) {							// For each value in "v1", 
		while (v1->id[iter_set] > v2->id[search_ptr] && search_ptr < v2->count - 1) search_ptr++;		// adjust ptr. to "v2" 
		if (v1->id[iter_set] == v2->id[search_ptr]) vint->id[vint->count++] = v1->id[iter_set];			// and check for equivalence
	}
	return (vint->count != 0);
}
