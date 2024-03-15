#include "speexC.h"

SpeexBits bitsEncode;  
SpeexBits bitsDecode; 

void *enc_state, *dec_state;/* Holds the states of the encoder & the decoder */

/**
  * @brief  Initializes the speex codec
  * @param  None
  * @retval : None
  */
void SpeexEncode_Init(void)
{
	spx_int32_t frame_size;
	spx_int32_t rate=0;
	int quality = 4;/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */

  /* Speex encoding initializations */ 
  speex_bits_init(&bitsEncode);
  enc_state = speex_encoder_init(&speex_nb_mode);
	
	speex_encoder_ctl(enc_state, SPEEX_RESET_STATE , 0);
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY,&quality);
	
	speex_encoder_ctl(enc_state, SPEEX_GET_BITRATE,&rate);
	speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE,&frame_size);
	
	quality = 1;
	speex_encoder_ctl(enc_state, SPEEX_SET_COMPLEXITY, &quality);
	
}


int SpeexEncodeData(uint16_t *EnCode,uint8_t *OutBytes)
{
	/* Flush all the bits in the struct so we can encode a new frame */
	speex_bits_reset(&bitsEncode);
	/* Encode the frame */
	speex_encode_int(enc_state, (spx_int16_t *)EnCode, &bitsEncode);
	/* Copy the bits to an array of char that can be decoded */
	return speex_bits_write(&bitsEncode, (char *)OutBytes, 200);
}


void SpeexEncodeFree(void)
{
	if(enc_state != NULL)
	{
    speex_encoder_destroy(enc_state);
    speex_bits_destroy(&bitsEncode);
		enc_state = NULL;
	}	
}





/**
  * @brief  Initializes the speex codec
  * @param  None
  * @retval : None
  */
void SpeexDecode_Init(void)
{
	int enh=1,frame_size;
  /* Speex encoding initializations */ 
  speex_bits_init(&bitsDecode);

  /* speex decoding intilalization */
  dec_state = speex_decoder_init(&speex_nb_mode);
  speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
  
	speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);
}


int SpeexDecodeData(uint8_t *DeCode,uint16_t *OutBytes,uint8_t size,uint8_t isLoss)
{
	int len=0;
	speex_bits_read_from(&bitsDecode, (char *)DeCode, size);
	if(isLoss == 1)
		len = speex_decode_int(dec_state, NULL, (spx_int16_t*)OutBytes);
	else
		len = speex_decode_int(dec_state, &bitsDecode, (spx_int16_t*)OutBytes);
	return len;
}


void SpeexDecodeFree(void)
{
	if(dec_state != NULL)
	{
		speex_decoder_destroy(dec_state);
		speex_bits_destroy(&bitsDecode);
		dec_state = NULL;
	}		
}



/*****************    Preprocessor  *************************/

#include <speex/speex_preprocess.h>

SpeexPreprocessState *st;
void SpeexDenoiseInit(void)
{
	float f;
	int i;
	
	st = speex_preprocess_state_init(160, 8000);
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC, &i);
	
	f = 75*327.68f;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_AGC_LEVEL, &f);
	
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DENOISE, &i);

	i = -30;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &i);
	
	i = 1;
	speex_preprocess_ctl(st, SPEEX_PREPROCESS_SET_DEREVERB, &i);

}


void SpeexDenoiseprocess(spx_int16_t *in)
{
	if(st != NULL)
	{	
		speex_preprocess_run(st, in);
	}
}


void SpeexDenoiseDestrory(void)
{
	if(st != NULL)
	{
		speex_preprocess_state_destroy(st);
		st = NULL;
	}
}


#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

SpeexEchoState *EchoSt;
SpeexPreprocessState *den;
void SpeexEchoInit(void)
{
	int sampleRate = 8000;
	EchoSt = speex_echo_state_init(160, 160);
	den = speex_preprocess_state_init(160, sampleRate);
	speex_echo_ctl(EchoSt, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
	speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
}


void SpeexEchoprocess(short *ref_buf,short *echo_buf,short *e_buf)
{
	speex_echo_cancellation(EchoSt, ref_buf, echo_buf, e_buf);
	speex_preprocess_run(den, e_buf);
}


void SpeexEchoDestroy(void)
{
	speex_echo_state_destroy(EchoSt);
	speex_preprocess_state_destroy(den);
}



/**
  * @brief  Ovveride the _speex_putc function of the speex library
  * @param  None
  * @retval : None
  */
void _speex_putc(int ch, void *file)
{
	__nop();
}


/**
  * @brief  Ovveride the _speex_fatal function of the speex library
  * @param  None
  * @retval : None
  */
 void _speex_fatal(const char *str, const char *file, int line)
{
	__nop();
}

