/*	###### Copyright Information ######
	This is the program (1st part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem IX. 
	Written by Chen, Ziyuan on 20/25~26 November 2021. 
	Debugged and committed by Chen, Ziyuan on 27~28 November 2021. 
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp9 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	The functions operate as follows: 
	find_nodes:	Starting at <array index "nnum"> of a <pyramid tree "p">, find vertices within range of <a locale "loc"> 
				and write their ids into a <vertex set "vs">. 
	trim_nodes:	Remove any vertices of a <graph "g"> not in range of a <locale "loc"> from a <vertex set "vs">. 
	dijkstra:	Find the shortest path between any node in the src vertex set and any node in the destination vertex set, 
			and write that path into path; returns 1 on success, or 0 on failure. 
*/

#include <stdint.h>
#include <stdio.h>
#include "mp5.h"
#include "mp9.h"
#define MY_INFINITY 1000000000
#define gid2vtx(gid) g->vertex[gid]												// Helper abbrs.
#define gid2dist(gid) gid2vtx(gid).from_src
#define gid2hid(gid) gid2vtx(gid).heap_id
#define hid2gid(hid) h->elt[hid]
#define hid2dist(hid) gid2dist(hid2gid(hid))

void find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum) {
	record_fn_call();
	if (4 * nnum + 1 >= p->n_nodes) {
		if (in_range(loc, p->node[nnum].x, p->node[nnum].y_left) && vs->count <= MAX_IN_VERTEX_SET) {
			vs->id[vs->count] = p->node[nnum].id;
			vs->count++;
		}
	} else {
		if (p->node[nnum].x >= loc->x - loc->range) {									// Left region
			if (in_range(loc, p->node[nnum].x, p->node[nnum].y_left)) {						// Left half of circle
				find_nodes(loc, vs, p, 4 * nnum + 1);
				find_nodes(loc, vs, p, 4 * nnum + 3);
			} else {												// Outside the circle
				if (p->node[nnum].y_left >= loc->y - loc->range) find_nodes(loc, vs, p, 4 * nnum + 1);		// Upper-left
				if (p->node[nnum].y_left <= loc->y + loc->range) find_nodes(loc, vs, p, 4 * nnum + 3);		// Lower-left
			}
		}
		if (p->node[nnum].x <= loc->x + loc->range) {									// Right region
			if (in_range(loc, p->node[nnum].x, p->node[nnum].y_right)) {						// Right half of circle
				find_nodes(loc, vs, p, 4 * nnum + 2);
				find_nodes(loc, vs, p, 4 * nnum + 4);
			} else {												// Outside the circle
				if (p->node[nnum].y_right >= loc->y - loc->range) find_nodes(loc, vs, p, 4 * nnum + 2);		// Upper-right
				if (p->node[nnum].y_right <= loc->y + loc->range) find_nodes(loc, vs, p, 4 * nnum + 4);		// Lower-right
			}
		}
	}
}

void trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc) {
	int32_t iter_count = vs->count, iter_ptr = 0, iter_vs, iter_ovwt;
	for (iter_vs = 0; iter_vs < iter_count; iter_vs++) {									// Go through vertices
		if (!in_range(loc, gid2vtx(vs->id[iter_ptr]).x, gid2vtx(vs->id[iter_ptr]).y)) {					// If not in range ...
			for (iter_ovwt = iter_ptr; iter_ovwt < vs->count - 1; iter_ovwt++)					// ... then overwrite
				vs->id[iter_ovwt] = vs->id[iter_ovwt + 1];
			vs->count--;												// Preserve pointer
		} else iter_ptr++;												// Else, increment ptr
	}
}

void initialize_dijkstra (graph_t* g, heap_t* h) {										// Init. g & h properties
	h->n_elts = g->n_vertices;
	for (int32_t iter_init = 0; iter_init < g->n_vertices; iter_init++) {
		gid2vtx(iter_init).pred = MY_INFINITY;										// graph_vtx.pred
		gid2dist(iter_init) = MY_INFINITY;										// graph_vtx.dist
		gid2hid(iter_init) = iter_init;											// graph_vtx.heap_id
		hid2gid(iter_init) = iter_init;											// heap_vtx.graph_id
	}
}

void heap_swap (graph_t* g, heap_t* h, int32_t hid_1, int32_t hid_2) {								// Swap two elmts in heap
	gid2hid(hid2gid(hid_1)) = hid_2;											// Swap graph_ids
	gid2hid(hid2gid(hid_2)) = hid_1;
	int32_t temp = hid2gid(hid_1);												// Swap values
	hid2gid(hid_1) = hid2gid(hid_2);
	hid2gid(hid_2) = temp;
}

void heapify_downward (graph_t* g, heap_t* h, int32_t node) {									// Rebuild heap from top
	if (node * 2 + 1 < h->n_elts) {												// Left child exists?
		int32_t smallest = node;
		int32_t dist_parent = hid2dist(node);
		int32_t dist_lchild = hid2dist(node * 2 + 1);
		if (dist_lchild < dist_parent) smallest = node * 2 + 1;								// Left child is smallest
		if (node * 2 + 2 < h->n_elts) {											// Right child exists?
			int32_t dist_rchild = hid2dist(node * 2 + 2);
			if (dist_rchild < dist_parent && dist_rchild < dist_lchild) smallest = node * 2 + 2;			// Right child is smallest
		}
		if (smallest != node) {
			heap_swap(g, h, smallest, node);									// Swap parent and child
			heapify_downward(g, h, smallest);									// Recurse downward
		}
	}
}

void heapify_upward (graph_t* g, heap_t* h, int32_t node) {									// Rebuild heap from bottom
	if (node > 0) {														// Parent exists?
		int32_t parent = (node - 1) / 2;
		int32_t dist_parent = hid2dist(parent);
		int32_t dist_child = hid2dist(node);
		if (dist_parent > dist_child) {											// Child is smaller
			heap_swap(g, h, parent, node);										// Swap parent and child
			heapify_upward(g, h, parent);										// Recurse upward
		}
	}
}

int32_t dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest, path_t* path) {					// Dijkstra PathFinder
	int32_t examiner, examinee, iter_set, iter_neighbor, dist_proposal, best_dest;
	initialize_dijkstra(g, h);												// Init. graph and heap
	for (iter_set = 0; iter_set < src->count; iter_set++) gid2dist(src->id[iter_set]) = 0;					// Set source points
	for (iter_set = h->n_elts - 1; iter_set >= 0; iter_set--) heapify_downward(g, h, iter_set);				// Build the heap
	while (h->n_elts > 0) {
		heap_swap(g, h, 0, --h->n_elts);										// Pop 1 element
		heapify_downward(g, h, 0);
		examiner = hid2gid(h->n_elts);
		for (iter_neighbor = 0; iter_neighbor < gid2vtx(examiner).n_neighbors; iter_neighbor++) {			// Go through neighbors
			examinee = gid2vtx(examiner).neighbor[iter_neighbor];
			dist_proposal = gid2dist(examiner) + gid2vtx(examiner).distance[iter_neighbor];
			if (dist_proposal < gid2dist(examinee)) {
				gid2dist(examinee) = dist_proposal;								// Update to a better dist
				gid2vtx(examinee).pred = examiner;								// Record pred. vertex
				heapify_upward(g, h, gid2hid(examinee));							// Push 1 element
			}
		}
	}
	path->tot_dist = MY_INFINITY;
	for (iter_set = 0; iter_set < dest->count; iter_set++)									// Go through dest points
		if (gid2dist(dest->id[iter_set]) < path->tot_dist) {
			best_dest = dest->id[iter_set];										// Write the best dist
			path->tot_dist = gid2dist(dest->id[iter_set]);								// Find the nearest dest
		}
	path->n_vertices = 0;
	int32_t trace_node = best_dest;
	while (1) {
		path->n_vertices++;
		if (gid2vtx(trace_node).pred == MY_INFINITY) break;
		else trace_node = gid2vtx(trace_node).pred;									// Trace through the path
	}
	for (iter_set = path->n_vertices - 1; iter_set >= 0; iter_set--) {
		path->id[iter_set] = best_dest;											// Write the path
		best_dest = gid2vtx(best_dest).pred;
	}
	if (path->n_vertices > MAX_PATH_LENGTH) path->n_vertices = 0;								// Overflow case
	if (path->n_vertices < 2) return 0;											// Underflow case
	else return 1;
}
