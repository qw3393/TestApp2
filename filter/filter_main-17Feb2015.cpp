/*=============================================================================

  Application UI MODULE

  FILE: UI_evt.cpp
  
  DESCRIPTION

    result data filtering 

  EXTERNALIZED FUNCTIONS


   Copyright (c) OptoDynamics Inc. All rights reserved.

=============================================================================*/
/*=============================================================================

  EDIT HISTORY

  ---------  -------  -------------------------------------------------------
    when       who       what, where, why
  ---------  -------  -------------------------------------------------------
   03/14/04   nso4th    Initial version
   15/09/02   Vijay		Added data filter code and test routines
						1) glass ng filter
						2) median and std dev filtering
						3) Moving average filter
						4) Consecutive values limit filtering
						5) added ng filer at 2 stages
						6) ini file configuration for above

=============================================================================*/


#include <Cust_AppDef.h>
#include "ntg_mod.h"

#include "filter_main.h"
#define MAX_MED_FILT_VAL 100
#define INTERP_DATA_INTERVAL 5
/**************************************************************

	linear_interp_enc :

	Linear interpolation of encoder value for interval of 1 mm

	Parameters
	mat_in	-> Input matrix [X encoder values] [Y thickness data]
	mat_in  -> result matrix [X encoder values] [Y thickness data]

	Note: Correct size calulation of out matrix is essential as it may be
			 smaller or larger in [Y]

**************************************************************/
bool linear_interp_enc(MATRIX mat_in,MATRIX& mat_out)
{
	if((NULL == mat_in)  || 
	   (NULL == mat_out) ||
	   (mat_in->iX != mat_out->iX)||
	   (mat_in->iY > mat_out->iY)||
	   (mat_in->iX<2))
	{
		return false;
	}
	//
	int min_x = (int)MAT(mat_in, 0, 0);
	int max_x = (int)MAT(mat_in, 0, mat_in->iY-1);
	int j=0;			// index for finding 
	int num=0;			// result array index
	double x1,x2,y1,y2;	// parameters for interpolation
	x1=x2=y1=y2=0.0;
	//j  =1;
	for(int index =min_x;index <max_x-1;index++)
	{
		bool bfound = false;
		//j =1;
		while(!bfound)
		{
			if(MAT(mat_in, 0, j) >(double) index)
			{
				x1 = MAT(mat_in, 0, j-1);
				x2 = MAT(mat_in, 0, j);
				y1 = MAT(mat_in, 1, j-1);
				y2 = MAT(mat_in, 1, j);
				// compute and add the data to output array
				//break;
				bfound= true;
			}
			else
			{
				j++;
			}
		}
		if(0 ==(index%INTERP_DATA_INTERVAL))
		{
			MAT(mat_out, 0, num) = (double)index;
			MAT(mat_out, 1, num) = y1 + (((double)(index))-x1)*((y2-y1)/(x2-x1));
			num++;
		}

	}
	// resize matrix
	MATRIX temp_matrix = Mat_GetChild(mat_out,0,1,0,num-1);
	MAT_FREE(mat_out);
	mat_out= temp_matrix;
	return true;
}

/**************************************************************

	aver		:

	simple averaging routine

	Parameters
	val_array	-> Input array of double values
	size		-> number of elements in input array
	out_val		-> output average value

**************************************************************/

bool aver(double *val_array,int size,double *out_val)
{
	if((NULL == val_array) || 
	   (NULL == out_val) || 
	   (1> size))
	{
		return false;
	}
	double Val =0.0;
	for(int index =0;index <size;index++)
	{
		Val += val_array[index];
	}
	*out_val = Val/size;
	return true;
};


/**************************************************************

	median		:

	finding median of the data set. The data set is not sorted

	Parameters
	val_array	-> Input array of double values
	mask_size	-> number of elements in input array
	median_val	-> output median value

	Note: Input array must have atleast mask_size elements

**************************************************************/

bool median(double *val_array,int mask_size,double *median_val)
{
	if((NULL == val_array) || 
	   (NULL == median_val) || 
	   (MAX_MED_FILT_VAL <mask_size) || 
	   (1> mask_size))
	{
		return false;
	}
	double temp_array[MAX_MED_FILT_VAL]={0},
		   temp_var	;

	for(int Index =0;Index <mask_size;Index++)
	{
		temp_array[Index] = val_array[Index];
	}
	for(int i =0;i <mask_size;i++ )
	{
		for(int j =0;j<mask_size-1;j++)
		{
			if(temp_array[j] > temp_array[j+1])
			{
				temp_var = temp_array[j];
				temp_array[j] = temp_array[j+1];
				temp_array[j+1] = temp_var;
			}
		}
	}
	if(1 == mask_size%2)
	{
		*median_val = temp_array[((mask_size+1)/2) -1];
	}
	else
	{
		*median_val = (temp_array[mask_size/2] +val_array[(mask_size/2) -1]) /2;
	}
	return true;
};

/**************************************************************

	median_filt		:

	apply median filter to second column of supplied matrix [mat_in]
	The median values are returned by mat_out

	Parameters
	mat_in		-> Input array of double values
	mat_out		-> output result of median values
	mask_size	-> mask size of median filter

	Note: Output matrix will be shorter by mask_size

**************************************************************/

bool median_filt(MATRIX mat_in,MATRIX mat_out,int mask_size)
{
	if((NULL == mat_in)  || 
	   (NULL == mat_out) ||
	   (mat_in->iX != mat_out->iX)||
	   (mat_in->iY > (mat_out->iY+mask_size))||
	   (mat_in->iX<2)||
	   (0 == mask_size%2))
	{
		LOGINFO("Median filter invalid parameter");
		return false;
	}
	// Get second col
	VECTOR Vec = Mat_GetColumn(mat_in,1);
	int offset_data = (mask_size-1)/2;
	double Val =0.0;
	for(int index = 0;index <(Vec->iNum -mask_size);index++)
	{
		Val =0.0;
		if(false == median(Vec->pData+index,mask_size,&Val))
		{
			VEC_FREE(Vec);
			LOGINFO("Median filter failed [software error]");
			return false;
		}
		mat_out->pData[index][1] = Val;
		mat_out->pData[index][0] = mat_in->pData[index+offset_data][0];
	}
	VEC_FREE(Vec);
	return true;
};


/**************************************************************

	moving_ave		:

	moving average filter for second column of supplied matrix
	The average values are returned by mat_out

	Parameters
	mat_in		-> Input array of double values
	mat_out		-> output result of median values
	ave_size	-> size for averaging filter filter

	Note: Output matrix will be shorter by ave_size

**************************************************************/
bool moving_ave(MATRIX mat_in,MATRIX mat_out, int ave_size)
{
	if((NULL == mat_in)  || 
	   (NULL == mat_out) ||
	   (mat_in->iX != mat_out->iX)||
	   (mat_in->iY >( mat_out->iY+ave_size))||
	   (mat_in->iX<2))
	{
		LOGINFO("moving ave filter invalid parameter");
		return false;
	}
	//
	// Get second col
	VECTOR Vec = Mat_GetColumn(mat_in,1);
	int offset_data = (ave_size-1)/2;
	double Val =0.0;
	for(int index = 0/*offset_data*/;index <(mat_in->iY -ave_size);index++)
	{
		if(false ==aver(Vec->pData+index,ave_size,&Val))
		{
			VEC_FREE(Vec);
			LOGINFO("Moving average failed [software error]");
			return false;
		}
		mat_out->pData[index+offset_data][1] = Val;
		mat_out->pData[index+offset_data][0] = mat_in->pData[index+offset_data][0];
	}
	// use original values for missing data
	for(index =0;index<(offset_data);index++)
	{
		MAT(mat_out,0,index) =MAT(mat_in,0,index);
		MAT(mat_out,1,index) =MAT(mat_in,1,index);
	}
	// use original values for missing data
	for(index =mat_in->iY-(offset_data+1);index<mat_in->iY;index++)
	{
		MAT(mat_out,0,index) =MAT(mat_in,0,index);
		MAT(mat_out,1,index) =MAT(mat_in,1,index);
	}
	VEC_FREE(Vec);
	return true;
}


bool read_ng_param(char * Filename,double64& Limit,double64& Lower,double64& Upper)
{
	char Section[] ="NGParam";
	char str[MAX_PATH] = {0};

	GetPrivateProfileString(Section,"Limit","100",str,MAX_PATH,Filename);
	Limit = atof(str);
 	GetPrivateProfileString(Section,"Lower","1",str,MAX_PATH,Filename);
	Lower = atof(str);
	GetPrivateProfileString(Section,"Upper","2999",str,MAX_PATH,Filename);
	Upper = atof(str);
	return true;
}

bool read_ng_param_second(char * Filename,double64& Limit,double64& Lower,double64& Upper)
{
	char Section[] ="NGParam";
	char str[MAX_PATH] = {0};

	GetPrivateProfileString(Section,"Limit2","100",str,MAX_PATH,Filename);
	Limit = atof(str);
 	GetPrivateProfileString(Section,"Lower","1",str,MAX_PATH,Filename);
	Lower = atof(str);
	GetPrivateProfileString(Section,"Upper","2999",str,MAX_PATH,Filename);
	Upper = atof(str);
	return true;
}

bool read_first_filt_param(char *Filename,double64& glass_thk, double64& lower_range, double64& upper_range,double64& thk_diff)
{
	char Section[] ="FirstFilt";
	char str[MAX_PATH] = {0};

	GetPrivateProfileString(Section,"GlassThk","700",str,MAX_PATH,Filename);
	glass_thk = atof(str);

 	GetPrivateProfileString(Section,"UpperRange","100",str,MAX_PATH,Filename);
	upper_range = atof(str);

	GetPrivateProfileString(Section,"LoweRange","100",str,MAX_PATH,Filename);
	lower_range = atof(str);

	GetPrivateProfileString(Section,"ThkDiff","10",str,MAX_PATH,Filename);
	thk_diff = atof(str);

	return true;
}

bool read_med_stdev_filt_param(char *Filename,int& mask_size, double64& std_scale_low/*,double64& std_scale_high*/)
{
	char Section[] ="MedFilt";
	char str[MAX_PATH] = {0};

	GetPrivateProfileString(Section,"MaskSize","21",str,MAX_PATH,Filename);
	mask_size = atoi(str);

 	GetPrivateProfileString(Section,"StdDevScaleLow","3.0",str,MAX_PATH,Filename);
	std_scale_low = atof(str);

// 	GetPrivateProfileString(Section,"StdDevScalehigh","3.0",str,MAX_PATH,Filename);
//	std_scale_high = atof(str);

	return true;

}

bool read_mov_avefilt_param(char *Filename,int& window_size)
{
	char Section[] ="MovAverage";
	char str[MAX_PATH] = {0};

	GetPrivateProfileString(Section,"WindowSize","51",str,MAX_PATH,Filename);
	window_size = atoi(str);

	return true;
}

bool mov_aver_filt(MATRIX raw_data, MATRIX& final_data)
{
	int window_size=0;
	char cfg_file[] = "D:/data/CFG_filter.txt";
	read_mov_avefilt_param(cfg_file,window_size);
	moving_ave(raw_data,final_data,window_size);
	// read parameters from file
	// Apply filter
#if 0
	if(FALSE ==Mat_SaveASCII(final_data,"d:/data/mov_ave_filt.txt"))
	{
		//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
	}
#endif
	return true;
};

bool lin_interpol(MATRIX raw_data, MATRIX& final_data)
{
	// read parameters from file
	// Apply filter
	linear_interp_enc(raw_data,final_data);
#if 0
	if(FALSE ==Mat_SaveASCII(final_data,"d:/data/interp_filt.txt"))
	{
		//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
	}
#endif
	return true;
};

bool med_std_filt(MATRIX raw_data, MATRIX filtered_data,int &num_data)
{
	bool bRes = false;
//	MATRIX	raw_data,filtered_data;
	boolean mat_exp = false;
	int32     mat_digit =5;
	Mat_GetSaveDigit(&mat_exp,&mat_digit);
	Mat_SetSaveDigit(FALSE,5);
	char cfg_file[] = "D:/data/CFG_filter.txt";
	int mask_size =0;

	double64 std_scale  =0.0;

	read_med_stdev_filt_param(cfg_file,mask_size,std_scale/*,std_scale_high*/);

	if((0 ==raw_data->iY)||
	   (NULL == raw_data))
	{
		//ui_cmd_mem("Load file failed med stdev filt",DEN_TXT_MEM0);
		LOGINFO("[med_std_filt] Invalid raw data for median filter");
		return false;
	}
	else
	{
		// copy thickness data to vector
		VECTOR thk_data = Mat_GetColumn(raw_data,1);
		// run median filter code
		MATRIX int_data = MAT_ALLOC(raw_data->iX,raw_data->iY);
		double64 Val =0.0;
		int real_index =0;
		// do median filter and data subtraction in one shot
		for(int index = 0/*offset_data*/;index <(thk_data->iNum -mask_size);index++)
		{
			Val =0.0;
			if(false == median(thk_data->pData+index,mask_size,&Val))
			{
				VEC_FREE(thk_data);
				// 
				LOGINFO("[med_std_filt] Median filter failed");
				return false;
			}
			real_index =index+(mask_size-1)/2;
			//mat_out->pData[filt_index][1] = Val;
			MAT(int_data,1,real_index) = MAT(raw_data,1,real_index)-Val;
			//mat_out->pData[indfilt_indexex][0] = mat_in->pData[index+offset_data][0];
			MAT(int_data,0,real_index)= MAT(raw_data,0,real_index);
		}
		// Get the vector of filtered data
		VECTOR med_filt_thk = Mat_GetColumn(int_data,1);
		VECTOR med_std_filt = NULL;
		//Vec_SaveASCII(med_filt_thk,"before_std.txt");
		// Get the std dev
		double64 std = Vec_STDSc(med_filt_thk);
		double64 limit =std*std_scale;
		//double64 higher_limit =std*std_scale_high;
		double64 cur_val =0.0;
		VECTOR temp_data = VEC_ALLOC(med_filt_thk->iNum);
		int filt_index =0;
		for(index =0;index<med_filt_thk->iNum;index++)
		{
			cur_val = fabs(VEC(med_filt_thk,index));
			if(cur_val <= limit)
			{
				VEC(temp_data,index)= cur_val;
			}
			else
			{
				VEC(temp_data,index)= 9999;
			}
		}
		//Vec_SaveASCII(temp_data,"after_std.txt");
		// compare and copy to filtered data
		if(filtered_data->iY < int_data->iY)
		{
			LOGINFO("[med_std_filt] insufficient result matrix [software error]");
			return false;
		}
		int full_filt_index =0;
		for (index =0;index <int_data->iY;index++)
		{

			if(VEC(temp_data,index) != 9999)
			{
				MAT(filtered_data,0,full_filt_index)= MAT(raw_data,0,index);;
				MAT(filtered_data,1,full_filt_index)= MAT(raw_data,1,index);
				full_filt_index++;
			}
		}
		if(0 ==full_filt_index)
		{
			LOGINFO("[med_std_filt] filtering resulted empty data set !");
			return false;
		}
		else
		{
			num_data = full_filt_index-1;
		}
#if 0
		// save to file
		if(FALSE ==Mat_SaveASCII(filtered_data,"median_std.txt"))
		{
			ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
		}
#endif
	}
	// cleanup
	return bRes;
};

bool thk_filt(MATRIX raw_data, MATRIX filtered_data,int &num_data)
{
	bool bRes = false;
	boolean mat_exp = false;
	int32     mat_digit =5;
	Mat_GetSaveDigit(&mat_exp,&mat_digit);
	Mat_SetSaveDigit(FALSE,5);
	double64 glass_thk =0.0,
		    upper_range =0.0,
			lower_range =0.0,
			thk_diff    = 0.0;

	char cfg_file[] = "D:/data/CFG_filter.txt";
	// load file matrix
	read_first_filt_param(cfg_file,glass_thk,lower_range,upper_range,thk_diff);

	if((0	== raw_data->iY)||
	  (NULL == raw_data))
	{
		LOGINFO("Invalid rw_data for first filter");
		return false;
	}
	else
	{
//		char message[MAX_PATH]={0};
		int filt_count =0;
		double64 thk_diff_cal=0.0;
		double64 thk =0.0;
		//Mat_SaveASCII(raw_data,"raw-data1.txt");
//		sprintf(message," raw Data %d filtered %d  %.2f %.2f %.2f",raw_data->iY,filtered_data->iY,glass_thk,lower_range,upper_range);
//		LOGINFO(message);


		//Mat_SaveASCII(filtered_data,"raw-data1.txt");
		for(int index =0;index<raw_data->iY-2;index++)
		{
			thk_diff_cal = fabs(MAT(raw_data,1,index)- MAT(raw_data,1,index+1));
			thk = MAT(raw_data,1,index);
//			sprintf(message," %d %.2f ->%.2f",index,thk_diff_cal,thk);
//			LOGINFO(message);
//			Sleep(1);
			if((RANGE_IN(MAT(raw_data,1,index),glass_thk-lower_range,glass_thk+upper_range))&&
				(thk_diff_cal <thk_diff))
			{
				// copy to output
				MAT(filtered_data,0,filt_count) = MAT(raw_data,0,index);
				MAT(filtered_data,1,filt_count) = MAT(raw_data,1,index);
				//sprintf(message,"Count %d",filt_count);
				//LOGINFO(message);
				filt_count++;
			}
		}
		// save to file
#if 0
		// @@TBD: Remove the save function after validation
		if(FALSE ==Mat_SaveASCII(filtered_data,"min_max_filt.txt"))
		{
			//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
			LOGINFO("[min max]Mareix save file failed");
		}
#endif
		if(0 == filt_count)
		{
			LOGINFO("[thk_filt] filtering resulted empty data set !");
		}
		else
		{
			LOGINFO("[thk_filt] OK");
			num_data = filt_count-1;
		}
	}
	// cleanup
	return true;
};

bool ng_test(MATRIX raw_data/*, MATRIX final_data*/,bool& valid_glass,bool second_run)
{
	char str[MAX_PATH] = {0};
	// Parameters
	char cfg_file[] = "D:/data/CFG_filter.txt";
	double64 limit_val =0.0,			// check validity of encoder difference
		     lower_val =0.0,			// Glass inlet limit
			 upper_val =0.0;			// Glass outlet limit

	if(second_run)
	{
		read_ng_param_second(cfg_file,limit_val,lower_val,upper_val);
	}
	else
	{
		read_ng_param(cfg_file,limit_val,lower_val,upper_val);
	}


	if((NULL == raw_data)		||
	   (0	 == raw_data->iY) )
	{
		sprintf(str,"[ng_test-%d] Invalid input or no input",(second_run)?(2):(1));
		LOGINFO(str);
		return false;
	}

	// parameter validation
	if(!RANGE_IN(limit_val,2.0,30.0))
	{
		sprintf(str,"[ng_test-%d] invalid %s parameter [range 2.0-30.0] [%.2f]",
														(second_run)?(2):(1),
														(second_run)?("[Limit2]"):("[Limit]"),
														limit_val);
		LOGINFO(str);
		return false;
	}

	if(!RANGE_IN(lower_val,0.0,3000.0))
	{
		sprintf(str,"[ng_test-%d] invalid [Lower] parameter [0.0-3000.0] [%.2f]",
														(second_run)?(2):(1),
														lower_val);
		LOGINFO(str);
		return false;
	}

	if(!RANGE_IN(upper_val,0.0,3000.0))
	{
		sprintf(str,"[ng_test-%d] invalid [Upper] parameter [0.0-3000.0] [%.2f]",
														(second_run)?(2):(1),
														upper_val);
		LOGINFO(str);
		return false;
	}

	if(upper_val <= lower_val)
	{
		sprintf(str,"[ng_test-%d] invalid Lower-Upper range [%.2f ->%.2f]",
														(second_run)?(2):(1),
														lower_val,
														upper_val);
		LOGINFO(str);
		return false;
	}
	//@@TBD def_data array to be removed after validation
	MATRIX def_data = MAT_ALLOC(raw_data->iX,raw_data->iY);
	valid_glass = true;
	
	int defect_count =0;
	double diff     = 0.0,
		   max_diff = 0.0	;
	for(int index =0;index< raw_data->iY-1;index++)
	{
		diff = fabs(MAT(raw_data,0,index+1)-MAT(raw_data,0,index));
		if(diff >limit_val)
		{
			if(RANGE_IN(MAT(raw_data,0,index),lower_val,upper_val))
			{
				MAT(def_data,0,defect_count) = MAT(raw_data,0,index);
				MAT(def_data,1,defect_count) = diff;
				defect_count++;
				valid_glass = false;
				if(max_diff < diff)
				{
					max_diff = diff;
				}
			}
		}
	}
	// print defect values
	if(0 !=defect_count)
	{
#if 0
		MATRIX temp_data = Mat_GetChild(def_data,0,1,0,defect_count-1);

		if(FALSE ==Mat_SaveASCII(temp_data,(second_run)?"defect_data_2.txt":"defect_data_1.txt"))
		{
			//ui_cmd_mem("defect data file save failed",DEN_TXT_MEM0);
			LOGINFO("defect data file save failed");
		}
		MAT_FREE(temp_data);
#endif
		sprintf(str,"[ng_test-%d] failed. [%d defects, Max %.3f]",
											(second_run)?(2):(1),
											defect_count,
											max_diff);
		LOGINFO(str);
	}
	else
	{
		LOGINFO("[ng_test-%d] OK",(second_run)?(2):(1));
	}
	MAT_FREE(def_data);
	return true;
};


bool filter_main(MATRIX raw_data,				// data input
				 MATRIX& final_data,				// data output after filtering
				 bool& valid_glass,				// glass valid?
				 int& num_data)					// final data count
{
	boolean mat_exp = false;
	int32     mat_digit =5;
	Mat_GetSaveDigit(&mat_exp,&mat_digit);
	Mat_SetSaveDigit(FALSE,5);
	/*
		First glass data filter
	
	*/
	// NG filter
	if(false == ng_test(raw_data,valid_glass))
	{
		// glass is invalid
		return false;
	}
	if(false ==valid_glass)
	{
		return false;
	}
	// Matrix for thickness filtering
	MATRIX def_data = MAT_ALLOC(raw_data->iX,raw_data->iY);
	
	thk_filt(raw_data,def_data,num_data);
	/*
		Adjust the MATRIX size for thickness filtered data
	*/
	MATRIX temp_data = Mat_GetChild(def_data,0,1,0,num_data-1);
	MAT_FREE(def_data);
	def_data = temp_data;
	temp_data = NULL;
	MATRIX filtered_data = MAT_ALLOC(def_data->iX,def_data->iY);
	LOGINFO("First NG");
	// pass on to median filter std filter
	LOGINFO("med filt");
	med_std_filt(def_data,filtered_data,num_data);
	LOGINFO("Second NG");
	/*
		Second glass data filter
	
	*/
	// NG filter
	if(false == ng_test(filtered_data,valid_glass,true))
	{
		// glass is invalid
		return false;
	}
	if(false ==valid_glass)
	{
		LOGINFO("Glass failed ng_test[2]");
		return false;
		//
		//cleanup
	}
	// Adjust Matrix size for linear interpolation as the linear interpolated data will be
	//   bigger in size than the 
	temp_data = Mat_GetChild(filtered_data,0,1,0,num_data-1);
	MAT_FREE(filtered_data);
	filtered_data = temp_data;
	temp_data = NULL;
	double end = MAT(filtered_data, 0, filtered_data->iY-1);
	double start = MAT(filtered_data, 0, 0);
	int Outsize =(int)(end) -(int)(start);
	MATRIX	final_filt = MAT_ALLOC(filtered_data->iX,Outsize-1);
	LOGINFO("lin interp");
	lin_interpol(filtered_data,final_filt);
	//Mat_SaveASCII(final_filt,"d:/data/filtered1.txt");
	//Mat_SaveASCII(final_filt,"after_interpol.txt");
	temp_data = MAT_ALLOC(final_filt->iX,final_filt->iY);
	MAT_FREE(filtered_data);
	filtered_data = temp_data;
	LOGINFO("mov_ave");
	mov_aver_filt(final_filt,filtered_data);
	int data_count =0;
	int step_size =5;
	int tmp_data =0.0;
	int tmp_int =0;
	for(int index =0;index<filtered_data->iY;index++)
	{
		tmp_data =MAT(filtered_data,0,index);
		tmp_int = ((int)(tmp_data));
		if(0 == (tmp_int%step_size) ) 
		{
			MAT(filtered_data,0,data_count) = MAT(filtered_data,0,index);
			MAT(filtered_data,1,data_count) = MAT(filtered_data,1,index);
			data_count++;
		}
	}
	MATRIX Tmp = Mat_GetChild(filtered_data,0,1,0,data_count-1);
	MAT_FREE(filtered_data);
	filtered_data =Tmp;
	num_data = data_count;
	LOGINFO("final data");
	Mat_SaveASCII(filtered_data,"d:/data/filtered1-2.txt");
	final_data=filtered_data;
	Mat_SetSaveDigit(mat_exp,mat_digit);
	MAT_FREE(final_filt);
	MAT_FREE(raw_data);
	MAT_FREE(def_data);
	LOGINFO("Filter OK");
	return true;
}

