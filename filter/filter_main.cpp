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

#include "pch.h"


#include "filter_main.h"
#define MAX_MED_FILT_VAL 100
#define INTERP_DATA_INTERVAL 5
//#define DUMP_ASCII_DATA
//#define DUMP_ALL_STEPS
#define DUMP_DEFECT_DATA
char DumpFolder[] = "d:/FilterDump/";
/**************************************************************

	linear_interp_enc :

	Linear interpolation of encoder value for interval of 1 mm

	Parameters
	mat_in	-> Input matrix [X encoder values] [Y thickness data]
	mat_in  -> result matrix [X encoder values] [Y thickness data]

	Note: Correct size calulation of out matrix is essential as it may be
			 smaller or larger in [Y]

**************************************************************/
void GetCurTimeStamp(std::string &TimeStr)
{
	CTime Tm = CTime::GetCurrentTime();
	CStringA Dstr(Tm.Format(_T("%d %b %Y-%H.%M.%S")).GetBuffer(0));
	TimeStr = Dstr.GetBuffer(0);
}
class FilterConfig
{
	std::string FileName;
	char str[MAX_PATH];
	double GetDoubleVal(const char* const Section,const char* const Key,double64 DefVal)
	{
		char Str[26] ={0};
		sprintf(Str,"%.5f",DefVal);
		GetPrivateProfileStringA(Section,Key,Str,str,MAX_PATH,FileName.c_str());
		return atof(str);
	};
	bool  SetDoubleVal(const char* const Section,const char* const Key,double64 DefVal)
	{
		char Str[26] ={0};
		sprintf(Str,"%.3f",DefVal);
		return (TRUE == WritePrivateProfileStringA(Section,Key,Str,FileName.c_str()));
		return true;
	}

	int GetIntVal(const char* const Section,const char* const Key,int DefVal)
	{
		return GetPrivateProfileIntA(Section,Key,DefVal,FileName.c_str());
	};

	bool  SetIntVal(const char* const Section,const char* const Key,const int IntVal)
	{
		char Str[26] ={0};
		sprintf(Str,"%d",IntVal);
		return (TRUE ==WritePrivateProfileStringA(Section,Key,Str,FileName.c_str()));
	}

public:
	FilterConfig(char * CfgName)
	{
		FileName = CfgName;
	}
	// NG filter helper functions
	bool GetNGLowerLimit(DWORD &LowerLimit)
	{
		char Section[] ="NGParam";	
		LowerLimit = GetIntVal(Section,	"Lower",100);
		return true;
	};
	bool GetNGUpperLimit(DWORD &UpperLimit)
	{
		char Section[] ="NGParam";	
		UpperLimit = GetIntVal(Section,	"upper",2999);
		return true;
	};

	bool GetNGDiffLimit_1(double64 &DiffLimit)
	{
		char Section[] ="NGParam";	
		DiffLimit = GetDoubleVal(Section,	"Limit",2999);
		return true;
	};

	bool GetNGDiffLimit_2(double64 &DiffLimit2)
	{
		char Section[] ="NGParam";	
		DiffLimit2 = GetDoubleVal(Section,	"Limit2",2999);
		return true;
	};

	bool SetNGLowerLimit(DWORD LowerLimit)
	{
		char Section[] ="NGParam";	
		return SetIntVal(Section,"Lower",LowerLimit);
	};

	bool SetNGUpperLimit(DWORD UpperLimit)	
	{
		char Section[] ="NGParam";	
		return SetIntVal(Section,"upper",UpperLimit);
	};

	bool SetNGDiffLimit_1(double64 DiffLimit)
	{
		char Section[] ="NGParam";	
		return SetDoubleVal(Section,"Limit",DiffLimit);
	};

	bool SetNGDiffLimit_2(double DiffLimit2)
	{
		char Section[] ="NGParam";	
		return SetDoubleVal(Section,"Limit2",DiffLimit2);
	};

	// First filter parameters
	bool GetFirstFiltGlassThk(DWORD & GlassThk)
	{
		char Section[] ="FirstFilt";
		GlassThk	= GetIntVal(Section,	"GlassThk",		500); 
		return true;
	};
	bool GetFirstFiltUpperTol(DWORD &UpperTol)
	{
		char Section[] ="FirstFilt";
		UpperTol	= GetIntVal(Section,	"UpperRange",	50); 
		return true;
	}
	bool GetFirstFiltLowerTol(DWORD &LowerTol)
	{
		char Section[] ="FirstFilt";
		LowerTol	= GetIntVal(Section,	"LoweRange",	50); 
		return true;
	}
	bool GetFirstFiltAllowThkDiff(DWORD &ThkDiff)
	{
		char Section[] ="FirstFilt";
		ThkDiff	= GetIntVal(Section,	"ThkDiff",	10); 
		return true;
	}
	// Set First Filter param
	bool SetFirstFiltGlassThk(DWORD  GlassThk)
	{
		char Section[] ="FirstFilt";
		return SetIntVal(Section,	"GlassThk",	GlassThk); 
	};

	bool SetFirstFiltUpperTol(DWORD UpperTol)	
	{
		char Section[] ="FirstFilt";
		return SetIntVal(Section,	"UpperRange",	UpperTol); 
	};

	bool SetFirstFiltLowerTol(DWORD LowerTol)
	{
		char Section[] ="FirstFilt";
		return SetIntVal(Section,	"LoweRange",	LowerTol); 
	}
	bool SetFirstFiltAllowThkDiff(DWORD ThkDiff)
	{
		char Section[] ="FirstFilt";
		return SetIntVal(Section,	"ThkDiff",	ThkDiff); 
	}
	bool GetFirstFiltAllowDefectLen(double64 &DefLen)
	{
		char Section[] ="FirstFilt";
		DefLen	= GetDoubleVal(Section,	"AllowedDefectLen",	5.0); 
		return true;
	};
	bool SetFirstFiltAllowDefectLen(double64 DefLen)
	{
		char Section[] ="FirstFilt";
		return SetDoubleVal(Section,	"AllowedDefectLen",	DefLen); 
	};
	bool GetMedFiltMask(DWORD &MaskVal)
	{
		char Section[] ="MedFilt";
		MaskVal= GetIntVal(Section,"MaskSize",35);
		return true;
	};
	bool SetMedFiltMask(int MaskVal)
	{
		char Section[] ="MedFilt";
		return SetIntVal(Section,"MaskSize",MaskVal);
	};

	bool GetMedFiltStdFac(double64 &StdFac)
	{
		char Section[] ="MedFilt";
		StdFac = GetDoubleVal(Section,"StdDevScaleLow",2.0);
		return true;
	};

	bool SetMedFiltStdFac(double64 StdFac)
	{
		char Section[] ="MedFilt";
		return SetDoubleVal(Section,"StdDevScaleLow",StdFac);
	};

	bool GetMovAveWindow(DWORD &WindowSize)
	{
		char Section[] ="MovAverage";
		WindowSize= GetIntVal(Section,"WindowSize",21);
		return true;
	};

	bool SetMovAveWindow(DWORD WindowSize)
	{
		char Section[] ="MovAverage";
		return SetDoubleVal(Section,"WindowSize",WindowSize);
	};
	bool GetFinalStepSize(int &StepSize)
	{
		char Section[] ="MovAverage";
		StepSize= GetIntVal(Section,"Interval",5);
		return true;
	}
	bool SetFinalStepSize(int StepSize)
	{
		char Section[] ="MovAverage";
		return SetIntVal(Section,"Interval",StepSize);
	}
	// First Filter param
	bool GetNgParam(int &LimitVal,int &LowerVal,int& UpperVal, bool bSecond= false)
	{
		char Section[] ="NGParam";
		if(true == bSecond)
		{
			LimitVal	= GetIntVal(Section,	"Limit2",	100);
		}
		else
		{
			LimitVal	= GetIntVal(Section,	"Limit",	100);
		}
		LowerVal	= GetIntVal(Section,	"Lower",	1);
		UpperVal	= GetIntVal(Section,	"Upper",	2999);
		return true;
	};

	bool ReadFirstFiltParam(int & GlassThk, int& LowerRange, int& UpperRange,double& ThkDiff)
	{
		char Section[] ="FirstFilt";
		GlassThk	= GetIntVal(Section,	"GlassThk",		500); 
		LowerRange	= GetIntVal(Section,	"LoweRange",	100);  
		UpperRange	= GetIntVal(Section,	"UpperRange",	100); 
		ThkDiff		= GetDoubleVal(Section,	"ThkDiff",		10.0); 
		return true;
	}
	bool ReadMedStdevFiltParam(unsigned& MaskSize, double& StdScale)
	{
		char Section[] ="MedFilt";

		MaskSize	= GetIntVal(Section,	"MaskSize",			35);
		StdScale	= GetDoubleVal(Section,	"StdDevScaleLow",	10.0); 
		return true;
	}
	int ReadMovAveWindowSize()
	{
		return GetIntVal("MovAverage","WindowSize",35);
	}
	bool GetRefrInd(double64 &RefrInd)
	{
		char Section[] = "General";
		RefrInd = GetDoubleVal(Section, "RefrIndex", 1.540);
		return true;
	};
	bool SetRefrInd(double64 RefrInd)
	{
		char Section[] = "General";
		return SetDoubleVal(Section, "RefrIndex", RefrInd);
	};
};
FilterConfig FiltConfig("D:/data/CFG_filter.txt");
bool MatrixSaveTimeStampC(MATRIX Mat,const std::string & Folder,const std::string &NameAdd)
{
	std::string TimeStamp;
	GetCurTimeStamp(TimeStamp);
	TimeStamp += "-";
	TimeStamp += NameAdd;
	std::string FileName = Folder+TimeStamp;
	Mat_SaveASCII(Mat,(char*)FileName.c_str());
	return true;
};

bool VecSaveTimeStampC(VECTOR Vec,const std::string & Folder,const std::string &NameAdd)
{
	std::string TimeStamp;
	GetCurTimeStamp(TimeStamp);
	TimeStamp += "-";
	TimeStamp += NameAdd;
	std::string FileName = Folder+TimeStamp;
	Vec_SaveASCII(Vec,(char*)FileName.c_str());
	return true;
};
bool linear_interp_enc(MATRIX mat_in,MATRIX& mat_out)
{
	if((NULL == mat_in)  || 
	   (NULL == mat_out) ||
	//   (mat_in->iX != mat_out->iX)||
	//   (mat_in->iY > mat_out->iY)||
	   (mat_in->iX<2))
	{
		LOGINFO(_T("[linear_interp_enc] Param Error"));
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
		//if(0 ==(index%INTERP_DATA_INTERVAL))
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
		*median_val = temp_array[((mask_size-1)/2)];
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
		LOGINFO(_T("Median filter invalid parameter"));
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
			LOGINFO(_T("Median filter failed [software error]"));
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
		LOGINFO(_T("moving ave filter invalid parameter"));
		return false;
	}
	//
	// Get second col
	VECTOR Vec = Mat_GetColumn(mat_in,1);
	int offset_data = ((ave_size-1)/2);
	double Val =0.0;
	int index = 0;
	for(index = 0/*offset_data*/;index <(mat_in->iY -ave_size);index++)
	{
		if(false ==aver(Vec->pData+index,ave_size,&Val))
		{
			VEC_FREE(Vec);
			LOGINFO(_T("Moving average failed [software error]"));
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

	GetPrivateProfileStringA(Section,"Limit","100",str,MAX_PATH,Filename);
	Limit = atof(str);
 	GetPrivateProfileStringA(Section,"Lower","1",str,MAX_PATH,Filename);
	Lower = atof(str);
	GetPrivateProfileStringA(Section,"Upper","2999",str,MAX_PATH,Filename);
	Upper = atof(str);
	return true;
}

bool read_ng_param_second(char * Filename,double64& Limit,double64& Lower,double64& Upper)
{
	char Section[] ="NGParam";
	char str[MAX_PATH] = {0};

	GetPrivateProfileStringA(Section,"Limit2","100",str,MAX_PATH,Filename);
	Limit = atof(str);
 	GetPrivateProfileStringA(Section,"Lower","1",str,MAX_PATH,Filename);
	Lower = atof(str);
	GetPrivateProfileStringA(Section,"Upper","2999",str,MAX_PATH,Filename);
	Upper = atof(str);
	return true;
}

bool read_first_filt_param(char *Filename,double64& glass_thk, double64& lower_range, double64& upper_range,double64& thk_diff,
						   double64& allowed_len)
{
	char Section[] ="FirstFilt";
	char str[MAX_PATH] = {0};

	GetPrivateProfileStringA(Section,"GlassThk","700",str,MAX_PATH,Filename);
	glass_thk = atof(str);

 	GetPrivateProfileStringA(Section,"UpperRange","100",str,MAX_PATH,Filename);
	upper_range = atof(str);

	GetPrivateProfileStringA(Section,"LoweRange","100",str,MAX_PATH,Filename);
	lower_range = atof(str);

	GetPrivateProfileStringA(Section,"ThkDiff","10",str,MAX_PATH,Filename);
	thk_diff = atof(str);

	GetPrivateProfileStringA(Section,"AllowedDefectLen","5.0",str,MAX_PATH,Filename);
	allowed_len = atof(str);

	return true;
}

bool read_med_stdev_filt_param(char *Filename,int& mask_size, double64& std_scale_low,double64& std_scale_high)
{
	char Section[] ="MedFilt";
	char str[MAX_PATH] = {0};

	GetPrivateProfileStringA(Section,"MaskSize","21",str,MAX_PATH,Filename);
	mask_size = atoi(str);

 	GetPrivateProfileStringA(Section,"StdDevScaleLow","3.0",str,MAX_PATH,Filename);
	std_scale_low = atof(str);

 	GetPrivateProfileStringA(Section,"StdDevScalehigh","3.0",str,MAX_PATH,Filename);
	std_scale_high = atof(str);

	return true;

}

bool read_mov_avefilt_param(char *Filename,int& window_size)
{
	char Section[] ="MovAverage";
	char str[MAX_PATH] = {0};

	GetPrivateProfileStringA(Section,"WindowSize","51",str,MAX_PATH,Filename);
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
	//if(FALSE ==Mat_SaveASCII(final_data,"d:/data/mov_ave_filt.txt"))
	if(FALSE ==MatrixSaveTimeStampC(final_data,DumpFolder,"mov_ave_filt.txt"))
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
	return linear_interp_enc(raw_data,final_data);
#ifdef DUMP_ALL_STEPS
	//if(FALSE ==Mat_SaveASCII(final_data,"d:/data/interp_filt.txt"))
	if(FALSE ==MatrixSaveTimeStampC(final_data,DumpFolder,"interp_filt.txt"))
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

	double64 std_scale_low  =0.0,
			 std_scale_high =0.0 ;

	read_med_stdev_filt_param(cfg_file,mask_size,std_scale_low,std_scale_high);

	if((0 ==raw_data->iY)||
	   (NULL == raw_data))
	{
		//ui_cmd_mem("Load file failed med stdev filt",DEN_TXT_MEM0);
		LOGINFO(_T("[med_std_filt] Invalid raw data for median filter"));
		return false;
	}
	else
	{
		// copy thickness data to vector
		VECTOR thk_data = Mat_GetColumn(raw_data,1);
		// run median filter code
		MATRIX int_data = MAT_ALLOC(raw_data->iX+1,raw_data->iY);
		double64 Val =0.0;
		int real_index =0;
		// do median filter and data subtraction in one shot
		int index = 0;
		for(index = 0/*offset_data*/;index <(thk_data->iNum -mask_size);index++)
		{
			real_index =index+(mask_size)/2;
			Val = MAT(raw_data,0,index);
			Val =0.0;
			if(false == median(thk_data->pData+index,mask_size,&Val))
			{
				VEC_FREE(thk_data);
				// 
				LOGINFO(_T("[med_std_filt] Median filter failed"));
				return false;
			}
			//mat_out->pData[filt_index][1] = Val;
			MAT(int_data,1,real_index) = MAT(raw_data,1,real_index)-Val;
			//mat_out->pData[indfilt_indexex][0] = mat_in->pData[index+offset_data][0];
			MAT(int_data,0,real_index)= MAT(raw_data,0,real_index);
			MAT(int_data,2,real_index)= Val;//MAT(raw_data,1,real_index);
		}
		// Get the vector of filtered data
		VECTOR med_filt_thk = Mat_GetColumn(int_data,1);
		//VECTOR med_std_filt = NULL;
		VEC_FREE(thk_data);
		thk_data = NULL;
#ifdef DUMP_ALL_STEPS
		//Mat_SaveASCII(int_data,"d:/data/before_std.txt");
		MatrixSaveTimeStampC(int_data,DumpFolder,"before_std.txt");
		// Get the std dev
		//Vec_SaveASCII(med_filt_thk,"d:/data/vec_std.txt");
		VecSaveTimeStampC(med_filt_thk,DumpFolder,"vec_std.txt");
#endif
		double64 std = Vec_STDSc(med_filt_thk);
		double temp =std;

		double64 abs_limit = std*std_scale_low;
		double64 cur_val =0.0;
		VECTOR temp_data = VEC_ALLOC(med_filt_thk->iNum);
		int filt_index =0;
		for(index =0;index<med_filt_thk->iNum;index++)
		{
			cur_val = fabs(VEC(med_filt_thk,index));
			//if(RANGE_IN(cur_val,lower_limit,higher_limit))
			if(cur_val <abs_limit)
			{
				VEC(temp_data,index)= cur_val;
			}
			else
			{
				VEC(temp_data,index)= 9999;
			}
		}
		VEC_FREE(med_filt_thk);
		med_filt_thk = NULL;
		//Vec_SaveASCII(temp_data,"after_std.txt");
		// compare and copy to filtered data
		if(filtered_data->iY < int_data->iY)
		{
			LOGINFO(_T("[med_std_filt] insufficient result matrix [software error]"));
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
			LOGINFO(_T("[med_std_filt] filtering resulted empty data set !"));
			return false;
		}
		else
		{
			num_data = full_filt_index-1;
		}
#ifdef DUMP_ALL_STEPS
		// save to file
		//if(FALSE ==Mat_SaveASCII(filtered_data,"d:/data/median_std.txt"))
		if(FALSE ==MatrixSaveTimeStampC(filtered_data,DumpFolder,"median_std.txt"))
		{
			//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
		}
#endif
		VEC_FREE(temp_data);
		temp_data = NULL;
		MAT_FREE(int_data);
		int_data = NULL;
	}
	// cleanup
	return true;
};
#if 1
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
			thk_diff    = 0.0,
			allowed_len =0;

	char cfg_file[] = "D:/data/CFG_filter.txt";
	// load file matrix
	read_first_filt_param(cfg_file,glass_thk,lower_range,upper_range,thk_diff,allowed_len);

	// Also need the NG param to filter the rekject data
	double64 limit_val =0.0,			// check validity of encoder difference
		     lower_val =0.0,			// Glass inlet limit
			 upper_val =0.0;			// Glass outlet limit
	read_ng_param_second(cfg_file,limit_val,lower_val,upper_val);
	if((0	== raw_data->iY)||
	  (NULL == raw_data))
	{
		DBGPRINT("Invalid rw_data for first filter\n");
		return false;
	}
	else
	{
		int filt_count =0;
		double64 thk_diff_cal=0.0;
		double64 thk =0.0;
		double64 len_diff =0.0;
		double len_gap =0.0;
		double enc_pos =0.0;
		double raw_data_check = 0.0;
		for(int index =0;index<raw_data->iY-1;index++)
		{
			thk_diff_cal = fabs(MAT(raw_data,1,index)- MAT(raw_data,1,index+1));
			len_diff     = fabs(MAT(raw_data,0,index)- MAT(raw_data,0,index+1));
			thk = MAT(raw_data,1,index);
			raw_data_check = MAT(raw_data, 1, index);
			if((RANGE_IN(raw_data_check,glass_thk-lower_range,glass_thk+upper_range))&&
				(thk_diff_cal <thk_diff))
			{
				// copy to output
				MAT(filtered_data,0,filt_count) = MAT(raw_data,0,index);
				MAT(filtered_data,1,filt_count) = MAT(raw_data,1,index);
				filt_count++;
				len_gap=0;
			}
			else
			{
				enc_pos = MAT(raw_data,0,index);
				if(RANGE_IN(enc_pos,lower_val,upper_val))
				{
					len_diff     = fabs(MAT(raw_data,0,index)- MAT(raw_data,0,index+1));
				len_gap += len_diff;
				if(len_gap > allowed_len )
				{
					// Error Message
					DBGPRINT("[thk_filt] failed at [ %d->%.2f]\n", index + 1, enc_pos);
					// save matrix
					MatrixSaveTimeStampC(raw_data,DumpFolder,"thk-filt-fail.txt");
					return false;
					}
				}
			}
		}
		// save to file
#ifdef DUMP_ALL_STEPS
		// @@TBD: Remove the save function after validation
		if(FALSE ==Mat_SaveASCII(filtered_data,"d:/data/min_max_filt.txt"))
		{
			//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
			LOGINFO("[min max]Mareix save file failed");
		}
#endif
		if(0 == filt_count)
		{
			//Mat_SaveASCII(raw_data,"d:/data/empty-Filt.txt");
			MatrixSaveTimeStampC(raw_data,DumpFolder,"empty-thk-Filt.txt");
			DBGPRINT("[thk_filt] filtering resulted empty data set !\n");
			return false;
		}
		else
		{
			num_data = filt_count-1;
		}
	}
	// cleanup
	return true;
};
#else// Optimized code
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
			thk_diff    = 0.0,
			allowed_len =0;

	char cfg_file[] = "D:/data/CFG_filter.txt";
	// load file matrix
	read_first_filt_param(cfg_file,glass_thk,lower_range,upper_range,thk_diff,allowed_len);

	// Also need the NG param to filter the rekject data
	double64 limit_val =0.0,			// check validity of encoder difference
		     lower_val =0.0,			// Glass inlet limit
			 upper_val =0.0;			// Glass outlet limit
	read_ng_param_second(cfg_file,limit_val,lower_val,upper_val);



	if((0	== raw_data->iY)||
	  (NULL == raw_data))
	{
		LOGINFO(_T("Invalid rw_data for first filter"));
		return false;
	}
	else
	{
		int filt_count =0;
		double64 thk_diff_cal=0.0;
		double64 thk =0.0;
		double64 len_diff =0.0;
		double len_gap =0.0;
		double enc_pos =0.0;
		double min_thk =glass_thk-lower_range,
			   max_thk =glass_thk+upper_range;
		for(int index =0;index<raw_data->iY-1;index++)
		{
			// get thickness
			enc_pos = MAT(raw_data,0,index);
			if(RANGE_IN(enc_pos,lower_val,upper_val))
			{
				thk_diff_cal = fabs(MAT(raw_data,1,index)- MAT(raw_data,1,index+1));
				
				thk = MAT(raw_data,1,index);
				//if((RANGE_IN(MAT(raw_data,1,index),glass_thk-lower_range,glass_thk+upper_range))&&
				if((RANGE_IN(thk,min_thk,max_thk))&&
					(thk_diff_cal <thk_diff))
				{
					// copy to output
					MAT(filtered_data,0,filt_count) = MAT(raw_data,0,index);
					MAT(filtered_data,1,filt_count) = MAT(raw_data,1,index);
					filt_count++;
					len_gap=0;
				}
				else
				{
					len_diff     = fabs(MAT(raw_data,0,index)- MAT(raw_data,0,index+1));
					len_gap += len_diff;
					if(len_gap > allowed_len )
					{
						// Error Message
						LOGINFO(_T("[thk_filt] failed at [ %d->%.2f]"),index+1,enc_pos);
						// save matrix
						MatrixSaveTimeStampC(raw_data,DumpFolder,"thk-filt-fail.txt");
						return false;
					}
				}
			}
			else
			{
				MAT(filtered_data,0,filt_count) = MAT(raw_data,0,index);
				MAT(filtered_data,1,filt_count) = MAT(raw_data,1,index);
				filt_count++;
			}
		}
		// save to file
#ifdef DUMP_ALL_STEPS
		// @@TBD: Remove the save function after validation
		if(FALSE ==Mat_SaveASCII(filtered_data,"d:/data/min_max_filt.txt"))
		{
			//ui_cmd_mem("[min max]Mareix save file failed",DEN_TXT_MEM0);
			LOGINFO("[min max]Mareix save file failed");
		}
#endif
		if(0 == filt_count)
		{
			//Mat_SaveASCII(raw_data,"d:/data/empty-Filt.txt");
			MatrixSaveTimeStampC(raw_data,DumpFolder,"empty-thk-Filt.txt");
			LOGINFO(_T("[thk_filt] filtering resulted empty data set !"));
			return false;
		}
		else
		{
			num_data = filt_count-1;
		}
	}
	// cleanup
	return true;
};
#endif
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
		//sprintf(str,"[ng_test-%d] Invalid input or no input",(second_run)?(2):(1));
		LOGINFO(_T("[ng_test-%d] Invalid input or no input"), (second_run) ? (2) : (1));
		return false;
	}

	// parameter validation
	if(!RANGE_IN(limit_val,2.0,100.0))
	{
		/*sprintf(str,"[ng_test-%d] invalid %s parameter [range 2.0-30.0] [%.2f]",
														(second_run)?(2):(1),
														(second_run)?("[Limit2]"):("[Limit]"),
														limit_val);*/
		LOGINFO(_T("[ng_test-%d] invalid %s parameter [range 2.0-30.0] [%.2f]"),
			(second_run) ? (2) : (1),
			(second_run) ? (_T("[Limit2]")) : (_T("[Limit]")),
			limit_val);
		return false;
	}

	if(!RANGE_IN(lower_val,0.0,3000.0))
	{
		/*sprintf(str,"[ng_test-%d] invalid [Lower] parameter [0.0-3000.0] [%.2f]",
														(second_run)?(2):(1),
														lower_val);*/
		LOGINFO(_T("[ng_test-%d] invalid [Lower] parameter [0.0-3000.0] [%.2f]"),
			(second_run) ? (2) : (1),
			lower_val);
		return false;
	}

	if(!RANGE_IN(upper_val,0.0,3000.0))
	{
		/*sprintf(str,"[ng_test-%d] invalid [Upper] parameter [0.0-3000.0] [%.2f]",
														(second_run)?(2):(1),
														upper_val);*/
		LOGINFO(_T("[ng_test-%d] invalid [Upper] parameter [0.0-3000.0] [%.2f]"),
			(second_run) ? (2) : (1),
			upper_val);
		return false;
	}

	if(upper_val <= lower_val)
	{
		/*sprintf(str,"[ng_test-%d] invalid Lower-Upper range [%.2f ->%.2f]",
														(second_run)?(2):(1),
														lower_val,
														upper_val);*/
		LOGINFO(_T("[ng_test-%d] invalid Lower-Upper range [%.2f ->%.2f]"),
			(second_run) ? (2) : (1),
			lower_val,
			upper_val);
		return false;
	}
	//@@Vijay#01Aug2016$ New code added for detection of X Position Error
	
	if (false == second_run)
	{
		double64 DiffVal1 = fabs(MAT(raw_data, 0, 1) - MAT(raw_data, 0, 0)),
				 DiffVal2 = fabs(MAT(raw_data, 0, 2) - MAT(raw_data, 0, 1));

		if (DiffVal2 >100.0 ||DiffVal1 >100.0)
		{
			LOGWARN(_T("X Position error [%.3f,%.3f]"), DiffVal1, DiffVal2);

			if (FALSE == MatrixSaveTimeStampC(raw_data, DumpFolder, "X-Pos Error.txt"))
			{
				//ui_cmd_mem("defect data file save failed",DEN_TXT_MEM0);
				LOGINFO(_T("[X POS] defect data file save failed"));
			}
			return false;
		}
	}
	//@@Vijay#01Aug2016$
	//@@TBD def_data array to be removed after validation
	MATRIX def_data = MAT_ALLOC(raw_data->iX,raw_data->iY);
	valid_glass = true;
	
	int defect_count =0;
	double diff     = 0.0,
		   max_diff = 0.0	,
		   diff_loc = 0.0;
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
					diff_loc = MAT(raw_data,0,index);
				}
			}
		}
	}
	// print defect values
	if(0 !=defect_count)
	{
#ifdef DUMP_DEFECT_DATA
		MATRIX temp_data = Mat_GetChild(def_data,0,1,0,defect_count-1);

		//if(FALSE ==Mat_SaveASCII(temp_data,(second_run)?"d:/data/defect_data_2.txt":"d:/data/defect_data_1.txt"))
		if(FALSE ==MatrixSaveTimeStampC(temp_data,DumpFolder,(second_run)?"defect_data_2.txt":"defect_data_1.txt"))
		{
			//ui_cmd_mem("defect data file save failed",DEN_TXT_MEM0);
			LOGINFO(_T("defect data file save failed"));
		}
		MAT_FREE(temp_data);
#endif
		/*sprintf(str,"[ng_test-%d] failed. [%d defects, Max %.3f at %.3f]",
											(second_run)?(2):(1),
											defect_count,
											max_diff,diff_loc);*/
		LOGINFO(_T("[ng_test-%d] failed. [%d defects, Max %.3f at %.3f]"),
			(second_run) ? (2) : (1),
			defect_count,
			max_diff, diff_loc);
	}
	else
	{
		LOGINFO(_T("[ng_test-%d] OK"), (second_run) ? (2) : (1));
	}
	MAT_FREE(def_data);
	return true;
};


bool filter_main(MATRIX raw_data,				// data input
				 MATRIX& final_data,			// data output after filtering
				 bool& valid_glass,				// glass valid?
				 int& num_data)					// final data count
{
	boolean mat_exp = false;
	int32     mat_digit =5;
	Mat_GetSaveDigit(&mat_exp,&mat_digit);
	Mat_SetSaveDigit(FALSE,5);
	if(NULL == raw_data)
	{
		DBGPRINT("empty data for filter [software error]\n");
		return false;
	}
#ifdef DUMP_ASCII_DATA	
	//Mat_SaveASCII(raw_data,"d:/data/ODI-thk_filtraw-data.txt");
	MatrixSaveTimeStampC(raw_data,DumpFolder,"raw-data.txt");
#endif

	// NG filter
	if(false == ng_test(raw_data,valid_glass))
	{
		DBGPRINT("NG filter false\n");
		// glass is invalid
		return false;
	}
	if(false ==valid_glass)
	{
		DBGPRINT("valid_glass false\n");
		return false;
	}

	/***************/
	// Thickness Filter
	/***************/
	// Matrix for thickness filtering
	MATRIX thk_filt_op = MAT_ALLOC(raw_data->iX,raw_data->iY);

	if(false ==thk_filt(raw_data,thk_filt_op,num_data))
	{
		DBGPRINT("thk_filt_op false\n");
		MAT_FREE(thk_filt_op);
		return false;
	}


#ifdef DUMP_ASCII_DATA	
	//Mat_SaveASCII(thk_filt_op,"d:/data/ODI-thk_filt.txt");
	MatrixSaveTimeStampC(thk_filt_op,DumpFolder,"thk-filt.txt");
#endif
	/***********************************/
	// Median Std dev filter
	/***********************************/
	// Adjust Matrix size for median and std-dev filter
	MATRIX std_filt_ip = Mat_GetChild(thk_filt_op,0,1,0,num_data-1);

	MATRIX std_filt_op = MAT_ALLOC(thk_filt_op->iX,thk_filt_op->iY);
	MAT_FREE(thk_filt_op);
	thk_filt_op = NULL;

	// pass on to median filter std filter
	if(false == med_std_filt(std_filt_ip,std_filt_op,num_data))
	{
		//Mat_SaveASCII(std_filt_ip,"d:/data/med-filt-err.txt");
		MatrixSaveTimeStampC(std_filt_ip,DumpFolder,"med-filt-err.txt");
		MAT_FREE(std_filt_op);
		MAT_FREE(std_filt_ip);
		DBGPRINT("med_std_filt false\n");
		return false;
	}
	MAT_FREE(std_filt_ip);
	std_filt_ip =NULL;
#ifdef DUMP_ASCII_DATA
	MatrixSaveTimeStampC(std_filt_op,DumpFolder,"Med-Filt.txt");
	LOGINFO("median FIltered %d",num_data);
#endif

	MATRIX int_filt_ip = Mat_GetChild(std_filt_op,0,1,0,num_data-1);
	MAT_FREE(std_filt_op);
	std_filt_op =NULL;
	/***********************************/
	// NG filter [2]
	/***********************************/

	if(false == ng_test(int_filt_ip,valid_glass,true))
	{
		// glass is invalid
		DBGPRINT("ng_test2 false\n");
		MAT_FREE(int_filt_ip);
		return false;
	}
	if(false ==valid_glass)
	{
		//LOGINFO("Glass failed ng_test[2]");
		DBGPRINT("int_filt_ip false\n");
		MAT_FREE(int_filt_ip);
		return false;
		//
		//cleanup
	}

	/***************************************/
	// Linear Interpolation to 1 mm distance
	/***************************************/
	// Adjust Matrix size for linear interpolation

	int Outsize =(int)( MAT(int_filt_ip, 0, int_filt_ip->iY-1)) -(int)(MAT(int_filt_ip, 0, 0));
	if (Outsize <200)
	{
		MAT_FREE(int_filt_ip);
		DBGPRINT("Insufficient data for filtering\n");

		return false;

	}
	MATRIX	int_filt_op = MAT_ALLOC(int_filt_ip->iX,Outsize-1);


	if(false ==lin_interpol(int_filt_ip,int_filt_op))
	{
		MAT_FREE(int_filt_ip);
		MAT_FREE(int_filt_op);
		DBGPRINT("Linear Interpolation failed\n");
		return false;
	}
	MAT_FREE(int_filt_ip);	
	int_filt_ip =NULL;
	MATRIX mov_ave_op = MAT_ALLOC(int_filt_op->iX,int_filt_op->iY);
	MAT_FREE(int_filt_ip);
	//filtered_data = temp_data;

#ifdef DUMP_ASCII_DATA
	//Mat_SaveASCII(int_filt_op,"d:/data/after-li.txt");
	MatrixSaveTimeStampC(int_filt_op,DumpFolder,"after-li.txt");
#endif
	/***************************************/
	// Noving averrage filter
	/***************************************/
	mov_aver_filt(int_filt_op,mov_ave_op);
	MAT_FREE(int_filt_op);
	int_filt_op =NULL;
#ifdef DUMP_ASCII_DATA
	//Mat_SaveASCII(mov_ave_op,"d:/data/filtered1.txt");
	MatrixSaveTimeStampC(mov_ave_op,DumpFolder,"after mov-ave.txt");
#endif
	int data_count =0;
	int step_size =5;
	int tmp_data =0;
	int tmp_int =0;
	FiltConfig.GetFinalStepSize(step_size);
	for(int index =0;index<mov_ave_op->iY;index++)
	{
		tmp_data =(int)MAT(mov_ave_op,0,index);
		tmp_int = ((int)(tmp_data));
		if(0 == (tmp_int%step_size) ) 
		{
			MAT(mov_ave_op,0,data_count) = MAT(mov_ave_op,0,index);
			MAT(mov_ave_op,1,data_count) = MAT(mov_ave_op,1,index);
			data_count++;
		}
	}
	MATRIX ret_filt_data = Mat_GetChild(mov_ave_op,0,1,0,data_count-1);
	MAT_FREE(mov_ave_op);
	mov_ave_op= NULL;
#ifdef DUMP_ASCII_DATA
	//Mat_SaveASCII(ret_filt_data,"d:/data/filtered1-2.txt");
	MatrixSaveTimeStampC(ret_filt_data,DumpFolder,"final.txt");
#endif
	final_data=ret_filt_data;
	Mat_SetSaveDigit(mat_exp,mat_digit);
	//MAT_FREE(final_filt);
	//MAT_FREE(raw_data);
	LOGINFO(_T("Filter OK"));
	return true;
}

bool NGFilt1Conf::GetLower(DWORD &LowerVal)
{
	return FiltConfig.GetNGLowerLimit(LowerVal);
}

bool NGFilt1Conf::GetUpper(DWORD &UpperVal)
{
	return FiltConfig.GetNGUpperLimit(UpperVal);
}

bool NGFilt1Conf::GetMaxDiff(double64 &Diff)
{
	return FiltConfig.GetNGDiffLimit_1(Diff);
}

bool NGFilt1Conf::SetLower(DWORD NewLower)
{
	return FiltConfig.SetNGLowerLimit(NewLower);
}

bool NGFilt1Conf::SetUpper(DWORD NewHigher)
{
	return FiltConfig.SetNGUpperLimit(NewHigher);
}

bool NGFilt1Conf::SetMaxDiff(double64 Diff)
{
	return FiltConfig.SetNGDiffLimit_1(Diff);
}

bool NGFilt2Cof::GetMaxDiff(double64 &Diff)
{
	return FiltConfig.GetNGDiffLimit_2(Diff);
}

bool NGFilt2Cof::SetMaxDiff(double64 Diff)
{
	return FiltConfig.SetNGDiffLimit_2(Diff);
}

bool FirstFiltConf::GetGlasThk(DWORD &Thk)
{
	return FiltConfig.GetFirstFiltGlassThk(Thk);
}
bool FirstFiltConf::GetDiffeence(DWORD &Diff)
{
	return FiltConfig.GetFirstFiltAllowThkDiff(Diff);
}
bool FirstFiltConf::GetAllowedDefect(double &DefLen)
{
	return FiltConfig.GetFirstFiltAllowDefectLen(DefLen);
};
bool FirstFiltConf::SetAllowedDefect(double DefLen)
{
	return FiltConfig.SetFirstFiltAllowDefectLen(DefLen);
};
bool FirstFiltConf::SetDiffeence(DWORD Diff)
{
	return FiltConfig.SetFirstFiltAllowThkDiff(Diff);
}
bool FirstFiltConf::GetUpperthkTol(DWORD &UpprtTOl)
{
	return FiltConfig.GetFirstFiltUpperTol(UpprtTOl);
}

bool FirstFiltConf::GetLowerthkTol(DWORD &LowerTol)
{
	return FiltConfig.GetFirstFiltLowerTol(LowerTol);
}

bool FirstFiltConf::SetGlasThk(DWORD Thk)
{
	return FiltConfig.SetFirstFiltGlassThk(Thk);
}

bool FirstFiltConf::SetUpperthkTol(DWORD UpprtTOl)
{
	return FiltConfig.SetFirstFiltUpperTol(UpprtTOl);
}

bool FirstFiltConf::SetLowerthkTol(DWORD LowerTol)
{
	return FiltConfig.SetFirstFiltLowerTol(LowerTol);
}

bool MedStdFiltConf::GetMaskSize(DWORD &Mask)
{
	return FiltConfig.GetMedFiltMask(Mask);
}

bool MedStdFiltConf::GetStdFact(double &Fact)
{
	return FiltConfig.GetMedFiltStdFac(Fact);
}

bool MedStdFiltConf::SetMaskSize(DWORD Mask)
{
	return FiltConfig.SetMedFiltMask(Mask);
}

bool MedStdFiltConf::SetStdFact(double Fact)
{
	return FiltConfig.SetMedFiltStdFac(Fact);
}

bool MovAveFiltConf::GetWindowSize(DWORD &Window)
{
	return FiltConfig.GetMovAveWindow(Window);
}

bool MovAveFiltConf::GetFInalEncInterval(int &Interval)
{
	return FiltConfig.GetFinalStepSize(Interval);
}

bool MovAveFiltConf::SetWindowSize(DWORD Window)
{
	return FiltConfig.SetMovAveWindow((Window));
}

bool MovAveFiltConf::SetFInalEncInterval(int Interval)
{
	return FiltConfig.SetFinalStepSize(Interval);
}
// NTG Calibration Support for Precitec Sensor
bool GetCalibLimits(int GlassThk,int &MinCalib,int& MaxCalib)
{
	if((GlassThk >1300) || 
	   (GlassThk<300) )
	{
		return false;
	}
	MinCalib = GlassThk;
	MaxCalib = 6000-GlassThk;
	return true;
}
bool LoadCalibData(MATRIX &CalibMat,int Thk)
{
	// Make the filename
	int Step = (Thk+25)/50;
	Thk = Step*50;
	// Create the filename
	if(NULL != CalibMat)
	{
		MAT_FREE(CalibMat);
		CalibMat =NULL;
	}
	char FileName[128] = {0};
	sprintf(FileName,"d:/Cal/%d.cal",Thk);
	CalibMat =Mat_Load(FileName);
	return NULL != CalibMat;
}

double64 GetRefrIndex()
{
	double64 RI;
	FiltConfig.GetRefrInd(RI);
	return RI;
};
bool SetRefrIndex(double64 RI)
{
	return FiltConfig.SetRefrInd(RI);
}

void ntg_filter_test_TC17(VECTOR NTG_DATA_X, VECTOR NTG_DATA_T, VECTOR& NTG_TEST_X, VECTOR& NTG_TEST_T)
{
	MATRIX raw_data;
	int num_data = 0;
	//VECTOR NTG_DATA_X;
	//VECTOR NTG_DATA_T;

	//DBGPRINT("hr1 테스트 데이터 생성\n");
	//NTG_DATA_X = Vec_Load("D:\\TestApp3\\NTG_TEST\\CHECK_DATA\\NTG_DATA_X.txt");
	//NTG_DATA_T = Vec_Load("D:\\TestApp3\\NTG_TEST\\CHECK_DATA\\NTG_DATA_T.txt");

	DBGPRINT("NTG_DATA_X->iNum = % d\n", NTG_DATA_X->iNum);
	raw_data = MAT_ALLOC(2, NTG_DATA_X->iNum);
	for (int32 i = 0; i < NTG_DATA_X->iNum; i++)
	{
		MAT(raw_data, 0, i) = VEC(NTG_DATA_X, i);
		MAT(raw_data, 1, i) = VEC(NTG_DATA_T, i) * 0.001;
	}

	MATRIX raw_data_ip = Mat_GetChild(raw_data, 0, 1, 0, raw_data->iY - 1);
	DBGPRINT("raw_data->iY = % d\n", raw_data->iY);
	DBGPRINT("hr2 데이터 필터링_raw_data_ip\n");

	//MATRIX thk_filt_op = MAT_ALLOC(raw_data->iX, raw_data->iY);
	//thk_filt(raw_data_ip, thk_filt_op, num_data);
	//DBGPRINT("hr2 데이터 필터링_thk_filt_op\n");

	MATRIX std_filt_op = MAT_ALLOC(raw_data_ip->iX, raw_data_ip->iY);
	med_std_filt(raw_data, std_filt_op, num_data);
	DBGPRINT("hr2 데이터 필터링_med_std_filt\n");

	MATRIX int_filt_ip = Mat_GetChild(std_filt_op, 0, 1, 0, num_data - 1);
	int Outsize = (int)(MAT(int_filt_ip, 0, int_filt_ip->iY - 1)) - (int)(MAT(int_filt_ip, 0, 0));
	DBGPRINT("Outsize = %d\n", Outsize);
	MATRIX	int_filt_op = MAT_ALLOC(int_filt_ip->iX, Outsize - 1);
	lin_interpol(int_filt_ip, int_filt_op);
	DBGPRINT("hr2 데이터 필터링_lin_interpol\n"); //1mm데이터변경

	MATRIX mov_ave_op = MAT_ALLOC(int_filt_ip->iX, int_filt_ip->iY);
	mov_aver_filt(int_filt_op, mov_ave_op);
	DBGPRINT("hr2 데이터 필터링_mov_ave_op\n");
	Mat_SetSaveDigit(FALSE, 4);
	MatrixSaveTimeStampC(mov_ave_op, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr6_mov_ave_op.txt");

	int data_count = 0;
	int step_size = 5;
	int tmp_data = 0;
	int tmp_int = 0;

	for (int index = 0; index < mov_ave_op->iY; index++)
	{
		tmp_data = (int)MAT(mov_ave_op, 0, index);
		tmp_int = ((int)(tmp_data));
		if (0 == (tmp_int % step_size))
		{
			MAT(mov_ave_op, 0, data_count) = MAT(mov_ave_op, 0, index);
			MAT(mov_ave_op, 1, data_count) = MAT(mov_ave_op, 1, index);
			data_count++;
		}
	}
	MATRIX ret_filt_data = Mat_GetChild(mov_ave_op, 0, 1, 0, data_count - 1);
	DBGPRINT("hr2 데이터 필터링_5mm\n");

	DBGPRINT("hr3 데이터 저장\n");

	Mat_SetSaveDigit(FALSE, 4);
	MatrixSaveTimeStampC(raw_data_ip, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr1_raw_data.txt");
	//MatrixSaveTimeStampC(thk_filt_op, "D:\\TestApp3\\FILTER_DATA", "hr2_thk_filt_op.txt");
	MatrixSaveTimeStampC(std_filt_op, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr3_std_filt_op.txt");
	MatrixSaveTimeStampC(int_filt_ip, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr4_int_filt_ip.txt");
	MatrixSaveTimeStampC(int_filt_op, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr5_int_filt_op.txt");
	MatrixSaveTimeStampC(ret_filt_data, "D:\\TestApp3\\NTG_TEST\\FILTER_DATA\\", "hr7_ret_filt_data.txt");

	NTG_TEST_X = Mat_GetColumn(ret_filt_data, 0);
	NTG_TEST_T = Mat_GetColumn(ret_filt_data, 1);

	MAT_FREE(raw_data);
	MAT_FREE(raw_data_ip);
	MAT_FREE(std_filt_op);
	//MAT_FREE(thk_filt_op);
	MAT_FREE(int_filt_ip);
	MAT_FREE(int_filt_op);
	MAT_FREE(mov_ave_op);
	MAT_FREE(ret_filt_data);
}
// *********** NTG Calibration Support for Precitec Sensor