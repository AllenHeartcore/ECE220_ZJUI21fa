#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp5.h"
#include "mp9.h"

// limits on graph size; no particular reason for these, but
// wanted to limit to something to avoid garbage in files
#define MAX_VERTICES  65536
#define MAX_NEIGHBORS 8

// helper structure for building a pyramid tree
typedef struct pos_t pos_t;
struct pos_t {
    int32_t x;
    int32_t y;
    int32_t id;
};


static void
free_graph (graph_t* g)
{
    int32_t vnum;

    for (vnum = g->n_vertices; 0 < vnum--; ) {
        free (g->vertex[vnum].neighbor);
        free (g->vertex[vnum].distance);
    }
    free (g);
}


static graph_t*
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


static int32_t
read_requests (const char* fname, request_t* r1, request_t* r2)
{
    FILE* f = fopen (fname, "r");

    if (NULL == f) {
	perror ("open graph file");
        return 0;
    }
    if (6 != fscanf (f, "%d%d%d%d%d%d", &r1->from.x, &r1->from.y,
    		     &r1->from.range, &r1->to.x, &r1->to.y, &r1->to.range) ||
	6 != fscanf (f, "%d%d%d%d%d%d", &r2->from.x, &r2->from.y,
    		     &r2->from.range, &r2->to.x, &r2->to.y, &r2->to.range)) {
        fprintf (stderr, "Could not parse request file.\n");
	fclose (f);
	return 0;
    }
    fclose (f);
    return 1;
}


static void
free_heap (heap_t* h)
{
    free (h->elt);
    free (h);
}

static heap_t*
make_heap_for_graph (graph_t* g)
{
    heap_t* h;

    if (NULL == (h = malloc (sizeof (*h))) ||
        NULL == (h->elt = malloc (g->n_vertices * sizeof (*h->elt)))) {
	if (NULL != h) {
	    free (h);
	}
        return NULL;
    }
    return h;
}


static void
free_pyramid_tree (pyr_tree_t* p)
{
    free (p->node);
    free (p);
}


static int
sort_by_x (const void* p1, const void* p2)
{
    const pos_t* pos1 = p1;
    const pos_t* pos2 = p2;

    if (pos1->x < pos2->x) {
        return -1;
    } 
    if (pos1->x > pos2->x) {
        return 1;
    }
    return 0;
}


static int
sort_by_y (const void* p1, const void* p2)
{
    const pos_t* pos1 = p1;
    const pos_t* pos2 = p2;

    if (pos1->y < pos2->y) {
        return -1;
    } 
    if (pos1->y > pos2->y) {
        return 1;
    }
    return 0;
}


static void
fill_pyramid_tree (pyr_tree_t* p, int32_t nnum, pos_t* pos, 
	           int32_t n_vertices, int32_t full)
{
    int32_t rem, part;
    int32_t upper_left, upper_right, left, right;

//if (0 == n_vertices) { printf ("error");}
//printf ("fpt: node %5d -- %d vertices, %d\n", nnum, n_vertices, full);

    if (0 == full || 1 == n_vertices) {
        p->node[nnum].x = pos[0].x;
        p->node[nnum].y_left = p->node[nnum].y_right = pos[0].y;
	p->node[nnum].id = pos[0].id;
	return;
    } 
    qsort (pos, n_vertices, sizeof (*pos), sort_by_x);

    if (1 == full) {
	upper_left = 1;
	upper_right = (1 < n_vertices);
        left = upper_left + (2 < n_vertices);
    } else {
	rem = n_vertices - full;
	part = 3 * full / 4;
        upper_left = full / 4;
	if (part >= rem) {
	    upper_left += rem;
	    rem = 0;
	} else {
	    upper_left += part;
	    rem -= part;
	}
	upper_right = full / 4;
	if (part >= rem) {
	    upper_right += rem;
	    rem = 0;
	} else {
	    upper_right += part;
	    rem -= part;
	}
	left = upper_left + full / 4;
	if (part >= rem) {
	    left += rem;
	    rem = 0;
	} else {
	    left += part;
	    rem -= part;
	}
    }
    right = n_vertices - left;
    p->node[nnum].x = 
	pos[left - 1].x + (pos[left].x - pos[left - 1].x) / 2;
    if (1 < left) {
	qsort (pos, left, sizeof (*pos), sort_by_y);
	p->node[nnum].y_left = pos[upper_left - 1].y + 
		(pos[upper_left].y - pos[upper_left - 1].y) / 2;
    } else {
	p->node[nnum].y_left = pos[0].y;
    }
    fill_pyramid_tree (p, 4 * nnum + 1, pos, upper_left, full / 4);
    if (upper_left < left) {
	fill_pyramid_tree (p, 4 * nnum + 3, pos + upper_left, 
			   left - upper_left, full / 4);
    }
    pos += left;
    if (1 < right) {
	qsort (pos, right, sizeof (*pos), sort_by_y);
	p->node[nnum].y_right = pos[upper_right - 1].y + 
		(pos[upper_right].y - pos[upper_right - 1].y) / 2;
    } else {
	p->node[nnum].y_right = pos[0].y;
    }
    fill_pyramid_tree (p, 4 * nnum + 2, pos, upper_right, full / 4);
    if (upper_right < right) {
	fill_pyramid_tree (p, 4 * nnum + 4, pos + upper_right, 
			   right - upper_right, full / 4);
    }
}


static pyr_tree_t*
make_pyramid_tree (graph_t* g)
{
    int32_t tot = 0;
    int32_t amt = 1;
    pos_t* pos;
    int32_t i;
    pyr_tree_t* p;

    while (amt < g->n_vertices) {
	tot += amt;
        amt *= 4;
    }
    tot += (4 * g->n_vertices - amt + 2) / 3;
    amt /= 4;

    if (NULL == (pos = malloc (g->n_vertices * sizeof (*pos)))) {
        return NULL;
    }
    for (i = g->n_vertices; 0 < i--; ) {
        pos[i].x = g->vertex[i].x;
        pos[i].y = g->vertex[i].y;
        pos[i].id = i;
    }

    if (NULL == (p = malloc (sizeof (*p))) ||
        NULL == (p->node = malloc (tot * sizeof (*p->node)))) {
	free (pos);
	if (NULL != p) {
	    free (p);
	}
	return NULL;
    }
    p->n_nodes = tot;
    fill_pyramid_tree (p, 0, pos, g->n_vertices, amt);
    free (pos);
    return p;
}


#if 0

// used for debugging pyramid tree building code

static void
check_pyramid_tree (pyr_tree_t* p, int32_t nnum, int32_t minx, int32_t maxx,
		    int32_t miny, int32_t maxy)
{
    int32_t child;

    if (p->node[nnum].x < minx || p->node[nnum].x > maxx ||
	p->node[nnum].y_left < miny || p->node[nnum].y_left > maxy ||
	p->node[nnum].y_right < miny || p->node[nnum].y_right > maxy) {
	printf ("%d is bad\n", nnum);
	return;
    }
    child = 4 * nnum + 1;
    if (p->n_nodes > child) {
        check_pyramid_tree (p, child, minx, p->node[nnum].x, 
			    miny, p->node[nnum].y_left);
    }
    child++;
    if (p->n_nodes > child) {
        check_pyramid_tree (p, child, p->node[nnum].x, maxx,
			    miny, p->node[nnum].y_right);
    }
    child++;
    if (p->n_nodes > child) {
        check_pyramid_tree (p, child, minx, p->node[nnum].x, 
			    p->node[nnum].y_left, maxy);
    }
    child++;
    if (p->n_nodes > child) {
        check_pyramid_tree (p, child, p->node[nnum].x, maxx,
			    p->node[nnum].y_right, maxy);
    }
}

#endif

int32_t
in_range (locale_t* loc, int32_t x, int32_t y)
{
    double dx = loc->x - x;
    double dy = loc->y - y;

    return (sqrt (dx * dx + dy * dy) <= loc->range);
}


static int32_t fn_count;

int32_t
find_nodes_inst (locale_t* loc, vertex_set_t* vs, pyr_tree_t* p, int32_t nnum)
{
    fn_count = 0;
    find_nodes (loc, vs, p, nnum);
    return fn_count;
}

void
record_fn_call ()
{
    fn_count++;
}

static int32_t
show_find_results (graph_t* g, request_t* r1, request_t* r2,
		   vertex_set_t* src_vs, vertex_set_t* dst_vs, path_t* path)
{
    FILE* f;
    int32_t v;
    int32_t i;
    int32_t edge;
    int32_t n;
    int32_t dx;
    int32_t dy;
    int32_t xmid;
    int32_t ymid;
    double xscale;
    double yscale;
    double scale;

    if (2 > path->n_vertices) { return 0;}

    dx = (g->vertex[path->id[path->n_vertices - 1]].x -
	  g->vertex[path->id[0]].x);
    dy = (g->vertex[path->id[path->n_vertices - 1]].y -
	  g->vertex[path->id[0]].y);
    xmid = g->vertex[path->id[0]].x + dx / 2;
    ymid = g->vertex[path->id[0]].y + dy / 2;

    if (0 == dx) {
        if (0 == dy) { 
	    return 0;
	}
	scale = HEIGHT / (1.5 * (dy > 0 ? dy : -dy));
    } else if (0 == dy) {
	scale = WIDTH / (1.5 * (dx > 0 ? dx : -dx));
    } else {
	xscale = WIDTH / (1.5 * (dx > 0 ? dx : -dx));
	yscale = HEIGHT / (1.5 * (dy > 0 ? dy : -dy));
	scale = (xscale < yscale ? xscale : yscale);
    }

    // circle in light yellow
    // roads in grey, 
    // path in black
    // all nodes in blue
    // chosen nodes in green

    if (NULL == (f = fopen ("result.c", "w"))) {
        perror ("write result.c");
	return 0;
    }
        
    fprintf (f, "#include \"mp5.h\"\nvoid\ndraw_map ()\n{\n");

    fprintf (f, "    set_color (0xFFFF80);\n");
    fprintf (f, "    draw_circle (%d,%d,%d,%d);\n",
	     WIDTH / 2 + ((int32_t)(scale * (r1->from.x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (r1->from.y - ymid))),
    	     0, 
	     (int32_t)(scale * r1->from.range));
    fprintf (f, "    draw_circle (%d,%d,%d,%d);\n", 
	     WIDTH / 2 + ((int32_t)(scale * (r1->to.x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (r1->to.y - ymid))),
    	     0, 
	     (int32_t)(scale * r1->to.range));

    fprintf (f, "    set_color (0xFFC080);\n");
    fprintf (f, "    draw_circle (%d,%d,%d,%d);\n",
	     WIDTH / 2 + ((int32_t)(scale * (r2->from.x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (r2->from.y - ymid))),
    	     0, 
	     (int32_t)(scale * r2->from.range));
    fprintf (f, "    draw_circle (%d,%d,%d,%d);\n", 
	     WIDTH / 2 + ((int32_t)(scale * (r2->to.x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (r2->to.y - ymid))),
    	     0, 
	     (int32_t)(scale * r2->to.range));

    fprintf (f, "    set_color (0xC0C0C0);\n");
    for (v = g->n_vertices; 0 < v--; ) {
        for (edge = g->vertex[v].n_neighbors; 0 < edge--; ) {
	    n = g->vertex[v].neighbor[edge];
	    fprintf (f, "    draw_line (%d,%d,%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[v].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[v].y - ymid))),
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[n].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[n].y - ymid))));
	}
    }

    fprintf (f, "    set_color (0x000000);\n");
    for (i = 0, v = path->id[0]; path->n_vertices - 1 > i; i++, v = n) {
	n = path->id[i + 1];
	fprintf (f, "    draw_line (%d,%d,%d,%d);\n",
	     WIDTH / 2 + ((int32_t)(scale * (g->vertex[v].x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (g->vertex[v].y - ymid))),
	     WIDTH / 2 + ((int32_t)(scale * (g->vertex[n].x - xmid))),
	     HEIGHT / 2 + ((int32_t)(scale * (g->vertex[n].y - ymid))));
    }

    fprintf (f, "    set_color (0x0000FF);\n");
    for (v = g->n_vertices; 0 < v--; ) {
	fprintf (f, "    draw_dot (%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[v].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[v].y - ymid))));
    }

    fprintf (f, "    set_color (0x00FF00);\n");
    for (v = src_vs->count; 0 < v--; ) {
	i = src_vs->id[v];
	fprintf (f, "    draw_dot (%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[i].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[i].y - ymid))));
    }
    for (v = dst_vs->count; 0 < v--; ) {
	i = dst_vs->id[v];
	fprintf (f, "    draw_dot (%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[i].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[i].y - ymid))));
    }

    fprintf (f, "}\n");
    fclose (f);
    return 1;
}


int
main (int argc, const char* const argv[])
{
    graph_t* g;
    pyr_tree_t* p;
    heap_t* h;
    request_t r1;
    request_t r2;
    vertex_set_t src_vs;
    vertex_set_t dst_vs;
    path_t path;

    if (3 != argc) {
        fprintf (stderr, "syntax: %s <graph file> <requests file>\n", argv[0]);
	return 2;
    }
    if (NULL == (g = read_graph (argv[1])) ||
        !read_requests (argv[2], &r1, &r2)) {
        return 3;
    }
    if (NULL == (h = make_heap_for_graph (g))) {
	free_graph (g);
	return 3;
    }
    if (NULL == (p = make_pyramid_tree (g))) {
        free_graph (g);
	free_heap (h);
    }

    //check_pyramid_tree (p, 0, -2000000000, 2000000000, -2000000000, 2000000000);

    if (!match_requests (g, p, h, &r1, &r2, &src_vs, &dst_vs, &path)) {
        printf ("Requests not matched.\n");
    } else {
	show_find_results (g, &r1, &r2, &src_vs, &dst_vs, &path);
    }

    free_pyramid_tree (p);
    free_heap (h);
    free_graph (g);

    return 0;
}

