/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/menu.c 
  * @author  MCD Application Team
  * @brief   This file implements Menu Functions
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
#include "transmitter.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define TOUCH_RECORD_XMIN       300
//#define TOUCH_RECORD_XMAX       340
//#define TOUCH_RECORD_YMIN       212
//#define TOUCH_RECORD_YMAX       252
//
//#define TOUCH_PLAYBACK_XMIN     125
//#define TOUCH_PLAYBACK_XMAX     165
//#define TOUCH_PLAYBACK_YMIN     212
//#define TOUCH_PLAYBACK_YMAX     252

// Definición de los límites para los botones de los modos TRANSMITTER y RECEIVER
#define TOUCH_TRANSMITTER_XMIN     40
#define TOUCH_TRANSMITTER_XMAX     210
#define TOUCH_TRANSMITTER_YMIN     150
#define TOUCH_TRANSMITTER_YMAX     180


#define TOUCH_RECEIVER_XMIN     40
#define TOUCH_RECEIVER_XMAX     210
#define TOUCH_RECEIVER_YMIN     200
#define TOUCH_RECEIVER_YMAX     230

/* Private macro -------------------------------------------------------------*/
/* Global extern variables ---------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
AUDIO_DEMO_StateMachine     AudioDemo;
AUDIO_PLAYBACK_StateTypeDef AudioState;

/* Private function prototypes -----------------------------------------------*/
static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode);
static void LCD_ClearTextZone(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Manages AUDIO Menu Process.
  * @param  None
  * @retval None
  */
void AUDIO_MenuProcess(void)
{
  AUDIO_ErrorTypeDef  status;
  TS_StateTypeDef  TS_State;
//  Point PlaybackLogoPoints[] = {{TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMIN},
//                                {TOUCH_PLAYBACK_XMIN, TOUCH_PLAYBACK_YMAX},
//                                {TOUCH_PLAYBACK_XMAX, TOUCH_PLAYBACK_YMIN},
//  	  	  	  	  	  	  	  	{TOUCH_PLAYBACK_XMAX, TOUCH_PLAYBACK_YMAX}};

  
  /* Hacemos un rectangulo con el tamaño del receiver y del transmitter */
//  Point TransmitterPoints[] = {{TOUCH_TRANSMITTER_XMIN,TOUCH_TRANSMITTER_YMIN},
//                              {TOUCH_TRANSMITTER_XMAX, TOUCH_TRANSMITTER_YMIN},
//                              {TOUCH_TRANSMITTER_XMAX, TOUCH_TRANSMITTER_YMAX},
//							  {TOUCH_TRANSMITTER_XMIN, TOUCH_TRANSMITTER_YMAX}};
//
//  Point ReceiverPoints[] = {{TOUCH_RECEIVER_XMIN, TOUCH_RECEIVER_YMIN},
//                            {TOUCH_RECEIVER_XMAX, TOUCH_RECEIVER_YMIN},
//                            {TOUCH_RECEIVER_XMAX, TOUCH_RECEIVER_YMAX},
//                            {TOUCH_RECEIVER_XMIN, TOUCH_RECEIVER_YMAX}};


  if(appli_state == APPLICATION_READY)
  { 
    switch(AudioDemo.state)
    {
    /*
    ESTADO 1: IDLE
    DEFINICION: actualizar la pantalla con la intefaz de usuario.
    TRANSICION--> WAIT
    */
    case IDLE:
      AudioDemo.state = WAIT;
	  BSP_LCD_SetBackColor(LCD_COLOR_DARKGREEN);
	  BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
	  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);

	  BSP_LCD_DisplayStringAtLine(2, (uint8_t *)"       ___          ___      __   _      __    ____________  ___");
	  BSP_LCD_DisplayStringAtLine(3, (uint8_t *)"      / _ |__ _____/ (_)__  / /  (_)__  / /__ / __/_  __/  |/  /");
	  BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"     / __ / // / _  / / _ \\/ /__/ / _ \\/  '_/_\\ \\  / / / /|_/ / ");
	  BSP_LCD_DisplayStringAtLine(5, (uint8_t *)"    /_/ |_\\_,_/\\_,_/_/\\___/____/_/_//_/_/\\_\\/___/ /_/ /_/  /_/  ");
	  //BSP_LCD_ClearStringLine(13);     /* Clear touch screen buttons dedicated zone */

	  // Pintamos en la pantalla las posibles opciones (estados) a los qque podamos pasar al pulsar.
	  // MIRAR LA TIPOGRAFÍA PARA PONER ALGUNA MÁS RETRO
	  BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
	  BSP_LCD_DisplayStringAtLine(10, (uint8_t *)"    >> TRANSMITTER");
	  BSP_LCD_DisplayStringAtLine(13, (uint8_t *)"    >> RECEIVER");

	  // Creamos los rectángulos de contanto para la selección del modo
	  // Aseguramos de seleccionar la capa 1 y, no se si funcionará, pero es poner la trasparencia.
	  BSP_LCD_SelectLayer(1);
	  //BSP_LCD_FillPolygon(TransmitterPoints, 4);
	  //BSP_LCD_FillPolygon(ReceiverPoints, 4);
	  break;

    /*											****************************************
    ESTADO 2: WAIT
    DEFINICION: Se crean los eventos de tocar la pantalla tactil en alguno de los rectángulos que identifican a cada uno de los estados.
    TRANSICION-->
    			TRANSMITTER: Si se toca el area correspondiente al estado TRANSMITTER
    			RECEIVER : Si se toca el area correspondiente al estado RECEIVER
    			EXPLORE ??????
    */
    case WAIT:
    	BSP_TS_GetState(&TS_State);
    	if(TS_State.touchDetected == 1)
    	{
    		//Se pulsa sobre el area correspondiente a TRANSMITTER, por lo que se transiciona a dicho estado
    		if ((TS_State.touchX[0] > TOUCH_TRANSMITTER_XMIN) &&
    			(TS_State.touchX[0] < TOUCH_TRANSMITTER_XMAX) &&
    			(TS_State.touchY[0] > TOUCH_TRANSMITTER_YMIN) &&
				(TS_State.touchY[0] < TOUCH_TRANSMITTER_YMAX))
    		{
    			AudioDemo.state = TRANSMITTER;

    			// Almacenamos todos los nombres de los ficheros almacenados en el USB
    			AUDIO_StorageParse();

    		}
    		//Se pulsa sobre el area correspondiente a RECEIVER, por lo que se transiciona a dicho estado
    		else if ((TS_State.touchX[0] > TOUCH_RECEIVER_XMIN) &&
    				 (TS_State.touchX[0] < TOUCH_RECEIVER_XMAX) &&
    				 (TS_State.touchY[0] > TOUCH_RECEIVER_YMIN) &&
					 (TS_State.touchY[0] < TOUCH_RECEIVER_YMAX))
    		{
    			AudioDemo.state = RECEIVER;
    			receiver_INIT();
    		}

        /* Wait for touch released */
        do
        {
          BSP_TS_GetState(&TS_State);
        }while(TS_State.touchDetected > 0);
      }
    break;

    case TRANSMITTER:
    	if(AudioState == AUDIO_STATE_IDLE)
    	{
    		BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
    		BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
    		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
    		BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"    >> --TRANSMITTER--");

    		// Ponemos en funcionamiento la máquina de estados del transmisor
    		AudioState = AUDIO_STATE_INIT;
    	}
    	else
    	{
    		// Si el estado NO era idle, se pasa del menu principal y se procede a entrar a la maquina de estados directamente
    		if(TRANSMITTER_Process() == AUDIO_ERROR_IO)
			{
				/* Clear the LCD */
				AudioDemo.state = IDLE;
			}

    	}
    	break;

    case RECEIVER:
        /* Clear the LCD */
		LCD_ClearTextZone();
		BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
		BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"    >> --RECEIVER--");
		BSP_LCD_DisplayStringAtLine(5, (uint8_t *)"    >> Ready to receive...");

		break;
      
    case AUDIO_DEMO_IN:
      if(appli_state == APPLICATION_READY)
      {
        if(AudioState == AUDIO_STATE_IDLE)
        {
          /* Start Playing */
          AudioState = AUDIO_STATE_INIT;
          
          /* Clear the LCD */
          LCD_ClearTextZone();
          
          /* Configure the audio recorder: sampling frequency, bits-depth, number of channels */
          if(AUDIO_REC_Start() == AUDIO_ERROR_IO)
          {
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
        }
        else /* Not idle */
        {
          status = AUDIO_REC_Process();
          if((status == AUDIO_ERROR_IO) || (status == AUDIO_ERROR_EOF))
          {
            /* Clear the LCD */
            LCD_ClearTextZone();
            
            AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU);  
            AudioDemo.state = AUDIO_DEMO_IDLE;
          }
        }
      }
      else
      {
        AudioDemo.state = AUDIO_DEMO_WAIT;
      }
      break;
      
    default:
      break;
    }
  }
  
  if(appli_state == APPLICATION_DISCONNECT)
  {
    appli_state = APPLICATION_IDLE;     
    AUDIO_ChangeSelectMode(AUDIO_SELECT_MENU); 
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);    
  }
}

/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Changes the selection mode.
  * @param  select_mode: Selection mode
  * @retval None
  */
static void AUDIO_ChangeSelectMode(AUDIO_DEMO_SelectMode select_mode)
{
  if(select_mode == AUDIO_SELECT_MENU)
  {
    LCD_LOG_UpdateDisplay(); 
    AudioDemo.state = AUDIO_DEMO_IDLE; 
  }
  else if(select_mode == AUDIO_PLAYBACK_CONTROL)
  {
    LCD_ClearTextZone();   
  }
}

/**
  * @brief  Clears the text zone.
  * @param  None
  * @retval None
  */
static void LCD_ClearTextZone(void)
{
  uint8_t i = 0;
  
  for(i= 0; i < 13; i++)
  {
    BSP_LCD_ClearStringLine(i + 3);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
