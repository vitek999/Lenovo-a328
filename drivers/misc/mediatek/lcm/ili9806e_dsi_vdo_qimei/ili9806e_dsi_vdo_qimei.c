
#if defined(BUILD_LK)
#include <string.h>
#else
#include <linux/string.h>
#endif


#if defined(BUILD_LK)
#include "cust_gpio_usage.h"
#include <platform/mt_gpio.h>
#else
#include "cust_gpio_usage.h"
#include <mach/mt_gpio.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  			(480)
#define FRAME_HEIGHT 			(854)

#define REGFLAG_DELAY             	0xFD
#define REGFLAG_END_OF_TABLE      	0xFE   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE		0


#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define GPIO_LCM_ID	GPIO119
#define LCM_TDT			0

bool lcm_ili9806e_vendor=LCM_TDT;	//default to choose byd panel


//set LCM IC ID
#define LCM_ID_ILI9806E 		(0x980604)

#define LCM_DEBUG

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    			(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 				(lcm_util.udelay(n))
#define MDELAY(n) 				(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        	lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};



static struct LCM_setting_table lcm_tdt_initialization_setting[] = {

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1
	{0x08,	1,	{0x10}},	// output SDA
	{0x21,	1,	{0x01}},	// DE = 1 Active
	{0x30,	1,	{0x01}},	// 480 X 854
	{0x31,	1,	{0x02}},	// 2-dot Inversion
	{0x40,	1,	{0x16}},	// BT
	{0x41,	1,	{0x33}},	// DVDDH DVDDL clamp 
	{0x42,	1,	{0x03}},	// VGH/VGL 
	{0x43,	1,	{0x09}},	// VGH_CLAMP 0FF ;
	{0x44,	1,	{0x07}},	// VGL_CLAMP OFF ; 
	{0x45,	1,	{0x16}},	// VGL_REG  -11V 
	{0x46,	1,	{0x55}},	// AVDD AVEE CHARGE PUMPING FREQ; 
	{0x47,	1,	{0x55}},	// VGH VGL CHARGE PUMPING FREQ; 
	{0x50,	1,	{0x78}},	// VGMP
	{0x51,	1,	{0x78}},	// VGMN
	{0x52,	1,	{0x00}},	// Flicker 
	{0x53,	1,	{0x62}},	// Flicker  83 adj by peter wang, 0x7c
	{0x60,	1,	{0x07}},	// SDTI
	{0x61,	1,	{0x00}},	// CRTI
	{0x62,	1,	{0x08}},	// EQTI
	{0x63,	1,	{0x00}},	// PCTI
	//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	{0xA0,	1,	{0x00}},	// Gamma 255     
	{0xA1,	1,	{0x0A}},	// Gamma 251    
	{0xA2,	1,	{0x10}},	// Gamma 247   
	{0xA3,	1,	{0x0C}},	// Gamma 239   
	{0xA4,	1,	{0x04}},	// Gamma 231  
	{0xA5,	1,	{0x07}},	// Gamma 203  
	{0xA6,	1,	{0x07}},	// Gamma 175  
	{0xA7,	1,	{0x05}},	// Gamma 147   
	{0xA8,	1,	{0x07}},	// Gamma 108  
	{0xA9,	1,	{0x0B}},	// Gamma 80  
	{0xAA,	1,	{0x13}},	// Gamma 52  
	{0xAB,	1,	{0x08}},	// Gamma 24   
	{0xAC,	1,	{0x10}},	// Gamma 16    
	{0xAD,	1,	{0x12}},	// Gamma 8  
	{0xAE,	1,	{0x0C}},	// Gamma 4      
	{0xAF,	1,	{0x00}},	// Gamma 0   
  ///==============Nagitive       
	{0xC0,	1,	{0x00}},	// Gamma 255    
	{0xC1,	1,	{0x0A}},	// Gamma 251   
	{0xC2,	1,	{0x10}},	// Gamma 247  
	{0xC3,	1,	{0x0C}},	// Gamma 239  
	{0xC4,	1,	{0x04}},	// Gamma 231 
	{0xC5,	1,	{0x07}},	// Gamma 203  
	{0xC6,	1,	{0x07}},	// Gamma 175   
	{0xC7,	1,	{0x05}},	// Gamma 147   
	{0xC8,	1,	{0x07}},	// Gamma 108   
	{0xC9,	1,	{0x0B}},	// Gamma 80   
	{0xCA,	1,	{0x13}},	// Gamma 52   
	{0xCB,	1,	{0x08}},	// Gamma 24    
	{0xCC,	1,	{0x10}},	// Gamma 16    
	{0xCD,	1,	{0x12}},	// Gamma 8   
	{0xCE,	1,	{0x0C}},	// Gamma 4    
	{0xCF,	1,	{0x00}},	// Gamma 0    
	
	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x06}},// Change to Page 6
	{0x00,	1,	{0x20}},	//
	{0x01,	1,	{0x0A}},	//
	{0x02,	1,	{0x00}},	//
	{0x03,	1,	{0x00}},	//
	{0x04,	1,	{0x01}},	//
	{0x05,	1,	{0x01}},	//
	{0x06,	1,	{0x98}},	//
	{0x07,	1,	{0x06}},	//
	{0x08,	1,	{0x01}},	//
	{0x09,	1,	{0x80}},	//
	{0x0A,	1,	{0x00}},	//
	{0x0B,	1,	{0x00}},	//
	{0x0C,	1,	{0x01}},	//
	{0x0D,	1,	{0x01}},	//
	{0x0E,	1,	{0x00}},	//
	{0x0F,	1,	{0x00}},	//
	{0x10,	1,	{0xF0}},	//
	{0x11,	1,	{0xF4}},	//
	{0x12,	1,	{0x01}},	//
	{0x13,	1,	{0x00}},	//
	{0x14,	1,	{0x00}},	//
	{0x15,	1,	{0xC0}},	//
	{0x16,	1,	{0x08}},	//
	{0x17,	1,	{0x00}},	//
	{0x18,	1,	{0x00}},	//
	{0x19,	1,	{0x00}},	//
	{0x1A,	1,	{0x00}},	//
	{0x1B,	1,	{0x00}},	//
	{0x1C,	1,	{0x00}},	//
	{0x1D,	1,	{0x00}},	//
	{0x20,	1,	{0x01}},	//
	{0x21,	1,	{0x23}},	//
	{0x22,	1,	{0x45}},	//
	{0x23,	1,	{0x67}},	//
	{0x24,	1,	{0x01}},	//
	{0x25,	1,	{0x23}},	//
	{0x26,	1,	{0x45}},	//
	{0x27,	1,	{0x67}},	//
	{0x30,	1,	{0x11}},	//
	{0x31,	1,	{0x11}},	//
	{0x32,	1,	{0x00}},	//
	{0x33,	1,	{0xEE}},	//
	{0x34,	1,	{0xFF}},	//	
	{0x35,	1,	{0xBB}},	//	
	{0x36,	1,	{0xAA}},	//
	{0x37,	1,	{0xDD}},	//
	{0x38,	1,	{0xCC}},	//
	{0x39,	1,	{0x66}},	//		
	{0x3A,	1,	{0x77}},	//
	{0x3B,	1,	{0x22}},	//		
	{0x3C,	1,	{0x22}},	//
	{0x3D,	1,	{0x22}},	//	
	{0x3E,	1,	{0x22}},	//
	{0x3F,	1,	{0x22}},	//
	{0x40,	1,	{0x22}},	//
	{0x52,	1,	{0x10}},	//
	{0x53,	1,	{0x10}},	//VGLO tie to VGL;

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x07}},// Change to Page 7
	{0x18,	1,	{0x1D}},	// VREG1 VREG2 output
	{0x17,	1,	{0x32}},	// VGL_REG ON
	{0x02,	1,	{0x77}},	//  

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
	{0x11,	1,	{0x00}},	// Sleep-Out
	{REGFLAG_DELAY, 150, {}},
	{0x29,	1,	{0x00}},	// Display on

	//{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_tdt_initialization_debug1[] = {

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1
	{0x08,	1,	{0x10}},	// output SDA
	{0x21,	1,	{0x01}},	// DE = 1 Active
	{0x30,	1,	{0x01}},	// 480 X 854
	{0x31,	1,	{0x02}},	// 2-dot Inversion
	{0x40,	1,	{0x16}},	// BT
	{0x41,	1,	{0x33}},	// DVDDH DVDDL clamp 
	{0x42,	1,	{0x03}},	// VGH/VGL 
	{0x43,	1,	{0x09}},	// VGH_CLAMP 0FF ;
	{0x44,	1,	{0x07}},	// VGL_CLAMP OFF ; 
	{0x45,	1,	{0x16}},	// VGL_REG  -11V 
	{0x50,	1,	{0x78}},	// VGMP
	{0x51,	1,	{0x78}},	// VGMN
	{0x52,	1,	{0x00}},	// Flicker 
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_tdt_initialization_debug2[] = {
	{0x60,  1,  {0x07}},	  // SDTI
	{0x61,  1,  {0x00}},	  // CRTI
	{0x62,  1,  {0x08}},	  // EQTI
	{0x63,  1,  {0x00}},	  // PCTI
	//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	{0xA0,  1,  {0x00}},	  // Gamma 255	   
	{0xA1,  1,  {0x0A}},	  // Gamma 251	  
	{0xA2,  1,  {0x10}},	  // Gamma 247	 
	{0xA3,  1,  {0x0C}},	  // Gamma 239	 
	{0xA4,  1,  {0x04}},	  // Gamma 231	
	{0xA5,  1,  {0x07}},	  // Gamma 203	
	{0xA6,  1,  {0x07}},	  // Gamma 175	
	{0xA7,  1,  {0x05}},	  // Gamma 147	 
	{0xA8,  1,  {0x07}},	  // Gamma 108	
	{0xA9,  1,  {0x0B}},	  // Gamma 80  
	{0xAA,  1,  {0x13}},	  // Gamma 52  
	{0xAB,  1,  {0x08}},	  // Gamma 24	
	{0xAC,  1,  {0x10}},	  // Gamma 16	 
	{0xAD,  1,  {0x12}},	  // Gamma 8  
	{0xAE,  1,  {0x0C}},	  // Gamma 4	  
	{0xAF,  1,  {0x00}},	  // Gamma 0   
	///==============Nagitive		
	{0xC0,  1,  {0x00}},	  // Gamma 255	  
	{0xC1,  1,  {0x0A}},	  // Gamma 251	 
	{0xC2,  1,  {0x10}},	  // Gamma 247	
	{0xC3,  1,  {0x0C}},	  // Gamma 239	
	{0xC4,  1,  {0x04}},	  // Gamma 231 
	{0xC5,  1,  {0x07}},	  // Gamma 203	
	{0xC6,  1,  {0x07}},	  // Gamma 175	 
	{0xC7,  1,  {0x05}},	  // Gamma 147	 
	{0xC8,  1,  {0x07}},	  // Gamma 108	 
	{0xC9,  1,  {0x0B}},	  // Gamma 80	
	{0xCA,  1,  {0x13}},	  // Gamma 52	
	{0xCB,  1,  {0x08}},	  // Gamma 24	 
	{0xCC,  1,  {0x10}},	  // Gamma 16	 
	{0xCD,  1,  {0x12}},	  // Gamma 8   
	{0xCE,  1,  {0x0C}},	  // Gamma 4	
	{0xCF,  1,  {0x00}},	  // Gamma 0	

	{0xFF,  5,  {0xFF, 0x98, 0x06, 0x04, 0x06}},// Change to Page 6
	{0x00,  1,  {0x20}},	  //
	{0x01,  1,  {0x0A}},	  //
	{0x02,  1,  {0x00}},	  //
	{0x03,  1,  {0x00}},	  //
	{0x04,  1,  {0x01}},	  //
	{0x05,  1,  {0x01}},	  //
	{0x06,  1,  {0x98}},	  //
	{0x07,  1,  {0x06}},	  //
	{0x08,  1,  {0x01}},	  //
	{0x09,  1,  {0x80}},	  //
	{0x0A,  1,  {0x00}},	  //
	{0x0B,  1,  {0x00}},	  //
	{0x0C,  1,  {0x01}},	  //
	{0x0D,  1,  {0x01}},	  //
	{0x0E,  1,  {0x00}},	  //
	{0x0F,  1,  {0x00}},	  //
	{0x10,  1,  {0xF0}},	  //
	{0x11,  1,  {0xF4}},	  //
	{0x12,  1,  {0x01}},	  //
	{0x13,  1,  {0x00}},	  //
	{0x14,  1,  {0x00}},	  //
	{0x15,  1,  {0xC0}},	  //
	{0x16,  1,  {0x08}},	  //
	{0x17,  1,  {0x00}},	  //
	{0x18,  1,  {0x00}},	  //
	{0x19,  1,  {0x00}},	  //
	{0x1A,  1,  {0x00}},	  //
	{0x1B,  1,  {0x00}},	  //
	{0x1C,  1,  {0x00}},	  //
	{0x1D,  1,  {0x00}},	  //
	{0x20,  1,  {0x01}},	  //
	{0x21,  1,  {0x23}},	  //
	{0x22,  1,  {0x45}},	  //
	{0x23,  1,  {0x67}},	  //
	{0x24,  1,  {0x01}},	  //
	{0x25,  1,  {0x23}},	  //
	{0x26,  1,  {0x45}},	  //
	{0x27,  1,  {0x67}},	  //
	{0x30,  1,  {0x11}},	  //
	{0x31,  1,  {0x11}},	  //
	{0x32,  1,  {0x00}},	  //
	{0x33,  1,  {0xEE}},	  //
	{0x34,  1,  {0xFF}},	  //  
	{0x35,  1,  {0xBB}},	  //  
	{0x36,  1,  {0xAA}},	  //
	{0x37,  1,  {0xDD}},	  //
	{0x38,  1,  {0xCC}},	  //
	{0x39,  1,  {0x66}},	  //	  
	{0x3A,  1,  {0x77}},	  //
	{0x3B,  1,  {0x22}},	  //	  
	{0x3C,  1,  {0x22}},	  //
	{0x3D,  1,  {0x22}},	  //  
	{0x3E,  1,  {0x22}},	  //
	{0x3F,  1,  {0x22}},	  //
	{0x40,  1,  {0x22}},	  //
	{0x52,  1,  {0x10}},	  //
	{0x53,  1,  {0x10}},	  //VGLO tie to VGL;

	{0xFF,  5,  {0xFF, 0x98, 0x06, 0x04, 0x07}},// Change to Page 7
	{0x18,  1,  {0x1D}},	  // VREG1 VREG2 output
	{0x17,  1,  {0x32}},	  // VGL_REG ON
	{0x02,  1,  {0x77}},	  //  

	{0xFF,  5,  {0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
	{0x11,  1,  {0x00}},	  // Sleep-Out
	{REGFLAG_DELAY, 150, {}},
	{0x29,  1,  {0x00}},	  // Display on

	//{0xFF,	  5,  {0xFF, 0x98, 0x06, 0x04, 0x01}},// Change to Page 1

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif


static struct LCM_setting_table lcm_sleep_out_setting[] = {

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0

	// Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 150, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	//{0x2C, 1, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {

	{0xFF,	5,	{0xFF, 0x98, 0x06, 0x04, 0x00}},// Change to Page 0
	
	// Display off sequence
	{0x28, 1, {0x00}},
    {REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
		dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		params->dsi.packet_size=256;

		// Video mode setting	

		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.intermediat_buffer_num = 2;	
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.word_count=480*3;

		//here is for esd protect by legen
		params->dsi.noncont_clock = true;
		params->dsi.noncont_clock_period=2;
		params->dsi.lcm_ext_te_enable=true;
		//for esd protest end by legen

		//delete by zhuqiang 2013.3.4 
		//	params->dsi.word_count=FRAME_WIDTH*3;	
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.vertical_sync_active=2;  //4
		params->dsi.vertical_backporch=18;	//16
		params->dsi.vertical_frontporch=20;
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.vertical_active_line=FRAME_HEIGHT;

		//delete by zhuqiang 2013.3.4 
		//	params->dsi.line_byte=2180;		
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.horizontal_sync_active=10;  
		
//zrl modify for improve the TP reort point begin,130916
		params->dsi.horizontal_backporch=60;   //50  60 
		params->dsi.horizontal_frontporch=60;  //50   200
//zrl modify for improve the TP reort point end,130916
		  
		// add by zhuqiang for FR437058 at 2013.4.25 end
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;	//added by zhuqiang 2013.3.4 
		
		// add by zhuqiang for FR437058 at 2013.4.25 begin
		params->dsi.pll_div1=1;         //  div1=0,1,2,3;  div1_real=1,2,4,4
		params->dsi.pll_div2=1;         // div2=0,1,2,3;div2_real=1,2,4,4
		
//zrl modify for improve the TP reort point begin,130916
        params->dsi.fbk_div =30;              // fref=26MHz,  fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)   //32
//zrl modify for improve the TP reort point end,130916

		// add by zhuqiang for FR437058 at 2013.4.25 end
}

//legen add for detect lcm vendor
static bool lcm_select_panel(void)
{
	int value=0;
	
	mt_set_gpio_mode(GPIO_LCM_ID,GPIO_MODE_00);

	mt_set_gpio_pull_enable(GPIO_LCM_ID, GPIO_PULL_DISABLE);

	mt_set_gpio_dir(GPIO_LCM_ID, GPIO_DIR_IN);

	value+=mt_get_gpio_in(GPIO_LCM_ID);
	if(value)
		return LCM_TDT;
	
	return LCM_TDT;
}
//legen add end 

static int first_init=0;
static void lcm_init(void)
{
    unsigned int data_array[16];

#if defined(BUILD_LK)
  	lcm_ili9806e_vendor=lcm_select_panel();
#else
  	if(!first_init)
  	{
  		first_init=1;
		lcm_ili9806e_vendor=lcm_select_panel();
  	}
#endif

#ifdef BUILD_LK
	printf("[%s]lk,ili9806e,zrl choose lcm vendor:%d-%s\n",__func__,lcm_ili9806e_vendor,lcm_ili9806e_vendor?"BYD":"TDT");
#else
	printk("[%s]kernel,ili9806e,zrl choose lcm vendor:%d-%s\n",__func__,lcm_ili9806e_vendor,lcm_ili9806e_vendor?"BYD":"TDT");
#endif

    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(120);

    if(lcm_ili9806e_vendor == LCM_TDT)
  	{
		push_table(lcm_tdt_initialization_setting, sizeof(lcm_tdt_initialization_setting) / sizeof(struct LCM_setting_table), 1);
  	}
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


#if 0

static void lcm_resume(void)
{
	static int temp = 0x46;
	unsigned int data_array[16];

	//zrl add for debug for Cut  screen, 121015
	push_table(lcm_tdt_initialization_debug1, sizeof(lcm_tdt_initialization_debug1) / sizeof(struct LCM_setting_table), 1);

	data_array[0]= 0x00023902;
	data_array[1]= 0x00|0x00|(temp<<8)|0x53;
	dsi_set_cmdq(&data_array, 2, 1);

	push_table(lcm_tdt_initialization_debug2, sizeof(lcm_tdt_initialization_debug2) / sizeof(struct LCM_setting_table), 1);

#if defined(BUILD_LK)
	printf("zrl lcm_resume,temp = 0x%x,data_array[1] = 0x%x\n",temp,data_array[1]);
#else
	printk("zrl lcm_resume,temp = 0x%x,data_array[1] = 0x%x\n",temp,data_array[1]);
#endif

	//x1=temp%256;
	//x2=temp/256;
	temp+=2;

}

#else

static void lcm_resume(void)
{

	//lcm_init();  
	//MDELAY(200);

	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#endif


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}


static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK

	unsigned char buffer[4];
	unsigned int array[16];

#if defined(LCM_DEBUG)
		printk("ili9806e: lcm_esd_check enter\n");
#endif

	array[0]=0x00063902;
	array[1]=0x0698ffff;
	array[2]=0x00000004;
	dsi_set_cmdq(array, 3, 1);
	MDELAY(10);

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x0A, buffer, 1);
	
#if defined(LCM_DEBUG)
	printk("lcm_esd_check  0x0A = %x\n",buffer[0]);
#endif

	if(buffer[0] != 0x9C)
	{
		return 1;
	}

	array[0] = 0x00043700;
	dsi_set_cmdq(array,1,1);
	read_reg_v2(0x0B, buffer[0], 1);

	array[0] = 0x00043700;
	dsi_set_cmdq(array,1,1);
	read_reg_v2(0x0C, buffer[1], 1);

	array[0] = 0x00043700;
	dsi_set_cmdq(array,1,1);
	read_reg_v2(0x0D, buffer[2], 1);

	array[0] = 0x00043700;
	dsi_set_cmdq(array,1,1);
	read_reg_v2(0x0E, buffer[3], 1);
	

#if defined(LCM_DEBUG)
	printk("lcm_esd_check  0x09(bit0~3) = %x, %x, %x, %x\n",buffer[0],buffer[1],buffer[2],buffer[3]);
#endif

	//if ((buffer[0]==0x80)&&(buffer[1]==0x73)&&(buffer[2]==0x04)&&(buffer[3]==0x00))
	if ((buffer[0]==0x80)&&(buffer[1]==0x73)&&(buffer[2]==0x04)&&((buffer[3]==0x00) || (buffer[3]==0x01)))
	{
	#if defined(LCM_DEBUG)
		printk("ili9806e: lcm_esd_check exit\n");
	#endif
		return 0;
	}
	else
	{
		return 1;
	}
	
#endif
}


static unsigned int lcm_esd_recover(void)
{
#ifndef BUILD_LK

   printk("ili9806e: lcm_esd_recover enter");
   lcm_init();
   return 1;

 #endif 
}


// ---------------------------------------------------------------------------
//  Get LCM ID Information
// ---------------------------------------------------------------------------
static unsigned int lcm_compare_id();
static unsigned int lcm_compare_id()
{

	 	int array[4];
        char buffer[5];
        char id_high=0;
        char id_midd=0;
        char id_low=0;
        int id=0;
 
        //Do reset here
        SET_RESET_PIN(1);
        SET_RESET_PIN(0);
        MDELAY(25);       
        SET_RESET_PIN(1);
        MDELAY(50);      
       
        array[0]=0x00063902;
        array[1]=0x0698ffff;
        array[2]=0x00000104;
        dsi_set_cmdq(array, 3, 1);
        MDELAY(10);
 
        array[0]=0x00033700;
        dsi_set_cmdq(array, 1, 1);
        //read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.
    
        read_reg_v2(0x00, buffer,1);
        id_high = buffer[0]; ///////////////////////0x98
 
        read_reg_v2(0x01, buffer,1);
        id_midd = buffer[0]; ///////////////////////0x06
 
        read_reg_v2(0x02, buffer,1);
        id_low = buffer[0]; ////////////////////////0x04
 
        id =(id_high << 16) | (id_midd << 8) | id_low;

#if defined(BUILD_LK)
			  printf("zrl_lk -- ili9806e 0x%x , 0x%x , 0x%x, 0x%x \n", id_high, id_midd, id_low, id);
#else
			  printk("zrl_kernel -- ili9806e 0x%x , 0x%x , 0x%x, 0x%x \n", id_high, id_midd, id_low, id);
#endif

		 return (id == LCM_ID_ILI9806E)?1:0;
 
        
} 

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER ili9806e_dsi_vdo_qimei_lcm_drv =
{
    .name			= "ili9806e_dsi_vdo_qimei",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
	
//	.set_backlight	= lcm_setbacklight,
//	.set_pwm        = lcm_setpwm,
//	.get_pwm        = lcm_getpwm,
   
//	.esd_check   = lcm_esd_check,
//   	.esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
};
