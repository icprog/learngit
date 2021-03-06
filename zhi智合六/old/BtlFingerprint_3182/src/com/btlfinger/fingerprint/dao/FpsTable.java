package com.btlfinger.fingerprint.dao;

/**
 * Table of all fingerprint
 *
 * @author blestech
 * @since 2015-11-26
 */
public class FpsTable {
    public static final String TABLE_NAME="fp_data_table";
    public static final String COL_ID="_id";
    public static final String COL_FP_NAME="fp_name";
    public static final String COL_FP_DATA_INDEX="fp_data_index";
    public static final String COL_FP_TEMPLATE_SIZE="fp_template_size";
    public static final String COL_FP_DATA_PATH="fp_data_path";
    public static final String COL_FP_LUNCH_PACKAGE="fp_lunch_package";

    public static final String SQL_CREATE="create table if not exists "+TABLE_NAME
                                          +"("
                                          +COL_ID+" integer primary key autoincrement,"
                                          +COL_FP_NAME+" text,"
                                          +COL_FP_DATA_INDEX+" integer,"
                                          +COL_FP_TEMPLATE_SIZE+" integer,"
                                          +COL_FP_DATA_PATH+" integer,"
                                          +COL_FP_LUNCH_PACKAGE+" text"
                                          +")";
}
