#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "lab11.h"


#define PRINT_CODE 0


/*
 * Dynamically create a code_stat_t to hold data from an LC-3 binary.
 * Returns new code_stat_t on success, or NULL on failure.
 */

static code_stat_t*
load_file (const char* fname)
{
    struct stat  sb;
    int32_t      len;
    code_stat_t* cs;
    FILE*        f;
    int32_t      i;
    uint8_t      buf[2];

    if (0 != stat (fname, &sb) || 0x10002 < sb.st_size || 
    	0 != (sb.st_size & 1)) {
        return NULL;
    }
    if (0 == (len = (sb.st_size / 2) - 1)) {
	// empty but valid file
	return 0;
    }
    if (NULL == (cs = (code_stat_t*)malloc (sizeof (*cs))) ||
	NULL == (cs->bits = (uint16_t*)malloc (len * sizeof (cs->bits[0])))) {
	if (NULL != cs) {
	    free (cs);
	}
        return NULL;
    }
    cs->name = fname;
    cs->len  = len;
    if (NULL == (f = fopen (fname, "rb"))) {
        free (cs->bits);
        free (cs);
	return NULL;
    }
    if (1 != fread (buf, 2, 1, f)) {
	(void)fclose (f);
        free (cs->bits);
        free (cs);
	return NULL;
    }
    cs->start = ((buf[0] << 8) | buf[1]);
    for (i = 0; cs->len > i; i++) {
        if (1 != fread (buf, 2, 1, f)) {
	    (void)fclose (f);
	    free (cs->bits);
	    free (cs);
	    return NULL;
	}
	cs->bits[i] = ((buf[0] << 8) | buf[1]);
    }
    (void)fclose (f);

    return cs;
}


static void
free_code_stat (code_stat_t* cs)
{
    free (cs->bits);
    free (cs->inst);
    free (cs);
}


static void
report_search_result (code_stat_t* cs, int32_t first, inst_flag_t res)
{
    int32_t warning = 0;

    if (0 == first) {
	printf ("analyzed code starting at %04X:\n", cs->start);
	if (!(res & INST_FLAG_HALT)) {
	    printf ("   does not reach HALT instruction\n");
	    warning = 1;
	}
	if ((res & INST_FLAG_START_SUBROUTINE)) {
	    printf ("   may include code called as a subroutine\n");
	    warning = 1;
	}
	if ((res & INST_FLAG_RET)) {
	    printf ("   may reach RET instruction\n");
	    warning = 1;
	}
    } else {
        printf ("analyzed subroutine starting at %04X:\n", 
		cs->inst[first].address);
	if ((res & INST_FLAG_HALT)) {
	    printf ("   may reach HALT instruction\n");
	    warning = 1;
	}
	if (!(res & INST_FLAG_START_SUBROUTINE)) {
	    printf ("   not marked as a subroutine\n");
	    warning = 1;
	}
	if (!(res & INST_FLAG_RET)) {
	    printf ("   does not reach RET instruction\n");
	    warning = 1;
	}
    }
    if ((res & INST_FLAG_BAD_BR_TGT)) {
	printf ("   may reach BR target outside of code\n");
	warning = 1;
    }
    if ((res & INST_FLAG_BAD_JSR_TGT)) {
	printf ("   may reach JSR target outside of code\n");
	warning = 1;
    }
    if ((res & INST_FLAG_UNKNOWN_JSR_TGT)) {
	printf ("   may reach unknown JSRR target\n");
	warning = 1;
    }
    if ((res & INST_FLAG_LEAVES_CODE)) {
	printf ("   may pass end of defined code\n");
	warning = 1;
    }
    if (warning) {
	puts ("");
    } else {
        printf ("   no warnings to report\n\n");
    }
}


static instruction_t* 
get_jsr_target (code_stat_t* cs, int32_t inst_num)
{
    int32_t  offset;
    uint16_t target_addr;
    uint16_t target_idx;
    uint16_t baseR;

    if ((cs->bits[inst_num] & 0x800)) {
	// JSR -- find PC-relative target
	offset = (cs->bits[inst_num] & 0x7FF);
	if (0 != (0x400 & offset)) {
	    offset |= ~0x3FF;
	}
	target_addr = cs->inst[inst_num].address + offset + 1;
	target_idx = (target_addr - cs->start);
	if (cs->len <= target_idx) {
	    cs->inst[inst_num].flags |= INST_FLAG_BAD_JSR_TGT;
	    return NULL;
	}
	return &cs->inst[target_idx];
    }

    // JSRR: our check is not sophisticated: we simply look for an LD
    // immediately preceding the JSRR.  Note that failures are marked
    // as 'unknown' rather than bad for JSRR.
    baseR = ((cs->bits[inst_num] & 0x1C0) >> 6);
    if (0 == inst_num ||
        0x2000 != (cs->bits[inst_num - 1] & 0xF000) ||
	baseR != ((cs->bits[inst_num - 1] & 0xE00) >> 9)) {
	cs->inst[inst_num].flags |= INST_FLAG_UNKNOWN_JSR_TGT;
	return NULL;
    }

    // find the 'instruction' targeted by the LD
    offset = (cs->bits[inst_num - 1] & 0x1FF);
    if (0 != (0x100 & offset)) {
	offset |= ~0xFF;
    }
    target_addr = cs->inst[inst_num].address + offset;
    target_idx = (target_addr - cs->start);
    if (cs->len <= target_idx) {
	cs->inst[inst_num].flags |= INST_FLAG_UNKNOWN_JSR_TGT;
	return NULL;
    }

    // the bits at the target are the target address of the JSRR
    target_addr = cs->bits[target_idx];
    target_idx = (target_addr - cs->start);
    if (cs->len <= target_idx) {
	cs->inst[inst_num].flags |= INST_FLAG_UNKNOWN_JSR_TGT;
	return NULL;
    }
    return &cs->inst[target_idx];
}


static instruction_t* 
get_branch_target (code_stat_t* cs, int32_t inst_num)
{
    int32_t  offset;
    uint16_t target_addr;
    uint16_t target_idx;

    offset = (cs->bits[inst_num] & 0x1FF);
    if (0 != (0x100 & offset)) {
	offset |= ~0xFF;
    }
    target_addr = cs->inst[inst_num].address + offset + 1;
    target_idx = (target_addr - cs->start);
    if (cs->len <= target_idx) {
	cs->inst[inst_num].flags |= INST_FLAG_BAD_BR_TGT;
        return NULL;
    }
    return &cs->inst[target_idx];
}


#if PRINT_CODE // change to 1 at top of file

static char
incoming_mark (int32_t n)
{
    if (4 > n) {
        return "X 23"[n];
    }
    return '*';
}


static void
print_code (code_stat_t* cs)
{
    int32_t i;

    puts ("----- START OF CODE -----");
    for (i = 0; cs->len > i; i++) {
        printf ("%c %04X %04X -> ", 
		((cs->inst[i].flags & INST_FLAG_START_SUBROUTINE) ? 
		 'S' : incoming_mark (cs->inst[i].n_incoming_arcs)),
		cs->inst[i].address, cs->bits[i]);
	if ((cs->inst[i].flags & INST_FLAG_LEAVES_CODE)) {
	    printf ("EXIT ");
        } else if (NULL != cs->inst[i].next_inst) {
	    printf ("%04X ", cs->inst[i].next_inst->address);
	} else {
	    printf ("     ");
	}
	if ((cs->inst[i].flags & INST_FLAG_BAD_BR_TGT)) {
	    printf (" BR BAD  ");
	} else if ((cs->inst[i].flags & INST_FLAG_BAD_JSR_TGT)) {
	    printf ("JSR BAD  ");
	} else if ((cs->inst[i].flags & INST_FLAG_UNKNOWN_JSR_TGT)) {
	    printf ("JSR UNK  ");
	} else if (NULL != cs->inst[i].branch_tgt) {
	    printf (" BR %04X ", cs->inst[i].branch_tgt->address);
	} else if (NULL != cs->inst[i].jsr_tgt) {
	    printf ("JSR %04X ", cs->inst[i].jsr_tgt->address);
	} else {
	    printf ("         ");
	}
	if ((cs->inst[i].flags & INST_FLAG_HALT)) {
	    printf ("HALT ");
	} else {
	    printf ("     ");
	}
	if ((cs->inst[i].flags & INST_FLAG_RET)) {
	    printf ("RET ");
	} else {
	    printf ("    ");
	}
	puts ("");
    }
    puts ("----- END OF CODE -----\n");
}

#endif // PRINT_CODE // change to 1 at top of file

// returns 1 on success, 0 on failure

static int32_t
analyze_code (code_stat_t* cs)
{
    int32_t i;
    uint16_t nzp_bits;

    if (NULL == (cs->inst = 
    		 (instruction_t*)malloc (sizeof (cs->inst[0]) * cs->len))) {
	return 0;
    }
    		
    for (i = 0; cs->len > i; i++) {
	cs->inst[i].address         = cs->start + i; // may wrap
	cs->inst[i].n_incoming_arcs = (0 == i);
	cs->inst[i].next_inst       = &cs->inst[i + 1];
	cs->inst[i].flags           = 0;

	if (0x4000 == (cs->bits[i] & 0xF000)) {
	    // JSR(R)
	    cs->inst[i].branch_tgt = NULL;
	    cs->inst[i].jsr_tgt = get_jsr_target (cs, i);
	} else {
	    cs->inst[i].jsr_tgt = NULL;
	    if (0x0000 == (cs->bits[i] & 0xF000)) {
		// a branch
		nzp_bits = (cs->bits[i] & 0xE00);
		if (0x000 == nzp_bits) {
		    cs->inst[i].branch_tgt = NULL;
		} else {
		    cs->inst[i].branch_tgt = get_branch_target (cs, i);
		    if (0xE00 == nzp_bits) {
			cs->inst[i].next_inst = NULL;
		    }
		}
	    } else {
		// not a branch
		cs->inst[i].branch_tgt = NULL;
		if (0xF025 == cs->bits[i]) {
		    // HALT -> no next instructions
		    cs->inst[i].flags |= INST_FLAG_HALT;
		    cs->inst[i].next_inst = NULL;
		} else if (0xC1C0 == cs->bits[i]) {
		    // RET -> no next instructions
		    cs->inst[i].flags |= INST_FLAG_RET;
		    cs->inst[i].next_inst = NULL;
		} 
	    }
	}
    }

    // last instruction should be HALT, RET, or BRnzp, otherwise it 
    // can leave the defined code region
    if (NULL != cs->inst[cs->len - 1].next_inst) {
	cs->inst[cs->len - 1].next_inst = NULL;
	cs->inst[cs->len - 1].flags |= INST_FLAG_LEAVES_CODE;
    }

    for (i = 0; cs->len > i; i++) {
	if (NULL != cs->inst[i].next_inst) {
	    cs->inst[i].next_inst->n_incoming_arcs++;
	}
	if (NULL != cs->inst[i].branch_tgt) {
	    cs->inst[i].branch_tgt->n_incoming_arcs++;
	}
	if (NULL != cs->inst[i].jsr_tgt) {
	    cs->inst[i].jsr_tgt->flags |= INST_FLAG_START_SUBROUTINE;
	}
    }

    return 1;
}



int
main (int argc, const char* const argv[])
{
    code_stat_t* cs;
    inst_flag_t  result;
    int32_t      i;
    int32_t      new_subs_found;

    if (2 != argc) {
        fprintf (stderr, "syntax: %s <.obj file>\n", argv[0]);
	return 2;
    }
    if (NULL == (cs = load_file (argv[1]))) {
        fprintf (stderr, "could not read %s\n", argv[1]);
	return 2;
    }

    if (1 != analyze_code (cs)) {
        fprintf (stderr, "analysis failed\n");
	return 3;
    }

#if PRINT_CODE // change to 1 at top of file
     print_code (cs);
#endif // PRINT_CODE // change to 1 at top of file

    result = do_search (cs, 0);
    report_search_result (cs, 0, result);

    do {
        new_subs_found = 0;
	for (i = 0; cs->len > i; i++) {
	    if ((cs->inst[i].flags & INST_FLAG_START_SUBROUTINE) &&
		(cs->inst[i].flags & INST_FLAG_SUB_CALLED) &&
		!(cs->inst[i].flags & INST_FLAG_SUB_EXPLORED)) {
		cs->inst[i].flags |= INST_FLAG_SUB_EXPLORED;
		new_subs_found = 1;
		result = do_search (cs, i);
		report_search_result (cs, i, result);
	    }
	}
    } while (new_subs_found);

    free_code_stat (cs);

    return 0;
}

