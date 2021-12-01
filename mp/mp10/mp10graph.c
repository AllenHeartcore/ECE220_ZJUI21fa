#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp10.h"


// limits on graph size; no particular reason for these, but
// wanted to limit to something to avoid garbage in files
#define MAX_VERTICES  65536
#define MAX_NEIGHBORS 8


void
free_graph (graph_t* g)
{
    int32_t vnum;

    for (vnum = g->n_vertices; 0 < vnum--; ) {
        free (g->vertex[vnum].neighbor);
        free (g->vertex[vnum].distance);
    }
    free (g);
}


graph_t*
read_graph (const char* fname)
{
    FILE* f = fopen (fname, "r");
    graph_t* g;
    int32_t vnum;
    int32_t edge;
    double dx;
    double dy;
    int32_t node;
    int32_t rev;

    if (NULL == f) {
	perror ("open graph file");
        return NULL;
    }
    if (NULL == (g = malloc (sizeof (*g)))) {
        perror ("malloc graph");
	fclose (f);
	return NULL;
    }
    if (1 != fscanf (f, "%d", &g->n_vertices) || 
        1 > g->n_vertices || MAX_VERTICES < g->n_vertices) {
	fputs ("Bad number of vertices.\n", stderr);
	free (g);
	fclose (f);
	return NULL;
    }
    if (NULL == (g->vertex = malloc (g->n_vertices * sizeof (*g->vertex)))) {
        perror ("malloc vertices");
	free (g);
	fclose (f);
	return NULL;
    }
    for (vnum = 0; g->n_vertices > vnum; vnum++) {
        if (3 != fscanf (f, "%d%d%d", &g->vertex[vnum].x, &g->vertex[vnum].y, 
			 &g->vertex[vnum].n_neighbors) ||
//	    0 > g->vertex[vnum].x || WIDTH <= g->vertex[vnum].x ||
//	    0 > g->vertex[vnum].y || P_HEIGHT <= g->vertex[vnum].y ||
	    0 > g->vertex[vnum].n_neighbors || 
	    MAX_NEIGHBORS < g->vertex[vnum].n_neighbors) {
	    fprintf (stderr, "Node %d missing/bad data.\n", vnum);
	    g->n_vertices = vnum;
	    free_graph (g);
	    fclose (f);
	    return NULL;
	}
	if (NULL == (g->vertex[vnum].neighbor = 
		     malloc (g->vertex[vnum].n_neighbors * 
		             sizeof (*g->vertex[vnum].neighbor))) ||
	    NULL == (g->vertex[vnum].distance = 
		     malloc (g->vertex[vnum].n_neighbors * 
		             sizeof (*g->vertex[vnum].distance)))) {
	    perror ("malloc neighbor/distance array");
	    if (NULL != g->vertex[vnum].neighbor) {
		free (g->vertex[vnum].neighbor);
	    }
	    g->n_vertices = vnum;
	    free_graph (g);
	    fclose (f);
	    return NULL;
	}
	for (edge = 0; g->vertex[vnum].n_neighbors > edge; edge++) {
	    if (1 != fscanf (f, "%d", &g->vertex[vnum].neighbor[edge]) ||
	        0 > g->vertex[vnum].neighbor[edge] ||
	        g->n_vertices <= g->vertex[vnum].neighbor[edge]) {
		fprintf (stderr, "Node %d has a bad neighbor.\n", vnum);
		g->n_vertices = vnum + 1;
		free_graph (g);
		fclose (f);
		return NULL;
	    }
	    node = g->vertex[vnum].neighbor[edge];
	    if (vnum >= node) {
		dx = g->vertex[vnum].x - g->vertex[node].x;
		dy = g->vertex[vnum].y - g->vertex[node].y;
	        g->vertex[vnum].distance[edge] = sqrt (dx * dx + dy * dy);
		for (rev = g->vertex[node].n_neighbors; 0 < rev--; ) {
		     if (vnum == g->vertex[node].neighbor[rev]) {
		         g->vertex[node].distance[rev] = 
				 g->vertex[vnum].distance[edge];
			 break;
		     }
		}
	    }
	}
    }
    fclose (f);
    return g;
}


