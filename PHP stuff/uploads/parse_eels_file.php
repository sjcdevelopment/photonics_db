<?php

#error_reporting(E_ERROR | E_PARSE);
#include('../header.php');
include('utilities.inc');
include('utilities_pho.inc');
$util=new utilities();
$util_pho = new utilities_pho();

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

		while(!feof($handle) && $string[0] != $stop) {	
			
			$key = $string[0];
			$util_pho->db_safe_name($string[0]);
			$val = addslashes(trim($string[1]));

			$header[$key]=array('val'=>$val,'type'=>type_guess($val));
		
			$string = fgetcsv($handle,0,$delim);
			
			
		}
		$key = $string[0];
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

	while(!feof($handle) && $string[0] != $stop) {
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

function convert_header_to_pho_db_compatible_eels($array,$operator,$id){
	$array['session_date'] = $array['Session Date'];
	$array['measurement_date'] = $array['Measurement Date'];
	$array['threshold_current_ma'] = $array['Threshold Current (mA)'];
	$array['threshold_voltage_v'] = $array['Threshold Voltage (V)'];
	$array['slope_efficiency'] = $array['Slope Efficiency'];
	$array['temperature'] = $array['Temperature'];
	$array['liv_measurement_recipe_id'] = $array['Measurement Recipe ID'];
	$array['comments'] = $array['Comments'];
	$array['device_id']['val'] = $id;
	$array['operator']['val'] = $operator;
	unset($array['Session Date']);
	unset($array['Measurement Date']);
	unset($array['Threshold Currrent (mA)']);
	unset($array['Threshold Voltage (V)']);
	unset($array['Slope Efficiency']);
	unset($array['Temperature']);
	unset($array['Measurement Recipe ID']);
	unset($array['Comments']);
	return $array;
}

function convert_data_to_pho_db_compatible_eels($array){
	$array = array_map(function($tag) {
    return array(
        'voltage_v' => $tag['V'],
        'current_ma' => $tag['I'],
        'power_mw' => $tag['P']
    );
}, $array);
	return $array;
}


function get_eels_device_id($data){
	global $util;
	global $util_pho;
	
	$wafername = $data['sample'];

	

	$query = "SELECT wafer_id FROM epi_wafer WHERE wafer_name = '" . $wafername . "'";
	$wafer_id = $util->local_query($query)[0]['wafer_id'];

	$query = "SELECT device_mask_id FROM epi_wafer WHERE wafer_id = " .$wafer_id;
	$mask_id = $util->local_query($query)[0]['device_mask_id'];

	$query = "SELECT eels_part_id FROM eels_mask_info WHERE part_name= '" . $data['part_name'] . "' AND mask_id= " . $mask_id;	
	$part_id = $util_pho->local_query($query)[0]['eels_part_id'];

	$query = "SELECT device_id FROM relations_table WHERE part_id= " . $part_id ." AND wafer_id= "  . $wafer_id;
	$device_id = $util_pho->local_query($query)[0]['device_id'];

	return $device_id;
}

function parse_eels_liv($data,$file){
	global $util;
	global $util_pho;
	
	$query = "SELECT filename_id FROM uploaded_filenames WHERE filename='".basename($file)."'";

	$result = $util_pho->local_query($query);
	if($result != NULL ) {
		return array('error'=>4,'point'=>'Previously Uploaded');
	}
	$init = false;

 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	$header = file_get_header_array($handle,'','Comments',",");
	$dat = file_get_data_array($handle,'V','','',0,'row',",",array());

	
	$header_table = 'laser_liv_measurement_parameters';
	$header_table_id = 'liv_measurement_id'; 
	$data_table = 'laser_liv_measurement_raw_data';
	
	$header_fields = $util_pho->get_table_columns($header_table);
	$data_fields = 	$util_pho->get_table_columns($data_table);

	#$util->echo_r($header_fields);
	#$util->echo_r($data_fields);
	
	$header_skip = array('liv_measurement_id');
	$header_fields = array_diff($header_fields,$header_skip);

	#$util->echo_r($header);
	#$util->echo_r($dat);
	#print_r($data);
	$data_skip = array('liv_raw_data_id','liv_measurement_id');
	$data_fields = array_diff($data_fields,$data_skip);

	$id = get_eels_device_id($data);
	$header = convert_header_to_pho_db_compatible_eels($header,$data['user'],$id);
	#$util->echo_r($header_fields);
	
	$query = "insert into $header_table set ";
	foreach ($header_fields as $f ) {
		$query.= "`$f` = '".addslashes($header[$f]['val'])."',";
	}
	$query .= " liv_measurement_id = ''";

	$result = $util_pho->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$query = "SELECT max(detector_sweep_measurement_id) FROM detector_measurement_parameters";
	$liv_measurement_id = $util_pho->local_query($query)[0]['max(detector_sweep_measurement_id)'];


	if($liv_measurement_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$tmp = array_transpose_2d($dat);
	$tmp = convert_data_to_pho_db_compatible_eels($tmp);
	#$util->echo_r($tmp);

	foreach ($tmp as $t) {
		$query = "INSERT into `$data_table` set ";
		foreach ($data_fields as $f) {
			$query .= " `$f` = '".$t[$f]."',";
		}
		$query .= " `$header_table_id` = '".$liv_measurement_id."'";

		$result = $util_pho->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}
	
	$query = "INSERT into uploaded_filenames SET filename = '" . basename($file) ."'";

	$util_pho->local_insert($query);
	return  array('error'=>0,'data_id'=>$liv_measurement_id);

}





$file = 'C:\Users\sswifter\Desktop\TGR-163-1_sswifter_Laser-0-0-1-15-1500-090618_094721.3AM-.csv';
$data = $util_pho->eels_file_name_extract($file);
#$handle = fopen($file, "r");
$result = parse_eels_liv($data,$file);
$util_pho->echo_r($result)



?>