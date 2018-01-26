#ifndef __BF_ALGO_H__
#define __BF_ALGO_H__
#include "btl_algo.h"
typedef struct bf_algo_data
{
	uint32_t nAlgoID;
	uint32_t width;
	uint32_t height;
	uint32_t pb_area_max;
	uint32_t pb_area_match_threhold;
	uint32_t pb_area_enroll_threhold;
	uint32_t nXuAreaMatch;
	uint32_t nXuAreaUpdate;
	uint32_t nXuAreaEnroll;
	uint32_t far_match;
	uint32_t far_update;
	uint32_t qscore_match;
	uint32_t qscore_enroll;
	uint32_t dacp_direction;
	uint32_t nVersion;
	uint32_t nRepairFlag;
	uint32_t nFingerDownValue;
	uint32_t nMaxSample;
	uint32_t nBadPixelMaxValue;
	uint32_t nBadPixelsNumThre;
	BL_INIT_PARAM initParam;
}bf_algo_t;

typedef struct bf_navi_point_data
{
	int pointX;
	int pointY;
}bf_point_t;

bf_algo_t *bf_algo_new(int chiptype, void * config);
#endif