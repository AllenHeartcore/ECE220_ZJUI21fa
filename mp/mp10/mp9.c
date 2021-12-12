/*	###### Copyright Information ######
	This is the program (1st part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem X. 
	This program is adapted from the program (1st part) of Machine Programming Problem IX. 
	Written			Commented		Debugged		Commited
	 8 December 2021	 8 December 2021	11 December 2021	12 December 2021
	Copyright © 2021 Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp10 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	find_nodes_unsorted:	Starting at <array index "nnum"> of a <pyramid tree "p">, find vertices within range of <a locale "loc">, 
					 and write their ids into a <vertex set "vs">. 
	find_nodes:		Wrapper function of "find_nodes_unsorted" and "heapsort" for unified API. Sort the vertex set before returning. 
	trim_nodes:		Remove any vertices of a <graph "g"> not in range of a <locale "loc"> from a <vertex set "vs">. 
	dijkstra:		Find the shortest path between nodes from <src vertex set "src"> and from <destination vertex set "dest">, 
					 and write that path into <path "path">. Incorporate <heap "h"> for accelerated extraction
					 of the unvisited vertex with the smallest recorded distance. 
					 >>> Return 1 on success, or 0 on failure. <<<
	heap_swap:		[HELPER] Swap two vertices in <heap "h"> with <HIDs of "hid_1" and "hid_2">, as well as their GIDs in <graph "g">. 
	heapify_downward:	[HELPER] Recursively heapify the <heap "h"> downward from a node with <HID "node"> when initializing the heap 
					 or popping an element. Use <graph "g"> as reference to recorded best distances of each vertex. 
	heapify_upward:		[HELPER] Recursively heapify the <heap "h"> upward from a node with <HID "node"> when pushing an element. 
					 Use <graph "g"> as reference to recorded best distances of each vertex. 
	heapsort_swap:		[HELPER] Swap two elements in <array "arr"> with <indexes of "idx_1" and "idx_2">. 
					 (Modified "heap_swap" for general use)
	heapsort_downward:	[HELPER] Recursively heapify the <array "arr"> with <size "arr_size"> downward from an element with <index "idx">. 
					 (Modified "heapify_downward" for general use)
	heapsort:		[HELPER] Perform heapsort on <array "arr"> with <size "arr_size">. 
					 (Modified "dijkstra" for general use)

	For comments, refer to the original version of this program in "mp/mp9/mp9.c". 
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "mp5.h"
#include "mp10.h"
#define MY_INFINITY   1000000000
#define gid2vtx(gid)  g->vertex[gid]
#define gid2dist(gid) gid2vtx(gid).from_src
#define gid2hid(gid)  gid2vtx(gid).heap_id
#define hid2gid(hid)  h->elt[hid]
#define hid2dist(hid) gid2dist(hid2gid(hid))

void heap_swap (graph_t* g, heap_t* h, int32_t hid_1, int32_t hid_2);
void heapify_downward (graph_t* g, heap_t* h, int32_t node);
void heapify_upward (graph_t* g, heap_t* h, int32_t node);
void heapsort_swap (int32_t* arr, int32_t idx_1, int32_t idx_2);
void heapsort_downward (int32_t* arr, int32_t arr_size, int32_t idx);
void heapsort (int32_t* arr, int32_t arr_size);

void find_nodes_unsorted (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum) {
	record_fn_call();
	int32_t update = 0;
	if (4 * nnum + 1 >= p->n_nodes) {
		if (in_range(loc, p->node[nnum].x, p->node[nnum].y_left)) {
			update = 1;
			if (vs->count >= vs->id_array_size) {
				int32_t* ptr_attempt = realloc(vs->id, sizeof(int32_t) * vs->id_array_size * 2);	// DYNAMIC REALLOC
				if (ptr_attempt) {
					vs->id = ptr_attempt;
					vs->id_array_size = vs->id_array_size * 2;
				} else update = 0;
			}
			if (update == 1) vs->id[vs->count++] = p->node[nnum].id;
		}
	} else {
		if (p->node[nnum].x >= loc->x - loc->range) {
			if (in_range(loc, p->node[nnum].x, p->node[nnum].y_left)) {
				find_nodes(loc, vs, p, 4 * nnum + 1);
				find_nodes(loc, vs, p, 4 * nnum + 3);
			} else {
				if (p->node[nnum].y_left >= loc->y - loc->range) find_nodes(loc, vs, p, 4 * nnum + 1);
				if (p->node[nnum].y_left <= loc->y + loc->range) find_nodes(loc, vs, p, 4 * nnum + 3);
			}
		}
		if (p->node[nnum].x <= loc->x + loc->range) {
			if (in_range(loc, p->node[nnum].x, p->node[nnum].y_right)) {
				find_nodes(loc, vs, p, 4 * nnum + 2);
				find_nodes(loc, vs, p, 4 * nnum + 4);
			} else {
				if (p->node[nnum].y_right >= loc->y - loc->range) find_nodes(loc, vs, p, 4 * nnum + 2);
				if (p->node[nnum].y_right <= loc->y + loc->range) find_nodes(loc, vs, p, 4 * nnum + 4);
			}
		}
	}
}

void find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum) {					// Wrapper for unsorted ver.
	find_nodes_unsorted(loc, vs, p, nnum);
	heapsort(vs->id, vs->count);
}

void trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc) {
	int32_t iter_count = vs->count, iter_ptr = 0, iter_vs, iter_ovwt;
	for (iter_vs = 0; iter_vs < iter_count; iter_vs++)
		if (!in_range(loc, gid2vtx(vs->id[iter_ptr]).x, gid2vtx(vs->id[iter_ptr]).y)) {
			for (iter_ovwt = iter_ptr; iter_ovwt < vs->count - 1; iter_ovwt++)
				vs->id[iter_ovwt] = vs->id[iter_ovwt + 1];
			vs->count--;
		} else iter_ptr++;
}

int32_t dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest, path_t* path) {
	int32_t examiner, examinee, iter_set, iter_neighbor, dist_proposal, best_dest;
	h->n_elts = g->n_vertices;											// Initialize g&h
	for (int32_t iter_init = 0; iter_init < g->n_vertices; iter_init++) {
		gid2vtx(iter_init).pred = MY_INFINITY;
		gid2dist(iter_init) = MY_INFINITY;
		gid2hid(iter_init) = iter_init;
		hid2gid(iter_init) = iter_init;
	}
	for (iter_set = 0; iter_set < src->count; iter_set++) gid2dist(src->id[iter_set]) = 0;
	for (iter_set = h->n_elts - 1; iter_set >= 0; iter_set--) heapify_downward(g, h, iter_set);
	while (h->n_elts > 0) {												// Explore map
		heap_swap(g, h, 0, --h->n_elts);
		heapify_downward(g, h, 0);
		examiner = hid2gid(h->n_elts);
		for (iter_neighbor = 0; iter_neighbor < gid2vtx(examiner).n_neighbors; iter_neighbor++) {
			examinee = gid2vtx(examiner).neighbor[iter_neighbor];
			dist_proposal = gid2dist(examiner) + gid2vtx(examiner).distance[iter_neighbor];
			if (dist_proposal < gid2dist(examinee)) {
				gid2dist(examinee) = dist_proposal;
				gid2vtx(examinee).pred = examiner;
				heapify_upward(g, h, gid2hid(examinee));
			}
		}
	}
	path->tot_dist = MY_INFINITY;											// Compare dests
	for (iter_set = 0; iter_set < dest->count; iter_set++){
		if (gid2dist(dest->id[iter_set]) <= path->tot_dist) {
			best_dest = dest->id[iter_set];
			path->tot_dist = gid2dist(dest->id[iter_set]);
		}
	}
	if (gid2dist(best_dest) == MY_INFINITY) return 0;
	path->n_vertices = 0;												// Trace path
	int32_t trace_node = best_dest;
	while (1) {
		path->n_vertices++;
		if (gid2vtx(trace_node).pred == MY_INFINITY) break;
		else trace_node = gid2vtx(trace_node).pred;
	}
	if (path->n_vertices < 1) return 0;
	free(path->id);
	path->id = malloc(sizeof(int32_t) * path->n_vertices);								// DYNAMIC MALLOC
	if (!path->id) return 0;
	for (iter_set = path->n_vertices - 1; iter_set >= 0; iter_set--) {						// Write path
		path->id[iter_set] = best_dest;
		best_dest = gid2vtx(best_dest).pred;
	}
	return 1;
}

void heap_swap (graph_t* g, heap_t* h, int32_t hid_1, int32_t hid_2) {
	gid2hid(hid2gid(hid_1)) = hid_2;
	gid2hid(hid2gid(hid_2)) = hid_1;
	int32_t temp = hid2gid(hid_1);
	hid2gid(hid_1) = hid2gid(hid_2);
	hid2gid(hid_2) = temp;
}

void heapify_downward (graph_t* g, heap_t* h, int32_t node) {
	if (node * 2 + 1 < h->n_elts) {
		int32_t smallest = node;
		int32_t dist_parent = hid2dist(node);
		int32_t dist_lchild = hid2dist(node * 2 + 1);
		if (dist_lchild < dist_parent) smallest = node * 2 + 1;
		if (node * 2 + 2 < h->n_elts) {
			int32_t dist_rchild = hid2dist(node * 2 + 2);
			if (dist_rchild < dist_parent && dist_rchild < dist_lchild) smallest = node * 2 + 2;
		}
		if (smallest != node) {
			heap_swap(g, h, smallest, node);
			heapify_downward(g, h, smallest);
		}
	}
}

void heapify_upward (graph_t* g, heap_t* h, int32_t node) {
	if (node > 0) {
		int32_t parent = (node - 1) / 2;
		int32_t dist_parent = hid2dist(parent);
		int32_t dist_child = hid2dist(node);
		if (dist_parent > dist_child) {
			heap_swap(g, h, parent, node);
			heapify_upward(g, h, parent);
		}
	}
}

void heapsort_swap (int32_t* arr, int32_t idx_1, int32_t idx_2) {							// Generalized "heap_swap"
	int32_t temp = arr[idx_1];											// Directly swap values in "arr"
	arr[idx_1] = arr[idx_2];
	arr[idx_2] = temp;
}

void heapsort_downward (int32_t* arr, int32_t arr_size, int32_t idx) {							// Generalized "heapify_downward"
	if (idx * 2 + 1 < arr_size) {
		int32_t largest = idx;
		int32_t gid_parent = arr[idx];										// Directly compare values in "arr"
		int32_t gid_lchild = arr[idx * 2 + 1];
		if (gid_lchild > gid_parent) largest = idx * 2 + 1;
		if (idx * 2 + 2 < arr_size) {
			int32_t gid_rchild = arr[idx * 2 + 2];
			if (gid_rchild > gid_parent && gid_rchild > gid_lchild) largest = idx * 2 + 2;
		}
		if (largest != idx) {
			heapsort_swap(arr, largest, idx);
			heapsort_downward(arr, arr_size, largest);
		}
	}
}

void heapsort (int32_t* arr, int32_t arr_size) {
	for (int32_t iter_arr = arr_size - 1; iter_arr >= 0; iter_arr--) heapsort_downward(arr, arr_size, iter_arr);	// Construct heap
	while (arr_size > 0) {
		heapsort_swap(arr, 0, --arr_size);									// Pop only
		heapsort_downward(arr, arr_size, 0);									// Re-construct heap
	}
}
