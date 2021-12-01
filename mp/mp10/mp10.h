#if !defined(MP10_H)
#define MP10_H

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

    // added for MP10
    int32_t mm_bit;		// minimap bit number
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
    locale_t      from;		// source / starting position
    locale_t      to;		// destination / ending position

    // added for MP10
    uint32_t      uid;		// unique id
    vertex_set_t* src_vs;	// vertex set for starting points
    vertex_set_t* dst_vs;	// vertex set for starting points
    path_t*       path;		// current path
    request_t*	  partner;	// partner list
    request_t*    next;		// next in available or partner list
};

// removed for MP10
// #define MAX_IN_VERTEX_SET 100	// maximum nodes considered
struct vertex_set_t {
    int32_t count;		// number of graph vertices found

    // modified for MP10 -- use dynamic resizing
    int32_t* id;		// array of graph vertex array indices
    				//    (valid ids from 0 to count-1)
    int32_t id_array_size;	// actual array size

    // added for MP10
    uint64_t minimap;		// bits in upper levels of pyramid tree
};

struct heap_t {
    int32_t  n_elts;		// number of things in the heap
    int32_t* elt;		// array of size equal to vertices in graph
};

// removed for MP10
// #define MAX_PATH_LENGTH 500	// max length of a path
struct path_t {
    int32_t n_vertices;		// number of actual vertices in path
    int32_t tot_dist;		// sum of distances along edges in path

    // modified for MP10 -- use dynamic allocation in dijkstra
    int32_t* id;		// graph vertex array indices for path

    // added for MP10
    uint64_t minimap;		// bits in upper levels of pyramid tree
};


// These are functions that you must write in your mp10alloc.c.

// Allocate a new vertex set.  Returns NULL on failure.
extern vertex_set_t* new_vertex_set ();

// Free a vertex set and its id array.
extern void free_vertex_set (vertex_set_t* vs);

// Allocate a new path.  Returns NULL on failure.
extern path_t* new_path ();

// Free a path and its id array.
extern void free_path (path_t* path);


// These are functions that you must write in your mp10.c.

// Marks each graph vertex in g with a minimap bit number based on the
// pyramid tree p.
extern int32_t mark_vertex_minimap (graph_t* g, pyr_tree_t* p);

// Builds a minimap for a vertex set.
extern void build_vertex_set_minimap (graph_t* g, vertex_set_t* vs);

// Builds a minimap for a path.
extern void build_path_minimap (graph_t* g, path_t* p);

// Merges (intersects) two vertex sets into a third vertex set.  The
// third vertex set must have enough space in its array to hold 
// the resulting number of ids.
extern int32_t merge_vertex_sets 
	(const vertex_set_t* v1, const vertex_set_t* v2, vertex_set_t* vint);


// This is a function that you must write in your mp10match.c.

// handle a new request by attempting to find walking partners among
// previous requests; return 1 on success, 0 on failure
extern int32_t handle_request
	(graph_t* g, pyr_tree_t* p, heap_t* h, request_t* r);


// These functions are provided for you in mp10match.c.

// Prints results from request handling.
extern void print_results ();

// Illustrates results for a particular request.
extern int32_t show_results_for (graph_t* g, int32_t which);

// Frees are request-related data.
extern void free_all_data ();


// These are functions from your mp9.c and mp9match.c.

// starting at array index nnum of a pyramid tree, find vertices within range 
// of a locale and write their ids into a vertex set
extern void find_nodes (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, 
			int32_t nnum);

// find the shortest path between any node in the src vertex set and any
// node in the destination vertex set, and write that path into path;
// returns 1 on success, or 0 on failure
extern int32_t dijkstra (graph_t* g, heap_t* h, vertex_set_t* src, 
			 vertex_set_t* dest, path_t* path);


// The functions below are provided in mp10main.c.

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

// write graphical illustration of vertex set and path results to
// result.c file
extern int32_t show_find_results (graph_t* g, request_t* r1, request_t* r2);


// The functions below are provided in mp10graph.c
// (they had to be available for the request generation tool, too).

// Read a graph from a file.  Returns NULL on failure.
extern graph_t* read_graph (const char* fname);

// Free a graph structure.
extern void free_graph (graph_t* g);


#endif // MP10_H

