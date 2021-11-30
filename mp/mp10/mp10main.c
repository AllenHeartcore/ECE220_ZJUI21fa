#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp5.h"
#include "mp10.h"


// helper structure for building a pyramid tree
typedef struct pos_t pos_t;
struct pos_t {
    int32_t x;
    int32_t y;
    int32_t id;
};


static request_t*
read_request (FILE* f)
{
    static uint32_t rcount = 0;
    request_t* r;
    int scanres;

    if (NULL == (r = malloc (sizeof (*r)))) {
        return NULL;
    }
    r->uid = rcount++;

    if (6 != (scanres = fscanf (f, "%d%d%d%d%d%d", &r->from.x, &r->from.y,
			        &r->from.range, &r->to.x, &r->to.y, 
				&r->to.range))) {
        if (-1 != scanres) {
	    fprintf (stderr, "Malformed request in file.\n");
	}
	free (r);
	return NULL;
    }
    return r;
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

int32_t
show_find_results (graph_t* g, request_t* r1, request_t* r2)
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
    int32_t show_path;

    if (NULL != r1->path && 2 <= r1->path->n_vertices) {
	dx = (g->vertex[r1->path->id[r1->path->n_vertices - 1]].x -
	      g->vertex[r1->path->id[0]].x);
	dy = (g->vertex[r1->path->id[r1->path->n_vertices - 1]].y -
	      g->vertex[r1->path->id[0]].y);
	xmid = g->vertex[r1->path->id[0]].x + dx / 2;
	ymid = g->vertex[r1->path->id[0]].y + dy / 2;
	show_path = 1;
    } else {
        dx = r1->to.x - r1->from.x;
        dy = r1->to.y - r1->from.y;
	xmid = r1->from.x + dx / 2;
	ymid = r1->from.y + dy / 2;
	show_path = 0;
    }

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
    if (show_path) {
	// no path -> single request, so use different colors for
	// src and dest
	fprintf (f, "    draw_circle (%d,%d,%d,%d);\n", 
		 WIDTH / 2 + ((int32_t)(scale * (r2->to.x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (r2->to.y - ymid))),
		 0, 
		 (int32_t)(scale * r2->to.range));
    }

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

    if (show_path) {
	fprintf (f, "    set_color (0x000000);\n");
	for (i = 0, v = r1->path->id[0]; r1->path->n_vertices - 1 > i; 
	     i++, v = n) {
	    n = r1->path->id[i + 1];
	    fprintf (f, "    draw_line (%d,%d,%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[v].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[v].y - ymid))),
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[n].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[n].y - ymid))));
	}
    }

    fprintf (f, "    set_color (0x0000FF);\n");
    for (v = g->n_vertices; 0 < v--; ) {
	fprintf (f, "    draw_dot (%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[v].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[v].y - ymid))));
    }

    fprintf (f, "    set_color (0x00FF00);\n");
    for (v = r1->src_vs->count; 0 < v--; ) {
	i = r1->src_vs->id[v];
	fprintf (f, "    draw_dot (%d,%d);\n",
		 WIDTH / 2 + ((int32_t)(scale * (g->vertex[i].x - xmid))),
		 HEIGHT / 2 + ((int32_t)(scale * (g->vertex[i].y - ymid))));
    }
    for (v = r1->dst_vs->count; 0 < v--; ) {
	i = r1->dst_vs->id[v];
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
    FILE* rfile;
    request_t* req;
    int32_t which;

    if (3 > argc || 4 < argc) {
        fprintf (stderr, "syntax: %s <graph file> <requests file> [<request id>]\n", argv[0]);
	return 2;
    }
    which = -1;
    if (4 == argc && 1 != sscanf (argv[3], "%d", &which)) {
        fprintf (stderr, "bad request id\n");
	return 2;
    }
    if (NULL == (g = read_graph (argv[1]))) {
        return 3;
    }
    if (NULL == (h = make_heap_for_graph (g))) {
	free_graph (g);
	return 3;
    }
    if (NULL == (p = make_pyramid_tree (g))) {
        free_graph (g);
	free_heap (h);
	return 3;
    }

    //check_pyramid_tree (p, 0, -2000000000, 2000000000, -2000000000, 2000000000);

    if (0 == mark_vertex_minimap (g, p)) {
	free_pyramid_tree (p);
	free_heap (h);
	free_graph (g);
	return 3;
    }

    if (NULL == (rfile = fopen (argv[2], "r"))) {
	perror ("open requests file");
	free_pyramid_tree (p);
	free_heap (h);
	free_graph (g);
	return 3;
    }

    while (NULL != (req = read_request (rfile))) {
	if (0 == handle_request (g, p, h, req)) {
	    free (req);
	    fprintf (stderr, "Request not handled--quitting.\n");
	    break;
	}
    }

    print_results ();

    if (-1 != which && 0 == show_results_for (g, which)) {
        fprintf (stderr, "Failed to illustrate results.\n");
    }

    fclose (rfile);
    free_all_data ();
    free_pyramid_tree (p);
    free_heap (h);
    free_graph (g);
    
    return 0;
}

