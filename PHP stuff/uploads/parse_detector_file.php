<?php

error_reporting(E_ERROR | E_PARSE);
#include('../header.php');
include('utilities.inc');
include('utilities_pho.inc');
$util=new utilities();
$util_pho = new utilities_pho();
//================= parse flash ========================//

function is_float_num($num) {
	
	return  preg_match('/^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?$/',trim($num));
	
	//return preg_match('/^[+-]?(([0-9]+)|([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)|(([0-9]+|([0-9]*\.[0-9]+|[0-9]+\.[0-9]*))[eE][+-]?[0-9]+))$/',$num);
	
	//return ereg('^[+-]?[0-9]+$',$num);
}
function is_int_num($num) {
	return preg_match('/^[+-]?[0-9]+$/',$num);
}


function type_guess($line) {
	
	if (is_float_num($line) || strtolower($line) =='nan' || strtolower($line) == 'inf') {
		if (is_int_num($line)) {
			return 'INT';
		}
		return 'FLOAT';
	}
	
	if (is_date($line)) {
		return 'DATETIME';
	}
	
	
	if (strlen($line) <= 256) {
		return 'VARCHAR(255)';
	}
	
	return 'TEXT';
}


function is_date($str)
{
  $stamp = strtotime( $str );

  if (!is_numeric($stamp))
  {
     return FALSE;
  }
  $month = date( 'm', $stamp );
  $day   = date( 'd', $stamp );
  $year  = date( 'Y', $stamp );

  if (checkdate($month, $day, $year))
  {
     return TRUE;
  }
 
  return FALSE;
} 

function file_get_header_array(&$handle,$start,$stop,$delim) {
	global $util;
	global $util_pho;
		$string = fgetcsv($handle,0,$delim);
		$header = array();

		while(!feof($handle) && !stristr($string[0],$start) && $start != '') {
			$string = fgetcsv($handle,0,$delim);

		}

		while(!feof($handle) && !stristr($string[0],$stop)) {	
			
			$key = $string[0];
			$util_pho->db_safe_name($string[0]);
			$val = addslashes(trim($string[1]));

			$header[$key]=array('val'=>$val,'type'=>type_guess($val));
			if (stristr($key,"date")) {
				$header[$key]['val'] =  "01012001";
				#print_r($header);
				#$util->parse_datetime($val);
			}
			$string = fgetcsv($handle,0,$delim);
		}
		$key = "date";
		$util_pho->db_safe_name($string[0]);
		$val = addslashes(trim($string[1]));

		$header[$key]=array('val'=>$val,'type'=>type_guess($val));

		
		return $header;
}

function file_get_data_array(&$handle,$start='',$start_after='',$stop='',$num=0,$row_col = 'row',$delim,$field_def = array()) {

	$string = fgetcsv($handle,0,$delim);

	$out = array();
	
	$skip = ($start_after!=''?$start_after:$start);
	
	while(!feof($handle) && $string[0] != $start && $start != '') {
		$string = fgetcsv($handle,0,$delim);
	}

	$count = 0;
	$data  =array();

	while(!feof($handle) && !stristr($string[0],$stop)) {
		if ($stop != '') {
			if(stristr($string[0],$stop)) break;
		}
		if ($num != 0) {
			
			if($count > $num) break;
		}

		$data[] = $string;
		$string = fgetcsv($handle,0,$delim);
		$count = $count+1;
	}
	#if (count($string) > 0 && $string[0] != '') {
	#	$data[] = $string;
	#}

	$out = array();
	
	if ($row_col == 'col') {
		$data = array_transpose_2d($data);
	}
	
	if (empty($field_def)) {
		foreach ($data as $d) {
			$out[array_shift($d)] = $d;
		}
	}
	if (!empty($field_def)) {
		foreach ($data as $d) {
			$out[array_shift($field_def)] = $d;
		}
	}
	
	
	return $out;
}

function array_transpose_2d($array){
#print_r($array);
	$ret = array();
	foreach($array as $key => $value) {
		foreach($value as $key2=>$value2) {
			$ret[$key2][$key] = $value2;
		}
	}
	
	return $ret;
}
#THIS IS WHERE TO GO CHANGE ONCE WE FIX FLASH FILES TO DETECTOR FILES
function convert_header_to_pho_db_compatible($array,$operator,$id){
	$array['session_date'] = $array['Session Date'];
	$array['measurement_date'] = $array['Date'];
	$array['input_current_ma'] = $array['Isotype Scalar'];
	$array['incident_power_density_mw'] = $array['Corrected Irradiance (W/cm_sq)'];
	$array['incident_power_mw'] = $array['P_In (W)'];
	$array['ambient_temperature'] = $array['Ambient Temperature (degrees C)'];
	$array['chuck_temperature'] = $array['Chuck Temperature (degrees C)'];
	$array['detector_measurement_recipe_id']['val'] = $array['Detector Measurement Recipe ID'];
	$array['reverse_breakdown']['val'] = $array['Reverse Breakdown'];
	$array['comments'] = $array['Comment'];
	#CHANGE THIS WHEN FINISHED
	$array['device_id']['val'] = $id;
	$array['operator']['val'] = $operator;
	unset($array['Session Date']);
	unset($array['Date']);
	unset($array['Isotype Scalar']);
	unset($array['Corrected Irradiance']);
	unset($array['P_In (W)']);
	unset($array['Ambient Temperature']);
	unset($array['Chuck Temperature']);
	unset($array['Comment']);
	return $array;
}

function convert_data_to_pho_db_compatible($array){
	$array = array_map(function($tag) {
    return array(
        'voltage_v' => $tag['V'],
        'current_ma' => $tag['I']
    );
}, $array);
	return $array;
}


function get_device_id($x, $y, $wafername){
	global $util;
	global $util_pho;
	
	$query = "SELECT wafer_id FROM epi_wafer WHERE wafer_name = '" . $wafername . "'";
	$wafer_id = $util->local_query($query)[0]['wafer_id'];

	$query = "SELECT device_mask_id FROM epi_wafer WHERE wafer_id = " .$wafer_id;
	$mask_id = $util->local_query($query)[0]['device_mask_id'];

	$query = "SELECT detector_part_id FROM detector_mask_info WHERE x_loc= " . $x ." AND y_loc= "  . $y ." AND mask_id= " . $mask_id;	
	$part_id = $util_pho->local_query($query)[0]['detector_part_id'];

	$query = "SELECT device_id FROM relations_table WHERE part_id= " . $part_id ." AND wafer_id= "  . $wafer_id;
	$device_id = $util_pho->local_query($query)[0]['device_id'];
	
	return $device_id;
}

function parse_flash($data,$file){
	global $util;
	global $util_pho;
	
	$init = false;
	
 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	$header = file_get_header_array($handle,'','Voc Fit (V)',",");

	$dat = file_get_data_array($handle,'V','P','',3,'row',",",array());

	
	$header_table = 'detector_measurement_parameters';
	$header_table_id = 'detector_sweep_measurement_id'; 
	$data_table = 'detector_measurement_liv_data';
	
	$header_fields = $util_pho->get_table_columns($header_table);
	$data_fields = 	$util_pho->get_table_columns($data_table);

	#$util->echo_r($header_fields);
	#$util->echo_r($data_fields);
	
	$header_skip = array('detector_sweep_measurement_id');
	$header_fields = array_diff($header_fields,$header_skip);

	#$util->echo_r($header);
	#$util->echo_r($dat);
	#print_r($data);
	$data_skip = array('detector_raw_data_id','detector_sweep_measurement_id');
	$data_fields = array_diff($data_fields,$data_skip);

	$id = get_device_id($header['X Coord']['val'], $header['Y Coord']['val'], $data['sample']);
	$header = convert_header_to_pho_db_compatible($header,$data['user'],9804);
	#$util->echo_r($header);

	
	$query = "insert into $header_table set ";
	foreach ($header_fields as $f ) {
		$query.= "`$f` = '".addslashes($header[$f]['val'])."',";
	}
	$query .= " detector_sweep_measurement_id = ''";

	$result = $util_pho->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$query = "SELECT max(detector_sweep_measurement_id) FROM detector_measurement_parameters";
	$detector_sweep_measurement_id = $util_pho->local_query($query)[0]['max(detector_sweep_measurement_id)'];


	if($detector_sweep_measurement_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$tmp = array_transpose_2d($dat);
	$tmp = convert_data_to_pho_db_compatible($tmp);
	#$util->echo_r($tmp);

	foreach ($tmp as $t) {
		$query = "INSERT into `$data_table` set ";
		foreach ($data_fields as $f) {
			$query .= " `$f` = '".$t[$f]."',";
		}
		$query .= " `$header_table_id` = '".$detector_sweep_measurement_id."'";
		#echo "<br> $query";

		$result = $util_pho->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}
	
	return  array('error'=>0,'data_id'=>$detector_sweep_measurement_id);

}


$file = 'C:\Users\sswifter\Desktop\Detector Data\SJC_ENG_370_4-GaAs\A-3680-3-A\IV\Open_Diode_70\0 mW\A-3680-3-A_sswifter_Flash-Open_Diode_70-072418_103216.8AM-_updated.csv';
$data = $util->file_name_extract($file);
$handle = fopen($file, "r");
$out = parse_flash($data,$file);
print_r($out)
#echo "<hr>";
#print_r($data);
#$handle = fopen($file, "r");
#$header = file_get_header_array($handle,'','Efficiency',",");

#file_get_data_array(&$handle,$start='',$start_after='',$stop='',$num=0,$row_col = 'row',$delim,$field_def = array())
?>