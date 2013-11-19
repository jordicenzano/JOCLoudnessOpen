#pragma once
#include <iostream>

//Errors header

#define JOC_Error_( code ) CjocException(code, GetErrorMsg(code) , __FUNCTION__ , __FILE__, __LINE__)
#define JOC_Error_arg_( code , arg) CjocException(code, GetErrorMsg(code) + (std::wstring)arg, __FUNCTION__ , __FILE__, __LINE__)

enum enjocErrors
{
	err_bad_arguments = -1,
	err_out_of_memory = -2,
	err_bad_state = -3,
	err_null_pointer = -4,
	err_load_image = -5,
	err_save_image = -6,
	err_creating_file_for_write = -7,
	err_renaming_file = -8,
	err_opening_file_for_append = -9,
	err_append_file = -10,
	err_not_found_config_value = -11,
	err_feature_not_implemented = -12,
	err_descriptor_size = -13,
	err_opening_file = -14,
	err_descriptor_format = -15,
	err_no_kpt_desc_file_loaded = -16,
	err_no_files_found = -17,
	err_out_of_bounds = -18,
	err_compute_fundamental_matrix = -19,
	err_bad_matrix_shape_or_type = -20,
	err_calc_F = -21,
	err_bad_matrix_shape = -22,
	err_loadingwavfile = -23,
	err_wavunsupportedBitFormat = -24,
	err_wavunsupportedFloatFormat = -25,
	err_wavunsupportedPCMFormat = -26,
	err_readingwavdata = -27,
	err_savingwavfile = -28,
	err_writingAudioOutFile = -29,
	err_wavNumberofChannelsNotSupported = 30,
	err_ini_wsa = 31,
	err_ini_sck = 32,
	err_conn_sck = 33,
	err_ini_db_con_driver = 34,
	err_ini_db_con = 35,
	err_ini_db_prepStatement = 36,
	err_db_get_queryrecordset = 37
};

#define TEXTERR_err_no_Text L"NO error text defined"
#define TEXTERR_err_bad_arguments L"Bad arguments"
#define TEXTERR_err_out_of_memory L"Out of memory"
#define TEXTERR_err_bad_state L"Bad state"
#define TEXTERR_err_null_pointer L"Null pointer"
#define TEXTERR_err_load_image L"Load image - "
#define TEXTERR_err_err_save_image L"Save image - "
#define TEXTERR_err_creating_file_for_write L"Create file for write - "
#define TEXTERR_err_renaming_file L"Rename file - "
#define TEXTERR_err_opening_file_for_append L"Opening file for append - "
#define TEXTERR_err_append_file L"Append - "
#define TEXTERR_err_not_found_config_value L"Not found config value - "
#define TEXTERR_err_feature_not_implemented L"Feature NOT implemented"
#define TEXTERR_err_descriptor_size L"Invalid descriptor size"
#define TEXTERR_err_opening_file L"Opening file"
#define TEXTERR_err_descriptor_format L"Descriptor format"
#define TEXTERR_err_no_kpt_desc_file_loaded L"no keypoint or description file loaded"
#define TEXTERR_err_no_files_found L"No files found"
#define TEXTERR_err_out_of_bounds L"out of bounds"
#define TEXTERR_err_compute_fundamental_matrix L"compute fundamental matrix"
#define TEXTERR_err_bad_matrix_shape_or_type L"bad matrix shape or type to perform the operation"
#define TEXTERR_err_calc_F L"Calc fundamental matrix"
#define TEXTERR_err_bad_matrix_shape L"Matrix shape not coherent"
#define TEXTERR_err_loadingwavfile L"Loading wav file"
#define TEXTERR_err_wavunsupportedBitFormat L"Unsupported bit format (only float and PCM are accepted)"
#define TEXTERR_err_wavunsupportedFloatFormat L"Unsupported float format (only float 32 bits per sample are accepted)"
#define TEXTERR_err_wavunsupportedPCMFormat L"Unsupported PCM bit format (only PCM 8,16,32 bits per sample are accepted)"
#define TEXTERR_err_readingwavdata  L"Reading wav data"
#define TEXTERR_err_savingwavfile L"Saving wav file"
#define TEXTERR_err_writingAudioOutFile L"Samples write error"
#define TEXTERR_err_wavNumberofChannelsNotSupported L"Number of channels of audio file is NOT supported"
#define TEXTERR_err_ini_wsa L"Initializing WSA (Microsoft sockets)"
#define TEXTERR_err_ini_sck L"Initializing socket"
#define TEXTERR_err_conn_sck L"Connecting socket"
#define TEXTERR_err_ini_db_con_driver L"Initializing DB driver"
#define TEXTERR_err_ini_db_con L"Initializing DB connection"
#define TEXTERR_err_ini_db_prepStatement L"Initializing DB prepared statement"
#define TEXTERR_err_db_get_queryrecordset L"Query DB, invalid recordset"

 
inline std::wstring GetErrorMsg(enjocErrors nErr)
{
	if (nErr == err_bad_arguments){return (std::wstring)TEXTERR_err_bad_arguments;}
	if (nErr == err_out_of_memory){return (std::wstring)TEXTERR_err_bad_arguments;}
	if (nErr == err_bad_state){return (std::wstring)TEXTERR_err_bad_state;}
	if (nErr == err_null_pointer){return (std::wstring)TEXTERR_err_null_pointer;}
	if (nErr == err_load_image){return (std::wstring)TEXTERR_err_load_image;}
	if (nErr == err_save_image){return (std::wstring)TEXTERR_err_err_save_image;}
	if (nErr == err_creating_file_for_write){return (std::wstring)TEXTERR_err_creating_file_for_write;}
	if (nErr == err_renaming_file){return (std::wstring)TEXTERR_err_renaming_file;}
	if (nErr == err_opening_file_for_append){return (std::wstring)TEXTERR_err_opening_file_for_append;}
	if (nErr == err_append_file){return (std::wstring)TEXTERR_err_append_file;}
	if (nErr == err_not_found_config_value){return (std::wstring)TEXTERR_err_not_found_config_value;}
	if (nErr == err_feature_not_implemented){return (std::wstring)TEXTERR_err_feature_not_implemented;}
	if (nErr == err_descriptor_size){return (std::wstring)TEXTERR_err_descriptor_size;}
	if (nErr == err_opening_file){return (std::wstring)TEXTERR_err_opening_file;}
	if (nErr == err_descriptor_format){return (std::wstring)TEXTERR_err_descriptor_format;}
	if (nErr == err_no_kpt_desc_file_loaded){return (std::wstring)TEXTERR_err_no_kpt_desc_file_loaded;}
	if (nErr == err_no_files_found){return (std::wstring)TEXTERR_err_no_files_found;}
	if (nErr == err_out_of_bounds){return (std::wstring)TEXTERR_err_out_of_bounds;}	
	if (nErr == err_compute_fundamental_matrix){return (std::wstring)TEXTERR_err_compute_fundamental_matrix;}	
	if (nErr == err_bad_matrix_shape_or_type){return (std::wstring)TEXTERR_err_bad_matrix_shape_or_type;}	
	if (nErr == err_calc_F){return (std::wstring)TEXTERR_err_calc_F;}	
	if (nErr == err_bad_matrix_shape){return (std::wstring)TEXTERR_err_bad_matrix_shape;}	
	if (nErr == err_loadingwavfile){return (std::wstring)TEXTERR_err_loadingwavfile;}	
	if (nErr == err_wavunsupportedBitFormat){return (std::wstring)TEXTERR_err_wavunsupportedBitFormat;}	
	if (nErr == err_wavunsupportedFloatFormat){return (std::wstring)TEXTERR_err_wavunsupportedFloatFormat;}	
	if (nErr == err_wavunsupportedPCMFormat){return (std::wstring)TEXTERR_err_wavunsupportedPCMFormat;}	
	if (nErr == err_readingwavdata){return (std::wstring)TEXTERR_err_readingwavdata;}	
	if (nErr == err_savingwavfile ){return (std::wstring)TEXTERR_err_savingwavfile ;}	
	if (nErr == err_writingAudioOutFile){return (std::wstring)TEXTERR_err_writingAudioOutFile;}	
	if (nErr == err_wavNumberofChannelsNotSupported){return (std::wstring)TEXTERR_err_wavNumberofChannelsNotSupported ;}	
	if (nErr == err_ini_wsa){return (std::wstring)TEXTERR_err_ini_wsa ;}	
	if (nErr == err_ini_sck){return (std::wstring)TEXTERR_err_ini_sck ;}	
	if (nErr == err_conn_sck){return (std::wstring)TEXTERR_err_conn_sck ;}
	if (nErr == err_ini_db_con_driver){return (std::wstring)TEXTERR_err_ini_db_con_driver ;}
	if (nErr == err_ini_db_con){return (std::wstring)TEXTERR_err_ini_db_con ;}
	if (nErr == err_ini_db_prepStatement){return (std::wstring)TEXTERR_err_ini_db_prepStatement;}
	if (nErr == err_db_get_queryrecordset){return (std::wstring)TEXTERR_err_db_get_queryrecordset;}
	
				 
	return (std::wstring)TEXTERR_err_no_Text;
}

