#ifndef _BF_LIB_H_
#define _BF_LIB_H_
#include "bf_custom.h"
typedef enum {
    BF_LIB_OK,                            // 0
    BF_LIB_WAIT_EVENT_FINGER_PRESENT,     // 1
    BF_LIB_CAPTURE_DONE,                  // 2
    BF_LIB_ENABLE_EVENT_FINGER_PRESENT,   // 3
    BF_LIB_WAIT_TIME,                     // 4
    BF_LIB_FINGER_PRESENT,                // 5
    BF_LIB_FINGER_LOST,                   // 6
    BF_LIB_ERROR_TOO_FAST,                // 7
    BF_LIB_ERROR_TOO_SLOW,                // 8
    BF_LIB_ERROR_GENERAL,                 // 9
    BF_LIB_ERROR_SENSOR,                  // 10
    BF_LIB_ERROR_MEMORY,                  // 11
    BF_LIB_ERROR_PARAMETER,               // 12
    BF_LIB_FAIL_LOW_QUALITY,
    BF_LIB_FAIL_INVAILD_TOUCH,

    BF_TA_ERROR_CODE_BASE = 0xFFFF6000,
    BF_TA_ERROR_TA_NOT_INIT,
    BF_TA_ERROR_STATE,
    BF_TA_ERROR_LIB_INIT_FAIL,
    BF_TA_ERROR_INIT_ALG_PPLIB_FAIL,
    BF_TA_ERROR_EROLL_EXCEED_MAX_FINGERPIRNTS,
    BF_TA_ERROR_EROLL_NOT_COMPLETED,
    BF_TA_ERROR_EROLL_GET_TEMPLATE_FAIL,
    BF_TA_ERROR_EROLL_PACK_TEMPLATE_FAIL,

    BF_TA_DB_CODE_BASE = BF_TA_ERROR_CODE_BASE | 0x1000,
    BF_TA_ERROR_DB_FS_INIT_FAIL ,
    BF_TA_ERROR_CREAT_GLOBAL_DB_FAIL,
    BF_TA_ERROR_CREAT_USER_DB_FAIL,
    BF_TA_ERROR_DB_SIZE_OVERFLOW,
    BF_TA_ERROR_DB_GET_FP_ID_ERROR,
    BF_TA_ERROR_DB_GET_FPSET_ID_ERROR,
    BF_TA_ERROR_DB_GET_SECURE_USER_ID_ERROR,
    BF_TA_ERROR_DB_GLOBAL_DB_FILE_LOST,
    BF_TA_ERROR_DB_USER_DB_FILE_LOST,
    BF_TA_ERROR_DB_SINGLE_TEMPLATE_FILE_LOST,
} bf_lib_return_t;

typedef enum {
    BF_LIB_IDENTIFY_NO_MATCH,
    BF_LIB_IDENTIFY_MATCH,
    BF_LIB_IDENTIFY_MATCH_UPDATED_TEMPLATE,
} bf_lib_identify_result_t;

typedef enum {
    BF_LIB_ENROLL_SUCCESS,
/////huawei define start///
    BF_LIB_ENROLL_HELP_SAME_AREA ,
    BF_LIB_ENROLL_HELP_TOO_WET,
    BF_LIB_ENROLL_HELP_ALREADY_EXIST,
/////huawei define end///
    BF_LIB_ENROLL_TOO_MANY_ATTEMPTS,
    BF_LIB_ENROLL_TOO_MANY_FAILED_ATTEMPTS,
    BF_LIB_ENROLL_FAIL_NONE,
    BF_LIB_ENROLL_FAIL_LOW_QUALITY,
    BF_LIB_ENROLL_FAIL_LOW_COVERAGE,
    BF_LIB_ENROLL_FAIL_LOW_QUALITY_AND_LOW_COVERAGE,
} bf_lib_enroll_result_t;

typedef enum {
    BF_LIB_SENSOR_OK,
    BF_LIB_SENSOR_WORKING,
    BF_LIB_SENSOR_INITIALISING,
    BF_LIB_SENSOR_OUT_OF_ORDER,
    BF_LIB_SENSOR_MALFUNCTIONED,
    BF_LIB_SENSOR_FAILURE,
} bf_lib_sensor_status_t;

/* Security levels for the matching function */
typedef enum {
    /* FAR 1/1000 */
    BF_LIB_SECURITY_LOW,
    /* FAR 1/10,000 */
    BF_LIB_SECURITY_REGULAR,
    /* FAR 1/50,000 */
    BF_LIB_SECURITY_HIGH,
    /* FAR 1/100,000 */
    BF_LIB_SECURITY_VERY_HIGH,
} bf_lib_security_t;

#endif
