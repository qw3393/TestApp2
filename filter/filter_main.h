#pragma once
#include <string>
class NGFilt1Conf
{
public:
	// Get
	bool GetLower(DWORD &LowerVal);
	bool GetUpper(DWORD &LowerVal);
	bool GetMaxDiff(double64 &Diff);
	// Set
	bool SetLower(DWORD NewLower);
	bool SetUpper(DWORD NewHigher);
	bool SetMaxDiff(double64 Diff);
};

class NGFilt2Cof
{
public:
	// Get
	bool GetMaxDiff(double64 &Diff);
	// Set
	bool SetMaxDiff(double64 Diff);
};

class FirstFiltConf
{
public:
// Get
	bool GetGlasThk(DWORD &Thk);
	bool GetUpperthkTol(DWORD &UpprtTOl);
	bool GetLowerthkTol(DWORD &UpprtTOl);
	bool GetDiffeence(DWORD &Diff);
	bool GetAllowedDefect(double &DefLen);
// Set
	bool SetGlasThk(DWORD Thk);
	bool SetUpperthkTol(DWORD UpprtTOl);
	bool SetLowerthkTol(DWORD UpprtTOl);
	bool SetDiffeence(DWORD Diff);
	bool SetAllowedDefect(double DefLen);
};

class MedStdFiltConf
{
public:
	// Get
	bool GetMaskSize(DWORD &Mask);
	bool GetStdFact(double &Fact);
	// Set
	bool SetMaskSize(DWORD Mask);
	bool SetStdFact(double Fact);
};

class MovAveFiltConf
{
public:
	// Get
	bool GetWindowSize(DWORD &Window);
	bool GetFInalEncInterval(int &Interval);
	// Set
	bool SetWindowSize(DWORD Window);
	bool SetFInalEncInterval(int Interval);
};
bool MatrixSaveTimeStampC(MATRIX Mat,const std::string & Folder,const std::string &NameAdd);
bool VecSaveTimeStampC(VECTOR Vec,const std::string & Folder,const std::string &NameAdd);
void GetCurTimeStamp(std::string &TimeStr);

bool linear_interp_enc(MATRIX mat_in,MATRIX& mat_out);
bool median_filt(MATRIX mat_in,MATRIX mat_out,int filt_size);
bool moving_ave(MATRIX mat_in,MATRIX mat_out, int ave_size);
bool aver(double *val_array,int size,double *OutValue);
bool median(double *val_array,int window_size,double *OutValue);
bool filter_main(MATRIX raw_data,				// data input
				 MATRIX& final_data,				// data output after filtering
				 bool& valid_glass,				// glass valid?
				 int& num_data);					// final data count
bool ng_test(MATRIX raw_data,bool& valid_glass,bool bSecond = false);
bool thk_filt(MATRIX raw_data, MATRIX filtered_data,int &num_data);
bool read_first_filt_param(char *file_name,double64& glass_thk, double64& lower_range, double64& upper_range,double64& thk_diff);
bool read_med_stdev_filt_param(char *Filename,int& mask_size, double64& std_scale_low/*,double64& std_scale_high*/);
bool med_std_filt(MATRIX raw_data, MATRIX filtered_data,int &num_data);
bool lin_interpol(MATRIX raw_data, MATRIX& final_data);
bool read_mov_avefilt_param(char *Filename,int& window_size);
bool mov_aver_filt(MATRIX raw_data, MATRIX& final_data);
// NTG Calibration Support for Precitec Sensor
bool LoadCalibData(MATRIX &CalibMat,int Thk);
bool GetCalibLimits(int GlassThk,int &MinCalib,int& MaxCalib);
double64 GetRefrIndex();
bool SetRefrIndex(double64 RI);
void ntg_filter_test_TC17(VECTOR NTG_DATA_X, VECTOR NTG_DATA_T, VECTOR& NTG_TEST_X, VECTOR& NTG_TEST_T);
// ..........Calibration

