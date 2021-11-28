/*	###### Copyright Information ######
	This is the program (2nd part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem IX. 
	Written by Chen, Ziyuan on 20/25~26 November 2021. 
	Debugged and committed by Chen, Ziyuan on 27~28 November 2021. 
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp9 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	The functions operate as follows: 
	match_requests:	Search for possible source and destination points based on <requests "r1" and "r2">, 
			and schedule the shortest path between them. Return 1 if succeed, 0 otherwise. 
*/

#include <stdint.h>
#include <stdio.h>
#include "mp5.h"
#include "mp9.h"

int32_t match_requests (graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r1, request_t* r2,
			vertex_set_t* src_vs, vertex_set_t* dst_vs, path_t* path) {
	src_vs->count = dst_vs->count = path->n_vertices = path->tot_dist = 0;			// Init. counters
	find_nodes(&r1->from, src_vs, p, 0);
	trim_nodes(g, src_vs, &r2->from);							// Calc. source vertex set
	find_nodes(&r1->to, dst_vs, p, 0);
	trim_nodes(g, dst_vs, &r2->to);								// Calc. destination vertex set
	return (src_vs->count && dst_vs->count && dijkstra(g, h, src_vs, dst_vs, path));	// Check validity & calc. path
}
