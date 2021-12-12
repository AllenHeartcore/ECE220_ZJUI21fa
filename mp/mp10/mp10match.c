/*	###### Copyright Information ######
	This is the program (4th part) for ECE220FA21 @ ZJUI Institute, Machine Programming Problem X. 
	Written			Commented		Debugged		Commited
	 9 December 2021	10 December 2021	11~12 December 2021	12 December 2021
	Functions other than "cleanup" and "handle_request" are written by Steve Lumetta. 
	Copyright © 2021 Lumetta, Steve and Chen, Ziyuan. 

	###### Functionality ######
	Compile by running:	make; make clean
	Execute by running:	./mp10 graph requests; make image
	The program outputs:	A .png file named "image.png"

	###### Function Documentation ######
	cleanup:	[HELPER] Free spaces of up to 6 vertex sets, 1 path, and 2 int32_t arrays. Extensively used in "handle_request". 
	handle_request:	Handle a single request containing 2 locale_h fields "from" and "to", and 1 uint32_t field "uid". 
				 This function operates on a 7-step basis:
					1. Alloc VSR & VSM
					2. Alloc PM				[Quit on alloc failure]
					3. Fill  VSR				[Quit on empty VSR]
					4. Build MM-VSR
					5. Alloc IDM				[Quit on alloc failure]
					6. Traverse through "available"
						6.1.  Alloc VSC 		[Quit on alloc failure]
						6.2.  Fill  VSC			[Continue on non-empty VSC]
						6.3.  Build MM-VSC		[Continue on matched MM-VSC, intersecting VSM, and valid PM]
					7A. Paired case
						7A.1. Delete candidate from "available"
						7A.2. Cleanup & overwrite VSR & VSC with VSM
						7A.3. Build MM-VSM & MM-PM
						7A.4. Link R & C into "shared"
					7B. Unpaired case
						7B.1. Link R into "available"
						7B.2. Final cleanup
				Abbreviations:
					R   - Request
					C   - Candidate
					MM  - Minimap
					VSR - Vertex sets for request
					VSC - Vertex sets for candidate
					VSM - Vertex sets for matching (tests)
					PM  - Path for matching
					IDM - "id" arrays for matching
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp5.h"
#include "mp10.h"


//
// These variables hold the heads of two singly-linked lists of 
// requests.  
//
// The avaialble list consists of unpaired requests: partner 
// should be NULL, and next is used to form the list.   
// 
// The shared list consists of groups with non-empty start and end 
// vertex intersections.  Only one of the requests in the group is in 
// the list.  The others are linked through the first's partner field 
// and then through the next field of the others in the group.  The 
// next field of the first request in a group is used to form the
// shared list.
//
// Note: for MP2, you should only build groups of two in the shared
// list.
//

static request_t* available = NULL;
static request_t* shared = NULL;


void cleanup (vertex_set_t* vs1, vertex_set_t* vs2, vertex_set_t* vs3, vertex_set_t* vs4, 
		vertex_set_t* vs5, vertex_set_t* vs6, path_t* p, int32_t* id1, int32_t* id2) {
	if (id1) free(id1);
	if (id2) free(id2);
	if (vs1) free_vertex_set(vs1);
	if (vs2) free_vertex_set(vs2);
	if (vs3) free_vertex_set(vs3);
	if (vs4) free_vertex_set(vs4);
	if (vs5) free_vertex_set(vs5);
	if (vs6) free_vertex_set(vs6);
	if (p) free_path(p);
}

int32_t handle_request (graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r) {
	printf("Checking request #%04d\n", r->uid);
	r->src_vs = new_vertex_set();								// 1. Allocate vertex sets for request (VSR)
	r->dst_vs = new_vertex_set();
	vertex_set_t* matching_src = malloc(sizeof(vertex_set_t));				// 1. Allocate vertex sets for matching (VSM)
	vertex_set_t* matching_dst = malloc(sizeof(vertex_set_t));
	r->path = new_path();									// 2. Allocate path for matching (PM)
	if (!r->src_vs || !r->dst_vs || !matching_src || !matching_dst || !r->path) {		// 	[Quit on allocation failure]
		cleanup(r->src_vs, r->dst_vs, NULL, NULL, 
		  matching_src, matching_dst, r->path, NULL, NULL);
		return 0;
	}
	find_nodes(&r->from, r->src_vs, p, 0);							// 3. Fill in VSR
	find_nodes(&r->to, r->dst_vs, p, 0);
	if (!r->src_vs->count || !r->dst_vs->count) {						// 	[Quit on empty VSR]
		cleanup(r->src_vs, r->dst_vs, NULL, NULL, 
		  matching_src, matching_dst, r->path, NULL, NULL);
		return 0;
	}
	mark_vertex_minimap(g, p);								// 4. Build minimaps of VSR
	build_vertex_set_minimap(g, r->src_vs);
	build_vertex_set_minimap(g, r->dst_vs);
	matching_src->id = malloc(sizeof(int32_t) * r->src_vs->count);				// 5. Allocate "id" arrays for matching (IDM)
	matching_dst->id = malloc(sizeof(int32_t) * r->dst_vs->count);
	if (!matching_src->id || !matching_dst->id) {						// 	[Quit on allocation failure]
		cleanup(r->src_vs, r->dst_vs, NULL, NULL, 
		  matching_src, matching_dst, r->path, matching_src->id, matching_dst->id);
		return 0;
	}
	matching_src->id_array_size = r->src_vs->count;						// 	Manually init "src" IDM
	matching_src->count = matching_src->minimap = 0;
	matching_dst->id_array_size = r->dst_vs->count;						// 	Manually init "dst" IDM
	matching_dst->count = matching_dst->minimap = 0;
	request_t* candidate = available;							// 6. Traverse through "available" linked list
	request_t* candidate_former;
	while (candidate) {
		candidate->src_vs = new_vertex_set();						// 	Allocate vertex sets for candidate (VSC)
		candidate->dst_vs = new_vertex_set();
		if (!candidate->src_vs || !candidate->dst_vs) {					// 	[Quit on allocation failure]
			cleanup(r->src_vs, r->dst_vs, candidate->src_vs, candidate->dst_vs, 
			  matching_src, matching_dst, r->path, matching_src->id, matching_dst->id);
			return 0;
		}
		find_nodes(&candidate->from, candidate->src_vs, p, 0);				// 	Fill in VSC
		find_nodes(&candidate->to, candidate->dst_vs, p, 0);
		if (candidate->src_vs->count && candidate->dst_vs->count) {			// 	[Continue on non-empty VSC]
			build_vertex_set_minimap(g, candidate->src_vs);				// 	Build minimaps of VSC
			build_vertex_set_minimap(g, candidate->dst_vs);
			if ((r->src_vs->minimap & candidate->src_vs->minimap)			// 	[Continue on matched minimaps]
			  && (r->dst_vs->minimap & candidate->dst_vs->minimap)
			  && merge_vertex_sets(r->src_vs, candidate->src_vs, matching_src)	// 	[Continue on intersecting VSM]
			  && merge_vertex_sets(r->dst_vs, candidate->dst_vs, matching_dst)
			  && dijkstra(g, h, matching_src, matching_dst, r->path)) {		// 	[Continue on valid PM]
				if (candidate == available) available = candidate->next;	// 7A. Paired case
				else candidate_former->next = candidate->next;			// 	Delete candidate from "available"
				cleanup(r->src_vs, r->dst_vs, candidate->src_vs, candidate->dst_vs, 
				  NULL, NULL, NULL, NULL, NULL);
				r->src_vs = candidate->src_vs = matching_src;			// 	Overwrite VSR and VSC with VSM
				r->dst_vs = candidate->dst_vs = matching_dst;
				r->path = candidate->path = r->path;
				build_vertex_set_minimap(g, matching_src);			// 	Build minimaps of VSM and PM
				build_vertex_set_minimap(g, matching_dst);
				build_path_minimap(g, r->path);
				candidate->partner = candidate->next = NULL;			// 	Link partners into "shared"
				r->partner = candidate;
				r->next = shared;
				shared = r;
				return 1;
			}
		} else cleanup(NULL, NULL, candidate->src_vs, candidate->dst_vs, NULL, NULL, NULL, NULL, NULL);
		candidate_former = candidate;
		candidate = candidate->next;
	}
	r->next = available;									// 7B. Unpaired case
	available = r;										// 	Link request into "available"
	cleanup(NULL, NULL, NULL, NULL, NULL, NULL, NULL, matching_src->id, matching_dst->id);
	return 1;
}


void
print_results ()
{
    request_t* r;
    request_t* prt;

    printf ("Matched requests:\n");
    for (r = shared; NULL != r; r = r->next) {
        printf ("%5d", r->uid);
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    printf (" %5d", prt->uid);
	}
	printf (" src=%016lX dst=%016lX path=%016lX\n", r->src_vs->minimap,
		r->dst_vs->minimap, r->path->minimap);
    }

    printf ("\nUnmatched requests:\n");
    for (r = available; NULL != r; r = r->next) {
        printf ("%5d src=%016lX dst=%016lX\n", r->uid, r->src_vs->minimap,
		r->dst_vs->minimap);
    }
}


int32_t
show_results_for (graph_t* g, int32_t which)
{
    request_t* r;
    request_t* prt;

    // Can only illustrate one partner.
    for (r = shared; NULL != r; r = r->next) {
	if (which == r->uid) {
	    return show_find_results (g, r, r->partner);
	}
	for (prt = r->partner; NULL != prt; prt = prt->next) {
	    if (which == prt->uid) {
		return show_find_results (g, prt, r);
	    }
	}
    }

    for (r = available; NULL != r; r = r->next) {
        if (which == r->uid) {
	    return show_find_results (g, r, r);
	}
    }
    return 0;
}


static void
free_request (request_t* r)
{
    free_vertex_set (r->src_vs);
    free_vertex_set (r->dst_vs);
    if (NULL != r->path) {
	free_path (r->path);
    }
    free (r);
}

void
free_all_data ()
{
    request_t* r;
    request_t* prt;
    request_t* next;

    // All requests in a group share source and destination vertex sets
    // as well as a path, so we need free those elements only once.
    for (r = shared; NULL != r; r = next) {
	for (prt = r->partner; NULL != prt; prt = next) {
	    next = prt->next;
	    free (prt);
	}
	next = r->next;
	free_request (r);
    }

    for (r = available; NULL != r; r = next) {
	next = r->next;
	free_request (r);
    }
}


