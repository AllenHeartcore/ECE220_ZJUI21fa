#if !defined(MP9_H)
#define MP9_H

#include <stdint.h>

// structures defined for your use
typedef struct vertex_t vertex_t;
typedef struct graph_t graph_t;
typedef struct pyr_node_t pyr_node_t;
typedef struct pyr_tree_t pyr_tree_t;
typedef struct locale_t locale_t;
typedef struct request_t request_t;
typedef struct vertex_set_t vertex_set_t;
typedef struct heap_t heap_t;
typedef struct path_t path_t;

struct vertex_t {
    int32_t  x;			// x position
    int32_t  y;			// y position
    int32_t  n_neighbors;	// number of neighbors
    // arrays of size n_neighbors
    int32_t* neighbor;		// neighbor indices in graph's vertex array
    int32_t* distance;		// Euclidean distance to neighbors

    // for use with Dijkstra's single-source shortest-path algorithm
    int32_t heap_id;		// heap array index for this vertex 
    int32_t from_src;		// distance from source
    int32_t pred;		// predecessor graph node (array index)
};

struct graph_t {
    int32_t   n_vertices;	// number of vertices in the graph
    vertex_t* vertex;		// array of vertices
};

struct pyr_node_t {
    int32_t x;			// x position of node / x-splitter
    int32_t y_left;		// y position of node / left y-splitter
    int32_t y_right;		// y position of node / right y-splitter
    int32_t id;			// graph vertex array index (for leaf nodes)
};

struct pyr_tree_t {
    int32_t     n_nodes;	// number of nodes in tree
    pyr_node_t* node;		// array of nodes
};

struct locale_t {
    int32_t x;			// x position of location
    int32_t y;			// y position of location
    int32_t range;		// maximum distance to chosen node 
};

struct request_t {
    locale_t from;		// source / starting position
    locale_t to;		// destination / ending position
};

#define MAX_IN_VERTEX_SET 100	// maximum nodes considered
struct vertex_set_t {
    int32_t count;		  // number of graph vertices found
    int32_t id[MAX_IN_VERTEX_SET];// array of graph vertex array indices
    				  //    (valid ids from 0 to count-1)
};

struct heap_t {
    int32_t  n_elts;		// number of things in the heap
    int32_t* elt;		// array of size equal to vertices in graph
};

#define MAX_PATH_LENGTH 500	// max length of a path
struct path_t {
    int32_t n_vertices;		// number of actual vertices in path
    int32_t tot_dist;		// sum of distances along edges in path
    int32_t id[MAX_PATH_LENGTH];// graph vertex array indices for path
};


// starting at array index nnum of a pyramid tree, find vertices within range 
// of a locale and write their ids into a vertex set
extern void find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, 
			int32_t nnum);

// remove any vertices not in range of a locale from a vertex set  
extern void trim_nodes (graph_t* g, vertex_set_t* vs, locale_t* loc);

// find the shortest path between any node in the src vertex set and any
// node in the destination vertex set, and write that path into path;
// returns 1 on success, or 0 on failure
extern int32_t dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, 
			 vertex_set_t* dest, path_t* path);

// find graph vertices within range of the starting and ending locales
// for two requests and find a shortest path between any vertex in the
// starting set and any vertex in the ending set.  return 1 on success,
// or 0 on failure
extern int32_t match_requests 
	(graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r1, request_t* r2,
	 vertex_set_t* src_vs, vertex_set_t* dst_vs, path_t* path);

// the functions below are provided in mp9main.c

// a version of find_nodes that counts the number of calls made.
// we will use this version to measure your recursion efficiency.
extern int32_t find_nodes_inst 
	(locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum);

// a subroutine that counts one call to find_nodes
extern void record_fn_call ();

// returns 1 if point (x,y) is within range of the given locale, or
// 0 otherwise; your code MUST use this function to check whether
// pyramid leaf nodes and graph vertices are within a locale
extern int32_t in_range (locale_t* loc, int32_t x, int32_t y);

#endif // MP9_H

