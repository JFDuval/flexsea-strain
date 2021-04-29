/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'flexsea-strain' 6-ch Strain Gauge Amplifier
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab 
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors] 
*****************************************************************************
	[This file] strain: strain gauge amplifier
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "strain.h"
#include "cyapicallbacks.h"
#include "flexsea_cmd_sensors.h"
#include "flexsea.h"
#include "peripherals.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

volatile uint16 adc_delsig_dma_array[STRAIN_BUF_LEN];
volatile uint8_t strainMemIndex[STRAIN_CHANNELS] = {0,0,0,0,0,0};

#ifdef FAKE_DATA
#warning "Project compiled with FAKE_DATA! 6x sawtooths, no real sensor values."
volatile uint16_t strainFakeData[STRAIN_CHANNELS] = {0,0,0,0,0,0};
#endif

//****************************************************************************
// Private Function Prototype(s)
//****************************************************************************

static void dma_2_config(void);
static void init_adc_delsig_dma_array(void);

#ifdef FAKE_DATA
static void generate_fake_data(void);
#endif

//****************************************************************************
// Puvblic Function(s)
//****************************************************************************

//Enables the peripherals associated with the strain amplifier and
//sets the default values.
//Make sure that you initialize I2C1 first!
void init_strain(void)
{
	int i = 0, j = 0;
	
	//Array prep:
	//=-=-=-=-=-=
	init_adc_delsig_dma_array();
	
	//Peripherals:
	//=-=-=-=-=-=
	
	//16-bits ADC:
	ADC_DelSig_1_Start();
	//ADC_DelSig_1_IRQ_Enable();
	dma_2_config();
	isr_delsig_StartEx(isr_delsig_Interrupt_InterruptCallback);
	
	//Defaults:
	//=-=-=-=-=-=
	for(i = 0; i < STRAIN_CHANNELS; i++)
	{
		strain1.ch[i].offset = STRAIN_DEFAULT_OFFSET;

		for(j = 0; j < STRAIN_BUF_LEN; j++)
		{
			strain1.ch[i].strain_raw[j] = 0;
		}
		strain1.ch[i].strain_filtered = 0;
		strain_config(i, strain1.ch[i].offset);
	}
	
	ADC_DelSig_1_StartConvert();
}

//Configure the strain gauge amplifier
//6-ch, 2 per digital pot. 
void strain_config(uint8 ch, uint8 offs)
{
	uint8 i2c_init_buf[2];
	uint8 addr = 0;
	
	//Assign addresses
	switch(ch)
	{
		case 0:
			i2c_init_buf[0] = STRAIN_OFFSET_A;
			addr = I2C_POT_ADDR_CH1;
			break;
		case 1:
			i2c_init_buf[0] = STRAIN_OFFSET_B;
			addr = I2C_POT_ADDR_CH1;
			break;
		case 2:
			i2c_init_buf[0] = STRAIN_OFFSET_A;
			addr = I2C_POT_ADDR_CH2;
			break;
		case 3:
			i2c_init_buf[0] = STRAIN_OFFSET_B;
			addr = I2C_POT_ADDR_CH2;
			break;
		case 4:
			i2c_init_buf[0] = STRAIN_OFFSET_A;
			addr = I2C_POT_ADDR_CH3;
			break;
		case 5:
			i2c_init_buf[0] = STRAIN_OFFSET_B;
			addr = I2C_POT_ADDR_CH3;
			break;
		default:
			return;
			break;
	}
	
	//Offset:
	i2c_init_buf[1] = offs;		//Offset
	I2C_1_MasterWriteBuf(addr, (uint8 *) i2c_init_buf, 2, I2C_1_MODE_COMPLETE_XFER);	
	CyDelayUs(100);
}

//Returns the latest filtered strain measurement
uint16 strain_read(uint8 ch)
{
	//ADC is auto-sampling, this function simply returns the last filtered value
	
	return strain1.ch[ch].strain_filtered;
}

//Moving average filter:
void strain_filter(void)
{
	uint i = 0;
	uint32 sum = 0;
	uint8 cnt = 0;
	uint16 avg = 0;
	
	//Average for each channel:
	for(i = 0; i < STRAIN_CHANNELS; i++)
	{
		//Zero variables:
		sum = 0;
		avg = 0;
		
		//Shift buffer and sum all but last term
		for(cnt = 0; cnt < STRAIN_BUF_LEN; cnt++)
		{
			sum += strain1.ch[i].strain_raw[cnt];
		}
			
		//Average
		avg = (uint16)(sum >> STRAIN_SHIFT);
		
		//Store in structure:
		strain1.ch[i].strain_filtered = avg;
		//strain_filtered = avg;
	}
}

//Averages the last values received
uint16_t strain_filter_ch(uint8_t ch)
{
	uint8 cnt = 0;
	uint32 sum = 0;
	uint16 avg = 0;
	
	//Shift buffer and sum all but last term
	for(cnt = 0; cnt < STRAIN_BUF_LEN; cnt++)
	{
		sum += strain1.ch[ch].strain_raw[cnt];
	}
		
	//Average
	avg = (uint16)(sum >> STRAIN_SHIFT);
	
	//Store in structure:
	strainMemIndex[ch]++;
	strainMemIndex[ch] %= 4;
	strain1.ch[ch].strain_mem[strainMemIndex[ch]] = avg;
	
	return avg;
}

void strain_filter_all(void)
{
	uint8_t i = 0;
	uint32_t sum = 0;
	
	//Average for each channel:
	for(i = 0; i < STRAIN_CHANNELS; i++)
	{
		sum = strain1.ch[i].strain_mem[0] + \
				strain1.ch[i].strain_mem[1] + \
				strain1.ch[i].strain_mem[2] + \
				strain1.ch[i].strain_mem[3];
		strain1.ch[i].strain_filtered = (uint16_t)(sum >> 2);
	}
}

//Compress 6x uint16 to 9 bytes (12bits per sensor).
//Needed to send all data with RIC/NU Read All function
uint8_t compressAndSplit6ch(uint8_t *buf, uint16_t ch0, uint16_t ch1, uint16_t ch2, \
                            uint16_t ch3, uint16_t ch4, uint16_t ch5)
{
    uint8_t tmp0 = 0, tmp1 = 0;
    uint16_t tmp[6] = {0,0,0,0,0,0};
    uint16_t combo[5] = {0,0,0,0,0};
	
    //Compress to 12bits
    tmp[0] = (ch0 >> 4) & 0x0FFF;
    tmp[1] = (ch1 >> 4) & 0x0FFF;
    tmp[2] = (ch2 >> 4) & 0x0FFF;
    tmp[3] = (ch3 >> 4) & 0x0FFF;
    tmp[4] = (ch4 >> 4) & 0x0FFF;
    tmp[5] = (ch5 >> 4) & 0x0FFF;

    //We want:
    //combo[0]: 0000 0000 0000 1111
    //combo[1]: 1111 1111 2222 2222
    //combo[2]: 2222 3333 3333 3333
    //combo[3]: 4444 4444 4444 5555
    //combo[4]: ____ ____ 5555 5555

    //Combine:
    combo[0] = (tmp[0] << 4) | ((tmp[1] >> 8) & 0xFF);
    combo[1] = (tmp[1] << 8) | ((tmp[2] >> 4) & 0xFFFF);
    combo[2] = (tmp[2] << 12) | (tmp[3]);
    combo[3] = (tmp[4] << 4) | ((tmp[5] >> 8) & 0xFF);
    combo[4] = (tmp[5] & 0xFF);


    //Stock in uint8_t buffer:
	uint16_t index = 0;
	SPLIT_16(combo[0], buf, &index);
	SPLIT_16(combo[1], buf, &index);
	SPLIT_16(combo[2], buf, &index);
	SPLIT_16(combo[3], buf, &index);
	buf[index++] = (uint8_t)combo[4];

    return 0;
}

void compressedStrainToEzi2c(void)
{
	int i = 0;
	
	for(i = MEM_R_CH1_H; i < MEM_R_CH1_H+9; i++)
	{
		ezI2Cbuf[i] = strain1.compressedBytes[i-MEM_R_CH1_H];
	}
}

//Copy latest filtered values to the EZI2C structure
void strain_to_ezi2c(void)
{
	ezI2Cbuf[MEM_R_CH1_H] = (uint8)((strain1.ch[0].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH1_L] = (uint8)((strain1.ch[0].strain_filtered) & 0xFF);
	
	ezI2Cbuf[MEM_R_CH2_H] = (uint8)((strain1.ch[1].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH2_L] = (uint8)((strain1.ch[1].strain_filtered) & 0xFF);
	
	ezI2Cbuf[MEM_R_CH3_H] = (uint8)((strain1.ch[2].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH3_L] = (uint8)((strain1.ch[2].strain_filtered) & 0xFF);
	
	ezI2Cbuf[MEM_R_CH4_H] = (uint8)((strain1.ch[3].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH4_L] = (uint8)((strain1.ch[3].strain_filtered) & 0xFF);
	
	ezI2Cbuf[MEM_R_CH5_H] = (uint8)((strain1.ch[4].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH5_L] = (uint8)((strain1.ch[4].strain_filtered) & 0xFF);
	
	ezI2Cbuf[MEM_R_CH6_H] = (uint8)((strain1.ch[5].strain_filtered >> 8) & 0xFF);
	ezI2Cbuf[MEM_R_CH6_L] = (uint8)((strain1.ch[5].strain_filtered) & 0xFF);
}

//Call this when you are ready to send a packet:
void prepStrainDataForComm(void)
{
	#ifdef FAKE_DATA
	generate_fake_data();
	compressAndSplit6ch(strain1.compressedBytes, strainFakeData[0],strainFakeData[1], 
						strainFakeData[2], strainFakeData[3], strainFakeData[4],
						strainFakeData[5]);
	
	#else
	compressAndSplit6ch(strain1.compressedBytes, strain1.ch[0].strain_filtered, 
						strain1.ch[1].strain_filtered, strain1.ch[2].strain_filtered,
						strain1.ch[3].strain_filtered, strain1.ch[4].strain_filtered,
						strain1.ch[5].strain_filtered);
	#endif//FAKE_DATA
	compressedStrainToEzi2c();
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

#ifdef FAKE_DATA
	
#define MAX_VAL		16384
#define CH_OFFSET	1024
static void generate_fake_data(void)
{
	static uint16_t counter = 0;
	uint8_t i = 0;
	counter += 4;
	if(counter >= MAX_VAL){counter = 0;}
	
	for(i = 0; i < STRAIN_CHANNELS; i++)
	{
		strainFakeData[i] = (counter + (i * CH_OFFSET)) % MAX_VAL;
	}
}

#endif	//FAKE_DATA

//DMA for Delta Sigma ADC.
static void dma_2_config(void)
{
	/* Variable declarations for DMA_2 */
	/* Move these variable declarations to the top of the function */
	uint8 DMA_2_Chan;
	uint8 DMA_2_TD[STRAIN_CHANNELS];

	/* DMA Configuration for DMA_2 */
	#define DMA_2_BYTES_PER_BURST 		2
	#define DMA_2_REQUEST_PER_BURST 	1
	#define DMA_2_SRC_BASE 				(CYDEV_PERIPH_BASE)
	#define DMA_2_DST_BASE 				(CYDEV_SRAM_BASE)
	
	DMA_2_Chan = DMA_2_DmaInitialize(DMA_2_BYTES_PER_BURST, DMA_2_REQUEST_PER_BURST, 
	    HI16(DMA_2_SRC_BASE), HI16(DMA_2_DST_BASE));
	DMA_2_TD[0] = CyDmaTdAllocate();
	CyDmaTdSetConfiguration(DMA_2_TD[0], DMA2_BYTES_PER_XFER, DMA_2_TD[0], DMA_2__TD_TERMOUT_EN | TD_INC_DST_ADR);
	CyDmaTdSetAddress(DMA_2_TD[0], LO16((uint32)ADC_DelSig_1_DEC_SAMP_PTR), LO16((uint32)adc_delsig_dma_array));
	CyDmaChSetInitialTd(DMA_2_Chan, DMA_2_TD[0]);
	CyDmaChEnable(DMA_2_Chan, 1);
}

//Fills the array with zeros
static void init_adc_delsig_dma_array(void)
{
	int i = 0, j = 0;
	
	//For each channel
	for(i = 0; i < STRAIN_CHANNELS; i++)
	{
		//For each sample
		//for(j = 0; j < STRAIN_BUF_LEN; j++)
		{
			//adc_delsig_dma_array[i][j] = 0;
			adc_delsig_dma_array[i] = 0;
		}
	}
}
