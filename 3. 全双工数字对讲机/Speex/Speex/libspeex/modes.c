/* Copyright (C) 2002-2006 Jean-Marc Valin
   File: modes.c

   Describes the different modes of the codec

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "modes.h"
#include "ltp.h"
#include "quant_lsp.h"
#include "cb_search.h"
#include "sb_celp.h"
#include "nb_celp.h"
#include "vbr.h"
#include "arch.h"
#include <math.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef DISABLE_ENCODER
#define nb_encoder_init NULL
#define nb_encoder_destroy NULL
#define nb_encode NULL
#define nb_encoder_ctl NULL

#define split_cb_search_shape_sign NULL
#define noise_codebook_quant NULL
#define pitch_search_3tap NULL
#define forced_pitch_quant NULL
#define lsp_quant_nb NULL
#define lsp_quant_lbr NULL
#endif /* DISABLE_ENCODER */

#ifdef DISABLE_DECODER
#define nb_decoder_init NULL
#define nb_decoder_destroy NULL
#define nb_decode NULL
#define nb_decoder_ctl NULL

#define noise_codebook_unquant NULL
#define split_cb_shape_sign_unquant NULL
#define lsp_unquant_nb NULL
#define lsp_unquant_lbr NULL
#define pitch_unquant_3tap NULL
#define forced_pitch_unquant NULL
#endif /* DISABLE_DECODER */

/* Extern declarations for all codebooks we use here */
extern const signed char gain_cdbk_nb[];
extern const signed char gain_cdbk_lbr[];
extern const signed char exc_5_256_table[];
extern const signed char exc_5_64_table[];
extern const signed char exc_8_128_table[];
extern const signed char exc_10_32_table[];
extern const signed char exc_10_16_table[];
extern const signed char exc_20_32_table[];



/* Parameters for Long-Term Prediction (LTP)*/
static const ltp_params ltp_params_lbr = {
   gain_cdbk_lbr,
   5,
   7
};



/* Split-VQ innovation parameters for low bit-rate narrowband */
static const split_cb_params split_cb_nb_lbr = {
   10,              /*subvect_size*/
   4,               /*nb_subvect*/
   exc_10_32_table, /*shape_cb*/
   5,               /*shape_bits*/
   0,
};



/* 8 kbps low bit-rate mode */
static const SpeexSubmode nb_submode3 = {
   -1,
   0,
   1,
   0,
   /*LSP quantization*/
   lsp_quant_lbr,
   lsp_unquant_lbr,
   /*Pitch quantization*/
   pitch_search_3tap,
   pitch_unquant_3tap,
   &ltp_params_lbr,
   /*Innovation quantization*/
   split_cb_search_shape_sign,
   split_cb_shape_sign_unquant,
   &split_cb_nb_lbr,
   QCONST16(.55,15),
   160
};

#if 0
/* Parameters for Long-Term Prediction (LTP)*/
static const ltp_params ltp_params_med = {
   gain_cdbk_lbr,
   5,
   7
};


/* Split-VQ innovation parameters narrowband */
static const split_cb_params split_cb_nb_med = {
   8,               /*subvect_size*/
   5,               /*nb_subvect*/
   exc_8_128_table, /*shape_cb*/
   7,               /*shape_bits*/
   0,
};

/* 11 kbps medium bit-rate mode */
static const SpeexSubmode nb_submode4 = {
   -1,
   0,
   1,
   0,
   /*LSP quantization*/
   lsp_quant_lbr,
   lsp_unquant_lbr,
   /*Pitch quantization*/
   pitch_search_3tap,
   pitch_unquant_3tap,
   &ltp_params_med,
   /*Innovation quantization*/
   split_cb_search_shape_sign,
   split_cb_shape_sign_unquant,
   &split_cb_nb_med,
   QCONST16(.45,15),
   220
};

#endif

/* Default mode for narrowband */
static const SpeexNBMode nb_mode = {
   NB_FRAME_SIZE,    /*frameSize*/
   NB_SUBFRAME_SIZE, /*subframeSize*/
   NB_ORDER,         /*lpcSize*/
   NB_PITCH_START,               /*pitchStart*/
   NB_PITCH_END,              /*pitchEnd*/
   QCONST16(0.92,15),  /* gamma1 */
   QCONST16(0.6,15),   /* gamma2 */
   QCONST16(.0002,15), /*lpc_floor*/
   {NULL, NULL, NULL, &nb_submode3, NULL, NULL , NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
   5,
   {1, 8, 2, 3, 3, 4, 4, 5, 5, 6, 7}
};


/* Default mode for narrowband */
EXPORT const SpeexMode speex_nb_mode = {
   &nb_mode,
   nb_mode_query,
   "narrowband",
   0,
   4,
   nb_encoder_init,
   nb_encoder_destroy,
   nb_encode,
   nb_decoder_init,
   nb_decoder_destroy,
   nb_decode,
   nb_encoder_ctl,
   nb_decoder_ctl,
};



EXPORT int speex_mode_query(const SpeexMode *mode, int request, void *ptr)
{
   return mode->query(mode->mode, request, ptr);
}

#ifdef FIXED_DEBUG
long long spx_mips=0;
#endif

