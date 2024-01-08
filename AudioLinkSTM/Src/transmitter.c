/**
 ******************************************************************************
 * @file    Audio/Audio_playback_and_record/Src/waveplayer.c
 * @author  MCD Application Team
 * @brief   This file provides the Audio Out (playback) interface API
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
#include "transmitter.h"

/* Private define ------------------------------------------------------------*/
//#define TOUCH_NEXT_XMIN         325
//#define TOUCH_NEXT_XMAX         365
//#define TOUCH_NEXT_YMIN         212
//#define TOUCH_NEXT_YMAX         252

#define TOUCH_BEGIN_TRANSMISSION_XMIN     185
#define TOUCH_BEGIN_TRANSMISSION_XMAX     430
#define TOUCH_BEGIN_TRANSMISSION_YMIN     180
#define TOUCH_BEGIN_TRANSMISSION_YMAX     215

#define TOUCH_STOP_XMIN         170
#define TOUCH_STOP_XMAX         210
#define TOUCH_STOP_YMIN         212
#define TOUCH_STOP_YMAX         252

#define TOUCH_PAUSE_XMIN        100
#define TOUCH_PAUSE_XMAX        124
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

// Modified 2019/10/19, Jose A. Tarifa Galisteo
#define TOUCH_MUTE_XMIN     402
#define TOUCH_MUTE_XMAX     452
#define TOUCH_MUTE_YMIN     155
#define TOUCH_MUTE_YMAX     179
#define TOUCH_FX_XMIN     402
#define TOUCH_FX_XMAX     452
#define TOUCH_FX_YMIN     83
#define TOUCH_FX_YMAX     126

// Se definen los puntos para los nuevos botones (PREVIOUS)  (NEXT)  (RETURN) (BEGGING TRANSMISSION) del transmisor
#define TOUCH_PREVIOUS_XMIN     80
#define TOUCH_PREVIOUS_XMAX     195
#define TOUCH_PREVIOUS_YMIN     135
#define TOUCH_PREVIOUS_YMAX     170

#define TOUCH_NEXT_XMIN     402
#define TOUCH_NEXT_XMAX     452
#define TOUCH_NEXT_YMIN     83
#define TOUCH_NEXT_YMAX     126

#define TOUCH_RETURN_XMIN     402
#define TOUCH_RETURN_XMAX     452
#define TOUCH_RETURN_YMIN     155
#define TOUCH_RETURN_YMAX     179

// Se definen los nuevos valores para los limites de los botones (BACK TO MENU) y (START TRANSISSION)
#define TOUCH_BTM_XMIN     602
#define TOUCH_BTM_XMAX     652
#define TOUCH_BTM_YMIN     83
#define TOUCH_BTM_YMAX     126

#define TOUCH_START_TX_XMIN     700
#define TOUCH_START_TX_XMAX     752
#define TOUCH_START_TX_YMIN     155
#define TOUCH_START_TX_YMAX     179

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static AUDIO_OUT_BufferTypeDef BufferCtl;
static DATA_FILE_BufferTypeDef BufferFile; // En este buffer se almacena el archivo a transmitir.
static AUDIO_IN_BufferTypeDef  WaveBuffer;
static int16_t FilePos = 0;
static __IO uint32_t uwVolume = 30;
static Point NextPoints[] = { { TOUCH_NEXT_XMIN, TOUCH_NEXT_YMIN }, {
		TOUCH_NEXT_XMAX, (TOUCH_NEXT_YMIN + TOUCH_NEXT_YMAX) / 2 }, {
		TOUCH_NEXT_XMIN, TOUCH_NEXT_YMAX } };
static Point PreviousPoints[] = { { TOUCH_PREVIOUS_XMIN, (TOUCH_PREVIOUS_YMIN
		+ TOUCH_PREVIOUS_YMAX) / 2 },
		{ TOUCH_PREVIOUS_XMAX, TOUCH_PREVIOUS_YMIN }, { TOUCH_PREVIOUS_XMAX,
				TOUCH_PREVIOUS_YMAX } };

WAVE_FormatTypeDef WaveFormat;
WAVE_FormatTypeDef MessageWaveFormat;
uint8_t pMessageHeaderBuff[44];
FIL WavFile;
FIL MessageWavFile;
FIL FileHandler;
extern FILELIST_FileTypeDef FileList;
const int16_t sineSamples[] = {
    0b0000000000000000,
    0b0101101010000001,
    0b0111111111111111,
    0b0101101010000001,
    0b0000000000000000,
    0b1010010101111111,
    0b1000000000000001,
    0b1010010101111111
};

static Point puntos_BTM[] = {{TOUCH_BTM_XMIN,TOUCH_BTM_YMIN},
							 {TOUCH_BTM_XMIN, TOUCH_BTM_YMAX},
							 {TOUCH_BTM_XMAX, TOUCH_BTM_YMIN},
							 {TOUCH_BTM_XMAX, TOUCH_BTM_YMIN}};

static Point puntos_START[] = {{TOUCH_START_TX_XMIN,TOUCH_START_TX_YMIN},
							   {TOUCH_START_TX_XMIN, TOUCH_START_TX_YMAX},
							   {TOUCH_START_TX_XMAX, TOUCH_START_TX_YMIN},
							   {TOUCH_START_TX_XMAX, TOUCH_START_TX_YMIN}};

/* Private function prototypes -----------------------------------------------*/
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx,
		WAVE_FormatTypeDef *info);
static uint8_t PlayerInit(uint32_t AudioFreq);
static void AUDIO_PlaybackDisplayButtons(void);
static void InsertarBit(uint8_t bit);
static void AUDIO_AcquireTouchButtons(void);
static void TRANSMITTER_AcquireTouchButtons(void);
static uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t* pHeader);
static uint32_t WavProcess_HeaderInit(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);
static uint32_t WavProcess_HeaderUpdate(uint8_t* pHeader, WAVE_FormatTypeDef* pWaveFormatStruct);
void InsertarBit(uint8_t byteLeido);
static void TRANSMITTER_AcquireTouchButtons_TX();


/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes Audio Interface.
 * @param  None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void) {
	if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, I2S_AUDIOFREQ_44K)
			== 0) {
		return AUDIO_ERROR_NONE;
	} else {
		return AUDIO_ERROR_IO;
	}
}

/**
 * @brief Almacenamos el fichero en BufferFile.
 * @param idx: File index
 * @retval Audio error
 */
AUDIO_ErrorTypeDef ReadFileIntoBuffer(uint8_t idx) {
	uint32_t bytesread;

	f_close(&FileHandler);
	if(f_open(&FileHandler, (char*) FileList.file[idx].name, FA_OPEN_EXISTING | FA_READ) == FR_OK)
	{
		if (FileList.ptr > idx)
		{
			BufferFile.state = BUFFER_OFFSET_NONE;

			/* Get Data from USB Flash Disk */
			f_lseek(&FileHandler, 0);

			/* Fill whole buffer at first time */
			if (f_read(&FileHandler, &BufferFile.buff[0], DATA_FILE_BUFFER_SIZE, (void*) &bytesread) == FR_OK)
			{
				if (bytesread != 0)
				{
					BufferFile.fptr = bytesread;
					return AUDIO_ERROR_NONE;
				}
			}

		}
		return AUDIO_ERROR_IO;
	} else{
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_ClearStringLine(3);
		BSP_LCD_DisplayStringAtLine(3, (uint8_t *)"    >>  FATAL ERROR FETCHING THE INFORMATION OF THE USB");
		return AUDIO_ERROR_IO;

	}

}

/**
 * @brief  Starts Audio streaming.
 * @param  idx: File index
 * @retval Audio error
 */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx) {
	uint32_t bytesread;

	f_close(&WavFile);
	if (AUDIO_GetWavObjectNumber() > idx) {
		GetFileInfo(idx, &WaveFormat);

		/*Adjust the Audio frequency */
		PlayerInit(WaveFormat.SampleRate);

		BufferCtl.state = BUFFER_OFFSET_NONE;

		/* Get Data from USB Flash Disk */
		f_lseek(&WavFile, 0);

		/* Fill whole buffer at first time */
		if (f_read(&WavFile, &BufferCtl.buff[0],
		AUDIO_OUT_BUFFER_SIZE, (void*) &bytesread) == FR_OK) {
			AudioState = AUDIO_STATE_PLAY;
			AUDIO_PlaybackDisplayButtons();
			BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t*) "  [PLAY ]",
					LEFT_MODE);
			{
				if (bytesread != 0) {
					BSP_AUDIO_OUT_Play((uint16_t*) &BufferCtl.buff[0],
							AUDIO_OUT_BUFFER_SIZE);
					BufferCtl.fptr = bytesread;
					return AUDIO_ERROR_NONE;
				}
			}
		}
	}
	return AUDIO_ERROR_IO;
}

/**
 * @brief  Manages Audio process.
 * @param  None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef TRANSMITTER_Process(void) {
	uint32_t bytesread, elapsed_time;
	uint32_t byteswritten = 0;
	uint8_t byteLeido = 0;
	uint8_t parity = 0;
	AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
	static uint32_t prev_elapsed_time = 0xFFFFFFFF;
	uint8_t str[16];
	uint8_t strFileName[FILEMGR_FILE_NAME_SIZE + 20];
//	Point Begin_TransmissionPoints[] = {{TOUCH_BEGIN_TRANSMISSION_XMIN,TOUCH_BEGIN_TRANSMISSION_YMIN},
//	                             {TOUCH_BEGIN_TRANSMISSION_XMAX, TOUCH_BEGIN_TRANSMISSION_YMIN},
//	                             {TOUCH_BEGIN_TRANSMISSION_XMAX, TOUCH_BEGIN_TRANSMISSION_YMAX},
//	    						  {TOUCH_BEGIN_TRANSMISSION_XMIN, TOUCH_BEGIN_TRANSMISSION_YMAX}};
	Point PreviousButtonPoints[] = {{TOUCH_PREVIOUS_XMIN, TOUCH_PREVIOUS_YMIN},
		                             {TOUCH_PREVIOUS_XMAX, TOUCH_PREVIOUS_YMIN},
		                             {TOUCH_PREVIOUS_XMAX, TOUCH_PREVIOUS_YMAX},
		    						  {TOUCH_PREVIOUS_XMIN, TOUCH_PREVIOUS_YMAX}};

	switch (AudioState) {

	/*
	 * Si el estado del búfer (BufferCtl.state) es BUFFER_OFFSET_HALF, se lee una porción del archivo de audio utilizando la función f_read().
	 * La cantidad de bytes leídos se almacena en la variable bytesread. Luego, se actualiza el estado del búfer a BUFFER_OFFSET_NONE y se
	 * incrementa BufferCtl.fptr en la cantidad de bytes leídos. Esto permite avanzar en el archivo de audio a medida que se reproduce.
	 *
	 * */
	case AUDIO_STATE_PLAY:
		if (BufferCtl.fptr >= WaveFormat.FileSize) {
			BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
			AudioState = AUDIO_STATE_NEXT;
		}

		if (BufferCtl.state == BUFFER_OFFSET_HALF) {
			if (f_read(&WavFile, &BufferCtl.buff[0],
			AUDIO_OUT_BUFFER_SIZE / 2, (void*) &bytesread) != FR_OK) {
				BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
				return AUDIO_ERROR_IO;
			}
			BufferCtl.state = BUFFER_OFFSET_NONE;
			BufferCtl.fptr += bytesread;
		}

		if (BufferCtl.state == BUFFER_OFFSET_FULL) {
			if (f_read(&WavFile, &BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE / 2],
			AUDIO_OUT_BUFFER_SIZE / 2, (void*) &bytesread) != FR_OK) {
				BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
				return AUDIO_ERROR_IO;
			}

			BufferCtl.state = BUFFER_OFFSET_NONE;
			BufferCtl.fptr += bytesread;
		}

		/* Display elapsed time */
		elapsed_time = BufferCtl.fptr / WaveFormat.ByteRate;
		if (prev_elapsed_time != elapsed_time) {
			prev_elapsed_time = elapsed_time;
			sprintf((char*) str, "[%02d:%02d]", (int) (elapsed_time / 60),
					(int) (elapsed_time % 60));
			BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
			BSP_LCD_DisplayStringAt(263, LINE(8), str, LEFT_MODE);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		}

		/* Update audio state machine according to touch acquisition */
		AUDIO_AcquireTouchButtons();
		break;

	case AUDIO_STATE_STOP:
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
		BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN, /* Stop rectangle */
		TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
		TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		AudioState = AUDIO_STATE_IDLE;
		audio_error = AUDIO_ERROR_IO;
		break;

	case AUDIO_STATE_NEXT:
		if (++FilePos >= AUDIO_GetWavObjectNumber()) {
			FilePos = 0;
		}
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		AUDIO_PLAYER_Start(FilePos);
		if (uwVolume == 0) {
			BSP_AUDIO_OUT_SetVolume(uwVolume);
		}
		break;

	case AUDIO_STATE_PREVIOUS:
		if (--FilePos < 0) {
			FilePos = AUDIO_GetWavObjectNumber() - 1;
		}
		BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
		AUDIO_PLAYER_Start(FilePos);
		if (uwVolume == 0) {
			BSP_AUDIO_OUT_SetVolume(uwVolume);
		}
		break;

	case AUDIO_STATE_PAUSE:
		BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
		BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t*) "  [PAUSE]",
				LEFT_MODE);
		BSP_LCD_SetTextColor(LCD_COLOR_RED); /* Display red pause rectangles */
		BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN, 15,
				TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
		BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15,
				TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
		BSP_AUDIO_OUT_Pause();
		AudioState = AUDIO_STATE_WAIT;
		break;

	case AUDIO_STATE_RESUME:
		BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
		BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t*) "  [PLAY ]",
				LEFT_MODE);
		/* Display blue cyan pause rectangles */
		BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN, 15,
				TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
		BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15,
				TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
		BSP_AUDIO_OUT_Resume();
		if (uwVolume == 0) {
			BSP_AUDIO_OUT_SetVolume(uwVolume);
		}
		AudioState = AUDIO_STATE_PLAY;
		break;

	case AUDIO_STATE_VOLUME_UP:
		if (uwVolume <= 90) {
			uwVolume += 10;
		}
		BSP_AUDIO_OUT_SetVolume(uwVolume);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		sprintf((char*) str, "Volume : %lu ", uwVolume);
		BSP_LCD_DisplayStringAtLine(9, str);
		AudioState = AUDIO_STATE_PLAY;
		break;

	case AUDIO_STATE_VOLUME_DOWN:
		if (uwVolume >= 10) {
			uwVolume -= 10;
		}
		BSP_AUDIO_OUT_SetVolume(uwVolume);
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
		sprintf((char*) str, "Volume : %lu ", uwVolume);
		BSP_LCD_DisplayStringAtLine(9, str);
		AudioState = AUDIO_STATE_PLAY;
		break;

	case AUDIO_STATE_INIT:
		/* Pintamos todos los botones */
		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
		BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
		sprintf((char*) strFileName, "    >> Prepared file to TX (%d/%d):", FilePos + 1, FileList.ptr);

		BSP_LCD_ClearStringLine(6);
		BSP_LCD_DisplayStringAtLine(6, strFileName);
		sprintf((char*) strFileName, "       %s", (char*) FileList.file[FilePos].name);

		BSP_LCD_ClearStringLine(7);
		BSP_LCD_DisplayStringAtLine(7, strFileName);

		BSP_LCD_DisplayStringAtLine(9, (uint8_t*) "       (PREVIOUS)  (NEXT)  (RETURN)");
		BSP_LCD_DisplayStringAtLine(12, (uint8_t*) "                >>  BEGING TRANSMISSION");

		// Poligono de prueba
		BSP_LCD_FillPolygon(PreviousButtonPoints, 4);
		AudioState = AUDIO_STATE_WAIT;
		break;

	/*
	Estado WAIT:
	Se inicializa la zona de pantalla sensible a los botones creados anteriormente.
	Para ello, se llama a la funcion de aduision de los botones creada especificamente para los botones
	(PREVIOUS)  (NEXT)  (RETURN) y >>  BEGING TRANSMISSION
	*/
	case AUDIO_STATE_WAIT:
		TRANSMITTER_AcquireTouchButtons();
		break;

	/*
	Estado BEGING_TRANSMISSION:
	Se limpia la pantalla y se coloca un nuevo mensage que indica que se está realizando la transmision.
	Se pinta un detalle de una antena.
	Se comienza con el tratamiento de los datos, obteniendolos del buffer y haciendo las operaciones necesarias
	para la conversion en formato wav a traves de la concatenacion de los dos simbolos con los que cuenta la modulacion.
	*/
	case AUDIO_STATE_BEGING_TRANSMISSION:
		
		BSP_LCD_DisplayStringAtLine(2, (uint8_t *)"    >> PREPARING THE TRANSMISSION... CREATING .wav FILE FROM THE SELECTED FILE");
		sprintf((char*) strFileName, "          / \\  `.  __..-,O  / \\_/ \\_/ %s", (char*) FileList.file[FilePos].name);

		BSP_LCD_DisplayStringAtLine(6, (uint8_t *)"           ,-.               _   _");
		BSP_LCD_DisplayStringAtLine(7, strFileName);
		BSP_LCD_DisplayStringAtLine(8, (uint8_t *)"         :   \\ --''_..-'.'");
		BSP_LCD_DisplayStringAtLine(9, (uint8_t *)"         |    . .-' `. '.");
		BSP_LCD_DisplayStringAtLine(10, (uint8_t *)"         :     .     .`.'");
		BSP_LCD_DisplayStringAtLine(11, (uint8_t *)"          \\     `.  /  ..");
		BSP_LCD_DisplayStringAtLine(12, (uint8_t *)"           \\      `.   ' .");
		BSP_LCD_DisplayStringAtLine(13, (uint8_t *)"            `,       `.   \\");
		BSP_LCD_DisplayStringAtLine(14, (uint8_t *)"           ,|,`.        `-.\\");
		BSP_LCD_DisplayStringAtLine(15, (uint8_t *)"          '.||  ``-...__..-`");
		BSP_LCD_DisplayStringAtLine(16, (uint8_t *)"           |  |");
		BSP_LCD_DisplayStringAtLine(17, (uint8_t *)"           |__|");
		BSP_LCD_DisplayStringAtLine(18, (uint8_t *)"           /||\\");
		BSP_LCD_DisplayStringAtLine(19, (uint8_t *)"          //||\\\\");
		BSP_LCD_DisplayStringAtLine(20, (uint8_t *)"         // || \\\\");
		BSP_LCD_DisplayStringAtLine(21, (uint8_t *)"      __//__||__\\\\__");
		// BSP_LCD_DisplayStringAtLine(21, (uint8_t *)"     '--------------' ");


		/* Llenamos el buffer con el contenido del fichero del USB */
		ReadFileIntoBuffer(FilePos);

		/* 
		PROCESO DE CREACION DEL ARCHIVO .wav
		Tenemos que ir leyendo el buffer que se acaba de escribir con el contenido del archivo, donde estaran almacenados 
		byte a byte los datos del fichero a transmitir, y haciendo uso de la funcion InsertarBit(), se iran insertando en 
		WaveBuffer.pcm_buff. Una vez se haya leido el fichero completo, y se haya escrito el contenido del mismo en este 
		buffer, se procede a la escritura del archivo .wav.
		*/

		// Creamos un nuevo fichero
		uint8_t fileCreation = f_open(&MessageWavFile, (char*) "mensajeModulado.wav", FA_CREATE_ALWAYS | FA_WRITE);
		if(fileCreation == FR_OK)
		{
			// Inicializamos
			WavProcess_EncInit(I2S_AUDIOFREQ_44K, pMessageHeaderBuff);
			// Se escribe en esta funcion la cabecera por primera vez.
			uint8_t headerWrite = f_write(&MessageWavFile, pMessageHeaderBuff, 44, (void*)&byteswritten);
			if(headerWrite == FR_OK)
			{
				//Reseteo de los valores de los campos de WaveBuffer
				WaveBuffer.fptr = byteswritten;
				WaveBuffer.pcm_ptr = 0;
				WaveBuffer.offset = 0;
				WaveBuffer.wr_state = BUFFER_EMPTY;

			}else
			{
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_ClearStringLine(4);
				sprintf((char*) strFileName,"    >>  ERROR WRITING THE .wav HEADER. ERROR CODE: %d", headerWrite);
				BSP_LCD_DisplayStringAtLine(4, strFileName);
			}

		}else
		{
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_ClearStringLine(4);
			sprintf((char*) strFileName,"    >>  ERROR CREATING THE .wav FILE. ERROR CODE: %d", fileCreation);
			BSP_LCD_DisplayStringAtLine(4, strFileName);
		}

		// En este buffer, vamos recorriendo la informacion del fichero a transmitir, cuya longitud viene 
		// dada por BufferFile.fptr.
		for (uint8_t i = 0; i < BufferFile.fptr; i++)
		{
			// Se lee un byte de datos del buffer que contiene el fichero a transmitir.
			byteLeido = BufferFile.buff[i];

			// Se insertan el bit de start y el bit de stop de la transmision en el archivo .wav haciendo
			// uso de la funcion InsertarBit().
			InsertarBit(1); // BIT STOP
			InsertarBit(0); // BIT START

			// Se va recorriendo el byteLeido bit a bit, comenzando desde el bit mas significativo. Los
			// bits leiidos se van insertando en el archivo .wav haciendo uso de la funcion InsertarBit().
			for (int8_t j = 7; j >= 0; j--)
			{
				InsertarBit((byteLeido >> j) & 1);
			}
			// Se calcula la paridad (par) del byte leido
			// El bit de paridad es 0 si el numero de unos es par, y 1 si es impar
			parity = 0;
			for (uint8_t i = 0; i < 8; i++) {
				parity ^= (byteLeido >> i) & 1;
			}
			InsertarBit(parity);
		}
		 
		// Una vez se ha reccorrido completament el buffer asociado al archvo y se ha escrito completamente, 
		// se insertan el bit de stop y el bit de finalizacion, que no son mas que dos bits de 1s.
		
		InsertarBit(1); // BIT STOP
		InsertarBit(1); // BIT FINALIZACION

		// Una vez relleno el buffer, se procede a realizar la escritura del .wav
		uint8_t fileWriting = f_write(&MessageWavFile, (uint16_t*)WaveBuffer.pcm_buff, WaveBuffer.pcm_ptr, (void*)&byteswritten);
		if(fileWriting == FR_OK)
		{
			WaveBuffer.fptr += byteswritten;

			// Se actualiza la cabecera del fichero .wav con el tamao total
			uint8_t seeker = f_lseek(&MessageWavFile, 0);
			if(seeker == FR_OK)
			{
				WavProcess_HeaderUpdate(pMessageHeaderBuff, &MessageWaveFormat);
				uint8_t updateHeader = f_write(&MessageWavFile, pMessageHeaderBuff, sizeof(WAVE_FormatTypeDef), (void*)&byteswritten);
				if(updateHeader == FR_OK)
				{
					WaveBuffer.fptr += byteswritten;
					// Se cierra el fichero wav
					f_close(&MessageWavFile);
					BSP_LCD_ClearStringLine(4);
					BSP_LCD_DisplayStringAtLine(4, (uint8_t *)"    >>  .wav FILE CREATED. READY TO TRANSMIT!");

					// Aqui deberia ir al estado de transmitir, pero mientras lo dejamos en este estado hasta que
					// decidamos que hacer.
					// Podriamos poner unos botones para volver al menu principal del estado INIT
					BSP_LCD_ClearStringLine(9);
					BSP_LCD_ClearStringLine(11);
					BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
					BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
					AudioState = AUDIO_STATE_WAV_CREATED;

				}else
				{
					BSP_LCD_SetTextColor(LCD_COLOR_RED);
					BSP_LCD_ClearStringLine(4);
					sprintf((char*) strFileName,"    >>  ERROR UPDATING THE .wav FILE HEADER. ERROR CODE: %d", updateHeader);
					BSP_LCD_DisplayStringAtLine(4, strFileName);
				}

			}else
			{
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				BSP_LCD_ClearStringLine(4);
				sprintf((char*) strFileName,"    >>  ERROR SEEKING THE .wav FILE. ERROR CODE: %d", seeker);
				BSP_LCD_DisplayStringAtLine(4, strFileName);
			}

		}else
		{
			BSP_LCD_SetTextColor(LCD_COLOR_RED);
			BSP_LCD_ClearStringLine(4);
			sprintf((char*) strFileName,"    >>  ERROR WRITING THE .wav FILE. ERROR CODE: %d", fileWriting);
			BSP_LCD_DisplayStringAtLine(4, strFileName);
		}

		/* MAX Recording time reached, so stop audio interface and close file */
//		if(WaveBuffer.fptr >= REC_SAMPLE_LENGTH)
//		{
//		  display_update = 1;
//		  AudioState = AUDIO_STATE_STOP;
//		  break;
//		}
		// Se me ocurren dos ideas: poner un boton transmit que sera el que nos llevara al estado de
		// reproducir el audio generado. La otra, limpiar la pantalla y volver al menu de transmitir

		// Otra opcion, aniadir un boton y que al pulsarlo nos lleve de vuelte al menu principal de
		// transmision. Voy a implementar esta ultima
		//BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
		//AudioState = AUDIO_STATE_INIT;
		break;

	case AUDIO_STATE_WAV_CREATED:
		// Ya se ha creado el fichero .wav a partir del fichero de de texto, por lo que ahora
		// se procede a pintar los botones y esperar a que sean pulsados.
		BSP_LCD_DisplayStringAtLine(9, (uint8_t *)"         |    . .-' `. '.       >>    (BACK TO TX MENU)");
		BSP_LCD_DisplayStringAtLine(11, (uint8_t *)"          \\     `.  /  ..     >>    (START TRANSMISSION)");


		AudioState = AUDIO_STATE_WAIT_FOR_TRANSMISSION;

		break;

	case AUDIO_STATE_WAIT_FOR_TRANSMISSION:
		// LLamada a una funcion para adquirir la informacion de los toques de la pantalla.
		// No se puede usar la que ya tenemos por si se lian a tocar ahi random en la pantalla
		// Pintamos los rectangulos para aproximar el espacio que queremos
		BSP_LCD_FillPolygon(puntos_BTM, 4);
		BSP_LCD_FillPolygon(puntos_START, 4);
		TRANSMITTER_AcquireTouchButtons_TX();
		break;

	case AUDIO_STATE_IDLE:
	case AUDIO_STATE_TRANSMISSION:
	default:
		/* Update audio state machine according to touch acquisition */
		AUDIO_AcquireTouchButtons();
		break;
	}
	return audio_error;
}

/**
 * @brief  Stops Audio streaming.
 * @param  None
 * @retval Audio error
 */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void) {
	AudioState = AUDIO_STATE_STOP;
	FilePos = 0;

	BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	f_close(&WavFile);
	return AUDIO_ERROR_NONE;
}

/**
 * @brief  Calculates the remaining file size and new position of the pointer.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
	if (AudioState == AUDIO_STATE_PLAY) {
		BufferCtl.state = BUFFER_OFFSET_FULL;
	}
}

/**
 * @brief  Manages the DMA Half Transfer complete interrupt.
 * @param  None
 * @retval None
 */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	if (AudioState == AUDIO_STATE_PLAY) {
		BufferCtl.state = BUFFER_OFFSET_HALF;
	}
}
/*******************************************************************************
 Static Functions
 *******************************************************************************/
/**
 * @brief  Escribe la secuencia en el archivo WAV.
 * @param  byteLeido: byte leido del fichero que txt que se quiere transmitir
 */
static void InsertarBit(uint8_t bit)
{
	uint32_t byteswritten = 0;

	if (WaveBuffer.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE)
	{
		// Si se ha llenado el buffer se escribe el buffer en el fichero
		f_write(&MessageWavFile, (uint16_t*)WaveBuffer.pcm_buff, WaveBuffer.pcm_ptr, (void*)&byteswritten);
		WaveBuffer.pcm_ptr = 0;
		WaveBuffer.fptr += byteswritten;
	}

	if (bit == 1)
	{
		// Si el bit leido es igual a 1, se transmiten unicamente las muestras del seno correspondientes 
		// a una señal de 11025 Hz, que serían el 0, 2, 4, 6
		for (uint8_t periodo = 0; periodo < 4; periodo++)
		{
			// Se configura el número de periodos a transmitir, en este caso serian 4 periodos por cada bit.
			for (uint8_t j = 0; j < 8; j += 2)
			{
				// Por ultimo, se sinsertan las muestras del seno en el buffer, dos veces, ya que la donfiguracion
				// que permite el STM32 es estereo.
				WaveBuffer.pcm_buff[WaveBuffer.pcm_ptr] = sineSamples[j];
				WaveBuffer.pcm_buff[WaveBuffer.pcm_ptr+1] = sineSamples[j];

				// Por ultimo se incrementa el puntero del buffer en 2 debido a las dos posiciones escritas.
				WaveBuffer.pcm_ptr += 2;
			}
		}
	} else 
	{
		// En este caso, el bit leido es un cero, por lo que la seal a transmitir es la correspondiente a 5512.5 Hz, y
		// se transmiten todas las muestras del seno. 
		for (uint8_t periodo = 0; periodo < 2; periodo++)
		{
			// Se configura el número de periodos a transmitir, en este caso serian 2 periodos por cada bit.
			for (uint8_t j = 0; j < 8; j++)
			{
				// De igual forma qque anteriormente, se insertan las muestras en el bufferm dos veces.
				WaveBuffer.pcm_buff[WaveBuffer.pcm_ptr] = sineSamples[j];
				WaveBuffer.pcm_buff[WaveBuffer.pcm_ptr+1] = sineSamples[j];
				
				// Se actualiza el puntero del buffer.
				WaveBuffer.pcm_ptr += 2;
			}
		}
	}
}

/**
 * @brief  Gets the file info.
 * @param  file_idx: File index
 * @param  info: Pointer to WAV file info
 * @retval Audio error
 */
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx,
		WAVE_FormatTypeDef *info) {
	uint32_t bytesread;
	uint32_t duration;
	uint8_t str[FILEMGR_FILE_NAME_SIZE + 20];

	if (f_open(&WavFile, (char*) FileList.file[file_idx].name,
			FA_OPEN_EXISTING | FA_READ) == FR_OK) {
		/* Fill the buffer to Send */
		if (f_read(&WavFile, info, sizeof(WaveFormat), (void*) &bytesread)
				== FR_OK) {
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			sprintf((char*) str, "Playing file (%d/%d): %s", file_idx + 1,
					FileList.ptr, (char*) FileList.file[file_idx].name);
			BSP_LCD_ClearStringLine(4);
			BSP_LCD_DisplayStringAtLine(4, str);

			BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
			sprintf((char*) str, "Sample rate : %d Hz",
					(int) (info->SampleRate));
			BSP_LCD_ClearStringLine(6);
			BSP_LCD_DisplayStringAtLine(6, str);

			sprintf((char*) str, "Channels number : %d", info->NbrChannels);
			BSP_LCD_ClearStringLine(7);
			BSP_LCD_DisplayStringAtLine(7, str);

			duration = info->FileSize / info->ByteRate;
			sprintf((char*) str, "File Size : %d KB [%02d:%02d]",
					(int) (info->FileSize / 1024), (int) (duration / 60),
					(int) (duration % 60));
			BSP_LCD_ClearStringLine(8);
			BSP_LCD_DisplayStringAtLine(8, str);
			BSP_LCD_DisplayStringAt(263, LINE(8), (uint8_t*) "[00:00]",
					LEFT_MODE);

			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			sprintf((char*) str, "Volume : %lu", uwVolume);
			BSP_LCD_ClearStringLine(9);
			BSP_LCD_DisplayStringAtLine(9, str);
			return AUDIO_ERROR_NONE;
		}
		f_close(&WavFile);
	}
	return AUDIO_ERROR_IO;
}

/**
 * @brief  Initializes the Wave player.
 * @param  AudioFreq: Audio sampling frequency
 * @retval None
 */
static uint8_t PlayerInit(uint32_t AudioFreq) {
	/* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
	if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0) {
		return 1;
	} else {
		BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
		return 0;
	}
}

/**
 * @brief  Display interface touch screen buttons
 * @param  None
 * @retval None
 */
static void AUDIO_PlaybackDisplayButtons(void) {
	BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
	BSP_LCD_ClearStringLine(13); /* Clear dedicated zone */
	BSP_LCD_ClearStringLine(14);
	BSP_LCD_ClearStringLine(15);

	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_FillPolygon(PreviousPoints, 3); /* Previous track icon */
	BSP_LCD_FillRect(TOUCH_PREVIOUS_XMIN, TOUCH_PREVIOUS_YMIN, 10,
			TOUCH_PREVIOUS_YMAX - TOUCH_PREVIOUS_YMIN);
	BSP_LCD_FillPolygon(NextPoints, 3); /* Next track icon */
	BSP_LCD_FillRect(TOUCH_NEXT_XMAX - 9, TOUCH_NEXT_YMIN, 10,
			TOUCH_NEXT_YMAX - TOUCH_NEXT_YMIN);
	BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN, 15,
			TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN); /* Pause rectangles */
	BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15,
			TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
	BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN, /* Stop rectangle */
	TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
	TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
	BSP_LCD_DrawRect(TOUCH_VOL_MINUS_XMIN, TOUCH_VOL_MINUS_YMIN, /* VOl- rectangle */
	TOUCH_VOL_MINUS_XMAX - TOUCH_VOL_MINUS_XMIN,
	TOUCH_VOL_MINUS_YMAX - TOUCH_VOL_MINUS_YMIN);
	BSP_LCD_DisplayStringAt(24, LINE(14), (uint8_t*) "VOl-", LEFT_MODE);
	BSP_LCD_DrawRect(TOUCH_VOL_PLUS_XMIN, TOUCH_VOL_PLUS_YMIN, /* VOl+ rectangle */
	TOUCH_VOL_PLUS_XMAX - TOUCH_VOL_PLUS_XMIN,
	TOUCH_VOL_PLUS_YMAX - TOUCH_VOL_PLUS_YMIN);
	BSP_LCD_DisplayStringAt(24, LINE(14), (uint8_t*) "VOl+", RIGHT_MODE);

	// ------------------------------------------------------------------------------------------
	// Modified 2019/10/19, Jose A. Tarifa Galisteo
	// Boton MUTE (silencio)
//	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
//	BSP_LCD_DrawRect(TOUCH_MUTE_XMIN, TOUCH_MUTE_YMIN, /* Mute rectangle */
//	TOUCH_MUTE_XMAX - TOUCH_MUTE_XMIN,
//	TOUCH_MUTE_YMAX - TOUCH_MUTE_YMIN);
//	BSP_LCD_DisplayStringAt(24, LINE(10), (uint8_t *) "mute", RIGHT_MODE);
	// ------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------
	// Boton para EFECTOS
//		BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
//		BSP_LCD_DrawRect(TOUCH_FX_XMIN,  TOUCH_FX_YMIN , /* Mute rectangle */
//	                   TOUCH_FX_XMAX - TOUCH_FX_XMIN,
//	                   TOUCH_FX_YMAX - TOUCH_FX_YMIN);
//		BSP_LCD_DisplayStringAt(26, LINE(6), (uint8_t *)"F X", RIGHT_MODE);
	// ------------------------------------------------------------------------------------------

	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
	BSP_LCD_ClearStringLine(15);
	BSP_LCD_DisplayStringAtLine(15, (uint8_t*) "Use stop button to exit");
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
}


/**
 * @brief  Aduiere la informacion de tocar los botones del transmisor
 * @param  None
 * @retval None
 */
static void TRANSMITTER_AcquireTouchButtons(void) {
	static TS_StateTypeDef TS_State = { 0 };

	if (TS_State.touchDetected == 1) /* If previous touch has not been released, we don't proceed any touch command */
	{
		BSP_TS_GetState(&TS_State);
	} else {
		// Se ha soltado el toque previo
		BSP_TS_GetState(&TS_State);
		if (TS_State.touchDetected == 1) {
			if ((TS_State.touchX[0] > TOUCH_PREVIOUS_XMIN) &&
				(TS_State.touchX[0] < TOUCH_PREVIOUS_XMAX) &&
				(TS_State.touchY[0] > TOUCH_PREVIOUS_YMIN) &&
				(TS_State.touchY[0] < TOUCH_PREVIOUS_YMAX))
			{
				AudioState = AUDIO_STATE_PREVIOUS;

			}else if ((TS_State.touchX[0] > TOUCH_NEXT_XMIN) &&
					  (TS_State.touchX[0] < TOUCH_NEXT_XMAX) &&
					  (TS_State.touchY[0] > TOUCH_NEXT_YMIN) &&
					  (TS_State.touchY[0] < TOUCH_NEXT_YMAX))
			{
				AudioState = AUDIO_STATE_NEXT;

			}else if ((TS_State.touchX[0] > TOUCH_RETURN_XMIN) &&
					  (TS_State.touchX[0] < TOUCH_RETURN_XMAX) &&
					  (TS_State.touchY[0] > TOUCH_RETURN_YMIN) &&
					  (TS_State.touchY[0] < TOUCH_RETURN_YMAX))
			{
				AudioState = AUDIO_STATE_BACKWARD;

			}else if((TS_State.touchX[0] > TOUCH_BEGIN_TRANSMISSION_XMIN) &&
					 (TS_State.touchX[0] < TOUCH_BEGIN_TRANSMISSION_XMAX) &&
					 (TS_State.touchY[0] > TOUCH_BEGIN_TRANSMISSION_YMIN) &&
					 (TS_State.touchY[0] < TOUCH_BEGIN_TRANSMISSION_YMAX))
			{
				BSP_LCD_Clear(LCD_COLOR_DARKGREEN);
				BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGREEN);
				BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);

				AudioState = AUDIO_STATE_BEGING_TRANSMISSION;
			}
		}
	}
}

static void TRANSMITTER_AcquireTouchButtons_TX(void) {
	static TS_StateTypeDef TS_State = { 0 };
	if (TS_State.touchDetected == 1) /* If previous touch has not been released, we don't proceed any touch command */
	{
		BSP_TS_GetState(&TS_State);
	} else {
		if (TS_State.touchDetected == 1)
		{
			if ((TS_State.touchX[0] > TOUCH_BTM_XMIN) &&
				(TS_State.touchX[0] < TOUCH_BTM_XMAX) &&
				(TS_State.touchY[0] > TOUCH_BTM_YMIN) &&
				(TS_State.touchY[0] < TOUCH_BTM_YMAX))
			{
				AudioState = AUDIO_STATE_INIT;

			}else
			{
				if ((TS_State.touchX[0] > TOUCH_START_TX_XMIN) &&
					(TS_State.touchX[0] < TOUCH_START_TX_XMAX) &&
					(TS_State.touchY[0] > TOUCH_START_TX_YMIN) &&
					(TS_State.touchY[0] < TOUCH_START_TX_YMAX))
				{
					AudioState = AUDIO_STATE_TRANSMISSION;
				}
			}
		}
	}
}



/**
 * @brief  Test touch screen state and modify audio state machine according to that
 * @param  None
 * @retval None
 */
static void AUDIO_AcquireTouchButtons(void) {
	static TS_StateTypeDef TS_State = { 0 };

	if (TS_State.touchDetected == 1) /* If previous touch has not been released, we don't proceed any touch command */
	{
		BSP_TS_GetState(&TS_State);
	} else {
		BSP_TS_GetState(&TS_State);
		if (TS_State.touchDetected == 1) {
			if ((TS_State.touchX[0] > TOUCH_PAUSE_XMIN)
					&& (TS_State.touchX[0] < TOUCH_PAUSE_XMAX)
					&& (TS_State.touchY[0] > TOUCH_PAUSE_YMIN)
					&& (TS_State.touchY[0] < TOUCH_PAUSE_YMAX)) {
				if (AudioState == AUDIO_STATE_PLAY) {
					AudioState = AUDIO_STATE_PAUSE;
				} else {
					AudioState = AUDIO_STATE_RESUME;
				}
			} else if ((TS_State.touchX[0] > TOUCH_NEXT_XMIN)
					&& (TS_State.touchX[0] < TOUCH_NEXT_XMAX)
					&& (TS_State.touchY[0] > TOUCH_NEXT_YMIN)
					&& (TS_State.touchY[0] < TOUCH_NEXT_YMAX)) {
				AudioState = AUDIO_STATE_NEXT;
			} else if ((TS_State.touchX[0] > TOUCH_PREVIOUS_XMIN)
					&& (TS_State.touchX[0] < TOUCH_PREVIOUS_XMAX)
					&& (TS_State.touchY[0] > TOUCH_PREVIOUS_YMIN)
					&& (TS_State.touchY[0] < TOUCH_PREVIOUS_YMAX)) {
				AudioState = AUDIO_STATE_PREVIOUS;
			} else if ((TS_State.touchX[0] > TOUCH_STOP_XMIN)
					&& (TS_State.touchX[0] < TOUCH_STOP_XMAX)
					&& (TS_State.touchY[0] > TOUCH_STOP_YMIN)
					&& (TS_State.touchY[0] < TOUCH_STOP_YMAX)) {
				AudioState = AUDIO_STATE_STOP;
			} else if ((TS_State.touchX[0] > TOUCH_VOL_MINUS_XMIN)
					&& (TS_State.touchX[0] < TOUCH_VOL_MINUS_XMAX)
					&& (TS_State.touchY[0] > TOUCH_VOL_MINUS_YMIN)
					&& (TS_State.touchY[0] < TOUCH_VOL_MINUS_YMAX)) {
				AudioState = AUDIO_STATE_VOLUME_DOWN;
			} else if ((TS_State.touchX[0] > TOUCH_VOL_PLUS_XMIN)
					&& (TS_State.touchX[0] < TOUCH_VOL_PLUS_XMAX)
					&& (TS_State.touchY[0] > TOUCH_VOL_PLUS_YMIN)
					&& (TS_State.touchY[0] < TOUCH_VOL_PLUS_YMAX)) {
				AudioState = AUDIO_STATE_VOLUME_UP;
			}
		}
	}
}

/**
  * @brief  Encoder initialization.
  * @param  Freq: Sampling frequency.
  * @param  pHeader: Pointer to the WAV file header to be written.
  * @retval 0 if success, !0 else.
  */
static uint32_t WavProcess_EncInit(uint32_t Freq, uint8_t *pHeader)
{
  /* Initialize the encoder structure */
  MessageWaveFormat.SampleRate = Freq;        /* Audio sampling frequency */
  MessageWaveFormat.NbrChannels = 2;          /* Number of channels: 1:Mono or 2:Stereo */
  MessageWaveFormat.BitPerSample = 16;        /* Number of bits per sample (16, 24 or 32) */
  MessageWaveFormat.FileSize = 0x001D4C00;    /* Total length of useful audio data (payload) */
  MessageWaveFormat.SubChunk1Size = 44;       /* The file header chunk size */
  MessageWaveFormat.ByteRate = (MessageWaveFormat.SampleRate * \
                        (MessageWaveFormat.BitPerSample/8) * \
                         MessageWaveFormat.NbrChannels);     /* Number of bytes per second  (sample rate * block align)  */
  MessageWaveFormat.BlockAlign = MessageWaveFormat.NbrChannels * \
                         (MessageWaveFormat.BitPerSample/8); /* channels * bits/sample / 8 */

  /* Parse the wav file header and extract required information */
  if(WavProcess_HeaderInit(pHeader, &MessageWaveFormat))
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
  pHeader[4] = (uint8_t)(WaveBuffer.fptr);
  pHeader[5] = (uint8_t)(WaveBuffer.fptr >> 8);
  pHeader[6] = (uint8_t)(WaveBuffer.fptr >> 16);
  pHeader[7] = (uint8_t)(WaveBuffer.fptr >> 24);
  /* Write the number of sample data -----------------------------------------*/
  /* This variable will be written back at the end of the recording operation */
  WaveBuffer.fptr -=44;
  pHeader[40] = (uint8_t)(WaveBuffer.fptr);
  pHeader[41] = (uint8_t)(WaveBuffer.fptr >> 8);
  pHeader[42] = (uint8_t)(WaveBuffer.fptr >> 16);
  pHeader[43] = (uint8_t)(WaveBuffer.fptr >> 24);

  /* Return 0 if all operations are OK */
  return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
