#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp10.h"


static int32_t
pick_node (graph_t* g, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    int32_t tries;
    int32_t pick;

    for (tries = 0; 10000 > tries; tries++) {
        pick = (rand () % g->n_vertices);
	if (x1 <= g->vertex[pick].x && x2 >= g->vertex[pick].x &&
	    y1 <= g->vertex[pick].y && y2 >= g->vertex[pick].y) {
	    return pick;
	}
    }
    return -1;
}


int
main (int argc, const char* const argv[])
{
    FILE* output;
    graph_t* g;
    int32_t n_req;
    uint32_t seed;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    int32_t min_range;
    int32_t max_range;
    int32_t i;
    int32_t ymid;
    int32_t src;
    int32_t dst;
    int32_t src_range;
    int32_t dst_range;
    int32_t src_dx;
    int32_t src_dy;
    int32_t dst_dx;
    int32_t dst_dy;

    if (11 != argc ||
        1 != sscanf (argv[3], "%d", &n_req) || 2 > n_req || 1000000 < n_req ||
	1 != sscanf (argv[4], "%u", &seed) ||
	1 != sscanf (argv[5], "%d", &x1) || 0 > x1 || 2000000 < x1 ||
	1 != sscanf (argv[6], "%d", &y1) || 0 > y1 || 2000000 < y1 ||
	1 != sscanf (argv[7], "%d", &x2) || 0 > x2 || 2000000 < x2 ||
	1 != sscanf (argv[8], "%d", &y2) || 0 > y2 || 2000000 < y2 ||
	x1 >= x2 || y1 >= y2 ||
	1 != sscanf (argv[9], "%d", &min_range) || 100 > min_range || 
		200000 < min_range ||
	1 != sscanf (argv[10], "%d", &max_range) || 100 > max_range || 
		200000 < max_range ||
	min_range >= max_range) {
	fprintf (stderr, "syntax: %s <graph file> <requests file> <# requests> <seed> <x1> <y1> <x2> <y2> <min range> <max range>\n", argv[0]);
	fprintf (stderr, "        # requests from 2 to 1,000,000\n");
	fprintf (stderr, "        coordinates from 0 to 2,000,000 (x1 < x2, y1 < y2)\n");
	fprintf (stderr, "        ranges from 100 to 200,000 (min < max)\n");
	return 2;
    }
    ymid = y1 + (y2 - y1) / 2;

    if (NULL == (g = read_graph (argv[1]))) {
        return 3;
    }

    if (NULL == (output = fopen (argv[2], "w"))) {
        free_graph (g);
	return 3;
    }

    srand (seed);
    for (i = 0; n_req > i; i++) {
        src = pick_node (g, x1, ymid, x2, y2);
        dst = pick_node (g, x1, y1, x2, ymid);
	if (-1 == src || -1 == dst) {
	    fprintf (stderr, "Could not find node in specified range!\n");
	} else {
	    src_range = min_range + (rand () % (max_range - min_range + 1));
	    dst_range = min_range + (rand () % (max_range - min_range + 1));
	    do {
		src_dx = (rand () % src_range) - src_range / 2;
		src_dy = (rand () % src_range) - src_range / 2;
	    } while (src_range * src_range < src_dx * src_dx + src_dy * src_dy);
	    do {
		dst_dx = (rand () % dst_range) - dst_range / 2;
		dst_dy = (rand () % dst_range) - dst_range / 2;
	    } while (dst_range * dst_range < dst_dx * dst_dx + dst_dy * dst_dy);
	    fprintf (output, "%d %d %d %d %d %d\n",
		     g->vertex[src].x + src_dx, g->vertex[src].y + src_dy,
		     src_range,
		     g->vertex[dst].x + dst_dx, g->vertex[dst].y + dst_dy,
		     dst_range);
	}
    }

    fclose (output);
    free_graph (g);
    
    return 0;
}

