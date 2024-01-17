/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/waverecorder.c 
  * @author  MCD Application Team
  * @brief   This file provides the Audio In (record) interface API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "receiver.h"
#include "FirFilter.h"
#include "EnvDetector.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TOUCH_RECORD_XMIN       300
#define TOUCH_RECORD_XMAX       340
#define TOUCH_RECORD_YMIN       212
#define TOUCH_RECORD_YMAX       252

#define TOUCH_STOP_XMIN         205
#define TOUCH_STOP_XMAX         245
#define TOUCH_STOP_YMIN         212
#define TOUCH_STOP_YMAX         252

#define TOUCH_PAUSE_XMIN        125
#define TOUCH_PAUSE_XMAX        149
#define TOUCH_PAUSE_YMIN        212
#define TOUCH_PAUSE_YMAX        252

#define TOUCH_VOL_MINUS_XMIN    20
#define TOUCH_VOL_MINUS_XMAX    70
#define TOUCH_VOL_MINUS_YMIN    212
#define TOUCH_VOL_MINUS_YMAX    252

#define TOUCH_VOL_PLUS_XMIN     402
#define TOUCH_VOL_PLUS_XMAX     452
#define TOUCH_VOL_PLUS_YMIN     212
#define TOUCH_VOL_PLUS_YMAX     252

// UMBRALES DE RECEPCIÓN
#define RX_THRESHOLD 8000
#define SILENCE_THRESHOLD 2000
#define SAMPLES_PER_BIT 32

uint8_t pHeaderBuff[44];

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static AUDIO_IN_BufferTypeDef  BufferCtl;
static __IO uint32_t uwVolume_rec = 100;
extern WAVE_FormatTypeDef WaveFormat;
extern FIL WavFile;
static uint32_t  display_update = 1;
static FirFilter filter_BP_5k;
static FirFilter filter_BP_10k;
static EnvDetector envDetector0;
static EnvDetector envDetector1;
static RX_BUFFER_TypeDef BufferRx;
static FIL RxFileHandler;

/* Private function prototypes -----------------------------------------------*/
static uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t* pHeader);
static uint32_t WavProcess_HeaderInit(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);
static uint32_t WavProcess_HeaderUpdate(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);
static AUDIO_ErrorTypeDef update_RX(int16_t sample);
static AUDIO_ErrorTypeDef Read_Buffer(uint16_t * buff);
static void AUDIO_REC_DisplayButtons(void);

/* Private functions ---------------------------------------------------------*/

/*  
  A double MEMS microphone MP45DT02 mounted on STM32746G-DISCOVERY is connected
  to the WM8994 audio codec. The SAI is configured in master
  receiver mode. In this mode, the SAI provides the clock to the WM8994. The
  WM8994 generates a clock to MEMS and acquires the data (Audio samples) from the MEMS
  microphone in PDM format. WM8994 performs PDM to PCM format conversion before
  sending samples to STM32.

  Data acquisition is performed in 16-bit PCM format and using SAI DMA mode.
  
  DMA is configured in circular mode

  In order to avoid data-loss, a 128 bytes buffer is used (BufferCtl.pdm_buff): 
   - When a DMA half transfer is detected using the call back BSP_AUDIO_IN_HalfTransfer_CallBack()
    PCM frame is saved in RecBuf.
  - The samples are then stored in USB buffer.
  - These two steps are repeated  when the DMA Transfer complete interrupt is detected
  - When half of internal USB buffer is reach, an evacuation though USB is done.
  
  To avoid data-loss:
  - IT ISR priority must be set at a higher priority than USB, this priority 
    order must be respected when managing other interrupts; 
*/

/**
 * @brief Comienza a meter muestras de audio en el buffer
 * @param None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef receiver_INIT(void) {
	BSP_AUDIO_IN_Init(I2S_AUDIOFREQ_44K, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
	BSP_AUDIO_IN_Record((uint16_t*)&BufferCtl.pcm_buff[0], AUDIO_IN_PCM_BUFFER_SIZE);
	//BufferCtl.fptr = byteswritten;
	BufferCtl.pcm_ptr = 0;
	BufferCtl.offset = 0;
	BufferCtl.wr_state = BUFFER_EMPTY;
	BufferRx.index = 0;

	// Inicializamos los filtros y el detector de envolvente
	FirFilter_Init(&filter_BP_5k, (uint8_t) BAND_PASS_5K);
	FirFilter_Init(&filter_BP_10k, (uint8_t) BAND_PASS_10K);
	EnvDetector_Init(&envDetector0);
	EnvDetector_Init(&envDetector1);

	return AUDIO_ERROR_NONE;
}


/**
  * @brief  Starts Audio streaming.    
  * @param  None
  * @retval Audio error
  */ 
AUDIO_ErrorTypeDef AUDIO_REC_Start(void)
{
  uint32_t byteswritten = 0;
  uint8_t str[FILEMGR_FILE_NAME_SIZE + 20]; 
  
  uwVolume_rec = 100;

  /* Create a new file system */
  if(f_open(&WavFile, REC_WAVE_NAME, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
  {  
    /* Initialize header file */
    WavProcess_EncInit(DEFAULT_AUDIO_IN_FREQ, pHeaderBuff);
    
    /* Write header file */
    if(f_write(&WavFile, pHeaderBuff, 44, (void*)&byteswritten) == FR_OK)
    {
      AudioState = AUDIO_STATE_PRERECORD;
      
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
      sprintf((char *)str, "Recording file: %s", 
              (char *)REC_WAVE_NAME);
      BSP_LCD_ClearStringLine(4);
      BSP_LCD_DisplayStringAtLine(4, str);
      
      BSP_LCD_SetTextColor(LCD_COLOR_CYAN); 
      sprintf((char *)str,  "Sample rate : %d Hz", (int)DEFAULT_AUDIO_IN_FREQ);
      BSP_LCD_ClearStringLine(6);
      BSP_LCD_DisplayStringAtLine(6, str);
      
      sprintf((char *)str,  "Channels number : %d", (int)DEFAULT_AUDIO_IN_CHANNEL_NBR);
      BSP_LCD_ClearStringLine(7);      
      BSP_LCD_DisplayStringAtLine(7, str);

      sprintf((char *)str,  "Volume : %d ", (int)uwVolume_rec);
      BSP_LCD_ClearStringLine(7);
      BSP_LCD_DisplayStringAtLine(7, str);

      sprintf((char *)str, "File Size :");
      BSP_LCD_ClearStringLine(8);
      BSP_LCD_DisplayStringAtLine(8, str);
      
      AUDIO_REC_DisplayButtons();

      BSP_LCD_DisplayStringAt(247, LINE(6), (uint8_t *)"  [     ]", LEFT_MODE);
      { 
        if(byteswritten != 0)
        {
          BSP_AUDIO_IN_Init(DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
          BSP_AUDIO_IN_Record((uint16_t*)&BufferCtl.pcm_buff[0], AUDIO_IN_PCM_BUFFER_SIZE);
          BufferCtl.fptr = byteswritten;
          BufferCtl.pcm_ptr = 0;
          BufferCtl.offset = 0;
          BufferCtl.wr_state = BUFFER_EMPTY;
          return AUDIO_ERROR_NONE;
        }
      }
    }
  }
  return AUDIO_ERROR_IO; 
}

//if(f_write(&WavFile, (uint8_t*)(BufferCtl.pcm_buff + BufferCtl.offset),
//                 AUDIO_IN_PCM_BUFFER_SIZE,
//                 (void*)&byteswritten) != FR_OK)

static AUDIO_ErrorTypeDef Read_Buffer(uint16_t * buff)
{
	// Actualizamos la máquina de estados del receptor para cada muestra
	for (uint16_t i; i<AUDIO_IN_PCM_BUFFER_SIZE/2; i += 2)
	{
		update_RX(buff[i]);
	}

	return AUDIO_ERROR_NONE;
}

static AUDIO_ErrorTypeDef update_RX(int16_t sample)
{
	static RECEIVER_StateTypeDef state;
	static uint8_t cont;
	static uint8_t currentBit = 0;
	static uint8_t byteRecibido = 0;
	static uint8_t parityCalc = 0;
	static uint16_t bytesCorrectos = 0;
	static uint16_t bytesErroneos = 0;
	static uint8_t fileIndex = 1;
	static uint8_t strFileName[FILEMGR_FILE_NAME_SIZE + 20];

	uint32_t byteswritten = 0;

	// Actualizamos los filtros y los detectores de envolvente
	FirFilter_Update(&filter_BP_5k, (float)sample);
	FirFilter_Update(&filter_BP_10k, (float)sample);
	EnvDetector_Update(&envDetector0, filter_BP_5k.out);
	EnvDetector_Update(&envDetector1, filter_BP_10k.out);

	// Máquina de estados del receptor
	switch(state){

		case SILENCE:
			if(envDetector1.out > RX_THRESHOLD){
				BSP_LCD_DisplayStringAtLine(6, (uint8_t*)"    >> Ha comenzado la recepcion del mensaje");
				state = STOP;
			}
			break;

		case STOP:
			if (envDetector1.out < SILENCE_THRESHOLD)
			{
				// Se pinta en pantalla el porcentaje de error
				//...........................................

				BSP_LCD_DisplayStringAtLine(8, (uint8_t*)"    >> Ha terminado la recepcion!");
				bytesCorrectos = 0;
				bytesErroneos = 0;

				// Abrimos el fichero para almacenar los datos recibidos
				sprintf((char*) strFileName, "RX_message_%d.txt", (uint8_t) fileIndex);
				if (f_open(&RxFileHandler, (char*) strFileName, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
				{
					if(f_write(&RxFileHandler, BufferRx.buff, BufferRx.index, (void*)&byteswritten) == FR_OK)
					{
						BufferRx.index = 0;
					} else
					{
						// IMPRIMIR MENSAJES DE ERROR
					}
				} else
				{
					// IMPRIMIR MENSAJES DE ERROR
				}

				f_close(&RxFileHandler);
				fileIndex++;
				state = SILENCE;
			} else if(envDetector1.out < envDetector0.out)
			{
				state = START;
				cont = SAMPLES_PER_BIT;
			}
			break;

		case START:
			cont--;
			if (cont == 0)
			{
				state = DATA;
				cont = SAMPLES_PER_BIT/2;
			}
			break;

		case DATA:
			cont--;
			if (cont == 0)
			{
				if (currentBit == 8)
				{
					cont = SAMPLES_PER_BIT;
					currentBit++;

					// Se calcula la paridad (par) del byte leido
					// El bit de paridad es 0 si el numero de unos es par, y 1 si es impar
					parityCalc = 0;
					for (uint8_t i = 0; i < 8; i++) {
						parityCalc ^= (byteRecibido >> i) & 1;
					}

					if (parityCalc == (uint8_t)(envDetector1.out > envDetector0.out))
						bytesCorrectos++;
					else
						bytesErroneos++;


				} else if (currentBit == 9)
				{
					currentBit = 0;
					// Se almacena el byte en el buffer
					BufferRx.buff[BufferRx.index] = byteRecibido;
					BufferRx.index++;

					// Se reinicia el byteRecibido
					byteRecibido = 0;
					state = STOP;
				} else
				{
					cont = SAMPLES_PER_BIT;
					// Leemos el bit
					byteRecibido |= (uint8_t)(envDetector1.out > envDetector0.out) << (7-currentBit);

					currentBit++;
				}
			}
			break;
	}

	return AUDIO_ERROR_NONE;
}

AUDIO_ErrorTypeDef Receiver_Process(void)
{
	AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
	// Máquina de estados general
	switch(AudioState){

	case AUDIO_STATE_RECORD:

		/* Check if there are Data to write to USB Key */
		if(BufferCtl.wr_state == BUFFER_FULL)
		{
		  /* write buffer in file */
		  Read_Buffer((uint16_t*)(BufferCtl.pcm_buff + BufferCtl.offset));

		  BufferCtl.wr_state =  BUFFER_EMPTY;
		}


		break;
    case AUDIO_STATE_INIT:
    	AudioState = AUDIO_STATE_RECORD;
    	receiver_INIT();

    	break;
    case AUDIO_STATE_IDLE:
    default:
		/* Do Nothing */
		break;
	}

	return audio_error;
}


/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_REC_Process(void)
{
  uint32_t byteswritten = 0;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  uint32_t elapsed_time; 
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  uint8_t str[16];
  static TS_StateTypeDef  TS_State={0};
  
  switch(AudioState)
  {
  case AUDIO_STATE_PRERECORD:
    if(TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch command */
    {
      BSP_TS_GetState(&TS_State);
    }
    else
    {
      BSP_TS_GetState(&TS_State);
      if(TS_State.touchDetected == 1)
      {
        if ((TS_State.touchX[0] > TOUCH_STOP_XMIN) && (TS_State.touchX[0] < TOUCH_STOP_XMAX) &&
            (TS_State.touchY[0] > TOUCH_STOP_YMIN) && (TS_State.touchY[0] < TOUCH_STOP_YMAX))
        {
          AudioState = AUDIO_STATE_STOP;
        }
        else if ((TS_State.touchX[0] > TOUCH_RECORD_XMIN) && (TS_State.touchX[0] < TOUCH_RECORD_XMAX) &&
                 (TS_State.touchY[0] > TOUCH_RECORD_YMIN) && (TS_State.touchY[0] < TOUCH_RECORD_YMAX))
        {
          display_update = 1;
          AudioState = AUDIO_STATE_RECORD;
        }
        else if((TS_State.touchX[0] > TOUCH_VOL_MINUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_MINUS_XMAX) &&
                (TS_State.touchY[0] > TOUCH_VOL_MINUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_MINUS_YMAX))
        {
          AudioState = AUDIO_STATE_VOLUME_DOWN;
          if(uwVolume_rec >= 5)
          {
        	  uwVolume_rec -= 5;
          }
        }
        else if((TS_State.touchX[0] > TOUCH_VOL_PLUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_PLUS_XMAX) &&
                (TS_State.touchY[0] > TOUCH_VOL_PLUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_PLUS_YMAX))
        {
          AudioState = AUDIO_STATE_VOLUME_UP;
          if(uwVolume_rec <= 95)
          {
        	  uwVolume_rec += 5;
          }
        }

        if ((AudioState == AUDIO_STATE_VOLUME_DOWN) || (AudioState == AUDIO_STATE_VOLUME_UP))
        {
          sprintf((char *)str,  "Volume : %d ", (int)uwVolume_rec);
          BSP_LCD_ClearStringLine(7);
          BSP_LCD_DisplayStringAtLine(7, str);
          BSP_AUDIO_IN_SetVolume(uwVolume_rec);
          AudioState = AUDIO_STATE_PRERECORD;
        }
      }
      else
      {
        AudioState = AUDIO_STATE_PRERECORD;
      }
    }
    break;
  case AUDIO_STATE_RECORD:
    if (display_update)
    {
      BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Display red record circle */
      BSP_LCD_FillCircle((TOUCH_RECORD_XMAX+TOUCH_RECORD_XMIN)/2,
                         (TOUCH_RECORD_YMAX+TOUCH_RECORD_YMIN)/2,
                         (TOUCH_RECORD_XMAX-TOUCH_RECORD_XMIN)/2);
      BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
      BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
      BSP_LCD_DisplayStringAt(247, LINE(6), (uint8_t *)"  [RECORD]", LEFT_MODE);
      display_update = 0;
    }
    
    if(TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch command */
    {
      BSP_TS_GetState(&TS_State);
    }
    else
    {
      BSP_TS_GetState(&TS_State);
      if(TS_State.touchDetected == 1)
      {
        if ((TS_State.touchX[0] > TOUCH_STOP_XMIN) && (TS_State.touchX[0] < TOUCH_STOP_XMAX) &&
            (TS_State.touchY[0] > TOUCH_STOP_YMIN) && (TS_State.touchY[0] < TOUCH_STOP_YMAX))
        {
          AudioState = AUDIO_STATE_STOP;
        }
        else if ((TS_State.touchX[0] > TOUCH_PAUSE_XMIN) && (TS_State.touchX[0] < TOUCH_PAUSE_XMAX) &&
                 (TS_State.touchY[0] > TOUCH_PAUSE_YMIN) && (TS_State.touchY[0] < TOUCH_PAUSE_YMAX))
        {
          AudioState = AUDIO_STATE_PAUSE;
        }
        else if((TS_State.touchX[0] > TOUCH_VOL_MINUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_MINUS_XMAX) &&
                (TS_State.touchY[0] > TOUCH_VOL_MINUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_MINUS_YMAX))
        {
          AudioState = AUDIO_STATE_VOLUME_DOWN;
        }
        else if((TS_State.touchX[0] > TOUCH_VOL_PLUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_PLUS_XMAX) &&
                (TS_State.touchY[0] > TOUCH_VOL_PLUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_PLUS_YMAX))
        {
          AudioState = AUDIO_STATE_VOLUME_UP;
        }
      }
    }
    
    /* MAX Recording time reached, so stop audio interface and close file */
    if(BufferCtl.fptr >= REC_SAMPLE_LENGTH)
    {
      display_update = 1;
      AudioState = AUDIO_STATE_STOP;
      break;
    }
    
    /* Check if there are Data to write to USB Key */
    if(BufferCtl.wr_state == BUFFER_FULL)
    {
      /* write buffer in file */
      if(f_write(&WavFile, (uint8_t*)(BufferCtl.pcm_buff + BufferCtl.offset), 
                 AUDIO_IN_PCM_BUFFER_SIZE, 
                 (void*)&byteswritten) != FR_OK)
      {
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"RECORD FAIL");
        return AUDIO_ERROR_IO;
      }
      BufferCtl.fptr += byteswritten;
      BufferCtl.wr_state =  BUFFER_EMPTY;
    }
    
    /* Display elapsed time */
    elapsed_time = BufferCtl.fptr / (DEFAULT_AUDIO_IN_FREQ * DEFAULT_AUDIO_IN_CHANNEL_NBR * 2); 
    if(prev_elapsed_time != elapsed_time)
    {
      prev_elapsed_time = elapsed_time;
      sprintf((char *)str, "[%02d:%02d]", (int)(elapsed_time /60), (int)(elapsed_time%60));
      BSP_LCD_SetTextColor(LCD_COLOR_YELLOW); 
      BSP_LCD_DisplayStringAt(263, LINE(8), str, LEFT_MODE);
      sprintf((char *)str, "%4d KB", (int)((int32_t)BufferCtl.fptr/1024));
      BSP_LCD_DisplayStringAt(83, LINE(8), str, LEFT_MODE);
    }
    break;
    
  case AUDIO_STATE_STOP:
    /* Stop recorder */
    BSP_AUDIO_IN_Stop(CODEC_PDWN_SW);
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);   /* Display blue cyan record circle */
    BSP_LCD_FillCircle((TOUCH_RECORD_XMAX+TOUCH_RECORD_XMIN)/2,
                       (TOUCH_RECORD_YMAX+TOUCH_RECORD_YMIN)/2,
                       (TOUCH_RECORD_XMAX-TOUCH_RECORD_XMIN)/2);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
                     TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
                     TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    display_update = 1;
    HAL_Delay(150);
    if(f_lseek(&WavFile, 0) == FR_OK)
    {
      /* Update the wav file header save it into wav file */
      WavProcess_HeaderUpdate(pHeaderBuff, &WaveFormat);
      
      if(f_write(&WavFile, pHeaderBuff, sizeof(WAVE_FormatTypeDef), (void*)&byteswritten) == FR_OK)
      {   
        audio_error = AUDIO_ERROR_EOF;
      }
      else
      {
        audio_error = AUDIO_ERROR_IO;
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"RECORD FAIL");          
      }
    }
    else
    {
      BSP_LCD_SetTextColor(LCD_COLOR_RED);
      BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"RECORD FAIL");
      audio_error = AUDIO_ERROR_IO;      
    }
    AudioState = AUDIO_STATE_IDLE;      
    /* Close file */
    f_close(&WavFile);
    break;
    
  case AUDIO_STATE_PAUSE:
    BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Displays red pause rectangles */
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);   /* Display blue cyan record circle */
    BSP_LCD_FillCircle((TOUCH_RECORD_XMAX+TOUCH_RECORD_XMIN)/2,
                       (TOUCH_RECORD_YMAX+TOUCH_RECORD_YMIN)/2,
                       (TOUCH_RECORD_XMAX-TOUCH_RECORD_XMIN)/2);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_DisplayStringAt(247, LINE(6), (uint8_t *)"  [PAUSE] ", LEFT_MODE);    
    BSP_AUDIO_IN_Pause();
    AudioState = AUDIO_STATE_WAIT;
    break;
    
  case AUDIO_STATE_RESUME:
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);    /* Displays blue cyan pause rectangles */
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Display red record circle */
    BSP_LCD_FillCircle((TOUCH_RECORD_XMAX+TOUCH_RECORD_XMIN)/2,
                       (TOUCH_RECORD_YMAX+TOUCH_RECORD_YMIN)/2,
                       (TOUCH_RECORD_XMAX-TOUCH_RECORD_XMIN)/2);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_DisplayStringAt(247, LINE(6), (uint8_t *)"  [RECORD]", LEFT_MODE);   
    BSP_AUDIO_IN_Resume();
    AudioState = AUDIO_STATE_RECORD;
    break;
    
  case AUDIO_STATE_VOLUME_UP:
    if(uwVolume_rec <= 95)
    {
    	uwVolume_rec += 5;
    }
    sprintf((char *)str,  "Volume : %d ", (int)uwVolume_rec);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_ClearStringLine(7);
    BSP_LCD_DisplayStringAtLine(7, str);
    BSP_AUDIO_IN_SetVolume(uwVolume_rec);
    AudioState = AUDIO_STATE_RECORD;
    break;
    
  case AUDIO_STATE_VOLUME_DOWN:
    if(uwVolume_rec >= 5)
    {
    	uwVolume_rec -= 5;
    }
    sprintf((char *)str,  "Volume : %d ", (int)uwVolume_rec);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    BSP_LCD_ClearStringLine(7);
    BSP_LCD_DisplayStringAtLine(7, str);
    BSP_AUDIO_IN_SetVolume(uwVolume_rec);
    AudioState = AUDIO_STATE_RECORD;
    break;
    
  case AUDIO_STATE_NEXT:
  case AUDIO_STATE_PREVIOUS:   
    AudioState = AUDIO_STATE_RECORD;
    break;
    
  case AUDIO_STATE_WAIT:
    if(TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch command */
    {
      BSP_TS_GetState(&TS_State);
    }
    else
    {
      BSP_TS_GetState(&TS_State);
      if(TS_State.touchDetected == 1)
      {
        if ((TS_State.touchX[0] > TOUCH_RECORD_XMIN) && (TS_State.touchX[0] < TOUCH_RECORD_XMAX) &&
            (TS_State.touchY[0] > TOUCH_RECORD_YMIN) && (TS_State.touchY[0] < TOUCH_RECORD_YMAX))
        {
          AudioState = AUDIO_STATE_RESUME;
        }
        else if ((TS_State.touchX[0] > TOUCH_PAUSE_XMIN) && (TS_State.touchX[0] < TOUCH_PAUSE_XMAX) &&
                 (TS_State.touchY[0] > TOUCH_PAUSE_YMIN) && (TS_State.touchY[0] < TOUCH_PAUSE_YMAX))
        {
          AudioState = AUDIO_STATE_RESUME;
        }
      }
    }
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT: 
  default:
    /* Do Nothing */
    break;
  }
  return audio_error;
}

/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  BufferCtl.pcm_ptr+= AUDIO_IN_PCM_BUFFER_SIZE/2;
  if(BufferCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
  {
    BufferCtl.wr_state   =  BUFFER_FULL;
    BufferCtl.offset  = 0;
  }
  
  if(BufferCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
  {
    BufferCtl.wr_state   =  BUFFER_FULL;
    BufferCtl.offset  = AUDIO_IN_PCM_BUFFER_SIZE/2;    
    BufferCtl.pcm_ptr = 0;
  }
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{ 
  BufferCtl.pcm_ptr+= AUDIO_IN_PCM_BUFFER_SIZE/2;
  if(BufferCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
  {
    BufferCtl.wr_state   =  BUFFER_FULL;
    BufferCtl.offset  = 0;
  }
  
  if(BufferCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
  {
    BufferCtl.wr_state   =  BUFFER_FULL;
    BufferCtl.offset  = AUDIO_IN_PCM_BUFFER_SIZE/2;    
    BufferCtl.pcm_ptr = 0;
  }
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Encoder initialization.
  * @param  Freq: Sampling frequency.
  * @param  pHeader: Pointer to the WAV file header to be written.  
  * @retval 0 if success, !0 else.
  */
static uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t *pHeader)
{  
  /* Initialize the encoder structure */
  WaveFormat.SampleRate = Freq;        /* Audio sampling frequency */
  WaveFormat.NbrChannels = 2;          /* Number of channels: 1:Mono or 2:Stereo */
  WaveFormat.BitPerSample = 16;        /* Number of bits per sample (16, 24 or 32) */
  WaveFormat.FileSize = 0x001D4C00;    /* Total length of useful audio data (payload) */
  WaveFormat.SubChunk1Size = 44;       /* The file header chunk size */
  WaveFormat.ByteRate = (WaveFormat.SampleRate * \
                        (WaveFormat.BitPerSample/8) * \
                         WaveFormat.NbrChannels);     /* Number of bytes per second  (sample rate * block align)  */
  WaveFormat.BlockAlign = WaveFormat.NbrChannels * \
                         (WaveFormat.BitPerSample/8); /* channels * bits/sample / 8 */
  
  /* Parse the wav file header and extract required information */
  if(WavProcess_HeaderInit(pHeader, &WaveFormat))
  {
    return 1;
  }
  return 0;
}

/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderInit(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct)
{
  /* Write chunkID, must be 'RIFF'  ------------------------------------------*/
  pHeader[0] = 'R';
  pHeader[1] = 'I';
  pHeader[2] = 'F';
  pHeader[3] = 'F';
  
  /* Write the file length ---------------------------------------------------*/
  /* The sampling time: this value will be written back at the end of the 
     recording operation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pHeader[4] = 0x00;
  pHeader[5] = 0x4C;
  pHeader[6] = 0x1D;
  pHeader[7] = 0x00;
  /* Write the file format, must be 'WAVE' -----------------------------------*/
  pHeader[8]  = 'W';
  pHeader[9]  = 'A';
  pHeader[10] = 'V';
  pHeader[11] = 'E';
  
  /* Write the format chunk, must be'fmt ' -----------------------------------*/
  pHeader[12]  = 'f';
  pHeader[13]  = 'm';
  pHeader[14]  = 't';
  pHeader[15]  = ' ';
  
  /* Write the length of the 'fmt' data, must be 0x10 ------------------------*/
  pHeader[16]  = 0x10;
  pHeader[17]  = 0x00;
  pHeader[18]  = 0x00;
  pHeader[19]  = 0x00;
  
  /* Write the audio format, must be 0x01 (PCM) ------------------------------*/
  pHeader[20]  = 0x01;
  pHeader[21]  = 0x00;
  
  /* Write the number of channels, ie. 0x01 (Mono) ---------------------------*/
  pHeader[22]  = pWaveFormatStruct->NbrChannels;
  pHeader[23]  = 0x00;
  
  /* Write the Sample Rate in Hz ---------------------------------------------*/
  /* Write Little Endian ie. 8000 = 0x00001F40 => byte[24]=0x40, byte[27]=0x00*/
  pHeader[24]  = (uint8_t)((pWaveFormatStruct->SampleRate & 0xFF));
  pHeader[25]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 8) & 0xFF);
  pHeader[26]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 16) & 0xFF);
  pHeader[27]  = (uint8_t)((pWaveFormatStruct->SampleRate >> 24) & 0xFF);
  
  /* Write the Byte Rate -----------------------------------------------------*/
  pHeader[28]  = (uint8_t)((pWaveFormatStruct->ByteRate & 0xFF));
  pHeader[29]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 8) & 0xFF);
  pHeader[30]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 16) & 0xFF);
  pHeader[31]  = (uint8_t)((pWaveFormatStruct->ByteRate >> 24) & 0xFF);
  
  /* Write the block alignment -----------------------------------------------*/
  pHeader[32]  = pWaveFormatStruct->BlockAlign;
  pHeader[33]  = 0x00;
  
  /* Write the number of bits per sample -------------------------------------*/
  pHeader[34]  = pWaveFormatStruct->BitPerSample;
  pHeader[35]  = 0x00;
  
  /* Write the Data chunk, must be 'data' ------------------------------------*/
  pHeader[36]  = 'd';
  pHeader[37]  = 'a';
  pHeader[38]  = 't';
  pHeader[39]  = 'a';
  
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  pHeader[40]  = 0x00;
  pHeader[41]  = 0x4C;
  pHeader[42]  = 0x1D;
  pHeader[43]  = 0x00;
  
  /* Return 0 if all operations are OK */
  return 0;
}

/**
  * @brief  Initialize the wave header file
  * @param  pHeader: Header Buffer to be filled
  * @param  pWaveFormatStruct: Pointer to the wave structure to be filled.
  * @retval 0 if passed, !0 if failed.
  */
static uint32_t WavProcess_HeaderUpdate(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct)
{
  /* Write the file length ---------------------------------------------------*/
  /* The sampling time: this value will be written back at the end of the 
     recording operation.  Example: 661500 Btyes = 0x000A17FC, byte[7]=0x00, byte[4]=0xFC */
  pHeader[4] = (uint8_t)(BufferCtl.fptr);
  pHeader[5] = (uint8_t)(BufferCtl.fptr >> 8);
  pHeader[6] = (uint8_t)(BufferCtl.fptr >> 16);
  pHeader[7] = (uint8_t)(BufferCtl.fptr >> 24);
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  BufferCtl.fptr -=44;
  pHeader[40] = (uint8_t)(BufferCtl.fptr); 
  pHeader[41] = (uint8_t)(BufferCtl.fptr >> 8);
  pHeader[42] = (uint8_t)(BufferCtl.fptr >> 16);
  pHeader[43] = (uint8_t)(BufferCtl.fptr >> 24); 
  
  /* Return 0 if all operations are OK */
  return 0;
}


/**
  * @brief  Display interface touch screen buttons
  * @param  None
  * @retval None
  */
static void AUDIO_REC_DisplayButtons(void)
{
  BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
  BSP_LCD_ClearStringLine(13);            /* Clear dedicated zone */
  BSP_LCD_ClearStringLine(14);
  BSP_LCD_ClearStringLine(15);

  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_FillCircle((TOUCH_RECORD_XMAX+TOUCH_RECORD_XMIN)/2, /* Record circle */
                     (TOUCH_RECORD_YMAX+TOUCH_RECORD_YMIN)/2,
                     (TOUCH_RECORD_XMAX-TOUCH_RECORD_XMIN)/2);
  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);    /* Pause rectangles */
  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
  BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
                   TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
                   TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
  BSP_LCD_DrawRect(TOUCH_VOL_MINUS_XMIN, TOUCH_VOL_MINUS_YMIN , /* VOl- rectangle */
                   TOUCH_VOL_MINUS_XMAX - TOUCH_VOL_MINUS_XMIN,
                   TOUCH_VOL_MINUS_YMAX - TOUCH_VOL_MINUS_YMIN);
  BSP_LCD_DisplayStringAt(24, LINE(14), (uint8_t *)"VOl-", LEFT_MODE);
  BSP_LCD_DrawRect(TOUCH_VOL_PLUS_XMIN, TOUCH_VOL_PLUS_YMIN , /* VOl+ rectangle */
                   TOUCH_VOL_PLUS_XMAX - TOUCH_VOL_PLUS_XMIN,
                   TOUCH_VOL_PLUS_YMAX - TOUCH_VOL_PLUS_YMIN);
  BSP_LCD_DisplayStringAt(24, LINE(14), (uint8_t *)"VOl+", RIGHT_MODE);
  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
  BSP_LCD_DisplayStringAtLine(15, (uint8_t *)"Use record button to start record, stop to exit");
  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
