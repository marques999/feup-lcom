#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	struct reg86u reg86;
	mmap_t mem;

	if (lm_init() == NULL) {
		return 1;
	}

	lm_alloc(sizeof(vbe_mode_info_t), &mem);
	phys_bytes buf = mem.phys;

	reg86.u.b.ah = VBE_CALL;
	reg86.u.b.al = VBE_GET_MODE_INFO;
	reg86.u.w.es = PB2BASE(buf);
	reg86.u.w.di = PB2OFF(buf);
	reg86.u.w.cx = mode;
	reg86.u.b.intno = VIDEO_INTERRUPT;

	if (sys_int86(&reg86) != OK) {
		return 1;
	}

    *vmi_p = *(vbe_mode_info_t*) mem.virtual;
	lm_free(&mem);

	return 0;
}
