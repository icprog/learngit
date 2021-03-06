#include "bl_fingerprint.h"
#include "bl_chip.h"

/*******************************************************
init params for 2390e
*******************************************************/
static struct bl_reg_value reg_values_2390e[] = {
{0x11,0x0},
{0x12,0x4},
{0xa,0x0},
{0xb,0xa},
{0x2,0x11},
{0x17,0x2c},
{0xd,0x1b},
{0x17,0xac},
{0x3a,0xf},
{0x39,0x75},
{0xe,0x1},
{0x1d,0x0},
{0x1e,0x0},
{0x1f,0x0},
{0x20,0x0},
{0x21,0x0},
{0x22,0x0},
{0x23,0x0},
{0x24,0x0},
{0x25,0x0},
{0x26,0x0},
{0x27,0x0},
{0x28,0x0},
{0x31,0x7f},
{0x32,0x48},
{0x1b,0x3f},
{0xff, 0xff},
};
static struct bl_chip_params chip_params_2390e = {
	.params = reg_values_2390e,
	.hostcmd_reg = {0x13,0x0},
	.fddacp_reg = {0x1b,0x3f},
	.capdacp_reg = {0x1b,0x3f},
	.fdgain_reg = {0x31,0x7f},
	.capgain_reg = {0x31,0x7f},
	.fdwidth = 32,
	.fdheight = 32,
	.fdframe_num = 0,
	.width = 112,
	.height = 96,
	.capframe_num = 3,
};

/*******************************************************
init params for 3290
*******************************************************/
static struct bl_reg_value reg_values_3290[] = {
{0x12, 0x6},
{0x11, 0x0},
{0xa, 0x0},
{0xb, 0xa},
{0x2, 0x11},
{0x17, 0x2c},
{0xd, 0x1b},
{0x17, 0xac},
{0x2d, 0xf0},
{0x3d, 0x75},
{0x33, 0x92},
{0x35, 0x52},
{0x1d, 0xff},
{0x1e, 0xff},
{0x1f, 0xff},
{0x20, 0xff},
{0x21, 0xff},
{0x22, 0xff},
{0x23, 0xff},
{0x24, 0xff},
{0x25, 0xff},
{0x26, 0xff},
{0x27, 0xff},
{0x28, 0xff},
{0x31, 0x2e},
{0x32, 0x48},
{0x1b, 0x20},
{0xff, 0xff},
};
static struct bl_chip_params chip_params_3290 = {
	.params = reg_values_3290,
	.hostcmd_reg = {0x13,0x0},
	.fddacp_reg = {0x1b,0x20},
	.capdacp_reg = {0x1b,0x20},
	.fdgain_reg = {0x31,0x56},
	.capgain_reg = {0x31,0x56},
	.fdwidth = 32,
	.fdheight = 32,
	.fdframe_num = 0,
	.width = 112,
	.height = 96,
	.capframe_num = 3,
};

/*******************************************************
init params for 3182
*******************************************************/
static struct bl_reg_value reg_values_3182[] = {
{0x2d,0x60},
{0x2b,0x3b},
{0x10,0x8},
{0x11,0x0},
{0x12,0x4},
{0x25,0x0},
{0x26,0xa},
{0x1b,0x40},
{0x1c,0x40},
{0x1d,0xff},
{0x1e,0xff},
{0x31,0x3f},
{0x32,0x48},
{0xff, 0xff},
};
static struct bl_chip_params chip_params_3182 = {
	.params = reg_values_3182,
	.hostcmd_reg = {0x34,0x0},
	.fddacp_reg = {0x1b,0x40},
	.capdacp_reg = {0x1c,0x40},
	.fdgain_reg = {0x31,0x67},
	.capgain_reg = {0x31,0x67},
	.fdwidth = 32,
	.fdheight = 32,
	.fdframe_num = 0,
	.width = 72,
	.height = 128,
	.capframe_num = 7,
};

/*******************************************************
init params for 3290
*******************************************************/
static struct bl_reg_value reg_values_3390[] = {
{0x2b, 0x7b},
{0x10, 0x48},
{0x11, 0x0},
{0x12, 0x6},
{0x25, 0x0},
{0x26, 0xa},
{0x1b, 0xff},
{0x1c, 0xff},
{0x20, 0xff},
{0x9, 0xff},
{0xa, 0xff},
{0xb, 0xff},
{0xc, 0xff},
{0xd, 0xff},
{0xe, 0xff},
{0xf, 0xff},
{0x1d, 0x9e},
{0x1e, 0x9e},
{0x31, 0x38},
{0x32, 0x48},
{0xff, 0xff},
};
static struct bl_chip_params chip_params_3390 = {
	.params = reg_values_3390,
	.hostcmd_reg = {0x34,0x0},
	.fddacp_reg = {0x1d,0xaa},
	.capdacp_reg = {0x1e,0xaa},
	.fdgain_reg = {0x31,0x30},
	.capgain_reg = {0x31,0x30},
	.fdwidth = 80,
	.fdheight = 32,
	.fdframe_num = 0,
	.width = 80,
	.height = 80,
	.capframe_num = 7,
};

/*******************************************************
all chip init params 
*******************************************************/
static struct bl_chip_params *all_chip_params[BL_FP_CHIP_MAX] = {
	&chip_params_2390e,
	&chip_params_3290,
	&chip_params_3182,
	&chip_params_3390,
};

int bl_add_chip_params(struct bl_fingerprint_data *bl_data,struct bl_chip_params *params)
{
	int i = 0;
	for (i = 0; i < BL_FP_CHIP_MAX; i++) {
		/* add tpd driver into list */
		if (all_chip_params[i] == NULL) {
			all_chip_params[i] = params;
			break;
		}
	}
}

/*******************************************************
select chip init params 
*******************************************************/

int bl_init_params(struct bl_fingerprint_data *bl_data)
{
	BTL_DEBUG(" bl_data->chiptype=%d ++\n",bl_data->chiptype);
	bl_data->chip_params = all_chip_params[bl_data->chiptype];
	return 0;
}
