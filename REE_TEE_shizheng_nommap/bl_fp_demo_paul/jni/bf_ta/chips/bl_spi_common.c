#include "bf_custom.h"
#include "bl_fingerprint.h"
#include "bl_spi_common.h"
#include "bf_config.h"
static struct bl_fingerprint_data *g_bl_data = NULL;
#define READ_CMD_SIZE	(10)
/* -------------------------------------------------------------------- */
void bl_hexdump(const unsigned char *buf, const int num)

{
	int i;
	for(i = 0; i < num; i++) {
		BTL_DEBUG("%02X ", buf[i]);
		if ((i+1)%8 == 0)
			BTL_DEBUG("\n");
	}
	BTL_DEBUG("\n");
	return;
}

/* -------------------------------------------------------------------- */
u8 bl_spi_read_reg(u8 nRegID)
{
    u8 nAddr;
    u8 data_tx[REG_CMD_SIZE];

    nAddr = nRegID << 1;
    nAddr &= 0x7F;

    data_tx[0] = nAddr;
    data_tx[1] = 0xff;
	
	bf_tee_spi_read_write(data_tx, REG_CMD_SIZE, data_tx, REG_CMD_SIZE);

	BTL_DEBUG(" nRegID=%x value=%x ++\n",nRegID,data_tx[1]);
    return data_tx[1];
}

/* -------------------------------------------------------------------- */
u8 bl_spi_read_frame(struct bl_fingerprint_data *bl_data,u32 len)
{
	int ret;
    u8 nAddr;
    u8 *data_tx = bl_data->tx_buf;
	BTL_DEBUG(" len=%d ++\n",len);
	bl_spi_write_reg(bl_data->chip_params->hostcmd_reg.addr, 7);
	bl_spi_write_reg(bl_data->chip_params->hostcmd_reg.addr, MODE_FG_PRINT);

    nAddr = REGA_FINGER_CAP << 1;
    nAddr &= 0x7F;

    data_tx[0] = nAddr;
    data_tx[1] = 0xff;
    
	ret = bf_tee_spi_read_write(data_tx, REG_CMD_SIZE, data_tx, len);

    return ret;
}

/* -------------------------------------------------------------------- */
u8 bl_spi_write_reg(u8 nRegID, u8 value)
{
	int ret;
    u8 nAddr;
    u8 data_tx[REG_CMD_SIZE];
	BTL_DEBUG(" nRegID=%x,value=%x ++\n",nRegID,value);
    nAddr = nRegID << 1;
    nAddr |= 0x80;

    data_tx[0] = nAddr;
    data_tx[1] = value;

	ret = bf_tee_spi_read_write(data_tx, REG_CMD_SIZE, data_tx, REG_CMD_SIZE);

    return ret;
}

/* -------------------------------------------------------------------- */
u8 bl_spi_write_reg_bit(u8 nRegID, u8 bit, u8 value)
{
    u8 tempvalue = 0;

	tempvalue = bl_spi_read_reg(nRegID);
	tempvalue &= ~(1 << bit);
	tempvalue |= (value << bit);
	bl_spi_write_reg(nRegID,tempvalue);
	
    return 0;
}

/*----------------------------------------------------------------------------*/

int bl_read_chipid_typeA(struct bl_fingerprint_data *bl_data)
{
	u32 chip_id;
	u8 regValue;
	u8 version;
	
	BTL_DEBUG("  ++\n");
	bl_spi_write_reg_bit(0x10, 5, 0); //enable

	regValue = bl_spi_read_reg(0x37);
	chip_id = regValue << 8;
	regValue = bl_spi_read_reg(0x36);
	chip_id += regValue;
	version = bl_spi_read_reg(0x38);
	bl_spi_write_reg_bit(0x10, 5, 1); //disable
	BTL_DEBUG(" chip_id=0x%x,version=0x%x\n",chip_id,version);
	if(chip_id == BL_CHIPID_3182)
		bl_data->chiptype = BL_FP_CHIP_3182;
	else if(chip_id == BL_CHIPID_3390)
		bl_data->chiptype = BL_FP_CHIP_3390;
	else if(chip_id == BL_CHIPID_3590)
		bl_data->chiptype = BL_FP_CHIP_3590;
	else if(chip_id == BL_CHIPID_81192)
		bl_data->chiptype = BL_FP_CHIP_81192;
	bl_data->chipid = chip_id;
	return chip_id;
}

int bl_read_chipid_3290(struct bl_fingerprint_data *bl_data)
{
    u8 val_low = 0,val_high = 0,chip_type=0xFF,driver_type=0x5A;
    int chip_id = 0;

    u8  old_value = 0;

     BTL_DEBUG("  ++\n");

	bl_spi_write_reg(0x13, MODE_IDLE);

     old_value = bl_spi_read_reg(0x3A);

     bl_spi_write_reg(0x3A, old_value|0x80);
        bf_tee_msleep(1000);
    val_low = bl_spi_read_reg(0x10);//id reg low
    BTL_DEBUG("val_low=0x%x",val_low);
    if(CHIP_ID_LOW != val_low)
        return -SPI_PIN_FAILED;

    val_high = bl_spi_read_reg(0x11);//id reg high
    BTL_DEBUG("val_high=0x%x",val_high);
    if(CHIP_ID_HIGH != val_high)
        return -SPI_PIN_FAILED;

    chip_type = bl_spi_read_reg(0x12);//ic type
    BTL_DEBUG("chip_type=0x%x",chip_type);

    chip_id =(val_high << 8) | (val_low & 0xff);
    BTL_DEBUG("chip_id=%x",chip_id);

     bl_spi_write_reg(0x3A, old_value);
     driver_type = bl_spi_read_reg(0x3e);
	if(chip_id == BL_CHIPID_3290)
	{
		if(driver_type == 0)
			bl_data->chiptype = BL_FP_CHIP_2390E;
		else
			bl_data->chiptype = BL_FP_CHIP_3290;
	}
	bl_data->chipid = chip_id;
     return chip_id;
}

int bl_read_chipid(struct bl_fingerprint_data *bl_data)
{
	int ret = -1;
	load_config_t* config = NULL;
	g_bl_data = bl_data;
	ret = bl_read_chipid_typeA(bl_data);
	if((ret == BL_CHIPID_3182) ||(ret == BL_CHIPID_3390) || (ret == BL_CHIPID_3590)|| (ret == BL_CHIPID_81192))
	{
		goto INIT_CHIP_PARAMS;
	}
	else {
		ret = bl_read_chipid_3290(bl_data);
		if(ret == BL_CHIPID_3290)
			goto INIT_CHIP_PARAMS;
	}
	if(NULL != bl_data->bl_private)
	{
		config = (load_config_t*)bl_data->bl_private;
		if(config->force_loading == 1)
		{
			ret = config->chipid;
			bl_data->chipid = config->chipid;
			bl_data->chiptype = config->chiptype;

			BTL_DEBUG("read chipid fail,but force loading \n\
			chipid:0x%x, chiptype:%d\n", bl_data->chipid, bl_data->chiptype);
			
			goto INIT_CHIP_PARAMS;
		}
	}
	
	BTL_DEBUG("read chipid fail exit");
	return -1;
	
	
INIT_CHIP_PARAMS:
	bl_init_params(bl_data);
	return ret;
}

static int bl_clear_reset_status(struct bl_fingerprint_data *bl_data)
{
	switch(bl_data->chipid){
		case BL_CHIPID_3290:
			break;
		case BL_CHIPID_3182:
		case BL_CHIPID_3390:
		case BL_CHIPID_3590:
		case BL_CHIPID_81192:
			bl_spi_write_reg(0x13, 0x40);
			bl_spi_write_reg(0x13, 0x0);
			break;
	}
	return 0;
}

int bl_dev_init(struct bl_fingerprint_data *bl_data)
{
	int i = 0;
	struct bl_chip_params *chip_params = bl_data->chip_params;
	struct bl_reg_value *params = chip_params->params;
	
	BTL_DEBUG("  ++");
	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_IDLE);
	bl_clear_reset_status(bl_data);
	while(params[i].addr != 0xff)
	{
		bl_spi_write_reg(params[i].addr, params[i].value);
		i++;
	}
	return 0;
}

int bl_set_frame_num(struct bl_fingerprint_data *bl_data,u32 frame_num)
{
	u8 value = 0;
	u8 bit = 3;
	if((bl_data->frame_num != frame_num)||(bl_data->is_force_set == 1))
	{
	    	BTL_DEBUG("bl_data->chipid=%x frame_num=%x  ++\n",bl_data->chipid,frame_num);
		switch(bl_data->chipid){
			case BL_CHIPID_3290:
				if(frame_num != 0)
					frame_num = 3;
				bl_spi_write_reg(0x17, 0x2c);
				value = bl_spi_read_reg(0x0d);
				value &= ~(0x3 << bit);
				value |= ((frame_num & 0x7) << bit);
				bl_spi_write_reg(0x0d, value);
				bl_spi_write_reg(0x17, 0xac);
				break;

			case BL_CHIPID_3390:
			case BL_CHIPID_3590:
				if(frame_num != 0)
					frame_num = 7;
			case BL_CHIPID_81192:
			case BL_CHIPID_3182:   //����7+1֡��ͼ
				value = bl_spi_read_reg(0x2b);
				value &= ~(0x7 << bit);
				BTL_DEBUG("value=%x",value);
				value |= ((frame_num & 0x7) << bit);
				bl_spi_write_reg(0x2b, value);
				break;
		}
		bl_data->frame_num = frame_num;
    	BTL_DEBUG("bl_data->chipid=%x value=%x  ++\n",bl_data->chipid,value);
	}

	return 0;
}

int bl_set_frame_size(struct bl_fingerprint_data *bl_data,u32 height,u32 width)
{
	chip_params_t *chip_params = bl_data->chip_params;
	static u32 tempwidth = 0;
	static u32 tempheight = 0;
	u32 row_start = 0;
	u32 row_len = 0;
	u32 col_start = 0;
	u32 col_len = 0;
	u32 is_dirty = 0;
	u8 reg_col_value = 0;
	
	if((tempheight != height)||(bl_data->is_force_set == 1))
	{
		if(height <= chip_params->height)
			tempheight = height;
		else
		{
		    BTL_DEBUG("height=%d is large than chipmax height=%d ++\n",height, chip_params->height);
		    tempheight = chip_params->height;
		}
		is_dirty = 1;
		bl_data->frameheight = tempheight;
		row_start = 1 + (chip_params->height - tempheight) / 2;
		row_len = tempheight;
	}

	if((tempwidth != width)||(bl_data->is_force_set == 1))
	{
		if(width <= chip_params->width)
			tempwidth = width;
		else
		{
		    BTL_DEBUG("width=%d is large than chipmax width=%d ++\n",width, chip_params->width);
			tempwidth = chip_params->width;
		}
		is_dirty = 1;	
		col_start = 1 + (chip_params->width - tempwidth) / 2 / 8;
		col_len = tempwidth / 8;
		bl_data->framewidth = col_len * 8;
		reg_col_value = (col_start & 0xf)|((col_len & 0xf) << 4);
	}
	if(is_dirty)
	{
		BTL_DEBUG("row_start=%d, row_len=%d, col_start=%d, col_len=%d reg_col_value=%x",row_start, row_len, col_start, col_len,reg_col_value);
		switch(bl_data->chipid){
			case BL_CHIPID_3290:
				break;
			case BL_CHIPID_3182:
				break;
			case BL_CHIPID_3590:
				bl_spi_write_reg(RRG_FRAME_COL_START_LEN, reg_col_value);
				break;
			case BL_CHIPID_3390:
				bl_spi_write_reg(RRG_FRAME_ROW_START, row_start & 0xff);
				bl_spi_write_reg(RRG_FRAME_ROW_LEN, row_len & 0xff);
				break;
			case BL_CHIPID_81192:
				break;//do nothing
		}
	}
	return 0;
}

int bl_set_gain_dacp(struct bl_fingerprint_data *bl_data,u8 mode,u8 gain,u8 dacp)
{
	static u8 mgain,mdacp,mmode;
	struct bl_chip_params *chip_params = bl_data->chip_params;
	BTL_DEBUG("mode=%x,gain=%x,dacp=%x,mgain=%x,mdacp=%x,mmode=%x,bl_data->is_force_set=%d",mode,gain,dacp,mgain,mdacp,mmode,bl_data->is_force_set);
		
	if((bl_data->is_force_set == 1)&&(mode == MODE_FG_DT))
	{
		bl_spi_write_reg(chip_params->fdgain_reg.addr, gain);
		bl_spi_write_reg(chip_params->fddacp_reg.addr, dacp);
	}else if(mode == MODE_FG_CAP)
	{
		if((mmode == mode)&&(mgain == gain)&&(mdacp == dacp))
		{
			BTL_DEBUG("");
			return 0;
		}
		bl_spi_write_reg(chip_params->capgain_reg.addr, gain);
		bl_spi_write_reg(chip_params->capdacp_reg.addr, dacp);
		bl_data->gain = gain;
		bl_data->dacp = dacp;
		mgain = gain;
		mdacp = dacp;
	}
	bl_data->mode = mode;
	mmode = mode;
	return 0;
}

int bl_interrupt_init(struct bl_fingerprint_data *bl_data)
{
     BTL_DEBUG("  ++\n");
	struct bl_chip_params *chip_params = bl_data->chip_params;
     BTL_DEBUG("  ++\n");

	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_IDLE);
	bl_clear_reset_status(bl_data);
	bl_set_frame_num(bl_data,0);
	bl_set_frame_size(bl_data, bl_data->frameheight, bl_data->framewidth);
	bl_set_gain_dacp(bl_data, MODE_FG_CAP,chip_params->capgain_reg.value,chip_params->capdacp_reg.value);
	bl_set_gain_dacp(bl_data, MODE_FG_DT,chip_params->fdgain_reg.value,chip_params->fddacp_reg.value);

	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_FG_DT);
	
	return 0;
}
int bl_capture_init(struct bl_fingerprint_data *bl_data)
{
	bl_capture_init_framenum(bl_data, 7);
	return 0;
}

int bl_capture_init_framenum(struct bl_fingerprint_data *bl_data,int framenum)
{
	struct bl_chip_params *chip_params = bl_data->chip_params;
	     BTL_DEBUG("  ++\n");

	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_IDLE);
	bl_clear_reset_status(bl_data);
	bl_set_frame_num(bl_data,framenum);
	bl_set_frame_size(bl_data, chip_params->height, chip_params->width);
	bl_set_gain_dacp(bl_data, MODE_FG_CAP,bl_data->gain,bl_data->dacp);
	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_FG_CAP);
	return 0;
}

int bl_set_chip_idlemode(struct bl_fingerprint_data *bl_data)
{
	struct bl_chip_params *chip_params = bl_data->chip_params;
	bl_spi_write_reg(chip_params->hostcmd_reg.addr, MODE_IDLE);
}
/*
u8 bl_getIntStatus(struct bl_fingerprint_data *bl_data)
{
    u8 unStatus = 0;
    unStatus = bl_spi_read_reg(REGA_INTR_STATUS);
    bl_data->nStatus = unStatus;
    BTL_DEBUG("nStatus=%2x",unStatus);
    return unStatus;
}

int bl_power_onoff(struct bl_fingerprint_data *bl_data,u32 enable)
{
	BTL_DEBUG("  enable=%d++\n",enable);
	return ioctl(bl_data->devfd,BL_POWER_ONOFF,enable);
}

int bl_enable_irq(struct bl_fingerprint_data *bl_data,u32 enable)
{
	BTL_DEBUG("  enable=%d++\n",enable);
	return ioctl(bl_data->devfd,BL_IRQ_ENABLE,enable);
}

static void btl_api_responseFunc(int signum)
{
    BTL_DEBUG("%s+++", __func__);
    pthread_mutex_lock(&g_bl_data->int_mutex);
	pthread_cond_signal(&g_bl_data->int_wait_cond);
	pthread_mutex_unlock(&g_bl_data->int_mutex);
	BTL_DEBUG("%s---", __func__);
}


static int setup_signal_for_int(struct bl_fingerprint_data *bl_data)
{
	int Oflags;
	BTL_DEBUG("+++");
    signal(SIGIO, btl_api_responseFunc);
    fcntl(bl_data->devfd, F_SETOWN, getpid());
    Oflags = fcntl(bl_data->devfd, F_GETFL);
    fcntl(bl_data->devfd, F_SETFL, Oflags | FASYNC);
	BTL_DEBUG("---");
    return 0;
}

int bl_waitSignal_int(struct bl_fingerprint_data *bl_data,uint32_t fmode)
{
	BTL_DEBUG("mutex+++");
	pthread_mutex_lock(&bl_data->int_mutex);
	setup_signal_for_int(bl_data);
	if(fmode == MODE_FG_DT)
		bl_interrupt_init(bl_data);
	else
		bl_capture_init(bl_data);
	bl_enable_irq(bl_data,1);
	pthread_cond_wait(&bl_data->int_wait_cond,&bl_data->int_mutex);
	BTL_DEBUG("mutex---");
	pthread_mutex_unlock(&bl_data->int_mutex);
	return 0;
}
*/
static int bl_alloc_mem_txbuf(struct bl_fingerprint_data *bl_data)
{
	uint32_t width = 80;
	uint32_t height = 80;
    int buf_size = 0;
	//init malloc buf for read frame
	width = bl_data->chip_params->width;
	height = bl_data->chip_params->height;
	buf_size = width * height;
	buf_size =(((buf_size / 1024) + 1)*1024);
	bl_data->buf_size = buf_size;
	BTL_DEBUG("User_GetRawImage devfd=%d width=%d height=%d buf_size=%d",bl_data->devfd, width, height, buf_size);
	bl_data->tx_buf = bf_tee_malloc(buf_size + READ_CMD_SIZE);
	return 0;
}

static int set_chip_default_params(struct bl_fingerprint_data *bl_data)
{
	struct bl_chip_params *chip_params = bl_data->chip_params;
	BTL_DEBUG("+++++++");

#ifdef CONFIG_DEBUG
	int i = 0;
	while(chip_params->params[i].addr != 0xff)
	{
		BTL_DEBUG("addr:0x%x   value:0x%x",chip_params->params[i].addr, chip_params->params[i].value);
		i++;
	}
	BTL_DEBUG("hostcmd_reg addr:0x%x   value:0x%x",chip_params->hostcmd_reg.addr, chip_params->hostcmd_reg.value);
	BTL_DEBUG("fddacp_reg addr:0x%x   value:0x%x",chip_params->fddacp_reg.addr, chip_params->fddacp_reg.value);
	BTL_DEBUG("capdacp_reg addr:0x%x   value:0x%x",chip_params->capdacp_reg.addr, chip_params->capdacp_reg.value);
	BTL_DEBUG("fdgain_reg addr:0x%x   value:0x%x",chip_params->fdgain_reg.addr, chip_params->fdgain_reg.value);
	BTL_DEBUG("capgain_reg addr:0x%x   value:0x%x",chip_params->capgain_reg.addr, chip_params->capgain_reg.value);

	BTL_DEBUG("chipid :0x%x",chip_params->chipid);
	BTL_DEBUG("fdwidth :%d",chip_params->fdwidth);
	BTL_DEBUG("fdheight :%d",chip_params->fdheight);
	BTL_DEBUG("fdframe_num :%d",chip_params->fdframe_num);
	BTL_DEBUG("width :%d",chip_params->width);
	BTL_DEBUG("height :%d",chip_params->height);
	BTL_DEBUG("capframe_num :%d",chip_params->capframe_num);
	BTL_DEBUG("paramslen :%d",chip_params->paramslen);
#endif	
	uint32_t width = 80;
	uint32_t height = 80;
    
	width = bl_data->chip_params->width;
	height = bl_data->chip_params->height;
	//set default params to chip
	bl_data->is_force_set = 1;
	bl_dev_init(bl_data);
	bl_set_frame_num(bl_data, 0);
	bl_set_frame_size(bl_data, height, width);
	bl_set_gain_dacp(bl_data, MODE_FG_DT,chip_params->fdgain_reg.value,chip_params->fddacp_reg.value);
	bl_set_gain_dacp(bl_data, MODE_FG_CAP,chip_params->capgain_reg.value,chip_params->capdacp_reg.value);
	bl_data->is_force_set = 0;
	return 0;
}

int esd_recovery_chip_params(struct bl_fingerprint_data *bl_data)
{
	bl_data->is_force_set = 1;
	bl_dev_init(bl_data);
	if(bl_data->mode == MODE_FG_DT)
	{
		bl_interrupt_init(bl_data);
	}
	else
	{
		bl_capture_init(bl_data);
	}
	
	bl_data->is_force_set = 0;
	return 0;
}

int init_new_fingerprint_data(struct bl_fingerprint_data *bl_data,int fd)
{
	int ret = 0;
	bl_data->devfd = fd;
	//determine the chip type and init params for later use
	ret = bl_read_chipid(bl_data);
	if(ret < 0 )
	{
		return -1;
	}	
	
	//alloc mem for read fingerprint frame
	bl_alloc_mem_txbuf(bl_data);
	
	//init buf and set params to chip
	set_chip_default_params(bl_data);

	return 0;
}

struct bl_fingerprint_data *bl_fingerprint_data_new(int fd, void * config)
{
	struct bl_fingerprint_data *bl_data = NULL;
	int ret = 0;
	if(g_bl_data == NULL)
	{
	bl_data = bf_tee_malloc(sizeof(struct bl_fingerprint_data));
		if(!bl_data)
		{	BTL_DEBUG("no mem!\n");
			return (void *)-1;
		}
		bf_tee_memset(bl_data, 0, sizeof(struct bl_fingerprint_data));
		bl_data->bl_private = config;
    	ret = init_new_fingerprint_data(bl_data, fd);
		if(-1 == ret)
		{
			BTL_DEBUG("init_new_fingerprint_data fail exit\n");
			goto err;
		}
		g_bl_data = bl_data;
	}
	return g_bl_data;
err:
	if(bl_data){
		bf_tee_free(bl_data);
	}
	g_bl_data = NULL;
	return NULL;
}

int destroy_fingerprint_data(struct bl_fingerprint_data *bl_data)
{
	bf_tee_free(bl_data->tx_buf);
	if(NULL != bl_data->bl_private)
	{
		bf_tee_free(bl_data->chip_params->params);
		bf_tee_free(bl_data->chip_params);		
	}

	g_bl_data = NULL;
	bf_tee_free(bl_data);
	return 0;
}
