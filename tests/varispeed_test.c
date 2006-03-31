/*
** Copyright (C) 2006 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <samplerate.h>

#include "util.h"

#define	BUFFER_LEN		(1 << 16)

static void varispeed_test (int converter) ;

int
main (void)
{
/*-	printf ("    Zero Order Hold interpolator    : ") ;
	varispeed_test (SRC_ZERO_ORDER_HOLD) ;

	printf ("    Linear interpolator             : ") ;
	varispeed_test (SRC_LINEAR) ;
-*/
	printf ("    Sinc interpolator               : ") ;
	varispeed_test (SRC_SINC_FASTEST) ;

	return 0 ;
} /* main */

static void
varispeed_test (int converter)
{	static float input [BUFFER_LEN], output [32 * BUFFER_LEN] ;
	double sine_freq ;

	SRC_STATE	*src_state ;
	SRC_DATA	src_data ;

	int input_len, error ;

	memset (input, 0, sizeof (input)) ;

	input_len = ARRAY_LEN (input) / 2 ;

	sine_freq = 0.0111 ;

	if (1)
		gen_windowed_sines (1, &sine_freq, 1.0, input, input_len) ;
	else
	{	int k ;
		for (k = 0 ; k < input_len ; k++)
			input [k] = (1.0 * k) / input_len ;
		} ;

	/* Perform sample rate conversion. */
	if ((src_state = src_new (converter, 1, &error)) == NULL)
	{	printf ("\n\nLine %d : src_new() failed : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ;

	src_data.end_of_input = 1 ;

	src_data.data_in = input ;
	src_data.input_frames = input_len ;

	src_data.src_ratio = 3.0 ;

	src_data.data_out = output ;
	src_data.output_frames = ARRAY_LEN (output) ;

	if ((error = src_set_ratio (src_state, 1.0 / src_data.src_ratio)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ;

	if ((error = src_process (src_state, &src_data)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		printf ("  src_data.input_frames  : %ld\n", src_data.input_frames) ;
		printf ("  src_data.output_frames : %ld\n\n", src_data.output_frames) ;
		exit (1) ;
		} ;

printf ("\n\ninput_frames_used: %ld    output_frames_gen : %ld\n", src_data.input_frames_used, src_data.output_frames_gen) ;

	if (src_data.input_frames_used != input_len)
	{	printf ("\n\nLine %d : unused input.\n", __LINE__) ;
		printf ("\tinput_len         : %d\n", input_len) ;
		printf ("\tinput_frames_used : %ld\n\n", src_data.input_frames_used) ;
		exit (1) ;
		} ;

	/* Copy the last output to the input. */
	memcpy (input, output, sizeof (input)) ;
	reverse_data (input, src_data.output_frames_gen) ;

	if ((error = src_reset (src_state)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ;

	src_data.end_of_input = 1 ;

	src_data.data_in = input ;
	input_len = src_data.input_frames = src_data.output_frames_gen ;

	src_data.data_out = output ;
	src_data.output_frames = ARRAY_LEN (output) ;

	if ((error = src_set_ratio (src_state, 1.0 / src_data.src_ratio)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		exit (1) ;
		} ;

	if ((error = src_process (src_state, &src_data)))
	{	printf ("\n\nLine %d : %s\n\n", __LINE__, src_strerror (error)) ;
		printf ("  src_data.input_frames  : %ld\n", src_data.input_frames) ;
		printf ("  src_data.output_frames : %ld\n\n", src_data.output_frames) ;
		exit (1) ;
		} ;

save_oct_float ("vs.mat", input, src_data.input_frames, output, src_data.output_frames_gen) ;

printf ("\n\ninput_frames_used: %ld    output_frames_gen : %ld\n", src_data.input_frames_used, src_data.output_frames_gen) ;

	if (src_data.input_frames_used != input_len)
	{	printf ("\n\nLine %d : unused input.\n", __LINE__) ;
		printf ("\tinput_len         : %d\n", input_len) ;
		printf ("\tinput_frames_used : %ld\n\n", src_data.input_frames_used) ;
		exit (1) ;
		} ;




	src_state = src_delete (src_state) ;

	puts ("ok") ;

	return ;
} /* varispeed_test */

/*
** Do not edit or modify anything in this comment block.
** The arch-tag line is a file identity tag for the GNU Arch
** revision control system.
**
** arch-tag: 0c9492de-9c59-4690-ba38-f384b547dc74
*/
