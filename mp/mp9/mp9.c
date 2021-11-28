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
#define gid2vtx(gid) g->vertex[gid]
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

void trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc) {
	int32_t iter_count = vs->count, iter_ptr = 0, iter_vs, iter_ovwt;
	for (iter_vs = 0; iter_vs < iter_count; iter_vs++) {
		if (!in_range(loc, gid2vtx(vs->id[iter_ptr]).x, gid2vtx(vs->id[iter_ptr]).y)) {
			for (iter_ovwt = iter_ptr; iter_ovwt < vs->count - 1; iter_ovwt++)
				vs->id[iter_ovwt] = vs->id[iter_ovwt + 1];
			vs->count--;
		} else iter_ptr++;
	}
}

void initialize_dijkstra (graph_t* g, heap_t* h) {
	h->n_elts = g->n_vertices;
	for (int32_t iter_init = 0; iter_init < g->n_vertices; iter_init++) {
		gid2vtx(iter_init).pred = MY_INFINITY;
		gid2dist(iter_init) = MY_INFINITY;
		gid2hid(iter_init) = iter_init;
		hid2gid(iter_init) = iter_init;
	}
}

void heap_swap (graph_t* g, heap_t* h, int32_t hid_1, int32_t hid_2) {
	gid2hid(hid2gid(hid_1)) = hid_2;
	gid2hid(hid2gid(hid_2)) = hid_1;
	int32_t temp = hid2gid(hid_1);
	hid2gid(hid_1) = hid2gid(hid_2);
	hid2gid(hid_2) = temp;
}

void heapify (graph_t* g, heap_t* h, int32_t node) {
	if (node * 2 + 1 < h->n_elts) {
		int32_t smallest = node;
		int32_t dist_parent = hid2dist(node);
		int32_t dist_lchild = hid2dist(node * 2 + 1);
		if (dist_lchild < dist_parent) smallest = node * 2 + 1;
		if (node * 2 + 2 < h->n_elts) {
			int32_t dist_rchild = hid2dist(node * 2 + 2);
			if (dist_rchild < dist_parent) smallest = node * 2 + 2;
		}
		if (smallest != node) {
			heap_swap(g, h, smallest, node);
			heapify(g, h, smallest);
		}
	}
}

void update_neighbors(graph_t* g, int32_t examiner) {
	for (int32_t iter_neighbor = 0; iter_neighbor < gid2vtx(examiner).n_neighbors; iter_neighbor++) {
		int32_t examinee = gid2vtx(examiner).neighbor[iter_neighbor];
		int32_t dist_proposal = gid2dist(examiner) + gid2vtx(examiner).distance[iter_neighbor];
		if (dist_proposal < gid2dist(examinee)) {
			gid2dist(examinee) = dist_proposal;
			gid2vtx(examinee).pred = examiner;
			update_neighbors(g, examinee);
		}
	}
}

int32_t dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, vertex_set_t* dest, path_t* path) {
	int32_t examiner, iter_set, best_dest;
	initialize_dijkstra(g, h);
	for (int32_t iter_src = 0; iter_src < src->count; iter_src++) gid2dist(src->id[iter_src]) = 0;
	for (int32_t iter_hid = h->n_elts - 1; iter_hid >= 0; iter_hid--) heapify(g, h, iter_hid);
	while (h->n_elts > 0) {
		heap_swap(g, h, 0, --h->n_elts);
		examiner = hid2gid(h->n_elts);
		update_neighbors(g, examiner);
		for (int32_t iter_hid = h->n_elts - 1; iter_hid >= 0; iter_hid--) heapify(g, h, iter_hid);
	}
	path->tot_dist = MY_INFINITY;
	for (iter_set = 0; iter_set < dest->count; iter_set++)
		if (gid2dist(dest->id[iter_set]) < path->tot_dist) {
			best_dest = dest->id[iter_set];
			path->tot_dist = gid2dist(dest->id[iter_set]);
		}
	path->n_vertices = 0;
	int32_t trace_node = best_dest;
	while (1) {
		path->n_vertices++;
		if (gid2vtx(trace_node).pred == MY_INFINITY) break;
		else trace_node = gid2vtx(trace_node).pred;
	}
	for (iter_set = path->n_vertices - 1; iter_set >= 0; iter_set--) {
		path->id[iter_set] = best_dest;
		best_dest = gid2vtx(best_dest).pred;
	}
	if (path->n_vertices > MAX_PATH_LENGTH) path->n_vertices = 0;
	if (path->n_vertices < 2) return 0;
	else return 1;
}
