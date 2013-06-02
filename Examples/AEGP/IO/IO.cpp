/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2007 Adobe Systems Incorporated                       */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/

#include "IO.h"

/*	Statics and globals are bad. Never use them in production code.  */

static	AEGP_PluginID			S_mem_id					=	0;

static A_Err DeathHook(	
	AEGP_GlobalRefcon unused1 ,
	AEGP_DeathRefcon unused2)
{
	return A_Err_NONE;
}

static A_Err
PretendToReadFileHeader(
	AEIO_BasicData	*basic_dataP,
	IO_FileHeader	*file)
{
	A_Err err = A_Err_NONE;
	
	AEGP_SuiteHandler suites(basic_dataP->pica_basicP);

	/*	Fake file:
		720 x 480, 32 bits per pixel, 29.97 fps, 
		29970 frames.
	*/
	
	file->fpsT.value		=	30000;
	file->fpsT.scale		=	1001;
	file->hasVideo			=	TRUE;
	file->hasAudio			=	FALSE;//TRUE;
	file->heightLu			=	480;
	file->widthLu			=	720;
	file->rowbytesLu		=	(4 * file->widthLu);
	file->numFramesLu		=	29970;
	file->bitdepthS			=	32;
	file->num_channels		=	AEIO_SndChannels_STEREO;
	file->rateF				=	44100.0;
	file->bytes_per_sample	=	4;
	file->encoding			=	AEIO_E_UNSIGNED_PCM;
	file->durationT.value	=	10000;
	file->durationT.scale	=	100;
	file->duration_in_msLu	=	(file->durationT.value / file->durationT.scale) * 1000;

	return err;
}

static A_Err	
My_InitInSpecFromFile(
	AEIO_BasicData		*basic_dataP,
	const A_UTF16Char	*file_pathZ,
	AEIO_InSpecH		specH)
{ 
	/*	Read the file referenced by the path. Use the 
		file information to set all fields of the AEIO_InSpecH.
	*/

	A_Err err						=	A_Err_NONE,
		  err2						=	A_Err_NONE;

	AEIO_Handle		optionsH		=	NULL,
					old_optionsH	=	NULL;
	IO_FileHeader	header;

	A_Fixed			native_fps		=	FLOAT2FIX(29.97);		

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEFX_CLR_STRUCT(header);

	ERR(PretendToReadFileHeader(basic_dataP, &header));
	
	if (!err) {
		IO_FileHeader	*new_headerP 	=	NULL;

		/*		What are we doing here? 

			+	Allocate a new OptionsH to hold our file header info.
			+	Lock it in memory, copy our local header into the OptionsH.
			+	If there's an old OptionsH, get rid of it. 
			+	Unlock handle so AE can move it at will.
		
		*/
		
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id,
														"SDK_IO optionsH", 
														sizeof(IO_FileHeader), 
														AEGP_MemFlag_CLEAR, 
														&optionsH));
													
		if (optionsH){
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void **>(&new_headerP)));
		}	
		if (new_headerP){
			
			*new_headerP = header;

			ERR(suites.IOInSuite4()->AEGP_SetInSpecOptionsHandle(	specH, 
																	optionsH, 
																	reinterpret_cast<void **>(&old_optionsH)));
		
			//	Do NOT free the old options handle. There is old code
			//	in the bowels of AE that does a flat (bytecopy) of the 
			//	input to output OptionsH in the case of a sync.
			/*
			if (old_optionsH){
				ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(old_optionsH));
			}*/
		}

		/*	
			Set specH information based on what we (pretended to) read from the file.
		*/

		ERR(suites.IOInSuite4()->AEGP_SetInSpecSoundRate(specH, new_headerP->rateF));
		ERR(suites.IOInSuite4()->AEGP_SetInSpecSoundEncoding(specH, new_headerP->encoding));
		ERR(suites.IOInSuite4()->AEGP_SetInSpecSoundSampleSize(specH, new_headerP->bytes_per_sample));
		ERR(suites.IOInSuite4()->AEGP_SetInSpecSoundChannels(specH, new_headerP->num_channels));

		ERR(suites.IOInSuite4()->AEGP_SetInSpecDepth(specH, new_headerP->bitdepthS)); // always 32 bits for .fak files
		ERR(suites.IOInSuite4()->AEGP_SetInSpecDuration(specH, &(new_headerP->durationT)));
		ERR(suites.IOInSuite4()->AEGP_SetInSpecDimensions(	specH, 
															static_cast<A_long>(new_headerP->widthLu),
															static_cast<A_long>(new_headerP->heightLu)));

		ERR(suites.IOInSuite4()->AEGP_SetInSpecNativeFPS(specH, native_fps));

		if (!err){
			AEIO_AlphaLabel	alpha;
			AEFX_CLR_STRUCT(alpha);

			alpha.alpha		=	AEIO_Alpha_PREMUL;
			alpha.flags		=	AEIO_AlphaPremul;
			alpha.version	=	AEIO_AlphaLabel_VERSION;
			alpha.red		=	146;
			alpha.green		=	123;
			alpha.blue		=	23;
			
			err = suites.IOInSuite4()->AEGP_SetInSpecAlphaLabel(specH, &alpha);
		}
		if (!err) {
			FIEL_Label	label;
			AEFX_CLR_STRUCT(label);

			label.order		=	FIEL_Order_LOWER_FIRST;
			label.type		=	FIEL_Type_FRAME_RENDERED;
			label.version	=	FIEL_Label_VERSION;
			label.signature	=	FIEL_Tag;

			ERR(suites.IOInSuite4()->AEGP_SetInSpecInterlaceLabel(specH, &label));
		}
		ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	}
	return err;
}

static A_Err
My_InitInSpecInteractive(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH)
{ 
	return A_Err_NONE; 
};

static A_Err
My_DisposeInSpec(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH)
{ 
	A_Err				err			=	A_Err_NONE; 
	AEIO_Handle			optionsH	=	NULL;
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	

	ERR(suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	if (optionsH) {
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}
	return err;
};

static A_Err
My_FlattenOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,
	AEIO_Handle		*flat_optionsPH)
{ 
	A_Err	err		=	A_Err_NONE,
			err2	=	A_Err_NONE; 

	AEIO_Handle			optionsH		= NULL;
	
	IO_FileHeader		*flat_headerP	= NULL,
						*old_headerP	= NULL;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	
	
	//	Given the AEIO_InSpecH, acquire the non-flat
	//	options handle and use it to create a flat
	//	version. Do NOT de-allocate the non-flat
	//	options handle!
	
	ERR(suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));

	if (optionsH) {
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&old_headerP)));
	}
			
	if (old_headerP){
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id,
														"flattened_options", 
														sizeof(IO_FileHeader), 
														AEGP_MemFlag_CLEAR, 
														flat_optionsPH));
	}

	if (*flat_optionsPH){
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(*flat_optionsPH, (void**)&flat_headerP));
	}			
	if (!err && flat_headerP) {
		*flat_headerP = *old_headerP;
	}

	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(*flat_optionsPH));

	return err;
};		

static A_Err
My_InflateOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,
	AEIO_Handle		flat_optionsH)
{ 
	/*	During this function, 
	
		+	Create a non-flat options structure, containing the info from the 
			flat_optionsH you're passed.
		
		+	Set the options for the InSpecH using AEGP_SetInSpecOptionsHandle().
	*/	
	return A_Err_NONE; 
};		

static A_Err
My_SynchInSpec(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Boolean		*changed0)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
My_GetActiveExtent(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,				/* >> */
	const A_Time	*tr,				/* >> */
	A_LRect			*extent)			/* << */
{ 
	return AEIO_Err_USE_DFLT_CALLBACK; 
};		

static A_Err	
My_GetInSpecInfo(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	AEIO_Verbiage	*verbiageP)
{ 
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, "Made-up name");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, "(SDK) Fake File");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->sub_type, "no particular subtype");

	return A_Err_NONE;
};


static A_Err	
My_DrawSparseFrame(
	AEIO_BasicData					*basic_dataP,
	AEIO_InSpecH					specH, 
	const AEIO_DrawSparseFramePB	*sparse_framePPB, 
	PF_EffectWorld					*wP,
	AEIO_DrawingFlags				*draw_flagsP)
{ 
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	PF_Pixel			color 	= 	{255, 123, 223, 0};
	PF_Rect				rectR	=	{0, 0, 0, 0};
	
	//	If the sparse_frame required rect is NOT all zeroes,
	//	use it. Otherwise, just blit the whole thing.
	
	if (!(sparse_framePPB->required_region.top	==
		sparse_framePPB->required_region.left	==
		sparse_framePPB->required_region.bottom ==
		sparse_framePPB->required_region.right)){
		
		rectR.top		= sparse_framePPB->required_region.top;
		rectR.bottom	= sparse_framePPB->required_region.bottom;
		rectR.left		= sparse_framePPB->required_region.left;
		rectR.right		= sparse_framePPB->required_region.right;
	}

	return suites.FillMatteSuite2()->fill(	0, 
											&color, 
											&rectR,
											wP);
};

static A_Err	
My_GetDimensions(
	AEIO_BasicData			*basic_dataP,
	AEIO_InSpecH			 specH, 
	const AEIO_RationalScale *rs0,
	A_long					 *width0, 
	A_long					 *height0)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK; 
};
					
static A_Err	
My_GetDuration(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Time			*tr)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK; 
};

static A_Err	
My_GetTime(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH, 
	A_Time			*tr)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK; 
};

static A_Err	
My_GetSound(
	AEIO_BasicData				*basic_dataP,	
	AEIO_InSpecH				specH,
	AEIO_SndQuality				quality,
	const AEIO_InterruptFuncs	*interrupt_funcsP0,	
	const A_Time				*startPT,	
	const A_Time				*durPT,	
	A_u_long					start_sampLu,
	A_u_long					num_samplesLu,
	void						*dataPV)
{ 
	A_Err			err			=	A_Err_NONE;
	AEIO_Handle		optionsH	=	NULL;

	IO_FileHeader	*headerP	=	NULL;

	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	ERR(suites.IOInSuite4()->AEGP_GetInSpecOptionsHandle(specH, reinterpret_cast<void**>(&optionsH)));
	ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&headerP)));

	if (!err && headerP) {
		A_char report[AEGP_MAX_ABOUT_STRING_SIZE] = {'\0'};
		
		suites.ANSICallbacksSuite1()->sprintf(report, "SDK_IO : %d samples of audio requested.", num_samplesLu); 
			
		ERR(suites.UtilitySuite3()->AEGP_ReportInfo(	basic_dataP->aegp_plug_id, report));
	}

	return err;
};

static A_Err	
My_InqNextFrameTime(
	AEIO_BasicData			*basic_dataP,
	AEIO_InSpecH			specH, 
	const A_Time			*base_time_tr,
	AEIO_TimeDir			time_dir, 
	A_Boolean				*found0,
	A_Time					*key_time_tr0)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK; 
};

static A_Err	
My_DisposeOutputOptions(
	AEIO_BasicData	*basic_dataP,
	void			*optionsPV)
{ 
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	AEIO_Handle			optionsH	=	reinterpret_cast<AEIO_Handle>(optionsPV);
	A_Err				err			=	A_Err_NONE;
	
	if (optionsH){
		ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(optionsH));
	}
	return err;
};

static A_Err	
My_UserOptionsDialog(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	const PF_EffectWorld	*sample0,
	A_Boolean				*user_interacted0)
{ 
	A_Err						err				= A_Err_NONE;
	AEGP_SuiteHandler			suites(basic_dataP->pica_basicP);
	AEIO_Handle					optionsH		= NULL, 
								old_optionsH	= 0;
	IO_FlatFileOutputOptions	*optionsP;

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));
	if (!err){
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&optionsP)));

		basic_dataP->msg_func(0, "Add your user options dialog here by\rimplementing the My_UserOptionsDialog func\rand then saving the results to the output spec.");

		ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));

		ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, optionsH, reinterpret_cast<void**>(&old_optionsH)));
	}

	return err;
};

static A_Err	
My_GetOutputInfo(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH,
	AEIO_Verbiage		*verbiageP)
{ 
	A_Err err			= A_Err_NONE;
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	suites.ANSICallbacksSuite1()->strcpy(verbiageP->name, "filename");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->type, "(SDK) Fake File");
	suites.ANSICallbacksSuite1()->strcpy(verbiageP->sub_type, "no subtypes supported");
	return err;
};

static A_Err	
My_SetOutputFile(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH, 
	const A_UTF16Char	*file_pathZ)
{ 
  	return AEIO_Err_USE_DFLT_CALLBACK;
}

static A_Err	
My_StartAdding(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH, 
	A_long				flags)
{ 
	A_Err		err			=	A_Err_NONE;
	IO_FileHeader	header;
	A_Time		duration	=	{0,1};
	A_Fixed		fps			=	0;
	A_long		widthL 		= 	0,	
				heightL 	= 	0;
	A_FpLong	soundRateF	=	0.0;
	A_char name[AEGP_MAX_PATH_SIZE] = {'\0'};
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);

	AEFX_CLR_STRUCT(header);

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDuration(outH, &duration));
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDimensions(outH, &widthL, &heightL));
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundRate(outH, &soundRateF));

	// If video
	if (!err && name && widthL && heightL) {
		header.hasVideo		=	TRUE;
		header.widthLu		=	(A_u_long)widthL;
		header.heightLu		=	(A_u_long)heightL;

		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFPS(outH, &fps));
		header.fpsT.value	=	duration.value;
		header.fpsT.scale	=	duration.scale;
		header.numFramesLu	=	(A_u_long) fps;

		header.rowbytesLu	=	(A_u_long)(4 * widthL);
	}

	if (!err && soundRateF > 0) {
		header.hasAudio		=	TRUE;
		header.rateF		=	soundRateF;
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundChannels(outH, &header.num_channels));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecSoundSampleSize(outH, &header.bytes_per_sample));

		header.encoding		=	AEIO_E_UNSIGNED_PCM;
	}

	if (!err) {
		/*
			+	Open file
			+	Write header
			+	(keep file open)
		*/
	}
	return err; 
};

static A_Err	
My_AddFrame(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_long					frame_index, 
	A_long					frames,
	const PF_EffectWorld	*wP, 
	const A_LPoint			*origin0,
	A_Boolean				was_compressedB,	
	AEIO_InterruptFuncs		*inter0)
{ 

	A_Err err = A_Err_NONE;
	
	/*	
		+	reinterpret the provided frame into your format.
		+	append it to the already-opened file.
	*/

	return err; 
};
								
static A_Err	
My_EndAdding(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_long					flags)
{ 
	/*
		+	Close file handle, clear out optionsH associated with add.
	*/
	return A_Err_NONE; 
};

static A_Err	
My_OutputFrame(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	const PF_EffectWorld	*wP)
{ 
	A_Err		err	=	A_Err_NONE;

	/*
		+	Re-interpret the supplied PF_World in your own
			format, and save it out to the outH's path.

	*/
	return err;
};

static A_Err	
My_WriteLabels(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	AEIO_LabelFlags	*written)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
My_GetSizes(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	A_u_longlong	*free_space, 
	A_u_longlong	*file_size)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK;
};

static A_Err	
My_Flush(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH)
{ 
	/*	free any temp buffers you kept around for
		writing.
	*/
	return A_Err_NONE; 
};

static A_Err	
My_AddSoundChunk(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	const A_Time	*start, 
	A_u_long		num_samplesLu,
	const void		*dataPV)
{ 
	A_Err err		= A_Err_NONE, err2 = A_Err_NONE;
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);
	
	AEIO_Handle					optionsH = NULL;
	IO_FlatFileOutputOptions	*optionsP = NULL;
	
	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&optionsH)));

	if (!err && optionsH) {
		ERR(suites.MemorySuite1()->AEGP_LockMemHandle(optionsH, reinterpret_cast<void**>(&optionsP)));
		if (!err) {
			A_char report[AEGP_MAX_ABOUT_STRING_SIZE] = {'\0'};
			suites.ANSICallbacksSuite1()->sprintf(report, "SDK_IO : Pretended to write %d samples of audio requested.", num_samplesLu); 
			ERR(suites.UtilitySuite3()->AEGP_ReportInfo(	basic_dataP->aegp_plug_id, report));
		}
	}
	ERR2(suites.MemorySuite1()->AEGP_UnlockMemHandle(optionsH));
	return err; 
};


static A_Err	
My_Idle(
	AEIO_BasicData			*basic_dataP,
	AEIO_ModuleSignature	sig,
	AEIO_IdleFlags			*idle_flags0)
{ 
	return A_Err_NONE; 
};	


static A_Err	
My_GetDepths(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	AEIO_SupportedDepthFlags		*which)
{ 
	/*	Enumerate possible output depths by ORing 
		together different AEIO_SupportedDepthFlags.
	*/
	
	*which =	AEIO_SupportedDepthFlags_DEPTH_8	|
				AEIO_SupportedDepthFlags_DEPTH_32;

	return A_Err_NONE; 
};

static A_Err	
My_GetOutputSuffix(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	A_char			*suffix)
{ 
	return AEIO_Err_USE_DFLT_CALLBACK;
};


static A_Err	
My_SeqOptionsDlg(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH	specH,  
	A_Boolean		*user_interactedPB0)
{ 
	basic_dataP->msg_func(0, "IO: Here's my sequence options dialog!");
	return A_Err_NONE; 
};

static A_Err	
My_NumAuxFiles(		
	AEIO_BasicData		*basic_dataP,
	AEIO_InSpecH		seqH,
	A_long				*files_per_framePL0)
{ 
	*files_per_framePL0 = 0;
	return A_Err_NONE; 
};

static A_Err	
My_CloseSourceFiles(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			seqH)
{ 
	return A_Err_NONE; 
};		// TRUE for close, FALSE for unclose

static A_Err	
My_CountUserData(
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			inH,
	A_u_long 				typeLu,
	A_u_long				max_sizeLu,
	A_u_long				*num_of_typePLu)
{ 
	return A_Err_NONE; 
};

static A_Err	
My_SetUserData(    
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH			outH,
	A_u_long				typeLu,
	A_u_long				indexLu,
	const AEIO_Handle		dataH)
{ 
	return A_Err_NONE; 
};

static A_Err	
My_GetUserData(   
	AEIO_BasicData	*basic_dataP,
	AEIO_InSpecH			inH,
	A_u_long 				typeLu,
	A_u_long				indexLu,
	A_u_long				max_sizeLu,
	AEIO_Handle				*dataPH)
{ 
	return A_Err_NONE; 
};
                            
static A_Err	
My_AddMarker(		
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH 			outH, 
	A_long 					frame_index, 
	AEIO_MarkerType 		marker_type, 
	void					*marker_dataPV, 
	AEIO_InterruptFuncs		*inter0)
{ 
	/*	The type of the marker is in marker_type,
		and the text they contain is in 
		marker_dataPV.
	*/
	return A_Err_NONE; 
};

static A_Err	
My_VerifyFileImportable(
	AEIO_BasicData			*basic_dataP,
	AEIO_ModuleSignature	sig, 
	const A_UTF16Char		*file_pathZ, 
	A_Boolean				*importablePB)
{ 
	//	This function is an appropriate place to do
	//	in-depth evaluation of whether or not a given
	//	file will be importable; AE already does basic
	//	extension checking. Keep in mind that this 
	//	function should be fairly speedy, to keep from
	//	bogging down the user while selecting files.
	
	//	-bbb 3/31/04

	*importablePB = TRUE;
	return A_Err_NONE; 
};		
	
static A_Err	
My_InitOutputSpec(
	AEIO_BasicData			*basic_dataP,
	AEIO_OutSpecH			outH, 
	A_Boolean				*user_interacted)
{
	A_Err						err				= A_Err_NONE;
	AEIO_Handle					new_optionsH	= NULL, 
								old_optionsH	= 0;
	IO_FlatFileOutputOptions	*new_optionsP,
								*old_optionsP;
	AEGP_SuiteHandler			suites(basic_dataP->pica_basicP);

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));

	if (!err) {
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id, 
														"InitOutputSpec options", 
														sizeof(IO_FlatFileOutputOptions), 
														AEGP_MemFlag_CLEAR, 
														&new_optionsH));
		if (!err && new_optionsH) {
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(new_optionsH, reinterpret_cast<void**>(&new_optionsP)));

			if (!err && new_optionsP) {

				if (!old_optionsH) {
					ERR(PretendToReadFileHeader(basic_dataP, reinterpret_cast<IO_FileHeader *>(&(new_optionsP->name))));
					new_optionsP->isFlat = FALSE;

				} else {
					ERR(suites.MemorySuite1()->AEGP_LockMemHandle(old_optionsH, reinterpret_cast<void**>(&old_optionsP)));

					if (!err && new_optionsP && old_optionsP) {
						memcpy(new_optionsP, old_optionsP, sizeof(IO_FlatFileOutputOptions));
						new_optionsP->isFlat = FALSE;

						// If you modify the output options, then set this to true to tell AE that the project has changed
						if (rand() % 3){
							*user_interacted = TRUE;
						} else {
							*user_interacted = FALSE;
						}

						ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(old_optionsH));
					}
				}
				ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(new_optionsH));

				ERR(suites.IOOutSuite4()->AEGP_SetOutSpecOptionsHandle(outH, new_optionsH, reinterpret_cast<void**>(&old_optionsH)));
			}
		}
	}
	return err;
}


static A_Err	
My_OutputInfoChanged(
	AEIO_BasicData		*basic_dataP,
	AEIO_OutSpecH		outH)
{
	/*	This function is called when either the user 
		or the plug-in has changed the output options info.
		You may want to update your plug-in's internal
		representation of the output at this time. 
		We've exercised the likely getters below.
	*/
	
	A_Err err					=	A_Err_NONE;
	
	AEIO_AlphaLabel	alpha;
	AEFX_CLR_STRUCT(alpha);
	
	FIEL_Label		fields;
	AEFX_CLR_STRUCT(fields);

	A_short			depthS		=	0;
	A_Time			durationT	=	{0,1};

	A_Fixed			native_fps	=	0;
	A_Ratio			hsf			=	{1,1};
	A_Boolean		is_missingB	=	TRUE;
	
	AEGP_SuiteHandler	suites(basic_dataP->pica_basicP);	

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecIsMissing(outH, &is_missingB));
	
	if (!is_missingB)
	{
		// Find out all the details of the output spec; update
		// your options data as necessary.
		
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecAlphaLabel(outH, &alpha));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDepth(outH, &depthS));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecInterlaceLabel(outH, &fields));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecDuration(outH, &durationT));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecFPS(outH, &native_fps));
		ERR(suites.IOOutSuite4()->AEGP_GetOutSpecHSF(outH, &hsf));
	}
	return err;
}



static A_Err	
My_GetFlatOutputOptions(
	AEIO_BasicData	*basic_dataP,
	AEIO_OutSpecH	outH, 
	AEIO_Handle		*new_optionsPH)
{
	A_Err						err				= A_Err_NONE;
	AEIO_Handle					old_optionsH	= NULL;
	IO_FlatFileOutputOptions	*new_optionsP,
								*old_optionsP;
	AEGP_SuiteHandler			suites(basic_dataP->pica_basicP);

	ERR(suites.IOOutSuite4()->AEGP_GetOutSpecOptionsHandle(outH, reinterpret_cast<void**>(&old_optionsH)));

	if (!err && old_optionsH) {
		ERR(suites.MemorySuite1()->AEGP_NewMemHandle(	S_mem_id, 
														"flat optionsH", 
														sizeof(IO_FlatFileOutputOptions), 
														AEGP_MemFlag_CLEAR, 
														new_optionsPH));
		if (!err && *new_optionsPH) {
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(*new_optionsPH, reinterpret_cast<void**>(&new_optionsP)));
			ERR(suites.MemorySuite1()->AEGP_LockMemHandle(old_optionsH, reinterpret_cast<void**>(&old_optionsP)));

			if (!err && new_optionsP && old_optionsP) {
				// Convert the old unflat structure into a separate flat structure for output
				// In this case, we just do a simple copy and set the isFlat flag
				memcpy(new_optionsP, old_optionsP, sizeof(IO_FlatFileOutputOptions));
				new_optionsP->isFlat = TRUE;

				ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(*new_optionsPH));
				ERR(suites.MemorySuite1()->AEGP_UnlockMemHandle(old_optionsH));
			}
		}
	}
	return err;
}

static A_Err
ConstructModuleInfo(
	SPBasicSuite		*pica_basicP,			
	AEIO_ModuleInfo		*info)
{
	A_Err err = A_Err_NONE;

	AEGP_SuiteHandler	suites(pica_basicP);
	
	if (info) {
		info->sig						=	'FAK_';
		info->max_width					=	1920;
		info->max_height				=	1080;
		info->num_filetypes				=	1;
		info->num_extensions			=	1;
		info->num_clips					=	0;
		
		info->create_kind.type			=	'FAK_';
		info->create_kind.creator		=	'DTEK';

		info->create_ext.pad			=	'.';
		info->create_ext.extension[0]	=	'f';
		info->create_ext.extension[1]	=	'a';
		info->create_ext.extension[2]	=	'k';

		suites.ANSICallbacksSuite1()->strcpy(info->name, "Fake Files (SKD)");
		
		info->num_aux_extensionsS		=	0;

		info->flags						=	AEIO_MFlag_INPUT			| 
											AEIO_MFlag_OUTPUT			| 
											AEIO_MFlag_FILE				|
											AEIO_MFlag_VIDEO			| 
											AEIO_MFlag_AUDIO			|
											AEIO_MFlag_NO_TIME			| 
											AEIO_MFlag_CAN_DO_MARKERS	|
											AEIO_MFlag_HAS_AUX_DATA;

		info->read_kinds[0].mac.type			=	'FAK_';
		info->read_kinds[0].mac.creator			=	AEIO_ANY_CREATOR;
		info->read_kinds[1].ext					=	info->create_ext;
		info->read_kinds[2].ext.pad				=	'.';
		info->read_kinds[2].ext.extension[0]	=	'f';
		info->read_kinds[2].ext.extension[1]	=	'a';
		info->read_kinds[2].ext.extension[2]	=	'k';
		info->read_kinds[2].ext.extension[3]	=	'\0';
	} else {
		err = A_Err_STRUCT;
	}
	return err;
}

A_Err
ConstructFunctionBlock(
	AEIO_FunctionBlock4	*funcs)
{
	if (funcs) {
		funcs->AEIO_AddFrame				=	My_AddFrame;
		funcs->AEIO_AddMarker				=	My_AddMarker;
		funcs->AEIO_AddSoundChunk			=	My_AddSoundChunk;
		funcs->AEIO_CloseSourceFiles		=	My_CloseSourceFiles;
		funcs->AEIO_CountUserData			=	My_CountUserData;
		funcs->AEIO_DisposeInSpec			=	My_DisposeInSpec;
		funcs->AEIO_DisposeOutputOptions	=	My_DisposeOutputOptions;
		funcs->AEIO_DrawSparseFrame			=	My_DrawSparseFrame;
		funcs->AEIO_EndAdding				=	My_EndAdding;
		funcs->AEIO_FlattenOptions			=	My_FlattenOptions;
		funcs->AEIO_Flush					=	My_Flush;
		funcs->AEIO_GetActiveExtent			=	My_GetActiveExtent;
		funcs->AEIO_GetDepths				=	My_GetDepths;
		funcs->AEIO_GetDimensions			=	My_GetDimensions;
		funcs->AEIO_GetDuration				=	My_GetDuration;
		funcs->AEIO_GetInSpecInfo			=	My_GetInSpecInfo;
		funcs->AEIO_GetOutputInfo			=	My_GetOutputInfo;
		funcs->AEIO_GetOutputSuffix			=	My_GetOutputSuffix;
		funcs->AEIO_GetSizes				=	My_GetSizes;
		funcs->AEIO_GetSound				=	My_GetSound;
		funcs->AEIO_GetTime					=	My_GetTime;
		funcs->AEIO_GetUserData				=	My_GetUserData;
		funcs->AEIO_Idle					=	My_Idle;
		funcs->AEIO_InflateOptions			=	My_InflateOptions;
		funcs->AEIO_InitInSpecFromFile		=	My_InitInSpecFromFile;
		funcs->AEIO_InitInSpecInteractive	=	My_InitInSpecInteractive;
		funcs->AEIO_InqNextFrameTime		=	My_InqNextFrameTime;
		funcs->AEIO_NumAuxFiles				=	My_NumAuxFiles;
		funcs->AEIO_OutputFrame				=	My_OutputFrame;
		funcs->AEIO_SeqOptionsDlg			=	My_SeqOptionsDlg;
		funcs->AEIO_SetOutputFile			=	My_SetOutputFile;
		funcs->AEIO_SetUserData				=	My_SetUserData;
		funcs->AEIO_StartAdding				=	My_StartAdding;
		funcs->AEIO_SynchInSpec				=	My_SynchInSpec;
		funcs->AEIO_UserOptionsDialog		=	My_UserOptionsDialog;
		funcs->AEIO_VerifyFileImportable	=	My_VerifyFileImportable;
		funcs->AEIO_WriteLabels				=	My_WriteLabels;
		funcs->AEIO_InitOutputSpec			=	My_InitOutputSpec;
		funcs->AEIO_GetFlatOutputOptions	=	My_GetFlatOutputOptions;
		funcs->AEIO_OutputInfoChanged		=	My_OutputInfoChanged;

		return A_Err_NONE;
	} else {
		return A_Err_STRUCT;
	}
}

static SPBasicSuite * m_sp;
static AEGP_Command m_testCommand;

static A_Err UpdateMenuHook(AEGP_GlobalRefcon plugin_refconPV, AEGP_UpdateMenuRefcon refconPV, AEGP_WindowType active_window)
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_sp);
	
	if (m_testCommand != 0)
	{
		ERR(suites.CommandSuite1()->AEGP_EnableCommand(m_testCommand));
	}
	
	return err;
}

A_Err
EntryPointFunc(
	struct SPBasicSuite		*pica_basicP,			/* >> */
	A_long				 	major_versionL,			/* >> */		
	A_long					minor_versionL,			/* >> */		
	AEGP_PluginID			aegp_plugin_id,			/* >> */
	AEGP_GlobalRefcon		*global_refconP)		/* << */
{
	A_Err 				err					= A_Err_NONE;
	AEIO_ModuleInfo		info;
	AEIO_FunctionBlock4	funcs;
	AEGP_SuiteHandler	suites(pica_basicP);	
	m_sp = pica_basicP;
	
	AEFX_CLR_STRUCT(info);
	AEFX_CLR_STRUCT(funcs);
	
	ERR(suites.RegisterSuite5()->AEGP_RegisterDeathHook(aegp_plugin_id, DeathHook, 0));
	ERR(ConstructModuleInfo(pica_basicP, &info));
	ERR(ConstructFunctionBlock(&funcs));

	ERR(suites.RegisterSuite5()->AEGP_RegisterIO(	aegp_plugin_id,
													0,
													&info, 
													&funcs));

	ERR(suites.UtilitySuite3()->AEGP_RegisterWithAEGP(	NULL,
														"SDK_IO",
														&S_mem_id));
	
	// 添加菜单项
	AEGP_Command testCommand;
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&testCommand));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(testCommand, "TestCommand", AEGP_Menu_EDIT, AEGP_MENU_INSERT_SORTED));
	ERR(suites.CommandSuite1()->AEGP_EnableCommand(testCommand));
	ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(aegp_plugin_id, UpdateMenuHook, NULL));
	
	m_testCommand = testCommand;
	
	return err;
}
