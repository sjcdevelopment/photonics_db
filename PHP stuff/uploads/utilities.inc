<?php
/**
 *=-----------------------------------------------------------=
 * utilities.inc
 *=-----------------------------------------------------------=
 * Author: Ilya Fushman 2008-08-08
 *
 * 
 *
 */
require_once('dbmanager.inc');
#require_once('constants.inc');
date_default_timezone_set('America/Los_Angeles');

/**
 *=-----------------------------------------------------------=
 * DBparse
 *=-----------------------------------------------------------=
 * This class is a single-instance class that parses the MySQL tables and generates
 * The input file to the table manager
 *
 *
 */
 
class utilities
{
 	/* MISC FUNCTIONS */
 	
 	function local_insert($query) {
 		
 		$conn = DBManager::getConnection();
 		
 		$result = $conn->query($query);
 		
 		return $result;
 	}
 	
 	function last_insert() {
 		$conn = DBManager::getConnection();
 		return $conn->insert_id;
 	}

 	function local_query($query)
	{
	 	$conn = DBManager::getConnection();
 		$result = $conn->query($query);
		if ($result) {
			$arr = array();
 			while(($row = $result->fetch_assoc()) != NULL ) {
 				$arr[]=$row;
 			}	
 			if (count($arr) > 0) {
 				return $arr;
 			}
		}
 		return NULL;
	}
 	
 	function local_create($query)
	{
	 	$conn = DBManager::getConnection();
 		return $conn->query($query);
	}
 	/* do url redirection from php via a script or http redirect */
 	
 	function url_redirect($url) {
 		if (stristr($url,'http') || stristr($url,'www')) {
 		} else {
 			$url=SJ_HOST_NAME.$url;
		}
		echo '<script type="text/javascript">';
		//echo 'window.location.href="'.$url.'";';
		echo 'window.location="'.$url.'";';	
		echo '</script>';
		echo '<noscript>';
		echo '<meta http-equiv="refresh" content="0;url='.$url.'" />';
		echo '</noscript>';
 	
 	}
 	
 	
 	 function url_redirect_rel($url) {
 		
		echo '<script type="text/javascript">';
		//echo 'window.location.href="'.$url.'";';
		echo 'window.location="'.$url.'";';	
		echo '</script>';
		echo '<noscript>';
		echo '<meta http-equiv="refresh" content="0;url='.$url.'" />';
		echo '</noscript>';
 	
 	}
 	
 	
 	/* returns a list of options from the array in the form <opts value = "value" >value</opts> */
 	public function get_array_options_list($opts,$selected = NULL) {
 	
 		array_walk($opts, create_function('&$v,$k', '$v = "<option value=\"$v\">".$v."</option>";'));
 		if ($selected !== NULL ){
 			$opts=str_replace('value="'.$selected.'"','value="'.$selected.'" selected="SELECTED" ',$opts);
 		}
 		
 		return $opts;
 	}

 	
 	/* returns a list of options from the array in the form <opts value = "value" >key</opts> */
 	public function get_array_options_list_kv($opts,$selected = NULL) {
 	
 		array_walk($opts, create_function('&$v,$k', '$v = "<option value=\"$v\">".$k."</option>";'));	
 		
 		if ($selected !== NULL ){
 			$opts=str_replace('value="'.$selected.'"','value="'.$selected.'" selected="SELECTED" ',$opts);
 		}
 		
 		return $opts;
 	}

	
	/* returns a list of options from the array in the form <opts value = "value" >key</opts> */
	public function get_array_list_kv($opts) {
 	
 		foreach($opts as $key=>$value) {
 			$arr[]="$key: $value";
 		
 		}
 		
 		
 		
 		return $arr;
 	}


 	
	public function getFilterType($mysql_datatype) {
	
	/* returns the format for the mysql_datatype to be used in the ext js filters plugin */

		$mysql_datatype=strtolower($mysql_datatype);

		if (strpos($mysql_datatype,'int') !== false || 
			strpos($mysql_datatype,'float') !== false||
			strpos($mysql_datatype,'double') !== false
			) {
			return 'numeric';	
		}
	
		if (strpos($mysql_datatype,'date') !== false) {
			return 'date';
		}
	
		return 'string';
	}

	//get list of foreign table values for search / filter.
	public function getFilterValues($table,$column) {
		
		$conn = DBManager::getConnection();
		$query="select $column from $table";
	
		$result=$conn->query($query);
	
		while(($row = $result->fetch_assoc()) !== NULL ) {
			$arr[]=$row[$column];
		}
	
		$result->close();
		return $arr;	
	}

        public function get_table_column_type($table, $column)
        {
                $conn = DBManager::getConnection();
                $query="show columns from $table where field like '$column'";
                //echo $query;
                $result=$conn->query($query);
                while(($row = $result->fetch_assoc()) !== NULL ) {
                        $arr[]=$row['Type'];
                }
                //print_r($arr);
                return $arr[0];
        }

	public function get_table_columns($table)
	{
		$conn = DBManager::getConnection();
		$query="show columns from $table";
		echo $query;
		$result=$conn->query($query);
		while(($row = $result->fetch_assoc()) !== NULL ) {
			$arr[]=$row['Field'];
		}
		print_r($arr);
		return $arr;
	}

  	public function get_table_values2($local_key,$foreign_key,$foreign_table) {
	
		$conn = DBManager::getConnection();
		
		$query  = "SELECT $local_key, $foreign_key FROM $foreign_table order by $local_key desc";
		
		$result =$conn->query($query);
	
		while($row = $result->fetch_assoc())
		{
			
			$arr[$row[$local_key]]=$row[$foreign_key];
		}
		
		$result->close();
		//$conn->close();
		return $arr;

	}
	
	
	function get_table_values($local_key,$foreign_key,$foreign_table) {

		$conn = DBManager::getConnection();
		
		$query  = "SELECT $local_key, $foreign_key FROM $foreign_table order by $local_key desc";
		
		$result = $conn->query($query);
	
		while($row = $result->fetch_assoc())
		{
			$arr[]=$row[$foreign_key];
		}
		print_r($junctions);
		$result->close();
	
		//$conn->close();
		return $arr;
	}
	
	
	/* FUNCTIONS FOR MANAGING WAFERS AND GROWTH */
  	/*
  	 *
  	 *
  	 * 
  	*/
  	/* gets all the wafers that are available for measurement */
  	function get_measurement_wafers($growth_id=NULL) {
  		$where='';
  		if ($growth_id !==NULL) {
  			$where="and g.growth_id = $growth_id ";
  		}
  		
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT g.wafer_id, gr.chamber_id, w.wafer_name, g.growth_id
					FROM epi_growth_wafer_relation g, epi_growth gr, epi_wafer w
					WHERE gr.growth_id = g.growth_id
					AND w.wafer_id = g.wafer_id
					$where 
					ORDER BY w.date_created DESC, gr.chamber_id DESC , gr.growth_run DESC";
		//	echo $query;
		$result = $conn->query($query);
  	
  		while($row = $result->fetch_assoc())
		{
			$arr[]=array('id'=>$row['wafer_id'],'value'=>$row['wafer_name']);
		}
		
		return $arr;
  	
  	}
  
 /*
   	function get_measurement_wafers($growth_id=NULL) {
  		$where='';
  		if ($growth_id !==NULL) {
  			$where="and g.growth_id = $growth_id ";
  		}
  		
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT g.wafer_id, w.wafer_name, g.growth_id
					FROM epi_growth_wafer_relation g, epi_wafer w
					WHERE w.wafer_id = g.wafer_id
					$where 
					ORDER BY w.date_created DESC";
			
		$result = $conn->query($query);
  	
  		while($row = $result->fetch_assoc())
		{
			$arr[]=array('id'=>$row['wafer_id'],'value'=>$row['wafer_name']);
		}
		
		return $arr;
  	
  	}*/
  
 
  
  /* gets all the wafers that are available for measurement as a dropdown*/
  	function get_measurement_wafers_html($selected=NULL,$disabled=NULL) {
  		
		$result = $this->get_measurement_wafers();
		
		//print_r($result);
		$html='<select name="wafer_id" '.$disabled.'>';
		foreach($result as $res) {
			$select ='';
			if ($res['id']==$selected) {
				
				$select="selected=\"selected\"";
			}
			$html.='<option value="'.$res['id'].'" '.$select.'>'.$res['value'].'</option>';
			
		}
  		$html.='</select>';
  		
  	return $html;
  	
  	}
  	
  	function get_regrowths($growth_id){
  		
  		$query = "select growth_id2 as growth_id from epi_growth_list where growth_id1= $growth_id";
  		
  		$result = $this->local_query($query);
  		
  		$g = array();
  		foreach ($result as $r) {
  			$g[]=$r['growth_id'];
  		}
  		
  		return $g;
  		
  	}
  	
  	function get_parent_regrowths($growth_id){
  		
  		$query = "select growth_id1 as growth_id from epi_growth_list where growth_id2= $growth_id";
  		$result = $this->local_query($query);
  		return $result[0]['growth_id'];
  		
  	}
  	
  	function get_growth_type($growth_id){
  		$query = "select type from epi_growth where growth_id = $growth_id";
  		$result = $this->local_query($query);
  		return $result[0]['type'];
  	}
  	
  	function set_growth_type($type,$growth_id) {
  		$query = "update epi_growth set type = '".$type."' where growth_id = $growth_id";
  		$result = $this->local_insert($query);
  		return $result;
  	}
  	
  	/* FUNCTIONS FOR MANAGING */
  	
  	
  	/* get all the users that can make measurements from the epi_user table */

	function get_open_projects($customer_name)
	{
		$query = "SELECT P.id AS project_id, P.project_number ";
		$query .= "FROM customer_projects P INNER JOIN epi_customer C ON (P.customer_id = C.id AND P.status = 'open') ";
		$query .= "WHERE C.name LIKE '$customer_name'";
		return $this->local_query($query);
	}

	function get_open_project_numbers($customer_id)
	{
		$query = "SELECT * FROM customer_projects WHERE customer_id = $customer_id AND status LIKE 'open'";
		return $this->local_query($query);
	}

  	function get_order_numbers()
	{
		$query  = "SELECT * FROM customer_projects ORDER BY id ASC";
		$result = $this->local_query($query);
  		$html="<option value='0'></option>";
		foreach ($result as $k=>$v) {
  			$html .= "<option value='".$v['id']."'>".$v['project_number']."</option>";
		}

		return $html;
  	}

  	function get_customer_options3()
	{
		$conn = DBManager::getConnection();
		$query  = "SELECT * FROM epi_customer ORDER BY id ASC";
		$result = $conn->query($query);
  		$html="<option value=\"\">SELECT</option>\n";
  		while($row = $result->fetch_assoc()) {
			$html.="<option value=\"".$row['id']."\">".$row['name']."</option>\n";
		}
		return $html;
  	}

  	function get_customer_options2()
	{
		$conn = DBManager::getConnection();
		$query  = "SELECT * FROM epi_customer ORDER BY id ASC";
		$result = $conn->query($query);
  		$html="<option value='0'></option>";
  		while($row = $result->fetch_assoc()) {
			$html.="<option value='".$row['id']."-".$row['name']."'>".$row['name']."</option>";
		}
		return $html;
  	}

  	function get_customer_options()
	{
		$conn = DBManager::getConnection();
		$query  = "SELECT * FROM epi_customer ORDER BY id ASC";
		$result = $conn->query($query);
  		$html="<option value='0'>SELECT</option>";
  		while($row = $result->fetch_assoc()) {
			$html.="<option value='".$row['name']."'>".$row['name']."</option>";
		}
		return $html;
  	}

  	function get_meas_user_options()
	{
		$conn = DBManager::getConnection();
		$query  = "SELECT * FROM epi_user ORDER BY user_id ASC";
		$result = $conn->query($query);
  		$html='';
  		while($row = $result->fetch_assoc()) {
			$html.='<option value="'.$row['user_id'].'">'.$row['user_name'].'</option>';
		}
		return $html;
  	}
  	
  	function get_epi_users()
	{
			$conn = DBManager::getConnection();
			$query  = "SELECT * FROM epi_user ORDER BY user_id ASC";
			$result = $conn->query($query);
  		$html='';
  		while($row = $result->fetch_assoc()) {
				$html.='<option value="'.$row['user_name'].'"> '.$row['user_name'].' </option>';
			}
			return $html;
  	}
  	
  	function get_users()
	{
  		$query  = "SELECT * from epi_user order by user_id asc";
  		$result = $this->local_query($query);
  		return $result;
  	}
  	
  	
  	function get_user_id_by_name($user_name) {
  		$query = "select user_id from epi_user where user_name = '".strtolower($user_name)."'";
  	  	$result = $this->local_query($query);
  		return $result[0]['user_id'];
  	}

  	function get_user_by_id($user_id)
	{
  		$query = "SELECT * FROM epi_user WHERE user_id = $user_id";
  	  	$result = $this->local_query($query);
  		return $result[0];
  	}

  	function get_users_options(){
  		$query  = "SELECT * from epi_user order by user_id asc";
  		$result = $this->local_query($query);
  		$html ='';
  		foreach($result as $row) {
  			$html.='<option value="'.$row['user_name'].'">'.$row['user_name'].'</option>';
  		}
  		return $html;
  	}

  	function get_epi_user_full_name_options()
	{
  		$query  = "SELECT * FROM epi_user WHERE last_name NOT LIKE '' ORDER BY user_id ASC, first_name ASC, last_name ASC";
  		$result = $this->local_query($query);
  		$html ="<option value=''>SELECT</option>";
  		foreach($result as $row) {
  			$html.="<option value='".$row['user_id']."'>".$row['first_name']." ".$row['last_name']."</option>";
  		}
  		return $html;
  	}

  	function get_mbe_grower_options() {
  		
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT * from epi_growers order by grower_id asc";
		//	echo $query;
		$result = $conn->query($query);
  		$html='';
  		while($row = $result->fetch_assoc())
		{
			$html.='<option value="'.$row['grower_id'].'">'.$row['grower_name'].'</option>';
		}
		
		return $html;
  	
  	}
  	
  	function get_grower_name_by_id($grower_id) {
  		$query  = "select grower_name from epi_growers where grower_id = $grower_id";
 
  		$result = $this->local_query($query);
  		return $result[0]['grower_name'];
  	}
  	
  	
  	/* get all the measurements */

  	function get_meas_options(){
  		return '<option value="PLMEAS"> PL </option>
			<option value="HALL"> HALL </option>';
  	}
  	
  	/* get all the processes */
  	function get_proc_options(){
  	/* may be defunct */
  		return '<option value="RTA"> RTA </option>
  				<option value="RTA"> EPI </option>';
  	}
  	
  	function get_process_options(){
  		return '<option value="RTA"> RTA </option>';
 	}

  	
  	/* get result of RTA measurements */
  	
  	function get_rta_param($rta_meas_id,$param){
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT $param from epi_rta_meas_values where rta_meas_id =$rta_meas_id ";
		$result = $conn->query($query);
		while(($row = $result->fetch_assoc()) !==NULL )
		{
			$arr[]=$row[$param];
		}
  		
  		return $arr;
  	
  	}
  	
  	function get_ecv_param($ecv_meas_id,$param){
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT $param from epi_ecv_meas_values where ecv_meas_id =$ecv_meas_id ";
		$result = $conn->query($query);
		while(($row = $result->fetch_assoc()) !==NULL )
		{
			$arr[]=$row[$param];
		}
  		
  		return $arr;
  	
  	}
  	
  	
  	
  	function get_pl_param($pl_meas_id,$param){
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT $param from epi_pl_meas_values where pl_meas_id =$pl_meas_id ";
		$result = $conn->query($query);
		while(($row = $result->fetch_assoc()) !==NULL )
		{
			$arr[]=$row[$param];
		}
  		
  		return $arr;
  	
  	}
  	function get_qe_param($qe_meas_id,$param)
	{
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT $param from epi_qe_meas_values where qe_meas_id =$qe_meas_id ";
		$result = $conn->query($query);
		while(($row = $result->fetch_assoc()) !== NULL ) {
			$arr[]=$row[$param];
		}
  		
  		return $arr;
  	
  	}
  	function get_cv_param($cv_meas_id,$param){
  		$conn = DBManager::getConnection();
		
		$query  = "SELECT $param from epi_cv_meas_values where cv_meas_id =$cv_meas_id ";
		$result = $conn->query($query);
		while(($row = $result->fetch_assoc()) !==NULL )
		{
			$arr[]=$row[$param];
		}
  		
  		return $arr;
  	
  	}
  	
  	
  	function get_new_layer_number($growth_id){
  		$conn = DBManager::getConnection();
		$query  = "SELECT layer_number from epi_growth_layers where growth_id = $growth_id order by layer_number desc LIMIT 1";
		//echo $query;
		$result=$conn->query($query);
			
		$row = $result->fetch_assoc();
	
		if ($row == NULL ) {
			return 1;
		} else {
			return $row['layer_number']+1;
		}
		
  	
  	}	
  
    function get_new_structure_layer_number($structure_id){
  		$conn = DBManager::getConnection();
		$query  = "SELECT layer_number from epi_structure_layers where structure_id = $structure_id order by layer_number desc LIMIT 1";
		//echo $query;
		$result=$conn->query($query);
			
		$row = $result->fetch_assoc();
	
		if ($row == NULL ) {
			return 1;
		} else {
			return $row['layer_number']+1;
		}
		
  	
  	}
  	
  	function copy_structure_layers($old_structure_id,$new_structure_id) {
  			$conn = DBManager::getConnection();
  			$query = "select * from epi_structure_layers where structure_id = $old_structure_id";
  			//echo "<br> $query";
  			$result=$conn->query($query);
  			while(($row = $result->fetch_assoc()) != NULL ){
  				unset($row['structure_layer_id']);
  				$row['structure_id']=$new_structure_id;
  				$row['status']='Pending';
  				$fields = array_keys($row);
  				$values = array_values($row);
  				$query2="insert into epi_structure_layers (".implode(",",$fields).")
  							values  ('".implode("','",$values)."')";
  				$result2=$conn->query($query2);
  				//echo "<br> $query2";
  			}
  	}

  	function structure_status($structure_id) {
  		$conn = DBManager::getConnection();
  		$query = "select status from epi_structure where structure_id = $structure_id";
  		//echo "status $query";
  		$result=$conn->query($query);
  		if (($row = $result->fetch_assoc()) != NULL) {
  			return $row['status'];
  		}
  		return NULL;
  	}
  	
  	function new_structure_notify($structure_id) 
	{
  		$msg =  "A new structure has been entered and is awaiting your approval. View the structure ".
			"here\n\n ".EPI_BASE_URL."/edit/epi_structure.php?structure_id=$structure_id";

  		$headers = 'From: webmaster@sj-solar.com' . "\r\n" .
			   'Reply-To: '.BEH_ADMIN.'@sj-solar.com' . "\r\n" .
    			   'X-Mailer: PHP/' . phpversion();
  	}
  	
  	function get_new_structure_version($structure_id) {
  		$conn = DBManager::getConnection();
		/* create this later - basically autoincrement structure version where we have particular structure_name */
  	}
  	
  	
  	function send_simple_email($from,$to,$title,$email) {
  			$headers = "From: $from \n" .
			"MIME-Version: 1.0\n" .
			"Content-type: text/html; charset=iso-8859-1";
			
			
			$email_msg = "\n\n ".$email;
		
			$date = time();		
			
		
			
			$to = "data@sj-solar.com";
			if (@mail($to,$title,$email_msg,$headers)) {
				return TRUE;
			} else {
				return FALSE;
			}
	}
  	
  
  	// set requirements on options array
	function require_numeric(&$opts,$column,$num_type='int')
	{
		if ($num_type == 'float') {
			$opts['fdd'][$column]['js']['regexp']='/^-?\d*(\.\d+)?([eE][-+]?\d+)?$/';
			$opts['fdd'][$column]['js']['hint']=$opts['fdd'][$column]['name'].' must be numeric';
		}
		else {
  			$opts['fdd'][$column]['js']['regexp']='/^-?\d*$/';
  			$opts['fdd'][$column]['js']['hint']=$opts['fdd'][$column]['name'].' must be an integer';
  	}
	}
  
  	function require_user_select(&$opts,$column) {
  		$opts['fdd'][$column]['js']['regexp']='/[^1]/';
		$opts['fdd'][$column]['js']['hint']='You must select a '.$opts['fdd'][$column]['name'];
  	}

  	function require_select(&$opts,$column)
		{
  		$opts['fdd'][$column]['js']['regexp']='/[^<?SELECT?>]/';
			$opts['fdd'][$column]['js']['hint']='You must select a '.$opts['fdd'][$column]['name'];
  	}

  	function require_user_name_select2(&$opts,$column)
		{
  		$opts['fdd'][$column]['js']['regexp']='/[^<?SELECT?>]/';
			$opts['fdd'][$column]['js']['hint']='You must select a '.$opts['fdd'][$column]['name'];
  	}

	function exclude_value_selection(&$opts,$column,$value) {
  		$opts['fdd'][$column]['js']['regexp']='/[^<?'.$value.'?>]/';
		$opts['fdd'][$column]['js']['hint']='You must select a valid '.$opts['fdd'][$column]['name'];
  	}

  	function require_user_name_select(&$opts,$column,$pat) {
  		$opts['fdd'][$column]['js']['regexp']=$pat;
		$opts['fdd'][$column]['js']['hint']='You must select a '.$opts['fdd'][$column]['name'];
  	}

  	
  	
  	// gets all the substrate wafers for a dropdown selection
  	
  	
  	function get_substrate_wafers_select($form="PME_sys_form",$select_name="substrate_select",$current_value = NULL) {
  		
  		
  		$conn = DBManager::getConnection();
		$query="select s.substrate_id as substrate_id,
					   s.ingot_number as ingot_number,
					   s.substrate_name as name,
					   s.substrate_type as type,
					   s.substrate_diameter as diameter,
					   s.substrate_thickness as thickness,
					   s.substrate_grade as grade,
					   s.substrate_EPD as EPD,
					   s.substrate_polished as polished,
					   s.substrate_dopant as dopant_type,
					   s.doping_level as doping_level,
					   s.orientation as orientation,
					   s.offcut_angle as angle,
					   s.offcut_direction as direction,
					   s.alpha_angle as angle,
					   s.number_left as number_left,
					   s.number_loaded as number_loaded,
					   s.notes as notes,
					   s.doped as doped,
					   v.name as vendor
					   from epi_substrate s, epi_substrate_vendor v
					   where s.number_left > -1000 
					   and v.substrate_vendor_id=s.substrate_vendor_id
					   order by  v.name asc, s.substrate_name asc, s.substrate_diameter asc, s.substrate_dopant desc, s.substrate_EPD asc, s.number_left desc";
		
		$result = $conn->query($query);
		
		$headers=array('vendor','ingot_number','name','type','diameter','doped','number_loaded','number_left','thickness','grade','EPD','polished','dopant_type','doping_level',
		'orientation','angle','direction','angle');
		
		$html.='<table class="pme-main">';
		
		$html.='<tr class="pme-row-0"><td class="pme-key-0">Select</td><td class="pme-key-0">'.implode('</td><td class="pme-key-0">',$headers).'</td></tr>';
		$htmlRows='';
		while (($row=$result->fetch_assoc()) !== NULL ) {
		
			$htmlRow='<tr class="pme-row-0">';
		//	$html.='<td class="pme-key-0"><input type="radio"  onClick="'.$form.'.'.$select_name.'.value="'.$row['substrate_id'].'"></td>';
			
			$checked='';
			if ($row['substrate_id'] == $current_value ) {
				$checked=' checked ';
			}
			
		
			$htmlRow.='<td class="pme-key-0"><input type="radio"  name="'.$select_name.'" value="'.$row['substrate_id'].'"  '.$checked.' ></td>';
		//'.$row['substrate_id'].'
			foreach($headers as $field) {
				$htmlRow.='<td class="pme-key-0">'.htmlspecialchars($row[$field]).'</td>';
			}
		
			$htmlRow.='</tr>';
		
			if ($row['substrate_id'] == $current_value ) {
				$firstRow=$htmlRow;
			} else {
				$htmlRows.=$htmlRow;
			}
		
		}
		$html.=$firstRow.$htmlRows;
		$html.='</table>';
		
		$html='<div style="overflow:scroll; height:300px;">'.$html.'</div>';
		
		return $html;
  	
  	
  	}
  	
  	function get_dopants() {
  		$query = "select dopant_name from epi_dopant";
  		$result =$this->local_query($query);
  		if ($result != NULL ){
  		  	foreach ($result as $r) {
  				$arr[]=$r['dopant_name'];
  			}
			return $arr;
  		}
  		return NULL;
  	}

  	function get_doping_type() {
  		$query = "select type from epi_doping";
  		$result =$this->local_query($query);
  		if ($result != NULL ){
  			foreach ($result as $r) {
  				$arr[]=$r['type'];
  			}
			return $arr;
  		}
  		return NULL;
  	}


  	function get_substrate_type() {
  		$query = "select type from epi_substrate_type";
  		$result =$this->local_query($query);
  		if ($result != NULL ){
  			foreach ($result as $r) {
  				$arr[]=$r['type'];
  			}
			return $arr;
  		}
  		return NULL;
  	}

  	
  	// get quick descriptor about substrate
  	function get_substrate_name_by_id($substrate_id) {
  		
  		
  		$conn = DBManager::getConnection();
		//$query="select s.substrate_id as substrate_id,
		
			$query="select v.name as vendor,
						   s.doped as doped,
						   s.substrate_type as type,
						   s.substrate_diameter as diameter,
						   s.substrate_dopant as dopant_type,
						   s.doping_level as doping_level,
						   s.number_left as number_left,
						   s.notes as notes
						   
						   
					   from 
					   		epi_substrate s, epi_substrate_vendor v, epi_doping d
					   where 
						   s.substrate_id =$substrate_id
						   and s.number_left > 0
						   and v.substrate_vendor_id=s.substrate_vendor_id
						   order by  v.name asc,  s.substrate_diameter asc, s.substrate_dopant desc, s.substrate_EPD asc, s.number_left desc";
			$result = $conn->query($query);
			if (($row=$result->fetch_assoc()) !== NULL ) {
				return implode(',',$row);
			
			} else {
				return NULL;
			}
		
	}
  	
  	
  	// get growth name e.g. SJ1-4 by growth id
  	
  	function growth_name_by_id($growth_id) {
  		$conn = DBManager::getConnection();
  		$query = "select g.growth_run as growth_run, 
  				 g.chamber_id as chamber_name 
  				  from epi_growth g
  				  where g.growth_id=$growth_id 
  				  ";
  		//echo $query;
  		$result = $conn->query($query);
// edited by mkursunlu  	        
		if ($result != NULL){	
  		  if (($row=$result->fetch_assoc()) !== NULL ) {
  		 	  return $row['chamber_name'].'-'.$row['growth_run'];
  		  } else {
	  		  return NULL;
  		  }
		  }
//********************
  	}

  	
  	  	// get growth name e.g. SJ1-4 by growth id
  	
  	function get_structure_name_by_id($structure_id) {
  		$conn = DBManager::getConnection();
  		$query = "select structure_name as structure_name from epi_structure where structure_id = $structure_id";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) != NULL ) {
  			return $row['structure_name'];
  		} else {
	  		return NULL;
  		}
  	}
  	
  	function get_structure_name_by_wafer($wid)
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT structure_name FROM epi_wafer WHERE wafer_id = $wid";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) != NULL ) {
  			return $row['structure_name'];
  		} else {
	  		return NULL;
  		}
  	}
  	
  	function get_process_lot_number_by_wafer($wid)
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT process_lot_number ".
			 "FROM epi_process_lot_wafer ".
			 "WHERE wafer_id = $wid AND process_lot_number LIKE 'ARC%' ".
			 "LIMIT 1";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) != NULL ) {
  			return $row['process_lot_number'];
  		} else {
	  		return NULL;
  		}
  	}

  	function get_structure_description($structure_id) {
  		$conn = DBManager::getConnection();
  		$query = "select structure_description as structure_description from epi_structure where structure_id = $structure_id";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) != NULL ) {
  			return $row['structure_description'];
  		} else {
	  		return NULL;
  		}
  	}
  	
  	//get srtucture for growth
  	
 	function get_structure_id_by_growth_id($growth_id)
	{ 
  		$query = "SELECT structure_id AS structure_id ".
			 "FROM epi_growth_structure_relation ".
			 "WHERE growth_id = $growth_id";

  		$result = $this->local_query($query);
  		
  		if ($result == NULL) return 0;
  		
  		return $result[0]['structure_id'];
	}
  	
// spc charts
	function valid_spc_add_forms($data)
	{
		return true;
	}

	function add_spc_chart($data)
	{
		if ($this->valid_spc_add_forms()) {	
			//$this->echo_r($query);
			$spc_chart = array();
			$spc_chart['name'] = addslashes(trim($_POST['spc_hname']));
			$spc_chart['description'] = addslashes(trim($_POST['spc_hdescription']));
			$spc_chart['user_id'] = addslashes(trim($_POST['spc_hauthor']));
			$spc_chart['date'] = addslashes(trim($_POST['spc_hdate']));
			$spc_chart['confidence'] = addslashes(trim($_POST['spc_hconfidence']));
			$spc_chart['rows_limit'] = addslashes(trim($_POST['spc_hlimit']));
			$spc_chart['table'] = addslashes(trim($_POST['spc_htable']));
			$spc_chart['variables'] = addslashes(trim($_POST['spc_hcolumns']));
			$spc_chart['filters'] = addslashes(trim($data['filters']));

			//$this->echo_r($spc_chart);

			$fields = array();
			$fields = array();
			foreach($spc_chart as $k=>$v) {
				$fields[] = "`$k`";
				$values[] = "'$v'";
			}
			$query = "INSERT INTO `epi_spc_chart` (".implode(", ", $fields).") VALUES (".implode(", ", $values).");";
			//echo "<p>query: $query</p>";
			$result = $this->local_insert($query);
			if ($result == 1) {
				return 1; // success
			}
			return 0;  // failure
		}
		return -1; // invalid forms
	}

	function add_spc_query($data)
	{
	$err_msg = "";
	if (empty($data['spc_name'])) {
	  $err_msg .= "<br><p> - invalid query name</p>";
	}

	if (empty($data['spc_description'])) {
	  $err_msg .= "<br><p> - invalid description</p>";
	}

	if (empty($data['spc_table'])) {
	  $err_msg .= "<br><p> - invalid database table</p>";
	}

	if (count($data['spc_columns']) == 0) {
	  $err_msg .= "<br><p> - invalid table columns</p>";
	}

	if ($data['spc_chamber'] == 0) {
	  $err_msg .= "<br><p> - invalid growth chamber</p>";
	}

	if (empty($data['spc_structure'])) {
	  $err_msg .= "<br><p> - invalid structure</p>";
	}

	if (empty($data['spc_layer'])) {
	  $err_msg .= "<br><p> - invalid layer</p>";
	}

	if (empty($data['spc_confidence'])) {
	  $err_msg .= "<br><p> - invalid confidence</p>";
	}

	if (!empty($data['spc_confidence']) && ($data['spc_confidence'] <= 0)) {
	  $err_msg .= "<br><p> - confidence must be greater than 0</p>";
	}

	if (empty($data['spc_limit'])) {
	  $err_msg .= "<br><p> - invalid rows limit</p>";
	}

	if (!empty($data['spc_limit']) && ($data['spc_limit'] > 100)) {
	  $err_msg .= "<br><p> - rows limit must not be greater than 100</p>";
	}

	if ($data['spc_author'] == 1) {
	  $err_msg .= "<br><p> - invalid user</p>";
	}

	if(empty($data['spc_date'])) {
	  $err_msg .= "<br><p> - invalid date</p>";
	}

	if (!empty($err_msg)) {
	  $err_msg  = "Input errors:<br><p style='padding-left:20px;'>$err_msg</p><br>";
          $err_msg .= "Please provide valid inputs and try again ...";
	  return $err_msg;
	}

	$spc_name = addslashes(trim($data['spc_name']));
	$spc_description = addslashes(trim($data['spc_description']));
	$spc_table = addslashes(trim($data['spc_table']));
	$spc_columns = $data['spc_columns'];

	$chambers = array('SELECT', 'A', 'B');
	$spc_chamber = $chambers[$data['spc_chamber']];

	// parse structure names
	$spc_structure = addslashes(trim($data['spc_structure']));
	$tmp = explode(",", $data['spc_structure']);
		$spc_structures = array();
		foreach ($tmp as $t) {
			$t = str_replace("*", "%", trim($t));
			$spc_structures[] = "structure LIKE ''".addslashes($t)."''";
		}

		// parse layer names
		$spc_layer = addslashes(trim($data['spc_layer']));
		$tmp = explode(",", $data['spc_layer']);
		$spc_layers = array();
		foreach ($tmp as $t) {
			$t = str_replace("*", "%", trim($t));
			$spc_layers[] = "layer_name LIKE ''".addslashes($t)."''";
		}
		//$spc_layer =  implode(" OR ", $spc_layers); 

	$spc_confidence = addslashes(trim($data['spc_confidence']));
	$spc_limit = addslashes(trim($data['spc_limit']));
	$spc_author = addslashes(trim($data['spc_author']));
	$spc_date = addslashes(trim($data['spc_date']));

	// build query
	$spc_query  = " SELECT growth_name, ";
	$spc_query .= implode(", ", $spc_columns);
	$spc_query .= " FROM $spc_table ";
	$spc_query .= " WHERE growth_name LIKE ''$spc_chamber-%'' ";
	$spc_query .= "   AND (".implode(" OR ", $spc_structures).") ";
	$spc_query .= "   AND (".implode(" OR ", $spc_layers).") ";
	$spc_query .= " ORDER BY mbe_id DESC ";
	//echo "<p>SPC Sub Query: $spc_query</p>";

	$spc_table_columns = implode(", ", $spc_columns);
	$query  = "INSERT INTO epi_spc (name, description, query, epi_table, epi_table_columns, growth_chamber, structure, layer, confidence, result_set_limit, author, date_created) ";
	$query .= "VALUES ('$spc_name', '$spc_description', '$spc_query', '$spc_table', '$spc_table_columns', '$spc_chamber', '$spc_structure', '$spc_layer', '$spc_confidence', '$spc_limit', '$spc_author', '$spc_date');";

	// echo "<p>Query: $query</p>";
	$result = $this->local_insert($query);
	//echo "<p>Result: $result</p>";

return $err_msg;
}

	function update_spc_chart_note()
	{
		$content = addslashes(trim($_POST['note']));
		$variable = addslashes(trim($_POST['variable']));
		$spc_chart_id = addslashes(trim($_POST['spc_chart_id']));
		$mbe_meas_id = addslashes(trim($_POST['mbe_meas_id']));

                $query  = "UPDATE `epi_spc_chart_note` SET `content` = '$content' ";
               	$query .= "WHERE `variable` LIKE '$variable' AND `spc_chart_id` = $spc_chart_id AND `mbe_meas_id` = $mbe_meas_id";
	//echo "<p>Query: $query</p>";
		$result = $this->local_insert($query);
	//echo "<p>Update Result: $result</p>";
                return $result;
	}

        function add_spc_chart_note()
        {
		$content = addslashes(trim($_POST['note']));
		$variable = addslashes(trim($_POST['variable']));
		$spc_chart_id = addslashes(trim($_POST['spc_chart_id']));
		$mbe_meas_id = addslashes(trim($_POST['mbe_meas_id']));

                $query  = "INSERT INTO `epi_spc_chart_note` (`content`, `variable`, `spc_chart_id`, `mbe_meas_id`) ";
               	$query .= "VALUES ('$content', '$variable', $spc_chart_id, $mbe_meas_id)";
		$result = $this->local_insert($query);

                return $result;
        }

        function get_spc_chart_note($variable, $spc_chart_id, $mbe_meas_id)
        {
                $query  = "SELECT * FROM `epi_spc_chart_note` ";
		$query .= "WHERE `variable` LIKE '$variable'";
		$query .= " AND `spc_chart_id` = $spc_chart_id";
		$query .= " AND `mbe_meas_id` = $mbe_meas_id";
	//echo "<p>Query: $query</p>";
		$result = $this->local_query($query);
	//echo "<p>Result: $result</p>";
                return $result[0];
        }

	function get_spc_report_recipients($category)
	{
		$query  = "SELECT DISTINCT U.user_name ";
		$query .= "FROM epi_admin_process_categories C ";
		$query .= "INNER JOIN epi_admin_process_categories_users CU ";
		$query .= "  ON (C.category_id = CU.category_id) ";
		$query .= "INNER JOIN epi_user U ";
		$query .= "  ON (U.user_id = CU.user_id) ";
		$query .= "WHERE C.category LIKE '$category'";
		$result = $this->local_query($query);

		//echo $query;

		return $result;
	}

        function build_sub_query1($q)
        {
                $sub_query  = " SELECT mbe_meas_id, growth_name, layer_number, layer_name ";
                $sub_query .= " FROM ". $q['table'];
                $sub_query .= " WHERE ".$q['filters'];
                $sub_query .= " ORDER BY mbe_id DESC ";

                return $sub_query;
        }

        function build_sub_query($q, $c)
        {
                $sub_query  = " SELECT mbe_meas_id, growth_name, layer_number, layer_name, $c ";
                $sub_query .= " FROM ". $q['table'];
                $sub_query .= " WHERE ".$q['filters'];
                $sub_query .= " ORDER BY mbe_id DESC ";

                return $sub_query;
        }

	function run_spc_query($sub_query, $col, $confidence)
	{
  		//echo "<p>$sub_query </p><br>";

		$query = "SELECT ";
		$query .= "count(*) AS result_set_size, ";
		$query .= "growth_name, ";
		$query .= "mbe_meas_id, ";
		$query .= "layer_number, ";
		$query .= "layer_name, ";
		$query .= "$confidence AS confidence, ";
		$query .= "$col AS DATA, ";
		$query .= "AVG(S.$col) AS 'AVG', ";
		$query .= "SUM(S.$col) AS 'SUM', ";
		$query .= "STD(S.$col) AS 'STD', ";
		$query .= "(AVG(S.$col) - ($confidence * (STD(S.$col)/SQRT(count(*))))) AS 'LCL', ";
		$query .= "(AVG(S.$col) + ($confidence * (STD(S.$col)/SQRT(count(*))))) AS 'UCL' ";
		$query .= "FROM ($sub_query) S";

  		//echo "<p>$query </p><br>";
		$result = $this->local_query($query);
		//$this->echo_r($result);
		return $result[0];
	}

	function get_spc_chart_queries()
	{
		$query = "SELECT * FROM epi_spc_chart ORDER BY id DESC";
		$result = $this->local_query($query);
		return $result;
	}

	function get_spc_chart_query($query_id)
	{
		$query = "SELECT * FROM epi_spc_chart WHERE id = $query_id";
		$result = $this->local_query($query);
		return $result;
	}

	function get_spc_queries()
	{
		$query = "SELECT * FROM epi_spc ORDER BY id DESC";
		$result = $this->local_query($query);
		return $result;
	}

	function get_spc_query($query_id)
	{
		$query = "SELECT * FROM epi_spc WHERE id = $query_id";
		$result = $this->local_query($query);
		return $result;
	}

function send_spc_report($to, $subject, $message)
{
	// To send HTML mail, the Content-type header must be set
	$headers  = 'MIME-Version: 1.0' . "\r\n";
	$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
	$headers .= "X-Mailer: PHP"."\r\n";

	// Additional headers
	$headers .= 'From: Behemoth Admin <'.BEH_ADMIN.'@sj-solar.com>' . "\r\n";
	//$headers .= 'Bcc: '.BEH_ADMIN.'@sj-solar.com' . "\r\n";
	$headers .= "\n\n";
	
	//$headers = 'From: webmaster@sj-solar.com' . "\r\n" .
	//	'Reply-To: '.BEH_ADMIN.'@sj-solar.com' . "\r\n" .
	//	'X-Mailer: PHP/' . phpversion();
	
	// Mail it
	//$to = "data@sj-solar.com";
	$mailsent = mail($to, $subject, $message, $headers);
	return $mailsent;
}

// spc charts

	function get_wafer_growth_per_lot($filters, $order_by, $limit_to = 100)
        {
		$query  = "SELECT w.wafer_id, w.wafer_name, g.growth_id, growth_name, m.device_mask_id, m.device_mask, l.id lot_id, l.name lot_name ";
		$query .= "FROM epi_wafer w ";
		$query .= "INNER JOIN epi_device_mask m ON m.device_mask_id = w.device_mask_id ";
		$query .= "INNER JOIN epi_growth g ON g.growth_id = w.growth_id ";
		$query .= "INNER JOIN manufacturing_lot_wafer lw ON lw.wafer_id = w.wafer_id ";
		$query .= "INNER JOIN manufacturing_lot l ON (l.type='PROCESS' AND l.id = lw.lot_id) ";
		$query .= "WHERE w.wafer_name NOT LIKE '%-p%' ";
		if (!empty($filters)) {
                	$query .= "AND ($filters) ";
		}
		if ($order_by == "process lot") {
                	$query .= "ORDER BY l.id DESC, g.growth_id ASC, w.wafer_id ASC ";
		}
		else {
                	$query .= "ORDER BY g.growth_id DESC, w.wafer_id ASC, l.id ASC ";
		}
                $query .= "LIMIT $limit_to";
                $result = $this->local_query($query);
                return $result;
	}

	function get_wafer_growth2($filters="")
        {
		$query  = "SELECT wafer_id, wafer_name as Wafer, g.growth_id, growth_name as Growth, m.device_mask_id, m.device_mask ";
		$query .= "FROM ((epi_wafer w INNER JOIN epi_device_mask m ON w.device_mask_id = m.device_mask_id) ";
		$query .= "INNER JOIN epi_growth g ON g.growth_id = w.growth_id) ";
		$query .= "WHERE wafer_name NOT LIKE '%-p%' ";
                $query .= "AND ($filters) ";
                $query .= "ORDER BY wafer_id DESC LIMIT 100";
                $result = $this->local_query($query);
                return $result;
	}

	function get_wafer_growth($filters="")
        {
                $query  = "SELECT Growth, Wafer FROM epi_wafer_growth_structure_view ";
                $query .= "WHERE (Wafer NOT LIKE '%-p%') ";
                $query .= "AND ($filters) ";
                $query .= "ORDER BY wafer_id DESC";
                $result = $this->local_query($query);
                return $result;
        }

  	function get_wafers_by_growth_id($growth_id) {
  		$query = "select wafer_id,wafer_name,status from epi_wafer where growth_id = $growth_id order by wafer_id asc";
  		$result = $this->local_query($query);
  		return $result;
  	}
  	
  	  	//get mbe for growth
  	
  	function get_mbe_id_by_growth_id($growth_id){ 
  		$query = "select mbe_id as mbe_id from epi_growth_mbe_relation where growth_id = $growth_id";
  		$result = $this->local_query($query);
  		
  		if ($result == NULL ) return 0;
  		
  		return $result[0]['mbe_id'];
  	
  	}
  	
  	function get_mbe_id_by_growth_id_multiple($growth_id){ 
  		$query = "select mbe_id as mbe_id from epi_growth_mbe_relation where growth_id in ($growth_id)";
  		
  		$result = $this->local_query($query);
  		
  		if ($result == NULL ) return 0;
  		$ret = array();
  		foreach ($result as $r) {
  			$ret[] = $r['mbe_id'];
  		}
		return $ret;  	
  	}
  	
  	 function get_growth_id_by_mbe_id($mbe_id){ 
  		$query = "select growth_id as growth_id from epi_growth_mbe_relation where mbe_id = $mbe_id";
  		$result = $this->local_query($query);
  		
  		if ($result == NULL ) return 0;
  		
  		return $result[0]['growth_id'];
  	
  	}
  	
  	// get the number of wafers corresponding to a particular growth.
  	
  	function get_wafer_number_by_growth_id($growth_id,$piece=TRUE) {
  		$conn = DBManager::getConnection();
  		
  		if (!$piece) {
  			$piece = 0;
  		} else {
  			$piece = 1;
  		}
  		$query = "select count(*) as count from epi_growth_wafer_relation g, epi_wafer w where g.growth_id = $growth_id and w.wafer_id = g.wafer_id and w.piece = $piece";
  		
  		//echo "$query";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) !== NULL ) {
				return $row['count'];
		} else {
				return 0;
		}
  		
  	
  	}
  	
  	
  	//get the wafer name by wafer id ...
  	/*function get_wafer_name_by_id($wafer_id) {
  		$conn = DBManager::getConnection();
  		$query = "select g.chamber_id as chamber,
  						g.growth_run as growth_run,
  						w.wafer_number as wafer_number
  					from epi_growth as g, epi_growth_wafer_relation as w
  					where w.growth_id=g.growth_id and w.wafer_id = $wafer_id
  					order by w.date asc
  				  ";
  					
  						
  		$result = $conn->query($query);
  		$wafer_name = '';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
				$wafer_name.=implode('-',$row);
		} 
  		return $wafer_name;
  	
  	}*/
  	
  	function get_wafer_by_status($status,$search = '') {
  		
  		$where = '';
  		if ($search != '' ) {
  			$where = ' AND (';
  			$tmp = explode(',',$search);
  			$w = array();
  			foreach ($tmp as $s) {
  					$w[] = "wafer_name LIKE '%".addslashes(trim($s))."%'";
  			}
  			$where .= implode(' OR ',$w).')';
  			
  		}
  		
  		
  		$query = "select * from epi_wafer where status = '$status' ".$where." order by wafer_id desc ";
  		
  		//echo "<br> $query";
  		
  		$result = $this->local_query($query);
  		return $result;
  	
  	}
  	
	function jsc_eqe($wafer_name, $junction)
        {
                $query  = "SELECT m.qe_meas_id, m.junction, m.jsc_am15d_eqe, t.tag_id, t2.name as tag_name ";
                $query .= "FROM epi_qe_meas m ";
		$query .= " LEFT JOIN epi_tag_meas t ON m.qe_meas_id = t.meas_id ";
                $query .= " LEFT JOIN epi_tag t2 ON t.tag_id = t2.id ";
                $query .= "WHERE m.wafer_name LIKE '$wafer_name' AND m.junction LIKE '$junction' ";
                $query .= "ORDER BY m.qe_meas_id DESC, m.junction ASC";
 		$result = $this->local_query($query);
		
 		return $result;
        }

  	function get_device_masks()
	{
 		$query ="SELECT device_mask_id as id, device_mask as name FROM epi_device_mask ORDER BY device_mask ";
 		$result = $this->local_query($query);
 		return $result;
 	}
  	
  	/* get wafer device mask info */
  	function get_wafer_device_mask_info($wafer_id)
	{
 		$query ="SELECT m.* ";
 		$query.="FROM epi_wafer w, epi_device_mask m WHERE w.device_mask_id = m.device_mask_id ";
 		$query.="AND w.wafer_id = $wafer_id";
 		$result = $this->local_query($query);
 		return $result[0];
 	}
  	
  	/* get wafer info from view table */
  	function get_wafer_info($wafer_id)
	{
 		$query="SELECT * from epi_wafer_growth_structure_view where wafer_id = $wafer_id";
 		$result = $this->local_query($query);
 		return $result[0];
 	}
  	
  	function get_wafer_name_by_id($wafer_id)
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT wafer_name FROM epi_wafer WHERE wafer_id = $wafer_id";
  		$result = $this->local_query($query);
  		$wafer_name = $result[0]['wafer_name'];
  		
  		return $wafer_name;
  	}
  	
  	function get_wafer_status($wafer_id){
  		$query = "select status from epi_wafer where wafer_id = $wafer_id";
  		$result = $this->local_query($query);
  		return $result[0]['status'];
  	}
  	
  	function get_wafer_next_status($wafer_id) {
  		$procs = $this->process_list();
  		$current = $this->get_wafer_status($wafer_id);
  		
  		$query = "select * from epi_wafer_process_request where wafer_id = $wafer_id and completed = 0";
		
	
		
		$result = $this->local_query($query);

		$proc_order = array_flip($procs);

		$i_current = $proc_order[$current];
		$i_ret = count($proc_order)-1;
	   	
	        if($result != NULL) {
		    foreach ($result as $r){
		  	    $p = $r['process'];
			    $i_p = $proc_order[$p];
			    if ($i_p < $i_ret) {
			 	    $i_ret = $i_p;
			    }
		    }
		}
// *****************************************************
		return $procs[$i_ret];
  	}
  	
  	function process_list(){
  		return array('growth','materials','process','test','available');
  	}

  	function process_order(){
  		$p = $this->process_list();
  		return array_flip($p);
  	}

	function process_category($process_id){ 
			$query = "select category from epi_admin_process_and_measure where process_and_measure_id = $process_id ";
			$result = $this->local_query($query);
			return $result[0]['category'];
	}

	function process_and_measure_name($process_id){ 
			$query = "select name from epi_admin_process_and_measure where process_and_measure_id = $process_id ";
			$result = $this->local_query($query);
			return $result[0]['name'];
	}

	function set_wafer_status($wafer_id,$status)
	{
  		$query = "update epi_wafer set status = '$status' where wafer_id = $wafer_id";
  		$result = $this->local_insert($query);
  		return $result;
  	}

	function set_wafer_device_mask_id($wafer_id,$device_mask_id)
	{
  		$query = "update epi_wafer set device_mask_id = '$device_mask_id' where wafer_id = $wafer_id";
  		$result = $this->local_insert($query);
  		return $result;
  	}

	function set_wafer_manufacturing_stage_id($wafer_id, $release_type)
	{
		switch (strtolower($release_type)) {
			case "process":
				$stage_id = "0201"; // initial queue front-fab
				break;

			case "test":
				$stage_id = "0301";  // initial queue test
				break;

			default:
				$stage_id = "NULL";  // unknown manufacturing stage
				break;
		}
  		$query = "update epi_wafer set manufacturing_stage_id = '$stage_id' where wafer_id = $wafer_id";
  		$result = $this->local_insert($query);
  		return $result;
  	}

  	function get_wafer_scrap_cause($wafer_id){
  		$query = "select result_cause from epi_wafer_traveler where wafer_id = $wafer_id and result = 'scrap'";
  		$result = $this->local_query($query);
  		return $result[0]['result_cause'];
  	}
  	  	
  	
  	function get_wafer_chambers_by_id($wafer_id) {
  		$conn = DBManager::getConnection();
  		$query = "select g.chamber_id as chamber,
  						g.growth_run as growth_run,
  						w.wafer_number as wafer_number
  					from epi_growth as g, epi_growth_wafer_relation as w
  					where w.growth_id=g.growth_id and w.wafer_id = $wafer_id
  					order by w.date asc
  				  ";
  					
  						
  		$result = $conn->query($query);
  		$chambers = '';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
				$chambers[]=$row['chamber'];
		} 
  		return $chambers;
  	
  	}
  	
  	
  	function update_wafer_names(){
  		/*$conn = DBManager::getConnection();
  		$query = "select wafer_id, wafer_name from epi_wafer where 1";
  		
  		$result = $conn->query($query);
  		
  		while (($row=$result->fetch_assoc()) !== NULL ) {
				$wafer_name = $this->get_wafer_name_by_id($row['wafer_id']);
				if ($wafer_name !== $row['wafer_name']) {
					$query2 = "update epi_wafer set wafer_name = '$wafer_name' where wafer_id = ".$row['wafer_id'];
					$result2 = $conn->query($query2);
				}
		} */
  	
  	}
  	
  	function update_wafer_names_by_id($wafer_ids){
  		/*$conn = DBManager::getConnection();
  		$wafer_ids = implode(',',$wafer_ids);
  		$query = "select wafer_id, wafer_name from epi_wafer where wafer_id in($wafer_ids)";
  		
  		$result = $conn->query($query);
  		
  		while (($row=$result->fetch_assoc()) !== NULL ) {
				$wafer_name = $this->get_wafer_name_by_id($row['wafer_id']);
				if ($wafer_name !== $row['wafer_name']) {
					$query2 = "update epi_wafer set wafer_name = '$wafer_name' where wafer_id = ".$row['wafer_id'];
					$result2 = $conn->query($query2);
				}
		} */
  	
  	}
  	
  	
  	function add_wafers_to_regrowth($parent_growth_id,$growth_id){
  		$query = "select growth_run from epi_growth where growth_id =$growth_id";
  		$result = $this->local_query($query);
  		$growth_run = $result[0]['growth_run'];
  		
  		$query = "select * from epi_growth_wafer_relation where growth_id = $parent_growth_id";
  		$result = $this->local_query($query);
  		foreach ($result as $r) {
  			unset($r['growth_wafer_relation_id']);
  			$r['growth_id'] = $growth_id;
  			$r['date'] = date('Y-m-d H:i:s');
  			$r['growth_number'] = $growth_run;
  			$query = 'insert ignore into epi_growth_wafer_relation ('.implode(',',array_keys($r)).') values ("'.implode('","',array_values($r)).'")';
  			$result = $this->local_insert($query);
  		}
  	
  	}
  	
  	function add_regrowth($growth_id,$wafer_id,$wafer_number) {
  		$conn = DBManager::getConnection();
  		// check if this relation does not exist yet
  		$query = "select count(*) as count from epi_growth_wafer_relation where wafer_id =$wafer_id and growth_id =$growth_id";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) !== NULL ) {
  			$count = $row['count'];
  		}
  		if ($count != NULL || $count != 0) {
  			return NULL;
  		}
  		// check if the wafer number is valid
  		$query = "select count(*) as count from epi_growth_wafer_relation where growth_id =$growth_id and wafer_number = $wafer_number";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) !== NULL ) {
  			$count = $row['count'];
  		}
  		if ($count != NULL || $count != 0) {
  			return NULL;
  		}
  		// if all is well do the insertion
  		$date = date("Y-m-d H:i:s");
  		$query = "insert into epi_growth_wafer_relation (growth_id,wafer_id,date,wafer_number) 
  				values  ($growth_id,$wafer_id,$date,$wafer_number)";
  		$result = $conn->query($query);	
  		return $conn->insert_id; 
  	}
  	
  	function get_growth_wafer_numbers($growth_id){
  		$conn = DBManager::getConnection();
  		$query = "select wafer_number from epi_growth_wafer_relation where growth_id = $growth_id";
  		
  		$result = $conn->query($query);
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['wafer_number'];
  		}
  		if (count($arr) > 0 ) {
  			return $arr;
  		}
  		return NULL;
  	}
  	
  	function get_growth_name_by_name($str) {
  		$conn = DBManager::getConnection();
  		$query = "select growth_id as growth_id, CONCAT(chamber_id,'-',growth_run) as growth_name from epi_growth where CONCAT(chamber_id,'-',growth_run) like ('%$str%') order by growth_id asc";
  		$result = $conn->query($query);
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['growth_name'];
  		}
  		return $arr;
  	}



  	
  	function get_wafer_name_by_name($str) {
  		$conn = DBManager::getConnection();
  		$query = "select wafer_name as wafer_name from epi_wafer where wafer_name like ('%$str%') order by wafer_id asc";
  		$result = $conn->query($query);
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['wafer_name'];
  		}
  		return $arr;
  	}
  	
	function get_growth_id_by_name($growth_name) 
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT growth_id ".
		         "FROM epi_growth ".
			 "WHERE CONCAT(chamber_id,'-',growth_run) LIKE ('$growth_name') ".
			 "LIMIT 1";
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) !== NULL )
		{
  		 return $row['growth_id'];
  		}
		else
		{
		 //if it's a 29- growth, we'll add a dummy growth as a placeholder so the upload goes through
		 //in the rare case where IQE sends us a 29 wafers (not grown on 28) - i.e. a single junction, someone from the materials team can correct it later
		 if (stristr($growth_name,'29-')!==FALSE)
		 {
		  //set the platen # to 888
		  //set the growth to 6040J
		  $parts = explode("-",$growth_name);
		  $chamber = $parts[0];
		  $run = $parts[1];
		  $query = "INSERT INTO `epi`.`epi_growth` (`growth_id`, `structure_id`, `grower_id`, `chamber_id`, `mbe_id`, `growth_structure`, `growth_run`, `growth_sequence`, `growth_name`, `growth_wafer_number`, `platen_number`, `description`, `action`, `recipe_name`, `growth_date`, `date_added`, `growth_tc_toxide`, `growth_bandit_toxide`, `growth_tc_tset`, `growth_bandit_tset`, `growth_oxide_des_time`, `growth_stack_id`, `growth_stack`, `growth_type`, `source`, `type`) VALUES (NULL, '1486', '7', '$chamber', '0', '6040J', '$run', '', '$growth_name', '0', '888', '', '', '', '2013-08-13 09:42:00', '2013-08-15 16:04:22', '0', '0', '0', '0', '0.000', '0', '', 'standard', '', 'single');";
		  $result = $conn->query($query);
		  $growth_id = $conn->insert_id;
		  return $growth_id;
		 }
		}
  		return 0;
  	}
	
	function convert_letter_to_number($wafer_name)
	{
	 $underscores = explode("_",$wafer_name);
	 $sections = explode("-",$underscores[0]);
	 $letter = end($sections);
	 if(ctype_alpha($letter))
	 {
	  $letter = strtoupper($letter);
	  $number = ord($letter) - 64;//A is 65, so 65-64 = 1
	  $last = count($sections)-1;
	  $sections[$last] = strval($number);
	  $first_part = implode("-",$sections);
	  $underscores[0] = $first_part;
	  $final = implode("_",$underscores);
	  return $final;
	 }
	 else
	  return $wafer_name;
	}
  	
  	function get_wafer_id_by_name($wafer_name,$user='SJ') {
  		if($user=="iqe")
		{
		 $wafer_name = $this->convert_letter_to_number($wafer_name);
		}
		$conn = DBManager::getConnection();
  		$query = "select wafer_id from epi_wafer where wafer_name like '$wafer_name' LIMIT 1";
  		
  		$result = $conn->query($query);
  		if (($row=$result->fetch_assoc()) !== NULL ) {
  			 return $row['wafer_id'];
  		}
  		return 0;
  	
  	}

  	function get_wafer_id_by_name_closest($wafer_name,$user='SJ')
	{
  		if($user=="iqe")
		{
		 $wafer_name = $this->convert_letter_to_number($wafer_name);
		}
		// find the sample in case we have a wrongly labeled one 
		// for now only handle the case of an annealed wafer not being named properly ... 
		
		$tmp = explode('-',$wafer_name);
		if (count($tmp) == 3 || count($tmp) == 4) {
			
			$wafer_name_test = $wafer_name.'-A';
			$query = "SELECT wafer_id FROM epi_wafer WHERE wafer_name LIKE ('%$wafer_name_test%') LIMIT 1";
			$result = $this->local_query($query);
			if($result == NULL ) {
				return 0;
			} else {
				return $result[0]['wafer_id'];
			}
			
			
			
			
		} else {
			return 0;
		}
  	
  	}
  	
  	function get_growth_chambers()
        {
  		$conn = DBManager::getConnection();
  		$query = "SELECT chamber_name FROM epi_growth_chamber WHERE chamber_name NOT LIKE 'IQE' ORDER BY chamber_id ASC";
  		$result = $conn->query($query);
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['chamber_name'];
  		}
  		return $arr;
  	
  	}
  	
  	function get_growth_wafers($growth_id, $pieces = TRUE)
	{
  		$conn = DBManager::getConnection();
  		$where = '';
  		if (!$pieces ) {
  			$where = " and w.piece = 0 ";
  		}
  		$query = "select g.wafer_number as wafer_number, w.wafer_name as wafer_name ".
			 "from epi_growth_wafer_relation g, epi_wafer w ".
			 "where g.growth_id = $growth_id and w.wafer_id = g.wafer_id ".$where;
  		$result = $conn->query($query);
  		$arr = '';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[$row['wafer_number']] = $row['wafer_name'];
  		}
  		return $arr;
  	}
  	
  	function get_wafer_pieces($wafer_id)
	{
  		$query = "select w.wafer_id, w.wafer_name from epi_wafer w, epi_wafer_pieces p where p.child_id=w.wafer_id and p.parent_id = $wafer_id ";
  		$result =  $this->local_query($query);
  		
  		if ($result != NULL ) {
  			$arr=array();
  			foreach ($result as $row) {
  				$arr[$row['wafer_id']] = $row['wafer_name'];
  			}
  			return $arr;
  		
  		} else {
  			return NULL;
  		}
  	
  	
  	}
  	
  	function get_wafer_for_piece($wafer_id) {
  		$query = "select w.wafer_id, w.wafer_name from epi_wafer w, epi_wafer_pieces p where p.child_id=$wafer_id and p.parent_id = w.wafer_id ";
  		$result =  $this->local_query($query);
  		
  		if ($result != NULL ) {
  			$arr=array();
  			foreach ($result as $row) {
  				$arr[$row['wafer_id']] = $row['wafer_name'];
  			}
  			return $arr;
  		
  		} else {
  			return NULL;
  		}
  	
  	
  	}
  	
  	
  	function get_wafer_growths($wafer_id) {
  		$conn = DBManager::getConnection();
  		$query = "select CONCAT(g.chamber_id,'-',g.growth_run) as growth_name from epi_growth_wafer_relation w, epi_growth g where w.wafer_id = $wafer_id and g.growth_id = w.growth_id order by w.date asc, w.growth_wafer_relation_id asc ";
  		
  		$result = $conn->query($query);
  		$arr = '';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['growth_name'];
  		}
  		return $arr;
  	}

		function get_process_lot_number($wafer_id)
		{
  		$conn = DBManager::getConnection();
        //$query = "SELECT process_lot_number FROM epi_process_lot_wafer WHERE wafer_id = $wafer_id ORDER BY process_lot_id DESC";
      	$query = "SELECT l.name process_lot_number ";
        $query .= "FROM manufacturing_lot l INNER JOIN manufacturing_lot_wafer m ON l.id = m.lot_id ";
        $query .= "WHERE m.wafer_id = $wafer_id";
  		$result = $conn->query($query);
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['process_lot_number'];
  		}
  		return $arr;
    }
  	
  	function get_wafer_lot($wafer_id)
	{
	  $lot_numbers = $this->get_process_lot_number($wafer_id);
	  $lot = '';
      foreach($lot_numbers as $lot_number) {
        // echo "<p>$lot_number</p>";

  		$conn = DBManager::getConnection();
      	//$query = "SELECT wafer_id, wafer_name FROM epi_process_lot_wafer WHERE process_lot_number LIKE '$lot_number'";
      	$query = "SELECT w.wafer_id, w.wafer_name FROM manufacturing_lot l INNER JOIN manufacturing_lot_wafer m ON l.id = m.lot_id ";
        $query .= "INNER JOIN epi_wafer w ON m.wafer_id = w.wafer_id WHERE l.name LIKE '$lot_number'";
  		$result = $conn->query($query);

  		$arr = '';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[$row['wafer_id']] = $row['wafer_name'];
  		}
		$lot[$lot_number] = $arr;
      }
	  return $lot;
  	}

  	  function get_growth_for_wafer($wafer_id) {
  		$conn = DBManager::getConnection();
  		$query = "select g.growth_id as growth_id, CONCAT(g.chamber_id,'-',g.growth_run) as growth_name ".
		  	 "from epi_growth_wafer_relation w, epi_growth g ".
			 "where w.wafer_id = $wafer_id and g.growth_id = w.growth_id order by w.date asc, w.growth_wafer_relation_id asc ";
  		
  		$result = $conn->query($query);
  		$arr = array();
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[$row['growth_id']]=$row['growth_name'];
  		}
  		return $arr;
  	}
  	
  	function get_growth_layers($growth_id) {
  		$conn = DBManager::getConnection();
  		$query = "SELECT layer_number from epi_growth_layers where growth_id = $growth_id order by layer_number asc";
  		
  		$result = $conn->query($query);
  		$arr='';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['layer_number'];
  		}
  		return $arr;
  	}

  	function get_structure_layers($structure_id) {
  		$conn = DBManager::getConnection();
  		$query = "SELECT layer_number from epi_structure_layers where structure_id = $structure_id order by layer_number asc";
  		
  		$result = $conn->query($query);
  		$arr='';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=$row['layer_number'];
  		}
  		return $arr;
  	}

  	
  	function growth_quick_view($growth_id) {
  		$conn = DBManager::getConnection();	
  		$query = "select g.*, gr.* from epi_growth g, epi_growers gr where g.growth_id = $growth_id and g.grower_id = gr.grower_id";
  		$result = $conn->query($query);
  		if (($row = $result->fetch_assoc()) != NULL) {
  
  			$arr['Grower']=$row['grower_name'];
  			$arr['Growth Date']=$row['growth_date'];
  			$arr['Growth Chamber']=$row['chamber_id'];
  			$arr['Growth Run']=$row['growth_run'];
  			$arr['Description']=$row['description'];
  			//$arr['Recipe']=$row['recipe_name'];
  			//print_r($arr);
  			return $arr;
  		}
  		return NULL;
  		
  	}
  	
  	/*
 	function get_growth_source_info($growth_id){
 		$query = "select g.*, e.* from epi_growth g, epi_external e,  where growth_id = $growth_id";
 		$result = $conn->query($query);
 	
 	}
  	*/
  	
  	/* function for getting PO/Growth infor for the wafer */
  	
  	function get_wafer_lot_source_html($wafer_id,$show_pieces = FALSE)
    {

  		/* get the wafer growths for the wafer */
  		
  		$lots  = $this->get_wafer_lot($wafer_id);
  		if (empty($lots))
				return "";
  		
  		$html = "<tr class='simple'><th class='simple'>Lot#</th><th class='simple'>Wafers</th></tr>";
  		foreach ($lots as $number=> $lot) {
				$html .= "<tr class='simple'><td class='simple' style='white-space:nowrap'> $number </td><td>";
  			foreach ($lot as $key=>$val) {
					if ($key != $wafer_id)
	  		  	$html .= '<a href="'.SJ_HOST_NAME.'epi/edit/wafer_center.php?wafer_name='.$val.'">'.$val.'</a> ';	
  			}	
				$html .= "</td></tr>";
  		}	
  		return "<table class='simple'>$html</table><br>";
  	}

 function get_wafer_source_html($wafer_id,$show_pieces = FALSE)
 {
  		/* get the wafer growths for the wafer */
  		
  		$growth  = $this->get_growth_for_wafer($wafer_id);
  		
  		// print_r($growth);
  		$html = '';
  		foreach ($growth as $key=>$val) {
	  		$ow = $this->get_growth_wafers($key, $show_pieces);
  			if (count($ow) > 0) {
  				$owHTML = '';
  				foreach ($ow as $ok=>$ov) {
	  				$owHTML[] = '<a href="'.EPI_BASE_URL.'/edit/wafer_center.php?wafer_name='.$ov.'">'.$ov.'</a>';
  				}
  				$owHTML ='(all wafers for this growth: ' . implode(',', $owHTML).')';
  			} else {
  				$owHTML = '';
  			}
	  		
	  		$html .= '<p><a style="color:blue" href="'.EPI_BASE_URL.'/edit/growth_center.php?growth_id='.$key.'">Growth Name: '.$val.' </a> '.$owHTML.'</p>';	
  			$gi=$this->growth_quick_view($key);
  			
  			$html .='<p><br><table width=750 style="border: solid 1px #CCCCCC; ">';
  			$html .='<tr style="background-color:#E3DFFF;"><td style="border: solid 1px #CCCCCC; padding:3px">';
  			$html .=implode('</td><td style="border: solid 1px #CCCCCC; padding:3px;">',array_keys($gi));
  			$html .='</tr></td>';
  			$html .='<tr><td>';
  			$html .=implode('</td><td style="border: solid 1px #CCCCCC; padding:3px">',array_values($gi));
  			$html .='</tr></td>';
  			$html .='</table><br></p>';
  		}	
  		return $html;
  	}
  	
  	function get_wafers_for_growth($growth_id,$show='all') {
  		
  		if ($show == 'wafers') {
  			$where = ' and w.piece = 0 ';
  		} elseif ($show == 'pieces') {
  			$where = ' and w.piece = 1 ';
  		} else {
  			$where = '';
  		}
  		$query = "select w.wafer_id, w.wafer_name from epi_wafer w, epi_growth_wafer_relation g where w.wafer_id  = g.wafer_id and g.growth_id = $growth_id $where";
  		$result = $this->local_query($query);
  		
  		if ($result == NULL ) {
  			return array();
  		}
  		
  		$arr=array();
  		
  		foreach ($result as $r) {
  			$arr[$r['wafer_id']] = $r['wafer_name'];
  		}
  		return $arr;
  	
  	}
  	
  	
	function get_wafer_process_rta_measure_html($wafer_id)
	{
  		 
  		/* get all RTA data */
  		/* display this in wiki style format with a quick view ... */
  		$pieces = $this->get_wafer_pieces($wafer_id);
  		
  		$meas_table=array('epi_rta_meas');
  		$meas_name=array('RTA');
  		$meas_url = array('parsedata/upload_rta.php');
  		$html='';
  		for ($i=0; $i< count($meas_table); $i++) {
  			
  			$html.='<h2><b>'.$meas_name[$i].'</b>: ';
  			$html.='<a style="color:blue" href="'.SJ_HOST_NAME.'epi/'.$meas_url[$i].'?wafer_id='.$wafer_id.'">Add</a>';
  			$html.='<h2>';
  			$html.='<hr>';
  			$html.='<br>';
  			
  			if($meas_name[$i] == 'RTA') {
		  		$tmp=$this->rta_report_html($wafer_id);
		  		$html .= ($tmp != ''?$tmp."<br>":'');
		  		
		  		$tmp=$this->rta_report_pieces_html($wafer_id);
		  		
		  		if ($tmp != '') {
		  			$html.='The following pieces have been annealed: <br><br>'.$tmp.'<br>';
		  		}
		  		
  			}
  			
  		}
  	
  		return $html;
  	}
  	
function get_wafer_process_sjc_measure_html($wafer_id)
{
    $q  = "SELECT T.wafer_name AS 'wafer name', T.ingot_number AS 'ingot#', ";
    $q .= " W.substrate_sn AS 'substrate', T.status, ";
    $q .= " R.comment AS comments, T.process_lot_number AS 'process lot#', ";
    $q .= " T.process_file AS 'process file', ";
    $q .= " T.process_name AS 'process name', T.date_shipped AS 'date shipped', ";
    $q .= " T.date_started AS 'date started', T.date_received as 'date received', T.notes ";
    $q .= "FROM processed_lot_wafers T INNER JOIN epi_wafer_process_request R ";
    $q .= "ON T.wafer_name = R.wafer_name ";
    $q .= "INNER JOIN epi_wafer W ";
    $q .= "ON W.wafer_id = R.wafer_id ";
    $q .= "WHERE R.process = 'process' AND R.wafer_id = $wafer_id ";

    $res = $this->local_query($q);
    $html='<b>SJC</b> ';
    $html.='<h2>';
    $html.='<hr>';
    $html.='<br>';
    $html .= "<tr class='simple' >";
    foreach ($res[0] as $k=>$v) {
      if (stristr($k,'_id')) continue;
      if (stristr($k,'_coord')) continue;
      if (stristr($k,'_parameters')) continue;
      if (stristr($k,'_header')) continue;

      $html .= "<th  class='simple'>".$k."</th>";
    }
    $html .= "</tr>";

    foreach ($res as $row) {
      $html .= "<tr  class='simple'>";
      foreach ($row as $k=>$v) {
        if (stristr($k,'_id')) continue;
        if (stristr($k,'_coord')) continue;
        if (stristr($k,'_parameters')) continue;
        if (stristr($k,'_header')) continue;

        if (stristr($k,'file')) {
          if (!empty($v)) {
            $tmp = explode("-",$v);
            array_shift($tmp);
            $v2 = implode("-",$tmp);
            $v = str_replace('parsedata/','',$v);
            $html .= '<td class="simple"><a href="../parsedata/'.$v.'">file</a></td>';
          }
          else {
            $html .= '<td class="simple"></td>';
          }
        }
        else if ((stristr($k,'comments')) || (stristr($k,'notes'))) {
          $html .= "<td class='simple'><p style='width:200px;'>".$v."</p></td>";
        }
        else {
          $html .= "<td class='simple'>".$v."</td>";
        }
      }
      $html .= "</tr>";
    }

    return "<table class='simple'>$html</table><br>";
}

	function get_wafer_process_pl_hall_measure_html($wafer_id) {
  		 
  		/* get all the RTA, HALL, PL, IV, EL, QE etc. data */
  		/* display this in wiki style format with a quick view ... */
  		$pieces = $this->get_wafer_pieces($wafer_id);
  		
  		$meas_table=array('epi_pl', 'epi_pl2_meas', 'epi_hall_meas');
  		$meas_name=array('PL', 'PL2', 'HALL');
  		$meas_url = array('parsedata/upload_plmap.php','parsedata/upload_pl2map.php','parsedata/upload_hall.php');
  		$html='';
  		for ($i=0; $i< count($meas_table); $i++) {
  			
  			$html.='<h2>'.$meas_name[$i].': ';
  			$html.='<a style="color:blue" href="'.SJ_HOST_NAME.'epi/'.$meas_url[$i].'?wafer_id='.$wafer_id.'">Add</a>';
  			$html.='<h2>';
  			$html.='<hr>';
  			$html.='<br>';
  			
  			if($meas_name[$i] == 'PL') {
  				$tmp=$this->pl_map_report_all_html($wafer_id);
  				$html .= ($tmp != ''?$tmp."<br>":'');
  				
  				$tmp =  $this->pl_map_report_pieces_anneal_html($wafer_id);
  				if ($tmp != ''){ 
  					$html.='The following PL data is available for pieces: <br><br>'.$tmp.'<br>';
  				}
  				
  			}
  			if($meas_name[$i] == 'PL2') {
  				$tmp=$this->pl2_map_report_all_html($wafer_id);
  				$html .= ($tmp != ''?$tmp."<br>":'');
  				
  				$tmp =  $this->pl2_map_report_pieces_anneal_html($wafer_id);
  				if ($tmp != ''){ 
  					$html.='The following PL2 data is available for pieces: <br><br>'.$tmp.'<br>';
  				}
  				
  			}
  			
  			if($meas_name[$i] == 'HALL') {
		  		$tmp=$this->hall_report_html($wafer_id);
		  		$html .= ($tmp != ''?$tmp."<br>":'');
		  		
		  		$tmp = '';
				if($pieces!=NULL)
				{
				 foreach($pieces as $k=>$v) {
		  			$piece_html = $this->hall_report_html($k);
		  			
		  			if ($piece_html !='') {
		  				$tmp .=$v.'<br>'.$piece_html.'<br>';
		  			}
		  		
				 }
				}

		  		if ($tmp != '') {
		  			$html.='The following pieces have Hall data:<br><br>'.$tmp;
		  		
		  		}
		  		
  			}
  		
  		}
  	
  		return $html;
  	}
  	
  	
	function get_wafer_process_measure_html($wafer_id)
	{
  		 
  		/* get all the RTA, HALL, PL, IV, EL, QE etc. data */
  		/* display this in wiki style format with a quick view ... */
  		$pieces = $this->get_wafer_pieces($wafer_id);
  		
  		$meas_table=array('epi_rta_meas','epi_pl','epi_hall_meas');
  		$meas_name=array('RTA', 'PL', 'HALL');
  		$meas_url = array('parsedata/upload_rta.php','parsedata/upload_plmap.php','parsedata/upload_hall.php');
  		$html='';
  		for ($i=0; $i< count($meas_table); $i++) {
  			
  			$html.='<h2>'.$meas_name[$i].': ';
  			$html.='<a style="color:blue" href="'.SJ_HOST_NAME.'epi/'.$meas_url[$i].'?wafer_id='.$wafer_id.'">Add</a>';
  			$html.='<h2>';
  			$html.='<hr>';
  			$html.='<br>';
  			
  			if($meas_name[$i] == 'PL') {
  				$tmp=$this->pl_map_report_all_html($wafer_id);
  				$html .= ($tmp != ''?$tmp."<br>":'');
  				
  				$tmp =  $this->pl_map_report_pieces_anneal_html($wafer_id);
  				if ($tmp != ''){ 
  					$html.='The following PL data is available for pieces: <br><br>'.$tmp.'<br>';
  				}
  			}
  			
  			if($meas_name[$i] == 'RTA') {
		  		$tmp=$this->rta_report_html($wafer_id);
		  		$html .= ($tmp != ''?$tmp."<br>":'');
		  		
		  		$tmp=$this->rta_report_pieces_html($wafer_id);
		  		
		  		if ($tmp != '') {
		  			$html.='The following pieces have been annealed: <br><br>'.$tmp.'<br>';
		  		
		  		}
		  		
  			}
  			
  			if($meas_name[$i] == 'HALL') {
		  		$tmp=$this->hall_report_html($wafer_id);
		  		$html .= ($tmp != ''?$tmp."<br>":'');
		  		
		  		$tmp = '';
		  		foreach($pieces as $k=>$v) {
		  			$piece_html = $this->hall_report_html($k);
		  			
		  			if ($piece_html !='') {
		  				$tmp .=$v.'<br>'.$piece_html.'<br>';
		  			}
		  		
		  		}
		  		if ($tmp != '') {
		  			$html.='The following pieces have Hall data:<br><br>'.$tmp;
		  		
		  		}
		  		
  			}
  		
  		}
  	
  		return $html;
  	}
  	
  	
  	
  	/* functions to get the measurement values for the wafer view */
  	
  	
  	
  	
  	
  	function update_growth_layers_delete($growth_id,$growth_layer_id,$layer_number) {
  	/* update layer numbers - called after the delete process has occurred to renumber the layers */
  	
  	$conn = DBManager::getConnection();
  		$max_layer = max($this->get_growth_layers($growth_id));
  		//echo "max layer".$max_layer;
  		//echo "current = $layer_number +1";
  			if ($layer_number < $max_layer ) {
  				$tmp_layer_number = $max_layer+1;
 		 		$query = "update epi_growth_layers set layer_number = $tmp_layer_number where growth_layer_id =$growth_layer_id";
  				
  		
 		 		$result = $conn->query($query);
  				for ($layer = $layer_number + 1 ; $layer <=$max_layer; $layer ++) {
  					$query = "UPDATE epi_growth_layers set layer_number = ".($layer-1)." 
  							where layer_number = $layer and growth_id = $growth_id";
  						//	echo $query;
  							$result = $conn->query($query);
  							
  				}
  			} 
	}  	  	
  	
  	function update_growth_layers_change($growth_id,$growth_layer_id,$old_layer_number,$new_layer_number) {
  	
  	/* update layer numbers - called before the update process has occurred to open up the slot for the new layer */
  		if ( $new_layer_number == $old_layer_number ) return;
  		
  		$conn = DBManager::getConnection();
  		
  		/* first shift the layer to a max+1 value */
  		$max_layer = max($this->get_growth_layers($growth_id));
  		$tmp_layer_number = $max_layer+1;
  		$query = "update epi_growth_layers set layer_number = $tmp_layer_number where growth_layer_id =$growth_layer_id";
  		
  		
  		$result = $conn->query($query);
  	
  		
  		
  		/* now move all the other layers to free up the slot and keep everything in order */
  		
  		if ($new_layer_number  >  $old_layer_number ) {
  			for ($layer = $old_layer_number + 1 ; $layer <=$new_layer_number; $layer ++) {
  					$query = "UPDATE epi_growth_layers set layer_number = layer_number - 1 
  							where layer_number = $layer and growth_id = $growth_id";
  							
  							$result = $conn->query($query);
  			}
  		
  		} else {
  			for ($layer = $old_layer_number - 1  ; $layer >=$new_layer_number; $layer -= 1) {
  					$query = "UPDATE epi_growth_layers set layer_number = layer_number + 1 
  							where layer_number = $layer and growth_id = $growth_id";
  							$result = $conn->query($query);
  			}
  		}
  		
  	}
  
  	function update_structure_layers_delete($structure_id,$structure_layer_id,$layer_number) {
  	/* update layer numbers - called after the delete process has occurred to renumber the layers */
  	
  		$conn = DBManager::getConnection();
  		$max_layer = max($this->get_structure_layers($structure_id));
  		//echo "max layer".$max_layer;
  		//echo "current = $layer_number +1";
  			if ($layer_number < $max_layer ) {
  				$tmp_layer_number = $max_layer+1;
 		 		$query = "update epi_structure_layers set layer_number = $tmp_layer_number where structure_layer_id =$structure_layer_id";
  				
  		
 		 		$result = $conn->query($query);
  				for ($layer = $layer_number + 1 ; $layer <=$max_layer; $layer ++) {
  					$query = "UPDATE epi_structure_layers set layer_number = ".($layer-1)." 
  							where layer_number = $layer and structure_id = $structure_id";
  						//	echo $query;
  							$result = $conn->query($query);
  							
  				}
  			} 
	}  	  	
  	
  	function update_structure_layers_change($structure_id,$structure_layer_id,$old_layer_number,$new_layer_number) {
  	
  	/* update layer numbers - called before the update process has occurred to open up the slot for the new layer */
  		if ( $new_layer_number == $old_layer_number ) return;
  		
  		$conn = DBManager::getConnection();
  		
  		/* first shift the layer to a max+1 value */
  		$max_layer = max($this->get_structure_layers($structure_id));
  		$tmp_layer_number = $max_layer+1;
  		$query = "update epi_structure_layers set layer_number = $tmp_layer_number where structure_layer_id =$structure_layer_id";
  		
  		
  		$result = $conn->query($query);
  	
  		
  		
  		/* now move all the other layers to free up the slot and keep everything in order */
  		
  		if ($new_layer_number  >  $old_layer_number ) {
  			for ($layer = $old_layer_number + 1 ; $layer <=$new_layer_number; $layer ++) {
  					$query = "UPDATE epi_structure_layers set layer_number = layer_number - 1 
  							where layer_number = $layer and structure_id = $structure_id";
  							
  							$result = $conn->query($query);
  			}
  		
  		} else {
  			for ($layer = $old_layer_number - 1  ; $layer >=$new_layer_number; $layer -= 1) {
  					$query = "UPDATE epi_structure_layers set layer_number = layer_number + 1 
  							where layer_number = $layer and structure_id = $structure_id";
  							$result = $conn->query($query);
  			}
  		}
  		
  	}
  	
  	
  	function get_growth_structure_id($growth_id) {
  		$conn = DBManager::getConnection();
  		$query = "select structure_id as structure_id from epi_growth where growth_id = $growth_id";
  		
  		$result = $conn->query($query);
  		$row = $result->fetch_assoc();
  		return $row['structure_id'];
  	}
  
  	function add_growth_structure_relation($growth_id,$structure_id) {
		$conn = DBManager::getConnection();
		$query  =  "insert into epi_growth_structure_relation
							(growth_id,structure_id)
						values
							('$growth_id','$structure_id')";
		$result = $conn->query($query);
		return $result;
  	}

  	function drop_growth_structure_relation($growth_id,$structure_id) {
		$conn = DBManager::getConnection();
		$query  = "delete from epi_growth_structure_relation ".
              "where growth_id='$growth_id' and structure_id = '$structure_id'";
		// echo "<br> $query";
		$result = $conn->query($query);
		return $result;
  	}

  
  
  	//
  	function show_growth_stack($growth_id){
  		$conn = DBManager::getConnection();
  		$query = "select layer_number, 
  							material,
  							layer_function,
  							dopant, 
  							thickness, 
  							si_est_carrier_conc,
  							be_est_carrier_conc,   	
  							cbr4_est_carrier_conc, 
  							notes from epi_growth_layers
  					where growth_id = $growth_id 
  					order by layer_number desc";
  		$result=$conn->query($query);
  		$html='<table class="growth_stack">';
  		$html.='<tr class="growth_stack_header">
  					<td class="growth_stack">Layer</td>
  					<td class="growth_stack">Thickness (um)</td>
  					<td class="growth_stack">Material</td>
  					<td class="growth_stack">Doped</td>
  					<td class="growth_stack">Concentration</td>
  					<td class="growth_stack">Layer Function</td>
  					<td class="growth_stack">Notes</td>
  				</tr>';
  		$eo=0;
  		
  			
  		
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$layer_number = $row['layer_number'];
  			$thickness = $row['thickness'];
  			$material=$row['material'];
  			$dopant= $row['dopant'];
  			$layer_function = $row['layer_function'];
  			$concentration='';
  			switch (trim($dopant)) {
  			
  			case 'C':
  				$css_class = "doped-c";
  				$concentration=$row['cbr4_est_carrier_conc'];
  				break;
  			case 'Si':
	  			$css_class = "doped-si";
  				$concentration=$row['si_est_carrier_conc'];
  				break;
  			case 'Be':
	  			$css_class = "doped-be";
  				$concentration=$row['be_est_carrier_conc'];
  				break;
  			default:
  				$css_class = "undoped";
  				$dopant='&nbsp;';
  				$concentration='&nbsp;';
  				break;
  				
  			}
  			if ($eo==1){
  				$eo=0;
  			} else {
  				$eo=1;
  			}
  			
  			$css_class.="-".$eo;
  			
  			$notes=$row['notes'];
  			if($notes == '') {
  				$notes= '&nbsp;';
  			}
  			$html.="<tr class=\"$css_class\">
  					<td class=\"growth_stack\">$layer_number</td>
  					<td class=\"growth_stack\">$thickness</td>
  					<td class=\"growth_stack\">$material</td>
  					<td class=\"growth_stack\">$dopant</td>
  					<td class=\"growth_stack\">$concentration</td>
  					<td class=\"growth_stack\">$layer_function</td>
  					<td class=\"growth_stack\">$notes</td>
  				</tr>";
  			
  			
  		}
  		$html.='</table>';
  		
  		return $html;
  	}
  
  	
  	//
  	function show_wafer_stack($wafer_id){
  		$conn = DBManager::getConnection();
		/*
		$query = "select growth_id from epi_growth_wafer_relation where wafer_id = $wafer_id order by growth_wafer_relation_id desc";
		echo $query;
		$result= $this->local_query($query);
		foreach ($result as $row) {
		echo "<br> growth id = ".$row['growth_id'];
			$growth_ids[]=$row['growth_id'];
		}

		foreach ($growth_ids as $g) {
			
			echo $this->show_growth_stack($g);
		
		}
		*/

  		$query = "select l.*, CONCAT(gr.chamber_id,'-',gr.growth_run) as growth_name from epi_growth_layers l, epi_growth_wafer_relation g, epi_growth gr where g.wafer_id =$wafer_id and g.growth_id = l.growth_id and gr.growth_id = l.growth_id";
  		$result=$conn->query($query);
  		$html='<table class="growth_stack">';
  		$html.='<tr class="growth_stack_header">
  					<td class="growth_stack">Layer </td>
  					
  					<td class="growth_stack">Thickness (um)</td>
  					<td class="growth_stack">Material</td>
  					<td class="growth_stack">Doped</td>
  					<td class="growth_stack">Concentration</td>
  					<td class="growth_stack">Layer Function</td>
  					<td class="growth_stack">Notes</td>
  				</tr>';
  		$eo=0;
  		
  		$layer=$result->num_rows;
  		
  	
  		while (($row=$result->fetch_assoc()) !== NULL ) {
			$layer_id = $row['growth_layer_id'];
			$growth_id =$row['growth_id'];
			echo "<br> $growth_id";
  			$layer_number = $row['layer_number'];
  			$thickness = $row['thickness'];
  			$material=$row['material'];
  			$dopant= $row['dopant'];
  			$layer_function = $row['layer_function'];
  			$concentration='';
  			switch (trim($dopant)) {
  			
  			case 'C':
  				$css_class = "doped-c";
  				$concentration=$row['cbr4_est_carrier_conc'];
  				break;
  			case 'Si':
	  			$css_class = "doped-si";
  				$concentration=$row['si_est_carrier_conc'];
  				break;
  			case 'Be':
	  			$css_class = "doped-be";
  				$concentration=$row['be_est_carrier_conc'];
  				break;
  			default:
  				$css_class = "undoped";
  				$dopant='&nbsp;';
  				$concentration='&nbsp;';
  				break;
  				
  			}
  			if ($eo==1){
  				$eo=0;
  			} else {
  				$eo=1;
  			}
  			
  			$css_class.="-".$eo;
  			
  			$notes=$row['notes'];
  			if($notes == '') {
  				$notes= '&nbsp;';
  			}
  			$html.="<tr class=\"$css_class\">
  					<td class=\"growth_stack\"><a style=\"color:blue\" href=\"epi_growth_layers.php?growth_id=$growth_id&PME_sys_operation=PME_op_View&PME_sys_rec=$layer_id\">$layer</a></td>
  					<td class=\"growth_stack\">$thickness</td>
  					<td class=\"growth_stack\">$material</td>
  					<td class=\"growth_stack\">$dopant</td>
  					<td class=\"growth_stack\">$concentration</td>
  					<td class=\"growth_stack\">$layer_function</td>
  					<td class=\"growth_stack\">$notes</td>
  					
  				</tr>";
  			
  			$layer=$layer-1;
  		}
  		$html.='</table>';
  		
  		// now attach the substrate 
  		$query="select s.*, sv.name as vendor_name from epi_substrate s, epi_wafer w, epi_substrate_vendor sv
  						where s.substrate_id = w.substrate_id
  						and sv.substrate_vendor_id = s.substrate_vendor_id
  						and w.wafer_id  =  $wafer_id";
  		$result=$conn->query($query);
  		
  		
  		if ($row = $result->fetch_assoc()) {
	  		$html .='<div style="padding:4px; font-weight:bold"><a style="color:blue;" href="epi_substrate.php?PME_sys_operation=PME_op_View&PME_sys_rec='.$row['substrate_id'].'"> Substrate: </a></div>';
  			$html.='<table class="growth_stack">';
  			$html.='<tr class="doped-be"><td class="growth_stack">Substrate Name</td><td>Vendor</td><td class="growth_stack">Type</td><td>Doped</td class="growth_stack"><td>Dopant</td><td class="growth_stack">Doping Level</td><td class="growth_stack">Diameter</td></tr>';
  			$html.='<tr class="doped-si"><td class="growth_stack">'.$row['substrate_name'].'</td><td class="growth_stack">'.$row['vendor_name'].'</td><td class="growth_stack">'.$row['substrate_type'].'</td><td class="growth_stack">'.$row['doped'].'</td><td class="growth_stack">'.$row['substrate_dopant'].'</td><td class="growth_stack">'.$row['doping_level'].'</td><td class="growth_stack">'.$row['substrate_diameter'].'</td></tr>';
  			
  			$html.='</table>';
  			
  		}	
  		
  		return $html;
  	}
  
  	
	function show_wafer_structure_stack($wafer_id)
	{
  		$conn = DBManager::getConnection();
		
		$query = "SELECT growth_id ".
			 "FROM epi_growth_wafer_relation ".
			 "WHERE wafer_id = $wafer_id ".
			 "ORDER BY growth_wafer_relation_id DESC";

		$result= $this->local_query($query);
		foreach ($result as $row) {
			$growth_ids[]=$row['growth_id'];
		}

		foreach ($growth_ids as $g) {
			$sid = $this->get_structure_id_by_growth_id($g);
			echo 'Growth: '.$this->growth_name_by_id($g)."<br>";
			echo '<br> Structure: '.$this->get_structure_name_by_id($sid);
			$structure_description = $this->get_structure_description($sid);
			if ($structure_description != NULL ){
				echo ": ".$structure_description;
			}
			echo "<br><br>";
			echo $this->show_structure_stack($sid);
		}
	}
  
  	
	// function show_arc_lot_number_notes($lot_number)
	function show_arc_lot_number_notes($wafer_id)
	{
  		$conn = DBManager::getConnection();
		
		$query = "SELECT l.notes AS notes, l.arc_design_name AS  arc_design ".
			 "FROM epi_process_lot_wafer w INNER JOIN epi_process_lot l ".
			 "ON w.process_lot_id = l.process_lot_id ".
			 "WHERE w.wafer_id = $wafer_id AND w.process_lot_number LIKE 'ARC%' LIMIT 1";

		//$query = "SELECT notes FROM epi_process_lot WHERE process_lot_number LIKE '$lot_number'";

		$result= $this->local_query($query);
		$notes = "";
		if ($result != NULL && (!empty($result[0]['notes']) || (!empty($result[0]['arc_design'])))) {
			$notes  = "<table style='margin:2px;padding:2px;'>";
			$notes .= "<tr><td>ARC Design: ".$result[0]['arc_design']." </td></tr>";
			if (!empty($result[0]['notes'])) {
				$notes .= "<tr><td>Notes: ".$result[0]['notes']." </td></tr>";
			}
			$notes .= "</table>";
		}
		return $notes;
	}
  
  	
	function show_wafer_structure_stack_v2($wafer_id)
	{
  		$conn = DBManager::getConnection();
		
		$query = "SELECT structure_name ".
			 "FROM epi_wafer ".
			 "WHERE wafer_id = $wafer_id LIMIT 1";

		$result= $this->local_query($query);
		if ($result != NULL) {
			$query = "SELECT structure_id as sid ".
			 "FROM epi_structure ".
			 "WHERE structure_name LIKE '".$result[0]['structure_name']."' LIMIT 1";

			$result= $this->local_query($query);
			if ($result != NULL) {
				return $this->show_structure_stack_v2($result[0]['sid']);
			}
		}
		return "No layers";
	}
  
	function show_structure_stack_v2($structure_id)
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT s.* , m.material_compound_name ".
  			 "FROM epi_structure_layers s, epi_growth_layer_materials m ".
  			 "WHERE s.structure_id LIKE $structure_id ".
  			 "AND m.material_name = s.material_name ".
  		 	 "ORDER BY layer_number DESC";
  		$result=$conn->query($query);
  		$html='<table class="growth_stack">';
  		$html.='<tr class="growth_stack_header">
  				<td class="growth_stack" align="center"><u>layer</u></td>
  				<td class="growth_stack" align="center"><u>function</u></td>
  				<td class="growth_stack" align="center"><u>thickness (nm)</u></td>
  				<td class="growth_stack" align="center"><u>material</u></td>
  				<td class="growth_stack" align="center"><u>x(%)</u></td>
  				<td class="growth_stack" align="center"><u>y(%)</u></td>
  				<td class="growth_stack" align="center"><u>z(%)</u></td>
  				<td class="growth_stack" align="center"><u>dopant</u></td>
  				<td class="growth_stack" align="center"><u>concentration (lo)</u></td>
  				<td class="growth_stack" align="center"><u>concentration (hi)</u></td>
  				<td class="growth_stack" align="center"><u>notes</u></td>
  			</tr>';
  		$eo=0;
  		
  			
  		
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$layer_number = $row['layer_number'];
  			$layer_function = $row['layer_function'];
  			$thickness = $row['thickness'];
  			$material=$row['material_name'];
  			$dopant= $row['dopant_name'];
  			$doped = $row['doped'];
  			$concentration_hi=$row['doping_level_hi'];
  			$concentration_lo=$row['doping_level_lo'];
  			$composition_x = $row['composition_x'];
  			$composition_y = $row['composition_y'];
  			$composition_z = $row['composition_z'];
			
  			switch (trim($dopant)) {
  			
  			case 'C':
  				$css_class = "doped-c";
  				
  				break;
  			case 'Si':
	  			$css_class = "doped-si";
  				break;
  			case 'Be':
	  			$css_class = "doped-be";
  				break;
  			default:
  				$css_class = "undoped";
  				$concentration='&nbsp;';
  				break;
  				
  			}
  			if ($eo==1){
  				$eo=0;
  			} else {
  				$eo=1;
  			}
  			
  			$css_class.="-".$eo;
  			
  			$notes=$row['notes'];
  			if($notes == '') {
  				$notes= '&nbsp;';
  			}
  			$html.="<tr class=\"$css_class\">
  				<td class=\"growth_stack\" align=\"center\">$layer_number</td>
  				<td class=\"growth_stack\" align=\"center\">$layer_function</td>
  				<td class=\"growth_stack\" align=\"center\">$thickness</td>
  				<td class=\"growth_stack\" align=\"center\">".strtolower($doped)."-$material</td>
  				<td class=\"growth_stack\" align=\"center\">$composition_x</td>
  				<td class=\"growth_stack\" align=\"center\">$composition_y</td>
  				<td class=\"growth_stack\" align=\"center\">$composition_z</td>
  				<td class=\"growth_stack\" align=\"center\">$dopant</td>
  				<td class=\"growth_stack\" align=\"center\">$concentration_lo</td>
  				<td class=\"growth_stack\" align=\"center\">$concentration_hi</td>
  				<td class=\"growth_stack\" align=\"center\">$notes</td>
  				</tr>";
  		}
  		$html.='</table>';
  		
  		return $html;
  	}

  
	function show_structure_stack($structure_id)
	{
  		$conn = DBManager::getConnection();
  		$query = "SELECT s.* , m.material_compound_name ".
  			 "FROM epi_structure_layers s, epi_growth_layer_materials m ".
  			 "WHERE s.structure_id = $structure_id ".
  			 "AND m.material_name = s.material_name ".
  		 	 "ORDER BY layer_number DESC";
  		$result=$conn->query($query);
  		$html='<table class="growth_stack">';
  		$html.='<tr class="growth_stack_header">
  					<td class="growth_stack" align="center"><u>layer</u></td>
  					<td class="growth_stack" align="center"><u>function</u></td>
  					<td class="growth_stack" align="center"><u>thickness (nm)</u></td>
  					<td class="growth_stack" align="center"><u>material</u></td>
  					<td class="growth_stack" align="center"><u>x(%)</u></td>
  					<td class="growth_stack" align="center"><u>y(%)</u></td>
  					<td class="growth_stack" align="center"><u>z(%)</u></td>
  					<td class="growth_stack" align="center"><u>dopant</u></td>
  					<td class="growth_stack" align="center"><u>concentration (lo)</u></td>
  					<td class="growth_stack" align="center"><u>concentration (hi)</u></td>
  					<td class="growth_stack" align="center"><u>notes</u></td>
  				</tr>';
  		$eo=0;
  		
  			
  		
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$layer_number = $row['layer_number'];
  			$layer_function = $row['layer_function'];
  			$thickness = $row['thickness'];
  			$material=$row['material_name'];
  			$dopant= $row['dopant_name'];
  			$doped = $row['doped'];
  			$concentration_hi=$row['doping_level_hi'];
  			$concentration_lo=$row['doping_level_lo'];
  			$composition_x = $row['composition_x'];
  			$composition_y = $row['composition_y'];
  			$composition_z = $row['composition_z'];
			
  			switch (trim($dopant)) {
  			
  			case 'C':
  				$css_class = "doped-c";
  				
  				break;
  			case 'Si':
	  			$css_class = "doped-si";
  				break;
  			case 'Be':
	  			$css_class = "doped-be";
  				break;
  			default:
  				$css_class = "undoped";
  				$concentration='&nbsp;';
  				break;
  				
  			}
  			if ($eo==1){
  				$eo=0;
  			} else {
  				$eo=1;
  			}
  			
  			$css_class.="-".$eo;
  			
  			$notes=$row['notes'];
  			if($notes == '') {
  				$notes= '&nbsp;';
  			}
  			$html.="<tr class=\"$css_class\">
  					<td class=\"growth_stack\" align=\"center\">$layer_number</td>
  					<td class=\"growth_stack\" align=\"center\">$layer_function</td>
  					<td class=\"growth_stack\" align=\"center\">$thickness</td>
  					<td class=\"growth_stack\" align=\"center\">".strtolower($doped)."-$material</td>
  					<td class=\"growth_stack\" align=\"center\">$composition_x</td>
  					<td class=\"growth_stack\" align=\"center\">$composition_y</td>
  					<td class=\"growth_stack\" align=\"center\">$composition_z</td>
  					<td class=\"growth_stack\" align=\"center\">$dopant</td>
  					<td class=\"growth_stack\" align=\"center\">$concentration_lo</td>
  					<td class=\"growth_stack\" align=\"center\">$concentration_hi</td>
  					<td class=\"growth_stack\" align=\"center\">$notes</td>
  				</tr>";
  			
  			
  		}
  		$html.='</table>';
  		
  		return $html;
  	}

  
  
  	function search_suggest($growth, $wafer)
	{
	  	// return JSON encoded array with wafer and growth name and id suggestions ... 
  		$conn = DBManager::getConnection();
  		$query ='';
  		if (strlen($growth) > 0) {
  			$growth=strtoupper($growth);
	  		$query  = "SELECT growth_id AS growth_id, ";
	  		$query .= "CONCAT(chamber_id,'-',growth_run) as growth_name ";
	  		$query .= "FROM epi_growth ";
	  		$query .= "WHERE CONCAT(chamber_id,'-',growth_run) LIKE ('%$growth%') ";
	  		$query .= "ORDER BY growth_id DESC";
  		}
  		$result = $conn->query($query);
  		$arr='';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[] = array('growth_id'=>$row['growth_id'], 'growth_name'=>$row['growth_name']);
  		}
		$search['growth']=$arr;
		
  		$query ='';
		if (strlen($wafer) > 0) {
	  		$query = "select wafer_id as wafer_id, wafer_name as wafer_name from epi_wafer where wafer_name like ('%$wafer%') order by wafer_id asc";
  		}
  		$result = $conn->query($query);
  		$arr2='';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr2[] = array('wafer_id'=>$row['wafer_id'], 'wafer_name'=>$row['wafer_name']);
 	 	}
		$search['wafer']=$arr2;
 	 	echo json_encode($search);
  	}
		

	function suggest_layer_name($layer)
	// returns JSON encoded array with layer id and name suggestions ... 
	{
		$conn = DBManager::getConnection();

		if (strlen($layer) > 0) {
			$query = "SELECT DISTINCT layer_name ";
	  		$query .= "FROM epi_mbe_growth_trend_summary ";
	 		$query .= "WHERE layer_name LIKE ('%$layer%') ";
	  		$query .= "ORDER BY layer_name ASC LIMIT 20";
  			$result = $conn->query($query);
  			$arr = array();
			$i = 1;
  			while (($row=$result->fetch_assoc()) !== NULL ) {
  				$arr[] = array('layer_id'=>$i, 'layer_name'=>$row['layer_name']);
				$i++;
 	 		}
 	 		echo json_encode(array('layer'=>$arr));
  		}
  	}
  	

  	function suggest_structure($structure)
	// returns JSON encoded array with structure id and name suggestions ... 
	{
  		$conn = DBManager::getConnection();

		if (strlen($structure) > 0) {
			$query = "SELECT structure_id, structure_name ";
	  		$query .= "FROM epi_structure ";
	 		$query .= "WHERE structure_name LIKE ('%$structure%') ";
	  		$query .= "ORDER BY structure_name ASC LIMIT 20";
  			$result = $conn->query($query);
  			$arr = array();
  			while (($row=$result->fetch_assoc()) !== NULL ) {
  				$arr[] = array('structure_id'=>$row['structure_id'], 'structure_name'=>$row['structure_name']);
 	 		}
 	 		echo json_encode(array('structure'=>$arr));
  		}
  	}
  	
  	 function search_suggest_wafer($wafer) {
  		
  		
	  		// return JSON encoded array with wafer and growth name and id suggestions ... 
		
			if (strlen($wafer) > 0) {
		  		$query = "select wafer_id as wafer_id, wafer_name as wafer_name from epi_wafer where wafer_name like ('%$wafer%') order by wafer_id asc";
  			} else {
  				$query ='';
  			}
  			$result = $this->local_query($query);
			
  			$res='';
  			if($result!=NULL)
			{
			 foreach($result as $row) {
  				$res[]=array('wafer_id'=>$row['wafer_id'],'wafer_name'=>$row['wafer_name']);
			 }
			}
			$search['wafer']=$res;
		
		
 	 	echo json_encode($search);
  	
  	}
  	
  	
  	
  	function check_growth($growth,$wafer) {
  		
  		
  		$conn = DBManager::getConnection();
  		$growth_id = $this->get_growth_id_by_name($growth);
  		
  		$wafer_id = $this->get_wafer_id_by_name($wafer);
  		
  		
  		$query = "select w.wafer_name as wafer_name,
  						g.wafer_number as wafer_number,
  						w.wafer_id as wafer_id
  						from epi_wafer w, epi_growth_wafer_relation as g
  						where g.growth_id = $growth_id and g.wafer_id = w.wafer_id order by g.wafer_number asc";
  		
  		$result = $conn->query($query);
  		$arr='';
  		while (($row=$result->fetch_assoc()) !== NULL ) {
  			$arr[]=array('wafer_number'=>$row['wafer_number'],'wafer_name'=>$row['wafer_name'],'wafer_id'=>$row['wafer_id']);
  			if ($row['wafer_id'] == $wafer_id) {
  			$err[]='This growth has already been added to this wafer. Please choose a different pair.';
  			}
  			$wafer_numbers[]=$row['wafer_number'];
  		}
  		
  		$wafer_number_options=range(1,7);
	
		if ($wafer_numbers !== NULL && is_array($wafer_numbers) ){
			$wafer_number_options=array_diff($wafer_number_options,$wafer_numbers);
		}
		if (count($wafer_number_options) == 0) {
			$err[]='ERROR: this growth has 7 wafers added to it. No new wafers can be added. Please select a different growth';
			$arr2='';
		} else {
			foreach($wafer_number_options as $wo) {
				$arr2[]=$wo;
			}
		}
		
		$check['growth']['current']=$arr;
		$check['growth']['errors']=$err;
		$check['growth']['empty']=$arr2;
	
  		echo json_encode($check);
  	}
  	
// end of class util;  
	function get_growth_total() {
		$conn = DBManager::getConnection();
		$query = "select count(*) as count from epi_growth where 1";
		$result = $conn->query($query);
		$row= $result->fetch_assoc();
		return $row['count'];
	}
	
	function get_wafer_total($pieces = FALSE) {
		$conn = DBManager::getConnection();
		$query = "select count(*) as count from epi_wafer where piece = 0";
		
		if ($pieces ) {
			$query = "select count(*) as count from epi_wafer where 1";
		}
		
		$result = $conn->query($query);
		$row= $result->fetch_assoc();
		return $row['count'];
	}
  
  	function get_substrate_total() {
		$conn = DBManager::getConnection();
		$query = "select number_left as number_left from epi_substrate where 1";
		$result = $conn->query($query);
		$number_left = 0;
		while(($row= $result->fetch_assoc()) !==NULL) {
			$number_left = $number_left + $row['number_left'];
		}
		return $number_left;
	}
  	
  	
  	function auto_label_substrates() {
  		$conn = DBManager::getConnection();	
  		$query = "select substrate_vendor_id as sid, name as name from epi_substrate_vendor where 1";
  		$result = $conn->query($query);
  		while(($row= $result->fetch_assoc()) !==NULL) {
			$sid=$row['sid'];
			$sname=$row['name'];
			$scount = 1;
			$query2="select substrate_id from epi_substrate where substrate_vendor_id = $sid order by substrate_id asc";
			$result2=$conn->query($query2);
			while(($row2= $result2->fetch_assoc()) !==NULL) {
				$query3="update epi_substrate set substrate_name='".$sname."-".$scount."' where substrate_id = ".$row2['substrate_id'];
				echo "query 3 $query3<br>";
				$result3=$conn->query($query3);
				$scount=$scount+1;
			}
		}
  	
  	}
  	
  	function substrate_auto_name($substrate_id) {
  		
  		/* first get the vendor name */
  		$query = "select v.name substrate_vendor, s.substrate_name as substrate_name from epi_substrate s, epi_substrate_vendor v where
  						s.substrate_vendor_id =v.substrate_vendor_id 
  						and s.substrate_id = $substrate_id";
  						// echo $query;
  		$row = $this->local_query($query);
  		print_r($row);
  		$substrate_vendor = $row[0]['substrate_vendor'];
  		$substrate_name = $row[0]['substrate_name'];
//  		echo "<br> V $substrate_vendor  N	$substrate_name";
  		if ($substrate_name != NULL ) {
  				return;
  		}
  		
  		$query = "select s.*, v.name as vname from epi_substrate s, epi_substrate_vendor v where 
  					s.substrate_vendor_id =v.substrate_vendor_id 
  					and v.name='$substrate_vendor' and s.substrate_id < $substrate_id order by s.substrate_id desc LIMIT 1";
  		// echo "<br> $query";
  		$row = $this->local_query($query);
  		// $this->echo_r($row);
  		if ($row == NULL ) {
  			$substrate_name = $substrate_vendor.'-1';
  		} else {
  			$last_name = explode('-',$row[0]['substrate_name']);
  			if (!is_numeric($last_name[1]) || $last_name[1] == NULL) {
  				$last_name[1]=0;
  			}
  			$num = $last_name[1];
  			
  			$substrate_name = $substrate_vendor.'-'.($num+1);
  		}
  		$query = "update epi_substrate set substrate_name = '$substrate_name' where substrate_id  = $substrate_id ";
  		// echo "<br>$query";
  		$result =$this->local_insert($query);
  		
  	
  	}
  	
  	function get_wafer_pl_ids($wafer_id) {
	
		$conn = DBManager::getConnection();	
		$query = "select pm.pl_meas_id as pl_meas_id from epi_pl_meas pm, epi_pl pl where pm.pl_id=pl.pl_id and pl.wafer_id  =$wafer_id ";
	
		$result = $conn->query($query);
  		while(($row= $result->fetch_assoc()) !==NULL) {
  		
  			$arr[]=$row['pl_meas_id'];
  			
  		}
		if (count($arr) > 0) {
			return $arr;
		} else {
			$arr[]=0;
			return $arr;
		}
	}

	function get_pl_meas_ids($pl_id) {
	
		$conn = DBManager::getConnection();	
		$query = "select pl_meas_id from epi_pl_meas where pl_id = $pl_id ";
	
		$result = $conn->query($query);
  		while(($row= $result->fetch_assoc()) !==NULL) {
  		
  			$arr[]=$row['pl_meas_id'];
  			
  		}
		if (count($arr) > 0) {
			return $arr;
		} else {
			$arr[]=0;
			return $arr;
		}
	}
	
	
	

    /**
     * Function to print out data from an uploaded CSV file into a table.
     *
     * @param array $data
     * @return
     */
    function printCSV ($data) {
        if (is_array($data)) {
            // open the csv file and read it
            $dataFile = $data['file']; 
            $open = fopen($dataFile, 'r');
            $csv = fread($open, filesize($dataFile));
           
            print "<h2>" . $data['file'] . "</h2>"; // optional
            // split up the rows
            $csv = preg_split('/[\r\n]/', $csv);
           
            $csvData = array();
            $i = 0;
            foreach ($csv as $line) {
                // splits lines with a comma but ignores a comma wrapped with double quotes
                $csvData[$i] = preg_split('/,(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))/', $line);
                $i++;
            }
            print "<table width='60%' cellpadding='0' cellspacing='0' border='1px' class='course-table'>";
                for ($j = 0; $j < count($csvData); $j++) {
                    // print the first line with <th> tags
                    if ($csvData && $j == 0) {
                        print "<tr>";
                        for ($k = 0; $k < count($csvData[$j]); $k++) {
                            // remove the double quotes
                            if (preg_match('/\"/', $csvData[$j][$k])) {
                                print "<th>" . trim($csvData[$j][$k], '"')  . "</th>"; // edit if you wish to keep the double quotes
                            } else {
                                print "<th>" . $csvData[$j][$k] . "</th>";
                            }
                        }
                        print "</tr>";
                    // print the rest with <td> tags
                    } else {
                        print "<tr>";
                        for ($k = 0; $k < count($csvData[$j]); $k++) {
                            // remove the double quotes
                            if (preg_match('/\"/', $csvData[$j][$k])) {
                                print "<td>" . trim($csvData[$j][$k], '"')  . "</td>"; // edit if you wish to keep the double quotes
                            } else {
                                print "<td>" . $csvData[$j][$k] . "</td>";
                            }
                        }
                        print "</tr>";
                    }
                }
            print "</table>";
        } else {
            return false;
        }
    }



    /**
     * Function to store data from an uploaded CSV file into an array.
     *
     * @param array $data
     * @return
     */
    function array_parseCSV ($data) {
        if (is_array($data)) {
            // open the csv file and read it
            $dataFile = $data['file']; 
            $open = fopen($dataFile, 'r');
            $csv = fread($open, filesize($dataFile));
           
           //	echo "<br> data file $dataFile $open";
            // split up the rows
            $csv = preg_split('/[\r\n]/', $csv);
           
            $csvData = array();
            $i = 0;
            foreach ($csv as $line) {
                // splits lines with a comma but ignores a comma wrapped with double quotes
                $csvData[$i] = preg_split('/,(?=(?:[^\"]*\"[^\"]*\")*(?![^\"]*\"))/', $line);
                $i++;
            }
           
                for ($j = 0; $j < count($csvData); $j++) {
                    // print the first line with <th> tags
                    if ($csvData && $j == 0) {
                        for ($k = 0; $k < count($csvData[$j]); $k++) {
                            // remove the double quotes
                            if (preg_match('/\"/', $csvData[$j][$k])) {
                               $csvData[$j][$k]=trim($csvData[$j][$k], '"');
                            } 
                        }
                    } else {
                        
                        for ($k = 0; $k < count($csvData[$j]); $k++) {
                            // remove the double quotes
                            if (preg_match('/\"/', $csvData[$j][$k])) {
                                $csvData[$j][$k]= trim($csvData[$j][$k], '"');
                            } 
                        }
                    }
                }
			return $csvData;
        } else {
            return false;
        }
    }

	

	
	/**
     * Function to print a 2 d array to a table
     *
     * @param array
     * @return
     */
  	
  	function array_to_table($data) {
  		 print "<table width='60%' cellpadding='0' cellspacing='0' border='1px' class=\"simple\">";
                for ($j = 0; $j < count($data); $j++) {
                    // print the first line with <th> tags
                    if ($data && $j == 0) {
                        print "<tr class=\"simple\">";
                        for ($k = 0; $k < count($data[$j]); $k++) {
                        	if( $data[$j][$k] == NULL ){
                             print "<th  class=\"simple\" >" .  '&nbsp;'. "</th>";
                            } else {
                            
                            print "<th  class=\"simple\" >" . $data[$j][$k] . "</th>";
                            
                            }
                        }
                        print "</tr>";
                    // print the rest with <td> tags
                    } else {
                        print "<tr class=\"simple\">";
                        for ($k = 0; $k < count($data[$j]); $k++) {
                        	if( $data[$j][$k] == NULL ){
	                          print "<td class=\"simple\">" . '&nbsp;' . "</td>";
                        	} else {
                        		 print "<td class=\"simple\">" . $data[$j][$k] . "</td>";
                        	}
                        }
                        print "</tr>";
                    }
                }
            print "</table>";
  	}
  	
  	
  	function array_to_table_html($data) {
  		
  		$html = "<table width='60%' cellpadding='0' cellspacing='0' border='1px' class='course-table'>";
                for ($j = 0; $j < count($data); $j++) {
                    // print the first line with <th> tags
                    if ($data && $j == 0) {
                        $html .= "<tr>";
                        for ($k = 0; $k < count($data[$j]); $k++) {
                             $html .=  "<th>" . $data[$j][$k] . "</th>";
                        }
                        $html .=  "</tr>";
                    // print the rest with <td> tags
                    } else {
                        $html .=  "<tr>";
                        for ($k = 0; $k < count($data[$j]); $k++) {
	                          $html .=  "<td>" . $data[$j][$k] . "</td>";
                        }
                        $html .=  "</tr>";
                    }
                }
            $html .= "</table>";
            return $html;
  	}
  	
  	
  	/** print_r but nice */
  	function echo_r($array)
		{
  		echo "<pre>";
			print_r($array);
			echo "</pre>";
  	}

  	function check_empty($var)
		{
  		return empty($var);
  	}

	function parse_date($date){
		$d=date_parse($date);
		
		return date("Y-m-d",mktime(0,0,0,$d['month'],$d['day'],$d['year']));
	
	}
	
	function parse_date_xls($date){
		$d = explode('/',trim($date));
		//print_r($d);
		return date("Y-m-d",mktime(0,0,0,$d[0],$d[1],$d[2]));
	}
	
	function parse_datetime($date){
		$d=date_parse($date);
		return date("Y-m-d H:i:s",mktime($d['hour'],$d['minute'],$d['second'],$d['month'],$d['day'],$d['year']));
	
	}
	
	function array_sanitize($array) {
		foreach($array as $key=>$val) {
			
			$res[$key]=addslashes($val);
		
		}
		return $res;
	}
 
 	/** 
 	
 	Function get wafer id's from external_epi
 	
 	*/
 	function get_external_wafers($external_id) {
 		$conn = DBManager::getConnection();	
 		$query  = "select wafer_ids as wafer_ids from epi_external where external_id = $external_id ";
 		$result = $conn->query($query);
  		if(($row= $result->fetch_assoc()) != NULL) {
  			return $row['wafer_ids'];
  		}
  		return NULL;
 	}
 
 	/** 
 	
 	Function get growth id's from external_epi
 	
 	*/
 	function get_external_growths($external_id) {
 		$conn = DBManager::getConnection();	
 		$query  = "select growth_ids as growth_ids from epi_external where external_id = $external_id ";
 		$result = $conn->query($query);
  		if(($row= $result->fetch_assoc()) != NULL) {
  			return $row['growth_ids'];
  		}
  		return NULL;
 	}
 
 /** 
 	
 	Function get substrate id's from external_epi
 	
 	*/
 	function get_external_substrates($external_id) {
 		$conn = DBManager::getConnection();	
 		$query  = "select substrate_ids as substrate_ids from epi_external where external_id = $external_id ";
 		$result = $conn->query($query);
  		if(($row= $result->fetch_assoc()) != NULL) {
  			return $row['substrate_ids'];
  		}
  		return NULL;
 	}
 
  /** 
 	
 	Function get structure id's from external_epi
 	
 	*/
 	function get_external_structures($external_id) {
 		$conn = DBManager::getConnection();	
 		$query  = "select structure_ids as structure_ids from epi_external where external_id = $external_id ";
 		$result = $conn->query($query);
  		if(($row= $result->fetch_assoc()) != NULL) {
  			return $row['structure_ids'];
  		}
  		return NULL;
 	}
 
 
 	/** 
 	
 	Function to delete an existing wafer and growth from epi_external
 	
 	*/
 	function delete_external_epi($external_id,$full = FALSE) {
 		$conn = DBManager::getConnection();	
 		
 		$wafer_ids = $this->get_external_wafers($external_id);
 		$growth_ids = $this->get_external_growths($external_id);
 		$substrate_ids = $this->get_external_substrates($external_id);
 		$structure_ids = $this->get_external_structures($external_id);
 		if ($wafer_ids != NULL ) {
 			$query = "delete from epi_wafer where wafer_id in ($wafer_ids)";
 			//echo "<br>".$query;
 			$result = $conn->query($query);
 			//echo "<br> $result ";
 		}
 		
 		if ($growth_ids != NULL ) {
 			$query = "delete from epi_growth where growth_id in ($growth_ids)";
 			//echo "<br>".$query;
 			$result = $conn->query($query);
 			//echo "<br> $result ";
 			
 		}
 		//IDF CHANGE
 		/*if ($substrate_ids != NULL ) {
 			$query = "delete from epi_substrate where substrate_id in ($substrate_ids)";
 			//echo "<br>".$query;
 			$result = $conn->query($query);
 			//echo "<br> $result ";
 		}
 		
 		if ($structure_ids != NULL ) {
 			$query = "delete from epi_structure where structure_id in ($structure_ids)";
 			//echo "<br>".$query;
 			$result = $conn->query($query);
 			//echo "<br> $result ";
 		} */
 		
 		
 		if ($full) {
	 		$query = "delete from epi_external where external_id = $external_id";
	 		//echo "<br>".$query;
 			$result = $conn->query($query);
 			//echo "<br> $result ";
 		}
 	}


 
 	/**
     * Function to parse an array into data for the structures
     *
     * @param array
     * @return
     */
 	/**
 	*/
 	
 	function parse_structure($data) {
 		$conn = DBManager::getConnection();	
 		$structure_layer_fields = array(
 		//	'growth_id',
 			'layer_number'=>'Layer Number',
 			'layer_function'=>'Layer Function',
 			'material_name'=>'Material Name',
 			'thickness'=>'Thickness',
 			'thickness_tol'=>'Thickness Tolerance',
 			'dopant_name'=>'Dopant Name',
 			'doped'=>'Doped',
 			'doping_level_hi'=>'Concentration High',
 			'doping_level_lo'=>'Concentration Low',
			'composition_x' =>'Composition x',
			'composition_x_tol' =>'Composition Tolerance x',
			'composition_y' =>'Composition y',
			'composition_y_tol' =>'Composition Tolerance y',
			'composition_z' =>'Composition z',
			'composition_z_tol' =>'Composition Tolerance z',
 			'notes'=>'Comments'
 		);
 		
 		$structure_fields =array( 
 			'structure_name' => 'Structure name',
 			'structure_description'=>'Description',
 			'date_added' => 'Date Created',
 			'user_id'=>'Created By',
 			'comments' => 'Comments'
 		);
 		
 		$datalength = count($data);
 		
 		/* start with the growth stack */
 		
 		// get the growth stack -
 		$numlayers = 0;
 		for ($i=($datalength-1);$i > 0; $i=$i-1) {
 		
 			$d=$data[$i];
 			$lnum=$d[0];
 			if (is_numeric($lnum)) {
 				
 				$stack_layers[] = $d;
 				$numlayers=$numlayers+1;
 			}
 			if (stristr($lnum,'Layer Number') ){
 				$stack_layers['keys']=$d;
 				break;
 			}
 			//echo "<br> $lnum ";
 		}
 		//print_r($stack_layers['keys']);
 		$tmp=$stack_layers['keys'];
 		
 		foreach($tmp as $t){
 			$t=explode('(',$t);
 			$t=trim($t[0]);
 			$sl[]=$t;
 		}
 		
 		$stack_layers['keys']=$sl;
 		
 		//print_r($stack_layers['keys']);
 		
 		/* now parse the structure fields */
 		$k=array_flip($structure_layer_fields);
 		$v=array_values($stack_layers['keys']);
 		$numrecords = count($stack_layers['keys']);
 		$structure_array='';
// 		echo "<br> nr $numrecords";
 		
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			for ($j = 0; $j < $numrecords; $j=$j+1) {
				$structure_array[$i][$k[$v[$j]]]=$stack_layers[$i][$j];
				
				if ($k[$v[$j]] == 'material_name' ) {
					$tmp = $structure_array[$i][$k[$v[$j]]];
					$tmp = explode('-',$tmp);
					$structure_array[$i][$k[$v[$j]]]=trim($tmp[0]);
				}
				
 			}
 		}
 		
 		
 		/* now make sure there are no null entries */
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			
 			$structure_array[$i] = array_filter($structure_array[$i]);
 			unset($structure_array[$i]['']);
 		}
 		
 		//echo "<br> The following structure layer information was extracted:";
 		//$this->echo_r($structure_array);
 		
 		/* now we have to crate the parent structure -  want to extract the other header info and do db insert */
 		
 		$dat=array();
 		foreach ($data as $d) {
 			
 			//print_r($d);
 			$arr_key = array_shift($d);
 			$tmp = explode('(',$arr_key);
 			$newkey = trim($tmp[0]);
 			if ($newkey != NULL ) {
 				$dat[$newkey]=$d;
 			}
 		}
 		
 		$ev='';
 		// first extract the PO information
 		foreach($structure_fields as $key=>$val) {
 			if (count($dat[$val]) > 0 ){
 				if (stristr($key,'date')) {
 					$ev[$key]=$this->parse_date($dat[$val][0]);
 				} else {
 					$ev[$key]=$dat[$val][0];
 				}
 			}
 		}
 		//echo "<br> The following header information was extracted:";
 		//$this->echo_r($ev);
 		$query = "select user_id as user_id from epi_user where user_name='".$ev['user_id']."' LIMIT 1";
 		$row = $this->local_query($query);
 		$ev['user_id']=$row[0]['user_id'];
 		if ($ev['user_id'] == NULL ){
 			$ev['user_id']=1;
 		}
 		/* now do some checking */
 		$query="select count(*) as count from epi_structure where structure_name ='".$ev['structure_name']."'";
 		$row = $this->local_query($query);
 		if ($row != NULL && $row[0]['count'] > 0 ) {
 			echo "<br> ERROR a structure with the name ".$ev['structure_name']." already exists";
 			exit;
 		}
 		$ev['data_file']=$data['data_file'];
 		$query = "insert into epi_structure (".implode(',',array_keys($ev)).") values ('".implode("','",$this->array_sanitize(array_values($ev)))."')";
 		$result = $this->local_insert($query);
 		$structure_id = $this->last_insert();
 		
 		if (!$result ) {
 					echo "<br> ERROR executing: $query";
 		}
 		
 		//$structure_id = 1;
 		if ($structure_id != NULL ){
 			
 			foreach($structure_array as $s) {
 				$s['structure_id']=$structure_id;
 				
 				$query = "insert into epi_structure_layers (".implode(',',array_keys($s)).") values ('".implode("','",$this->array_sanitize(array_values($s)))."')";
 				//echo "<br>".$query;
 				$result = $this->local_insert($query);
 				if (!$result ) {
 					echo "<br> ERROR executing: $query";
 				
 				}
 			}
 			
 		} else {
 			echo "<br> ERROR with inserting the structure";
 			exit;
 		}
 		
 		// echo "<br> The structure was inserted sucessfully";
 		echo "<br> You may view the structure entry <a href=\"".SJ_HOST_NAME."epi/edit/epi_structure.php?structure_id=$structure_id\"> HERE </a>";
 		echo "<br> View and edit the layers here <a href=\"".SJ_HOST_NAME."epi/edit/epi_structure_layers.php?structure_id=$structure_id\"> HERE </a>";
 		
 		
 	}
 
 	/**
     * Function to parse an array into data for the external supplier wafers e.g. epiworks/ spire 
     *
     * @param array
     * @return
     */
 	/**
 	*/
 	function parse_external($data) {
 	
 		$conn = DBManager::getConnection();	
 		
 		/* fields for mapping between db and file */
 		
 		$substrate_fields = array(
 							'substrate_vendor_id'=>'Wafer Vendor', 	 
 							'substrate_name'=>'Wafer Name',	 
 							'substrate_type'=>'Wafer Type', 	
							'substrate_diameter'=>'Wafer Diameter', 	 
							'substrate_thickness'=>'Wafer Thickness', 	 
							'substrate_grade'=>'Wafer Grade', 	 	 
							'substrate_EPD'=>'Wafer EPD', 	 
							'substrate_polished'=>'Wafer Polished', 	 	 
							'substrate_dopant'=>'Wafer Dopant', 	 
							'doped'=>'Wafer Doping Type', 	 
							'doping_level'=>'Wafer Doping level', 	
							'orientation'=>'Wafer Orientation', 		 
							'offcut_angle'=>'Wafer Offcut angle', 	
							'offcut_direction'=>'Wafer Offcut direction', 		 
							'alpha_angle'=>'Wafer Alpha angle', 		 
							'notes'=>'Wafer Notes');
		$wafer_fields = array(
							'wafer_name'=>'Growth Run IDs',
							'wafer_number'=>'Growth Platen Location ID/Wafer #',
							'substrate_sn'=>'Wafer Scribe IDs'
							);
		$external_fields = array(
							'vendor_name'=>'Epi Vendor Name',
							'ordered_by'=>'Ordered By',
							'notes'=>'Notes/Description',
							'date_ordered'=>'Date Ordered',
							'date_received'=>'Date Received',
							'po_number'=>'PO #',
							'product_id'=>'Product ID');
							
//		$structure_fields =array('
 		
 		$layer_fields = array(
 		//	'growth_id',
 			'layer_number'=>'Layer #',
 			'layer_function'=>'Layer Function',
 			'material'=>'Material',
 			'thickness'=>'Thickness',
 			'dopant'=>'Dopant',
 			'concentration'=>'Concentration High',
 			//'si_est_carrier_conc',
 			//'be_est_carrier_conc',
 			//'cbr4_est_carrier_conc',
 			'notes'=>'Comments'
 			);
 		$layer_dopant_fields = array(
 			'si'=>'si_est_carrier_conc',
 			'c' =>'cbr4_est_carrier_conc',
 			'be'=>'be_est_carrier_conc'
 		);
 		
 		$structure_fields = array(
 		//	'growth_id',
 			'layer_number'=>'Layer #',
 			'layer_function'=>'Layer Function',
 			'material_name'=>'Material',
 			'thickness'=>'Thickness',
 			'dopant_name'=>'Dopant',
 			'doping_level_hi'=>'Concentration High',
 			'doping_level_lo'=>'Concentration Low',

 			//'si_est_carrier_conc',
 			//'be_est_carrier_conc',
 			//'cbr4_est_carrier_conc',
 			'notes'=>'Comments'
 			);
 		
 		$growth_run_params = array(
 							'growth_run');
 		
 		/* extract data */
 		
 		$datalength = count($data);
 		
 		/* start with the growth stack */
 		
 		// get the growth stack -
 		$numlayers = 0;
 		for ($i=($datalength-1);$i > 0; $i=$i-1) {
 		
 			$d=$data[$i];
 			$lnum=$d[0];
 			if (is_numeric($lnum)) {
 				
 				$stack_layers[] = $d;
 				$numlayers=$numlayers+1;
 			}
 			if (stristr($lnum,'Layer #') ){
 				$stack_layers['keys']=$d;
 				break;
 			}
 		
 		}
 		
 		$tmp=$stack_layers['keys'];
 		
 		foreach($tmp as $t){
 			$t=explode('(',$t);
 			$t=trim($t[0]);
 			$sl[]=$t;
 		}
 		
 		$stack_layers['keys']=$sl;
 		/* now parse the structure fields */
 		$k=array_flip($structure_fields);
 		$v=array_values($stack_layers['keys']);
 		$numrecords = count($stack_layers['keys']);
 		$structure_array='';
// 		echo "<br> nr $numrecords";
 		
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			for ($j = 0; $j < $numrecords; $j=$j+1) {
				$structure_array[$i][$k[$v[$j]]]=$stack_layers[$i][$j];
				if ($k[$v[$j]] == 'material_name' ) {
					$tmp = $structure_array[$i][$k[$v[$j]]];
					$tmp = explode('-',$tmp);
					$structure_array[$i][$k[$v[$j]]]=trim($tmp[0]);
				}
 			}
 		
 		
 		
 		}
 		
 		
 		/* now make sure there are no null entries */
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			
 			$structure_array[$i] = array_filter($structure_array[$i]);
 			unset($structure_array[$i]['']);
 		}
 		
 		//echo "<br> The following structure layer information was extracted:";
 		
 		// $this->echo_r($structure_array);
 		
 		
 		/* now parse defunct layer fields */
 		/* 
 			this is no longer needed, but keep for a bit
 		*/
 		
 		
// 		echo "<br> $numlayers growthlayers";
 		
 		$k=array_flip($layer_fields);
 		$v=array_values($stack_layers['keys']);
 		$numrecords = count($stack_layers['keys']);
 		$layer_array='';
// 		echo "<br> nr $numrecords";
 		
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			for ($j = 0; $j < $numrecords; $j=$j+1) {
				$layer_array[$i][$k[$v[$j]]]=$stack_layers[$i][$j];
 			}
 		}
 		
 		for ($i=0; $i < $numlayers; $i=$i+1) {
 			
 			$cvalue = trim($layer_array[$i]['concentration']);
 			$dvalue = strtolower(trim($layer_array[$i]['dopant']));
 			unset($layer_array[$i]['concentration']);
 			if (in_array($dvalue,array_keys($layer_dopant_fields))) {
 				$layer_array[$i][$layer_dopant_fields[$dvalue]]=$cvalue;
 			}
 			
 		}

		for ($i=0; $i < $numlayers; $i=$i+1) {
 			
 			$layer_array[$i] = array_filter($layer_array[$i]);
 			unset($layer_array[$i]['']);
 			
 		}

 //		$this->echo_r($stack_layers);
 		
 	//	echo "<br> The following layer information was extracted:";
 		
// 		$this->echo_r($layer_array);
 	
// 		$this->array_to_table($layer_array);		
 		
 		/* now do the rest of the data */
 		foreach ($data as $d) {
 			
 			//print_r($d);
 			$arr_key = array_shift($d);
 			$tmp = explode('(',$arr_key);
 			$newkey = trim($tmp[0]);
 	//		echo "<br> $newkey";
 			if ($newkey != NULL ) {
 				$dat[$newkey]=$d;
 	//			echo "<br> XXX ";
 	//		print_r($dat[$newkey]);
 	//		echo "<br>";
 			}
 			
 			
 		}
 		
 		//echo "<br> Importing PO info <br>";
 		
 		$ev='';
 		// first extract the PO information
 		foreach($external_fields as $key=>$val) {
 			if (count($dat[$val]) > 0 ){
 				if (stristr($key,'date')) {
 					$ev[$key]=$this->parse_date($dat[$val][0]);
 				} else {
 					$ev[$key]=$dat[$val][0];
 				}
 			}
 		}
 		
// 		// $this->echo_r($ev);
 		
 		/* open transaction */
 		
 		$this->transaction_open();
 		
 		
 		/* before we insert the external record let's check if this already existed  - PO_number should be unique.
		 if so, we'll delete the old record and all the wafers and growths in it, and then add from scratch rather than updating */
		 
		$query = "select count(*) as count from epi_external where po_number = '".$ev['po_number']."'";
		
		$result = $conn->query($query);
		
		if ($row = $result->fetch_assoc()) {
		
			if($row['count'] > 0) {
				$query = "select external_id as external_id from epi_external where po_number = '".$ev['po_number']."'";
		
				$result = $conn->query($query);
				
				if ($row = $result->fetch_assoc()) {
					if ($row['external_id'] > 0) {
						
						echo "<br>Error a PO with PO # : ".$ev['po_number']." Already exists. Please delete the duplicate before uploading again.";
 						echo "<br>Cannot proceed with the insert.";
 			
 						$this->transaction_rollback();
 						$this->transaction_close();
 						exit;
					
					
					//	$this->delete_external_epi($row['external_id']);
					
					}
				}
			}		
		}
 		
 		
 		if(stristr($ev['vendor_name'],'Epiworks')) {
 			$vendor = 'Epiworks';
 			
 		} elseif (stristr($ev['vendor_name'],'Spire')) {
 			$vendor = 'Spire';
 		} else {
 			echo "<br>Error unknown Epi Vendor : ".$ev['vendor_name'].". Allowed values are Spire and Epiworks";
 			echo "<br>Cannot proceed with the insert. Please contact the system administrator if a new vendor should be added";
 			
 			$this->transaction_rollback();
 			$this->transaction_close();
 			exit;
 		}
 		
		$query = "select chamber_name as chamber_name from epi_growth_chamber where chamber_source like '$vendor'";
//		echo "<br> VQ $query";
		$result= $conn->query($query);
		$row = $result->fetch_assoc();
		$chamber_name = $row['chamber_name'];
// 		echo "<br>Chamber name $chamber_name<br>";
 		
 		/* now let's get the wafers .
 		   clear all null elements from the wafer names - this will tell us how many new wafers we have */

 		$dat[$wafer_fields['wafer_name']]=array_filter($dat[$wafer_fields['wafer_name']]);
 		
 		$wafernum = count($dat[$wafer_fields['wafer_name']]);
 		
 		//echo "<br> Importing $wafernum wafers <br>";
 		
 		$waf='';
 		$sub='';
 		for ($i=0; $i<$wafernum; $i=$i+1 ){
 		
 			$sub[$i]['date_ordered']=$ev['date_ordered'];
 			$sub[$i]['date_received']=$ev['date_received'];
 			$waf[$i]['date_created']=$ev['date_ordered'];
 			
 			foreach($wafer_fields as $key=>$val) {
 				$waf[$i][$key]=$dat[$val][$i];
 			}
 			foreach($substrate_fields as $key=>$val) {
 				$sub[$i][$key]=$dat[$val][$i];
 			}
 			
 		}
 		
 		
	 	
 		
// 		echo "<br> wafer and substrate info<br>";
// 		// $this->echo_r($waf);
// 		// $this->echo_r($sub);
 		
 		/* enter the substrate info - duplicate substrates are allowed ... */
 		
 		for ($i=0; $i<$wafernum; $i=$i+1 ){
 			$ssub=$sub[$i];
 			$tmp=array_pop($ssub);
 			$subcomp[$i]=serialize($ssub);
 		}
 		
 		$subunique = array_unique($subcomp);
 		
 		$unique_keys = array_keys($subunique);
 		// $this->echo_r($unique_keys);
 		
 		/* now for each unique key, create a new substrate */
 		
 		foreach ($unique_keys as $key ){
			
			/* if substrate already exists create a new one with a unique name ... 
			  check how many we have with the vendor - i.e. if there are 5 AXT wafers from Epiworks, this will be Epiworks-6 */
			  
 			$query = "select count(*) as count from epi_substrate where substrate_name like '%".$sub[$key]['substrate_name']."%' ";
 //			echo "<br>".$query;
 			$result = $conn->query($query);
 			if (($row = $result->fetch_assoc()) != NULL ) {
 				$count = $row['count'];
 			} else {
 				$count = 0;
 			}
 			$sub[$key]['substrate_name']=$sub[$key]['substrate_name'].'-'.($count+1);
 			
 			/* get the right vendor id */
 			
 			$sub_vedor_name=$sub[$key]['substrate_vendor_id'];
	 		$query2  = "select substrate_vendor_id from epi_substrate_vendor where name like '%$sub_vendor_name%' LIMIT 1";
	 		$result2 = $conn->query($query2);
	 		if ($row2 = $result2->fetch_assoc()){ 
	 			$sub[$key]['substrate_vendor_id'] = $row2['substrate_vendor_id'];
	 		} else {
	 			$sub[$key]['substrate_vendor_id'] = 9;
	 		}

 			$sub[$key]['number_left']=0;

 			$query = "insert into epi_substrate (".implode(',',array_keys($sub[$key])).") values ('".implode("','",$this->array_sanitize(array_values($sub[$key])))."')";
			$substrate_name[]=$sub[$key]['substrate_name'];
// 			echo "<br>".$query;
 
 			$result = $conn->query($query);
			$substrate_ids[$key] = $conn->insert_id;
// 			$substrate_ids[$key]=$key;
 		
 		}

// 		// $this->echo_r($subtrate_ids);
		
		//echo "<br> Inserting substrates ".implode(', ',$substrate_name)."<br>";
		
 		/* now for each of those insert wafer and wafer ids */
 		
 		for($i=0; $i<count($waf) ; $i=$i+1) {
 			
 			if (in_array($i,$unique_keys)) {
 				$substrate_id=$substrate_ids[$i];
 			}
 			
 			$waf[$i]['substrate_id']=$substrate_id;
 			
 			$growth_name[$i]=$waf[$i]['wafer_name'];
 			$wafer_number[$i]=$waf[$i]['wafer_number'];
 			$wafer_name[$i]=$chamber_name.'-'.$growth_name[$i].'-'.$wafer_number[$i];
 			
 			/* check if we're not trying to insert a duplicate wafer */
 			if($this->get_wafer_id_by_name($wafer_name[$i]) !== 0) {
 				echo "<br>Error: duplicate wafer with name ".$wafer_name[$i];
 				echo "<br>Cannot proceed with the insert. Please contact the system administrator if this is an error<br>";
 			
 				$this->transaction_rollback();
 				$this->transaction_close();
 				exit;
 			}
 			
 			if (!isset($growth_wafer_count[$growth_name[$i]]) ) {
 					$growth_wafer_count[$growth_name[$i]]=1;
 			} else {
	 				$newnum=($growth_wafer_count[$growth_name[$i]]+1);
	 				$growth_wafer_count[$growth_name[$i]]=$newnum;
	 		}
 			
 			
// 			// $this->echo_r($growth_wafer_count);
 			
 			$waf[$i]['wafer_name']=$wafer_name[$i];
 			$wafinsert[$i]=$waf[$i];
 			unset($wafinsert[$i]['wafer_number']);
 			$query  = "INSERT INTO epi_wafer (".implode(',',array_keys($wafinsert[$i])).") ";
			$query .= "VALUES ('".implode("','",$this->array_sanitize(array_values($wafinsert[$i])))."')";

// 			echo "<br> $query";
			$result = $conn->query($query);
			$wafer_ids[$i] = $conn->insert_id;
//			$wafer_ids[$i]=$i;	
 		}
//		 // $this->echo_r($wafer_ids);
		$date_added=date("Y-m-d H:i:s");
		
		/* first create the structure and structure layers */
		$query = "select user_id from epi_user where user_name like('%".trim($ev['ordered_by'])."%')";
		$result = $conn->query($query);
		$row=$result->fetch_assoc();
		
		if($row == NULL) {
			$user_id = 1;
		} else {
			$user_id = $row['user_id'];
		}
		if ($user_id ==NULL ) $user_id = 1;
		
		
		/* use the following convention for now: structure_name  = Vendor-POnumber */
		$query = "insert into epi_structure
					(structure_name,structure_description,date_added,user_id,status)
				 values
				 	('".$vendor.'-'.$ev['po_number']."','PO number ".$ev['po_number']."','".$date_added."','".$user_id."','Available')";
		
		$result = $conn->query($query);
		$structure_id = $conn->insert_id;
		
		/* now for this structure create the layers */
		for ($l=0; $l<count($structure_array); $l=$l+1) {
				$structure_array[$l]['structure_id']=$structure_id;
				$query2 = "insert into epi_structure_layers (".implode(',',array_keys($structure_array[$l])).") 
							values ('".implode("','",$this->array_sanitize(array_values($structure_array[$l])))."')";
				
				//echo "<br> Structure Insert : $query2";
				$result2 = $conn->query($query2);
		}
		
			
		/* now that we've inserted wafers do the growth */
		
		// now create the growth an the growth-wafer relation so we don't violate the key contraints on the relation table
 		$unique_growths = array_unique($growth_name);
 		
 		//echo "<br> Inserting growths ".implode(', ',$unique_growths)."<br>";
 		//echo "<br> Inserting wafers ".implode(', ',$wafer_name)."<br>";
 		
// 		// $this->echo_r($growth_wafer_count);
 		foreach($unique_growths as $ug) {
 		
 			/* check if we're not trying to insert a duplicate growth */
 			if($this->get_growth_id_by_name($ug) !== 0) {
 				echo "<br>Error: duplicate growth with name ".$ug;
 				echo "<br>Cannot proceed with the insert. Please contact the system administrator if this is an error<br>";
 			
 				$this->transaction_rollback();
 				$this->transaction_close();
 				exit;
 			}
 		
 		
 			$growth_date = $ev['date_received'];
 			
 			$growth_wafer_number = $growth_wafer_count[$ug];
 			$recipe_name = addslashes('PO:'.$ev['po_number']);
 			$query = "insert into epi_growth 
 					(grower_id,structure_id,chamber_id,growth_run,growth_wafer_number,recipe_name,growth_date,date_added,growth_type) 
 					values
 					(6,$structure_id,'$chamber_name','$ug','$growth_wafer_number','$recipe_name','$growth_date','$date_added','standard')";
// 			echo "<br> GQ $query";
 			$result = $conn->query($query);
			$growth_ids[$ug] = $conn->insert_id;
//			$growth_ids[$ug]=$ug;
			
			// now add the layers
			
			/* now do the layer */
			
			for ($l=0; $l<count($layer_array); $l=$l+1) {
				$layer_array[$l]['growth_id']=$growth_ids[$ug];
				$query2 = "insert into epi_growth_layers (".implode(',',array_keys($layer_array[$l])).") 
							values ('".implode("','",$this->array_sanitize(array_values($layer_array[$l])))."')";
				//echo "<br> GROWTH: $query";
				//echo "<br> LAYER: $query2";
				$result2 = $conn->query($query2);
			}
			//echo "<br> Inserting ".count($layer_array)." layers for growth ".$ug."<br>";
 		}
 		
 		/* now create the growth structure relations*/
 		
 		foreach ($growth_ids as $growth_id) {
 			$this->add_growth_structure_relation($growth_id,$structure_id);
 		}
 		
 		// we now have to update the relational tables
 		// growth - wafer
 		// external - growth -wafer
 		// add array of growths and wafer ids to external - comma separated.
 		// upload file and html of the table for quick viewing into external 
 		
 		// now finally add the external record
 		// growth_ids has all the growth ids added
 		// wafer_ids has all the wafer ids
 		// substrate_ids has all the substrates
 		
 		/* now create the PO record and relations */
 		
 		$ev['data_file']=$data['data_file'];
 		$ev['data_html']=(htmlspecialchars($this->array_to_table_html($data)));
 		$ev['data_array']=serialize($data);
 		$ev['growth_ids']=implode(',',$growth_ids);
 		$ev['wafer_ids']=implode(',',$wafer_ids);
 		$ev['substrate_ids']=implode(',',$substrate_ids);
 		$ev['structure_ids']=$structure_id;
// 		// $this->echo_r($this->array_sanitize($ev));
 		$query = "insert into epi_external (".implode(',',array_keys($ev)).") values ('".implode("','",$this->array_sanitize(array_values($ev)))."')";
//		echo "<br> $query";
		$result = $conn->query($query);
		$external_id = $conn->insert_id;
//		$external_id=1;
 		
 		if($external_id == NULL ){
 				echo "<br>Error: could not create PO record";
 				echo "<br>Cannot proceed with the insert. Please contact the system administrator<br>";
 			
 				$this->transaction_rollback();
 				$this->transaction_close();
 				exit;
 		}
 		
 		for($i=0; $i<$wafernum; $i++){
			$wafer_id = $wafer_ids[$i];
			$growth_id = $growth_ids[$growth_name[$i]];
			$wafer_num=$wafer_number[$i];
			$substrate_id = $waf[$i]['substrate_id'];
			$date_added = date('Y-m-d H:i:s');
 			$query = "insert into epi_external_wafer_relation 
 						(external_id,wafer_id,growth_id,substrate_id,date_added)
 						values 
 						('$external_id','$wafer_id','$growth_id','$substrate_id','$date_added')
 						";
 			//echo "<br> $query";
 			$result = $conn->query($query);
 			
 			if(!$result ){
 				echo "<br>Error: could not create relation";
 				echo "<br>Cannot proceed with the insert. Please contact the system administrator<br>";
 				$this->transaction_rollback();
 				$this->transaction_close();
 				exit;
 			}
 			
 			$query = "insert into epi_growth_wafer_relation 
 						(wafer_id,growth_id,substrate_id,wafer_number,date)
 						values 
 						('$wafer_id','$growth_id','$substrate_id','$wafer_num','$date_added')
 						";
 			//echo "<br> $query";
 			$result = $conn->query($query);
 			if(!$result ){
 				echo "<br>Error: could not create relation";
 				echo "<br>Cannot proceed with the insert. Please contact the system administrator<br>";
 				$this->transaction_rollback();
 				$this->transaction_close();
 				exit;
 			}
 		
 		}
 		
 		
 		// now that we have this, let's do the layers.
 		
 		/* if we made it, commit and close */
 		
 		$this->transaction_commit();
 		$this->transaction_close();
 		
 		
 	// eof	
 	}
 	
 	function echo_error($text){ 
 	
 		echo "<br><span style=\"error\"> ERROR: $text </span></br>";
 	
 	}
 	
 	function check_exists($table,$column,$value){ 
 	
 		$query= "select count(*) as count from $table where $column='$value'";
 	
 		$result = $this->local_query($query);
 		
 		if ($result == NULL ) {
 			return FALSE;
 		} else {
 			return $result[0]['count'];
 		}
 	}

 	function get_field($field,$table,$column,$value){ 
 	
 		$query= "select $field from $table where $column='$value'";
 		//echo "<br> gfn $query";
 		$result = $this->local_query($query);
 		
 		if ($result == NULL ) {
 			return FALSE;
 		} else {
 			return $result[0][$field];
 		}
 		
 	}

 	
 	
    /**
     * Function to parse an array into data for the external supplier wafers e.g. epiworks/ spire 
     *
     * @param array
     * @return
     */
     function parse_external_multi($data)
     {
 	
 		$conn = DBManager::getConnection();	
 		
 		/* fields for mapping between db and file */
 		$qq=TRUE;
 		$substrate_fields = array(
 						'substrate_vendor_id'=>'Wafer Vendor', 	 
 						'substrate_name'=>'Substrate Name',	 
 						'substrate_type'=>'Wafer Type', 	
						'substrate_diameter'=>'Wafer Diameter', 	 
						'substrate_thickness'=>'Wafer Thickness', 	 
						'substrate_grade'=>'Wafer Grade', 	 	 
						'substrate_EPD'=>'Wafer EPD', 	 
						'substrate_polished'=>'Wafer Polished', 	 	 
						'substrate_dopant'=>'Wafer Dopant', 	 
						'doped'=>'Wafer Doping Type', 	 
						'doping_level'=>'Wafer Doping level', 	
						'orientation'=>'Wafer Orientation', 		 
						'offcut_angle'=>'Wafer Offcut angle', 	
						'offcut_direction'=>'Wafer Offcut direction', 		 
						'alpha_angle'=>'Wafer Alpha angle', 		 
						'notes'=>'Wafer Notes');
		$wafer_fields = array(
						'wafer_name'=>'Growth Run IDs',
						'wafer_number'=>'Growth Platen Location ID/Wafer #',
						'substrate_sn'=>'Wafer Scribe IDs'
						);
		$external_fields = array(
						'vendor_name'=>'Epi Vendor Name',
						'ordered_by'=>'Ordered By',
						'notes'=>'Notes/Description',
						'date_ordered'=>'Date Ordered',
						'date_received'=>'Date Received',
						'po_number'=>'PO #');
							
//		$structure_fields =array('
 		 		
 		$structure_fields = array(
 		//	'growth_id',
 			'layer_number'=>'Layer #',
 			'layer_function'=>'Layer Function',
 			'material_name'=>'Material',
 			'thickness'=>'Thickness',
 			'dopant_name'=>'Dopant',
 			'doping_level_hi'=>'Concentration High',
 			'doping_level_lo'=>'Concentration Low',

 			//'si_est_carrier_conc',
 			//'be_est_carrier_conc',
 			//'cbr4_est_carrier_conc',
 			'notes'=>'Comments'
 			);
 		
 		$growth_run_params = array(
 							'growth_run');
 		
 		/* extract data */
 		
 		$datalength = count($data);
 		
 		/* start with the growth stack */
 		for ($i=0;$i<$datalength; $i++) {
 			if (strlen(implode('',$data[$i])) > 0) {
 				$dat[]=$data[$i];
 			}
 		
 		}
 		// $this->echo_r($dat);
 		
 		
 		$datalength = count($dat);
 		$odi=-1;
 		for ($i=0; $i<count($dat); $i++) {
 			$key = array_shift($dat[$i]);
 			
 			if ($key == 'Order Details:') {
 				$odi=$i;
 				break;
 			} else {
 				$key = explode('(',$key);
 				$key = $key[0];
 				$d[$key]=$dat[$i];
 			}
 		}
 		
 		// $this->echo_r($d);
 		
 		foreach ($external_fields as $k=>$v) {
 			$ev[$k] = trim($d[$v][0]);
 		}
 		
 		$ev['date_ordered']=$this->parse_date($ev['date_ordered']);
 		$ev['date_received']=$this->parse_date($ev['date_received']);

 		
 		if(!isset($ev['po_number']) || $ev['po_number'] == NULL ){
 			$this->echo_error('Invalid PO number. Please review your data');
 			exit;
 		} elseif ($this->check_exists('epi_external','po_number',$ev['po_number'])) {
 			$this->echo_error('This PO number has already been entered. Please check the database.');
 			exit;
 		}  		
 		// $this->echo_r($ev);
 		
 		/*  now parse the PO data */
 		if ($odi < 0 ) {
 			$this->echo_error('Could not extract the order details. Something is wrong with your input.');
 			exit;
 		}
 		
 		for ($i=$odi+1;$i<$datalength;$i++) {
 			$str = $dat[$i];
 			$key = array_shift($str);
 			if ($key == 'Product ID') {
 				for($j=0;$j<count($dat[$i]);$j++) {
 					$wvkeys[$dat[$i][$j]]=$j;
 				}
 				$odi=$i;
 				$ncols = count($dat[$i]);
 				break;
 			}
 		
 		}
 		
 		
 //		// $this->echo_r($keys);
 		for ($i=$odi+1;$i<$datalength;$i++) {
	 		$str = $dat[$i];
 			$key = array_shift($str);
 			if ($key == 'Structure Details:' || $key == 'Substrate Details:') {
 				$odi = $i;
 				break;
 			}
 			for ($j=0;$j<$ncols;$j++) {
 				
 				$wv[$i][$j]=$dat[$i][$j];
 				if ($wv[$i][$j] === NULL || strlen($wv[$i][$j]) == 0 || !isset($wv[$i][$j])  && $i>0) {
 					$wv[$i][$j] = $wv[$i-1][$j];
 				}
 				
 			}
 		}
 		
 		
 //		// $this->echo_r($wv);
 		/* now if we're not at the end of data  there is structure info and substrate info */
 		
 		if ($key == 'Structure Details:') {
 			//echo "<br> Structure Details";
 			$si=0;
 			$sv=array();
 			$tmp = array();
	 		for ($i=$odi+1;$i<$datalength;$i++) {
 				$str = $dat[$i];
 				$key = array_shift($str);
 				if ($key == 'Substrate Details:') {
 					if (count($tmp) > 0)  {
 						$sv[$si] = $tmp;
 					}
 					
 					$odi = $i;
 					break;
 				}
 				
 				if (strtolower($key) == 'structure name' && count($tmp) > 0) {
 					/* starting a new structure */
 				//	echo "<br> new structure $si";
 					$sv[$si] = $tmp;
 					$si = $si +1;
 					$tmp = array();
 				
 				}
 			//	if (strlen(trim(implode('',$dat[$i]))) > 0  ) {
 						$tmp[]=$dat[$i];
 			//	}
 				
 			
 			
 			}
 			
 		} 
 		
 //		// $this->echo_r($sv);
 		
 		if ($key == 'Substrate Details:') {
	 		$si=0;
 			$suv=array();
 			$tmp = array();
	 		for ($i=$odi+1;$i<$datalength;$i++) {
 				$str = $dat[$i];
 				$key = array_shift($str);
 				$key = explode('(',$key);
 				$key = trim($key[0]);
 				if (strtolower($key) == strtolower('Substrate Name') && count($tmp) > 0) {
 					/* starting a new structure */
 					
 					$suv[$si] = $tmp;
 					$si = $si +1;
 					$tmp = array();
 				
 				}
 				if (strlen(trim(implode('',$dat[$i]))) > 0  ) {
 						$tmp[]=$dat[$i];
 				}
 				
 			
 			
 			}
 			
 			
 			if (count($tmp) > 0)  {
 				$suv[$si] = $tmp;
 			}
 			
	 	}
 		
 		
 	
 	
 		/* now that everything has been inserted */
 		
 		
 		/* create substrate */
 		$substrate_fields=array_flip($substrate_fields);
 		
 		
 		// $this->echo_r($substrate_fields);
 		
 		foreach ($suv as $s) {
 			
 			$doInsert = TRUE;
 			$insert=array();
			for($i=0;$i<count($s); $i++) {
				$str = $s[$i];
 				$key = array_shift($str);
				//echo "<br> $key =>".$str[0];
				if (strtolower($key) == 'substrate name') {
					$substrate_name = $str[0];
					if ($this->check_exists('epi_substrate','substrate_name',$substrate_name)) {
						//echo "<br> Substrate $substrate_name already exists";
						$doInsert=FALSE;
					}
					
				}
				
				//if(!$doInsert) { break; }
				
				$insert[$substrate_fields[$key]]=addslashes(trim($str[0]));
				//echo "<br> ".$substrate_fields[$key]." $key x ".addslashes(trim($str[0]));
			}
			if ($doInsert) {
				$insert=array_filter($insert);
				
				$query = "select substrate_vendor_id from epi_substrate_vendor where name='".$insert['substrate_vendor_id']."'";
				$result = $this->local_query($query);
				$insert['substrate_vendor_id']=$result[0]['substrate_vendor_id'];
				
				$query = "insert into epi_substrate (".implode(',',array_keys($insert)).") values ('".implode("','",$this->array_sanitize(array_values($insert)))."')";
				//echo "<br> Adding substrate $substrate_name <br>";
				//echo "<br> New Sub query = $query<br>";
				if ($qq) { 
 					$result = $this->local_insert($query); 
 				}
			
			
			}

 		
 		
 		}
 		
 		 
 		/* create structure */
 		 
 		// $this->echo_r($sv);
 		
 		//echo "<br> Found ".count($sv)." structures ";
 		
 		foreach ($sv as $s) {
 			$doInsert = TRUE;
 			$insert=array();
 			for($i=0;$i<count($s); $i++) {
				$str = $s[$i];
 				$key = array_shift($str);
 				if (strtolower($key) == 'structure name') {
 					
					$structure_name = $str[0];
					
					//echo "<br> inserting structure $structure_name $doInsert";
					//$this->echo_r($s);
					if ($this->check_exists('epi_structure','structure_name',$structure_name)) {
						//echo "<br> Structure $structure_name already exists";
						$doInsert=FALSE;
					}
					
				}
				//$insert[]=$s[$i];
 			}
 			
 			if ($doInsert && $structure_name != NULL) {
 				//echo "<br> Adding New Structure $structure_name <br>";
				$this->parse_structure($s);
			}
 		
 		
 		}
 		/* create PO */
 		
 		
 		
 		$ev['data_file']=$data['data_file'];
 		$query = "insert into epi_external (".implode(',',array_keys($ev)).") values ('".implode("','",$this->array_sanitize(array_values($ev)))."')";
 		//echo "<br> Adding new PO ".$ev['po_number'];
 		
 		
 		if ($qq) { 
 					$result = $this->local_insert($query); 
 					$external_id = $this->last_insert();
 		} else {
 					$external_id = 0;
 		}	
 		
 		
 		/* create wafers and growth */
 	
 		$query = "select chamber_name as chamber_name from epi_growth_chamber where chamber_source like '".$ev['vendor_name']."'";
 		$row= $this->local_query($query);
 		$chamber = $row[0]['chamber_name'];
 		$pids=array();
 		$rids=array();
 		$wids=array();
 		$wnames = array();
 		$sids=array();
 		$scids = array();
 		//$this->echo_r($wv);
 		$date_added = date("Y-m-d H:i:s");
 		foreach ($wv as $w) {
 			$product_id = addslashes($w[$wvkeys['Product ID']]);
 			$growth_run = addslashes($w[$wvkeys['Run #']]);
 			$wafer_id   = addslashes($w[$wvkeys['Wafer ID']]);
 			$substrate_sn  = addslashes($w[$wvkeys['Wafer Scribe ID']]);
 			$processed  = addslashes($w[$wvkeys['Processed (Y/N)']]);
 			$growth_structure  = addslashes($w[$wvkeys['Structure']]);
 			$substrate  = addslashes($w[$wvkeys['Substrate']]);
 			$comments   = addslashes($w[$wvkeys['Comments']]);
 			
 			if (!in_array($product_id,$pids)) {
 				$pids[]=$product_id;
 			}
 			
 			if (!in_array($growth_run,array_keys($rids))) {
 				/* need to create growth run and store the id */
 				$query = "select structure_id as structure_id from epi_structure where structure_name = '$growth_structure'";
 				$result= $this->local_query($query);
				$structure_id = $result[0]['structure_id'];
				$source = $ev['vendor_name'];
				$sids[$growth_structure] = $structure_id;
 				$grower_id =6;

 				$query = "select chamber_name as chamber_name from epi_growth_chamber where chamber_source like '$source'";
 				//echo $query;
				$result= $this->local_query($query);
				$chamber_id = $result[0]['chamber_name'];
				
				
 				$query = "insert into epi_growth (structure_id,grower_id,chamber_id,growth_structure,growth_run,date_added,source,growth_name)
 											values ('$structure_id','$grower_id','$chamber_id','$growth_structure','$growth_run','$date_added','$source','".($chamber_id.'-'.$growth_run)."')";
 				//echo "<br>GQ:<br> $query ";
 				
 				//echo "<br> Added growth $growth_run <br>";
 				
 				if ($qq) { 
 					$result = $this->local_insert($query); 
 					$growth_id = $this->last_insert();
 					//echo "<br> growth_id = $growth_id";
 					$rids[$growth_run] = $growth_id;
 				} else {
 					$rids[$growth_run] = 1;
 				}	
 				
 				
 				
 				$query = "insert into epi_growth_structure_relation (structure_id,growth_id) values 
 															('$structure_id','".$rids[$growth_run]."')";
 				//echo "<br>GSR:<br> $query ";
 				
 				if ($qq) { $result = $this->local_insert($query); }
 				
 			}
 			
 			$query = "select substrate_id as substrate_id from epi_substrate where substrate_name = '$substrate'";
 			$result= $this->local_query($query);
			$substrate_id = $result[0]['substrate_id'];
 			$suids[$substrate]=$substrate_id;
 			
 			$wafer_name = $chamber_id.'-'.$growth_run.'-'.$wafer_id;
 			
 			$query  = "INSERT INTO epi_wafer (wafer_name, growth_id, substrate_id, substrate_sn, date_created, comments, user_id,processed) "; 
 			$query .= "VALUES ('$wafer_name','".$rids[$growth_run]."','$substrate_id','$substrate_sn','$date_added','$comments','1','$processed')";

 			//echo "<br>WQ:<br> $query ";
 			
 			//echo "<br> Added wafer $wafer_name <br>";
 			
 			if ($qq) { 
 				$result = $this->local_insert($query); 
 				$wids[$wafer_name] = $this->last_insert();
 			} else {
 			$wids[$wafer_name] = 2;
 			}
 			//echo "<br> adding $product_id $run_number $wafer_id $substrate_sn $processed $structure $substrate $comments";
 		
 			/* update growth wafer relation */
 			$query = "insert into epi_growth_wafer_relation (wafer_id,growth_id,substrate_id,date,wafer_number,growth_number) values 
 															('".$wids[$wafer_name]."','".$rids[$growth_run]."','$substrate_id','$date_added','$wafer_id','$growth_run')";
 			//echo "<br> GWR:<br> $query ";
 			
 			if ($qq) { $result = $this->local_insert($query); }
 			
 			$query = "insert into epi_external_wafer_relation (external_id,wafer_id,growth_id,substrate_id,date_added) values 
 															('$external_id','".$wids[$wafer_name]."','".$rids[$growth_run]."','$substrate_id','$date_added')";
 			//echo "<br> EWR:<br> $query ";												
 			if ($qq) { $result = $this->local_insert($query); }
 		
 															
 		}
 	
 		/* now update the external table to have all the wafer ids etc. */
 		$query = "update epi_external set structure_ids = '".implode(",",array_values($sids))."',
 										substrate_ids = '".implode(",",array_values($suids))."',
 										  growth_ids    = '".implode(",",array_values($rids))."',
 										  wafer_ids     = '".implode(",",array_values($wids))."',
 										  product_id    = '".implode(",",array_values($pids))."'
 								      where external_id = $external_id";
 		//echo "<br> $query";
 		
 		if ($qq){ $result = $this->local_insert($query); }
 		
 		
 	
 	// eof	parse_external_multi
 	}
 	
 
 
 	/* function for getting the latest template for a particular action */
 	
 	function get_template_file($action) {
 		
 		$conn = DBManager::getConnection();	
 		$tname = '';
 		$tpath = EPI_BASE_URL.'/edit/files/templates/';
 		
 	
 		if ($action !='') {
 			$query  = "select template_id as template_id, data_file as data_file from epi_template where name like '%$action%' order by template_id desc LIMIT 1";
 			//echo "$query";
 			$result = $conn->query($query);
 			if ($row = $result->fetch_assoc()) {
 				return $tpath.$row['template_id'].'.'.$row['data_file'];
 			}
 		}
 		return $tpath;
 	}
 	/* end of function */
 
 
 	/* functions to manage transactions in db */
 	
 	function transaction_open(){
 		$conn = DBManager::getConnection();	
		if ($this->transaction_is_open() ) {
			return TRUE;
		} 
// 		echo "<br>transaction open";
		$query="set autocommit=0";
		$result = $conn->query($query);
		$query="BEGIN";
		$result = $conn->query($query);
		$check=$this->transaction_is_open();
		return $check;
	}
	
	function transaction_is_open(){ 
		$conn = DBManager::getConnection();	
		$query="select @@autocommit";
//		echo "<br>checking if transaction is open";
		$result = $conn->query($query);
		$row=$result->fetch_assoc();

		return ($row['@@autocommit'] == 0);
		
	}
	
	function transaction_close(){
		$conn = DBManager::getConnection();	
//		echo "<br>closing transaction";
		$query="set autocommit=1";
		$result = $conn->query($query);
		$check=$this->transaction_is_open();
		return $check;
	}

	function transaction_commit(){
		$conn = DBManager::getConnection();	
//		echo "<br> committing transaction";
		$query="COMMIT";
		return $conn->query($query);
	}

	function transaction_rollback(){
		$conn = DBManager::getConnection();	
//		echo "<br> rolling back transaction";
		$query="ROLLBACK";
		return $conn->query($query);	
	}
 
 
 /* end of functions to manage transactions */
 
 
 /* mbe functions */
 
 function mbe_key_to_sql_short($mbe_key) {
 	return str_replace('%','pcnt',str_replace('Be/CBr4','BeCBr4',str_replace(' ','__',str_replace(')','___',str_replace('(','___',trim($mbe_key))))));
 }
 function mbe_key_to_sql_long($mbe_key) {
 	return str_replace('%','pcnt',str_replace('Be/CBr4','BeCBr4',str_replace(' ','__',str_replace(')','___',str_replace('(','___',trim($mbe_key))))));
 }
 function sql_key_to_mbe_short($sql_key) {
 	return str_replace('BeCBr4','Be/CBr4',str_replace('__',' ',str_replace('pcnt','%',preg_replace('/_____(\w+)___/',' (${1})',$sql_key))));
 }
 function sql_key_to_mbe_long($sql_key) {
 	return str_replace('BeCBr4','Be/CBr4',str_replace('__',' ',str_replace('pcnt','%',preg_replace('/_____(\w+)_____/',' (${1}) ',$sql_key))));
 }
 
 
 function mbe_to_sql_values_map() {
 		$conn = DBManager::getConnection();	
 		$query="select mbe_key,mbe_val from epi_mbe_meas_values_kv";
 		$result = $conn->query($query);
 		while($row=$result->fetch_assoc()) {
 			$arr[$row['mbe_val']]=$row['mbe_key'];
 		}
 	return $arr;
 }
 
  function mbe_to_sql_meas_map() {
 		$conn = DBManager::getConnection();	
 		$query="select mbe_key,mbe_val from epi_mbe_meas_kv";
 		$result = $conn->query($query);
 		while($row=$result->fetch_assoc()) {
 			$arr[$row['mbe_val']]=$row['mbe_key'];
 		}
 	return $arr;
 }
 
 
 
 /* function to select the kind of mbe layer */

function mbe_layer($row) {
	
	if ( ($row["In1 Shutter"] > 0) &&
	($row["Ga1 Shutter"] > 0) &&
	($row["In2 Shutter"] > 0) &&
	($row["Ga2 Shutter"] > 0) ) { $layer_name = 'Background';}
	
	
	if ( ($row["As1 Shutter"] > 0) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Substrate Center (C)"] > 720) &&
	(($row["Substrate Target (C)"] - $row["Substrate Center (C)"]) < 1) ) { $layer_name = 'Oxide Desorption';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 1) &&
	($row["In1 Shutter"] < 1) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	($row["SVT_RF Shutter"] < 1) &&
	($row["Oxford_RF Shutter"] < 1) &&
	($row["Veeco_RF1 Shutter"] < 1) &&
	($row["Veeco_RF2 Shutter"] < 1) &&
	($row["In2 Shutter"] < 1) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaAs';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 1) &&
	($row["In1 Shutter"] < 1) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	($row["SVT_RF Shutter"] < 1) &&
	($row["Oxford_RF Shutter"] < 1) &&
	($row["Veeco_RF1 Shutter"] < 1) &&
	($row["Veeco_RF2 Shutter"] < 1) &&
	($row["In2 Shutter"] < 1) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] > 0) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaAs:Si';}
	
	
	if ( ($row["Be/CBr4 shutter"] > 0) &&
	($row["In1 Shutter"] < 1) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	($row["SVT_RF Shutter"] < 1) &&
	($row["Oxford_RF Shutter"] < 1) &&
	($row["Veeco_RF1 Shutter"] < 1) &&
	($row["Veeco_RF2 Shutter"] < 1) &&
	($row["In2 Shutter"] < 1) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] > 4) ) { $layer_name = 'GaAs:C';}
	
	
	if ( ($row["Be/CBr4 shutter"] > 0) &&
	($row["In1 Shutter"] < 1) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	($row["SVT_RF Shutter"] < 1) &&
	($row["Oxford_RF Shutter"] < 1) &&
	($row["Veeco_RF1 Shutter"] < 1) &&
	($row["Veeco_RF2 Shutter"] < 1) &&
	($row["In2 Shutter"] < 1) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaAs:Be';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 1) &&
	( ($row["In1 Shutter"] > 0) || ($row["In2 Shutter"] > 0) ) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	($row["SVT_RF Shutter"] < 1) &&
	($row["Oxford_RF Shutter"] < 1) &&
	($row["Veeco_RF1 Shutter"] < 1) &&
	($row["Veeco_RF2 Shutter"] < 1) &&
	( ($row["In2 Shutter"] > 0) || ($row["In1 Shutter"] > 0) ) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'InGaAs';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 1) &&
	($row["In1 Shutter"] < 1) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	( ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Oxford_RF Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF1 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF2 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) ) &&
	($row["In2 Shutter"] < 1) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaNAs';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 1) &&
	( ($row["In1 Shutter"] > 0) || ($row["In2 Shutter"] > 0) ) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	( ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Oxford_RF Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF1 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF2 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) ) &&
	( ($row["In2 Shutter"] > 0) || ($row["In1 Shutter"] > 0) ) &&
	($row["Sb1 shutter"] < 1) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 1) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] < 1) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaInNAs';}
	
	
	if ( ($row["Be/CBr4 shutter"] < 5) &&
	( ($row["In1 Shutter"] > 0) || ($row["In2 Shutter"] > 0) ) &&
	($row["As1 Shutter"] > 0) &&
	( ($row["Ga1 Shutter"] > 0) || ($row["Ga2 Shutter"] > 0) ) &&
	( ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Oxford_RF Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF1 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF2 Shutter"] > 0) ) &&
	( ($row["Veeco_RF2 Shutter"] > 0) || ($row["SVT_RF Shutter"] > 0) || ($row["Oxford_RF Shutter"] > 0) || ($row["Veeco_RF1 Shutter"] > 0) ) &&
	( ($row["In2 Shutter"] > 0) || ($row["In1 Shutter"] > 0) ) &&
	($row["Sb1 shutter"] > 0) &&
	( ($row["Ga2 Shutter"] > 0) || ($row["Ga1 Shutter"] > 0) ) &&
	($row["Si1 Shutter"] < 5) &&
	($row["As1 Valve (mils)"] > 0) &&
	($row["Sb1 Valve (mils)"] > 0) &&
	($row["CBr4 State"] < 5) ) { $layer_name = 'GaInNAsSb';}


return $layer_name;
}
 

/* function to select the kind of mbe layer */
//11-08


function mbe_layer_sql($row) {

/* identify layers in the MBE data */

$layer_name = NULL;

//background
if ( ($row["In1__Shutter"] > 0) &&
($row["Ga1__Shutter"] > 0) &&
($row["In2__Shutter"] > 0) &&
($row["Ga2__Shutter"] > 0) ) { $layer_name = 'Background';}

// oxide desorption
/*if ( ($row["As1__Shutter"] > 0) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Substrate__Center_____C___"] > 690) &&
(($row["Substrate__Target_____C___"] - $row["Substrate__Center_____C___"]) < 1) ) { 
	$layer_name = 'Oxide Desorption';
}*/

if ( ($row["As1__Shutter"] > 0) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Substrate__Center_____C___"] > 680) ) { 
	$layer_name = 'Oxide Desorption';
}

// GaAs 
if ( ($row["BeCBr4__shutter"] < 1) &&
($row["In1__Shutter"] < 1) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
($row["SVT_RF__Shutter"] < 1) &&
($row["Oxford_RF__Shutter"] < 1) &&
($row["Veeco_RF1__Shutter"] < 1) &&
($row["Veeco_RF2__Shutter"] < 1) &&
($row["In2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaAs';}


if ( ($row["BeCBr4__shutter"] < 1) &&
($row["In1__Shutter"] < 1) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
($row["SVT_RF__Shutter"] < 1) &&
($row["Oxford_RF__Shutter"] < 1) &&
($row["Veeco_RF1__Shutter"] < 1) &&
($row["Veeco_RF2__Shutter"] < 1) &&
($row["In2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] > 0) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaAs:Si';}


if ( ($row["BeCBr4__shutter"] > 0) &&
($row["In1__Shutter"] < 1) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
($row["SVT_RF__Shutter"] < 1) &&
($row["Oxford_RF__Shutter"] < 1) &&
($row["Veeco_RF1__Shutter"] < 1) &&
($row["Veeco_RF2__Shutter"] < 1) &&
($row["In2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] > 4) ) { $layer_name = 'GaAs:C';}


if ( ($row["BeCBr4__shutter"] > 0) &&
($row["In1__Shutter"] < 1) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
($row["SVT_RF__Shutter"] < 1) &&
($row["Oxford_RF__Shutter"] < 1) &&
($row["Veeco_RF1__Shutter"] < 1) &&
($row["Veeco_RF2__Shutter"] < 1) &&
($row["In2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaAs:Be';}


if ( ($row["BeCBr4__shutter"] < 1) &&
( ($row["In1__Shutter"] > 0) || ($row["In2__Shutter"] > 0) ) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
($row["SVT_RF__Shutter"] < 1) &&
($row["Oxford_RF__Shutter"] < 1) &&
($row["Veeco_RF1__Shutter"] < 1) &&
($row["Veeco_RF2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'InGaAs';}


if ( ($row["BeCBr4__shutter"] < 1) &&
($row["In1__Shutter"] < 1) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
( 
	($row["SVT_RF__Shutter"] > 0) || 
	($row["Oxford_RF__Shutter"] > 0) || 
	($row["Veeco_RF1__Shutter"] > 0) || 
	($row["Veeco_RF2__Shutter"] > 0) 
) &&
($row["In2__Shutter"] < 1) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaNAs';}


if ( ($row["BeCBr4__shutter"] < 1) &&
( ($row["In1__Shutter"] > 0) || ($row["In2__Shutter"] > 0) ) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
( 
	($row["SVT_RF__Shutter"] > 0) || 
	($row["Oxford_RF__Shutter"] > 0) || 
	($row["Veeco_RF1__Shutter"] > 0) || 
	($row["Veeco_RF2__Shutter"] > 0) 
) &&
($row["Sb1__shutter"] < 1) &&
($row["Si1__Shutter"] < 1) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] < 1) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaInNAs';}


if ( ($row["BeCBr4__shutter"] < 5) &&
( ($row["In1__Shutter"] > 0) || ($row["In2__Shutter"] > 0) ) &&
($row["As1__Shutter"] > 0) &&
( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) ) &&
( 
	($row["SVT_RF__Shutter"] > 0) || 
	($row["Oxford_RF__Shutter"] > 0) || 
	($row["Veeco_RF1__Shutter"] > 0) || 
	($row["Veeco_RF2__Shutter"] > 0) 
) &&
($row["Sb1__shutter"] > 0) &&
($row["Si1__Shutter"] < 5) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Sb1__Valve_____mils___"] > 0) &&
($row["CBr4__State"] < 5) ) { $layer_name = 'GaInNAsSb';}


if ($layer_name != NULL ) {
	return $layer_name;
} 


$layer_name = 'PAUSE';
$layer_name_add=array();

foreach (array_keys($row) as $k) {
	if (stristr($k,'shutter') || stristr($k,'valve')) {
		if ($row[$k] > 0) {
			$tmp = explode('__',$k);
			$layer_name_add[]= $tmp[0].' '.$tmp[1];
		}
	}

}

if (count($layer_name_add) > 0 ) {
	$layer_name.='-'.implode(',',$layer_name_add);
}

return $layer_name;

// end of mbe_layer_sql function
}



function mbe_layer_sql2($row) {

/* identify layers in the MBE data */

$layer_name = NULL;

//background


// oxide desorption
/*if ( ($row["As1__Shutter"] > 0) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Substrate__Center_____C___"] > 690) &&
(($row["Substrate__Target_____C___"] - $row["Substrate__Center_____C___"]) < 1) ) { 
	$layer_name = 'Oxide Desorption';
}*/


$l = array();

if ( ($row["Al1__tip_ShutterStatus"] > 0) || ($row["Al2_tip_ShutterStatus"] > 0) ) {
	$l[] = 'Al'.($row["Al1__tip_ShutterStatus"] >0?'1':'').($row["Al2_tip_ShutterStatus"] >0?'2':'');
}

if ( ($row["Ga1__Shutter"] > 0) || ($row["Ga2__Shutter"] > 0) || ($row["Ga3_tip_ShutterStatus"] > 0)  || ($row["Ga4__tip_Shutter"] > 0)) {
	$l[] = 'Ga'.($row["Ga1__Shutter"] >0?'1':'').($row["Ga2__Shutter"] >0?'2':'').($row["Ga3_tip_ShutterStatus"] > 0?'3':'').($row["Ga4__tip_Shutter"] > 0?'4':'');
}

if ( ($row["Er1_base_ShutterStatus"] > 0) ) {
	$l[] = 'Er';
}

if (  ($row["In1__Shutter"] > 0) || ($row["In2__Shutter"] > 0) ) {
	$l[] = 'In'.($row["In1__Shutter"] >0?'1':'').($row["In2__Shutter"] >0?'2':'');
}

if ($row['P1__valve_ShutterStatus'] > 0 || $row['P1__valve_Measured'] > 0 ) {
	$l[]='P';
}

if ( ($row["SVT_RF__Shutter"] > 0) ||  ($row["Oxford_RF__Shutter"] > 0) ||  ($row["Veeco_RF1__Shutter"] > 0) ||  ($row["Veeco_RF2__Shutter"] > 0) ) {
	$l[] = 'N';	
}

if ((($row["As1__Shutter"] > 0) && ($row["As1__Valve_____mils___"] > 0)) || (($row["As2__valve_Shutter"] > 0) && ($row["As2__valve_Measured"] > 0))) {
	$l[]= 'As';
}

if (($row["Sb1__shutter"] > 0) && ($row["Sb1__Valve_____mils___"] > 0) ) {
	$l[] = 'Sb';
}

if (($row["CBr4__State"] > 4) ) {
	$l[]=':C';
}

if (($row["Si1__Shutter"] > 0)) {
	$l[]=':Si';
}

if (($row["BeCBr4__shutter"] > 0) || ($row["Be2__shutter"] > 0)) {
	$l[]=':Be'.(($row["BeCBr4__shutter"] > 0)?'1':'').(($row["Be2__shutter"] > 0)?'2':'');
}

if ((($row["As1__Shutter"] > 0) &&
($row["As1__Valve_____mils___"] > 0) &&
($row["Substrate__Center_____C___"] > 680)) || (($row["As2__Shutter"] > 0) &&
($row["As2__Valve_____mils___"] > 0) &&
($row["Substrate__Center_____C___"] > 680))) { 
	if (implode('',$l) == 'As') {
	 return 'Oxide Desorption';
	}
}

if ( ($row["In1__Shutter"] > 0) &&
($row["Ga1__Shutter"] > 0) &&
($row["In2__Shutter"] > 0) &&
($row["Ga2__Shutter"] > 0) &&
($row["Ga3_tip_ShutterStatus"] > 0) &&
($row["Ga4__tip_Shutter"] > 0)) { 
	if(implode('',$l) == 'GaIn') {
		return 'Background';
	}
}

$layer_name = implode('',$l);

if ($layer_name != NULL ) {
	return $layer_name;
} 


$layer_name = 'PAUSE';
$layer_name_add=array();

foreach (array_keys($row) as $k) {
	if (stristr($k,'shutter') || stristr($k,'valve')) {
		if ($row[$k] > 0) {
			$tmp = explode('__',$k);
			$layer_name_add[]= $tmp[0].' '.$tmp[1];
		}
	}

}

if (count($layer_name_add) > 0 ) {
	$layer_name.='-'.implode(',',$layer_name_add);
}

return $layer_name;

// end of mbe_layer_sql function
}



// 11-14 added

function create_mbe_values_table_and_query()
{
		$query  = "SELECT * FROM epi_mbe_meas_values_kv WHERE mbe_epitrend != ''";
		$result = $this->local_query($query);
		
		$Mquery = 'SELECT ';
		
		echo '<pre>';

		// create the measurement table 

		echo '
		CREATE TABLE IF NOT EXISTS `epi_mbe_meas` (
		  `mbe_meas_id` int(11) NOT NULL auto_increment,
		  `mbe_id` int(11) NOT NULL,
		  `layer_number` int(3) NOT NULL,
		  `layer_name` int(3) NOT NULL,
		  `notes` varchar(255) collate utf8_unicode_ci NOT NULL,<br>';
		  foreach ($result as $r) {
			 if (stristr($r['mbe_key'],'time')) {
				echo '`'.$r['mbe_key'].'__MIN` float NOT NULL,<br>';
				echo '`'.$r['mbe_key'].'__MAX` float NOT NULL,<br>';
				echo '`'.$r['mbe_key'].'` float NOT NULL,<br>';
				
				
				$Mquery.=' MIN('.$r['mbe_key'].') as '.$r['mbe_key'].'__MIN, ';
				$Mquery.=' MAX('.$r['mbe_key'].') as '.$r['mbe_key'].'__MAX, ';
				$Mquery.=' MAX('.$r['mbe_key'].')-MIN('.$r['mbe_key'].') as '.$r['mbe_key'].',';
				
				
			 } elseif (!stristr($r['mbe_key'],'shutter') ){  
				echo '`'.$r['mbe_key'].'__AVG` float NOT NULL,<br>';
				echo '`'.$r['mbe_key'].'__STD` float NOT NULL,<br>';
				echo '`'.$r['mbe_key'].'__MIN` float NOT NULL,<br>';
				echo '`'.$r['mbe_key'].'__MAX` float NOT NULL,<br>';
				
				$Mquery.=' MIN('.$r['mbe_key'].') as '.$r['mbe_key'].'__MIN, ';
				$Mquery.=' MAX('.$r['mbe_key'].') as '.$r['mbe_key'].'__MAX, ';
				$Mquery.=' STDDEV_POP('.$r['mbe_key'].') as '.$r['mbe_key'].'__STD, ';
				$Mquery.=' AVG('.$r['mbe_key'].') as '.$r['mbe_key'].'__AVG, ';
		
			 }
		  }
		  
		echo '
		 
		  PRIMARY KEY  (`mbe_meas_id`),
		  KEY `mbe_id` (`mbe_id`)
		) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;
		
		ALTER TABLE `epi_mbe_meas`
		  ADD CONSTRAINT `epi_mbe_meas_ibfk_1` FOREIGN KEY (`mbe_id`) REFERENCES `epi_mbe` (`mbe_id`) ON DELETE CASCADE;
		';
		echo '</pre>';
		
		$Mquery .= ' from epi_mbe_meas_values where mbe_meas_id = XXX';
		
		echo $Mquery;


}



function get_stats_for_layer_query($mbe_meas_id){ 

	$cols=$this->get_table_columns('epi_mbe_meas');	
	$cols2 = $this->get_table_columns('epi_mbe_meas_values');

	$q = array();
	// prepopulate the time variable ...
	$q[]='MIN(Time_____s___) as Time_____s_____MIN';
	$q[]='MAX(Time_____s___) as Time_____s_____MAX';
	$q[]='MAX(Time_____s___)-MIN(Time_____s___) as Time_____s___';

	// skip the first 6 entries so we don't try to insert a time query;
	for($i=6; $i<count($cols2); $i++) {
	
		$col=$cols2[$i];
	
		$ext = array('__MIN','__MAX','__AVG','__STD');
		$fun = array('__MIN'=>'MIN(@arg)','__MAX'=>'MAX(@arg)','__AVG'=>'AVG(@arg)','__STD'=>'STDDEV_POP(@arg)');
	
		foreach($ext as $e) {
			if (in_array($col.$e,$cols) && !stristr($col,'shutter')) {
				$q[] = str_replace('@arg',$col,$fun[$e])." as ".$col.$e; 		
			}
		}

	}				
	
	$query2 = "select ".implode(',',$q)." from epi_mbe_meas_values where mbe_meas_id = $mbe_meas_id ";
	return $query2;
}


function get_stats_for_layer_query_v0($mbe_meas_id){
	return "SELECT MIN(As1__bulk_____C___) as As1__bulk_____C_____MIN, 
				MAX(As1__bulk_____C___) as As1__bulk_____C_____MAX, 
				STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____STD, 
				AVG(As1__bulk_____C___) as As1__bulk_____C_____AVG, 
				MIN(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__MIN, 
				MAX(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__MAX,
				STDDEV_POP(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__STD, 
				AVG(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__AVG, 
				MIN(As1__Cracker_____C___) as As1__Cracker_____C_____MIN, 
				MAX(As1__Cracker_____C___) as As1__Cracker_____C_____MAX, 
				STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____STD, 
				AVG(As1__Cracker_____C___) as As1__Cracker_____C_____AVG, 
				MIN(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__MIN, 
				MAX(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__MAX, 
				STDDEV_POP(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__STD, 
				AVG(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__AVG, 
				MIN(As1__Valve_____mils___) as As1__Valve_____mils_____MIN, 
				MAX(As1__Valve_____mils___) as As1__Valve_____mils_____MAX, 
				STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____STD, 
				AVG(As1__Valve_____mils___) as As1__Valve_____mils_____AVG, 
				MIN(BandiT_____C___) as BandiT_____C_____MIN, 
				MAX(BandiT_____C___) as BandiT_____C_____MAX, 
				STDDEV_POP(BandiT_____C___) as BandiT_____C_____STD, 
				AVG(BandiT_____C___) as BandiT_____C_____AVG, 
				MIN(Be1__base_____C___) as Be1__base_____C_____MIN, 
				MAX(Be1__base_____C___) as Be1__base_____C_____MAX, 
				STDDEV_POP(Be1__base_____C___) as Be1__base_____C_____STD, 
				AVG(Be1__base_____C___) as Be1__base_____C_____AVG, 
				MIN(Be1__base__Percent__Power) as Be1__base__Percent__Power__MIN, 
				MAX(Be1__base__Percent__Power) as Be1__base__Percent__Power__MAX, 
				STDDEV_POP(Be1__base__Percent__Power) as Be1__base__Percent__Power__STD, 
				AVG(Be1__base__Percent__Power) as Be1__base__Percent__Power__AVG, 
				MIN(BFM_____torr___) as BFM_____torr_____MIN, 
				MAX(BFM_____torr___) as BFM_____torr_____MAX, 
				STDDEV_POP(BFM_____torr___) as BFM_____torr_____STD, 
				AVG(BFM_____torr___) as BFM_____torr_____AVG, 
				MIN(CBr4__Orifice) as CBr4__Orifice__MIN, 
				MAX(CBr4__Orifice) as CBr4__Orifice__MAX, 
				STDDEV_POP(CBr4__Orifice) as CBr4__Orifice__STD, 
				AVG(CBr4__Orifice) as CBr4__Orifice__AVG, 
				MIN(CBr4__Pressure_____torr___) as CBr4__Pressure_____torr_____MIN, 
				MAX(CBr4__Pressure_____torr___) as CBr4__Pressure_____torr_____MAX, 
				STDDEV_POP(CBr4__Pressure_____torr___) as CBr4__Pressure_____torr_____STD, 
				AVG(CBr4__Pressure_____torr___) as CBr4__Pressure_____torr_____AVG, 
				MIN(CBr4__State) as CBr4__State__MIN, MAX(CBr4__State) as CBr4__State__MAX, 
				STDDEV_POP(CBr4__State) as CBr4__State__STD, AVG(CBr4__State) as CBr4__State__AVG, 
				MIN(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____MIN, 
				MAX(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____MAX, 
				STDDEV_POP(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____STD, 
				AVG(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____AVG, 
				MIN(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____MIN, 
				MAX(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____MAX, 
				STDDEV_POP(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____STD,
				AVG(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____AVG,
				MIN(Ga1__Base_____C___) as Ga1__Base_____C_____MIN, 
				MAX(Ga1__Base_____C___) as Ga1__Base_____C_____MAX, 
				STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____STD, 
				AVG(Ga1__Base_____C___) as Ga1__Base_____C_____AVG, 
				MIN(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__MIN, 
				MAX(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__MAX, 
				STDDEV_POP(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__STD, 
				AVG(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__AVG, 
				MIN(Ga1__Tip_____C___) as Ga1__Tip_____C_____MIN, 
				MAX(Ga1__Tip_____C___) as Ga1__Tip_____C_____MAX, 
				STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____STD, 
				AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____AVG, 
				MIN(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__MIN, 
				MAX(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__MAX, 
				STDDEV_POP(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__STD, 
				AVG(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__AVG, 
				MIN(Ga2__Base_____C___) as Ga2__Base_____C_____MIN, 
				MAX(Ga2__Base_____C___) as Ga2__Base_____C_____MAX,
				STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____STD, 
				AVG(Ga2__Base_____C___) as Ga2__Base_____C_____AVG, 
				MIN(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__MIN, 
				MAX(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__MAX, 
				STDDEV_POP(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__STD,
				AVG(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__AVG,
				MIN(Ga2__Tip_____C___) as Ga2__Tip_____C_____MIN,
				MAX(Ga2__Tip_____C___) as Ga2__Tip_____C_____MAX, 
				STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____STD, 
				AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____AVG, 
				MIN(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__MIN, 
				MAX(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__MAX, 
				STDDEV_POP(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__STD, 
				AVG(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__AVG, 
				MIN(In1__Base_____C___) as In1__Base_____C_____MIN,
				MAX(In1__Base_____C___) as In1__Base_____C_____MAX,
				STDDEV_POP(In1__Base_____C___) as In1__Base_____C_____STD,
				AVG(In1__Base_____C___) as In1__Base_____C_____AVG,
				MIN(In1__Base__Percent__Power) as In1__Base__Percent__Power__MIN, 
				MAX(In1__Base__Percent__Power) as In1__Base__Percent__Power__MAX,
				STDDEV_POP(In1__Base__Percent__Power) as In1__Base__Percent__Power__STD, 
				AVG(In1__Base__Percent__Power) as In1__Base__Percent__Power__AVG, 
				MIN(In1__Tip_____C___) as In1__Tip_____C_____MIN, 
				MAX(In1__Tip_____C___) as In1__Tip_____C_____MAX, 
				STDDEV_POP(In1__Tip_____C___) as In1__Tip_____C_____STD, 
				AVG(In1__Tip_____C___) as In1__Tip_____C_____AVG, 
				MIN(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__MIN, 
				MAX(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__MAX, 
				STDDEV_POP(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__STD, 
				AVG(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__AVG, 
				MIN(In2__Base_____C___) as In2__Base_____C_____MIN, 
				MAX(In2__Base_____C___) as In2__Base_____C_____MAX, 
				STDDEV_POP(In2__Base_____C___) as In2__Base_____C_____STD, 
				AVG(In2__Base_____C___) as In2__Base_____C_____AVG, 
				MIN(In2__Base__Percent__Power) as In2__Base__Percent__Power__MIN, 
				MAX(In2__Base__Percent__Power) as In2__Base__Percent__Power__MAX, 
				STDDEV_POP(In2__Base__Percent__Power) as In2__Base__Percent__Power__STD, 
				AVG(In2__Base__Percent__Power) as In2__Base__Percent__Power__AVG, 
				MIN(In2__Tip_____C___) as In2__Tip_____C_____MIN, 
				MAX(In2__Tip_____C___) as In2__Tip_____C_____MAX, 
				STDDEV_POP(In2__Tip_____C___) as In2__Tip_____C_____STD, 
				AVG(In2__Tip_____C___) as In2__Tip_____C_____AVG, 
				MIN(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__MIN, 
				MAX(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__MAX, 
				STDDEV_POP(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__STD, 
				AVG(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__AVG, 
				MIN(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____MIN, 
				MAX(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____MAX, 
				STDDEV_POP(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____STD, 
				AVG(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____AVG, 
				MIN(N2_RF1_MFC) as N2_RF1_MFC__MIN, MAX(N2_RF1_MFC) as N2_RF1_MFC__MAX, 
				STDDEV_POP(N2_RF1_MFC) as N2_RF1_MFC__STD, 
				AVG(N2_RF1_MFC) as N2_RF1_MFC__AVG, 
				MIN(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__MIN, 
				MAX(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__MAX, 
				STDDEV_POP(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__STD, 
				AVG(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__AVG, 
				MIN(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__MIN, 
				MAX(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__MAX, 
				STDDEV_POP(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__STD, 
				AVG(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__AVG, 
				MIN(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__MIN, 
				MAX(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__MAX, 
				STDDEV_POP(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__STD, 
				AVG(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__AVG, 
				MIN(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__MIN, 
				MAX(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__MAX,
				STDDEV_POP(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__STD, 
				AVG(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__AVG, 
				MIN(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__MIN, 
				MAX(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__MAX, 
				STDDEV_POP(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__STD, 
				AVG(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__AVG, 
				MIN(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__MIN, 
				MAX(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__MAX, 
				STDDEV_POP(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__STD, 
				AVG(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__AVG,
				MIN(N2_RF2_MFC) as N2_RF2_MFC__MIN, 
				MAX(N2_RF2_MFC) as N2_RF2_MFC__MAX, 
				STDDEV_POP(N2_RF2_MFC) as N2_RF2_MFC__STD, 
				AVG(N2_RF2_MFC) as N2_RF2_MFC__AVG, 
				MIN(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__MIN,
				MAX(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__MAX,
				STDDEV_POP(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__STD,
				AVG(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__AVG, 
				MIN(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__MIN, 
				MAX(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__MAX, 
				STDDEV_POP(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__STD, 
				AVG(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__AVG, 
				MIN(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__MIN, 
				MAX(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__MAX, 
				STDDEV_POP(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__STD, 
				AVG(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__AVG, 
				MIN(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__MIN, MAX(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__MAX, STDDEV_POP(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__STD, AVG(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__AVG, MIN(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__MIN, MAX(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__MAX, STDDEV_POP(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__STD, AVG(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__AVG, MIN(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__MIN, MAX(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__MAX, STDDEV_POP(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__STD, AVG(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__AVG, MIN(Pyrometer_____C___) as Pyrometer_____C_____MIN, MAX(Pyrometer_____C___) as Pyrometer_____C_____MAX, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____STD, AVG(Pyrometer_____C___) as Pyrometer_____C_____AVG, MIN(Reactor_____torr___) as Reactor_____torr_____MIN, MAX(Reactor_____torr___) as Reactor_____torr_____MAX, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____STD, AVG(Reactor_____torr___) as Reactor_____torr_____AVG, MIN(RF_pressure_____torr___) as RF_pressure_____torr_____MIN, MAX(RF_pressure_____torr___) as RF_pressure_____torr_____MAX, STDDEV_POP(RF_pressure_____torr___) as RF_pressure_____torr_____STD, AVG(RF_pressure_____torr___) as RF_pressure_____torr_____AVG, MIN(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____MIN, MAX(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____MAX, STDDEV_POP(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____STD, AVG(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____AVG, MIN(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__MIN, MAX(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__MAX, STDDEV_POP(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__STD, AVG(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__AVG, MIN(Sb1__Cond_____C___) as Sb1__Cond_____C_____MIN, MAX(Sb1__Cond_____C___) as Sb1__Cond_____C_____MAX, STDDEV_POP(Sb1__Cond_____C___) as Sb1__Cond_____C_____STD, AVG(Sb1__Cond_____C___) as Sb1__Cond_____C_____AVG, 
				MIN(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__MIN, MAX(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__MAX, STDDEV_POP(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__STD, AVG(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__AVG, MIN(Sb1__Crack_____C___) as Sb1__Crack_____C_____MIN, MAX(Sb1__Crack_____C___) as Sb1__Crack_____C_____MAX, STDDEV_POP(Sb1__Crack_____C___) as Sb1__Crack_____C_____STD, AVG(Sb1__Crack_____C___) as Sb1__Crack_____C_____AVG, MIN(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__MIN, MAX(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__MAX, STDDEV_POP(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__STD, AVG(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__AVG, MIN(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____MIN, MAX(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____MAX, STDDEV_POP(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____STD, AVG(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____AVG, MIN(Si1__Tip_____C___) as Si1__Tip_____C_____MIN, MAX(Si1__Tip_____C___) as Si1__Tip_____C_____MAX, STDDEV_POP(Si1__Tip_____C___) as Si1__Tip_____C_____STD, AVG(Si1__Tip_____C___) as Si1__Tip_____C_____AVG, MIN(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__MIN, MAX(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__MAX, STDDEV_POP(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__STD, AVG(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__AVG, MIN(Substrate_____rpm___) as Substrate_____rpm_____MIN, MAX(Substrate_____rpm___) as Substrate_____rpm_____MAX, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____STD, AVG(Substrate_____rpm___) as Substrate_____rpm_____AVG, MIN(Substrate__Center_____A___) as Substrate__Center_____A_____MIN, MAX(Substrate__Center_____A___) as Substrate__Center_____A_____MAX, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____STD, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____AVG, MIN(Substrate__Center_____C___) as Substrate__Center_____C_____MIN, MAX(Substrate__Center_____C___) as Substrate__Center_____C_____MAX, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____STD, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____AVG, MIN(Substrate__Center_____w___) as Substrate__Center_____w_____MIN, MAX(Substrate__Center_____w___) as Substrate__Center_____w_____MAX, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____STD, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____AVG, MIN(Substrate__Outer_____w___) as Substrate__Outer_____w_____MIN, MAX(Substrate__Outer_____w___) as Substrate__Outer_____w_____MAX, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____STD, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____AVG, MIN(Substrate__Ramp__Rate_____dpm___) as Substrate__Ramp__Rate_____dpm_____MIN, MAX(Substrate__Ramp__Rate_____dpm___) as Substrate__Ramp__Rate_____dpm_____MAX, STDDEV_POP(Substrate__Ramp__Rate_____dpm___) as Substrate__Ramp__Rate_____dpm_____STD, AVG(Substrate__Ramp__Rate_____dpm___) as Substrate__Ramp__Rate_____dpm_____AVG, MIN(Substrate__Target_____C___) as Substrate__Target_____C_____MIN, MAX(Substrate__Target_____C___) as Substrate__Target_____C_____MAX, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____STD, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____AVG, MIN(Time_____s___) as Time_____s_____MIN, MAX(Time_____s___) as Time_____s_____MAX, MAX(Time_____s___)-MIN(Time_____s___) as Time_____s___
				from epi_mbe_meas_values where mbe_meas_id = $mbe_meas_id";

}
// 11-14 added

function mbe_layer_values($layer) {


		if ($layer == 'Background') {
			return "AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(Sb1__Crack_____C___) as Sb1__Crack_____C_____Average, AVG(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____Average, AVG(Sb1__Cond_____C___) as Sb1__Cond_____C_____Average, AVG(Be1__base_____C___) as Be1__base_____C_____Average, AVG(Si1__Tip_____C___) as Si1__Tip_____C_____Average, AVG(In1__Tip_____C___) as In1__Tip_____C_____Average, AVG(In1__Base_____C___) as In1__Base_____C_____Average, AVG(In2__Tip_____C___) as In2__Tip_____C_____Average, AVG(In2__Base_____C___) as In2__Base_____C_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__Average, AVG(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__Average, AVG(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__Average, AVG(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__Average, AVG(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__Average, AVG(Be1__base__Percent__Power) as Be1__base__Percent__Power__Average, AVG(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__Average, AVG(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__Average, AVG(In1__Base__Percent__Power) as In1__Base__Percent__Power__Average, AVG(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__Average, AVG(In2__Base__Percent__Power) as In2__Base__Percent__Power__Average, AVG(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__Average, AVG(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__Average, AVG(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__Average, AVG(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__Average, AVG(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____Average, AVG(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____Average, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(Sb1__Crack_____C___) as Sb1__Crack_____C_____Standard__Deviation, STDDEV_POP(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____Standard__Deviation, STDDEV_POP(Sb1__Cond_____C___) as Sb1__Cond_____C_____Standard__Deviation, STDDEV_POP(Be1__base_____C___) as Be1__base_____C_____Standard__Deviation, STDDEV_POP(Si1__Tip_____C___) as Si1__Tip_____C_____Standard__Deviation, STDDEV_POP(In1__Tip_____C___) as In1__Tip_____C_____Standard__Deviation, STDDEV_POP(In1__Base_____C___) as In1__Base_____C_____Standard__Deviation, STDDEV_POP(In2__Tip_____C___) as In2__Tip_____C_____Standard__Deviation, STDDEV_POP(In2__Base_____C___) as In2__Base_____C_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(As1__Cracker__Percent__Power) as As1__Cracker__Percent__Power__Standard__Deviation, STDDEV_POP(As1__bulk__Percent__Power) as As1__bulk__Percent__Power__Standard__Deviation, STDDEV_POP(Sb1__Crack__Percent__Power) as Sb1__Crack__Percent__Power__Standard__Deviation, STDDEV_POP(Sb1__Bulk__Percent__Power) as Sb1__Bulk__Percent__Power__Standard__Deviation, STDDEV_POP(Sb1__Cond__Percent__Power) as Sb1__Cond__Percent__Power__Standard__Deviation, STDDEV_POP(Be1__base__Percent__Power) as Be1__base__Percent__Power__Standard__Deviation, STDDEV_POP(Si1__Tip__Percent__Power) as Si1__Tip__Percent__Power__Standard__Deviation, STDDEV_POP(In1__Tip__Percent__Power) as In1__Tip__Percent__Power__Standard__Deviation, STDDEV_POP(In1__Base__Percent__Power) as In1__Base__Percent__Power__Standard__Deviation, STDDEV_POP(In2__Tip__Percent__Power) as In2__Tip__Percent__Power__Standard__Deviation, STDDEV_POP(In2__Base__Percent__Power) as In2__Base__Percent__Power__Standard__Deviation, STDDEV_POP(Ga1__Tip__Percent__Power) as Ga1__Tip__Percent__Power__Standard__Deviation, STDDEV_POP(Ga1__Base__Percent__Power) as Ga1__Base__Percent__Power__Standard__Deviation, STDDEV_POP(Ga2__Tip__Percent__Power) as Ga2__Tip__Percent__Power__Standard__Deviation, STDDEV_POP(Ga2__Base__Percent__Power) as Ga2__Base__Percent__Power__Standard__Deviation, STDDEV_POP(Cyro__Panel__Water_____C___) as Cyro__Panel__Water_____C_____Standard__Deviation, STDDEV_POP(LN2__Level_____pcnt___) as LN2__Level_____pcnt_____Standard__Deviation ";
		}
		
		if ($layer == 'Oxide Desorption') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation ";
		}
		
		if ($layer == 'GaAs') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation ";
		}
		
		if ($layer == 'GaAs:Si') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation ";
		}
		
		if ($layer == 'GaAs:C') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(CBr4__State) as CBr4__State__Average, AVG(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(CBr4__State) as CBr4__State__Standard__Deviation, STDDEV_POP(CBr4__Temperature_____C___) as CBr4__Temperature_____C_____Standard__Deviation ";
		}
		
		if ($layer == 'GaAs:Be') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation ";
		}
		
		if ($layer == 'InGaAs') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(In1__Tip_____C___) as In1__Tip_____C_____Average, AVG(In1__Base_____C___) as In1__Base_____C_____Average, AVG(In2__Tip_____C___) as In2__Tip_____C_____Average, AVG(In2__Base_____C___) as In2__Base_____C_____Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(In1__Tip_____C___) as In1__Tip_____C_____Standard__Deviation, STDDEV_POP(In1__Base_____C___) as In1__Base_____C_____Standard__Deviation, STDDEV_POP(In2__Tip_____C___) as In2__Tip_____C_____Standard__Deviation, STDDEV_POP(In2__Base_____C___) as In2__Base_____C_____Standard__Deviation ";
		}
		
		if ($layer == 'GaNAs') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(RF_pressure_____torr___) as RF_pressure_____torr_____Average, AVG(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Average, AVG(N2_RF1_MFC) as N2_RF1_MFC__Average, AVG(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Average, AVG(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Average, AVG(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Average, AVG(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Average, AVG(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Average, AVG(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Average, AVG(N2_RF2_MFC) as N2_RF2_MFC__Average, AVG(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Average, AVG(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Average, AVG(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Average, AVG(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Average, AVG(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Average, AVG(Oxford_MFC_Run) as Oxford_MFC_Run__Average, AVG(Oxford_MFC) as Oxford_MFC__Average, AVG(Oxford_RF_Forward) as Oxford_RF_Forward__Average, AVG(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Average, AVG(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Average, AVG(Oxford_RF_Reflective) as Oxford_RF_Reflective__Average, AVG(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Average, AVG(SVT_MFC_Run) as SVT_MFC_Run__Average, AVG(SVT_MFC) as SVT_MFC__Average, AVG(SVT_RF_Forward) as SVT_RF_Forward__Average, AVG(SVT_RF_TuningCap) as SVT_RF_TuningCap__Average, AVG(SVT_RF_LoadCap) as SVT_RF_LoadCap__Average, AVG(SVT_RF_Reflective) as SVT_RF_Reflective__Average, AVG(SVT_RF_DC_bias) as SVT_RF_DC_bias__Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(RF_pressure_____torr___) as RF_pressure_____torr_____Standard__Deviation, STDDEV_POP(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF1_MFC) as N2_RF1_MFC__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Standard__Deviation, STDDEV_POP(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF2_MFC) as N2_RF2_MFC__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Standard__Deviation, STDDEV_POP(Oxford_MFC_Run) as Oxford_MFC_Run__Standard__Deviation, STDDEV_POP(Oxford_MFC) as Oxford_MFC__Standard__Deviation, STDDEV_POP(Oxford_RF_Forward) as Oxford_RF_Forward__Standard__Deviation, STDDEV_POP(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Standard__Deviation, STDDEV_POP(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Standard__Deviation, STDDEV_POP(Oxford_RF_Reflective) as Oxford_RF_Reflective__Standard__Deviation, STDDEV_POP(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Standard__Deviation, STDDEV_POP(SVT_MFC_Run) as SVT_MFC_Run__Standard__Deviation, STDDEV_POP(SVT_MFC) as SVT_MFC__Standard__Deviation, STDDEV_POP(SVT_RF_Forward) as SVT_RF_Forward__Standard__Deviation, STDDEV_POP(SVT_RF_TuningCap) as SVT_RF_TuningCap__Standard__Deviation, STDDEV_POP(SVT_RF_LoadCap) as SVT_RF_LoadCap__Standard__Deviation, STDDEV_POP(SVT_RF_Reflective) as SVT_RF_Reflective__Standard__Deviation, STDDEV_POP(SVT_RF_DC_bias) as SVT_RF_DC_bias__Standard__Deviation ";
		}
		
		if ($layer == 'GaInNAs') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(In1__Tip_____C___) as In1__Tip_____C_____Average, AVG(In1__Base_____C___) as In1__Base_____C_____Average, AVG(In2__Tip_____C___) as In2__Tip_____C_____Average, AVG(In2__Base_____C___) as In2__Base_____C_____Average, AVG(RF_pressure_____torr___) as RF_pressure_____torr_____Average, AVG(RF__Source__Name) as RF__Source__Name, AVG(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Average, AVG(N2_RF1_MFC) as N2_RF1_MFC__Average, AVG(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Average, AVG(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Average, AVG(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Average, AVG(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Average, AVG(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Average, AVG(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Average, AVG(N2_RF2_MFC) as N2_RF2_MFC__Average, AVG(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Average, AVG(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Average, AVG(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Average, AVG(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Average, AVG(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Average, AVG(Oxford_MFC_Run) as Oxford_MFC_Run__Average, AVG(Oxford_MFC) as Oxford_MFC__Average, AVG(Oxford_RF_Forward) as Oxford_RF_Forward__Average, AVG(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Average, AVG(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Average, AVG(Oxford_RF_Reflective) as Oxford_RF_Reflective__Average, AVG(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Average, AVG(SVT_MFC_Run) as SVT_MFC_Run__Average, AVG(SVT_MFC) as SVT_MFC__Average, AVG(SVT_RF_Forward) as SVT_RF_Forward__Average, AVG(SVT_RF_TuningCap) as SVT_RF_TuningCap__Average, AVG(SVT_RF_LoadCap) as SVT_RF_LoadCap__Average, AVG(SVT_RF_Reflective) as SVT_RF_Reflective__Average, AVG(SVT_RF_DC_bias) as SVT_RF_DC_bias__Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(In1__Tip_____C___) as In1__Tip_____C_____Standard__Deviation, STDDEV_POP(In1__Base_____C___) as In1__Base_____C_____Standard__Deviation, STDDEV_POP(In2__Tip_____C___) as In2__Tip_____C_____Standard__Deviation, STDDEV_POP(In2__Base_____C___) as In2__Base_____C_____Standard__Deviation, STDDEV_POP(RF_pressure_____torr___) as RF_pressure_____torr_____Standard__Deviation, STDDEV_POP(RF__Source__Name) as RF__Source__Name, STDDEV_POP(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF1_MFC) as N2_RF1_MFC__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Standard__Deviation, STDDEV_POP(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF2_MFC) as N2_RF2_MFC__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Standard__Deviation, STDDEV_POP(Oxford_MFC_Run) as Oxford_MFC_Run__Standard__Deviation, STDDEV_POP(Oxford_MFC) as Oxford_MFC__Standard__Deviation, STDDEV_POP(Oxford_RF_Forward) as Oxford_RF_Forward__Standard__Deviation, STDDEV_POP(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Standard__Deviation, STDDEV_POP(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Standard__Deviation, STDDEV_POP(Oxford_RF_Reflective) as Oxford_RF_Reflective__Standard__Deviation, STDDEV_POP(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Standard__Deviation, STDDEV_POP(SVT_MFC_Run) as SVT_MFC_Run__Standard__Deviation, STDDEV_POP(SVT_MFC) as SVT_MFC__Standard__Deviation, STDDEV_POP(SVT_RF_Forward) as SVT_RF_Forward__Standard__Deviation, STDDEV_POP(SVT_RF_TuningCap) as SVT_RF_TuningCap__Standard__Deviation, STDDEV_POP(SVT_RF_LoadCap) as SVT_RF_LoadCap__Standard__Deviation, STDDEV_POP(SVT_RF_Reflective) as SVT_RF_Reflective__Standard__Deviation, STDDEV_POP(SVT_RF_DC_bias) as SVT_RF_DC_bias__Standard__Deviation ";
		}
		
		if ($layer == 'GaInNAsSb') {
			return "AVG(Pyrometer_____C___) as Pyrometer_____C_____Average, AVG(BandiT_____C___) as BandiT_____C_____Average, AVG(Reactor_____torr___) as Reactor_____torr_____Average, AVG(BFM_____torr___) as BFM_____torr_____Average, AVG(Substrate__Center_____C___) as Substrate__Center_____C_____Average, AVG(Substrate__Target_____C___) as Substrate__Target_____C_____Average, AVG(Substrate__Center_____w___) as Substrate__Center_____w_____Average, AVG(Substrate__Outer_____w___) as Substrate__Outer_____w_____Average, AVG(Substrate__Center_____A___) as Substrate__Center_____A_____Average, AVG(Substrate__Outer_____A___) as Substrate__Outer_____A_____Average, AVG(Substrate_____rpm___) as Substrate_____rpm_____Average, AVG(As1__Cracker_____C___) as As1__Cracker_____C_____Average, AVG(As1__bulk_____C___) as As1__bulk_____C_____Average, AVG(As1__Valve_____mils___) as As1__Valve_____mils_____Average, AVG(Sb1__Crack_____C___) as Sb1__Crack_____C_____Average, AVG(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____Average, AVG(Sb1__Cond_____C___) as Sb1__Cond_____C_____Average, AVG(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____Average, AVG(Ga1__Tip_____C___) as Ga1__Tip_____C_____Average, AVG(Ga1__Base_____C___) as Ga1__Base_____C_____Average, AVG(Ga2__Tip_____C___) as Ga2__Tip_____C_____Average, AVG(Ga2__Base_____C___) as Ga2__Base_____C_____Average, AVG(In1__Tip_____C___) as In1__Tip_____C_____Average, AVG(In1__Base_____C___) as In1__Base_____C_____Average, AVG(In2__Tip_____C___) as In2__Tip_____C_____Average, AVG(In2__Base_____C___) as In2__Base_____C_____Average, AVG(RF_pressure_____torr___) as RF_pressure_____torr_____Average, AVG(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Average, AVG(N2_RF1_MFC) as N2_RF1_MFC__Average, AVG(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Average, AVG(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Average, AVG(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Average, AVG(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Average, AVG(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Average, AVG(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Average, AVG(N2_RF2_MFC) as N2_RF2_MFC__Average, AVG(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Average, AVG(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Average, AVG(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Average, AVG(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Average, AVG(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Average, AVG(Oxford_MFC_Run) as Oxford_MFC_Run__Average, AVG(Oxford_MFC) as Oxford_MFC__Average, AVG(Oxford_RF_Forward) as Oxford_RF_Forward__Average, AVG(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Average, AVG(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Average, AVG(Oxford_RF_Reflective) as Oxford_RF_Reflective__Average, AVG(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Average, AVG(SVT_MFC_Run) as SVT_MFC_Run__Average, AVG(SVT_MFC) as SVT_MFC__Average, AVG(SVT_RF_Forward) as SVT_RF_Forward__Average, AVG(SVT_RF_TuningCap) as SVT_RF_TuningCap__Average, AVG(SVT_RF_LoadCap) as SVT_RF_LoadCap__Average, AVG(SVT_RF_Reflective) as SVT_RF_Reflective__Average, AVG(SVT_RF_DC_bias) as SVT_RF_DC_bias__Average, STDDEV_POP(Pyrometer_____C___) as Pyrometer_____C_____Standard__Deviation, STDDEV_POP(BandiT_____C___) as BandiT_____C_____Standard__Deviation, STDDEV_POP(Reactor_____torr___) as Reactor_____torr_____Standard__Deviation, STDDEV_POP(BFM_____torr___) as BFM_____torr_____Standard__Deviation, STDDEV_POP(Substrate__Center_____C___) as Substrate__Center_____C_____Standard__Deviation, STDDEV_POP(Substrate__Target_____C___) as Substrate__Target_____C_____Standard__Deviation, STDDEV_POP(Substrate__Center_____w___) as Substrate__Center_____w_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____w___) as Substrate__Outer_____w_____Standard__Deviation, STDDEV_POP(Substrate__Center_____A___) as Substrate__Center_____A_____Standard__Deviation, STDDEV_POP(Substrate__Outer_____A___) as Substrate__Outer_____A_____Standard__Deviation, STDDEV_POP(Substrate_____rpm___) as Substrate_____rpm_____Standard__Deviation, STDDEV_POP(As1__Cracker_____C___) as As1__Cracker_____C_____Standard__Deviation, STDDEV_POP(As1__bulk_____C___) as As1__bulk_____C_____Standard__Deviation, STDDEV_POP(As1__Valve_____mils___) as As1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Sb1__Crack_____C___) as Sb1__Crack_____C_____Standard__Deviation, STDDEV_POP(Sb1__Bulk_____C___) as Sb1__Bulk_____C_____Standard__Deviation, STDDEV_POP(Sb1__Cond_____C___) as Sb1__Cond_____C_____Standard__Deviation, STDDEV_POP(Sb1__Valve_____mils___) as Sb1__Valve_____mils_____Standard__Deviation, STDDEV_POP(Ga1__Tip_____C___) as Ga1__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga1__Base_____C___) as Ga1__Base_____C_____Standard__Deviation, STDDEV_POP(Ga2__Tip_____C___) as Ga2__Tip_____C_____Standard__Deviation, STDDEV_POP(Ga2__Base_____C___) as Ga2__Base_____C_____Standard__Deviation, STDDEV_POP(In1__Tip_____C___) as In1__Tip_____C_____Standard__Deviation, STDDEV_POP(In1__Base_____C___) as In1__Base_____C_____Standard__Deviation, STDDEV_POP(In2__Tip_____C___) as In2__Tip_____C_____Standard__Deviation, STDDEV_POP(In2__Base_____C___) as In2__Base_____C_____Standard__Deviation, STDDEV_POP(RF_pressure_____torr___) as RF_pressure_____torr_____Standard__Deviation, STDDEV_POP(N2_RF1_MFC_Run) as N2_RF1_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF1_MFC) as N2_RF1_MFC__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Forward) as N2_RF1_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF1_RF_TuningCap) as N2_RF1_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_LoadCap) as N2_RF1_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF1_RF_Reflective) as N2_RF1_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF1_RF_DC_bias) as N2_RF1_RF_DC_bias__Standard__Deviation, STDDEV_POP(N2_RF2_MFC_Run) as N2_RF2_MFC_Run__Standard__Deviation, STDDEV_POP(N2_RF2_MFC) as N2_RF2_MFC__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Forward) as N2_RF2_RF_Forward__Standard__Deviation, STDDEV_POP(N2_RF2_RF_TuningCap) as N2_RF2_RF_TuningCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_LoadCap) as N2_RF2_RF_LoadCap__Standard__Deviation, STDDEV_POP(N2_RF2_RF_Reflective) as N2_RF2_RF_Reflective__Standard__Deviation, STDDEV_POP(N2_RF2_RF_DC_bias) as N2_RF2_RF_DC_bias__Standard__Deviation, STDDEV_POP(Oxford_MFC_Run) as Oxford_MFC_Run__Standard__Deviation, STDDEV_POP(Oxford_MFC) as Oxford_MFC__Standard__Deviation, STDDEV_POP(Oxford_RF_Forward) as Oxford_RF_Forward__Standard__Deviation, STDDEV_POP(Oxford_RF_TuningCap) as Oxford_RF_TuningCap__Standard__Deviation, STDDEV_POP(Oxford_RF_LoadCap) as Oxford_RF_LoadCap__Standard__Deviation, STDDEV_POP(Oxford_RF_Reflective) as Oxford_RF_Reflective__Standard__Deviation, STDDEV_POP(Oxford_RF_DC_bias) as Oxford_RF_DC_bias__Standard__Deviation, STDDEV_POP(SVT_MFC_Run) as SVT_MFC_Run__Standard__Deviation, STDDEV_POP(SVT_MFC) as SVT_MFC__Standard__Deviation, STDDEV_POP(SVT_RF_Forward) as SVT_RF_Forward__Standard__Deviation, STDDEV_POP(SVT_RF_TuningCap) as SVT_RF_TuningCap__Standard__Deviation, STDDEV_POP(SVT_RF_LoadCap) as SVT_RF_LoadCap__Standard__Deviation, STDDEV_POP(SVT_RF_Reflective) as SVT_RF_Reflective__Standard__Deviation, STDDEV_POP(SVT_RF_DC_bias) as SVT_RF_DC_bias__Standard__Deviation ";
		}
		return NULL;
		
}



function mbe_layer_names() {

	return array('GaInNAsSb','GaInNAs','GaNAsSb','InGaAs','GaAs:Be','GaAs:Si','GaAs:C','GaAs','Background','Oxide Desorption');
}


 /* end of mbe functions */
 
 
 /* XML  output */
 
 
 function array_to_ExcelXML($data) {
	 $xml ='<?xml version="1.0"?>
		<Workbook xmlns="urn:schemas-microsoft-com:office:spreadsheet"
	 xmlns:o="urn:schemas-microsoft-com:office:office"
	 xmlns:x="urn:schemas-microsoft-com:office:excel"
	 xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"
	 xmlns:html="http://www.w3.org/TR/REC-html40">
	 <DocumentProperties xmlns="urn:schemas-microsoft-com:office:office">
	  <Author>Ilya Fushman</Author>
	  <LastAuthor>Ilya Fushman</LastAuthor>
	  <Created>2008-10-02T08:58:38Z</Created>
	  <Company>solar junction</Company>
	  <Version>12.258</Version>
	 </DocumentProperties>
	 <OfficeDocumentSettings xmlns="urn:schemas-microsoft-com:office:office">
	  <AllowPNG/>
	 </OfficeDocumentSettings>
	 <ExcelWorkbook xmlns="urn:schemas-microsoft-com:office:excel">
	  <WindowHeight>14420</WindowHeight>
	  <WindowWidth>23240</WindowWidth>
	  <WindowTopX>700</WindowTopX>
	  <WindowTopY>700</WindowTopY>
	  <ActiveSheet>1</ActiveSheet>
	  <MaxIterations>500</MaxIterations>
	  <Date1904/>
	  <Iteration/>
	  <ProtectStructure>False</ProtectStructure>
	  <ProtectWindows>False</ProtectWindows>
	 </ExcelWorkbook>
	 <Styles>
	  <Style ss:ID="Default" ss:Name="Normal">
	   <Alignment ss:Vertical="Bottom"/>
	   <Borders/>
	   <Font ss:FontName="Verdana"/>
	   <Interior/>
	   <NumberFormat/>
	   <Protection/>
	  </Style>
	 </Styles>';
 
 	foreach($data as $key=>$val ){
 		
 		$col ='<Row>';
 		$row ='<Row>';
 		foreach ($val as $k=>$v) {
 			$col.='<Cell><Data ss:Type="String">'.$k.'</Data></Cell>';
 			$row.='<Cell><Data ss:Type="Number">'.$v.'</Data></Cell>';
 		}
 		$col.='</Row>';
 		$row.='</Row>';

		$xml.='<Worksheet ss:Name="'.$key.'">
			<Table>
		   '.$col.'
		   '.$row.'
		  </Table>
		  <WorksheetOptions xmlns="urn:schemas-microsoft-com:office:excel">
		   
		   <PageLayoutZoom>100</PageLayoutZoom>
		   <Panes>
			<Pane>
			 <Number>3</Number>
			 <ActiveRow>1</ActiveRow>
			 <ActiveCol>2</ActiveCol>
			</Pane>
		   </Panes>
		   <ProtectObjects>False</ProtectObjects>
		   <ProtectScenarios>False</ProtectScenarios>
		  </WorksheetOptions>
		 </Worksheet>';
 		
 		
 	}
 	$xml.='</Workbook>';

 	return $xml;
 
 }
 
 function get_growth_select_options($growth_id) {
 
	$conn = DBManager::getConnection();	
	$query = "select growth_id as growth_id, concat(chamber_id,'-',growth_run) as growth_name, cast(growth_run as signed) as growth_run from epi_growth order by CHAR_LENGTH(chamber_id) asc, growth_run desc";
	$result = $conn->query($query);
	
	while(($row = $result->fetch_assoc()) != NULL) {
		$arr[$row['growth_id']]=$row['growth_name'];
	}
	return implode('',$this->get_array_options_list_kv(array_flip($arr),$growth_id));
	
 }
 
 function pl_map_report_html($pl_id) {
 
 	$query = "select * from epi_pl where pl_id = $pl_id";
 	
	$result = $this->local_query($query);
	if ($result != NULL ){
		$r = $result[0];
		
		$html = '<table width=750  style="border: solid 1px #CCCCCC; ">
			<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
				<td>Date</td><td>laser</td><td>power</td><td>filter</td><td>Lam center</td><td>Pow center</td><td>Lam avg</td><td>Pow avg</td><td>Lam stddev</td><td>Pow sttdev</td>
			</tr>
			<tr style="border: solid 1px #CCCCCC; padding:3px">
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['laser'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['power'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['filter'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_center'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_center'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_avg'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_avg'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_stdev'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_stdev'].'</td>
			</tr></table>';
	  	return $html;
  	} 
  	
  	
  	return '';
		
 
} 

function pl2_map_report_all_html($wafer_id) {
 	$query = "select * from epi_pl2_meas where wafer_id = $wafer_id order by date desc";
	$result = $this->local_query($query);
	if ($result != NULL ){
		$html = '<table width=1000  style="border: solid 1px #CCCCCC; ">
				<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
					<td style="border: solid 1px #CCCCCC; padding:2px;">Wafer </td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Laser</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Peak Lamb Avg</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Peak Lamb Med</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Peak Int Avg</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Peak Int Med</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Fwhm Avg</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Fwhm Med</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Date Time</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Description</td>
				</tr>';
		foreach ($result as $r ) {
			$html.='<tr style="border: solid 1px #CCCCCC; padding:2px">
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.strtoupper($r['wafer_name']).'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['laser'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_lambda_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_lambda_med'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_int_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_int_med'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['fwhm_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['fwhm_med'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['description'].'</td>
				</tr>';
		}
	  	$html.='</table>';
	  	return $html;
	}
  	return '';
}
	
function pl_map_report_all_html($wafer_id)
{
 	$query = "select * from epi_pl where wafer_id = $wafer_id order by date desc";
	$result = $this->local_query($query);
	if ($result != NULL ){
		$html = '<table width=1000  style="border: solid 1px #CCCCCC; ">
				<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
					<td style="border: solid 1px #CCCCCC; padding:2px;">Wafer </td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Date</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">laser</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">power</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">filter</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam center (Eg[eV])</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow center</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam avg (Eg [eV])</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow avg</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam stddev</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow sttdev</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Scan Rate</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Gain</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Slit</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Plot</td>
				</tr>';
		foreach ($result as $r ) {
		
			$html.='<tr style="border: solid 1px #CCCCCC; padding:2px">
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.strtoupper($r['wafer_name']).'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['laser'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['power'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['filter'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_center'].' ('.sprintf("%1.2f",1240/$r['pl_lambda_center']).') </td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_center'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_avg'].'  ('.sprintf("%1.2f",1240/$r['pl_lambda_avg']).')</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_stdev'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_stdev'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['scan_rate'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['gain'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['slit_width'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;"><a style="color:blue" href="../parsedata/plot_pl.php?pl_id='.$r['pl_id'].'">PLOT</a></td>
				</tr>';
	  	
	  	}
	  	$html.='</table>';
	  	return $html;
  	} 
  	return '';
} 

function pl_map_report_pieces_html($wafer_id) {
	$pieces = $this->get_wafer_pieces($wafer_id);
	if (!$pieces ) return '';
 	$query = "select * from epi_pl p where wafer_id in(".implode(',',array_keys($pieces)).") order by date desc";
 	
	$result = $this->local_query($query);
	if ($result != NULL ){
	
		$html = '<table width=800  style="border: solid 1px #CCCCCC; ">
				<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px;whitespace:nowrap">
					<td> Piece </td><!--<td>Date</td>--><td>laser</td><td>power</td><td>filter</td><td>Lam center</td><td>Pow center</td><td>Lam avg</td><td>Pow avg</td><td>Lam STD</td><td>Pow STD</td><td>Scan Rate</td><td>Gain</td>
				</tr>';
		foreach ($result as $r ) {
		
			$html.='<tr style="border: solid 1px #CCCCCC; padding:2px">
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$pieces[$r['wafer_id']].'</td>
				<!--	<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td> -->
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['laser'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['power'].'</td><td>'.$r['filter'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_center'].'</td><td>'.$r['pl_power_center'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_avg'].'</td><td>'.$r['pl_power_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_stdev'].'</td><td>'.$r['pl_power_stdev'].'</td>
				</tr>';
	  	
	  	}
	  	$html.='</table>';
	  	return $html;
  	} 
  	
  	
  	return '';
} 



function pl2_map_report_pieces_anneal_html($wafer_id) {
	$pieces = $this->get_wafer_pieces($wafer_id);
	if (!$pieces ) return '';
 	$query = "select p.*, a.peak_temp_actual ".
		 "from epi_pl2_meas p, epi_rta_meas a ".
		 "where p.wafer_id in(".implode(',',array_keys($pieces)).") and p.wafer_id = a.wafer_id order by p.laser asc, a.peak_temp_actual asc, p.date desc";

	$result = $this->local_query($query);
	if ($result != NULL ){
		$html = '<table width=1000  style="border: solid 1px #CCCCCC; ">
				<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px;whitespace:nowrap">
					<td style="border: solid 1px #CCCCCC; padding:2px;"> Piece </td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Date</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Description</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Temperature</td>
				</tr>';
		foreach ($result as $r ) {
			$html.='<tr style="border: solid 1px #CCCCCC; padding:2px">
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['wafer_name'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['description'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['temperature'].'</td>
				</tr>';
	  	}
	  	$html.='</table>';
	  	return $html;
  	} 
  	
  	return '';
}

function pl_map_report_pieces_anneal_html($wafer_id)
{
	$pieces = $this->get_wafer_pieces($wafer_id);
	if (!$pieces ) return '';
 	$query = "select p.*, a.peak_temp_actual from epi_pl p, epi_rta_meas a where p.wafer_id in(".implode(',',array_keys($pieces)).") and p.wafer_id = a.wafer_id order by p.laser asc, a.peak_temp_actual asc, p.date desc";
 	//echo $query;
	$result = $this->local_query($query);
	if ($result != NULL ){
	
		$html = '<table width=1000  style="border: solid 1px #CCCCCC; ">
				<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px;whitespace:nowrap">
					<td style="border: solid 1px #CCCCCC; padding:2px;"> Piece </td>
					<!--<td>Date</td>-->
					<td style="border: solid 1px #CCCCCC; padding:2px;">laser</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">power</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">filter</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Anneal T[C]</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam center (Eg[eV])</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow center</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam avg (Eg[eV])</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow avg</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Lam STD</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Pow STD</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Scan Rate</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Gain</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Slit</td>
					<td style="border: solid 1px #CCCCCC; padding:2px;">Plot</td>
				</tr>';
		foreach ($result as $r ) {
		
			$html.='<tr style="border: solid 1px #CCCCCC; padding:2px">
					<!--<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$pieces[$r['wafer_id']].'</td> -->
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['wafer_name'].'</td>
					
				<!--	<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td> -->
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['laser'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['power'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['filter'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.($r['peak_temp_actual'] > 0?$r['peak_temp_actual']:'NA').'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_center'].' ('.sprintf("%1.2f",1240/$r['pl_lambda_center']).') </td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_center'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_avg'].' ('.sprintf("%1.2f",1240/$r['pl_lambda_avg']).') </td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_avg'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_lambda_stdev'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['pl_power_stdev'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['scan_rate'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['gain'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['slit_width'].'</td>
					<td  style="border: solid 1px #CCCCCC; padding:2px;"><a style="color:blue" href="../parsedata/plot_pl.php?pl_id='.$r['pl_id'].'">PLOT</a></td>
				</tr>';
	  	
	  	}
	  	$html.='</table>';
	  	return $html;
  	} 
  	
  	
  	return '';
}


 function rta_report_pieces_html($wafer_id) {
	$pieces = $this->get_wafer_pieces($wafer_id);
 	if (!$pieces ) return '';
 	$query = "select * from epi_rta_meas p where wafer_id in(".implode(',',array_keys($pieces)).") order by peak_temp_intended asc";
 	
	$result = $this->local_query($query);
	
	if ($result != NULL ){
		$html = '<table width=800  style="border: solid 1px #CCCCCC;white-space:nowrap">
			<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
				<td style="border: solid 1px #CCCCCC; padding:2px;">Piece</td>
				<!--<td>Date</td>-->
				<td style="border: solid 1px #CCCCCC; padding:2px;">Recipe</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak T Intended [C]</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak T Actual [C]</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak time Intended [s]</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak time Actual [s]</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Total Power</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Click to Plot</td>
			</tr>';
		foreach ($result as $r) {
			$html.='<tr style="border: solid 1px #CCCCCC; padding:3px">
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$pieces[$r['wafer_id']].'</td>
				<!--<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>-->
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['rta_recipe'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_temp_intended'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_temp_actual'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_time_intended'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_time_actual'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['total_power'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;"><a style="color:blue" href="../parsedata/plot_rta.php?id='.$r['rta_meas_id'].'">PLOT</a></td>
			</tr>';
	  	}
	  	$html.='</table>';
		return $html;
  	} 
  	
  	
  	return '';
		
 
} 


function is_piece($wafer_id) {
	//echo "bla";
	return $this->check_exists('epi_wafer_pieces','child_id',$wafer_id);
}

function get_piece_xy($wafer_id) {
	
	$query = "select x,y,w,h from epi_wafer_pieces where child_id = $wafer_id";
	$result = $this->local_query($query);
	return $result[0];
	
}

function get_piece_center($wafer_id) {
	$xy = $this->get_piece_xy($wafer_id);
	// $this->echo_r($xy);
/*	$center =array(	'x'=>($xy['x']+$xy['w']/2),
					'y'=>($xy['y']-$xy['h']/2)
				); */
	$center =array(	'x'=>($xy['x']),
					'y'=>($xy['y'])
				); 
	// $this->echo_r($center);
	
	return $center;
}


 function rta_report_html($wafer_id) {
 
 	$query = "select * from epi_rta_meas where wafer_id = $wafer_id";
 	//echo $query;
	$result = $this->local_query($query);
	if ($result != NULL ){
		$html = '<table width=800  style="border: solid 1px #CCCCCC;white-space:nowrap">
			<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
				<td style="border: solid 1px #CCCCCC; padding:2px;">Wafer</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">New name</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Date</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Recipe</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak T (intended)</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak T (actual)</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak time (intended)</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Peak time (actual)</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Total Power</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Data file</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Click to Plot</td>
			</tr>';
		foreach ($result as $r) {
			$html.='<tr style="border: solid 1px #CCCCCC; padding:3px">
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['wafer_name'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['new_wafer_name'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['rta_recipe'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_temp_intended'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_temp_actual'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_time_intended'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['peak_time_actual'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['total_power'].'</td>';

			  $html .= '<td style="border: solid 1px #CCCCCC; padding:2px;"><a style="color:blue" href="../parsedata/'.$r['data_file'].'">file</a></td>';
      $html .= '<td style="border: solid 1px #CCCCCC; padding:2px;"><a style="color:blue" href="../parsedata/plot_rta.php?id='.$r['rta_meas_id'].'">PLOT</a></td>
			</tr>';
	  	}
	  	$html.='</table>';
		return $html;
  	} 
  	
  	
  	return '';
		
 
} 

function hall_report_html($wafer_id) {
 
 	$query = "select * from epi_hall_meas where wafer_id = $wafer_id";
 	//echo $query;
	$result = $this->local_query($query);
	if ($result != NULL ){
		$html = '<table width=800  style="border: solid 1px #CCCCCC;white-space:nowrap">
			<tr style="background-color:#E3DFFF;border: solid 1px #CCCCCC; padding:3px">
				<td style="border: solid 1px #CCCCCC; padding:2px;">Date</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Description</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Temperature</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Thickness (intended)</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Symmetry</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Sheet R</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Bulk R</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Sheet Hall Coef</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Bulk Hall Coef</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Sheet N</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Bulk N</td>
				<td style="border: solid 1px #CCCCCC; padding:2px;">Mobility</td>
			</tr>';
		foreach ($result as $r) {
			$html.='<tr style="border: solid 1px #CCCCCC; padding:3px">
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['date'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['descr'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['temperature'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['thickness'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['symmetry'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['sheet_r'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['bulk_r'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['sheet_hall_coef'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['bulk_hall_coef'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['sheet_n'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['bulk_n'].'</td>
				<td  style="border: solid 1px #CCCCCC; padding:2px;">'.$r['mobility'].'</td>
			</tr>';
	  	}
	  	$html.='</table>';
		return $html;
  	} 
  	
  	
  	return '';
		
 
} 


function get_substrate_select_options($all=FALSE) {

	$where=" where number_loaded > 0 ";
	if ($all) {
		$where="";
	}

	$query = "select substrate_id,substrate_name, ingot_number, number_loaded,number_left from epi_substrate ".$where." order by substrate_vendor_id asc";
	$result = $this->local_query($query);
	$opts='';
	foreach ($result as $r) {
		$opts.='<option value="'.$r['substrate_id'].'">'.$r['substrate_name'].'-'.$r['ingot_number'].' ('.$r['number_loaded'].' loaded, '.$r['number_left'].' left to load)</option>';
	}
	return $opts;
}



function get_wafer_ingot_number($wafer_id) {
	$query = "select ingot_number from epi_wafer where wafer_id = $wafer_id ";
	$result = $this->local_query($query);
	return $result[0]['ingot_number'];


}

function open_file($file,$mode) {
	$handle = fopen($file,$mode);
	if($handle) {
		return $handle;
	 
	} else {
		$this->echo_error(" could not open file: $file ");	
		return false;
	}
}


// ======= IV DATA Upload ========/

function parse_iv_old($file,$params){
	
	$f=$this->open_file($file,'r');
	
	if($f) {
		
		$oX=-4;
		$oY=-4;
		
		$today = date('Y-m-d H:i:s');
		
		$count=0;
		
		/* now create the measurement instance */
		$query = "insert into epi_iv (".implode(',',array_keys($params)).") values ('".implode("','",array_values($params))."')";
		//echo "<br> $query <br>";
		
		$result = $this->local_insert($query);
		if(!$result) {
			$this->echo_error(" Could not insert iv measurement <br> $query <br> ");
			exit;
		}
		
		$iv_id=$this->last_insert();
		

		while(!feof($f)) {
			
			
			$extract = fgets($f);
			
			$tmp=explode(',',$extract);
			
			if($tmp[0] > 0 && $tmp[1] > 0) {
				$I =fgets($f);
				if (strlen(trim(str_replace(',','',str_replace('0.000E+0','',$I)))) >0 ){
					$count=$count+1;
					$I=explode(',',$I);
					
					
					
					$V = explode(',',fgets($f));
					$extracted = array( 'die_x'=>(int)$tmp[0],
										'die_y'=>(int)$tmp[1],
										'die_w'=>'10',
										'die_h'=>'10',
										'device_name'=>$params['device_name'], // will need to get this dynamically and actually create a measurement instance for each device name ... 
										'wafer_id'=>$params['wafer_id'],
										'wafer_name'=>$params['wafer_name'],
										'SMF'=>$tmp[2],
										'I_sc'=>$tmp[3],
										'V_oc'=>$tmp[4],
										'I_mp'=>$tmp[5],
										'V_mp'=>$tmp[6],
										
										'efficiency'=>$tmp[7],
										'P_max'=>$tmp[8],
										'P_in'=>$tmp[9],
										'FF'=>$tmp[10],
										'R_sh'=>$tmp[11],
										'R_s'=>$tmp[12]);
					//echo "<br> Measurement point $count at ".((int)$tmp[0]).",".((int)$tmp[1]);
					
					$extracted['x']=($extracted['die_x']+$oX)*10.0;
					$extracted['y']=($extracted['die_y']+$oY)*(-10.0);
					
					
					
					
					
					// $this->echo_r($extracted);
					$extracted['iv_id']=$iv_id;
					
					$query = "insert into epi_iv_meas (".implode(',',array_keys($extracted)).") values ('".implode("','",array_values($extracted))."')";
					//echo "<br> $query <br>";
					$result = $this->local_insert($query);
					if(!$result) {
						$this->echo_error(" Could not insert iv measurement <br> $query <br> ");
						exit;
					}
					
					$iv_meas_id=$this->last_insert();
					
					for ($i=0;$i<count($I);$i++){
						$query = "insert into epi_iv_meas_values set iv_meas_id='$iv_meas_id', I='".$I[$i]."',V='".$V[$i]."'";
						//echo "<br> $query <br>";
						$result = $this->local_insert($query);
						if(!$result) {
							$this->echo_error(" Could not insert iv measurement point <br> $query <br> ");
							exit;
						}
					
					}
					
					
				}	
			}
			
			
		}
	}
}


//=========================//

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
	
	if ($this->is_float_num($line) || strtolower($line) =='nan' || strtolower($line) == 'inf') {
		if ($this->is_int_num($line)) {
			return 'INT';
		}
		return 'FLOAT';
	}
	
	if ($this->is_date($line)) {
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

//////////


function file_get_header_array(&$handle,$start,$stop,$delim)
{
		$string = fgetcsv($handle,0,$delim);
		$header = array();
		while(!feof($handle) && !stristr($string[0],$start) && $start != '') {
			$string = fgetcsv($handle,0,$delim);
		}

		while(!feof($handle) && !stristr($string[0],$stop)) {	
			
			$key = $this->db_safe_name($string[0]);
			$val = addslashes(trim($string[1]));
			
			$header[$key]=array('val'=>$val,'type'=>$this->type_guess($val));
			if (stristr($key,'date')) {
				$header[$key]['val'] =  $this->parse_datetime($val);
			}
			$string = fgetcsv($handle,0,$delim);
		}
		$key = $this->db_safe_name($string[0]);
		$val = addslashes(trim($string[1]));
		$header[$key]=array('val'=>$val,'type'=>$this->type_guess($val));

		return $header;
}

function file_get_data_array(&$handle,$start='',$start_after='',$stop='',$num=0,$row_col = 'row',$delim,$field_def = array()) {
	$string = fgetcsv($handle,0,$delim);
	$out = array();
	
	$skip = ($start_after!=''?$start_after:$start);
	
	while(!feof($handle) && !stristr($string[0],$start) && $start != '') {
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
	if (count($string) > 0 && $string[0] != '') {
		$data[] = $string;
	}

	$out = array();
	
	if ($row_col == 'col') {
		$data = $this->array_transpose_2d($data);
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

	$ret = array();
	foreach($array as $key => $value) {
		foreach($value as $key2=>$value2) {
			$ret[$key2][$key] = $value2;
		}
	}
	
	return $ret;
}

function parse_flash($data, $file)
{
	$init = false;
	
	ini_set('auto_detect_line_endings',TRUE);

 	$handle = fopen($file, "r");
	if ($handle == NULL) {
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	// $header = $this->file_get_header_array($handle,'','Comment',",");
	$header = $this->file_get_header_array($handle,'','Error Bottom',",");

	if ($init) {
		$table_name = 'epi_flash_meas';
		$primary_key = 'flash_meas_id';
		$object_id = 'wafer_id';
		$object_name = 'wafer_name';
		echo "<br>";
		echo "CREATE TABLE IF NOT EXISTS `$table_name` (
			  $primary_key INT(11) NOT NULL auto_increment,
			  $object_id INT(11) NOT NULL,
			  $object_name VARCHAR(255) NOT NULL,
			  file_name VARCHAR(255) NULL,
				";
		foreach ($header as $k=>$v){
			//echo "<br>$k ".$v['type']." NULL, ";
		}
		echo "	PRIMARY KEY (`$primary_key`), 
  				INDEX(`wafer_id`,`wafer_name`)
				) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
	
		echo "<br>";
		
		echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`wafer_id`) 
				REFERENCES epi_wafer(wafer_id) ON DELETE CASCADE";
	}
	
	$dat = $this->file_get_data_array($handle,'V','','',3,'row',",",array());
	
	if ($init) {
		$parent_table = 'epi_flash_meas';
		$table_name = 'epi_flash_meas_values';
		$primary_key = 'flash_meas_id';
		
		echo "<br>CREATE TABLE IF NOT EXISTS `$table_name` (";
		echo "$primary_key INT(11) NOT NULL, ";
		foreach (array_keys($dat) as $k=>$v){
			echo "<br>$k FLOAT NULL, ";
		}
		echo "
				) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
		
		echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`$primary_key`) 
				REFERENCES $parent_table($primary_key) ON DELETE CASCADE";

	}

	
	$header_table = 'epi_flash_meas';
	$header_table_id = 'flash_meas_id'; 
	$data_table = 'epi_flash_meas_values';
	
	$header_fields = $this->get_table_columns($header_table);
	$data_fields = 	$this->get_table_columns($data_table);
	
	$header_skip = array('flash_meas_id','wafer_id','wafer_name','file_name');
	$header_fields = array_diff($header_fields,$header_skip);

	$data_skip = array('flash_meas_id');
	$data_fields = array_diff($data_fields,$header_skip);
		
	$query = "insert into $header_table set ";
	foreach ($header_fields as $f ) {
		$query.= "`$f` = '".addslashes($header[$f]['val'])."',";
	}
	$query .= " wafer_id = '".$data['sample_id']."',
		wafer_name = '".$data['sample']."',
		file_name='".addslashes($file)."'";
	
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	$flash_meas_id = $this->last_insert();
	if($flash_meas_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$tmp = $this->array_transpose_2d($dat);

	foreach ($tmp as $t) {
		$query = "INSERT into `$data_table` set ";
		foreach ($data_fields as $f) {
			$query .= " `$f` = '".$t[$f]."',";
		}
		$query .= " `$header_table_id` = '".$flash_meas_id."'";
		$result = $this->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}
	
	return  array('error'=>0,'data_id'=>$flash_meas_id);
	
}


// =================== parse flash ===================//

// =================== parse Reflectivity ===========//

function parse_refl($data,$file) 
{
	$header = array(

				"Collection Time:"=>array('datetime','yes'),
				"Operator Name  :"=>array('varchar(255)','yes'),
				"Scan Software Version:"=>array('varchar(255)','yes'),
				"Parameter List :"=>array('varchar(255)','no'),
				"Instrument"=>array('varchar(255)','yes'),
				"Instrument Version"=>array('varchar(255)','yes'),
				"Start (nm)"=>array('int(4)','yes'),
				"Stop (nm)"=>array('int(4)','yes'),
				"X Mode"=>array('varchar(255)','yes'),
				"Y Mode"=>array('varchar(5)','yes'),
				"UV-Vis Data Interval (nm)"=>array('float','yes'),
				"UV-Vis SBW (nm)"=>array('float','yes'),
				"Slit Height"=>array('varchar(25)','yes'),
				"Beam Mode"=>array('varchar(25)','yes'),
				"Signal-to-noise Mode"=>array('varchar(25)','yes'),
				"Acceptable S/N Ratio"=>array('float','yes'),
				"S/N Time Out (sec)"=>array('varchar(255)'),
				"UV Source"=>array('varchar(25)','yes'),
				"Vis Source"=>array('varchar(25)','yes'),
				"Third Source"=>array('varchar(25)','yes'),
				"Source Changeover (nm)"=>array('float','yes'),
				"Detector Changeover (nm)"=>array('float','yes'),
				"Grating Changeover (nm)"=>array('float','yes'),
				"Baseline Correction"=>array('varchar(25)','yes'),
				"Baseline Type"=>array('varchar(255)','yes'),
				"Status:"=>array('varchar(32)','yes')
			);

	$init = false;
	
	ini_set('auto_detect_line_endings',TRUE);

	$table_name = 'epi_refl_meas';
	
	$meas_table_name = 'epi_refl_meas_values';
	
	$primary_key = 'refl_meas_id';
	$object_id = 'wafer_id';
	$object_name = 'wafer_name';
		
	if ($init) {

		echo "<br>";
		echo "CREATE TABLE IF NOT EXISTS `$table_name` (
			  $primary_key INT(11) NOT NULL auto_increment,
			  $object_id INT(11) NOT NULL,
			  $object_name VARCHAR(255) NOT NULL,
			  file_name VARCHAR(255) NULL,
				";
		foreach ($header as $k=>$v){
		
			if ($v[1] == 'yes') 
			{
				 $ksave = trim(trim($this->db_safe_name($k),'_'));
				echo "<br>`$ksave` ".$v['type']." ".$v[0]." NULL COMMENT '".addslashes($k)."', ";
			}
		}
		echo "	PRIMARY KEY (`$primary_key`), 
  				INDEX(`$object_id`,`$object_name`)
				) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
	
		echo "<br>";
		
		echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`wafer_id`) 
				REFERENCES epi_wafer(wafer_id) ON DELETE CASCADE";

		echo "CREATE TABLE IF NOT EXISTS `$meas_table_name` (
			  $primary_key INT(11) NOT NULL,
			  `wavelength_nm` float NOT NULL,
			  `reflectivity` float NOT NULL
			) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci ;";	
	
		echo "ALTER TABLE `$meas_table_name` ADD CONSTRAINT FOREIGN KEY (`$primary_key`) 
				REFERENCES $table_name($primary_key) ON DELETE CASCADE";

	
	}
	
	echo "<br> trying to open $file ";

 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		echo "<br> Could not open file";
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	$data_set = array();
	while(!feof($handle)) 
	{
		$string = fgetcsv($handle);
		
		if (stristr($string[0],'Wavelength (nm)'))
		{
			break;
		}
	}
	
	while(!feof($handle)) 
	{
		$string = fgetcsv($handle);

		if(
		    (!$this->is_float_num($string[1]) && !$this->is_int_num($string[1]))
			|| (!$this->is_float_num($string[0]) && !$this->is_int_num($string[0]))
			)
		{
			break;
		} else {
			
			$data_set[] = array('wavelength_nm' => $string[0],'reflectivity'=>$string[1]);
		}
	}
	
	//$header = $this->file_get_header_array($handle,'','Comment',",");
	
	$header_set = array();

	
	$header_set[$object_name] = $data['sample'];
	
	$header_set[$object_id] = $data['sample_id'];
	
	$header_set['file_name'] = $file;
	
	// now special fields:
	$tmp = explode('-',$data['extra']);
	$header_set['ARC'] = $tmp[0];
	$header_set['ARC_vendor'] = $tmp[1];
	if (count($tmp) > 2) {
		$header_set['ARC_design'] = $tmp[2];
		$header_set['annealed'] = ($tmp[3] == 'Y') ? 1 : 0;
	}
	
	while(!feof($handle)) 
	{
		$string = fgets($handle);
		
		foreach ($header as $k=>$v)
		{
			if (stristr($string,$k) && $v[1] === 'yes')
			{
				$ksave = trim(trim($this->db_safe_name($k),'_'));
				
				$vsave = addslashes(trim(trim(trim(str_replace($k,'',$string)),',')));
				
				if($v[0] === 'datetime')
				{
					$vsave = $this->parse_datetime($vsave);
				}
				
				$header_set[$ksave] = $vsave;
				
				unset($header[$k]);

			}
		}
	}
	
	fclose($handle);
	
	$query = "INSERT INTO 
				`$table_name` 
				SET ";
	$q = array();
	
	foreach ($header_set  as $k=>$v)
	{
		$q[] = "`$k` = '".$v."'";
	}
	
	$query = $query . implode(" , ",$q);
	
	$result = $this->local_insert($query);
	
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$meas_id = $this->last_insert();
	
	if ($meas_id == NULL){
		return array('error'=>2,'point'=>$query);
	}
	
	
	foreach ($data_set as $s)
	{
		$query = "INSERT INTO `$meas_table_name` SET ";
		$query .= " `$primary_key` = $meas_id , ";
		$q = array();
		foreach ($s as $k=>$v)
		{
			$q[] = "`$k` = '".$v."'";
		}
		
		$query .= implode(" , ",$q);
		
		$result = $this->local_insert($query);
	
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}

		
	}
	
	return  array('error'=>0,'data_id'=>$meas_id);
	
}

// ====================================================//

// =================== parse TLM ======================//

function create_tmp_table($query)
{
  	$result = $this->local_create($query);
	return ($result) ? $result : NULL;
}

function parse_tlm($data, $file)
{
	$init = false;
	
	ini_set('auto_detect_line_endings', TRUE);

 	$handle = fopen($file, "r");
	
	if ($handle == NULL ) {
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	// $header = $this->file_get_header_array($handle,'','R-Squared',",");
	$header = $this->file_get_header_array($handle,'','Temperature (C)',",");

	if ($init) {
		$table_name = 'epi_tlm';
		$primary_key = 'tlm_id';
		$object_id = 'wafer_id';
		$object_name = 'wafer_name';
		echo "<hr><br>";
		echo "CREATE TABLE IF NOT EXISTS `$table_name` (
			  $primary_key INT(11) NOT NULL auto_increment,
			  $object_id INT(11) NOT NULL,
			  $object_name VARCHAR(255) NOT NULL,
			  'file_name' VARCHAR(255) NULL,
				";
		foreach ($header as $k=>$v){
			echo "<br>$k ".$v['type']." NULL, ";
		}
		echo "	PRIMARY KEY (`$primary_key`), 
  				INDEX(`wafer_id`),INDEX(`wafer_name`)
				) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
	
		echo "<br>";
		
		echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`wafer_id`) 
				REFERENCES epi_wafer(wafer_id) ON DELETE CASCADE";
	echo "<br><hr>";
	}
	
	//$this->echo_r($header);
	$agg_table = 'epi_tlm';
	$agg_table_id='tlm_id';
	
	$header_skip = array('wafer_id','wafer_name','file_name', $agg_table_id);
	
	$header_fields = $this->get_table_columns($agg_table);
	
	$header_fields = array_diff($header_fields, $header_skip);

	//$this->echo_r($header_fields);
		
	$query = "insert into $agg_table set ";
	foreach ($header_fields as $f ) {
		$query.= "`$f` = '".addslashes($header[$f]['val'])."',";
	}
	$query .= " wafer_id = '".$data['sample_id']."',
				wafer_name = '".$data['sample']."',
				file_name='".addslashes($file)."'";
	//echo "<br> $query";
	
	
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	$agg_id = $this->last_insert();
	
	if ($agg_id == NULL){
		return array('error'=>2,'point'=>$query);
	}
	
	//echo "<br> new TLM with id $agg_id";
	
// now for each repeating header /row ... 	
	$rowcount = 0;
	$header_count = 0;
	$data_count = 0;
	while(!feof($handle)) {
		
		$header = $this->file_get_header_array($handle,'Date','Radius',",");
		
		if ($rowcount > 0){
			if (count($header) < $header_count) break;
			$header_count = count($header);
		}
		
		
		if ($init) {
			$table_name = 'epi_tlm_meas';
			$primary_key = 'tlm_meas_id';
			$primary_key2 = 'tlm_id';
			$foreign_table = "epi_tlm";
			$object_id = 'wafer_id';
			$object_name = 'wafer_name';
			echo "<hr><br>";
			echo "CREATE TABLE IF NOT EXISTS `$table_name` (
				  $primary_key INT(11) NOT NULL auto_increment,
				  $primary_key2 INT(11) NOT NULL,
				  $object_id INT(11) NOT NULL,
				  $object_name VARCHAR(255) NOT NULL,
				  'file_name' VARCHAR(255) NULL,
					";
			foreach ($header as $k=>$v){
				echo "<br>$k ".$v['type']." NULL, ";
			}
			echo "	PRIMARY KEY (`$primary_key`), 
	  				INDEX(`wafer_id`,`wafer_name`)
					) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
		
			echo "<br>";
			
			echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`wafer_id`) 
					REFERENCES epi_wafer(wafer_id) ON DELETE CASCADE";
			echo "<br>";		
			echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`".$primary_key2."`) 
					REFERENCES $foreign_table($primary_key2) ON DELETE CASCADE";
			echo "<br><hr>";
		}
		
		
		$dat = $this->file_get_data_array($handle,'V','','I',2,'row',",",array());
		
	
		
		if ($rowcount > 0){
			if (count($dat) < $data_count) break;
			$data_count = count($dat);
		}
		
		if ($init) {
			$parent_table = 'epi_tlm_meas';
			$table_name = 'epi_tlm_meas_values';
			$primary_key = 'tlm_meas_id';
			
			echo "<hr><br>CREATE TABLE IF NOT EXISTS `$table_name` (";
			echo "$primary_key INT(11) NOT NULL, ";
			foreach (array_keys($dat) as $k){
				echo "<br>$k FLOAT NULL, ";
			}
			echo "
					) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1";	
			
			echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`$primary_key`) 
					REFERENCES $parent_table($primary_key) ON DELETE CASCADE";
	
		
			echo "<br><hr>";
		}
		
		//exit();
		
		$header_table = 'epi_tlm_meas';
		$header_table_id = 'tlm_meas_id'; 
		$data_table = 'epi_tlm_meas_values';
		
		
		$header_fields = $this->get_table_columns($header_table);
		$data_fields = 	$this->get_table_columns($data_table);
		
		//$this->echo_r($header);
		
		$header_skip = array($header_table_id,'wafer_id','wafer_name','file_name',$agg_table_id);
		
		$header_fields = array_diff($header_fields,$header_skip);
	
		//$this->echo_r($header_fields);
		
		//$this->echo_r($dat);
	
		$data_skip = array($header_table_id);
		$data_fields = array_diff($data_fields,$header_skip);
		
		//$this->echo_r($data_fields);
		
		$query = "insert into $header_table set ";
		foreach ($header_fields as $f ) {
			$query.= "`$f` = '".addslashes($header[$f]['val'])."',";
		}
		$query .= " $agg_table_id = '".$agg_id."',";
		$query .= " wafer_id = '".$data['sample_id']."',
					wafer_name = '".$data['sample']."',
					file_name='".addslashes($file)."'";
		//echo "<br> $query";
		
		$result = $this->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
		
		$header_id = $this->last_insert();
		
		
		if($header_id == NULL ) {
			return array('error'=>2,'point'=>$query);
		} 

		$tmp = $this->array_transpose_2d($dat);

		foreach ($tmp as $t) {
			$query = "INSERT into `$data_table` set ";
			foreach ($data_fields as $f) {
				$query .= " `$f` = '".$t[$f]."',";
			}
			$query .= " `$header_table_id` = '".$header_id."'";
		//	echo "<br> $query";
			$result = $this->local_insert($query);
			if($result == NULL ) {
				return array('error'=>2,'point'=>$query);
			} 
		}
		
			
		$rowcount = $rowcount+1;
	}
	
	// echo "<br> inserted ".($rowcount-1)." measurement instances ";
	
	return  array('error'=>0,'data_id'=>$agg_id);
	
}




// =================== parse TLM ======================//



// 2008-11-03

function get_wafer_images_html($wafer_id) {

	$html.='<h2><b>Images</b>: ';
	$html.='<a style="color:blue" href="'.SJ_HOST_NAME.'epi/parsedata/upload_images.php?wafer_id='.$wafer_id.'">Add</a>';
	$html.='  <a style="color:blue" href="'.SJ_HOST_NAME.'epi/edit/epi_wafer_images.php?wafer_id='.$wafer_id.'">View/Edit</a>';
	$html.='<h2>';
	$html.='<hr>';
	$html.='<br>';
	
	$html.='<div style="width:500px">';
	
	$html.=$this->show_wafer_images_thumbs($wafer_id);
	
	$html.='</div>';
	
	
	return $html;
}

function save_new_image($im) {
	$query = "insert into epi_images (".implode(',',array_keys($im)).") values ('".implode("','",array_values($im))."') ";
	$result = $this->local_insert($query);
	return $this->last_insert();
}

function new_wafer_image_relation($wafer_id,$image_id) {
	$query  = "INSERT INTO epi_wafer_images (wafer_id,image_id,wafer_name) ";
	$query .= "VALUES ($wafer_id,$image_id,'".$this->get_wafer_name_by_id($wafer_id)."')";
	$result = $this->local_insert($query);
	return $this->last_insert();
}


function render_image_and_caption($im) {
	echo '<hr>';
	echo '<img height="300" src="../parsedata/'.$im['image_src'].'" alt="'.$im['image_name'].'" >';
	echo '<div> Added on '.date('Y-m-d',$im['image_upload_date']).'</div>';
	echo '<div >Title: '.$im['image_name'].'</div>';
	echo '<div>Caption:<br>'.$im['image_caption'].'</div>';
}

function render_image_and_caption_edit($im) {
	echo '<hr>';
	echo '<div id = "image_container_'.$im['image_id'].'">';
	echo '<img height="300" src="../parsedata/'.$im['image_src'].'" alt="'.$im['image_name'].'" >';
	echo '<div> Added on '.date('Y-m-d',$im['image_upload_date']).'</div>';
	echo '<div >Title: <input type="text" name="image_name_'.$im['image_id'].'" value="'.$im['image_name'].'"></div>';
	echo '<div>Caption:<br><textarea rows="3" cols="35" name="image_caption_'.$im['image_id'].'" >'.$im['image_caption'].'</textarea></div>';
	echo '<input type="submit" value="Save" name="submit">';
	echo '<input type="button" value="Delete Image" name="delete_'.$im['image_id'].'" onClick="image_delete('.$im['image_id'].')">';
	echo '</div>';
}

function save_new_sem_image($im) {
	$query = "INSERT into epi_sem_image (".implode(',',array_keys($im)).") values ('".implode("','",array_values($im))."') ";
	$result = $this->local_insert($query);
	return $this->last_insert();
}

function get_sem_locations($sem_meas_id)
{
  	$query = "SELECT id, wafer_id, wafer_name, location ";
	$query.= "FROM epi_sem_meas ";
	$query.= "WHERE wafer_id = (SELECT wafer_id FROM epi_sem_meas WHERE id = $sem_meas_id)";
  	$result = $this->local_query($query);
	return ($result) ? $result : NULL;
}

function get_sem_measurement_details($sem_meas_id)
{
  	$query = "SELECT * FROM epi_sem_meas WHERE id = $sem_meas_id";
  	$result = $this->local_query($query);
	if ($result) {
  		return $result[0];
	}
	return NULL;
}

function get_wafer_name_by_sem_meas_id($sem_meas_id)
{
  		$query = "SELECT wafer_name, wafer_id FROM epi_sem_meas WHERE id = $sem_meas_id";
  		$result = $this->local_query($query);
  		$wafer_name = $result[0]['wafer_name'];

		//echo "get_wafer_name_by_sem_meas_id($sem_meas_id) returns $wafer_name";

  		return $wafer_name;
}
  	
function get_sem_images($sem_meas_id)
{
	$query = "SELECT * ";
	$query.= "FROM epi_sem_image ";
	$query.= "WHERE sem_meas_id = $sem_meas_id ";
	$query.= "ORDER BY id DESC";
	//echo $query;
	$result = $this->local_query($query);
	return ($result) ? $result : NULL;
}

function show_sem_images_view($sem_meas_id)
{
	$query = "select i.* ";
	$query.= "from epi_wafer_images w, epi_images i ";
	$query.= "where w.wafer_id = $wafer_id and w.image_id = i.image_id ";
	$query.= "order by i.image_upload_date desc";
	//echo $query;
	$result = $this->local_query($query);
	foreach ($result as $r) {
		//$this->echo_r($r);
		$this->render_image_and_caption_edit($r);
	}
}

function show_wafer_images_view($wafer_id)
{
	$query = "select i.* ";
	$query.= "from epi_wafer_images w, epi_images i ";
	$query.= "where w.wafer_id = $wafer_id and w.image_id = i.image_id ";
	$query.= "order by i.image_upload_date desc";
	//echo $query;
	$result = $this->local_query($query);
	foreach ($result as $r) {
		//$this->echo_r($r);
		$this->render_image_and_caption_edit($r);
	}
}

function show_wafer_images_thumbs($wafer_id)
{
	$query = "select i.* from epi_wafer_images w, epi_images i where w.wafer_id = $wafer_id and w.image_id = i.image_id order by i.image_upload_date desc";
	//echo $query;
	$result = $this->local_query($query);
	$html='';
	if($result != NULL){
		foreach ($result as $r) {
			//$this->echo_r($r);
			$html.='<img height="50" src="../parsedata/'.$r['image_src'].'" alt="'.$r['image_name'].'" >';
		}
	}
	return $html;
}

function update_sem_images($sem_meas_id, $post_vars )
{
	$query = "SELECT id image_id from epi_sem_image where sem_meas_id = $sem_meas_id";
	$result = $this->local_query($query);
	foreach($result as $r) {
		$image_id = $r['image_id'];
		if (isset($post_vars['image_name_'.$image_id]) && isset($post_vars['image_caption_'.$image_id]) ) {
			$query2  = "UPDATE epi_sem_image ";
			$query2 .= "SET name = '".$post_vars['image_name_'.$image_id]."', ";
			$query2 .= "    description = '".$post_vars['image_caption_'.$image_id]."' ";
			$query2 .= "WHERE id = $image_id";
			//echo $query2;
			$result2 = $this->local_insert($query2);
		}
	}
}

function update_wafer_images($wafer_id,$post_vars ) {
	$query = "select image_id from epi_wafer_images where wafer_id = $wafer_id";
	$result = $this->local_query($query);
	foreach($result as $r) {
		$image_id = $r['image_id'];
		if (isset($post_vars['image_name_'.$image_id]) && isset($post_vars['image_caption_'.$image_id]) ) {
			$query2="update epi_images set
							image_name = '".$post_vars['image_name_'.$image_id]."',
							image_caption = '".$post_vars['image_caption_'.$image_id]."' 
							where image_id = $image_id";
			//echo $query2;
			$result2 = $this->local_insert($query2);
		}
	
	}
	
}

function image_delete($image_id) {
	$query = "delete from epi_images where image_id = $image_id";
	$result = $this->local_insert($query);
	return $result;
}

function get_xrd_html($wafer_id) {

	$html.='<h2>XRD: ';
	$html.='<a style="color:blue" href="'.SJ_HOST_NAME.'epi/edit/epi_xrd.php?wafer_id='.$wafer_id.'&PME_sys_operation=Add">Add</a>';
	$html.=' <a style="color:blue" href="'.SJ_HOST_NAME.'epi/edit/epi_xrd.php?wafer_id='.$wafer_id.'">View/Edit</a>';
	
	$html.='<h2>';
	$html.='<hr>';
	$html.='<br>';
	
	$html.='<div style="width:500px">';
	
	//$html.=$this->show_wafer_images_thumbs($wafer_id);
	
	$html.='</div>';
	
	
	return $html;
}

function wafer_flag_has_images($wafer_id) {
	$query = "update epi_wafer set images = 1 where wafer_id = $wafer_id";
	$result = $this->local_insert($query);
}


// 11-17 
// function for automatic directory parsing
	
	
	
function file_name_extract($file)
{
	$data['file']=$file;
	$file = basename($file);

	$tmp = explode('.',$file);
	if (count($tmp) > 1) {
		$data['ext'] = array_pop($tmp);
	}
	else {
		$data['ext']='';
	}
	$tmp = implode('.',$tmp);
	
	$tmp = explode('_',$tmp);

	$data['sample'] = $tmp[0];
	$data['user'] = strtolower($tmp[1]);
	if (count($tmp) > 3) {
		$data['extra'] = $tmp[3];
	}
	else {
		$data['extra'] = '';
	}
	
	// $this->echo_r($tmp);
	
	$tmp = explode('-',$tmp[2]);
	if (strcmp($tmp[0], 'FSM') == 0) {
		if ($tmp[1] == 'A') {
			$data['annealed'] = YES;
			$data['temp'] = $tmp[2];
		}
		else {
			$data['annealed'] = NO;
		}
	}
	
	// due to stupidity we need to check if the '-' concatenated
	// measurement parameters are also "measurements"

	$meas = array();
	$data['meas'] = '';
	
	foreach($tmp as $t) {
		$meas[]= $t;
		// echo "<br> Checking for measurement ".implode('-',$meas);
		if ($this->check_pam_exists(implode('-',$meas))){
			$data['meas'] =implode('-',$meas);
			break;
		}
	}
	
	if ($data['meas'] == '') {
		$data['meas'] = array_shift($tmp);
	}
	else {
		$dump = array_shift($tmp);
	}
	
	$data['detail']=implode('-',$tmp);

	// add file last modified time info
	// $data['last_modified'] = date("Y-m-d H:i:s", filemtime($file));

	//if (strcasecmp($data['meas'], "RTA2") == 0) {
	if ((strcasecmp($data['meas'], "RTA2") == 0) || (strcasecmp($data['meas'], "RTA3") == 0)) {
		$data['meas'] = "RTA";
	}

	return $data;
}

function check_user($user) {
	
	$query = "select user_id from epi_user where user_name='$user'";
	
	$result = $this->local_query($query);
	
	if($result == NULL ) {
		return 0;
	}
	
	return $result[0]['user_id'];
	
}

function get_pam_file_selector(){
	$query = "select file_selector from epi_admin_process_and_measure ";
	$result = $this->local_query($query);
	$out =array();
	foreach ($result as $r) {
		$out[] = strtolower($r['file_selector']);
	}
	return $out;
}


function check_pam_exists($meas){
	$query = "select process_and_measure_id from epi_admin_process_and_measure where file_selector = '$meas'";
	// echo "<br> $query";
	$result = $this->local_query($query);
	
	if ($result != NULL && $result[0]['process_and_measure_id'] > 0) {
		return TRUE;
	}
	return FALSE;
}

function check_meas($meas)
{
	$measurements = array(
						'mberec',
						'mbetrend',
						'rheed',
						'rga',
						'microscope',
						'surfscan',
						'tarc',
						'xrr',
						'rsm',
						'pl',
						'ecv',
						'hall',
						'sims',
						'tfsims',
						'rta',
						'liv',
						'div',
						'civ',
						'cv',
						'qe',
						'qtqe',
						'qtiv',
						'fsm'
					);
					
	$measurements = $this->get_pam_file_selector();		
	return in_array(strtolower($meas),$measurements); 
}

function move_data_file($current_file, $target_file)
{
	if (file_exists($target_file) && file_exists($current_file)) {
		rename($target_file, $target_file.'-DUPLICATE-'.time());
	}

	if (copy($current_file, $target_file) ) {
		// echo "<br> $current_file copied to $target_file<br>";
		return $target_file;
	}
	else {
		return NULL;
	}
}



function check_meas_order()
// returns order in which files should be parsed starting with growth 
// dependent measurements and then going to wafer dep. If rta data is
// not present for the annealed pieces, an error will be thrown.
{
	$query = "SELECT CONCAT('_',file_selector) AS meas ".
           "FROM epi_admin_process_and_measure ".
           "ORDER BY pam_order ASC";
	$result = $this->local_query($query);
	$out = array();
	foreach ($result as $r) {
		$out[]=$r['meas'];
	}
	return $out;
}


function get_measurements() {

/*		
*		returns order in which files should be parsed starting with 
*   	growth dependent measurements and then going to wafer dep
*   	if rta data is not present for the annealed pieces, an error will
*   	be thrown
*/

	$query = "SELECT category,name, concat('_',file_selector) as meas FROM epi_admin_process_and_measure WHERE data_table <> '' order by category desc, pam_order desc";
	$result = $this->local_query($query);
	return $result;

} // end of check_meas_order



function meas_type($meas)
{
	$query = "SELECT object ".
		 "FROM epi_admin_process_and_measure ".
		 "WHERE file_selector = '$meas'";
	
	$result = $this->local_query($query);
	return $result[0]['object'];
}

function data_field_names($field) {

	$nicelookup = array("ext"=>"File Extension",
						"meas"=>"Measurement",
						"detail"=>"Measurement Details",
						"file"=>"File Name",
						"sample"=>"Sample Name",
						"user"=>"User Name");	
	return $nicelookup[$field];
}

function error_text($error) {

	$nicelookup = array(
			"user"=>"Unknown User Name",
			"meas"=>"Unknown Measurement",
			"detail"=>"Wrong Measurement Details",
			"file"=>"Could Not Read File",
			"sample"=>"Unknown Sample",
			"anneal"=>"Anneal Data Missing For Sample");	
	return $nicelookup[$error];
}


	
function extract_failure_notify($upload_id,$error,$data,$who)
{
	if ($who == 'admin') {
		$who = BEH_ADMIN;
	}
	
	$to = 'upload_errors@sj-solar.com';
	$subject=" DATA UPLOAD ERROR: ".$this->error_text($error);
		
	$message= "\n\n ".$subject.' when uploading '.$data['file']." on ".date('Y-m-d')."\n\n";
	$message.="The following parameters were extracted from the filename:\n\n";

	foreach($data as $k=>$v) {
			$message.="\n\n".$k.":\t\t".$v;
	}
			
	$message.="\n\n";
	// echo "<br> $message <br>";
		
	$mailsent = 1;//$this->send_report($to,$subject,$message); too many emails being generated, so turned this off
	
	$logged = $this->extract_error_log($upload_id,$who,$message,$mailsent);

	if (!$logged) {
		echo "<br> ERROR: Could not log error";
	}
}


function data_upload_notify($users)
{
	foreach ($users as $user) {
		echo "<br><br>Sending upload notice to: $user <br>";
		$this->parse_sucess_notify($user);
	}
}


function parse_sucess_notify($who)
{
	if ($who == 'admin') {
		$who = BEH_ADMIN;
	}
	
	$query ="SELECT upload_id, file, upload_file, upload_result ".
			"FROM epi_data_upload ".
			"WHERE notice_sent = 0 ".
			"AND user_name LIKE '$who' ".
			"AND upload_status LIKE 'success'";
	$result = $this->local_query($query);
	
	$subject = "Data upload results for: ".$who."\r\n"."\r\n";;
	$message ='';
	foreach ($result as $r) {
		$message.="Data \t\t ".$r['file']." was uploaded successfully \r\n";
		$message.="The uploaded file is in: \t\t ".$r['upload_file'];
		
		if (isset($r['warning']) && (strlen($r['warning']) > 0)) {
			$warning = explode(';',$r['warning']);
			$message.="The following WARNINGS were issued: \r\n";
			foreach ($warning as $wa) {
				$message.=$wa."\r\n";
			}
		}
		
		$message.="\r\n";
		$message.="Please review the data here: \t\t".$r['upload_result'];
		$message.="\r\n";
		$message.="\r\n";
	}
	
	$to = $who.'@sj-solar.com';
	$mailsent = $this->send_report($to,$subject,$message);
	foreach($result as $r) {
		$logged = $this->extract_success_log($r['upload_id'],$who,$message,$mailsent);
	}
}


function parse_failure_notify($upload_id,$measurement,$output,$data,$who)
{
	if ($who == 'admin') {
		$who = BEH_ADMIN;
	}
	// leave like this for now.
	$who = BEH_ADMIN;
	
	$to = 'upload_errors@sj-solar.com';
	//$to.= ', data@sj-solar.com';
	$subject="DATA UPLOAD PARSE ERROR: ".strtoupper(str_replace(' ','_',$measurement));
		
	$message = "\r\n";
	$message.= "\t".$subject." when uploading ".$data['file']." on ".date('Y-m-d')."\r\n";
	$message.= "\tThe following parameters were errors encountered when parsing the file:\r\n";

	foreach($output as $k=>$v) {
		$message.="\t".$k.":\t".$v."\r\n";
	}
			
	echo "<pre><br> DATA Structure:";
        // $this->echo_r($data);
	echo "</pre>";
	echo "<pre><br> $message <br></pre>";
		
	$mailsent = $this->send_report($to,$subject,$message);
	
	$logged = $this->extract_error_log($upload_id,$who,$message,$mailsent);
	
	if (!$logged) {
		echo "<br> ERROR: Could not log error";
	}
	
}

function parse_success($upload_id,$m,$upload_file,$upload_result,$table,$id,$data )
{
	
	$query = "update epi_data_upload 
		  set upload_file = '$upload_file',
		      upload_status = 'success',
		      upload_result = '$upload_result',
		      record_table = '$table',
		      record_id = '$id'
		where upload_id =  $upload_id ";
	$result = $this->local_insert($query);
}
	
function send_report($to, $subject, $message)
{
	// To send HTML mail, the Content-type header must be set
	$headers  = "MIME-Version: 1.0" . "\r\n";
	$headers .= "Content-type: text/html; charset=iso-8859-1" . "\r\n";
	$headers .= "X-Mailer: PHP"."\r\n";

	// Additional headers
	$headers .= "From: Behemoth Admin <".BEH_ADMIN."@sj-solar.com>" . "\r\n";
	$headers .= "Bcc: ".BEH_ADMIN."@sj-solar.com" . "\r\n";
	$headers .= "\n\n";
	
	$headers  = "From: webmaster@sj-solar.com" . "\r\n" ;
	$headers .= "Reply-To: ".BEH_ADMIN."@sj-solar.com" . "\r\n" ;
	$headers .= "Cc: data@sj-solar.com" . "\r\n";
	$headers .= "X-Mailer: PHP/" . phpversion()."\r\n";
	
	// Mail it
	$mailsent = mail($to, $subject, $message, $headers);
	return $mailsent;
}

function extract_error_log($upload_id,$who,$message,$mailsent) {
	$query = "update epi_data_upload
		      set   upload_status='error',
		      		message='".addslashes($message)."',
		      		notice_sent=$mailsent,
		      		user_notify='".$who."'
		      where upload_id = $upload_id
		      		";
	$result = $this->local_insert($query);
	return $result;
}


function extract_success_log($upload_id,$who,$message,$mailsent) {
	$query = "update epi_data_upload
		      set   upload_status='success',
		      		message='".addslashes($message)."',
		      		notice_sent=$mailsent,
		      		user_notify='".$who."'
		      where upload_id = $upload_id
		      		";
	$result = $this->local_insert($query);
	return $result;
}

function file_has_errors($filename){
	$qstart  = time();
	$query = "select * from epi_data_upload where file = '$filename' and upload_status='error'";
		
	// echo "<br> executing $query";		
	$result= $this->local_query($query);
	// echo "<br> query $query <br> took ".(time()-$qstart)." seconds ";	
	
	if ($result == NULL ) {
		return array('error'=>FALSE);
	}
	
	return array('error'=>TRUE,'date'=>$result[0]['date']);
	
}

function file_upload_time($filename){
	$qstart  = time();
	$query = "select * from epi_data_upload where file = '$filename'";

	// echo "<br> executing $query";		
	$result= $this->local_query($query);

	// echo "<br> query $query <br> took ".(time()-$qstart)." seconds ";	
	
	return array('date'=>$result[0]['date']);
	
}

function get_data_upload_id($data){
 	$qstart  = time();
	$query= "select upload_id from epi_data_upload where sample = '".$data['sample']."' AND measurement = '".$data['meas']."' AND user_name='".$data['user']."' AND file='".$data['file']."'";
	$result = $this->local_query($query);

	// echo "<br> query $query <br> took ".(time()-$qstart)." seconds ";

	if ($result == NULL ) {
		return NULL;
	} else {
		return $result[0]['upload_id'];
	}
	
}

function get_data_upload_record($data)
{
	$query= "SELECT * ".
		"FROM epi_data_upload ".
		"WHERE sample = '".$data['sample']."' ".
		  "AND measurement = '".$data['meas']."' ".
		  "AND user_name='".$data['user']."' ".
		  "AND file='".$data['file']."'";
	$result = $this->local_query($query);

	if ($result == NULL) {
		// return array();
		return NULL;
	} 
	return $result[0];
}

function new_data_upload($data)
{ 
	$query = "INSERT INTO epi_data_upload 
		  SET
			file ='".$data['file']."',
			date ='".date('Y-m-d H:i:s')."',
			user_name ='".$data['user']."',
			sample ='".$data['sample']."',
			measurement ='".$data['meas']."',
			details ='".$data['detail']."'";

	$result = $this->local_insert($query);
	return $this->last_insert();
}


// log the upload 

function data_log_check_finished()
// Checks if the previous data upload has finished
{
	$query = "SELECT * FROM epi_data_upload_log ORDER BY log_id DESC LIMIT 1";
	$result = $this->local_query($query);
	$row = $result[0];
	if ($row['finished'] < $row['started']) {
		return FALSE;
	}
	return TRUE;
}

function data_log_start()
// Starts new data upload
{
	if (!$this->data_log_check_finished()) {
		return 0;
	}
	
	$query = "INSERT INTO epi_data_upload_log SET started = ".time();
	$result = $this->local_insert($query);
	$data_log_id = $this->last_insert();

	return $data_log_id;
}

function data_log_stop($data_log_id)
{
	$query ="UPDATE epi_data_upload_log ".
			"SET finished = ".time()." WHERE log_id = $data_log_id";
	$result = $this->local_insert($query);
	return $this->data_log_time($query);
}


function data_log_time($data_log_id) {
	$query = "select finished-started as time from epi_data_upload_log where log_id= $data_log_id";
	$result= $this->local_query($query);
	return $result[0]['time'];
}

function last_data_log_id()
{
	$query = "SELECT log_id FROM epi_data_upload_log ORDER BY log_id DESC LIMIT 1";
	$result= $this->local_query($query);
	return $result[0]['log_id'];
}

function extract_anneal_name($data,$old_format=FALSE)
{
	$old_name = $data['sample'];
	$tmp  = explode('-',$data['detail']);
	$anneal_temp = trim($tmp[1]);
	if($old_format==false)
		$anneal_name  = $old_name.'-A';//.$anneal_temp;
	else
		$anneal_name = $old_name.'-A'.$anneal_temp;
	return $anneal_name;
}

function check_annealed($data)
{
	$query = "SELECT * FROM epi_rta_meas WHERE new_wafer_name = '".$this->extract_anneal_name($data)."'";
	$result = $this->local_query($query);
	if ($result == NULL )
	{
		$query = "SELECT * FROM epi_rta_meas WHERE new_wafer_name = '".$this->extract_anneal_name($data,TRUE)."'";
		$result = $this->local_query($query);
		if($result == NULL)
			return NULL;
	}
	
	return array('wafer_id'=>$result[0]['wafer_id'],'new_wafer_name'=>$result[0]['new_wafer_name'],'wafer_name'=>$result[0]['wafer_name']);
}


function check_wafer_annealed($wafer_name){
	
	$query = "select * from epi_rta_meas where new_wafer_name = '".$wafer_name."'";
	
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return NULL;
	}
	
	return array('wafer_id'=>$result[0]['wafer_id'],'new_wafer_name'=>$result[0]['new_wafer_name'],'wafer_name'=>$result[0]['wafer_name']);
}



// ====


//===== PL2 Extraction Functions =====//


function parse_pl2($data,$file)
{
	
//	echo "<br> New format parser : ";
//	$this->echo_r($data);
	
//	echo "<br> Data array";
	
	$header = array(
		"Date" => array('datetime','yes'),
		"Filename" => array('varchar(255)','yes'),
		"Folder"  => array('varchar(255)','yes'),
		"Operator"  => array('varchar(255)','yes'),
		"Wafer ID PL"  => array('varchar(255)','yes'),
		"Batch ID"  => array('varchar(255)','yes'),
		"Material"  => array('varchar(255)','yes'),
		"Description"  => array('varchar(255)','yes'),
		"Thickness"  => array('varchar(255)','yes'),
		"Wafer Diam"  => array('varchar(255)','yes'),
		"Scan Diam"  => array('varchar(255)','yes'),
		"Resolution"  => array('varchar(255)','yes'),
		"Scan Rate"  => array('varchar(255)','yes'),
		"Laser"  => array('varchar(255)','yes'),
		"Power"  => array('varchar(255)','yes'),
		"Filter"  => array('varchar(255)','yes'),
		"Gain"  => array('varchar(255)','yes'),
		"Temperature"   => array('varchar(255)','yes'),
		"Exc Zone"  => array('varchar(255)','yes'),
		"Center wavelength"  => array('varchar(255)','yes'),
		"Range"  => array('varchar(255)','yes'),
		"Slit width" => array('varchar(255)','yes'),
		"Grating"    => array('varchar(255)','yes'),
		"Detector"   => array('varchar(255)','yes')
	);

	$agg_fields = array("Peak Lambda","Peak Int","FWHM");

	$agg_fun = array("Max"=>'MAX(val)',
			 "Min"=>'MIN(val)',
			 "MED"=>'MEDIAN(val,5)',
			 "AVG"=>'AVG(val)',
			 "STD"=>'STDDEV_POP(val)');

	$agg_values = array(
		"Peak Lambda Max" => array('float','yes'),
		"Peak Lambda Min" => array('float','yes'),
		"Peak Lambda AVG" => array('float','yes'),
		"Peak Lambda MED" => array('float','yes'),
		"Peak Lambda STD" => array('float','yes'),
		"Peak Int Max" => array('float','yes'),
		"Peak Int Min" => array('float','yes'),
		"Peak Int AVG" => array('float','yes'),
		"Peak Int MED" => array('float','yes'),
		"Peak Int STD" => array('float','yes'),
		"FWHM Max" => array('float','yes'),
		"FWHM Min" => array('float','yes'),
		"FWHM AVG" => array('float','yes'),
		"FWHM MED" => array('float','yes'),
		"FWHM STD" => array('float','yes')
	);

	$header_sep = ":";	
	
	$values = array(
		"X"=>array('float','yes'),
		"Y"=>array('float','yes'),
		"Peak Lambda"=>array('float','yes'), 
		"Peak Int"=>array('float','yes'), 
		"Int. Signal"=>array('float','yes'),  
		"FWHM"=>array('float','yes')
	);
	
	
	$init = false;
	
	ini_set('auto_detect_line_endings',TRUE);

	$table_name = 'epi_pl2_meas';
	
	$meas_table_name = 'epi_pl2_meas_values';
	
	$primary_key = 'pl2_meas_id';
	$object_id = 'wafer_id';
	$object_name = 'wafer_name';
		
	if ($init) {

		echo "<br>";
		echo "CREATE TABLE IF NOT EXISTS `$table_name` (
			  $primary_key INT(11) NOT NULL auto_increment,
			  $object_id INT(11) NOT NULL,
			  $object_name VARCHAR(255) NOT NULL,
			  file_name VARCHAR(255) NULL,
				";
		foreach ($header as $k=>$v){
		
			if ($v[1] == 'yes') 
			{
				 $ksave = trim(trim($this->db_safe_name($k),'_'));
				echo "<br>`$ksave` ".$v['type']." ".$v[0]." NULL COMMENT '".addslashes($k)."', ";
			}
		}
		
		foreach ($agg_values as $k=>$v){
		
			if ($v[1] == 'yes') 
			{
				 $ksave = trim(trim($this->db_safe_name($k),'_'));
				echo "<br>`$ksave` ".$v['type']." ".$v[0]." NULL COMMENT '".addslashes($k)."', ";
			}
		}
		
		echo "	PRIMARY KEY (`$primary_key`), 
  				INDEX(`$object_id`,`$object_name`)
				) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1;";	
	
		echo "<br><br>";
		
		echo "ALTER TABLE `$table_name` ADD CONSTRAINT FOREIGN KEY (`wafer_id`) 
				REFERENCES epi_wafer(wafer_id) ON DELETE CASCADE;";

		echo "<br><br>";

		echo "CREATE TABLE IF NOT EXISTS `$meas_table_name` (
			  $primary_key INT(11) NOT NULL,
			 ";
		
		foreach ($values as $k=>$v){
			if ($v[1] == 'yes') 
			{
				 $ksave = trim(trim($this->db_safe_name($k),'_'));
				echo "<br>`$ksave` ".$v['type']." ".$v[0]." NULL COMMENT '".addslashes($k)."', ";
			}
		}
		
		echo "	 
			) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci ;";	

		echo "<br><br>";
	
		echo "ALTER TABLE `$meas_table_name` ADD CONSTRAINT FOREIGN KEY (`$primary_key`) 
				REFERENCES $table_name($primary_key) ON DELETE CASCADE;";
		
		echo "<br><br>";

	}
	

 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		echo "<br> Could not open file";
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	// check if this is the right format
	
	$string = fgetcsv($handle);
	while(!feof($handle) && $string[0] == NULL) 
	{
		$string = fgetcsv($handle);
	
	}
	
	if (!stristr($string[0],'ASCII'))
	{
		echo "<br> Wrong file format";
		return array('error'=>3,'point'=>'Wrong File Format');
	}
	
	$header_set = array();
	
	$header_set[$object_name] = $data['sample'];
	
	$header_set[$object_id] = $data['sample_id'];
	
	$header_set['file_name'] = $file;

	
	while(!feof($handle)) 
	{
		$string = fgets($handle);	
		
		$tmp = explode($header_sep,$string);
		
		$key = trim(array_shift($tmp));
		
		$val = addslashes(trim(implode($header_sep,$tmp)));
		
		//special:
		
		if(stristr($key,'Wafer ID'))
		{
			$key.= ' PL';
		}
		
		if (!empty($header[$key]) )
		{
			if(stristr($key,'date'))
			{
				$val = $this->parse_datetime($val);
			}
			$header_set[trim(trim($this->db_safe_name($key),'_'))] = $val;;		
		}
		
		// $ksave = trim(trim($this->db_safe_name($k),'_'));
		
		
		if (stristr($key,'Detector'))
		{
			break;
		}
	}
	
	
	
	
	// now get the data:
	$string = fgetcsv($handle);
	
	while(!feof($handle) && !stristr($string[0],'X')) 
	{
			$string = fgetcsv($handle);
	}
	
	array_pop($string);
	
	$data_order=array();
	foreach ($string as $str)
	{
		$data_order[] = trim(trim($this->db_safe_name($str),'_'));
	}
	
	$tmp = fgetcsv($handle);
	
	$data_set = array();
	
	while(!feof($handle))
	{
		$string = fgetcsv($handle);
	
		if($string == NULL)
			continue;
		else if($string != NULL)
		{
			if(strlen(trim(implode("",$string))) == 0 ) continue;
		}

		$dat = array_map("addslashes",array_map("trim",$string));
		$tmp = array();	
		for($i=0;$i<count($data_order); $i++ )
		{
			$tmp[$data_order[$i]] = $dat[$i];
		}
		$data_set[] = $tmp;
	}
	
	// close
	fclose($handle);
	
	//now add record and data
	
	//$this->echo_r($header_set);
	$header_set["manufacturing_location"] = ($data['user']=="iqe" ? "IQE" : "SJ");
	
	$query = "INSERT INTO 
				`$table_name` 
				SET ";
	$q = array();
	
	foreach ($header_set  as $k=>$v)
	{
		$q[] = "`$k` = '".$v."'";
	}
	
	$query = $query . implode(" , ",$q);
	
	$result = $this->local_insert($query);
	
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$meas_id = $this->last_insert();
	
	if ($meas_id == NULL){
		return array('error'=>2,'point'=>$query);
	}

	//$this->echo_r($header_set);
	
//	$this->echo_r($data_set);
	foreach ($data_set as $s)
	{
		$query = "INSERT INTO `$meas_table_name` SET ";
		$query .= " `$primary_key` = $meas_id , ";
		$q = array();
		foreach ($s as $k=>$v)
		{
			$q[] = "`$k` = '".$v."'";
		}
		
		$query .= implode(" , ",$q);
		
		//echo "<br> adding point $query";
		
		$result = $this->local_insert($query);
	
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}

		
	}
	
	// now clean up the data:
	
	$query= "SELECT range from `$table_name` WHERE `$primary_key` = $meas_id";
	
	$result = $this->local_query($query);

//	$this->echo_r($result);
	
	$range = trim($result[0]['range']);
	
	$tmp = explode(" ",$range);

	sscanf($tmp[0],"%f",$from);
	
	sscanf($tmp[2],"%f",$to);
	
	$to = 1240/($to - 2.5);
	
	$from = 1240/($from + 2.5);	
	
	$query = "DELETE FROM `$meas_table_name` WHERE 
				`peak_lambda` < $to AND `$primary_key` =$meas_id";
	
	//echo "<br> del to $query";
	
	$result = $this->local_insert($query);

	$query = "DELETE FROM `$meas_table_name` WHERE 
				`peak_lambda` > $from AND `$primary_key` =$meas_id";

	//echo "<br> del from $query";
	
	$result = $this->local_insert($query);
	
	
	// now we want to do the stats:
	
	$query = "SELECT ";
	$q=array();

	
	foreach ($agg_fields as $fld)
	{
		foreach ($agg_fun as $fun=>$fun_call)
		{
		
			$f = trim(trim($this->db_safe_name($fld),'_'));
		
			$as = trim(trim($this->db_safe_name($fld.' '.$fun),'_'));
			
			$q[] = str_replace('val',$f,$fun_call).' AS '.$as;
		}
		
	}

	$query .= implode(",",$q);
	$query .= " FROM `$meas_table_name` WHERE `$primary_key` = $meas_id ";
	
	
	
	$result = $this->local_query($query);
	
	$q = array();
	
	$query = " UPDATE `$table_name` SET ";
	foreach ($result[0] as $k=>$v)
	{
		$q[] = " `$k` = $v ";
	}
	
	$query .= implode(",",$q);
	$query .= " WHERE `$primary_key` = $meas_id ";
	
	//echo "<br> Stats query : $query";
	
	$result = $this->local_insert($query);
	
	return  array('error'=>0,'data_id'=>$meas_id);

}

// ====




//===== PL Extraction Functions =====//


function get_plmap_savefields(){
	return array('Material',
					'Description',
					'Grating',
					'Detector',
					'Laser',
					'Power',
					'Filter',
					'Date',
					'Filename',
					'Gain',
					'Scan Rate',
					'Slit width',
					'Thickness',
					'Temperature');

}


function parse_plmap_header($header) {
	
	$savefields = $this->get_plmap_savefields();
	
	foreach($savefields as $s) {
		$scompare[]=strtolower($s);
	}
	
	foreach ($header as $h) {
		$str = explode(':',$h);
		$key = strtolower(trim($str[0]));
		if (in_array($key,$scompare)) {
			if ($key == 'date') {
				$v=$this->parse_datetime(trim(str_replace('Date :','',$h)));
			} elseif( $key=='filename') {
				$v = trim(str_replace('Filename :','',$h));
			}else {
				$v = explode(' ',trim($str[1]));
				$v = trim($v[0]);
			}
			if ($key == 'scan rate') { 
				$key = 'scan_rate';
				$v = trim($str[1]);
			} 
			if ($key == 'slit width') { 
				$key = 'slit_width';
			} 
			
			if ($key == 'gain') {
				$v = trim($str[1]);
			}
			$header_store[$key]=trim($v);
		}
		if (stristr($key,'No of spectra')) {
			$total = trim($str[1]);
		}

	}
	
	$header_store['date_uploaded']=date('Y-m-d H:i:s');
	$header_store['file_header'] = addslashes(implode('',$header));
	return $header_store;
}

function array_to_set_query($array){
	$q=array();
	foreach ($array as $k=>$v) {
		$q[]="$k = '".addslashes($v)."'";
		
	}

	return implode(',',$q);
}


function add_pl_meas_values($wavelength,$power,$pl_meas_id) {
	
	for($i=0;$i<count($power);$i++) {
		$query = "insert into epi_pl_meas_values set
											pl_meas_id = $pl_meas_id,
											wavelength = '".$wavelength[$i]."',
											power = '".$power[$i]."'";
		$result = $this->local_insert($query);
		if (!$result ) {
			return array('error'=>2,'point'=>$query); ; // sql error
		} 			
		$result_ids[]=$this->last_insert();
		
	}
	
	return $result_ids;

}

function add_pl_meas_stats($pldata,$pl_meas_id) {
		$query = "update epi_pl_meas set pl_lambda_max = '".$pldata['xmax']."',
								 pl_power_max  = '".$pldata['max']."',
								 pl_fwhm       = '".$pldata['fwhm']."',
								 pl_fwhm_l	   = '".$pldata['xfwhm_l']."',
								 pl_fwhm_r	   = '".$pldata['xfwhm_r']."'
								 where pl_meas_id = $pl_meas_id
								 ";
		$result = $this->local_insert($query);
		return $result;
}


function get_pl_center_values($pl_id){
	
	$wafer_id = $this->get_pl_wafer_id($pl_id);
	
	$offset['X']=0;
	$offset['Y']=0;

	if ($this->is_piece($wafer_id)) {

		$xy = $this->get_piece_center($wafer_id);
		$offset['X']=$xy['x'];
		$offset['Y']=$xy['y'];

	}

	$query=	"SELECT *,abs((x-".$offset['X'].")*(x-".$offset['X'].")+(y-".$offset['Y'].")*(y-".$offset['Y'].")) as r FROM `epi_pl_meas` WHERE pl_id = $pl_id order by r asc limit 1";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	return $result[0];
}


function get_pl_stats($pl_id,$quant) { 
	$query ="select avg($quant) as avg, stddev_pop($quant) as std from epi_pl_meas where pl_id = $pl_id and spec='in'";
	$result = $this->local_query($query);
	return $result[0];
}

function filter_pl_data_by_lambda($pl_id,$nsigma) {
	
	// first get the stats:
	$query = "SELECT avg( pl_lambda_max ) as avg, STDDEV_POP( pl_lambda_max ) as std FROM epi_pl_meas WHERE pl_id =$pl_id and spec='in' ";
	$result = $this->local_query($query);
	$avg = $result[0]['avg'];
	$std = $result[0]['std'];
	
	$query = "update epi_pl_meas set spec = 'out' where
						abs( pl_lambda_max - ".$avg.") > ".($nsigma*$std)." and pl_id = $pl_id and spec = 'in'";
	echo "<br> $query <br>";
	$result= $this->local_insert($query);
	return array('avg'=>$avg,'std'=>$std);
	
}


function filter_pl_data_by_power($pl_id,$nsigma) {
	
	// first get the stats:
	$query = "SELECT avg( pl_power_max ) as avg, STDDEV_POP( pl_power_max ) as std FROM epi_pl_meas WHERE pl_id =$pl_id and spec='in' ";
	$result = $this->local_query($query);
	// $this->echo_r($result);
	$avg = $result[0]['avg'];
	$std = $result[0]['std'];
	
	$query = "update epi_pl_meas set spec = 'out' where
						abs( pl_power_max - ".$avg.") > ".($nsigma*$std)." and pl_id = $pl_id and spec = 'in'";
	echo "<br> $query <br>";
	$result= $this->local_insert($query);
	return array('avg'=>$avg,'std'=>$std);
	
}


function get_wafer_diam($wafer_id) {
	$query = "select s.substrate_diameter from epi_substrate s, epi_wafer w where w.wafer_id = $wafer_id and s.substrate_id = w.substrate_id";
	$result = $this->local_query($query);
	$d=$result[0]['substrate_diameter'];
	if ($d == NULL ) {
		return 4;
	}
	return $d;
}

function in_to_mm($in) {
	return $in*25.4;
}

function filter_pl_data_by_radius($pl_id,$edge) {
	
	$wafer_id  = $this->get_pl_wafer_id($pl_id);
	$d = $this->get_wafer_diam($wafer_id);
	$radius=($this->in_to_mm($d))/2-$edge;
	
	echo "bla";
	// first get the stats:
	$number_in = $this->filter_spec_count($pl_id,'in');
	echo "bla";
	$query = "select avg(y) as avy, avg(x) as avx from epi_pl_meas where pl_id = $pl_id";
	// echo "<br> $query";
	$result = $this->local_query($query);
	$xy = $result[0];
	
	// $this->echo_r($xy);
	$query = "update epi_pl_meas set spec = 'out' where
						((x-".$xy['avx'].")*(x-".$xy['avx'].")+(y-".$xy['avy'].")*(y-".$xy['avy'].")) > ".($radius*$radius)." and pl_id = $pl_id and spec = 'in'";
	echo "<br> $query <br>";
	$result= $this->local_insert($query);
	$new_number_in = $this->filter_spec_count($pl_id,'in');
	return ($number_in - $new_number_in);
}


function filter_pl_data_by_edge($pl_id) {
// may need to replicate their spatial map on upload to do this ... 
}

function filter_spec_count($pl_id,$spec) {
	$query  = "select count(*) as count from epi_pl_meas where pl_id = $pl_id and spec = '".$spec."'";
	//echo $query;
	$result = $this->local_query($query);
	return $result[0]['count'];

}


function pl_update_wafer_stats($pl_id)
{
	$center_values = $this->get_pl_center_values($pl_id);

	/* now update the toplevel entry with stats */
	
	$query = " UPDATE epi_pl
		   SET	pl_lambda_avg = ( SELECT avg( pl_lambda_max ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_power_avg = ( SELECT avg( pl_power_max ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_fwhm_avg = ( SELECT avg( pl_fwhm ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_lambda_median = ( SELECT MEDIAN( pl_lambda_max,5 ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_power_median = ( SELECT MEDIAN( pl_lambda_max,5 ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_lambda_stdev = ( SELECT STDDEV_POP( pl_lambda_max ) FROM epi_pl_meas WHERE pl_id =$pl_id ) , 
			pl_power_stdev = (SELECT STDDEV_POP( pl_power_max ) FROM epi_pl_meas WHERE pl_id =$pl_id ), 
			pl_fwhm_stdev = (SELECT STDDEV_POP( pl_fwhm ) FROM epi_pl_meas WHERE pl_id =$pl_id ), 
			pl_lambda_center = '".$center_values['pl_lambda_max']."',
			pl_power_center  = '".$center_values['pl_power_max']."',
			pl_fwhm_center   = '".$center_values['pl_fwhm']."' 
		   WHERE pl_id = $pl_id ";

	$result = $this->local_insert($query);

	return array('result'=>$result,'query'=>$query);

}

function pl_update_wafer_stats_spec($pl_id) {
	
	$center_values = $this->get_pl_center_values($pl_id);

	/* now update the toplevel entry with stats */
	
	$query = "
	 UPDATE epi_pl SET  pl_lambda_avg = ( SELECT avg( pl_lambda_max ) FROM epi_pl_meas WHERE pl_id =$pl_id and spec='in' ) , 
	 				    pl_power_avg = ( SELECT avg( pl_power_max ) FROM epi_pl_meas WHERE pl_id =$pl_id  and spec='in') , 
	 				    pl_fwhm_avg = ( SELECT avg( pl_fwhm ) FROM epi_pl_meas WHERE pl_id =$pl_id  and spec='in') , 
						pl_lambda_stdev = ( SELECT STDDEV_POP( pl_lambda_max ) FROM epi_pl_meas WHERE pl_id =$pl_id  and spec='in') , 
						pl_power_stdev = (SELECT STDDEV_POP( pl_power_max ) FROM epi_pl_meas WHERE pl_id =$pl_id  and spec='in'), 
						pl_fwhm_stdev = (SELECT STDDEV_POP( pl_fwhm ) FROM epi_pl_meas WHERE pl_id =$pl_id  and spec='in'), 
						pl_lambda_center = '".$center_values['pl_lambda_max']."',
						pl_power_center  = '".$center_values['pl_power_max']."',
						pl_fwhm_center   = '".$center_values['pl_fwhm']."' 
						where pl_id = $pl_id 	
						";

	$result = $this->local_insert($query);

	return array('result'=>$result,'query'=>$query);

}

function get_pl_wafer_id($pl_id) {
	$query = "select wafer_id from epi_pl where pl_id = $pl_id";
	$result = $this->local_query($query);
	if ($result == NULL ){
		return 0;
	}
	return $result[0]['wafer_id'];
}



function extract_plmap_spm_values(&$handle,$pl_id,$header){
	
	$wafer_id = $header['wafer_id'];
	
	$offset['X']=0;
	$offset['Y']=0;

	if ($this->is_piece($wafer_id)) {

		$xy = $this->get_piece_center($wafer_id);
		$offset['X']=$xy['x'];
		$offset['Y']=$xy['y'];

	}
	
	
	$wavelength = array();
	
	
	while(!feof($handle)) {
		$string=fgets($handle);
		if (stristr($string,'POS:')) {
			break;
		} else {
			$wavelength[]=trim($string);
		}
	}
	
	/* now get the data */
	
	$power=array();
	
	$str = explode(' ',str_replace(',','',$string));
	$pos_x = (float)trim($str[1]);
	$pos_y = (float)trim($str[2]);
	
	$insert_count = 0;
	
	while(!feof($handle)) {
		
		if (stristr($string,'POS:')) {
			
			if(count($power) > 0 ){
				$posX=$pos_x;
				$posY=$pos_y;
				
				//echo "<br> $string $posX,$posY ".$offset['X'].",".$offset['Y'];
				
				$query = "insert into epi_pl_meas set
											pl_id = $pl_id,
											wafer_id = $wafer_id,
											x='".($posX+$offset['X'])."',
											y='".($posY+$offset['Y'])."'";
											
				$result = $this->local_insert($query);
				$insert_count = $insert_count+1;
				if (!$result ) {
					return array('error'=>2,'point'=>$query); ; // sql error
				} else {
					$pl_meas_id = $this->last_insert();
				}							
			

				$result = $this->add_pl_meas_values($wavelength,$power,$pl_meas_id);
				if (isset($result['error']) && $result['error'] > 0 ) {
					return $result; // sql error
				}
				
				$pldata = $this->find_max_min_fwhm($wavelength,$power);
				
				$result = $this->add_pl_meas_stats($pldata,$pl_meas_id);
											
				if (!$result ) {
					return array('error'=>2,'point'=>$query); ; // sql error
				}
				
			}
			$str = explode(' ',str_replace(',','',$string));
			$pos_x = (float)trim($str[1]);
			$pos_y = (float)trim($str[2]);
			
			$power = array();
		} else {
			$power[]=trim($string);
		
		}
		$string = fgets($handle);
	}
	
	if ($insert_count == 0 ){
		return array('error'=>4,'point'=>'No Data Found');
	}
	
	return array('error'=>0,'point'=>'parse spm data');
	
}


function extract_plmap_spl_values(&$handle,$pl_id,$header) {
	
	$wafer_id = $header['wafer_id'];
	
	$coords = $this->get_pl_header_coords($header);

	$origin['X']=0;
	$origin['Y']=0;

	if ($this->is_piece($wafer_id)) {

		$xy = $this->get_piece_center($wafer_id);
		
		//$origin['X']=$xy['x']-$coords['X'];
		//$origin['Y']=$xy['y']-$coords['Y'];
		
		$origin['X']=$xy['x']; // spl for a piece always goes to center in case someone decides to plmap > 1 chips together
		$origin['Y']=$xy['y'];

	} else {
	
		$origin['X']=$coords['X']; // if this isn't a piece take the actual values from the header ...  
		$origin['Y']=$coords['Y']; 

	}
	
	$wavelength = array();
	$power=array();
		
	while(!feof($handle)) {			
				$string = fgets($handle);
				$string = preg_replace('/,?\s\s+/',',', trim($string));
				$str=explode(',',$string);
				
				if (trim($str[0]) != NULL ) {
					$wavelength[]=trim($str[0]);
					$power[]=trim($str[1]);
				}
				
	}
				
	if (count($power) > 0) {
		$query = "insert into epi_pl_meas set
							pl_id = $pl_id,
							wafer_id = $wafer_id,
							x='".$origin['X']."',
							y='".$origin['Y']."'";
		//echo $query;	
		$result = $this->local_insert($query);

		if (!$result ) {
			return array('error'=>2,'point'=>$query); // sql error
		} else {
			$pl_meas_id = $this->last_insert();
		}							
			
		$result = $this->add_pl_meas_values($wavelength,$power,$pl_meas_id);
		if (isset($result['error']) && $result['error'] > 0 ) {
			return $result; // sql error
		}
				
		$pldata = $this->find_max_min_fwhm($wavelength,$power);		
		$result = $this->add_pl_meas_stats($pldata,$pl_meas_id);
				
	} else {
		return array('error'=>4,'point'=>'No Data Found');
	}
	
	return array('error'=>0,'point'=>'parse spl data');
}



function get_pl_header_coords($header) {
	$xy = FALSE;
	$ra = FALSE;
	foreach ($header as $h) {
		$str = explode(':',$h);
		$key = strtolower(trim($str[0]));

		if (stristr($key,'Radius')) {
			$ra = TRUE;
			$rad = trim($str[1]);
		}
		if (stristr($key,'Angle')) {
			$ang = trim($str[1]);
			$ra = TRUE;
		}
		
		if ($key == 'x') {
			$v=explode(' ',trim($str[1]));
			$X = trim($v[0]);
			$xy = TRUE;
		}

		if ($key == 'y') {
			$v=explode(' ',trim($str[1]));
			$Y = trim($v[0]);
			$xy = TRUE;
		}

	}

	if ($xy) {
		return array('X'=>$X,'Y'=>$Y);
	} elseif ($ra) {
		return array('X'=>$rad*cos($ang*M_PI/180.0),'Y'=>$rad*sin($ang*M_PI/180.0));
	} else {
		return array('X'=>0,'Y'=>0);
	}
	
}


function parse_plmap($type,$user_id,$filename,$wafer_id,$wafer_name) {
	
	if ($wafer_name == NULL ) {
		$wafer_name = $this->get_wafer_name_by_id($wafer_id);
	}

	// get contents of a file into a string

	$handle = fopen($filename, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}

  	ini_set('auto_detect_line_endings',TRUE);

	// check if this is a new format, then try to parse with new parser -
	if ($type == 'spm' ) {		
		while(!feof($handle)) 
		{
			$str = fgets($handle);
			if (stristr($str,'FWHM'))
			{
				fclose($handle);
				return 2;
			}
		}
	}
	
	rewind($handle);
	
	/*skip the ascii line */
	while(!stristr($string,'ASCII') && !feof($handle)) {
		$string=fgets($handle);
	}
	if (!stristr($string,'ASCII')) {
		return array('error'=>1,'point'=>'ASCII'); // wrong file format
	}

	if ($type == 'spm' ) {
		$data_start  = 'DATA:';
	} else {
		$data_start  = '[nm]';
	}


	while(!stristr($string,$data_start) && !feof($handle)) {
		$header[]=$string;
		$string=fgets($handle);
	} 
	
	// if file format was wrong 
	if (feof($handle)) {
		return array('error'=>1,'point'=>'DATA'); // wrong file format
	}
	
	
	
	// now parse the header 
	$header_store = $this->parse_plmap_header($header);

	$header_store['filename_source']= $header_store['filename'];
	$header_store['filename']=addslashes($filename);
	$header_store['wafer_id']=$wafer_id;
	$header_store['wafer_name']=$wafer_name;
	$header_store['user_id']=$user_id;
	
	// build the query
	$set_query = $this->array_to_set_query($header_store);
	
	// insert the measurement
	$query = "insert into epi_pl set ".$set_query;
	
	$result = $this->local_insert($query);

	if (!$result ) {
		return array('error'=>2,'point'=>$query); ; // sql error
	} else {
		$pl_id = $this->last_insert();
	}	
	
	/* we want to insert the PL value closest to center as the average, and then the deviation from this across the wafer */
	if ($type == 'spm') {
		$result = $this->extract_plmap_spm_values($handle,$pl_id,$header_store);
	} else {
		$result = $this->extract_plmap_spl_values($handle,$pl_id,$header_store);
	}
	
	if ($result['error']>0) {
		return $result;
	}
	
	$result = $this->pl_update_wafer_stats($pl_id);
	
	if (!$result['result'] ) {
			return array('error'=>2,'point'=>$result['query']); ; // sql error
	} else {
		
	}
			
	fclose($handle);

	return array('error'=>0,'data_id'=>$pl_id);
} // eof parse_plmap_spm



function find_max_min_fwhm($x,$y){
// do the stupid thing:
	
	$max = -1;
	$imax = 0;
	$min = 1e8;
	$imin = 0;
	for ($i=0;$i<count($x);$i++ ) {
		if ($y[$i] > $max) {
			$max=$y[$i];
			$imax = $i;
		}
		if ($y[$i] < $min) {
			$min=$y[$i];
			$imin = $i;
		}
	}
	
	
	
	$target = $max-($max-$min)/2.0;
	
	
	
	$ifwhm_r = 0;
	$ifwhm_l = 0;

	for ($i=$imax;$i<count($x);$i++) {
		$ifwhm_r = $i;
		if ($y[$i] <= $target) {
			break;	
		}
	}

	for ($i=$imax-1;$i>=0;$i--) {
		$ifwhm_l = $i;
		if ($y[$i] <= $target) {
			break;	
		}
	}
	
	$dat= array(	'max'=>$max,'imax'=>$imax,'xmax'=>$x[$imax],
			 		'min'=>$min,'imin'=>$imin,'xmin'=>$x[$imin],
			 		'fwhm_r'=>$y[$ifwhm_r],'ifwhm_r'=>$ifwhm_r,'xfwhm_r'=>$x[$ifwhm_r],
			 		'fwhm_l'=>$y[$ifwhm_l],'ifwhm_l'=>$ifwhm_l,'xfwhm_l'=>$x[$ifwhm_l],
			 		'fwhm'=>($x[$ifwhm_r]-$x[$ifwhm_l])
			 	);
	//$this->echo_r($dat);
	return $dat;

}







//==================================//


//===== MBE trend data extraction functions ===== //

 

function get_growth_details($growth_id) {
	
	$query = "select * from epi_growth where growth_id = $growth_id ";
	$result = $this->local_query($query);
		
	if($result != NULL ){
		return $result[0];
	}
	return NULL;
}

function get_mbe_conversion_values() {
	
	$query  = "select * from epi_mbe_meas_values_kv where mbe_epitrend != ''";
	$result = $this->local_query($query);
	
	foreach ($result as $r) {
		$conversion_values[$r['mbe_epitrend']]=$r['mbe_key'];
	}
	
	return $conversion_values;
}


function get_mbe_for_growth($growth_id) {
	$query = "select * from epi_mbe where growth_id = $growth_id";
	$result = $this->local_query($query);
	return $result;
	
}

/*
function mbe_add_regrowth($parent_growth_id,$growth_id){
	echo "<br> adding regrowth ";
	$query = "select mbe_id from epi_mbe where growth_id = $growth_id";
	$result = $this->local_query($query);
	if($result == NULL || $result[0]['mbe_id'] == NULL ) {
		echo "<br> no mbe data found";
		return;
	}
}*/

function parse_mbe_epitrender($file,$growth_id) {
	

echo "<p> parse_mbe_epitrender: file: $file - growth_id: $growth_id</p>";

	$growth_details = $this->get_growth_details($growth_id);
	
	$current_mbe_data = $this->get_mbe_for_growth($growth_id);
	
	if ($current_mbe_data != NULL )
	{
	 return array('error'=>5,'point'=>'Data Exists');
	}
	if ($growth_details == NULL )
	{
	  return array('error'=>1,'point'=>'Growth Details');
	}	
	
	$grower_id   = $growth_details['grower_id'];
	$description = $growth_details['description'];
	$growth_date = $growth_details['growth_date'];
	$growth_name = $growth_details['growth_name'];
	$growth_chamber = $growth_details['chamber_id'];
	
	$handle = fopen($file, 'r');
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	/* first get the column headers and make sure that they match our data structure */
	$string = fgets($handle);
	$string=explode(',',preg_replace('/\t/',',',$string));
	
	/* now create the index array */
	$conversion_values = $this->get_mbe_conversion_values();
	
	$count = 0;
	$column_headers = array();
	foreach ($string as $s) {
		$column_headers[]=$conversion_values[trim($s)];
	}

	/* now scan the data */
	$layer  =  NULL;
	$current_layer  = NULL ;
	$layer_count = 0;
	$layer_material_count = array();
	
	/* create the mbe entry so that we can add data to it */
	$query = "insert into epi_mbe (growth_id,growth_name,date,date_added,description,grower_id,data_file_raw) 
		values ('$growth_id','$growth_name','$growth_date','".date('Y-m-d H:i:s')."','".addslashes($description)."','$grower_id','".addslashes($file)."')"; 
	$result = $this->local_insert($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query); ; // sql error
	}
	
	$mbe_id = $this->last_insert();
	
	if ($mbe_id == NULL ) {
		return array('error'=>2,'point'=>$query); ; // sql error
	}
	
	$query = "insert into epi_growth_mbe_relation (growth_id,mbe_id,date) 
		values    ('$growth_id','$mbe_id','$growth_date')";
	$result = $this->local_insert($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query); ; // sql error
	}
	
	$mbe_meas_id = NULL;
	$mbe_meas_id_array=array();
	while(!feof($handle) ) {
		$row = array();
		$string = fgets($handle);
		
		if (strlen(trim($string)) <= 0) {
			continue;
		}
		
		$string=explode(',',preg_replace('/\t/',',',$string));
		
		for($i=0;$i<count($string); $i++) {
			$val = trim($string[$i]);
			if(stristr($column_headers[$i],'shutter'))
			{
				//if this is a 'shutter' column, switch the on/off values from open==0 and closed==1 to the opposite
				if ($val == 0 ){ 
					$val = 1;
				} elseif ($val == 1) {
					$val = 0;
				}
			}
			$row[$column_headers[$i]] = $val;
			
		}
		$layer = $this->mbe_layer_sql2($row);
		if ($current_layer != $layer && $layer != NULL ) {
		
			if ($mbe_meas_id != NULL ){
				// update the stats from the previous values 
				
				echo "<br>Extracting stats for layer $layer_name<br>";
				
				$stats_query  = $this->get_stats_for_layer_query($mbe_meas_id);
				//echo "<br> $stats_query";
				$result = $this->local_query($stats_query);
				foreach ($result as $r) {
					
					if ($r['Time_____s___'] > 0) {
						$query2 = "update epi_mbe_meas set ";
						foreach ($r as $k=>$v) {
							 $q[]= " $k = '$v' ";
							 
						}		
						$query2 .= implode(',',$q)." where mbe_meas_id = $mbe_meas_id";
						$result2 = $this->local_insert($query2);
						
						if ($result2 == NULL ) {
							return array('error'=>2,'point'=>$query2); ; // sql error
						}
						
					} else {
						// otherwise we want to delete this layer 
						echo "<br> found layer with 0 time ... undoing";
						$query2="delete from epi_mbe_meas where mbe_meas_id = $mbe_meas_id";
						
						$result2=$this->local_insert($query2);
						
						if ($result2 == NULL ) {
							return array('error'=>2,'point'=>$query2); ; // sql error
						}
						// decrement layer if it was erroneous 
						$count_reduce = $layer_material_count[$layer];
						if ($count_reduce > 1) {
							$layer_material_count[$layer]= $count_reduce-1;
						}
						
					}
				}
			
			}
		
		
			$current_layer = $layer;
			
			$layer_count = $layer_count+1;
			$layer_material = $layer;
			
			$prev_num = $layer_material_count[$layer];
			if ($prev_num == NULL ) {
				$prev_num = 0;
			}
			$layer_material_count[$layer] = $prev_num + 1;
			
			$layer_name = $layer.'-'.$layer_material_count[$layer];
			
			$query = "insert ignore into epi_mbe_meas (mbe_id,growth_name,growth_chamber,layer_number,layer_material,layer_name) values ('$mbe_id','$growth_name','$growth_chamber','$layer_count','$layer','$layer_name')";
			echo "<br>Found a new layer $layer_name";
			$result= $this->local_insert($query);
			
			if ($result != NULL ){
				$mbe_meas_id = $this->last_insert();
				echo "<br> Adding new MBE layer with id $mbe_meas_id";
				if ($mbe_meas_id == NULL ) {
					echo "<br> WARNING 0 id: $query";
				}
				$mbe_meas_id_array[] = $mbe_meas_id;
			}
			
			if ($result == NULL ) {
				echo "<br> Warning when executing $query";
			}
			
			if ($result == NULL && $mbe_meas_id == NULL ){
				return array('error'=>2,'point'=>$query); // sql error
			}
			
			
		} else {
			if ($layer != NULL ){
				$query = "insert into epi_mbe_meas_values (mbe_id,mbe_meas_id,layer_number,layer_name,".implode(',',array_keys($row)).") values ('$mbe_id','$mbe_meas_id','$layer_count','$layer_name','".implode("','",array_values($row))."')";
				$result = $this->local_insert($query);
				//echo "<br> $query";
				
				if ($result == NULL  ){
					return array('error'=>2,'point'=>$query); // sql error
				}
				
			}
		}
	}
	
	/* last insert on the last layer */
	if ($mbe_meas_id != NULL ){
				// update the stats from the previous values 
				
				echo "<br>Extracting stats for layer $layer_name<br>";
				
				$stats_query  = $this->get_stats_for_layer_query($mbe_meas_id);
				//echo "<br> $stats_query";
				$result = $this->local_query($stats_query);
				foreach ($result as $r) {
					
					if ($r['Time_____s___'] > 0) {
						$query2 = "update epi_mbe_meas set ";
						foreach ($r as $k=>$v) {
							 $q[]= " $k = '$v' ";
							 
						}		
						$query2 .= implode(',',$q)." where mbe_meas_id = $mbe_meas_id";
						$result2 = $this->local_insert($query2);
						
						if ($result2 == NULL ) {
							return array('error'=>2,'point'=>$query2); ; // sql error
						}
						
					} else {
						// otherwise we want to delete this layer 
						echo "<br> found layer with 0 time ... undoing";
						
						$query2="delete from epi_mbe_meas where mbe_meas_id = $mbe_meas_id";
						echo "<br> $query2";
						$result2=$this->local_insert($query2);
						
						if ($result2 == NULL ) {
							return array('error'=>2,'point'=>$query2); ; // sql error
						}
						
						$count_reduce = $layer_material_count[$layer];
						$layer_material_count[$layer]= $count_reduce-1;
					}
				}
			
	}
	
	return array('error'=>0,'data_id'=>$mbe_id);
	
}

//==================================//
 
//========= RTA Functions  ===========//
 
function check_annealed_rta($wafer_name)
{
 	$query = "SELECT count(*) AS count FROM epi_rta_meas WHERE wafer_name = '$wafer_name'";
	// echo "<br> $query <br>";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return FALSE;
 	}
 	$count = $result[0]['count'];
 	return ($count > 0);
}
 
function parse_rta($file,$user_id,$wafer_id,$date,$peak_temp_intended,$peak_time_intended, $rta_tool)
{
echo "<p>1. $rta_tool</p>";
 	$wafer_name = $this->get_wafer_name_by_id($wafer_id);
	
	//$new_wafer_name = $wafer_name."-A";
	$new_wafer_name = $wafer_name."-A";//.$peak_temp_intended; (we no longer append A780 to wafer names, just -A)
	// $new_wafer_name = $wafer_name.'-A'.$peak_temp_intended.'t'.$peak_time_intended;
	
	$rta_recipe = 'TC '.$peak_temp_intended.'C';
	
	// echo "<br>Wafer: $wafer_name New Name: $new_wafer_name Recipe: $rta_recipe File: $file";
	
	$warnings = array();
	if ($peak_time_intended == NULL ){
		$peak_time_intended = 0;
		$warnings[]='Intended Peak Time of 0 seconds!';
	}
		
	if ($this->check_annealed_rta($wafer_name) ) {
		return array('error'=>5,'point'=>'Anneal Data Exists');	
	}
	
	$handle = fopen($file, "r");

	if ($handle == NULL ) {
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	// create the new RTA entry
	$query="insert into epi_rta_meas (rta_recipe,wafer_id,wafer_name,new_wafer_name,date,user_id,peak_temp_intended,peak_time_intended,data_file, tool) 
			values ('$rta_recipe','$wafer_id','$wafer_name','$new_wafer_name','$date','$user_id','$peak_temp_intended','$peak_time_intended','$file', '$rta_tool')";
	
	$result  = $this->local_insert($query);
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query); // sql error
	}
	$rta_meas_id = $this->last_insert();

	if ($rta_meas_id == NULL ) {
		return array('error'=>2,'point'=>$query); // sql error
	}
	
	/* file layout
	1: Time (sec)
	2: Temperature (C)
	3: NA (keep this as data, it might end up being the pyrometer temp after we get that working)
	4: Lamp Power (AU)
	5: N2 Flow (sccms)
	*/
	
	$total_power =-1000;
	$peak_time = -1000;
	$peak_temp = -1000;
	
	$rowcount=0;
	$rowdelay = 10;
	$rectime = FALSE;
	$rec_times = array();
	$time_at_peak=0;
	$time_previous=0;
	while(!feof($handle)) {
		$string=fgets($handle);
		$rowcount = $rowcount+1;
		if (($rowcount <= 2) && (strcasecmp($rta_tool, "IQE RTA") == 0)) { 
			continue;	
		}
		if (($rowcount <= 2) && (strcasecmp($rta_tool, "AllWin 610") == 0)) { 
			continue;	
		}
		if (strcasecmp($rta_tool, "IQE RTA") == 0) {
			$string = ereg_replace("[ \t]", "", $string);
			echo "<p>RTA tool: $rta_tool - string: $string</p>";
		}
		else if (strcasecmp($rta_tool, "AllWin 610") == 0) {
			$string = ereg_replace("[ \t]", "", $string);
			echo "<p>RTA tool: $rta_tool - string: $string</p>";
		}
		else {
			$string = ereg_replace("[ \t,]", " ", $string);
		}
	
		if (strlen($string) > 0 && $rowcount > $rowdelay) {
			switch ($rta_tool) {
				case "IQE RTA":
				case "AllWin 610": // RTA2
					$str = explode(",", $string);
					//$this->echo_r($str);
					$time = trim($str[0]);
					$temp = trim($str[2]);
					$pyro = trim($str[1]);
					$lamp = trim($str[3]);
					$n2fl = 3; //trim($str[4]);
				break;

				case "SSI":
				default:
					$str=explode(" ", $string);
					$time = $str[0];
					$temp = $str[1];
					$pyro = $str[2];
					$lamp = $str[3];
					$n2fl = $str[4];
				break;
			}
			
			$query = "insert into epi_rta_meas_values (rta_meas_id,time,temp,pyro,lamp,n2fl) 
					values ($rta_meas_id,$time,$temp,$pyro,$lamp,$n2fl) ";
		
			$result  = $this->local_insert($query);
			
			if ($result == NULL ) {
				echo '<br> there was an error insertin the data point <br> ';
				echo $query;
			//	return array('error'=>2,'point'=>$query); // sql error
			} else {

			
				if ($temp > 0.97*$peak_temp_intended && $temp < 1.03*$peak_temp_intended ) {
					$rectime = TRUE;
					$rec_times[]=$time;
				} else {
					if ($rectime && count($rec_times) > 0 ) {
						$time_at_peak = $time_at_peak + max($rec_times) - min($rec_times);
						$rec_times = array();
						$rectime = FALSE;
					}
				}
			
				if ($temp > $peak_temp ) {
					$peak_temp = $temp;
					$peak_time = $time;
				}
				
				if (($lamp >= 2) && ($time_previous != 0)) {
					if ($total_power == -1000) {
						$total_power = 0;
					}
					$total_power += $lamp * ($time - $time_previous);
				}
				$time_previous = $time;
			}	
		}
		
		
	}	
	
	// if we're ok thus far, update the wafer name 
	
	$query = "update epi_wafer set wafer_name = '$new_wafer_name', annealed = 1 where wafer_id = $wafer_id";
	$result = $this->local_insert($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query); // sql error
	}
	// echo "<br> Important the wafer should now be renamed from $wafer_name to $new_wafer_name";
	$warnings[]="Important: the wafer should now be renamed from $wafer_name to $new_wafer_name";
	// echo "<br> found a peak temp of $peak_temp at time $peak_time <br> total power is $total_power";
	if ($peak_temp > 0 && $peak_time > 0 ) {
		/* to fix old values use:
		SELECT concat('update epi_rta_meas set peak_time_actual=',max(v.time)-min(v.time),' where rta_meas_id=',m.rta_meas_id,';') as q,v.rta_meas_id, min(v.time) as tstart, max(v.time) as tstop, max(v.time)-min(v.time) as tdiff FROM epi_rta_meas_values v, epi_rta_meas m WHERE m.rta_meas_id = v.rta_meas_id and v.temp > 0.97*m.peak_temp_intended group by v.rta_meas_id
		*/
		$query ="SELECT min(v.time) as tstart, max(v.time) as tstop, max(v.time)-min(v.time) as tdiff FROM epi_rta_meas_values v, epi_rta_meas m WHERE m.rta_meas_id = v.rta_meas_id and v.temp > ".(0.95*$peak_temp)." and m.rta_meas_id = $rta_meas_id";
		$result = $this->local_query($query);
		
		if ($result != null ) {
			$peak_time = $result[0]['tdiff'];
		}
		
		$query = "update epi_rta_meas set peak_temp_actual = $peak_temp,
											peak_time_actual = $time_at_peak,
											total_power = $total_power
											where rta_meas_id = $rta_meas_id
											";
		//echo $query;
		$result = $this->local_insert($query);
		if ($result == NULL ) {
			return array('error'=>2,'point'=>$query); // sql error
		}
		
	} else {
		echo "<p> an error was encountered when finding the max time and temp. <p>";
		$warnings[]='Could not find max time and max temp';
	
	}
 
 	return array('error'=>0,'data_id'=>$rta_meas_id,'warnings'=>$warnings);
 
}
//== parse_rta =================================// 
 
 // ==== HALL =====//
 
 function check_hall($wafer_name) {
 	$query = "select count(*) as count from epi_hall_meas where wafer_name = '$wafer_name'";
	echo "<br> $query <br>";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return FALSE;
 	}
 	$count = $result[0]['count'];
 	return ($count > 0);
 	
 }
 
 
  function parse_hall($file,$user_id,$wafer_id,$wafer_name) {
 	
 	if ($wafer_name == NULL ) {
	 	$wafer_name = $this->get_wafer_name_by_id($wafer_id);
	}
	
	/*if ($this->check_hall($wafer_name)) {
		return array('error'=>5,'point'=>'Hall Data Exists');	
	}*/
	
	$handle = fopen($file, "r");

	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	$dbkeys=$this->get_table_columns('epi_hall_meas');
	// $this->echo_r($dbkeys);
	$arr=array();
	while(!feof($handle)) {
		$string=fgets($handle);
		echo "<br> $string";
		$string_tmp=$string;
		$string=explode(':',$string);
		$key=str_replace(' ','_',trim(strtolower($string[0])));
	
		if (stristr($key,'descr')) {
			$value=trim($string[1]);	
		} elseif(stristr($key,'date')) {
		
			$key='date';
			
			$value=implode(':',$string);
			$value=explode(' ',$value);
			$value=trim($value[1]);
		
			$value=explode(',',$value);
			$time=$value[0];
			$value=explode('/',$value[1]);
			$day=$value[0];
			$month=$value[1];
			$year=$value[2];
			list($hour,$min)=explode(':',$time);
			$value= date("Y-m-d H:i:s",mktime($hour,$min,0,$month,$day,'20'.$year));
		
		} else {
	
			$value=explode(' ',trim($string[1]));
			$value=$value[0];
	
		}
	
		$value=trim($value);
		if ($key != NULL && in_array($key,$dbkeys) ) {
			$arr[$key] = $value; 
		}
	}
 	$arr['wafer_id']=$wafer_id;
 	$arr['wafer_name']=$wafer_name;
 	$arr['user_id']=$user_id;
 	$arr['data_file']=$file;
 	$arr['date_added']=date('Y-m-d');
 	// $this->echo_r($arr);
 	
 	$query = "insert into epi_hall_meas (".implode(",",array_keys($arr)).")
		 						values ('".implode("','",array_values($arr))."')";
	$result = $this->local_insert($query);
	
	if ($result == NULL ) {
			return array('error'=>2,'point'=>$query); // sql error
	}
	
	$hall_meas_id = $this->last_insert();
 	if ($hall_meas_id == NULL ) {
			return array('error'=>2,'point'=>$query); // sql error
	}
 	
 	return array('error'=>0,'data_id'=>$hall_meas_id);
 
 }
 
 
 //===================================//
 
 
   //============ Mask Map ==============//
 
 
  function check_device_mask($device_mask) {
 	$query = "select count(*) as count from epi_device_mask where device_mask = '$device_mask'";
	echo "<br> $query <br>";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return FALSE;
 	}
 	$count = $result[0]['count'];
 	return ($count > 0);
 	
 }
 
 function create_device_mask($device_mask,$user_id,$date) {
 	$query = "insert into epi_device_mask set
 							device_mask = '$device_mask',
 							user_id         = '$user_id',
 							date_created    = '$date'";
 	//echo "<br> $query";
 	$result = $this->local_insert($query);
 	return array('result'=>$result,'query'=>$query,'id'=>$this->last_insert());
 }
 
 function parse_device_mask($file,$user_id,$device_mask) {
 	$device_mask = trim($device_mask);
 	
  	ini_set('auto_detect_line_endings',TRUE);

	
 	$handle = fopen($file, "r");
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
 	
  	// create the device map
 	if ($this->check_device_mask($device_mask)) {
		return array('error'=>5,'point'=>'Device Mask Exists');	
	}
 	
 	$result = $this->create_device_mask($device_mask,$user_id,date('Y-m-d H:i:s'));
 	
 	if ($result['result'] == NULL ) {
 		return array('error'=>2,'point'=>$result['query']);
 	} 
 	
 	$device_mask_id =$result['id'];
 	
 	if ($device_mask_id == NULL ) {
 		return array('error'=>2,'point'=>$result['query']);
 	}
 	
 	// first extract the columns : 
 	$string = fgetcsv($handle);
 	

 	for ($i=0; $i<count($string); $i++) {
		$s = $string[$i];
 		$tmp=explode('(',$s);
 		$s=$tmp[0];
 		
		$cols[$i]=implode('_',explode(' ',trim(strtolower($s)))); 	
 	}
 	
 	$cols[0]='device_mask';
 	
 	$cols = array_flip($cols);
 	
 	$dbkeys=$this->get_table_columns('epi_device');
 	
 	// foreach of the devices (if device is found get the id) else create it
 	while(!feof($handle)) {
		
		$string = fgetcsv($handle);
		
		if (strlen(trim(implode('',$string))) > 0) {
			$data = array();
			foreach($dbkeys as $k) { 
				$data[$k]=str_replace('%','',$string[$cols[$k]]);
			}
			
			$data['device_mask']= $device_mask;
			$data['device_mask_id']=$device_mask_id;
			unset($data['device_id']);
			
			$query = "insert into epi_device 
								(".implode(',',array_keys($data)).") 
						values  ('".implode("','",array_values($data))."')";
			
			//echo "<br> $query";
			$result = $this->local_insert($query);
			if ($result == NULL ) {
				return array('error'=>2,'point'=>$query);
			}
			
		}
	}
	
	return array('error'=>0,'data_id'=>$device_mask_id);
	
 	 	
 }
 
 function db_safe_name($name){
	return strtolower(preg_replace('/[^a-z0-9\_]/','',str_replace(' ','_',strtolower(addslashes($name)))));
 }
 
 
  //===================================//
 
  
   //============ QE Data ==============//
 
function qe_get_header(&$handle) {
	
	$string = fgetcsv($handle);
	$header = array();
	while(!feof($handle) && !stristr($string[1],'Cal')) {	
			$tmp = array();
			if (stristr($string[0],'time')) {
				$str = implode(',',$string);
				echo "<br> Found Time ".$string[0]." whole $str";;
				$tmp2=explode(':',trim($str));
				
				$tmp[0]=array_shift($tmp2);
				$tmp[1]=implode(':',$tmp2);
				
			} else {
				$tmp=explode(':',trim($string[0]));
			}
			
			if (!stristr($string[0],'Standard EQE') && !stristr($string[0],'Standard File Path')){
				$key = strtolower(str_replace(' ','_',str_replace(')','',str_replace('(','',str_replace('.','',str_replace('-','',trim($tmp[0])))))));
				$val = trim($tmp[1]);
				$header[$key]=$val;
			}
			$string = fgetcsv($handle);
			//$this->echo_r($string);
	}
	

	unset($header['absolute_die_coords']);
	unset($header['']);
	
	return $header;
}
 
 
function check_qe($header){
	unset($header['jsc_am15d']);
	unset($header['jsc_am15g']);
	unset($header['jsc_am0']);
	unset($header['x']);
	unset($header['y']);
	unset($header['die_number']);
	unset($header['start_time']);
	unset($header['end_time']);
	unset($header['multiplier_grid']);  	//varchar(10)  	utf8_unicode_ci  	  	No  	 	 	  Browse distinct values   	  Change   	  Drop   	  Primary   	  Unique   	  Index   	 Fulltext
	unset($header['multiplier_detector_area']); 	//varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['multiplier_ar_coating']); 	//varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['multiplier_all']); 	//varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['starting_wavelength_nm']); //	varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['ending_wavelength_nm']); 	//varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['integration_start_nm']); 	//varchar(10) 	utf8_unicode_ci 		No 			Browse distinct values 	Change 	Drop 	Primary 	Unique 	Index 	Fulltext
	unset($header['jsc_am15d_iqe']);
	unset($header['jsc_am15d_eqe']);
	unset($header['bandgap_ev']);
	unset($header['standard_file_path']);
	unset($header['step_size_nm']);
	unset($header['file_name']);
	
	$query = "select qe_id from epi_qe where ";
	foreach ($header as $k=>$v) {
			$v = trim($v,",");

			if($k === 'chuck_temperature') {
				$q[]= " $k LIKE '".rtrim($v,'0')."%' ";  
			} else {
				$q[]=" $k='$v' ";
			}		
	
	}
	
	$query .=implode(' and ',$q);
	$query.= " ORDER BY qe_id desc LIMIT 1 ";
	
	//echo '<pre>';
	// echo "<br> $query";
	//echo '</pre>';
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return 0;
	}
	return $result[0]['qe_id'];
	
}

function check_qe_meas($qe_id,$header) {

	return 0;

	if ($this->is_qtqe($header['file_header']) ) {
		return 0;
	}
	
	$query = "select qe_meas_id from epi_qe_meas where qe_id = $qe_id and die_number = '".$header['die_number']."'";
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return 0;
	}
	
	return $result[0]['qe_meas_id'];
	
}

function new_qe($header) {
	unset($header['jsc_am15d']);
	unset($header['jsc_am15g']);
	unset($header['jsc_am0']);
	unset($header['x']);
	unset($header['y']);
	unset($header['die_number']);
	unset($header['start_time']);
	unset($header['end_time']);
	unset($header['file_name']);
	unset($header['jsc_am15d_iqe']);
	unset($header['jsc_am15d_eqe']);
	unset($header['bandgap_ev']);
	
	echo '<pre>';
	$query = "insert into epi_qe (".implode(',',array_keys($header)).") 
						values	('".implode("','",array_values($header))."') ";
	
	// echo "<br> $query";
	echo '</pre>';
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$qe_id = $this->last_insert();
	if($qe_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0,'data_id'=>$qe_id);
	

}

function new_qe_meas($qe_id,$header) {
	$dbkeys=$this->get_table_columns('epi_qe_meas');
	$skip=array(
				'qe_meas_id',
				'qe_id',
				'eqe_max',
				'eqe_wavelength_max',
				'iqe_max',
				'iqe_wavelength_max',
				'pl_x',
				'pl_y',
			 	'pl_lambda_max_avg',
			 	'pl_lambda_max_std',
			 	'pl_power_max_avg',
			 	'pl_power_max_std',
			 	'pl_number_of_points',
			 	'pl_id',
			 	'jsc_am15d_ideal',
			 	'jsc_am15d_relative'
			);
	
	$query = "INSERT INTO epi_qe_meas SET  qe_id = $qe_id, ";
	foreach ($dbkeys as $k) {
		if (!in_array($k,$skip)) {
			if (empty($header[$k])) { continue; }
			$q[]=" $k = '".$header[$k]."'";
		}
	}
	$query .= implode(' , ',$q);
	$result = $this->local_insert($query);
	
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	$qe_meas_id = $this->last_insert();
	if($qe_meas_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0,'data_id'=>$qe_meas_id);
}

function qe_meas_values_columns(&$handle) {
	rewind($handle);
	$string = array();
	
	while(!feof($handle) && !stristr($string[1],'Cal')) {	
		$string = fgetcsv($handle);
	//	$this->echo_r($string);
	}
	if (count($string) == 0 ) {
		return NULL;
	}
	for($i=0;$i<count($string); $i++) {
		$tmp = explode('(',$string[$i]);
		$columns[$i]=str_replace(' ','_',trim(str_replace('%','',str_replace('-','',strtolower($tmp[0])))));
	}	
	echo "<br> Measurement Columns ";
	// $this->echo_r($columns);
	return $columns;
}


function new_qe_meas_values($qe_meas_id,$columns,$values) {
	//Wavelength (nm),Cal Factor,Monitor Responsivity (A/W),Cell Current (A),Monitor Lock-in Current (A),EQE,Standard Reflectance %,Reflectance Cal data,Reflectance Lock-In Current,Reflectance of Cell,IQE,Scaled EQE (Estimated IQE)
	if (strlen(trim(implode('',$values))) == 0 ) {
		return array('error'=>0);;
	}
	$query = "insert into epi_qe_meas_values 
					set qe_meas_id = $qe_meas_id,";
					
	for ($i=0; $i< count($columns) ; $i++ ) {
		
		if (stristr($values[$i],'nan') ) {
			$values[$i] = -1;
		}
		
		$q[]=$columns[$i]." = '".$values[$i]."'";
	}
	$query .=implode(',',$q);
	// echo "<br>QE CHECK $query";
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0);
}

function qe_meas_stats($qe_meas_id) {

	$query="select wavelength,eqe from epi_qe_meas_values where qe_meas_id = $qe_meas_id order by eqe desc limit 1";
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$query="update epi_qe_meas set eqe_max = '".$result[0]['eqe']."',
									eqe_wavelength_max = '".$result[0]['wavelength']."'
									where qe_meas_id = $qe_meas_id";
	// echo "<br> $query";
	$result = $this->local_insert($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	/*
	$query="select wavelength,scaled_eqe from epi_qe_meas_values where qe_meas_id = $qe_meas_id order by scaled_eqe desc limit 1";
	
	echo "<br> $query";
	
	
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$query="update epi_qe_meas set scaled_eqe_max = '".$result[0]['scaled_eqe']."',
									scaled_eqe_wavelength_max = '".$result[0]['wavelength']."'
									where qe_meas_id = $qe_meas_id";
	echo "<br> $query";
	$result = $this->local_insert($query);

	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	*/
	
	return array('error'=>0);
}

function qe_stats($qe_id)
{
	$query=" SELECT AVG(jsc_am0) AS jsc_am0_avg,
			AVG(jsc_am15g) as jsc_am15g_avg,
			AVG(jsc_am15d) as jsc_am15d_avg,
			MAX(jsc_am0) as jsc_am0_max,
			MAX(jsc_am15g) as jsc_am15g_max,
			MAX(jsc_am15d) as jsc_am15d_max,
			STDDEV_POP(jsc_am0) as jsc_am0_std,
			STDDEV_POP(jsc_am15g) as jsc_am15g_std,
			STDDEV_POP(jsc_am15d) as jsc_am15d_std,
			MEDIAN(jsc_am15d,5) as jsc_am15d_median,	
			MEDIAN(jsc_am15g,5) as jsc_am15g_median,	
			MEDIAN(jsc_am0,5) as jsc_am0_median,
			AVG(jsc_am15d_iqe) as jsc_am15d_iqe_avg,
			MEDIAN(jsc_am15d_iqe) as jsc_am15d_iqe_median,
			STDDEV_POP(jsc_am15d_iqe) as jsc_am15d_iqe_std,
			MIN(jsc_am15d_eqe) as jsc_am15d_eqe_min,
			MAX(jsc_am15d_eqe) as jsc_am15d_eqe_max,
			AVG(jsc_am15d_eqe) as jsc_am15d_eqe_avg,
			MEDIAN(jsc_am15d_eqe) as jsc_am15d_eqe_median,
			STDDEV_POP(jsc_am15d_eqe) as jsc_am15d_eqe_std,
			MIN(bandgap_ev) as bandgap_ev_min,
			MAX(bandgap_ev) as bandgap_ev_max,
			AVG(bandgap_ev) as bandgap_ev_avg,
			MEDIAN(bandgap_ev) as bandgap_ev_median,
			STDDEV_POP(bandgap_ev) as bandgap_ev_std
		 FROM epi_qe_meas
		 WHERE qe_id = $qe_id";
					
	// echo "<br> $query";
	
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	$row  =$result[0];
	
	$query = "update epi_qe set ";
	foreach($row as $k=>$v) {
		$q[]=" $k = '$v' ";
	
	}
	$query .= implode(',',$q);
	$query .=" where qe_id =$qe_id";
	
	// echo "<br> $query";

	$result = $this->local_insert($query);

	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0);
}

function qe_date($qe_id) {
	$query = "select start_time from epi_qe_meas where qe_id = $qe_id order by start_time asc limit 1";
	$result = $this->local_query($query);
	$start_time = $result[0]['start_time'];
	$query = "update epi_qe set date = '$start_time' where qe_id = $qe_id";
	$result = $this->local_insert($query);
}

function qe_check_header($header) {
	if ($header['mask_name'] == NULL || $header['cell_name'] == NULL || $header['die_number'] == NULL ) {
		return array('error'=>3,'point'=>'File Read Error : could not extract key parameters');
	}
	return array('error'=>0);
}
 
function qe_get_pl($qe_id) {
 
	$query = "select  m.qe_meas_id,
					  q.wafer_name,
					  q.wafer_id,
					  q.cell_name,
					  d.x_loc,
					  d.y_loc,
					  d.mesa_x,
					  d.mesa_y,
					  m.x,
					  m.y,
					  m.jsc_am15d,
					  m.starting_wavelength_nm 
					  from epi_qe_meas m left join (epi_qe q, epi_device d) on( q.cell_name = d.device_name and q.mask_name = d.device_mask and m.qe_id = q.qe_id) where m.qe_id = $qe_id";
	
		$query = "select  m.qe_meas_id,
					  q.wafer_name,
					  q.wafer_id,
					  q.cell_name,
					  d.x_loc,
					  d.y_loc,
					  d.mesa_x,
					  d.mesa_y,
					  m.x,
					  m.y,
					  m.jsc_am15d,
					  m.starting_wavelength_nm 
					  from epi_qe_meas m, epi_qe q, epi_device d where q.cell_name = d.device_name and q.mask_name = d.device_mask and m.qe_id = q.qe_id and m.qe_id = $qe_id";
	
	
	// echo "<br> PL qe $query";;
	$result = $this->local_query($query);
	// $this->echo_r($result);
	
	foreach ($result as $r) {
		echo "<br> Updating PL values in the QE measurement of ".$r['wafer_name'];
		$qe_meas_id = $r['qe_meas_id'];
		$wafer_name  =$r['wafer_name'];
		$wafer_id = $r['wafer_id'];
		$die = $r['cell_name'];
		$xloc = $r['x_loc'];
		$yloc = $r['y_loc'];
		$mx = $r['mesa_x'];
		$my = $r['mesa_y'];
		$jsc_am15d = $r['jsc_am15d'];
		$x = $r['x'];
		$y = $r['y'];
		$lambda_start = $r['starting_wavelength_nm'];
		$qex = $x+$xloc+$mx/2;
		$qey = $y+$yloc;
		$x = $x/1000;
		$y = $y/1000;
		$qex = $qex/1000;
		$qey = $qey/1000;
		
		// echo "<br> Wafer ID ".$wafer_id;
		
		$query2 = "select * from epi_pl where wafer_id = $wafer_id and wafer_name = '$wafer_name' and laser=980";
		
		$result2 = $this->local_query($query2);
		
		if ($result2 == NULL ){
			echo "<br> NO PL DATA FOUND";
			continue;
		}
		
		$pl_id = $result2[0]['pl_id'];
		$query3 = "select *, abs(x-$qex) as dx , abs(y-$qey) as dy from epi_pl_meas where pl_id = 263 order by abs((x-$qex)),abs((y-$qey)) asc limit 1";
		
		$query3 = "select *, abs(x-$qex) as dx , abs(y-$qey) as dy from epi_pl_meas where pl_id = 263 and abs((x-$qex)) < ".($mx/2000)." and abs((y-$qey)) < ".($my/2000)." ";
		
		$query3 = "select  
						   avg(pl_lambda_max) as pl_lambda_max_avg, 
						  avg(pl_power_max) as pl_power_max_avg, 
						  stddev_pop(pl_lambda_max) as pl_lambda_max_std, 
						  stddev_pop(pl_power_max) as pl_power_max_std,
						  count(*) as pl_number_of_points
						  
				from epi_pl_meas where pl_id = $pl_id and abs((x-$qex)) < ".($mx/2000)." and abs((y-$qey)) < ".($my/2000)." ";
		
		
		// echo "<br>".$query3;
	
		$result3 = $this->local_query($query3);
	
		
		if ($result3 == NULL ){
		
			echo "<br> COULD NOT GET STATS";
			continue;
		}
	
		$lambda_peak = $result3[0]['pl_lambda_max_avg'];
	
		if ($lambda_peak == NULL ) {
			$query3 = "select  
						   avg(pl_lambda_max) as pl_lambda_max_avg, 
						  avg(pl_power_max) as pl_power_max_avg, 
						  stddev_pop(pl_lambda_max) as pl_lambda_max_std, 
						  stddev_pop(pl_power_max) as pl_power_max_std,
						  count(*) as pl_number_of_points
						  
				from epi_pl_meas where pl_id = $pl_id ";
		
		
			// echo "<br>".$query3;
	
			$result3 = $this->local_query($query3);
		
		
			if ($result3 == NULL ){
		
				echo "<br> COULD NOT GET STATS";
				continue;
			}
	
			$lambda_peak = $result3[0]['pl_lambda_max_avg'];
		
			if ($lambda_peak == NULL ) {
				return array('error'=>0);;
			}
		
		}
	
		/*$xpl = $result3[0]['x'];
		$ypl = $result3[0]['y'];
		$dx = $result3[0]['dx'];
		$dy = $result3[0]['dy'];
		
		echo "<br>QE die location at $x $y device loc $xloc $yloc wh $mx $my QE meas at $qex $qey PL meas at $xpl $ypl difference $dx $dy";
		*/
		
		// $this->echo_r($result3);
		
		$query4="update epi_qe_meas set ";
		foreach ($result3[0] as $k=>$v) {
			$q[] = " $k = '$v' ";
		}
		$q[]=" pl_id = $pl_id ";
		$q[]=" pl_x = '".($qex)."'";
		$q[]=" pl_y = '".($qey)."'";
		$query4.=implode(',',$q);
		$query4.=" where qe_meas_id = $qe_meas_id";
	
		// echo "<br> PL UPDATE QUERY $query4";
	
		$result4 = $this->local_insert($query4);
		if ($result4 == NULL ) {
			echo "<br> ERROR COULD NOT UPDATE PL STATS";
		}
		
		// echo "<br> PL STATS UPDATED SUCCESSFULLY ";
		
		// now if we've updated the stats, we can calculate the expected jsc for this point 
		$query5 =  "SELECT sum( 1000*am15d1kW_current_density_cm2_nm * d_lambda_nm ) AS jsc_ideal
					FROM `epi_solar_spectra`
					WHERE lambda_nm > $lambda_start
				AND lambda_nm < $lambda_peak ";
		
		// echo "<br> QE NORMALIZATION QUERY: $query5";
		
		$result5 = $this->local_query($query5);
		
		if ($result5 == NULL ) {
			echo "<br> COULD NOT GET IDEAL JSC";
			continue;
		}
		
		$jsc_ideal = $result5[0]['jsc_ideal'];
		
		// echo "<br> FOUND IDEAL JSC = $jsc_ideal ACTUAL jsc = $jsc_am15d SCALED Jsc = ".($jsc_am15d/$jsc_ideal);
		
		if ($jsc_ideal != NULL ) {
			$query6 = "update epi_qe_meas set 
					jsc_am15d_ideal = '$jsc_ideal', 
					jsc_am15d_relative = '".($jsc_am15d/$jsc_ideal)."', 
					jsc_am15d_ideal_lambda_start = '$lambda_start',
					jsc_am15d_ideal_lambda_stop  = '$lambda_peak'
					where qe_meas_id = $qe_meas_id";
			
			$result6 = $this->local_insert($query6);
			if ($result6 == NULL ){
				echo "<br> ERROR: could not update the relative and scaled JSC values";
			}
		}
		
	
	}
 
 
 }
 
 
 function ideal_jsc($lambda_start,$lambda_peak){
 		
 		$query =  "SELECT sum( 1000*am15d1kW_current_density_cm2_nm * d_lambda_nm ) AS jsc_ideal
					FROM `epi_solar_spectra`
					WHERE lambda_nm > $lambda_start
				AND lambda_nm < $lambda_peak ";
				
		$result = $this->local_query($query);
		
		if ($result == NULL ) {
			return 0;
		}
 		return $result[0]['jsc_ideal'];
 }
 
 function get_qe_mask($qe_id) {
 
 	$query = "select mask_name from epi_qe where qe_id = $qe_id";
 
 	$result = $this->local_query($query);
 	
 	if ($result == NULL ) {
 	
 		return NULL;
 	
 	}
 	
 	return $result[0]['mask_name'];
 
 }
 
 function is_qtqe($file) {
 	return stristr($file,'QTQE');
 }
 
 
 
function get_qe_xy($qe_meas_id) {
	
	$query = "select wavelength,scaled_eqe from epi_qe_meas_values where qe_meas_id = $qe_meas_id order by wavelength asc";
	
	$result = $this->local_query($query);

	$ret = array();
	foreach($result as $r) {
		$ret[] = array('wavelength'=>$r['wavelength'],'eqe'=>$r['scaled_eqe']);
		
	}
	
	return $ret;
}


function get_qe_bg($qe_dat,$max_eqe,$thresh) {
	$ret = array();
	$target = $max_eqe*$thresh;
	//echo "<br> target $target threshold $thresh max eqe $max_eqe";
	$eqe_bg = 0;
	$lambda_bg = 0;
	for ($i=count($qe_dat);$i >= 0;$i-- ) {
		if ($qe_dat[$i]['eqe'] > $target){
			if ($i < count($qe_dat)) {
			$m=($qe_dat[$i+1]['eqe']-$qe_dat[$i]['eqe'])/($qe_dat[$i+1]['wavelength']-$qe_dat[$i]['wavelength']);
			$x=(target - $qe_dat[$i]['eqe'])/$m+$qe_dat[$i]['wavelength'];
			$eqe_bg  = $target;
			$lambda_bg =$x;
			
			//echo "<br> m=".$m." x= ".$x." lam = ".$qe_dat[$i]['wavelength']." eqe = ".$qe_dat[$i]['eqe']." target = ".$target;
			
			} else {
			   $eqe_bg  = $qe_dat[$i]['eqe'];
		       $lambda_bg = $qe_dat[$i]['wavelength'];
			
			}
			
			  // $eqe_bg  = $qe_dat[$i]['eqe'];
		      // $lambda_bg = $qe_dat[$i]['wavelength'];
			
			break;
		}
	
	}

	return array('eqe_bg'=>$eqe_bg,'lambda_bg'=>$lambda_bg);
}

function get_wafer_thinned($wid)
{
	$query = "SELECT thinned FROM epi_wafer WHERE wafer_id = $wid";
	$result = $this->local_query($query);
	return $result[0]['thinned'];
}

function set_wafer_thinned($wid, $thinned)
{
	$query = "UPDATE epi_wafer SET thinned = $thinned WHERE wafer_id = $wid";
	$result = $this->local_insert($query);
}

function get_wafer_diced($wid)
{
	$query = "SELECT diced2 FROM epi_wafer WHERE wafer_id = $wid";
	$result = $this->local_query($query);
	return $result[0]['diced2'];
}

function set_wafer_diced($wid, $diced)
{
	$query = "UPDATE epi_wafer SET diced2 = $diced WHERE wafer_id = $wid";
	$result = $this->local_insert($query);
}

// EPI Measurements TAG functions
function update_qe_meas_value($qe_meas_id)
{
	$query = "SELECT wafer_id, junction FROM epi_qe_meas WHERE qe_meas_id = $qe_meas_id ";
	//echo "update_qe_meas_value: $query<br>";
	$result = $this->local_query($query);
	//$this->echo_r($result);

	$wafer_id = $result[0]['wafer_id'];
	$junction = $result[0]['junction'];

	$query  = "SELECT truncate(median(jsc_am15d_eqe),3) AS new_median_jsc_am15d_eqe, ";
	$query .= " truncate(median(jsc_am15d_iqe),3) AS new_median_jsc_am15d_iqe ";
	$query .= "FROM epi_qe_meas LEFT JOIN epi_tag_meas ON qe_meas_id = meas_id ";
	$query .= "WHERE wafer_id = $wafer_id ";
	$query .= " AND junction LIKE '$junction' ";
	$query .= " AND tag_id IS NULL";

	//echo "update_qe_meas_value: $query<br>";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	switch ($junction) {
		case "J1": $jsc_eqe_variable = "j1_jsc_am15d_eqe_avg"; $jsc_iqe_variable = "j1_jsc_am15d_iqe_avg"; break;
		case "J2": $jsc_eqe_variable = "j2_jsc_am15d_eqe_avg"; $jsc_iqe_variable = "j2_jsc_am15d_iqe_avg"; break;
		case "J3": $jsc_eqe_variable = "j3_jsc_am15d_eqe_avg"; $jsc_iqe_variable = "j3_jsc_am15d_iqe_avg"; break;
		default: return NULL;
	}

	$query  = "UPDATE epi_wafer_summary_table ";
	if (!empty($result[0]['new_median_jsc_am15d_eqe'])) {
		$query .= " SET $jsc_eqe_variable = ".$result[0]['new_median_jsc_am15d_eqe'];
		$query .= " , $jsc_iqe_variable = ".$result[0]['new_median_jsc_am15d_iqe'];
	} 
	else {
		$query .= " SET $jsc_eqe_variable = NULL ";
		$query .= " , $jsc_iqe_variable = NULL ";
	}
	$query .= " WHERE wafer_id IN (SELECT wafer_id FROM epi_qe_meas WHERE qe_meas_id = $qe_meas_id) ";
	//echo "update_qe_meas_value: $query<br>";
	$result = $this->local_insert($query);
	//$this->echo_r($result);

	return $result;
}

function get_tag_meas_count($qe_meas_id)
{
	$query = "SELECT count(*) AS tag_count FROM epi_tag_meas WHERE meas_id = $qe_meas_id AND meas_table LIKE 'epi_qe_meas'";
	//$query = "SELECT count(*) AS tag_count FROM epi_tag_meas LEFT JOIN epi_qe_meas ON meas_id = qe_mead_id WHERE meas_id = $qe_meas_id";
	$result = $this->local_query($query);
	return $result[0]['tag_count'];
}

function restore_qe_meas($qe_meas_id, $tag_id)
{
	//$query = "DELETE FROM epi_tag_meas WHERE tag_id = $tag_id AND meas_id = $qe_meas_id AND meas_table LIKE 'epi_qe_meas'";
	$query = "DELETE FROM epi_tag_meas WHERE meas_id = $qe_meas_id AND meas_table LIKE 'epi_qe_meas'";
	$result = $this->local_insert($query);
}

function tag_qe_meas($qe_meas_id, $tag_id)
{
	$query  = "DELETE FROM epi_tag_meas ";
	$query .= "WHERE meas_id = '$qe_meas_id' ";
	$query .= "  AND meas_table LIKE 'epi_qe_meas' ";
	$query .= "  AND junction LIKE (SELECT junction FROM epi_qe_meas WHERE qe_meas_id = $qe_meas_id)";
	$result = $this->local_insert($query);

	$query = "INSERT INTO epi_tag_meas (tag_id, meas_id, meas_table) VALUES ('$tag_id', '$qe_meas_id', 'epi_qe_meas')";
	$result = $this->local_insert($query);
}

function get_wafer_qe_meas($wafer_id, $junction)
{
	$query  = "SELECT wafer_name as Wafer, jsc_am15d_eqe AS 'JSC EQE', junction as Junction, qe_meas_id AS 'QE Meas ID', tag_id, name AS tag_name ";
	$query .= "FROM epi_qe_meas LEFT OUTER JOIN epi_tag_meas ON (qe_meas_id = meas_id AND meas_table LIKE 'epi_qe_meas') LEFT OUTER JOIN epi_tag ON tag_id = id ";
	$query .= "WHERE wafer_id = $wafer_id AND junction LIKE '$junction' ";
	$query .= "ORDER BY qe_meas_id DESC ";
	$result = $this->local_query($query);
	return $result;
}

function get_qe_meas($qe_meas_id)
{
	$query  = "SELECT wafer_name as Wafer, jsc_am15d_eqe AS 'JSC EQE', junction as Junction, qe_meas_id AS 'QE Meas ID' ";
	$query .= "FROM epi_qe_meas ";
	$query .= "WHERE qe_meas_id = $qe_meas_id ";
	$query .= "ORDER BY qe_meas_id DESC ";
	$result = $this->local_query($query);
	return $result[0];
}

function get_meas_tags($table)
{
	$query  = "SELECT m.meas_id, m.tag_id, t.name AS tag_name ";
	$query .= "FROM epi_tag_meas m INNER JOIN epi_tag t ON (m.tag_id = t.id AND m.meas_table LIKE 'epi_qe_meas')";
	//$query .= "WHERE m.meas_table LIKE 'epi_qe_meas' ";
	$query .= "ORDER BY m.tag_id DESC ";
	$result = $this->local_query($query);
	return $result;
}

function is_valid_cell($wafer_name, $x_coord, $y_coord)
{
	// get mask name
	$query  = "SELECT m.device_mask ";
	$query .= "FROM epi_wafer w INNER JOIN epi_device_mask m ON (w.device_mask_id = m.device_mask_id) ";
	$query .= "INNER JOIN epi_device d ON d.device_mask_id = m.device_mask_id ";
	$query .= "WHERE w.wafer_name LIKE '$wafer_name' AND d.grid_spacing > 0 AND d.x_loc = $x_coord AND d.y_loc = $y_coord";
	//echo "<br>$query";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	if ($result != NULL) {
		return TRUE;
	}
	return FALSE;
}

function get_flash_session_dates($wafer_id)
{
        $query  = "SELECT session_date, count(session_date) AS meas_count, chuck_temperature_degrees_c meas_temperature ";
        $query .= "FROM epi_flash_meas ";
        $query .= "WHERE wafer_id = $wafer_id ";
        $query .= "GROUP BY session_date ";
        $query .= "ORDER BY session_date";
        $result = $this->local_query($query);
        foreach($result as $k=>$v) {
                $sessions[] = $v['session_date'] ." | ".sprintf("%3.1f", $v['meas_temperature'])." &deg;C | ".sprintf("%03d", $v['meas_count']);
        }
        return $sessions;
}

function old_get_flash_session_dates($wafer_id)
{
	$query  = "SELECT session_date, count(session_date) AS meas_count ";
	$query .= "FROM epi_flash_meas ";
	$query .= "WHERE wafer_id = $wafer_id ";
	$query .= "GROUP BY session_date ";
	$query .= "ORDER BY session_date";
	$result = $this->local_query($query);
	foreach($result as $k=>$v) {
		$sessions[] = $v['session_date'] .", (". sprintf("%03d", $v['meas_count']) .")";
	}
	return $sessions;
}

function get_shippable_cells_v2($wafer_id, $meas_min = 0, $meas_max = 100, $session_date)
{
	$query  = "SELECT S1.wafer_id, S2.wafer_name, S1.cells, S2.tagged, S4.shippable, S3.shipped ";
	$query .= "FROM (";

	$query .= "  SELECT c.wafer_id, count(distinct c.id) AS 'cells' ";
	$query .= "  FROM epi_cell c ";
	$query .= "  WHERE wafer_id = $wafer_id ";
	$query .= "  GROUP BY c.wafer_id ";
	$query .= "  ORDER BY wafer_id DESC) S1 ";

	$query .= " INNER JOIN (";
	$query .= "  SELECT w.wafer_name, c.wafer_id, count(DISTINCT c.id) AS 'tagged' ";
	$query .= "  FROM epi_wafer w LEFT JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "   INNER JOIN epi_tag_meas m ON (c.id = m.meas_id AND m.meas_table LIKE 'epi_cell' AND tag_id > 0) ";
	$query .= "  WHERE c.wafer_id = $wafer_id ";
	$query .= "  GROUP BY c.wafer_id ";
	$query .= "  ORDER BY c.wafer_id DESC) S2 ";
	$query .= " ON (S1.wafer_id = S2.wafer_id) ";

	$query .= " INNER JOIN (";
	$query .= "  SELECT c.wafer_id, count(DISTINCT c.id) AS 'shippable' ";
	$query .= "  FROM epi_cell c INNER JOIN epi_flash_meas f ON (f.wafer_id = c.wafer_id AND (f.x_coord = c.x_coord) AND (f.y_coord = c.y_coord) ";
	$query .= "     AND ((100 * f.efficiency) >= $meas_min) ";
	$query .= "     AND ((100 * f.efficiency) <= $meas_max)) ";
	$query .= "     AND (f.session_date LIKE '$session_date') ";
	$query .= "  WHERE c.wafer_id = $wafer_id AND c.id NOT IN (SELECT meas_id FROM epi_tag_meas WHERE meas_table LIKE 'epi_cell' AND tag_id > 0) ";
	$query .= "  GROUP BY c.wafer_id ) S4 ";
	$query .= " ON (S2.wafer_id = S4.wafer_id) ";

	$query .= " INNER JOIN (";
	$query .= "  SELECT c2.wafer_id, count(c2.shipment_id) AS 'shipped' ";
	$query .= "  FROM epi_cell c2 ";
	$query .= "  WHERE c2.wafer_id = $wafer_id AND c2.shipment_id IS NOT NULL ";
	$query .= "  GROUP BY c2.wafer_id ";
	$query .= "  ORDER BY c2.wafer_id DESC) S3 ";
	$query .= " ON (S3.wafer_id = S2.wafer_id) ";

	//echo $query;

	$result = $this->local_query($query);
	$result[0]['session'] = $session_date;
	$result[0]['sessions'] = $this->get_flash_session_dates($wafer_id);
	//$this->echo_r($result);

	return $result[0];
}

function cells_tagged()
{
	$query = "SELECT wafer_id, count(distinct meas_id) AS tagged ";
	$query .= "FROM epi_cell INNER JOIN epi_tag_meas ON (id = meas_id AND meas_table LIKE 'epi_cell' AND tag_id > 0)";
	$query .= "GROUP BY wafer_id";
	$result = $this->local_query($query);
	//$this->echo_r($result);

	$tagged = array();
	foreach ($result as $k=>$v) {
		$tagged[$v['wafer_id']] = $v['tagged'];
	}
	return $tagged;
}

function cells_shipped()
{
	$query = "SELECT wafer_id, count(id) AS shipped FROM epi_cell WHERE shipment_id IS NOT NULL GROUP BY wafer_id";
	$result = $this->local_query($query);
	$shipped = array();
	foreach ($result as $k=>$v) {
		$shipped[$v['wafer_id']] = $v['shipped'];
	}
	return $shipped;
}

function cell_inventory()
{
	$query .= "SELECT wafer_id, count(*) AS cells FROM epi_cell GROUP BY wafer_id";

	$result = $this->local_query($query);

	$res = array();
	foreach ($result as $k=>$v) {
		$res[$v['wafer_id']] = $v['cells'];
	}
	return $res;
}

function get_shippable_cells_v4($wafer_id, $meas_type, $min = 0, $max = 100, $session_date)
{
        $query = "SELECT m.session_date, m.wafer_name, c.*, m.efficiency, t.defects ";
        $query .= "FROM epi_flash_meas m ";
        $query .= "INNER JOIN epi_cell c ON (m.wafer_id = c.wafer_id and m.x_coord = c.x_coord and m.y_coord = c.y_coord) ";
        $query .= "LEFT JOIN (SELECT meas_id, count(*) defects FROM epi_tag_meas GROUP BY meas_id) t ON t.meas_id = c.id ";
        $query .= "WHERE m.wafer_id = $wafer_id AND m.session_date = '$session_date'";
        $result = $this->local_query($query);
        $w = array('wafer_id'=>$wafer_id, 'meas_type'=>$meas_type, 'session_date'=>$session_date, 'min'=>$min, 'max'=>$max, 'total'=>0, 'shipped'=>0, 'rejected'=>0, 'above'=>0, 'below'=>0, 'within'=>0);
        //$this->echo_r($result);
        foreach ($result as $k=>$v) {
                if ($v['shipment_id']) {
                        $w['shipped']++;
                }
                else if ($v['defects']) {
                        $w['rejected']++;
                }
                else {
                        if (($v['efficiency']*100) < $min) {
                                $w['below']++;
                        }
                        else if (($v['efficiency']*100) > $max) {
                                $w['above']++;
                        }
                        else {
                                $w['within']++;
                        }
                }
                $w['total']++;
        }
        //$this->echo_r($w);
        return $w;

}

function get_shippable_cells_v3($wid, $meas_type, $min = 0, $max = 100, $session_date)
{
	//echo "<br>meas: $meas_type; min= $min, max=$max";
	$sessions = array();
	$query  = "SELECT wafer_id, session_date, count(session_date) AS meas_count ";
	$query .= "FROM epi_flash_meas ";
	$query .= "WHERE wafer_id = $wid ";
	$query .= "GROUP BY session_date ";
	$query .= "ORDER BY wafer_id, session_date";
	$result[$wid] = $this->local_query($query);

	foreach ($result[$wid] as $k=>$v) {
		if ($wid == $v['wafer_id']) {
			$sessions[$wid][] = $v['session_date']; 
		}
	}
	
	$query  = "  SELECT w.wafer_name, c.wafer_id, count(distinct c.id) AS 'cells' ";
	$query .= "  FROM epi_wafer w INNER JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "  WHERE c.wafer_id = $wid ";
	$query .= "  GROUP BY c.wafer_id ";
	$query .= "  ORDER BY c.wafer_id DESC ";
	$result = $this->local_query($query);
	$cells = array();
	$name = array();
	foreach ($result as $k=>$v) {
		$cells[$v['wafer_id']] = $v['cells'];
		$name[$v['wafer_id']] = $v['wafer_name'];
	}

	$tagged = array();
	$query  = "SELECT wafer_id, count(distinct meas_id) AS tagged ";
	$query .= "FROM epi_cell INNER JOIN epi_tag_meas ON (id = meas_id AND meas_table LIKE 'epi_cell' AND tag_id > 0) ";
	$query .= "WHERE wafer_id = $wid ";
	$query .= "GROUP BY wafer_id ";
	$query .= "ORDER BY wafer_id DESC ";
	$result = $this->local_query($query);
	foreach ($result as $k=>$v) {
		$tagged[$v['wafer_id']] = $v['tagged'];
	}

	$rejected = array();
	$query  = "SELECT c.wafer_id, count(distinct c.id) AS rejected ";
	$query .= "FROM epi_cell c ";
	$query .= "INNER JOIN epi_flash_meas m ON (c.wafer_id = m.wafer_id AND c.x_coord = m.x_coord AND c.y_coord = m.y_coord) ";
	$query .= "WHERE c.wafer_id = $wid ";
	$query .= "  AND (((100.0 * m.efficiency) < $min) OR ((100.0 * m.efficiency) > $max)) ";
	$query .= "  AND m.session_date = '$session_date' ";
	$query .= "  AND c.shipment_id IS NULL ";
	$query .= "  AND c.id NOT IN (SELECT distinct meas_id FROM epi_tag_meas WHERE meas_table LIKE 'epi_cell' AND tag_id > 0) ";
	$query .= "GROUP BY c.wafer_id ";
	$query .= "ORDER BY c.wafer_id DESC ";
	//echo "<p>$query</p>";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	foreach ($result as $k=>$v) {
		//$this->echo_r($v);
		$rejected[$v['wafer_id']] = $v['rejected'];
	}

	$query = "SELECT wafer_id, count(id) AS shipped ";
	$query .= "FROM epi_cell ";
	$query .= "WHERE wafer_id = $wid ";
	$query .= "AND shipment_id IS NOT NULL ";
	$query .= "GROUP BY wafer_id ";
	$query .= "ORDER BY wafer_id DESC ";
	$result = $this->local_query($query);
	$shipped = array();
	foreach ($result as $k=>$v) {
		$shipped[$v['wafer_id']] = $v['shipped'];
	}

	$shippable = $cells[$wid]-$tagged[$wid]-$rejected[$wid]-$shipped[$wid];
	$inventory[] =	array(
			'wafer_id'=>$wid,
			'wafer_name'=>$name[$wid],
			'cells'=>$cells[$wid],
			'tagged'=>$tagged[$wid],
			'shipped'=>$shipped[$wid],
			'shippable'=>$shippable,
			'session'=>$sessions[$wid][0]
			);

$this->echo_r($inventory);
	return $inventory[0];
}

function get_shippable_cells($wafer_ids)
{
	$wafers = explode(",", $wafer_ids);

	$sessions = array();
	foreach ($wafers as $k=>$wid) {
		$query  = "SELECT wafer_id, session_date, count(session_date) AS meas_count, min(efficiency) as min_efficiency, max(efficiency) as max_efficiency ";
		$query .= "FROM epi_flash_meas ";
		$query .= "WHERE wafer_id = $wid ";
		//$query .= "GROUP BY session_date ";
		$query .= "ORDER BY session_date";
		//echo "<br>$query";
		$result[$wid] = $this->local_query($query);
		//$this->echo_r($result[$wid]);

		foreach ($result[$wid] as $k=>$v) {
			if ($wid == $v['wafer_id']) {
				//$sessions[$wid][] = $v['session_date'].", ". $v['meas_count']; 
				$sessions[$wid]['min'] = 0;//sprintf("%.2f", 100*$v['min_efficiency']); 
				$sessions[$wid]['max'] = 100;//sprintf("%.2f", 100*$v['max_efficiency']); 
				$sessions[$wid][] = $v['session_date']; 
			}
		}
	}
	//$this->echo_r($result);
	//$this->echo_r($sessions);
	
	$query  = "  SELECT w.wafer_name, c.wafer_id, count(distinct c.id) AS 'cells' ";
	$query .= "  FROM epi_wafer w INNER JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "  WHERE c.wafer_id IN ($wafer_ids) ";
	$query .= "  GROUP BY c.wafer_id ";
	$query .= "  ORDER BY c.wafer_id DESC ";
	//echo "<br>cells:";
	$result = $this->local_query($query);
	$cells = array();
	$name = array();
	foreach ($result as $k=>$v) {
		//$cells[$v['wafer_id']] = array($v['wafer_name'], $v['cells']);
		$cells[$v['wafer_id']] = $v['cells'];
		$name[$v['wafer_id']] = $v['wafer_name'];
	}
	//$this->echo_r($cells);

	$query  = "SELECT wafer_id, count(distinct meas_id) AS tagged ";
	$query .= "FROM epi_cell INNER JOIN epi_tag_meas ON (id = meas_id AND meas_table LIKE 'epi_cell' AND tag_id > 0) ";
	$query .= "WHERE wafer_id IN ($wafer_ids) ";
	$query .= "GROUP BY wafer_id ";
	$query .= "ORDER BY wafer_id DESC ";
	//echo "<br>tagged:";
	$result = $this->local_query($query);
	$tagged = array();
	foreach ($result as $k=>$v) {
		$tagged[$v['wafer_id']] = $v['tagged'];
	}
	//$this->echo_r($tagged);

	$query = "SELECT wafer_id, count(id) AS shipped ";
	$query .= "FROM epi_cell ";
	$query .= "WHERE wafer_id IN ($wafer_ids) ";
	$query .= "AND shipment_id IS NOT NULL ";
	$query .= "GROUP BY wafer_id ";
	$query .= "ORDER BY wafer_id DESC ";
	//echo "<br>shipped:";
	$result = $this->local_query($query);
	$shipped = array();
	foreach ($result as $k=>$v) {
		$shipped[$v['wafer_id']] = $v['shipped'];
	}
	//$this->echo_r($shipped);

	foreach ($wafers as $wid) {
		$shippable = $cells[$wid]-$tagged[$wid]-$shipped[$wid];
		$inventory[] =	array(
					'wafer_id'=>$wid,
					'wafer_name'=>$name[$wid],
					'cells'=>$cells[$wid],
					'tagged'=>$tagged[$wid],
					'shipped'=>$shipped[$wid],
					'shippable'=>$shippable,
					'session'=>$sessions[$wid][0],
					'min'=>$sessions[$wid]['min'],
					'max'=>$sessions[$wid]['max']
				);
	}
	//$this->echo_r($inventory);

	return $inventory;
}

function get_wafer_shippable_cells_v2($wafers)
{
	//echo "<br>Shippable wafers"; $this->echo_r($wafers);
	$results = array();
	foreach ($wafers as $k=>$v) {
		$wafer_id = $v['wafer_id'];
		$session_date = trim($v['session']);
		$min_efficiency = 0.01 * trim($v['min']);
		$max_efficiency = 0.01 * trim($v['max']);
		//echo "<br>$k: $v";
		$query  = "SELECT ";
		$query .= " c.id as cell_id, ";
		$query .= " c.x_coord, c.y_coord, ";
		$query .= " c.wafer_id, ";
		$query .= " f.session_date, ";
		$query .= " f.wafer_name, ";
		$query .= " f.efficiency, ";
		$query .= " f.fill_factor, ";
		$query .= " f.equiv_1_sun_jsc as jsc, ";
		//$query .= " (f.isc_a/f.cell_area_cm_sq/10.0/f.corrected_irradiance_wcm_sq) AS jsc, ";
		$query .= " f.voc_v as voc, ";
		$query .= " f.corrected_irradiance_wcm_sq AS conc, ";
		$query .= " f.isc_a AS isc, ";
		$query .= " f.vmax_v AS vmax, ";
		$query .= " f.imax_a AS imax, ";
		$query .= " f.pmax_w AS pmax, ";
		$query .= " f.cell_area_cm_sq AS area ";
		$query .= "FROM epi_cell c INNER JOIN epi_flash_meas f ON (c.wafer_id = $wafer_id AND c.wafer_id = f.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord AND f.session_date = '$session_date') ";
		$query .= "WHERE c.id NOT IN (SELECT meas_id FROM epi_tag_meas WHERE meas_table LIKE 'epi_cell' AND tag_id > 0) ";
		$query .= " AND c.id IN (SELECT id FROM epi_cell WHERE wafer_id = $wafer_id AND shipment_id IS NULL) ";
		$query .= " AND f.efficiency >= $min_efficiency ";
		$query .= " AND f.efficiency <= $max_efficiency ";
		$query .= "ORDER BY c.wafer_id DESC, c.y_coord, c.x_coord ";
		$result = $this->local_query($query);
		//echo "<p>$query</p>";
		//$this->echo_r($result);
		foreach ($result as $r=>$res) {
			$results[] = $res;
		}
	}
	//$this->echo_r($results);
	
	return $results;
}

function get_wafer_shippable_cells($wafers)
{
	//echo "<br>Shippable cells"; $this->echo_r($wafers);
	$results = array();
	foreach ($wafers as $k=>$v) {
		//echo "<br>$k: $v";
		$query  = "SELECT f.session_date, f.wafer_name, c.wafer_id, c.id as cell_id, c.x_coord, c.y_coord, f.efficiency, f.fill_factor, f.equiv_1_sun_jsc as jsc, f.voc_v as voc, ";
		$query .= "f.corrected_irradiance_wcm_sq AS conc, f.isc_a AS isc, f.vmax_v AS vmax, f.imax_a AS imax, f.pmax_w AS pmax, f.cell_area_cm_sq AS area ";
		$query .= "FROM epi_cell c INNER JOIN epi_flash_meas f ON (c.wafer_id = f.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
		$query .= "WHERE c.wafer_id = ".$v['wafer_id']." AND c.id NOT IN (SELECT meas_id FROM epi_tag_meas WHERE meas_table LIKE 'epi_cell' AND tag_id > 0)";
		$query .= "ORDER BY c.wafer_id DESC, c.y_coord, c.x_coord ";
		$result = $this->local_query($query);
		//echo "<br>$query";
		//$this->echo_r($result);
		foreach ($result as $r=>$res) {
			$results[] = $res;
		}
	}
	//$this->echo_r($results);
	
	return $results;
}

function get_wafer_shippable__cells2($wafers)
{
	$this->echo_r($wafers);
	$wafer_id_list = implode(",", $wafers);
	//echo $wafer_id_list;

	$query  = "SELECT f.wafer_name, c.wafer_id, c.id as cell_id, c.x_coord, c.y_coord, f.efficiency, f.fill_factor, f.equiv_1_sun_jsc as jsc, f.voc_v as voc, ";
	$query  .= "f.corrected_irradiance_wcm_sq AS conc, f.isc_a AS isc, f.vmax_v AS vmax, f.imax_a AS imax, f.pmax_w AS pmax, f.cell_area_cm_sq AS area ";
	$query .= "FROM epi_cell c INNER JOIN epi_flash_meas f ON (c.wafer_id = f.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
	$query .= "WHERE c.wafer_id IN ($wafer_id_list) AND c.id NOT IN (SELECT meas_id FROM epi_tag_meas WHERE meas_table LIKE 'epi_cell' AND tag_id > 0)";
	//$query .= "ORDER BY c.wafer_id DESC, c.y_coord, c.x_coord ";
	$result = $this->local_query($query);
	//echo "<br>$query";
	$this->echo_r($result);

	return $result;
}

function get_wafer_summary_data($wafer_id)
{
        $query  = "SELECT f.wafer_id, f.wafer_name, f.session_date, count(f.session_date) as session_date_count, f.mask_name, f.coating, ";
	$query .= "       s.scribe, s.jsc, s.arc_design_name, s.j1_jsc_am15d_eqe_avg, s.j2_jsc_am15d_eqe_avg, s.j1_bandgap_ev ";
        $query .= "FROM epi_flash_meas f INNER JOIN epi_wafer_summary_table s ON f.wafer_id = s.wafer_id ";
        $query .= "WHERE f.wafer_id = $wafer_id ";
        $query .= "GROUP BY f.session_date ";
        $query .= "ORDER BY UNIX_TIMESTAMP(f.session_date) DESC ";
	//echo "<br>$query";
        $result = $this->local_query($query);
	//$this->echo_r($result);
	return $result[0];

}
function get_untagged_cells($wafers)
{
	$this->echo_r($wafers);
	$wafer_id_list = implode(",", $wafers);
	echo $wafer_id_list;

	$query  = "SELECT c.wafer_id, w.wafer_name, count(c.id) as 'untagged' ";
	$query .= "FROM epi_wafer w INNER JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "INNER JOIN epi_tag_meas m ON (c.id = m.meas_id and m.meas_table like 'epi_cell') ";
	$query .= "WHERE c.wafer_id IN ($wafer_id_list) ";
	$query .= "GROUP BY c.wafer_id ";
	$query .= "ORDER BY c.wafer_id DESC";
	$result = $this->local_query($query);
	echo "<br>$query";
	$this->echo_r($result);
}

function get_shipped_cells($wafer_name)
{
	$query = "SELECT c.*, s.status ";
	$query .= "FROM epi_wafer w INNER JOIN epi_cell c ";
	$query .= "  ON (w.wafer_id = c.wafer_id AND w.wafer_name LIKE '$wafer_name' AND c.shipment_id IS NOT NULL) ";
	$query .= " INNER JOIN epi_shipment s ON (s.id = c.shipment_id AND c.shipment_id IS NOT NULL) ";
	$query .= "ORDER BY c.id";
	$result = $this->local_query($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	if ($result != NULL) {
		return $result;
	}
	return NULL;
}

function get_cell_id($wafer_name, $x_coord, $y_coord)
{
	$query  = "SELECT c.id AS id ";
	$query .= "FROM epi_wafer w INNER JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "WHERE w.wafer_name LIKE '$wafer_name' ";
	$query .= "AND c.x_coord = $x_coord ";
	$query .= "AND c.y_coord = $y_coord "; 
	$result = $this->local_query($query);
	if ($result != NULL) {
		return $result[0]['id'];
	}
	return NULL;
}

function set_cell_tag($tag_id, $cell_id, $meas_table)
{
	$query = "UPDATE epi_tag_meas SET tag_id = $tag_id WHERE meas_id = $cell_id AND meas_table LIKE '$meas_table'";
	$result = $this->local_insert($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	return $result;
}

function delete_cell_tags($wafer_name, $x_coord, $y_coord, $meas_table)
{
	$cell_id = $this->get_cell_id($wafer_name, $x_coord, $y_coord);
	if ($cell_id != NULL) {
		$query = "DELETE FROM epi_tag_meas WHERE meas_id = $cell_id AND meas_table LIKE '$meas_table'";
		$result = $this->local_insert($query);
	}
	return $cell_id;
}

function add_cell_tags($wafer_name, $x_coord, $y_coord, $tag_id, $meas_table)
{
	$cell_id = $this->get_cell_id($wafer_name, $x_coord, $y_coord);
	if ($cell_id != NULL) {
		$query = "DELETE FROM epi_tag_meas WHERE tag_id = $tag_id AND meas_id = $cell_id AND meas_table LIKE '$meas_table'";
		$result = $this->local_insert($query);
		return $this->add_cell_tag($tag_id, $cell_id, $meas_table);
	}
	else {
		return $this->add_cell_and_tag($wafer_name, $x_coord, $y_coord, $tag_id, $meas_table);
	}
}

function add_cell_tag($tag_id, $cell_id, $meas_table)
{
	$query  = "INSERT INTO epi_tag_meas SET tag_id = $tag_id , meas_id = $cell_id,  meas_table = '$meas_table'";
	$result = $this->local_insert($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	return $result;
}


function update_shipment_status($shipment_id, $shipment_status)
{
	$query = "UPDATE epi_shipment SET status = '$shipment_status' WHERE id = $shipment_id";
	$result = $this->local_insert($query);
}

function add_shipment_container($name, $description, $matrix, $pocket, $attachment)
{
	//$this->echo_r($_POST);
	$query  = "INSERT INTO epi_shipment_container_type ";
	$query .= "SET ";
	$query .= "  name = '$name', ";
	$query .= "  description = '$description', ";
	$query .= "  rows = ".$matrix['rows'].", ";
	$query .= "  cols = ".$matrix['columns'].", ";
	$query .= "  columns = ".$matrix['columns'].", ";
	$query .= "  pocket_width = ".$pocket['width'].", ";
	$query .= "  pocket_length = ".$pocket['length'].", ";
	$query .= "  pocket_depth = ".$pocket['depth'].", ";
	$query .= "  attachment = '$attachment'";

	$result = $this->local_insert($query);
}

function get_last_shipment_container_sn($customer_id)
{
	$query  = "SELECT c.sn ";
	$query .= "FROM epi_shipment s INNER JOIN epi_shipment_container c ON (s.id = c.shipment_id) ";
	$query .= "WHERE s.customer_id = $customer_id ";
	$query .= "ORDER BY c.sn DESC ";
	$query .= "LIMIT 1";
	//echo "<br>$query";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	return $result[0]['sn'];
}

function get_wafer_shipment($shipment_id, $shipment_type='cells')
{
	$cmap = array (
			"#add8e6", "#ffa07a", "#98fb98", 
			"#d8bfd8", "#f0e68c", "#87cefa", 
			"#ffe4b5", "#e6e6fa", "#9acd32", 
			"#ffb6c1", "#104e8b", "#5d478b", 
			"#8b0a50", "#00688b", "#6e8b3d", 
			"#8b1a1a", "#2f4f4f", "#cd6600", 
			"#8b3a62", "#daa520", "#1874cd", 
			"#8968cd", "#ee1289", "#009acd", 
			"#a2cd5a", "#1874cd", "#8968cd", 
			"#ee1289", "#009acd", "#a2cd5a", 
			"#cd2626", "#698b69", "#8b4500", 
			"#cd6090", "#cd9b1d", "#8b1a1a", 
			"#2f4f4f", "#cd6600", "#8b3a62", 
			"#daa520", "#104e8b", "#5d478b", 
			"#8b0a50", "#00688b", "#6e8b3d",
			"#cd2626", "#698b69", "#8b4500", 
			"#cd6090", "#cd9b1d"
		);

	switch ($shipment_type) {
		case "wafers":
			$query  = "SELECT DISTINCT s.wafer_id, w.wafer_name ";
			$query .= "FROM epi_shipped_wafer s INNER JOIN epi_wafer w ON (s.wafer_id = w.wafer_id) ";
			$query .= "WHERE s.shipment_id = $shipment_id ";
			$query .= "ORDER BY s.wafer_id ASC";
			break;

		case "processed_wafers":
			$query  = "SELECT DISTINCT s.wafer_id, w.wafer_name, w.ingot_number, w.substrate_sn, concat('SJ',sw.customer_id, '-',s.customer_wafer_number) wafer_sn ";
			$query .= "FROM epi_shipped_wafer s ";
			$query .= "INNER JOIN epi_wafer w ON (s.wafer_id = w.wafer_id) ";
			$query .= "INNER JOIN epi_cell c ON (c.wafer_id = w.wafer_id) ";
			$query .= "INNER JOIN epi_shipment sw ON (sw.id = s.shipment_id) ";
			$query .= "WHERE s.shipment_id = $shipment_id ";
			$query .= "ORDER BY s.wafer_id ASC";
			//echo "<p>$query</p>";
			break;

		case "cells":
		default:
			$query  = "SELECT DISTINCT c.wafer_id, w.wafer_name ";
			$query .= "FROM epi_cell c ";
			$query .= "INNER JOIN epi_shipment_container_cell_location l ON (c.id = l.cell_id) ";
			$query .= "INNER JOIN epi_wafer w ON (c.wafer_id = w.wafer_id) ";
			$query .= "WHERE c.shipment_id = $shipment_id ";
			$query .= "ORDER BY l.id ASC";
			break;
	}

	//echo "<br>$query";
	$result = $this->local_query($query);
	foreach ($result as $k=>$v) {
		$color = $cmap[$k];
		$result[$k]['color'] = $color;
	}
	//$this->echo_r($result);

	return $result;
}

function get_wafer_color_shipment($shipment_id)
{
	$cmap = array (
			"#add8e6", "#ffa07a", "#98fb98", 
			"#d8bfd8", "#f0e68c", "#87cefa", 
			"#ffe4b5", "#e6e6fa", "#9acd32", 
			"#ffb6c1", "#104e8b", "#5d478b", 
			"#8b0a50", "#00688b", "#6e8b3d", 
			"#8b1a1a", "#2f4f4f", "#cd6600", 
			"#8b3a62", "#daa520", "#1874cd", 
			"#8968cd", "#ee1289", "#009acd", 
			"#a2cd5a", "#1874cd", "#8968cd", 
			"#ee1289", "#009acd", "#a2cd5a", 
			"#cd2626", "#698b69", "#8b4500", 
			"#cd6090", "#cd9b1d", "#8b1a1a", 
			"#2f4f4f", "#cd6600", "#8b3a62", 
			"#daa520", "#104e8b", "#5d478b", 
			"#8b0a50", "#00688b", "#6e8b3d",
			"#cd2626", "#698b69", "#8b4500", 
			"#cd6090", "#cd9b1d"
		);

	$query  = "SELECT DISTINCT c.wafer_id ";
	$query .= "FROM epi_cell c INNER JOIN epi_shipment_container_cell_location l ON (c.id = l.cell_id) ";
	$query .= "WHERE c.shipment_id = $shipment_id ";
	$query .= "ORDER BY l.id ASC";
	//echo "<br>$query";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	foreach ($result as $k=>$v) {
		$wids[$v['wafer_id']] = $cmap[$k];
		//echo "<br>$k ".$v['wafer_id'];
	}
	//$this->echo_r($wids);
	return $wids;
}

function get_cell_shipment_details($cell_id)
{
	$query  = "SELECT ";
	$query .= "c.id as cell_id, ";
	//$query .= "l.shipment_container_id AS shipment_container_id, ";
	//$query .= "l.cell_sn, ";
	//$query .= "l.cell_id, ";
	//$query .= "l.row, ";
	//$query .= "l.col, ";
	//$query .= "l.cell_sn, ";
	$query .= "c.x_coord, ";
	$query .= "c.y_coord, ";
	$query .= "f.session_date, ";
	$query .= "c.wafer_id, ";
	$query .= "f.wafer_name, ";
	$query .= "f.efficiency, ";
	$query .= "f.fill_factor, ";
	$query .= "f.equiv_1_sun_jsc as jsc, ";
	$query .= "f.voc_v as voc, ";
	$query .= "f.corrected_irradiance_wcm_sq AS concentration, ";
	$query .= "f.isc_a AS isc, ";
	$query .= "f.vmax_v AS vmax, ";
	$query .= "f.imax_a AS imax, ";
	$query .= "f.pmax_w AS pmax, ";
	$query .= "f.cell_area_cm_sq AS area ";
	$query .= "FROM epi_cell c ";
	$query .= "INNER JOIN epi_flash_meas f ON (f.wafer_id = c.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
	$query .= "WHERE c.id = $cell_id ";
	$query .= "LIMIT 1";
	//echo "<br>$query";
	$result = $this->local_query($query);
	//$this->echo_r($result);
	return $result;
}

function get_shipment_details($id, $shipment_type="cells")
{

	if ($shipment_type === "wafers") {
		$query = "SELECT S.date AS 'shipment date', C.name AS 'customer name', P.project_number AS 'project number', U.user_name AS user ";
		$query .= "FROM epi_shipment S ";
		$query .= "INNER JOIN epi_customer C ON (C.id = S.customer_id) ";
		$query .= "INNER JOIN customer_projects P ON (P.id = S.customer_project_id) ";
		$query .= "INNER JOIN epi_user U ON (U.user_id = S.user_id) ";
		$query .= "WHERE S.id = $id";
		$result = $this->local_query($query);
		$shipment['details'] = $result;

		$query  = "SELECT W.wafer_name AS wafer, C.name AS customer, ";
		$query .= "concat(W.ingot_number, '-',W.substrate_sn) AS scribe, ";
		$query .= "concat(T.customer_id, '-',S.customer_wafer_number) AS sn ";
		$query .= "FROM epi_shipped_wafer S ";
		$query .= "INNER JOIN epi_wafer W ON (W.wafer_id = S.wafer_id) ";
		$query .= "INNER JOIN epi_shipment T ON (T.id=$id)";
		$query .= "INNER JOIN epi_customer C ON (T.customer_id=C.id)";
		$query .= "WHERE shipment_id = $id ";
		$query .= "ORDER BY S.customer_wafer_number ASC";
		$result = $this->local_query($query);
		//$this->echo_r($result);
		$shipment['wafers'] = $result;
		return $shipment;
	}

	$query  = "SELECT c.shipment_id, c.id AS shipment_container_id, c.sn, c.quantity, t.rows, t.columns ";
	$query .= "FROM epi_shipment_container c INNER JOIN epi_shipment_container_type t ON (t.id = c.type_id) ";
	$query .= "WHERE c.shipment_id = $id ";
	$query .= "ORDER BY c.id ASC";
	$result = $this->local_query($query);
	//echo "<p>$query</p><hr>";
	//$this->echo_r($result);
	//echo "<hr>";

	$results = array();
	foreach ($result AS $k=>$v) {

		$query  = "SELECT ";
		$query .= "l.id as cell_location_id, ";
		$query .= "l.shipment_container_id AS shipment_container_id, ";
		$query .= "l.cell_sn, ";
		$query .= "l.cell_id, ";
		$query .= "l.row, ";
		$query .= "l.col, ";
		$query .= "l.cell_sn, ";
		$query .= "c.x_coord, ";
		$query .= "c.y_coord, ";
		$query .= "f.session_date, ";
		$query .= "c.wafer_id, ";
		$query .= "f.wafer_name, ";
		$query .= "f.efficiency, ";
		$query .= "f.fill_factor, ";
		$query .= "f.equiv_1_sun_jsc as jsc, ";
		$query .= "f.voc_v as voc, ";
		$query .= "f.corrected_irradiance_wcm_sq AS concentration, ";
		$query .= "f.isc_a AS isc, ";
		$query .= "f.vmax_v AS vmax, ";
		$query .= "f.imax_a AS imax, ";
		$query .= "f.pmax_w AS pmax, ";
		$query .= "f.cell_area_cm_sq AS area ";
		$query .= "FROM epi_shipment_container_cell_location l ";
		$query .= "INNER JOIN epi_cell c ON (c.id = l.cell_id) ";
		$query .= "INNER JOIN epi_shipment_container_cell_location s ON (c.id = s.cell_id) ";
		$query .= "INNER JOIN epi_flash_meas f ";
		$query .= "  ON (f.wafer_id = c.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord AND f.session_date = s.flash_session_date) ";
		$query .= "WHERE l.shipment_container_id = '".$v['shipment_container_id']."' ";
		$query .= "ORDER BY l.shipment_container_id ASC, l.id ASC, l.row ASC, l.col ASC";
		//echo "<br>$query";
		$res = $this->local_query($query);
	//echo "<p>$query</p><hr>";
	//$this->echo_r($res);
	//echo "<hr>";
		$shipment_container_id = $v['shipment_container_id'];
		$results[$shipment_container_id]['sn'] = $v['sn'];
		$results[$shipment_container_id]['count'] = $v['quantity'];
		$results[$shipment_container_id]['rows'] = $v['rows'];
		$results[$shipment_container_id]['cols'] = $v['columns'];
		$results[$shipment_container_id]['cells'] = $res;
	}
	//$this->echo_r($results);
	return $results;
}

function get_cell_shipment_details_2($shipment_id, $wafers, $customer)
{
foreach ($wafers as $w=>$wafer_name) {
	$query  = "SELECT DISTINCT ";
	$query .= "c.id, ";
	$query .= "c.x_coord, ";
	$query .= "c.y_coord, ";
	$query .= "c.wafer_id, ";
	$query .= "w.ingot_number, ";
	$query .= "w.substrate_sn, ";
	$query .= "f.session_date, ";
	$query .= "f.date cell_flash_date, ";
	$query .= "f.wafer_name, ";
	$query .= "f.efficiency, ";
	$query .= "f.fill_factor, ";
	$query .= "f.equiv_1_sun_jsc as jsc, ";
	$query .= "f.voc_v as voc, ";
	$query .= "f.corrected_irradiance_wcm_sq AS concentration, ";
	if($customer=="Solar Systems")
	{
	 $query .= "ROUND(f.pmax_w * 50/f.corrected_irradiance_wcm_sq,5) AS pnorm, ";//take the pmax_w value, and multiply by 50 and divide by corrected_irradiance_wcm2
	}
	$query .= "f.isc_a AS isc, ";
	$query .= "f.vmax_v AS vmax, ";
	$query .= "f.imax_a AS imax, ";
	$query .= "f.pmax_w AS pmax, ";
	$query .= "f.cell_area_cm_sq AS area, ";
	$query .= "f.chuck_temperature_degrees_c temp ";
	$query .= "FROM epi_shipment s, epi_cell c INNER JOIN epi_flash_meas f ON (f.wafer_id = c.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
	//$query .= "INNER JOIN epi_shipped_wafer z ON (z.wafer_id = c.wafer_id) ";
	//$query .= "INNER JOIN epi_shipment s ON (s.id = z.shipment_id) ";
	//$query .= "AND (z.flash_session_date = f.session_date) ";
	$query .= "INNER JOIN epi_shipment_container_cell_location l ON (c.id = l.cell_id) ";
	$query .= "INNER JOIN epi_wafer w ON (w.wafer_id = c.wafer_id AND w.wafer_name LIKE '$wafer_name') ";
	//$query .= "WHERE z.shipment_id = $shipment_id ";
	$query .= "WHERE s.id = $shipment_id AND f.session_date = l.flash_session_date ";
	$query .= "ORDER BY c.y_coord ASC, c.x_coord ASC";
	//echo "<br>$query";
	$result = $this->local_query($query);
	$results = $result;
}
	return $results;
}

function get_bare_wafer_shipment_details($shipment_id, $wafers, $customer)
{
foreach ($wafers as $w=>$wafer_name) {
	$query  = "SELECT DISTINCT ";
	$query .= "c.id, ";
	$query .= "c.x_coord, ";
	$query .= "c.y_coord, ";
	$query .= "c.wafer_id, ";
	$query .= "w.ingot_number, ";
	$query .= "w.substrate_sn, ";
	$query .= "f.session_date, ";
	$query .= "f.date cell_flash_date, ";
	$query .= "f.wafer_name, ";
	$query .= "f.efficiency, ";
	$query .= "f.fill_factor, ";
	$query .= "f.equiv_1_sun_jsc as jsc, ";
	$query .= "f.voc_v as voc, ";
	$query .= "f.corrected_irradiance_wcm_sq AS concentration, ";
	$query .= "f.isc_a AS isc, ";
	$query .= "f.vmax_v AS vmax, ";
	$query .= "f.imax_a AS imax, ";
	$query .= "f.pmax_w AS pmax, ";
	$query .= "f.cell_area_cm_sq AS area, ";
	$query .= "f.chuck_temperature_degrees_c temp, ";
	if($customer=="Solar Systems")
	{
	 $query .= "ROUND(f.pmax_w * 50/f.corrected_irradiance_wcm_sq,5) AS pnorm, ";//take the pmax_w value, and multiply by 50 and divide by corrected_irradiance_wcm2
	}
	$query .= "t.id AS customer_id, ";
	$query .= "t.name AS customer ";
	$query .= "FROM epi_cell c INNER JOIN epi_flash_meas f ON (f.wafer_id = c.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
	$query .= "INNER JOIN epi_shipped_wafer z ON (z.wafer_id = c.wafer_id) ";
	$query .= "INNER JOIN epi_shipment s ON (s.id = z.shipment_id) ";
	$query .= "AND (z.flash_session_date = f.session_date) ";
	$query .= "INNER JOIN epi_customer t ON (t.id = s.customer_id) ";
	$query .= "INNER JOIN epi_wafer w ON (w.wafer_id = c.wafer_id AND w.wafer_name LIKE '$wafer_name') ";
	$query .= "WHERE z.shipment_id = $shipment_id ";
	$query .= "ORDER BY c.y_coord ASC, c.x_coord ASC";
	//echo "<br>$query";
	$result = $this->local_query($query);
	$results = $result;
}
	return $results;
}

function get_processed_wafer_shipment_details($shipment_id, $wafers, $customer)
{
foreach ($wafers as $w=>$wafer_name) {
	$query  = "SELECT DISTINCT ";
	$query .= "c.id, ";
	$query .= "c.x_coord, ";
	$query .= "c.y_coord, ";
	$query .= "c.wafer_id, ";
	$query .= "w.ingot_number, ";
	$query .= "w.substrate_sn, ";
	$query .= "f.session_date, ";
	$query .= "f.date cell_flash_date, ";
	$query .= "f.wafer_name, ";
	$query .= "f.efficiency, ";
	$query .= "f.fill_factor, ";
	$query .= "f.equiv_1_sun_jsc as jsc, ";
	$query .= "f.voc_v as voc, ";
	$query .= "f.corrected_irradiance_wcm_sq AS concentration, ";
	$query .= "f.isc_a AS isc, ";
	$query .= "f.vmax_v AS vmax, ";
	$query .= "f.imax_a AS imax, ";
	$query .= "f.pmax_w AS pmax, ";
	$query .= "f.cell_area_cm_sq AS area, ";
	$query .= "f.chuck_temperature_degrees_c temp, ";
	if($customer=="Solar Systems")
	{
	 $query .= "ROUND(f.pmax_w * 50/f.corrected_irradiance_wcm_sq,5) AS pnorm, ";//take the pmax_w value, and multiply by 50 and divide by corrected_irradiance_wcm2
	}
	$query .= "t.id AS customer_id, ";
	$query .= "t.name AS customer ";
	$query .= "FROM epi_cell c INNER JOIN epi_flash_meas f ON (f.wafer_id = c.wafer_id AND c.x_coord = f.x_coord AND c.y_coord = f.y_coord) ";
	$query .= "INNER JOIN epi_shipment s ON (s.id = c.shipment_id) ";
	$query .= "INNER JOIN epi_shipped_wafer z ON (z.wafer_id = c.wafer_id) AND (z.flash_session_date = f.session_date) ";
	$query .= "INNER JOIN epi_customer t ON (t.id = s.customer_id) ";
	$query .= "INNER JOIN epi_wafer w ON (w.wafer_id = c.wafer_id AND w.wafer_name LIKE '$wafer_name') ";
	$query .= "WHERE c.shipment_id = $shipment_id ";
	$query .= "ORDER BY c.y_coord ASC, c.x_coord ASC";
	//echo "<br>$query";
	$result = $this->local_query($query);
	$results = $result;
}
	return $results;
}

function scrapped_cells($shipment_id)
{
        $query = "SELECT c.shipment_id, c.id cell_id, c.wafer_id, c.x_coord, c.y_coord ";
        $query.= "FROM epi_tag_meas m ";
        $query.= "INNER JOIN epi_cell c ON (m.tag_id=38 AND m.meas_id=c.id) ";
        $query.= "WHERE c.shipment_id=$shipment_id";
        $result = $this->local_query($query);
	$scrapped = array();
	foreach ($result as $k=>$v) {
		$scrapped[] = $v['cell_id']; 
	}
        return $scrapped;
}

function replaced_cells($shipment_id)
{
	$query = "SELECT * FROM epi_cell_replacement WHERE shipment_id = $shipment_id";
	//echo "<br>$query";
	$result = $this->local_query($query);
	$replaced = array();
	foreach ($result as $k=>$v) {
		$replaced[$v['replaced_cell_id']] = $v['replacement_cell_id']; 
	}
	//$this->echo_r($result);
	return $replaced;
}

function replace_cell($shipment_id, $old_cell_id, $new_cell_id)
{
	$query  = "UPDATE epi_cell SET shipment_id = $shipment_id WHERE id = $new_cell_id";
	$result = $this->local_insert($query);

	$query  = "INSERT INTO epi_tag_meas SET tag_id = 38, meas_id = $old_cell_id, meas_table = 'epi_cell'";
	$result = $this->local_insert($query);

	$query  = "INSERT INTO epi_cell_replacement ";
	$query .= "SET shipment_id = $shipment_id, replaced_cell_id = $old_cell_id, replacement_cell_id = $new_cell_id";
	$result = $this->local_insert($query);
}

function wafer_serial_number($wafer_name)
{
        $query = "SELECT * FROM epi_wafer WHERE wafer_name LIKE '$wafer_name'";
        $result = $this->local_query($query);
        $this->echo_r($results);

        $wafer_id = $result[0]['wafer_id'];

        $seed = $wafer_id; // Seed the random number generator.
        mt_srand($seed); // Seed the random number generator. 
        $s = sha1(mt_rand());
        $wafer_sn = "W".strtoupper(substr($s, 0, 9));

        return $wafer_sn;
}

function cell_serial_number($cell_uid)
{
	$query = "SELECT * FROM epi_cell WHERE id = $cell_uid";
	$result = $this->local_query($query);
	$this->echo_r($results);

	$cell_wid = $result[0]['wafer_id'];

	$seed = (float)($cell_wid.$cell_uid); // Seed the random number generator.
	mt_srand($seed); // Seed the random number generator. 
	$s = sha1(mt_rand());
	$cell_sn = strtoupper(substr($s, 0, 10));

	return $cell_sn;
}

function get_shipment_types()
{
	$query = "SELECT * FROM epi_shipment_container_type ORDER BY name, rows, cols";
	$result = $this->local_query($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	return $result;
}

function get_customer_id($customer_name)
{
	$query = "SELECT id AS customer_id FROM epi_customer WHERE name LIKE '$customer_name' LIMIT 1";
	$result = $this->local_query($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	return $result[0]['customer_id'];
}

function shipped_processed_wafers()
{
	$query  = "SELECT S.id, S.`date`, S.description, S.status, S.shipment_type, P.project_number, ";
	$query .= "count(W.shipment_id) AS 'shipment_size', ";
	$query .= "C.name AS 'customer', ";
	$query .= "U.user_name AS 'user_name', U.last_name AS 'last_name', U.first_name AS 'first_name' ";
	$query .= "FROM epi_shipment S INNER JOIN epi_shipped_wafer W ON (S.id = W.shipment_id) ";
	$query .= "INNER JOIN epi_customer C ON (S.customer_id = C.id) ";
	$query .= "INNER JOIN epi_user U ON (S.user_id = U.user_id) ";
	$query .= "INNER JOIN customer_projects P ON (P.id = S.customer_project_id) ";
	$query .= "GROUP BY W.shipment_id ";
	$query .= "ORDER BY S.date DESC";

	//echo "<br>$query";
	$result = $this->local_query($query);
	return $result;
}

function shipped_wafers()
{
	$query  = "SELECT S.id, DATE_FORMAT(S.`date`, '%Y-%m-%d') AS `date`, S.description, S.status, S.shipment_type, P.project_number, ";
	$query .= "count(W.shipment_id) AS 'shipment_size', ";
	$query .= "C.name AS 'customer', ";
	$query .= "U.user_name AS 'user_name', U.last_name AS 'last_name', U.first_name AS 'first_name' ";
	$query .= "FROM epi_shipment S INNER JOIN epi_shipped_wafer W ON (S.id = W.shipment_id AND W.wafer_id NOT IN (SELECT DISTINCT wafer_id FROM epi_cell)) ";
	$query .= "INNER JOIN epi_customer C ON (S.customer_id = C.id) ";
	$query .= "INNER JOIN epi_user U ON (S.user_id = U.user_id) ";
	$query .= "INNER JOIN customer_projects P ON (P.id = S.customer_project_id) ";
	$query .= "GROUP BY W.shipment_id ";
	$query .= "ORDER BY S.date DESC";

	//echo "<br>$query";
	$result = $this->local_query($query);
	return $result;
}

function shipments($shipment_type='cells')
{
	$sub_query  = "SELECT shipment_id, count(*) AS shipment_size ";
	$sub_query .= "FROM epi_cell ";
	$sub_query .= "WHERE shipment_id IS NOT NULL AND id NOT IN (SELECT replaced_cell_id FROM epi_cell_replacement) ";
	$sub_query .= "GROUP BY shipment_id";

	$query  = "SELECT S.id, DATE_FORMAT(S.`date`, '%Y-%m-%d') AS `date`, S.description, S.status, S.shipment_type, ";
	$query .= "T.shipment_size, C.name AS 'customer', ";
	$query .= "P.project_number, P.type AS project_type, P.notes AS project_description, ";
	$query .= "U.user_name AS 'user_name', U.last_name AS 'last_name', U.first_name AS 'first_name' ";
	$query .= "FROM epi_shipment S ";
	$query .= "INNER JOIN epi_customer C ON (S.customer_id = C.id AND S.shipment_type LIKE '$shipment_type') ";
	$query .= "INNER JOIN epi_user U ON (S.user_id = U.user_id) ";
	$query .= "LEFT JOIN customer_projects P ON (P.id = S.customer_project_id) ";
	$query .= "INNER JOIN ($sub_query) T ON (T.shipment_id = S.id) ";
	$query .= "ORDER BY S.date DESC";
	//echo "<br>$query";
	$result = $this->local_query($query);
	//$this->echo_r($result);

	return $result;
}

function get_shipment_selected_measurements($shipment_id)
{
	$query = "SELECT m.*, s.status FROM epi_shipment_selected_measurement m INNER JOIN epi_shipment s ON (m.shipment_id = s.id) WHERE m.shipment_id = $shipment_id";
	$result = $this->local_query($query);
	return $result[0];
}

function get_wafers($filter)
{
	$query = "SELECT wafer_id, wafer_name, status, ingot_number, substrate_sn ";
	$query .= "FROM epi_wafer ";
	$query .= "WHERE $filter ";
	$query .= "AND wafer_id NOT IN (SELECT wafer_id FROM epi_shipped_wafer) ";
	$query .= "ORDER BY wafer_id ASC";
	$result = $this->local_query($query);
	return $result;
}

function get_processed_wafers($filter)
{
	$query = "SELECT w.wafer_id, w.wafer_name, w.status, w.ingot_number, w.substrate_sn, s.shipment_id ";
	$query .= "FROM epi_wafer w LEFT OUTER JOIN epi_shipped_wafer s ON (s.wafer_id = w.wafer_id) ";
	$query .= "WHERE $filter ";
	//$query .= "AND wafer_id NOT IN (SELECT wafer_id FROM epi_shipped_wafer) ";
	$query .= "ORDER BY w.wafer_id ASC";
    //echo "<p>$query</p>";
	$result = $this->local_query($query);

	if ($result) {
		foreach ($result as $k=>$v) {
			$q1 = "SELECT DISTINCT session_date, wafer_id, count(*) AS flash_count ";
			$q1 .= "FROM epi_flash_meas WHERE wafer_id = '".$v['wafer_id']."' ";
			$q1 .= "GROUP BY session_date, wafer_id ORDER BY session_date DESC";
			$r1 = $this->local_query($q1);

			$q2  = "SELECT device_mask ";
			$q2 .= "FROM epi_wafer w INNER JOIN epi_device d ON (w.device_mask_id = d.device_mask_id) ";
			$q2 .= "WHERE w.wafer_id = ".$v['wafer_id']." ";
			$q2 .= "LIMIT 1";
			$r2 = $this->local_query($q2);
			//$this->echo_r($r2);
			$result[$k]['device_mask'] = $r2[0]['device_mask'];

			$q3  = "SELECT count(*) as cell_count ";
			$q3 .= "FROM epi_cell ";
			$q3 .= "WHERE wafer_id = ".$v['wafer_id'];
			$r3 = $this->local_query($q3);
			//$this->echo_r($r3);

			$result[$k]['cell_count'] = $r3[0]['cell_count'];

			if ($r1) {
				foreach ($r1 as $j=>$s) {
					$result[$k]['flash_sessions'][] = $s;
				}
			}
			//$this->echo_r($r);
		}
		return $result;
	}
}
  	
function get_pwafers($filter)
{
	$query = "SELECT wafer_id, wafer_name, status, ingot_number, substrate_sn ";
	$query .= "FROM epi_wafer ";
	$query .= "WHERE $filter ";
	$query .= "AND wafer_id NOT IN (SELECT wafer_id FROM epi_shipped_wafer) ";
	$query .= "ORDER BY wafer_id ASC";
	$result = $this->local_query($query);

	if ($result) {
		foreach ($result as $k=>$v) {
			$q1 = "SELECT DISTINCT session_date, wafer_id, count(*) AS flash_count ";
			$q1 .= "FROM epi_flash_meas WHERE wafer_id = '".$v['wafer_id']."' ";
			$q1 .= "GROUP BY session_date, wafer_id ORDER BY session_date DESC";
			$r1 = $this->local_query($q1);

			$q2  = "SELECT device_mask ";
			$q2 .= "FROM epi_wafer w INNER JOIN epi_device d ON (w.device_mask_id = d.device_mask_id) ";
			$q2 .= "WHERE w.wafer_id = ".$v['wafer_id']." ";
			$q2 .= "LIMIT 1";
			$r2 = $this->local_query($q2);
			//$this->echo_r($r2);
			$result[$k]['device_mask'] = $r2[0]['device_mask'];

			$q3  = "SELECT count(*) as cell_count ";
			$q3 .= "FROM epi_cell ";
			$q3 .= "WHERE wafer_id = ".$v['wafer_id'];
			$r3 = $this->local_query($q3);
			//$this->echo_r($r3);

			$result[$k]['cell_count'] = $r3[0]['cell_count'];

			if ($r1) {
				foreach ($r1 as $j=>$s) {
					$result[$k]['flash_sessions'][] = $s;
				}
			}
			//$this->echo_r($r);
		}
		return $result;
	}
}

function ship_to_customer($wafers, $project_id, $description, $customer_id, $user_id, $sales_order='')
{
	$query  = "SELECT max(customer_wafer_number) AS count FROM epi_shipped_wafer ";
	$query .= "WHERE shipment_id = (SELECT max(id) FROM epi_shipment WHERE customer_id = $customer_id)";
	$result = $this->local_query($query);

	$next_wafer_sn = $result[0]['count']+1;

	$query  = "INSERT INTO epi_shipment ";
	$query .= "SET `date` = '".date('Y-m-d H:i:s')."', ";
	$query .= "    description = '$description', ";
	$query .= "    sales_order = '$sales_order', ";
	$query .= "    customer_id = $customer_id, ";
	$query .= "    user_id = $user_id, ";
	$query .= "    status = 'shipped', ";
	$query .= "    shipment_type = 'wafers', ";
	$query .= "    shipment_type_id = 3, "; // 3: processed wafer id
	$query .= "    customer_project_id = '$project_id'";
	$result = $this->local_insert($query);

	$shipment_id = $this->last_insert();

	$shipped_wafers = array();
	foreach ($wafers as $wid) {
		$wafer_sn = "$customer_id-$next_wafer_sn";
		$query  = "INSERT INTO epi_shipped_wafer ";
		$query .= "SET wafer_id = $wid, ";
		$query .= "shipment_id = $shipment_id, ";
		$query .= "customer_wafer_number = $next_wafer_sn";
		$result = $this->local_insert($query);
		if (!empty($result) && ($result == 1)) {
			$shipped_wafers[] = array('wafer_id' => $wid, 'wafer_sn'=> $wafer_sn);
		}
		$next_wafer_sn++;
	}

	return $shipped_wafers;
}

function ship_processed_wafers($wafers, $project_id, $description, $customer_id, $user_id, $sales_order='')
{
	$queries = array();
	$query  = "SELECT max(customer_wafer_number) AS count FROM epi_shipped_wafer ";
	$query .= "WHERE shipment_id = (SELECT max(id) FROM epi_shipment WHERE customer_id = $customer_id)";
	$result = $this->local_query($query);
	$queries[] = $query;

	$next_wafer_sn = $result[0]['count']+1;

	$query  = "INSERT INTO epi_shipment ";
	$query .= "SET `date` = '".date('Y-m-d H:i:s')."', ";
	$query .= "    sales_order = '$sales_order', ";
	$query .= "    description = '$description', ";
	$query .= "    customer_id = $customer_id, ";
	$query .= "    user_id = $user_id, ";
	$query .= "    status = 'shipped', ";
	$query .= "    shipment_type = 'wafers', ";
	$query .= "    shipment_type_id = 3, "; // 3: processed wafer id
	$query .= "    customer_project_id = '$project_id'";
	//echo "<p>$query</p>";
	$result = $this->local_insert($query);
	$queries[] = $query;

	$shipment_id = $this->last_insert();
	//echo "<p>shipment id: $shipment_id</p>";

	$shipped_wafers = array();
	foreach ($wafers as $wid => $flash_session_date) {
		if (!empty($flash_session_date)) {
			$wafer_sn = "$customer_id-$next_wafer_sn";
			$query  = "INSERT INTO epi_shipped_wafer ";
			$query .= "SET wafer_id = $wid, ";
			$query .= "shipment_id = $shipment_id, ";
			$query .= "flash_session_date = '$flash_session_date', ";
			$query .= "customer_wafer_number = $next_wafer_sn";
			//echo "<p>$query</p>";
			$result = $this->local_insert($query);
			$queries[] = $query;
			if (!empty($result) && ($result == 1)) {
				$shipped_wafers[] = array('wafer_id' => $wid, 'wafer_sn'=> $wafer_sn);
			}
			$next_wafer_sn++;

			$query = "UPDATE epi_cell SET shipment_id = '$shipment_id' WHERE wafer_id = $wid";
			$result = $this->local_insert($query);
			$queries[] = $query;
		}
	}

	return array($shipped_wafers,$queries);
}

function create_shipment($description, $customer_name, $project_id, $wp_sns, $raw_meas, $cor_meas, $sys_meas, $user_id, $cell_ids, $shipment_container, $csv_data_file, $sales_order='')
{

	if (!empty($raw_meas)) {
		//echo "<br>raw: $raw_meas";
		$selected_raw_meas = explode(",", $raw_meas);
		foreach ($selected_raw_meas as $v) {
			$selected_meas[] = "raw_".$v."=1";
		}
	}

	if (!empty($cor_meas)) {
		//echo "<br>corrected: $cor_meas";
		$selected_cor_meas = explode(",", $cor_meas);
		foreach ($selected_cor_meas as $v) {
			$selected_meas[] = "corrected_".$v."=1";
		}
	}

	if (!empty($sys_meas)) {
		//echo "<br>system: $sys_meas";
		$selected_sys_meas = explode(",", $sys_meas);
		foreach ($selected_sys_meas as $v) {
			$selected_meas[] = "system_".$v."=1";
		}
	}
	//$this->echo_r($selected_meas);

	//exit;

	$tmp = explode(",", $cell_ids);
	//$this->echo_r($tmp);

	$cell_count = count($tmp);

	$sc = explode("x", $shipment_container);
	$sc_name = $sc[0];
	$sc_id = $sc[1];
	$sc_row = $sc[2];
	$sc_col = $sc[3];
	$sc_capacity = $sc_row*$sc_col;

	$sc_count = ceil($cell_count/$sc_capacity);

	$customer_id = $this->get_customer_id(trim($customer_name));

	//echo "<br>description: $description ";
	//echo "<br>customer id: $customer_id";
	//echo "<br>customer name: $customer_name";
	//echo "<br>user id: $user_id";
	//echo "<br>cell-ids: $cell_ids ";
	//echo "<br>cell count: $cell_count ";
	//echo "<br>container: $shipment_container ";
	//echo "<br> sc-id: $sc_id ";
	//echo "<br>sc-capacity: $sc_capacity ";
	//echo "<br>sc-count: $sc_count ";
	//echo "<br>csv-data: $csv_data_file";

	$query  = "INSERT INTO epi_shipment ";
	$query .= "SET `date` = '".date('Y-m-d H:i:s')."', ";
	$query .= "    sales_order = '$sales_order', ";
	$query .= "    description = '$description', ";
	$query .= "    customer_id = $customer_id, ";
	$query .= "    shipment_type = 'cells', "; // 3: processed wafer id
	$query .= "    shipment_type_id = 3, "; // 3: processed wafer id
	$query .= "    customer_project_id = $project_id, ";
	$query .= "    user_id = $user_id, ";
	$query .= "    status = 'pending'";
	//echo "<br>$query";

	$result = $this->local_insert($query);
	//$this->echo_r($result);

	$sc_sns = explode(",", $wp_sns);

	//$this->echo_r($sc_sns);

	$shipment_id = $this->last_insert();
	//echo "<br><br>";

	$selected_meas[] = "shipment_id=$shipment_id";
	$query = "INSERT INTO epi_shipment_selected_measurement SET ".implode(", ", $selected_meas);
	//echo $query;

	$result = $this->local_insert($query);
	//$this->echo_r($result);

	$p = 0;
	for ($i = 0; $i < $sc_count; $i++) {
		if ($cell_count - $sc_capacity > 0) {
			$cell_quantity = $sc_capacity;
			$cell_count -= $sc_capacity;
		}
		else {
			$cell_quantity = $cell_count;
		}
		$query = "INSERT INTO epi_shipment_container SET type_id = $sc_id, quantity = $cell_quantity, shipment_id = $shipment_id, sn = ".$sc_sns[$i];
		//echo "<br>$query";

		$result = $this->local_insert($query);
		//$this->echo_r($result);
		//echo "<br><br>";

		$shipment_container_id = $this->last_insert();

		$t = 0;
		for ($r = 0; $r < $sc_row; $r++) {
			$row = $r+1;
			for ($c = 0; $c < $sc_col; $c++) {
				$col = $c+1;
				if ($t < $cell_quantity) {
					$cell = explode("_", $tmp[$p]);
					$cell_id = $cell[0];
					$cell_sn = $cell[1];
					$cell_col = $cell[2];
					$cell_row = $cell[3];
					$cell_wafer = $cell[5];
					$cell_session_date = $cell[6];
					$query = "INSERT INTO epi_shipment_container_cell_location ";
					$query .= "SET shipment_container_id = $shipment_container_id, ";
					$query .= "    row = $row, ";
					$query .= "    col = $col, ";
					$query .= "    cell_sn = '$cell_sn', ";
					$query .= "    cell_id = $cell_id, ";
					$query .= "    flash_session_date = '$cell_session_date' ";
					//echo "<br>$query";

					$result = $this->local_insert($query);
					//$this->echo_r($result);
					$t++;
					$p++;

				}
			}
		}
	}

	foreach ($tmp as $k=>$c) {
		$ctmp = explode("_", $c);
		$cell_id = $ctmp[0];
		$cell_sn = $ctmp[1];
		$query = "UPDATE epi_cell SET shipment_id = $shipment_id WHERE id = $cell_id ";
		//echo "<br>$query";
		$result = $this->local_insert($query);
	}
}

function get_all_wafer_cells($wafer_name)
{
	$query  = "SELECT ";
	$query .= " w.wafer_name, ";
	$query .= " w.wafer_id, ";
	$query .= " concat(w.ingot_number,'-', w.substrate_sn) AS ingot_substrate, ";
	$query .= " m.device_mask, ";
	$query .= " d.x_loc, ";
	$query .= " y_loc, ";
	$query .= " mesa_x, ";
	$query .= " mesa_y ";
	$query .= "FROM ";
	$query .= " epi_wafer w ";
	$query .= " INNER JOIN epi_device_mask m ON (w.wafer_name = '$wafer_name' AND w.device_mask_id = m.device_mask_id) ";
	$query .= " INNER JOIN epi_device d ON (d.device_mask_id = m.device_mask_id AND d.grid_spacing > 0) ";
	$query .= "ORDER BY d.y_loc ASC, d.x_loc ASC";

	//echo "$query";
	$result = $this->local_query($query);
	return $result;
}

function get_cell_x_loc($wafer_name)
{
	$query = "select distinct d.x_loc as x_coord FROM epi_wafer w INNER JOIN epi_device_mask m ON (w.device_mask_id = m.device_mask_id) INNER JOIN epi_device d ON d.device_mask_id = m.device_mask_id WHERE w.wafer_name LIKE '$wafer_name' AND d.grid_spacing > 0 ORDER BY d.x_loc ASC";

	//echo "$query";
	$result = $this->local_query($query);
	return $result;
}

function get_cell_y_loc($wafer_name)
{
	$query = "SELECT distinct d.y_loc as y_coord ";
	$query .= "FROM epi_wafer w ";
	$query .= "INNER JOIN epi_device_mask m ON (w.device_mask_id = m.device_mask_id) ";
	$query .= "INNER JOIN epi_device d ON (d.device_mask_id = m.device_mask_id) ";
	$query .= "WHERE w.wafer_name LIKE '$wafer_name' AND d.grid_spacing > 0 ";
	$query .= "ORDER BY d.y_loc ASC";

	//echo "$query";
	$result = $this->local_query($query);
	return $result;
}

function get_cell_details($wafer_name)
{
	$query = "SELECT w.wafer_name, w.wafer_id, m.device_mask, d.x_loc as cell_width, y_loc as cell_height, mesa_x, mesa_y ";
	$query .= "FROM epi_wafer w INNER JOIN epi_device_mask m ON (w.device_mask_id = m.device_mask_id) ";
	$query .= "INNER JOIN epi_device d ON d.device_mask_id = m.device_mask_id ";
	$query .= "WHERE w.wafer_name LIKE '$wafer_name' AND d.grid_spacing > 0 AND d.x_loc > 0 AND d.y_loc > 0 ";
	$query .= "ORDER BY d.y_loc ASC, d.x_loc ASC LIMIT 1";

	//echo "$query";
	$result = $this->local_query($query);
	return $result[0];
}

function add_cell_and_tag($wafer_name, $x_coord, $y_coord, $tag_id, $meas_table)
{
	$query = "INSERT INTO epi_cell SET ";
	$query .= " wafer_id = (select wafer_id from epi_wafer where wafer_name LIKE '$wafer_name'), ";
	$query .= " x_coord = $x_coord, ";
	$query .= " y_coord = $y_coord ";
	//echo "<br>$query";
	$result = $this->local_insert($query);
	//$this->echo_r($result);

	$query = "SELECT LAST_INSERT_ID() AS cell_id";
	$result = $this->local_query($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	$cell_id = $result[0]['cell_id'];
 
	$query  = "INSERT INTO epi_tag_meas SET tag_id = $tag_id , meas_id = $cell_id,  meas_table = '$meas_table'";
	$result = $this->local_insert($query);
	//echo "<br>$query";
	//$this->echo_r($result);
	return $result;
}

function get_tag_id($tag_name)
{
	$query = "SELECT id ";
	$query .= "FROM epi_tag ";
	$query .= "WHERE name LIKE '$tag_name'"; 
	$result = $this->local_query($query);
	return $result[0]['id'];
	
}

function get_tags($type)
{
	$query  = "SELECT id AS tag_id, name AS tag_name, short_description AS description ";
	$query .= "FROM epi_tag ";
	$query .= "WHERE type LIKE '$type' ";
	$query .= "ORDER BY id ASC ";
	$result = $this->local_query($query);
	return $result;
}

function get_cell_tags($wafer_name, $x_coord, $y_coord, $meas_table)
{
	$cell_id = $this->get_cell_id($wafer_name, $x_coord, $y_coord);
	if ($cell_id != NULL) {
		$query  = "SELECT m.meas_id as cell_id, t.id as tag_id, t.name AS tag_name, t.short_description AS tag_description ";
		$query .= "FROM epi_tag_meas m INNER JOIN epi_tag t ON (t.id = m.tag_id AND m.meas_table LIKE '$meas_table') ";
		$query .= "WHERE m.meas_id = $cell_id";
		//$query .= "WHERE m.meas_id = $cell_id AND m.meas_table LIKE '$meas_table'"; 
		$result = $this->local_query($query);
		return $result;
	}
	return NULL;
}

// EPI Measurements TAG functions
// Cell tags START
function get_cell_image_tag($wafer_name, $x_coord, $y_coord) 
{
	$query  = "SELECT t.name AS tag_name, t.short_description AS tag_description ";
	$query .= "FROM epi_wafer w ";
	$query .= "  INNER JOIN epi_cell c ON (w.wafer_id = c.wafer_id) ";
	$query .= "  INNER JOIN epi_tag_meas m ON (c.id = m.meas_id) ";
	$query .= "  INNER JOIN epi_tag t ON (t.id = m.tag_id) "; 
	$query .= "WHERE w.wafer_name LIKE '$wafer_name' ";
	$query .= "AND c.x_coord = $x_coord ";
	$query .= "AND c.y_coord = $y_coord"; 
	//echo "<br> $query";
	$result = $this->local_query($query);
	$this->echo_r($result);
	//echo "<br>x=$x_coord, y=$y_coord, tag=".$result[0]['tag_name'];
	return $result[0]['tag_name'];
}
// Cell Tags END

function qe_get_bg($qe_id) {

	$query = "select  m.* from epi_qe_meas m where  m.qe_id =$qe_id ";
	//echo "<br> $query";
	$result = $this->local_query($query);

	foreach ($result as $r) {
		//echo "<br> getting QE values";
		//print_r($r);
		$qe_dat = $this->get_qe_xy($r['qe_meas_id']);
		
		//print_r($qe_dat);
		//echo "<br> getting QE xy values";
		$res = $this->get_qe_bg($qe_dat,$r['scaled_eqe_max'],0.15);
		
		//print_r($res);
		//echo "<br> getting QE res values";
		
		$lambda_bg_qe = $res['lambda_bg'];
		
		$lambda_start = $r['integration_start_nm'];
		if ($lambda_start ==NULL ){
			$lambda_start = $r['starting_wavelength_nm'];
		}
		
		//fix this to account for scan gap!
		
		$jsc_ideal_bg_qe =  $this->ideal_jsc($lambda_start,$res['lambda_bg']);
		$jsc_relative_bg_qe = $r['jsc_am15d']/$jsc_ideal_bg_qe;
		
		$query = sprintf("update epi_qe_meas set lambda_bg_qe = %.3f, jsc_ideal_bg_qe  =%.3f, jsc_relative_bg_qe =%.3f where qe_meas_id = %d ",$lambda_bg_qe,$jsc_ideal_bg_qe,$jsc_relative_bg_qe,$r['qe_meas_id']);;
	
			//echo "<br> $query";
	
		$result2 = $this->local_insert($query);
		
	}
}
 
 
 
function parse_qe($file, $data)
{

	echo $file;
	// $this->echo_r($data);
	ini_set('auto_detect_line_endings',TRUE);
	
 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
 	
 	$header = $this->qe_get_header($handle);
 	
 	if (!empty($header['jsc_am_15d_iqe']) ){
		$header['jsc_am15d_iqe'] = $header['jsc_am_15d_iqe']; 	
		unset($header['jsc_am_15d_iqe']);
 	}
 	
 	if (!empty($header['chuck_temp']) ){
		$header['chuck_temperature'] = $header['chuck_temp']; 	
		unset($header['chuck_temp']);
 	}
 	
 	// echo "<br> ".$header['start_time']."  to  ".$header['end_time'];
 	$tmp=date_parse($header['start_time']);
 	// $this->echo_r($tmp);
	$header['start_time']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'].' '.$tmp['hour'].':'.$tmp['minute'].':'.$tmp['second'];
 	$tmp=date_parse($header['end_time']);
 	// $this->echo_r($tmp);
	$header['end_time']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'].' '.$tmp['hour'].':'.$tmp['minute'].':'.$tmp['second'];
	
 	// $this->echo_r($header);
 	
 	if (count($header) == 0 ) {
 		return array('error'=>1,'point'=>'file read error');
 	}
 
 	$result = $this->qe_check_header($header);
	if ($result['error'] > 0 ) {
		return $result;
	}
 	
 	$header['wafer_id']=$data['sample_id'];
  $header['wafer_name']=$data['sample'];
  		
 	$header['user_id'] = $this->check_user($data['user']);
 	$header['file_name']=$data['file'];
 	
 	
 	// echo "<br> checking for existing records ";
 	
	$qe_id = $this->check_qe($header);
	// echo "<br> $qe_id";
	if ($qe_id == NULL ) {
		$result = $this->new_qe($header);
		if ($result['error'] > 0 ) {
			return $result;
		}
		$qe_id = $result['data_id'];	
	}
	// echo "<br> $qe_id";
	
	$qe_meas_id = $this->check_qe_meas($qe_id,$header);
	
	if ($qe_meas_id != NULL ) {
		return array('error'=>4,'point'=>' QE data for die '.$header['die_number'].' already exists');
	}
	
	$result = $this->new_qe_meas($qe_id,$header);	
	if ($result['error'] > 0 ) {
		return $result;
	}
	$qe_meas_id = $result['data_id'];
	
	$columns = $this->qe_meas_values_columns($handle);
	
	while(!feof($handle)) {
		$values = fgetcsv($handle);
		$result = $this->new_qe_meas_values($qe_meas_id,$columns,$values);
		if ($result['error'] > 0 ) {
			return $result;
		}
	}
	
	// now if this is a piece make sure we get the right coords ... 
	
	if ($this->is_piece($header['wafer_id'])) {

		$xy = $this->get_piece_center($header['wafer_id']);
		
		$piece_x=$xy['x']*1000;
		
		$piece_y=$xy['y']*1000;

		$query = "update epi_qe_meas set x = $piece_x, y=$piece_y where qe_meas_id = $qe_meas_id";
		// echo "<br>updating PL coords $query<br>";
		$result = $this->local_insert($query);
	
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}

	// echo "<br> QE MEAS stats <br>";
	// update statistics
	$result = $this->qe_meas_stats($qe_meas_id);
	
	if ($result['error'] > 0 ) {
		return $result;
	}
	// echo "<br> QE stats <br>";

	$result =  $this->qe_stats($qe_id);
	if ($result['error'] > 0 ) {
		return $result;
	}
	
		// echo "<br> QE date <br>";

	$this->qe_date($qe_id);
	
	// echo "<br> QE PL <br>";

	$this->qe_get_pl($qe_id);
	
//	$this->qe_get_bg($qe_id);
	return array('error'=>0,'data_id'=>$qe_id);
	
}
 
  
  //===================================//
  
  
  // ============= IV Data  =============//
  	function iv_get_header(&$handle) {
	
		$string = fgetcsv($handle);
		$header = array();
		
		$delim = ",";
		
		while(!feof($handle) && !stristr($string[0],'Die Number')) {	
			$tmp = array();
			/*if (stristr($string[0],'date') && $delim === ":") {
					$str = implode(',',$string);
				//echo "<br> Found Time ".$string[0]." whole $str";;
				$tmp2=explode($delim,trim($str));
				
				$tmp[0]=array_shift($tmp2);
				$tmp[1]=implode($delim,$tmp2);
				
			} else {
				$tmp=explode($delim,trim($string[0]));
			}*/
			
			$tmp[0] = $string[0];
			$tmp[1] = $string[1];
			
			$key = strtolower(str_replace(' ','_',str_replace(')','',str_replace('(','',str_replace('.','',str_replace('-','',trim($tmp[0])))))));
			$val = trim($tmp[1]);
			
			if(stristr($key,'points')) {
				$val = (int)$val;
			}
			
			$header[$key]=$val;
			$string = fgetcsv($handle);
			//$this->echo_r($string);
		}
	
		return $header;
	}

  
  
	function iv_check_header($header)
	{
		if ($header['mask_name'] == NULL || $header['device_name'] == NULL || (isset($header['type']) && $header['type'] == 'Light' && $header['filter'] == NULL) ) {
			return array('error'=>3,'point'=>'File Read Error : could not extract key parameters');
		}
		return array('error'=>0);
	}
 
  
  function check_iv($header){
		
	/*$query = "select iv_id from epi_iv where  ";
	foreach ($header as $k=>$v) {
		
			$q[]=" $k='$v' ";
		
	}
	
	$query .=implode(' and ',$q);
	*/
	$query  = "
			select iv_id from epi_iv where   
			date='".$header['date']."'  and  
			start_voltage LIKE '%".rtrim($header['start_voltage'],'0')."%'  and  
			stop_voltage LIKE '%".rtrim($header['stop_voltage'],'0')."%'  and  
			points='".$header['points']."'  and  
			current_range LIKE '%".rtrim($header['current_range'],'0')."%'  and  
			dwell_time LIKE '%".rtrim($header['dwell_time'],'0')."%'  and  
			lamp_current LIKE '%".rtrim($header['lamp_current'],'0')."%'  and  
			filter='".$header['filter']."'  and  
			mask_name='".$header['mask_name']."'  and  
			device_name='".$header['device_name']."'  and  
			wafer_id='".$header['wafer_id']."'  and  
			wafer_name='".$header['wafer_name']."'  and  
			user_id='".$header['user_id']."'  and  
			file_name='".$header['file_name']."'  and  
			type='".$header['type']."' 
			order by iv_id desc 
			limit 1";
	
	$query  = "
			select iv_id from epi_iv where   
			
			start_voltage LIKE '%".rtrim($header['start_voltage'],'0')."%'  and  
			stop_voltage LIKE '%".rtrim($header['stop_voltage'],'0')."%'  and  
			
			current_range LIKE '%".rtrim($header['current_range'],'0')."%'  and  
			dwell_time LIKE '%".rtrim($header['dwell_time'],'0')."%'  and  
			lamp_current LIKE '%".rtrim($header['lamp_current'],'0')."%'  and  
			filter='".$header['filter']."'  and  
			mask_name='".$header['mask_name']."'  and  
			coating='".$header['coating']."'  and 
			chuck_temp LIKE '%".rtrim($header['chuck_temp'],'0')."%'  and  
			device_name='".$header['device_name']."'  and  
			wafer_id='".$header['wafer_id']."'  and  
			wafer_name='".$header['wafer_name']."'  and  
			user_id='".$header['user_id']."'  and  
			
			revision LIKE '%".rtrim($header['revision'],'0')."%'  and  
			
			type='".$header['type']."' 
			order by iv_id desc 
			limit 1";
	
	
	
	$result = $this->local_query($query);
	if ($result == NULL ) {
			return 0;
	}
	return $result[0]['iv_id'];
	
}

function check_iv_meas($iv_id,$header) {
	$query = "select iv_meas_id from epi_iv_meas where iv_id = $iv_id and die_number = '".$header['die_number']."'";
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return 0;
	}
	
	return $result[0]['iv_meas_id'];
	
}
  
 function new_iv($header) {
		
	
	$query = "insert into epi_iv (".implode(',',array_keys($header)).") 
						values	('".implode("','",array_values($header))."') ";
	
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$iv_id = $this->last_insert();
	if($iv_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0,'data_id'=>$iv_id);
	

}

function iv_meas_type($meas) {
	if (strtolower($meas) == 'liv') {
		return 'Light';
	}
	return 'Dark';
} 


function iv_meas_values_columns(&$handle) {
	rewind($handle);
	$string = array();
	
	while(!feof($handle) && !stristr($string[0],'Die Number')) {	
		$string = fgetcsv($handle);
	//	$this->echo_r($string);
	}
	if (count($string) == 0 ) {
		return NULL;
	}
	for($i=0;$i<count($string); $i++) {
		$tmp = explode('(',$string[$i]);
		$columns[$i]=str_replace('isat','jsat',str_replace('imp','jmp',str_replace('isc','jsc',str_replace(' ','_',trim(str_replace('%','',str_replace('-','',strtolower($tmp[0]))))))));
	}	
	//$this->echo_r($columns);
	return $columns;
}

function new_iv_meas($iv_id,$header) {
	
	$query = "insert into epi_iv_meas set ";
	foreach ($header as $k=>$v) {
		if (strlen(trim($k)) > 0) { 
			$q[]= $k."='".addslashes($v)."'";		
		}
	}
	$query .= implode(',',$q);
	$result = $this->local_insert($query);

	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$iv_meas_id = $this->last_insert();
	if($iv_meas_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	return array('error'=>0,'data_id'=>$iv_meas_id);	

}


function new_iv_meas_values($iv_meas_id,$j,$v){
	for($i=0;$i<count($j); $i++) {
		$query = "insert into epi_iv_meas_values set 
					iv_meas_id = $iv_meas_id, 
					j= ".addslashes($j[$i]).", 
					v= ".addslashes($v[$i])." ";
					
		$result = $this->local_insert($query);
	
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}
	
	return array('error'=>0);	
}

function iv_stats($iv_id){

	$query = "select 
					AVG(jsc) as jsc_avg,
					MIN(jsc) as jsc_min,
					STDDEV_POP(jsc) as jsc_std,
					AVG(jmp) as jmp_avg,
					MIN(jmp) as jmp_min,
					STDDEV_POP(jmp) as jmp_std,				
					AVG(voc) as voc_avg,
					MAX(voc) as voc_max,
					STDDEV_POP(voc) as voc_std,
					AVG(vmp) as vmp_avg,
					MAX(vmp) as vmp_max,
					STDDEV_POP(vmp) as vmp_std,					
					AVG(efficiency) as efficiency_avg,
					MAX(efficiency) as efficiency_max,
					STDDEV_POP(efficiency) as efficiency_std,
					AVG(pmax) as pmax_avg,
					MAX(pmax) as pmax_max,
					STDDEV_POP(pmax) as pmax_std,
					AVG(fill_factor) as fill_factor_avg,
					MAX(fill_factor) as fill_factor_max,
					STDDEV_POP(fill_factor) as fill_factor_std,
					AVG(rshunt_at_jsc) as rshunt_at_jsc_avg,
					MAX(rshunt_at_jsc) as rshunt_at_jsc_max,
					STDDEV_POP(rshunt_at_jsc) as rshunt_at_jsc_std,
					AVG(rseries_at_voc) as rseries_at_voc_avg,
					MIN(rseries_at_voc) as rseries_at_voc_min,
					STDDEV_POP(rseries_at_voc) as rseries_at_voc_std,
					AVG(percent_error_jsc) as percent_error_jsc_avg,
					AVG(percent_error_voc) as percent_error_voc_avg,
					AVG(spectral_mismatch_factor) as spectral_mismatch_factor_avg,
					AVG(ideality_factor) as ideality_factor_avg,
					AVG(jsat) as jsat_avg,
					MEDIAN(efficiency,5) as efficiency_median,
					MEDIAN(jsc,5) as jsc_median,
					MEDIAN(voc,5) as voc_median,
					MEDIAN(fill_factor,5) as fill_factor_median,
					MEDIAN(rshunt_at_jsc,5) as rshunt_at_jsc_median,
					MEDIAN(rseries_at_voc,5) as rseries_at_voc_median,
					MEDIAN(jmp,5) as jmp_median,
					MEDIAN(vmp,5) as vmp_median
					
					
					from epi_iv_meas where iv_id =$iv_id
					";
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);	
	}
	
	$row  =$result[0];
	
	$query = "update epi_iv set ";
	foreach($row as $k=>$v) {
		$q[]=" $k = '$v' ";
	
	}
	$query .= implode(',',$q);
	$query .=" where iv_id =$iv_id";
	
//	echo "<br> $query";
	
	$result = $this->local_insert($query);

	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0);	

}


  
function parse_iv($file,$data) {

	echo $file;
	// $this->echo_r($data);
	ini_set('auto_detect_line_endings',TRUE);
	
 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
 	// echo "<br> Can read file";
 	$header = $this->iv_get_header($handle);
 	$this->echo_r($header);
 	 
 	echo "header date: ".$header['date'];
 	$tmp = date_parse($header['date']);

 	$this->echo_r($tmp);
	$header['date']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'].' '.$tmp['hour'].':'.$tmp['minute'].':'.$tmp['second'];
 	
 	// $this->echo_r($header);
 	// echo "<br> count header ".count($header);

 	if (count($header) == 0 ) {
 		return array('error'=>1,'point'=>'file read error');
 	}
 
 	$result = $this->iv_check_header($header);
	if ($result['error'] > 0 ) {
		return $result;
	}
	// echo "<br> header OK";
 	
 	
 	$header['wafer_id']=$data['sample_id'];
  	$header['wafer_name']=$data['sample'];
  		
 	$header['user_id'] = $this->check_user($data['user']);
 	$header['file_name']=$data['file'];
 	
	$header['type']=$this->iv_meas_type($data['meas']);
 	
 	// echo "<br> checking for existing records ";
 	
	//$iv_id = $this->check_iv($header);
	
	$iv_id = NULL;
	
	if ($iv_id == NULL ) {
		echo "<br> no measurement found - creating new IV measurement instance ";
		$result = $this->new_iv($header);
		if ($result['error'] > 0 ) {
			return $result;
		}
		$iv_id = $result['data_id'];	
	}
	
	// echo "<br> IV ID: $iv_id";
	
	$columns = $this->iv_meas_values_columns($handle);

//	$this->echo_r($columns);
		
	
/*	$iv_meas_id = $this->check_iv_meas($iv_id,$header);
	
	if ($iv_meas_id != NULL ) {
		return array('error'=>4,'point'=>' IV data for die '.$header['die_number'].' already exists');
	}
	
	$result = $this->new_qe_meas($qe_id,$header);	
	if ($result['error'] > 0 ) {
		return $result;
	}
	$qe_meas_id = $result['data_id'];
*/	
	while(!feof($handle)) {
		$extracted = fgetcsv($handle);
		$j = fgetcsv($handle);
		$v = fgetcsv($handle);
		$tmp = array();
		
		$tmp['iv_id']=$iv_id;
		$tmp['wafer_id']= $header['wafer_id'];
		$tmp['wafer_name']= $header['wafer_name'];
		$tmp['mask_name']= $header['mask_name'];
		$tmp['device_name']= $header['device_name'];
		$tmp['lamp_current']= $header['lamp_current'];
		$tmp['filter']= $header['filter'];
		$tmp['type']= $header['type'];
		$tmp['chuck_temp']= $header['chuck_temp'];
		$tmp['coating']= $header['coating'];

		
		for($i=0;$i<count($columns);$i++) {
			
			if ($columns[$i] == 'fill_factor') {
				$extracted[$i]=abs($extracted[$i]);
			}
			
			$tmp[$columns[$i]] = $extracted[$i];
		}
		//$this->echo_r($tmp);
		
		if ($tmp['die_number'] == NULL ) {
			break;
		}
		
		//$iv_meas_id = $this->check_iv_meas($iv_id,$tmp);
		
		//if ($iv_meas_id != NULL ) {
		//	echo "<br> ERROR measurement for die ".$tmp['die_number']." already exists! <br>";
		//	continue;
		//} else {
		
			$result = $this->new_iv_meas($iv_id,$tmp);		
		
			if ($result['error'] > 0 ) {
				return $result;
			}
		
			$iv_meas_id = $result['data_id'];
	//	}
		
		$result = $this->new_iv_meas_values($iv_meas_id,$j,$v);
		
		if ($result['error'] > 0 ) {
			return $result;
		}
	}
	
		
	// update statistics
	$result = $this->iv_stats($iv_id);
	if ($result['error'] > 0 ) {
		return $result;
	}
	
	return array('error'=>0,'data_id'=>$iv_id);
	
}

  
  
  
  
  //=====================================//
   //============ CIV Data ==============//
   
   
	function parse_civ($file,$data){
	
		// echo "<br> Parsing CIV ";
		$data['file_name'] = $file;
		// $this->echo_r($data);
	
		ini_set('auto_detect_line_endings',TRUE);
	
 		$handle = fopen($file, "r");
	
		if ($handle == NULL ){
			return array('error'=>3,'point'=>'File Read Error');
		}
 		
 		
 		$hdelim = ",";
		$ddelim = ",";
		
		$hend = "Area";
		$dend = "";
		
		$line = array();
		$header = array();
		
		while(!feof($handle) && !stristr($line[0],$hend)){
			$extract = fgets($handle);
			$line=explode($hdelim,$extract);
			if(count($line) > 2){ 
				$tmp1 =array_shift($line);
				$tmp2 =implode($hdelim,$line);
				$line = array(trim($tmp1),trim($tmp2));
			}
			$header[]=$line;
		}
		
		$parsed_header = $this->parse_civ_header($header);

		
		
		$parsed_header['file_name'] = $data['file_name'];
		$parsed_header['user_name'] = $data['user'];
		$parsed_header['wafer_name'] = $data['sample'];
		$parsed_header['wafer_id'] = $data['sample_id'];
		$parsed_header['date_uploaded'] = date('Y-m-d H:i:s');
		
		
		// $this->echo_r($parsed_header);
		
		$result = $this->new_civ_meas($parsed_header);
		
		
		
		if ($result['error']) {
		
			return $result;
		
		}
		
		$civ_meas_id = $result['data_id'];
		
		$data = array();
		
		while(!feof($handle) && !stristr($line[0],$dend)){
			$extract = fgets($handle);
			
			$line = explode($ddelim,$extract);
			if (strlen(trim(implode('',$line))) > 0 ) {
				$data[] = $line;
			}
		}
 		
 		$result = $this->new_civ_meas_values($civ_meas_id,$data);
 		
 		if ($result['error']) {
 			return $result;
 		}
 		
 		
 		$result = $this->calculate_civ_params($civ_meas_id);
 		
 		
 		return array('error'=>0,'data_id'=>$civ_meas_id);
 		
 	}
   
    function calculate_civ_params($civ_meas_id) {
    	$query ="update epi_civ_meas set Jmp_ov_C =  Jmp/Concentration where civ_meas_id = $civ_meas_id";
    	$result = $this->local_insert($query);
    	$query ="update epi_civ_meas set Efficiency_from_Jmp_ov_C =  1000*Jmp_ov_C*Vmp where civ_meas_id = $civ_meas_id ";
    	$result = $this->local_insert($query);
    	$query ="update epi_civ_meas set Efficiency_from_Jsc_and_Vmp =  1000*Jsc_at_1Sun*Vmp where civ_meas_id = $civ_meas_id ";
    	$result = $this->local_insert($query);
    }
   
   	function parse_civ_header($header) {
		$parsed = array();
		foreach ($header as $h) {
		 
		  $k = $h[0];
		  $v = $h[1];
		  if(count(trim($k)) == 0 ) {
		  	continue;
		  }
		  $key = $this->clean_header_line(trim($k));
		  
		  if (stristr($key,'date') ){
		  	$v = $this->parse_datetime($v);
		  }
		  
		  if (strtolower($v) == 'inf' || strtolower($v) == 'nan') {
		  
		  }
		  
		  if ($key != '') {
		  	$parsed[$key] = trim($v);
		  }
		}
		return $parsed;
	}
   
   	function new_civ_meas($parsed_header) {
   		$query = "insert into epi_civ_meas (".implode(',',array_keys($parsed_header)).") values ('".implode("','",$this->array_sanitize(array_values($parsed_header)))."')";
		
		// echo "<br> $query";
		
		$result = $this->local_insert($query);
		if($result == NULL ) {
			echo "<br> Error: could not insert $query <br>";
			return array('error'=>1,'point'=>$query);;
		}
		
		$civ_meas_id = $this->last_insert();
   		
   		return array('error'=>0,'data_id'=>$civ_meas_id);
   	}
   	
   	function new_civ_meas_values($civ_meas_id,$data){ 
   		
   		/* faster insert query */
   		/*$qf = "insert into epi_civ_meas_values (civ_meas_id,V,I,J) VALUES ";
		echo $qf;
		$qfi = array();
   		for($i = 0; $i< count($data[0]); $i++) {
   			$qfi[] = "(".$civ_meas_id.",'".$data[0][$i]."','".$data[1][$i]."','".$data[2][$i]."')";
   		
   		}
   		
   		$qf .= implode(',',$qfi);
   		echo $qf;
   		$result = $this->local_insert($qf);
   		if ($result == NULL ) {
   			echo "<br> Error: could not insert $query <br>";
			return array('error'=>1,'point'=>$qf);;
   		}

   		return array('error'=>0,'data_id'=>$civ_meas_id);
	*/
   		
   		for($i = 0; $i< count($data[0]); $i++) {
   			$query = "insert into epi_civ_meas_values set
   							civ_meas_id = '".$civ_meas_id."',
   							V = '".$data[0][$i]."',
   							I = '".$data[1][$i]."',
   							J = '".$data[2][$i]."'";
   			$result = $this->local_insert($query);
   			if ($result == NULL ) {
   				echo "<br> Error: could not insert $query <br>";
				return array('error'=>1,'point'=>$query);;
   			}
   		
   		}
   	
   		return array('error'=>0,'data_id'=>$civ_meas_id);
   	
   	}
   	
   
    //=====================================//
  //============ TJIV Data ==============//
	
	/* check for duplicates / existing measurements */
	
	function check_tjiv($data) {
		$query = "select tjiv_id from epi_tjiv where wafer_id = ".$data['sample_id']." and wafer_name = '".$data['sample']."'";
		
		$result = $this->local_query($query);
		return $result[0]['tjiv_id'];
	
	}
	
	/* create a new measurement instance */
	function new_tjiv($data){
		$query = "insert into epi_tjiv set
						wafer_id = ".$data['sample_id'].",
						wafer_name = '".$data['sample']."'";
						
		// echo "<br> $query";
		$result = $this->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	
		$id = $this->last_insert();
		if($id == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
		return array('error'=>0,'data_id'=>$id);
	}


	/* main file parser */
	function parse_tjiv($file,$data){
	
		// echo "<br> Parsing TJIV ";
		$data['file_name'] = $file;
		// $this->echo_r($data);
	
		ini_set('auto_detect_line_endings',TRUE);
	
 		$handle = fopen($file, "r");
	
		if ($handle == NULL ){
			return array('error'=>3,'point'=>'File Read Error');
		}
 		
 		$delim = 'Date';
 		$arr = array();
 		
 		//first get the measurement id:
 		$tjiv_id = $this->check_tjiv($data);
 		
 		if ($tjiv_id == NULL ) {
 			$result = $this->new_tjiv($data);
 			if ($result['error']){
 				return $result;
 			}
 			$tjiv_id = $result['data_id'];
 		}
 		
 		while(!feof($handle)) {
 			$str = fgetcsv($handle);
 			if (stristr($str[0],$delim) && count($arr) > 0 ) {
				//echo "<hr> New Entry for TJIV id $tjiv_id";
 				//$this->echo_r($arr);
 				
 				$result = $this->parse_tjiv_entry($data,$arr,$tjiv_id);
 				if ($result['error']) {
 					return $result;
 				}
 				$arr = array();
 				$arr[] = $str;
 			} else {
 				$arr[] = $str;
 			
 			}
 		
 		}
 		
 		if ( count($arr) > 0 ) {
 				$result = $this->parse_tjiv_entry($data,$arr,$tjiv_id);
 				if ($result['error']) {
 					return $result;
 				}
 				$arr = array();
 		}
 		
 		
 		$this->tjiv_stats($tjiv_id);
 		
 		return array('error'=>0,'data_id'=>$tjiv_id);
 		
 		//$header = $this->iv_get_header($handle); 
 	
	}
	
	/* calculate stats and averages */
	function tjiv_stats($tjiv_id){ 
		$stats = array('MIN','MAX','AVG','STDDEV_POP','median');
		$fields = array(
					'Voltage_Drop_at_J7_5__V_',
					'Voltage_Drop_at_J15__V_',
					'Voltage_Drop_at_J30__V_',
					'Resistivity_at_J7_5__Ohms_cm_sq_',
					'Resistivity_at_J15__Ohms_cm_sq_',
					'Resistivity_at_J30__Ohms_cm_sq_'
					);
	
		
		foreach( $fields as $f) {
			$query  = 'select ';
			$q = array();
			foreach ($stats as $s) {
				$dig ='';
				if ($s == 'median') {
					$dig = ',5';
				}
				$q[]= $s.'('.$f.$dig.') as '.$f.'_'.$s;
			}
			
			$query .= implode(',',$q);
			
			$query .= " from epi_tjiv_meas where tjiv_id = $tjiv_id 
						and $f > 0 ";

			// echo "<br>xxx-  $query";
			
			$result = $this->local_query($query);
			// $this->echo_r($result);
			
			foreach ($result[0] as $k=>$v) {
				$query  ="update epi_tjiv set $k = ".($v == NULL? 0.0 : $v)." where tjiv_id = $tjiv_id;";
				// echo "<br>yyy -  $query";
				$result = $this->local_insert($query);
			
			}	
		}
	}
	
	
	/* parse measurement in file */	
	function parse_tjiv_entry($data,$entry,$tjiv_id){
		
		$header  = array();
		$numdata = array();

		for($i=0;$i<count($entry); $i++) {
			if (!is_numeric($entry[$i][0])) {
				$header[] = $entry[$i];
			} else {
				$numdata[] = $entry[$i];
			}
		
		}
		//echo "<br> parsing header";
		$parsed_header = $this->parse_tjiv_header($header);
		// $this->echo_r($parsed_header);
		//$this->echo_r($numdata);
		//echo "<br> COUNT: ".count($numdata);
		
		$parsed_header['wafer_id'] = $data['sample_id'];
		$parsed_header['wafer_name'] = $data['sample'];
		$parsed_header['tjiv_id']=$tjiv_id;
		$parsed_header['file_name'] = $data['file_name'];
		
		$area = $parsed_header['Area'];
		
		$query = "insert into epi_tjiv_meas (".implode(',',array_keys($parsed_header)).") values ('".implode("','",$this->array_sanitize(array_values($parsed_header)))."')";
		// echo "<br> $query";
		
		$query = str_replace('NaN','0.0',$query);
		
		// echo "<br> $query";
		
		$result = $this->local_insert($query);
		if($result == NULL ) {
			echo "<br> Error: could not insert $query <br>";
			return array('error'=>1,'point'=>$query);;
		}
		
		$tjiv_meas_id = $this->last_insert();
		
		$result = $this->new_tjiv_meas_values($tjiv_meas_id,$numdata,$area);
		
	}
	
	/* add data */
	function new_tjiv_meas_values($tjiv_meas_id,$numdata,$area) {
		for ($i=0;$i<count($numdata[0]);$i++){
		
			if ($numdata[0][$i] == 'NaN') {
				$numdata[0][$i] = 0.0;
			}
			if ($numdata[1][$i] == 'NaN') {
				$numdata[1][$i] = 0.0;
			}
			if ($numdata[2][$i] == 'NaN') {
				$numdata[2][$i] = 0.0;
			}
		
			$query = "insert into epi_tjiv_meas_values
						set 
							tjiv_meas_id = $tjiv_meas_id,
							I = ".$numdata[0][$i].",
							J = ".($numdata[0][$i]/$area).",
							V = ".$numdata[1][$i].",
							dV_dI = ".$numdata[2][$i].",
							dV_dJ = ".($numdata[2][$i]*$area)."
						";
			echo "<br> Adding data point $query";
			$result = $this->local_insert($query);
			if ($result == NULL ) {
				echo "<br> ERROR inserting data point ".$query;
			}
		}
		
		return 1;
		
	}
	
	function parse_tjiv_entry_newtable($data,$entry,$tjiv_id){
		
		$header  = array();
		$numdata = array();

		for($i=0;$i<count($entry); $i++) {
			if (!is_numeric($entry[$i][0])) {
				$header[] = $entry[$i];
			} else {
				$numdata[] = $entry[$i];
			}
		
		}
	
		//$this->echo_r($header);
		//$this->echo_r($numdata);
		// echo "<br> parsing header";
		$parsed_header = $this->parse_tjiv_header($header);
		// $this->echo_r($parsed_header);
		
		echo ' CREATE TABLE `epi`.`epi_tjiv_meas` (
			`tjiv_meas_id` INT( 11 ) NOT NULL AUTO_INCREMENT ,
			`tjiv_id` INT( 11 ) NOT NULL ,
			`wafer_id` INT( 11 ) NOT NULL ,
			`wafer_name` VARCHAR( 256 ) NOT NULL ,
			
			';
			
		
		foreach ($parsed_header as $k => $v) {
			echo " $k ";
			if (stristr($k,'date') ){
				echo 'DATETIME NOT NULL , '.'<br>';
				continue;
			}
			if (is_numeric($v)) {
				
				if (stristr($v,'.')) {
					echo ' FLOAT NOT NULL , '.'<br>';
				} else {
					echo ' INT( 11 ) NOT NULL , '.'<br>';
				}
				
				
			} else {
				echo ' VARCHAR( 256 ) NOT NULL , '.'<br>';
			}
		
		}
		
		echo 'PRIMARY KEY ( `tjiv_meas_id` )
				) ENGINE = InnoDB CHARACTER SET utf8 COLLATE utf8_unicode_ci ';
				
		
	
	}
	
	/* parse file header */
	
	function parse_tjiv_header($header) {
		$parsed = array();
		foreach ($header as $h) {
		  if(count(trim($h)) == 0 ) {
		  	continue;
		  }
		  $tmp = explode(':', $h[0]);
		 
		  $key = $this->clean_header_line(array_shift($tmp));
		  
		  $val = trim(rtrim(implode(':',$tmp),','));
		 
		  if (stristr($key,'date') ){
		  	$val = $this->parse_datetime($val);
		  }

		  if ($key != '') {
		  	$parsed[$key] = $val;
		  }
		}
		return $parsed;
	}

	function clean_header_line($line) {
	
		return preg_replace('/[^a-zA-Z0-9]/','_',$line);
	
	}

  //=====================================//
  
  
    //============ CIV Data ==============//

  //=====================================//
  
  
  
   //============ CV Data ==============//


 


function cv_get_header($filedata) {
	
	$dat = array();
	foreach ($filedata as $d) {
		if(stristr($d[1],'Voltage')) {
			break;
		}
		
		
		
		$dat[]=$d;
	}
	
	$delim = ",";
	
	$header=array();
	foreach ($dat as $h) {
		//if(stristr($h[0],'date')) {
	//		$tmp2 = explode(':',$h[0]);
//			$tmp[0]=array_shift($tmp2);
//			$tmp[1]=implode(':',$tmp2);
//		} else {
		//	$tmp = explode($delim,$h[0]);
//		}
		//$this->echo_r($tmp);
		$key = strtolower(str_replace(' ','_',trim(str_replace('(','',str_replace(')','',$h[0])))));
		$val = trim($h[1]);
		$header[$key]=$val;
	}
	
	
	if (!empty($header['date'])) 
	{
		$header['date'] = $this->parse_datetime($header['date']);
	}
	
	return $header;

}
 
 
 
function cv_get_data($filedata) {
	
	
	for ($i=0;$i<count($filedata); $i++) {
		$d = $filedata[$i];
		if(stristr($d[1],'Voltage')) {
			$columns = $d;
			$istart = $i;
			break;
		}
	}

	for($i=0;$i<count($columns);$i++) {
		$tmp = $columns[$i];
		$columns[$i]=strtolower(str_replace(' ','_',trim(str_replace('(','',str_replace(')','',$tmp)))));
	}
	$data = array();
	
	for ($i=$istart+1;$i<count($filedata); $i++) {
		$data[$i-$istart-1] = $filedata[$i];
	}
	
	return array('columns'=>$columns,'data'=>$data);

} 
 
 
 function check_cv($header){

	//$tmp=date_parse($header['date']);
	//$header['date']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'];

	$query = "select cv_id from epi_cv where 
					wafer_id = '".$header['wafer_id']."' and
					user_id = '".$header['user_id']."' and
					wafer_name = '".$header['wafer_name']."' and
					date = '".$header['date']."' and
					mask_name = '".$header['mask_name']."' and
					device_name = '".$header['device_name']."'";

//	echo '<pre>';
//	echo "<br> $query";
//	echo '</pre>';

	$result = $this->local_query($query);

	if ($result == NULL ) {
		return 0;
	}
	return $result[0]['cv_id'];
	
}
 
 function check_cv_meas($cv_id,$header) {

 	//$tmp=date_parse($header['date']);
	//$header['date']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'].' '.$tmp['hour'].':'.$tmp['minute'].':'.$tmp['second'];
	
	$query = "select cv_meas_id from epi_cv_meas where 
					cv_id = $cv_id and
					wafer_id = '".$header['wafer_id']."' and
					wafer_name = '".$header['wafer_name']."' and
					date = '".$header['date']."' and
					die  = '".$header['die']."'";
	
//	echo '<pre>';
//	echo "<br> $query";
//	echo '</pre>';
	
	$result = $this->local_query($query);

	if ($result == NULL ) {
		return 0;
	}
	return $result[0]['cv_meas_id'];
 
 }
 
 function new_cv($header){

	//$tmp=date_parse($header['date']);
	//$header['date']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'];

	$query = "insert into epi_cv set 
					wafer_id = '".$header['wafer_id']."',
					user_id = '".$header['user_id']."',
					wafer_name = '".$header['wafer_name']."',
					date = '".$header['date']."',
					mask_name = '".$header['mask_name']."',
					device_name = '".$header['device_name']."',
					file_name  =  '".$header['file_name']."'
					";

//	echo '<pre>';
//	echo "<br> $query";
//	echo '</pre>';

	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$cv_id = $this->last_insert();
	if($cv_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0,'data_id'=>$cv_id);
} 

 function new_cv_meas($cv_id,$header){

	$tmp=date_parse($header['date']);
	$header['date']=$tmp['year'].'-'.$tmp['month'].'-'.$tmp['day'].' '.$tmp['hour'].':'.$tmp['minute'].':'.$tmp['second'];
	$header['cv_id'] = $cv_id;
	
	$dbkeys=array_flip($this->get_table_columns('epi_cv_meas'));
	unset($dbkeys['action']);
	$dbkeys=array_flip($dbkeys);
	
	$query = "insert into epi_cv_meas set "; 
	foreach($dbkeys as $key) {
		if (empty($header[$key])) 
		{
			continue;
		}
		$q[] = " $key = '".$header[$key]."' ";
	}
	$query .= implode(',',$q);
	
//	echo '<pre>';
//	echo "<br> $query";
//	echo '</pre>';

	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$cv_meas_id = $this->last_insert();
	if($cv_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0,'data_id'=>$cv_meas_id);
	
} 


function new_cv_meas_values($cv_meas_id,$columns,$values) {
	//Depletion Width (nm),Voltage,Capacitance,Conductance,Inv_C_sq,Best Fit of inv_C_sq
	if (strlen(trim(implode('',$values))) == 0 ) {
		return array('error'=>0);;
	}
	$query = "insert ignore into epi_cv_meas_values 
					set cv_meas_id = $cv_meas_id,";
					
	for ($i=0; $i< count($columns) ; $i++ ) {
		
		if (stristr($values[$i],'nan') ) {
			$values[$i] = -1;
		}
		
		$q[]=$columns[$i]." = '".$values[$i]."'";
	}
	$query .=implode(',',$q);
//	echo "<br> $query";
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0);
}
 
function check_cv_meas_values($cv_meas_id) {
	$query = "select count(*) as count from epi_cv_meas_values where cv_meas_id = $cv_meas_id";
	$result = $this ->local_query($query);
	return ($result[0]['count'] > 0);
}


function cv_stats($cv_id) {
	$query="
			select  AVG(depletion_width_at_zero_bias_nm) as depletion_width_at_zero_bias_nm_avg,
					AVG(n_num_cm_cubed) as n_num_cm_cubed_avg,
					MAX(depletion_width_at_zero_bias_nm) as depletion_width_at_zero_bias_nm_max,
					MIN(depletion_width_at_zero_bias_nm) as depletion_width_at_zero_bias_nm_min,
					MAX(n_num_cm_cubed) as n_num_cm_cubed_max,
					MIN(n_num_cm_cubed) as n_num_cm_cubed_min,
					STDDEV_POP(depletion_width_at_zero_bias_nm) as depletion_width_at_zero_bias_nm_std,
					STDDEV_POP(n_num_cm_cubed) as n_num_cm_cubed_std,
					
					MAX(n_at_zero_bias_num_cm_cubed) as n_at_zero_bias_num_cm_cubed_max,
					MIN(n_at_zero_bias_num_cm_cubed) as n_at_zero_bias_num_cm_cubed_min,
					AVG(n_at_zero_bias_num_cm_cubed) as n_at_zero_bias_num_cm_cubed_avg,
					
					MAX(r_squared) as r_squared_max,
					MIN(r_squared) as r_squared_min,
					AVG(r_squared) as r_squared_avg
					
					
					
					from epi_cv_meas where cv_id = $cv_id";
					
					
	$result = $this->local_query($query);
	
	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	$row  =$result[0];
	
	$query = "update epi_cv set ";
	foreach($row as $k=>$v) {
		$q[]=" $k = '$v' ";
	
	}
	$query .= implode(',',$q);
	$query .=" where cv_id =$cv_id";
	$result = $this->local_insert($query);

	if ($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	return array('error'=>0);
}


function parse_cv($file,$data) {

	echo $file;
	// $this->echo_r($data);
	ini_set('auto_detect_line_endings',TRUE);
	
 	$handle = fopen($file, "r");
	
	if ($handle == NULL ){
		return array('error'=>3,'point'=>'File Read Error');
	}
	
	$header = array();
	
	$filedata=array();
	
	while(!feof($handle)) {
		$string = fgetcsv($handle);
		if (stristr($string[0],'date')) {
			//echo "<br> new record start</br>";
			if (count($filedata) > 0 ){
			//	echo "<br> Parsing old record </br>";
				$result = $this->parse_cv_data($filedata,$data);
				if ($result['error'] > 0 ) {
					if($result['error'] == 4) {
						echo "<br> WARNING: ".$result['point'];
					} else {
						return $result;
					}
				}
				$filedata=array();
			}
		}
		$filedata[]=$string;
	}
	
 	if (count($filedata) > 0 ){
	
		$result = $this->parse_cv_data($filedata,$data);
		if ($result['error'] > 0 ) {
					if($result['error'] == 4) {
						echo "<br> WARNING: ".$result['point'];
					} else {
						return $result;
					}
		}
				
	}
 
	return array('error'=>0,'data_id'=>$result['data_id']);
	
	
}
 
function cv_check_header($header) {
	if ($header['mask_name'] == NULL || $header['device_name'] == NULL || $header['die'] == NULL ) {
		return array('error'=>3,'point'=>'File Read Error : could not extract key parameters');
	}
	return array('error'=>0);
}
 

 
function parse_cv_data($filedata,$data) {
//	$this->echo_r($d);'

	$header = $this->cv_get_header($filedata);
	// $this->echo_r($header);

	// adjust the coords for the die to absolute values
	$header['x']=$header['x']+$header['x_coord'];
	$header['y']=$header['y']+$header['y_coord'];
	$header['wafer_id']= $data['sample_id'];
	$header['wafer_name'] = $data['sample'];
	$header['user_id']  = $this->check_user($data['user']);
	$header['file_name']=$data['file'];
	
	$result = $this->cv_check_header($header);
	if ($result['error'] > 0 ) {
		return $result;
	}

	$cv_id = $this->check_cv($header);

	if ($cv_id == NULL ){
		$result = $this->new_cv($header);
		if ($result['error'] > 0) {
			return $result;
		}
		$cv_id = $result['data_id'];
	}
	
	// echo "<br> adding data for cv id $cv_id";
	
	$dat= $this->cv_get_data($filedata);
	$columns = $dat['columns'];
	$data = $dat['data'];
	
	$cv_meas_id =  $this->check_cv_meas($cv_id,$header);
		
	if ($cv_meas_id == NULL ){
		$result = $this->new_cv_meas($cv_id,$header);
		if ($result['error'] > 0) {
			return $result;
		}
		$cv_meas_id = $result['data_id'];
	}
	
	// echo "<br> adding data for die # ".$header['die']." with id $cv_meas_id";
	
	if ($this->check_cv_meas_values($cv_meas_id)) {
		return array('error'=>4,'point'=>' CV data for die '.$header['die'].' exists for this data set');
	}
	
	foreach($data as $values) {
	
		$result = $this->new_cv_meas_values($cv_meas_id,$columns,$values);
	
		if ($result['error'] > 0 ) {
			return $result;
		}
	
	}
	
	$result = $this->cv_stats($cv_id);
	
	if ($result['error'] > 0 ) { 
		return $result;
	}
	
	return array('error'=>0,'data_id'=>$cv_id);
}
  
  //===================================//
  
  
  //====== SIMS data upload ===========//
  
  // for now sims displays the structure layers and then allows the elements to be entered.

  function sims_vendor_list(){
  	return array('EAG','Qspec');
  }
    
  function sims_vendor_list_options($selected=NULL){
  	$vendors = $this->sims_vendor_list();
  	$o=array();
  	foreach($vendors as $v){
  
  	//	$o[]='<option '.($v == $selected? 'SELECTED' : '').'>'.$v.'</option>';
  	//	$o[]='<option value = "'.$v.'">'.$v.'</option>';
  		
  		$o[]='<option  value = "'.$v.'" '.($v == $selected? ' SELECTED ' : '').'>'.$v.'</option>';

  		
  	}
  	return implode(',',$o);
  }
  
  
  function get_sims_data($sims_id) {
  	$query = "select * from epi_sims where sims_id = $sims_id";
  	
  	$result = $this->local_query($query);
  
  	if ($result == NULL ){
  		return NULL;
  	}
	return $result[0];
	  
  }
  
  function get_sims_meas_data($sims_id) {
  	$query = "select * from epi_sims_meas where sims_id = $sims_id";
  	
  	$result = $this->local_query($query);
  
  	if ($result == NULL ){
  		return NULL;
  	}
	return $result;
	  
  }
  
  function get_sims_wafer_id($sims_id) {
  
  	$query = "select wafer_id from epi_sims where sims_id = $sims_id";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	return $result[0]['wafer_id'];
  }
  
  function get_structure_by_wafer($wafer_name)
  {
  	$query = "SELECT w.wafer_id, s.structure_id, s.structure_name ".
                 "FROM epi_structure s, epi_wafer w, epi_growth g ".
                 "WHERE w.wafer_name = '$wafer_name' ".
  		   "AND g.growth_id = w.growth_id ".
  		   "AND s.structure_id = g.structure_id ";

        // echo "<p>query: $query</p><br>";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	
        $arr = array('wafer_id' => $result[0]['wafer_id'],
                     'structure_id' => $result[0]['structure_id'],
                     'structure_name' => $result[0]['structure_name']);
       // $this->echo_r($arr);

       return $arr;
  }
  
  
  function get_structure_for_wafer($wafer_id) {
  	$query = "select s.* from epi_structure s, epi_wafer w, epi_growth g 
  						where w.wafer_id = $wafer_id
  						and g.growth_id = w.growth_id
  						and s.structure_id =g.structure_id
  						";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	
  	return array('structure_id'=>$result[0]['structure_id'],
  				'structure_name'=>$result[0]['structure_name']);
  
  }
  
  
  function check_sims_layers($sims_id) {

  	$query = "select count(*) as count from epi_sims_meas where sims_id = $sims_id";
  	$result = $this->local_query($query);
  	if($result == NULL) {
  		return NULL;
  	}
  	return ($result[0]['count'] > 0);
  
  }
 
  
  function add_sims_layers($wafer_id,$sims_id){
  	$query = "select l.*,s.structure_name from epi_structure_layers l, epi_growth g, epi_structure s, epi_wafer w 
  				where w.wafer_id = $wafer_id and
  				w.growth_id  = g.growth_id and
  				g.structure_id  = s.structure_id and
  				l.structure_id  = s.structure_id 
  				order by l.layer_number desc
  			";
  	//echo "<br> $query <br>";
  	$result = $this->local_query($query);
  	if ($result == NULL ){
  		echo "<br> ERROR: Could not find structure information <br> $query <br>";
  	}
//	$this->echo_r($result);  
  	
  	$wafer_name = $this->get_wafer_name_by_id($wafer_id);	
  
//  	echo "<br> wafer name $wafer_name";
  
  	foreach ($result as $r){
  	
  		$structure_name =$r['structure_name'];
  		$structure_layer_id  = $r['structure_layer_id'];
  		$layer_number  = $r['layer_number'];
  		$material = (strtoupper(trim($r['doped'])) != 'NA'?trim($r['doped']).'-':'').trim($r['material_name']).(strtoupper(trim($r['dopant_name']))!='NA'?':'.trim($r['dopant_name']):'').(trim($r['layer_function']) != ''?'('.trim($r['layer_function']).')':'');
//  		echo "<br> $layer_number ===== $material";
  		
  		$query2 = "insert into epi_sims_meas set 
  					sims_id  = '$sims_id',
  					wafer_id = '$wafer_id',
  					wafer_name = '$wafer_name',
  					structure_name = '$structure_name',
  					structure_layer_id = '$structure_layer_id',
  					layer_number = '$layer_number',
  					material_name='$material'
  				";
 // 		echo "<br> $query2";
  		$result2 = $this->local_insert($query2);
  		
  		
  	}
  }
 
   function sims_summary($sims_id) {
  	$sims_data = $this->get_sims_data($sims_id);
 	//$this->echo_r($ecv_data); 
  	
  	echo "<br><br><table border=0 cellpadding=0 cellspacing=0 width=400>
 			<tr >
			  <td colspan=2 class=simple style='background-color:yellow;'> SIMS Measurement Summary </td>
  			</tr>
  			<tr>
  			<td class=simple> Wafer:
  			</td>
  			<td class=simple_header>".$sims_data['wafer_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Date:
  			</td>
  			<td class=simple_header>".$sims_data['date']."
  			</td>
  			</tr>
  			
  			<td class=simple> Structure:
  			</td>
  			<td class=simple_header>".$sims_data['structure_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Vendor:
  			</td>
  			<td class=simple_header>".$sims_data['vendor_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Comment:
  			</td>
  			<td class=simple_header>".$sims_data['comment']."
  			</td>
  			</tr>
  			
  			";
  	echo "</table>";
  			
  }

  
  
  function upload_sims($wafer_id,$sims_id){
  		
  		$sims_data = $this->get_sims_data($sims_id);
  		
  		if ($sims_data == NULL) {
  			echo "<br> ERROR: No SIMS entry found. Please follow this link to add data:";
  			echo '<br> <a style="color:blue" href="'.SJ_HOST_NAME.'epi/edit/epi_sims.php?PME_sys_operation=Add">Add SIMS Data</a>';
			return;
  		}
  		
  		if (!$this->check_sims_layers($sims_id)) {
	  		$this->add_sims_layers($wafer_id,$sims_id);
		}
  		
  		//$this->echo_r($sims_data);
  		
  		$sims_meas_data = $this->get_sims_meas_data($sims_id);
  		//$this->echo_r($sims_meas_data);

  		echo "<br><br><table border=0 cellpadding=0 cellspacing=0 width=1800>
 			 <tr >
			  <td >&nbsp;</td>
			  <td >&nbsp;</td>
			  <td colspan=4 class=simple style='background-color:yellow;'>Mole fraction (Click on any cell to add data)</td>
			  <td colspan=14 class=simple style='background-color:#00FF11;'>Atomic concentration (Click on any cell to add data)</td>
			  <td colspan=2 class=simple style='background-color:#FF8181;'>Measurement Information</td>
			 </tr>
			 <tr>
			  <td  class=simple_header >Layer</td>
			  <td class=simple_header >Material</td>
			  <td class=simple_header>In low</td>
			  <td class=simple_header>In high</td>
			  <td class=simple_header>Al low</td>
			  <td class=simple_header>Al high</td>
			  <td class=simple_header>N</td>
			  <td class=simple_header>Sb</td>
			  <td class=simple_header>Si</td>
			  <td class=simple_header>Be</td>
			  <td class=simple_header>Zn</td>
			  <td class=simple_header>C</td>
			  <td class=simple_header>O</td>
			  <td class=simple_header>H</td>
			  <td class=simple_header>Cu</td>
			  <td class=simple_header>Au</td>
			  <td class=simple_header>Ag</td>
			  <td class=simple_header>F</td>
			  <td class=simple_header>B</td>
			  <td class=simple_header>Ta</td>
			  <td class=simple_header>Cr</td>
			  <td class=simple_header>Fe</td>
			  <td class=simple_header>Vendor</td> 
			  <td class=simple_header>Comment</td> 
			 </tr>";
			foreach ($sims_meas_data as $c) {
				echo " <tr>
				  <td class=simple>".$c['layer_number']."</td>
				  <td class=simple>".$c['material_name']."</td>
				  ";
				 
				 $sims_id = $c['sims_id'];
				 $sims_meas_id = $c['sims_meas_id'];
				 $wafer_id = $c['wafer_id'];
				 $wafer_name = $c['wafer_name'];
				 $structure_name = $c['structure_name'];
				 $structure_layer_id = $c['structure_layer_id'];
				 $vendor_name = $c['vendor_name'];
				 $comment = $c['comment'];
				 
				 unset($c['layer_number']); 
				 unset($c['material_name']); 
				 unset($c['sims_id']); 
				 unset($c['sims_meas_id']); 
				 unset($c['wafer_id']); 
				 unset($c['wafer_name']); 
				 unset($c['structure_name']); 
				 unset($c['structure_layer_id']);
				 unset($c['vendor_name']); 
				 unset($c['comment']);
				 foreach ($c as $k=>$v) {
				 	 echo "<td class=simple><input type='text' style='border:0;'  size=5 name='".$k.'-'.$sims_meas_id."' value='".$v."'></td>";
				 }
				  echo "<td class=simple><select name='vendor_name-".$sims_meas_id."'>".$this->sims_vendor_list_options($vendor_name)."</select></td>";
				  echo "<td class=simple><input style='border:0;' type='text' size=20 name='comment-".$sims_meas_id."' value='".$comment."'></td>";
				 echo "</tr>";			
			
			
			}
			 
			 
			echo "</table>";
  
  }


  function upload_sims_save($post){
  		
  		$wafer_id = $post['wafer_id'];
  		$sims_id  = $post['sims_id'];
  		$submit = $post['submit'];
  		unset($post['wafer_id']);
  		unset($post['sims_id']);
  		unset($post['submit']);
  		  		
  		$sims_meas_data = $this->get_sims_meas_data($sims_id);
  		//$this->echo_r($sims_meas_data);


			foreach ($sims_meas_data as $c) {

				 
				 $sims_id = $c['sims_id'];
				 $sims_meas_id = $c['sims_meas_id'];
				 $wafer_id = $c['wafer_id'];
				 $wafer_name = $c['wafer_name'];
				 $structure_name = $c['structure_name'];
				 $structure_layer_id = $c['structure_layer_id'];
				
				 
				 unset($c['layer_number']); 
				 unset($c['material_name']); 
				 unset($c['sims_id']); 
				 unset($c['sims_meas_id']); 
				 unset($c['wafer_id']); 
				 unset($c['wafer_name']); 
				 unset($c['structure_name']); 
				 unset($c['structure_layer_id']);
			
				 
				 $query = "update epi_sims_meas set ";
				 
				 foreach ($c as $k=>$v) {
					if (isset($post[$k.'-'.$sims_meas_id])) {
						$q[]="$k = '".addslashes($post[$k.'-'.$sims_meas_id])."'";
					}

				 }
				 $query .= implode(' , ',$q);
				 $query .="where sims_id = $sims_id and sims_meas_id = $sims_meas_id";	
				 $result = $this->local_insert($query);
				 if ($result == NULL) {
				 	echo "<br> ERROR: Could not update values <br> $query <br>";
				 
				 }	
				//echo "<br> $query <br>";
			}
  
  }
  
  //====================================//
  //============= XRD Data =============//
  
  function add_xrd_measurements($data)
  {
    $err_msg = "";
    if (empty($data['wafer_name'])) {
      $err_msg .= "<br><p> - invalid wafer name</p>";
    }

    if (strcmp($data['user'], 'SELECT') == 0) {
      $err_msg .= "<br><p> - invalid user name</p> ";
    }

    if(empty($data['date'])) {
      $err_msg .= "<br><p> - invalid date</p>";
    }

    foreach($data['layer'] as $l) {
      $layer_err_msg = "";
      if(!is_numeric(trim($l['number']))) { $layer_err_msg .= "<br> - invalid layer number"; }
      //if(!is_numeric(trim($l['pintensity']))) { $layer_err_msg .= "<br> - invalid peak intensity"; }
      //if(!is_numeric(trim($l['fwhm']))) { $layer_err_msg .= "<br> - invalid fwhm"; }
      if ($l['number'] == 4) {
        if(!is_numeric(trim($l['lthickness']))) { $layer_err_msg .= "<br> - invalid lthickness"; }
      }
      //if(!is_numeric(trim($l['pseparation'])) && ($l['number'] != 0)) { $layer_err_msg .= "<br> - invalid peak separation"; }
      if (!empty($layer_err_msg)) { $err_msg .= $layer_err_msg . " <br> in layer number ".$l['number']." (".trim($l['material']).")<br>";}
    }

    if (!empty($err_msg)) {
      $err_msg = "Input errors:<br><p style='padding-left:20px;'>$err_msg</p><br>Please provide valid inputs and try again ...";
      return $err_msg;
    }

    $wafer_name = trim($data['wafer_name']);
    $structure = array();
    $structure = $this->get_structure_by_wafer($wafer_name);
    $wafer_name = addslashes(trim($data['wafer_name']));
    if ($structure == NULL) {
      $err_msg = "<br>No structure found for this wafer.";
      return $err_msg;
    }

    $user_name = addslashes(trim($data['user']));
    $wafer_id = $structure['wafer_id'];
    $structure_id = $structure['structure_id'];
    $structure_name = addslashes($structure['structure_name']);
    $date = trim($data['date']);
    $annealed = $data['annealed'];

    $query = "INSERT INTO epi_xrd (user_name, wafer_id, wafer_name, structure_id, structure_name, date, annealed) ".
             "VALUES ('$user_name', '$wafer_id', '$wafer_name', '$structure_id', '$structure_name', '$date', '$annealed');";
    //echo "<br><p> Query: $query</p><br>";
    $result = $this->local_insert($query);
    //echo "Result: $result<p>";

    $query = "SELECT LAST_INSERT_ID() AS xrd_id";
    $result = $this->local_query($query);
    $xrd_id = $result[0]['xrd_id'];

    $layers = $data['layer'];
    foreach($layers as $l) {
	if ($l['pseparation'] == "") {
		$l['in'] = "";
	}
	else {
		switch($l['number']) {
			case "0": $l['in'] = (float)($l['pseparation'] * 0.0); break;
			case "1": $l['in'] = (float)($l['pseparation'] * (-0.0054139)) + 8.7846; break;
			case "2": $l['in'] = (float)($l['pseparation'] * (-0.0054225)) + 8.2414; break;
			case "3": $l['in'] = (float)($l['pseparation'] * (-0.005059)) + 48.611; break;
			case "4": $l['in'] = (float)($l['pseparation'] * (-0.005046)) + 47.653; break;
			case "5": $l['in'] = (float)($l['pseparation'] * (-0.010295)) - 1.569; break;
			default: break;
      		}
        }

        $query  = "INSERT INTO epi_xrd_meas (xrd_id, layer_number, material_name, thickness_nm_, peak_separation_arcsec_, composition_x, FWHM_deg_, intensity_cps_, comment) ";
        $query .= " VALUES ('".$xrd_id."', '".$l['number']."', '".$l['material']."',";
	if ($l['number'] != "4") {
	  $query .= " NULL,";
	}
	else {
	  $query .= " '".$l['lthickness']."',";
	}

	if ($l['pseparation'] == "") {
	  $query .= " NULL,";
	  $query .= " NULL,";
	}
	else {
	  $query .= " '".$l['pseparation']."',";
	  $query .= " '".round($l['in'],2)."',";
	}

	if ($l['fwhm'] == "") {
	  $query .= " NULL,";
	}
	else {
	  $query .= " '".$l['fwhm']."',";
	}
	if ($l['pintensity'] == "") {
	  $query .= " NULL,";
	}
	else {
	  $query .= " '".$l['pintensity']."',";
	}
        $query .= " '".$l['comment']."'); ";

        //echo "<br><p>Q3 Query: $query</p><br>";
        $result = $this->local_insert($query);
        //echo "Q3: Result: $result<p>";
    }
    return $err_msg;
  }

  function delete_xrd_meas($data)
  {
	//$this->echo_r($data);
	$result = array();
	$query = "DELETE FROM epi_xrd_meas WHERE xrd_id = '".$data['xrd_id']."'";
	$result[] = $this->local_insert($query);

	$query = "DELETE FROM epi_xrd WHERE xrd_id = '".$data['xrd_id']."'";
	$result[]= $this->local_insert($query);

	return $result;
}

  function update_xrd_meas2($data)
  {
	//$this->echo_r($data);
	$layers = $data['layer'];
	$results = array();
	$query = "";
	foreach($layers as $l) {
		if ($l['pseparation'] == "") {
			$l['in'] = "";
		}
		else {
			switch($l['number']) {
				case "0": $l['in'] = (float)($l['pseparation'] * 0.0); break;
				case "1": $l['in'] = (float)($l['pseparation'] * (-0.0054139)) + 8.7846; break;
				case "2": $l['in'] = (float)($l['pseparation'] * (-0.0054225)) + 8.2414; break;
				case "3": $l['in'] = (float)($l['pseparation'] * (-0.005059)) + 48.611; break;
				case "4": $l['in'] = (float)($l['pseparation'] * (-0.005046)) + 47.653; break;
				case "5": $l['in'] = (float)($l['pseparation'] * (-0.010295)) - 1.569; break;
				default: break;
			}
		}

		$query  = "UPDATE epi_xrd_meas ";
		$query .= "SET ";
		$query .=	"layer_number = '".$l['number']."', ";
		$query .=	"material_name = '".$l['material']."', ";

		if ($l['pintensity'] == "") {
		  $query .=	"intensity_cps_ = NULL, ";
		}
		else {
		  $query .=	"intensity_cps_ = '".$l['pintensity']."', ";
		}

		if ($l['fwhm'] == "") {
		  $query .=	"FWHM_deg_ = NULL, ";
		}
		else {
		  $query .=	"FWHM_deg_ = '".$l['fwhm']."', ";
		}

		if ($l['pseparation'] == "") {
		  $query .=     "peak_separation_arcsec_ = NULL, ";
		  $query .=	"composition_x = NULL, ";
		}
		else {
		  $query .=     "peak_separation_arcsec_ = '".$l['pseparation']."', ";
		  $query .=	"composition_x = '".$l['in']."', ";
		}

		if ($l['number'] != "4") {
		  $query .=	"thickness_nm_ = NULL, ";
		}
		else {
		  $query .=	"thickness_nm_ = '".$l['lthickness']."', ";
		}
		$query .=	"comment = '".$l['comment']."' ";
		$query .= "WHERE xrd_meas_id = '".$l['xrd_meas_id']."'; ";

		//echo "Query1: <p>$query</p>";
		$result = $this->local_insert($query);
		//echo "Result1: <p>$result</p>";

		$results[] = $result;
	}

	$query = "UPDATE epi_xrd ".
		  "SET ".
			"user_name = '".$data['user']."', ".
			"annealed = '".$data['annealed']."', ".
			"date = '".$data['date']."' ".
			"WHERE xrd_id = '".$data['xrd_id']."'";
	//echo "Query2: <p>$query</p>";
	$result = $this->local_insert($query);
	//echo "Result2: <p>$result</p>";

	$results[] = $result;

	return $results;
}

  function update_xrd_meas($data)
  {
    $results = array();
    $query1 = "UPDATE epi_xrd_meas ".
              "SET ".
                "layer_number = '".$data['layer']."', ".
                "material_name = '".$data['material']."', ".
                "intensity_cps_ = '".$data['pintensity']."', ".
                "FWHM_deg_ = '".$data['fwhm']."', ".
                "peak_separation_arcsec_ = '".$data['pseparation']."', ".
                "composition_x = '".$data['in']."', ".
                "thickness_nm_ = '".$data['thickness']."', ".
                "comment = '".$data['comment']."' ".
              "WHERE xrd_meas_id = '".$data['xrd_meas_id']."'";
    $result1 = $this->local_insert($query1);
    $results[] = $result1;

    $query2 = "UPDATE epi_xrd ".
              "SET ".
                "user_name = '".$data['user']."', ".
                "annealed = '".$data['annealed']."', ".
                "date = '".$data['date']."' ".
              "WHERE xrd_id = '".$data['xrd_id']."'";
    $result2 = $this->local_insert($query2);
    $results[] = $result2;

    return $results;
  }

  function get_xrd_meas_data2($xrd_meas_id)
  {
    $query = "SELECT * FROM epi_xrd_meas_view WHERE xrd_id IN (SELECT xrd_id FROM epi_xrd_meas_view WHERE xrd_meas_id = '$xrd_meas_id') ORDER BY layer_number ASC";
    $result = $this->local_query($query);
    return $result;
  }

  function get_xrd_meas_data($xrd_meas_id)
  {
    $query = "SELECT * FROM epi_xrd_meas_view WHERE xrd_meas_id = '$xrd_meas_id'";
    $result = $this->local_query($query);
    return $result[0];
  }

  function get_xrd_wafer_id($xrd_id)
  {
  	$query = "SELECT wafer_id FROM epi_xrd WHERE xrd_id = $xrd_id";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	return $result[0]['wafer_id'];
  }

  function get_xrd_wafer($xrd_id) {
  
  	$query = "select wafer_name from epi_xrd where xrd_id = $xrd_id";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	return $result[0]['wafer_name'];
  }
   
   function xrd_summary($xrd_id) {
  	$xrd_data = $this->get_xrd_data($xrd_id);
 	//$this->echo_r($ecv_data); 
  	
  	echo "<br><br><table border=0 cellpadding=0 cellspacing=0 width=400>
 			 <tr >
			 
			 
			  <td colspan=2 class=simple style='background-color:yellow;'> ECV Measurement Summary </td>
  			</tr>
  			<tr>
  			<td class=simple> Wafer:
  			</td>
  			<td class=simple_header>".$xrd_data['wafer_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Date:
  			</td>
  			<td class=simple_header>".$xrd_data['date']."
  			</td>
  			</tr>
  			
  			<td class=simple> Structure:
  			</td>
  			<td class=simple_header>".$xrd_data['structure_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Comment:
  			</td>
  			<td class=simple_header>".$xrd_data['comment']."
  			</td>
  			</tr>
  			
  			";
  	echo "</table>";
  			
  }
  
  
  
  //==============
  
  
  //============= ECV DATA ========//
  
  
  function get_ecv_measurement_type(){
  
  	return array(
  				'GaAs p (Tartrate)',
  				'GaAs n (Tartrate)',
  				'GaInNAs p (Tartrate)',
  				'GaInNAs n (Tartrate)',
  				'GaInNAs p (Tiron)',
  				'GaInNAs n (Tiron)',
  				'GaAs pn (Tartrate)',
  				'GaAs np (Tartrate)',
  				'GaAs p (Tiron)',
  				'GaAs n (Tiron)',
  				'GaAs np (Tiron)',
  				'GaAs pn (Tiron)',
  				'GaAs p (EDTA)',
  				'GaAs n (EDTA)',
  				'GaAs pn (EDTA)',
  				'GaAs np (EDTA)'
  				);
  				
  
  
  }
  
  function ecv_dope_list(){
  	return array('P','N','P+','N+');
  }
    
  function ecv_dope_list_options($selected=NULL){
  	$doped = $this->ecv_dope_list();
  	$o=array();
  	foreach($doped as $v){
  
  		$o[]='<option  value = "'.$v.'" '.($v == $selected? ' SELECTED ' : '').'>'.$v.'</option>';
  		//$o[]='<option value = "'.$v.'">'.$v.'</option>';
  	}
  	return implode(',',$o);
  }
  
  
   function get_ecv_data($ecv_id) {
  	$query = "select * from epi_ecv where ecv_id = $ecv_id";
  	
  	$result = $this->local_query($query);
  
  	if ($result == NULL ){
  		return NULL;
  	}
	return $result[0];
	  
  }
  
  function get_ecv_meas_data($ecv_id) {
  	$query = "select * from epi_ecv_meas where ecv_id = $ecv_id";
  	
  	$result = $this->local_query($query);
  
  	if ($result == NULL ){
  		return NULL;
  	}
	return $result;
	  
  }

    function get_ecv_wafer_id($ecv_id) {
  
  	$query = "select wafer_id from epi_ecv where ecv_id = $ecv_id";
  	$result = $this->local_query($query);
  	if ($result == NULL) {
  		return NULL;
  	}
  	return $result[0]['wafer_id'];
  }
  
   
  function check_ecv_layers($ecv_id) {

  	$query = "select count(*) as count from epi_ecv_meas where ecv_id = $ecv_id";
  	$result = $this->local_query($query);
  	if($result == NULL) {
  		return NULL;
  	}
  	return ($result[0]['count'] > 0);
  
  }
 
  
  function add_ecv_layers($wafer_id,$ecv_id){
  	$query = "select l.*,s.structure_name from epi_structure_layers l, epi_growth g, epi_structure s, epi_wafer w 
  				where w.wafer_id = $wafer_id and
  				w.growth_id  = g.growth_id and
  				g.structure_id  = s.structure_id and
  				l.structure_id  = s.structure_id 
  				order by l.layer_number desc
  			";
  	
  	$result = $this->local_query($query);
  	
  	if ($result == NULL ){
  		echo "<br> ERROR: Could not find structure information <br> $query <br>";
  	}
  	
  	$wafer_name = $this->get_wafer_name_by_id($wafer_id);	
  
  
  	foreach ($result as $r){
  	
  		$structure_name =$r['structure_name'];
  		$structure_layer_id  = $r['structure_layer_id'];
  		$layer_number  = $r['layer_number'];
  		$material = (strtoupper(trim($r['doped'])) != 'NA'?trim($r['doped']).'-':'').trim($r['material_name']).(strtoupper(trim($r['dopant_name']))!='NA'?':'.trim($r['dopant_name']):'').(trim($r['layer_function']) != ''?'('.trim($r['layer_function']).')':'');
  		
  		$query2 = "insert into epi_ecv_meas set 
  					ecv_id  = '$ecv_id',
  					wafer_id = '$wafer_id',
  					wafer_name = '$wafer_name',
  					structure_name = '$structure_name',
  					structure_layer_id = '$structure_layer_id',
  					layer_number = '$layer_number',
  					material_name='$material'
  				";
 
  		$result2 = $this->local_insert($query2);
  		
  		
  	}
  }
 
 
  function ecv_summary($ecv_id) {
  	$ecv_data = $this->get_ecv_data($ecv_id);
 	//$this->echo_r($ecv_data); 
  	
  	echo "<br><br><table border=0 cellpadding=0 cellspacing=0 width=400>
 			 <tr >
			 
			 
			  <td colspan=2 class=simple style='background-color:yellow;'> ECV Measurement Summary </td>
  			</tr>
  			<tr>
  			<td class=simple> Wafer:
  			</td>
  			<td class=simple_header>".$ecv_data['wafer_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Date:
  			</td>
  			<td class=simple_header>".$ecv_data['date']."
  			</td>
  			</tr>
  			
  			<td class=simple> Structure:
  			</td>
  			<td class=simple_header>".$ecv_data['structure_name']."
  			</td>
  			</tr>
  			
  			<td class=simple> Measurement Type:
  			</td>
  			<td class=simple_header>".$ecv_data['measurement_type']."
  			</td>
  			</tr>
  			
  			<td class=simple> Comment:
  			</td>
  			<td class=simple_header>".$ecv_data['comment']."
  			</td>
  			</tr>
  			
  			";
  	echo "</table>";
  			
  }
  
  
  
  function upload_ecv($wafer_id,$ecv_id){
  		
  		$ecv_data = $this->get_ecv_data($ecv_id);
  		
  		if ($ecv_data == NULL) {
  			echo "<br> ERROR: No ECV entry found. Please follow this link to add data:";
  			echo '<br> <a style="color:blue" href="'.SJ_HOST_NAME.'epi/edit/epi_ecv.php?PME_sys_operation=Add">Add ECV Data</a>';
			return;
  		}
  		
  		if (!$this->check_ecv_layers($ecv_id)) {
	  		$this->add_ecv_layers($wafer_id,$ecv_id);
		}
  		
  		//$this->echo_r($sims_data);
  		
  		$ecv_meas_data = $this->get_ecv_meas_data($ecv_id);
  		//$this->echo_r($sims_meas_data);

  		echo "<br><br><table border=0 cellpadding=0 cellspacing=0 width=800>
 			 <tr >
			  <td >&nbsp;</td>
			  <td >&nbsp;</td>
			  <td colspan=4 class=simple style='background-color:yellow;'> ECV Data (Click on any cell to edit) </td>
			 </tr>
			 <tr>
			  <td  class=simple_header >Layer</td>
			  <td class=simple_header >Material</td>
			  <td class=simple_header>Doped</td>
			  <td class=simple_header>Doping level (1/cm^3)</td>
			  <td class=simple_header>Doping level error (1/cm^3)</td>
			  <td class=simple_header>Comment</td> 
			  <td class=simple_header>Flag For Study</td> 
			 </tr>";
			foreach ($ecv_meas_data as $c) {
				echo " <tr>
				  <td class=simple>".$c['layer_number']."</td>
				  <td class=simple>".$c['material_name']."</td>
				  ";
				 
				 $ecv_id = $c['ecv_id'];
				 $ecv_meas_id = $c['ecv_meas_id'];
				 $wafer_id = $c['wafer_id'];
				 $wafer_name = $c['wafer_name'];
				 $structure_name = $c['structure_name'];
				 $structure_layer_id = $c['structure_layer_id'];
				 $comment = $c['comment'];
				 
				 unset($c['layer_number']); 
				 unset($c['material_name']); 
				 unset($c['ecv_id']); 
				 unset($c['ecv_meas_id']); 
				 unset($c['wafer_id']); 
				 unset($c['wafer_name']); 
				 unset($c['structure_name']); 
				 unset($c['structure_layer_id']);
				
				 foreach ($c as $k=>$v) {
				 	 if ($k == 'doping_type') {
				 	 	echo "<td class=simple><select name='".$k."-".$ecv_meas_id."'>".$this->ecv_dope_list_options($v)."</select></td>";
				 	 } elseif ($k == 'comment') {
				 	 	echo "<td class=simple><input style='border:0;' type='text' size=20 name='".$k."-".$ecv_meas_id."' value='".$v."'></td>";
				 	 } elseif ($k == 'flag_for_study'){
				 	 	echo "<td class=simple><input type=radio name=".$k." value=".$ecv_meas_id." ".($v==1?' CHECKED ':'')." </td>";
				 	 } else {
					 	 echo "<td class=simple><input type='text' style='border:0;'  size=5 name='".$k.'-'.$ecv_meas_id."' value='".$v."'></td>";
					 }
				 }
				  
				  
				 echo "</tr>";			
			
			
			}
			 
			 
			echo "</table>";
  
  }


  function upload_ecv_save($post){
  		
  		$wafer_id = $post['wafer_id'];
  		$ecv_id  = $post['ecv_id'];
  		$submit = $post['submit'];
  		unset($post['wafer_id']);
  		unset($post['ecv_id']);
  		unset($post['submit']);
  		  		
  		if(isset($post['flag_for_study']) && $post['flag_for_study'] > 0) {
  			$query = "update epi_ecv_meas set flag_for_study = 0 where ecv_id = $ecv_id";
  			$result = $this->local_insert($query);
  			
  			$query = "update epi_ecv_meas set flag_for_study=1 where ecv_id = $ecv_id and ecv_meas_id = ".$post['flag_for_study'];
  			$result = $this->local_insert($query);
  		
  		}  		
  		  		
  		$ecv_meas_data = $this->get_ecv_meas_data($ecv_id);
  		//$this->echo_r($ecv_meas_data);


			foreach ($ecv_meas_data as $c) {

				 
				 $ecv_id = $c['ecv_id'];
				 $ecv_meas_id = $c['ecv_meas_id'];
				 $wafer_id = $c['wafer_id'];
				 $wafer_name = $c['wafer_name'];
				 $structure_name = $c['structure_name'];
				 $structure_layer_id = $c['structure_layer_id'];
				
				 
				 unset($c['layer_number']); 
				 unset($c['material_name']); 
				 unset($c['ecv_id']); 
				 unset($c['ecv_meas_id']); 
				 unset($c['wafer_id']); 
				 unset($c['wafer_name']); 
				 unset($c['structure_name']); 
				 unset($c['structure_layer_id']);
				 unset($c['flag_for_study']);
			
				 
				 $query = "update epi_ecv_meas set ";
				 
				 foreach ($c as $k=>$v) {
					if (isset($post[$k.'-'.$ecv_meas_id])) {
						$q[]="$k = '".addslashes($post[$k.'-'.$ecv_meas_id])."'";
					}

				 }
				 $query .= implode(' , ',$q);
				 $query .="where ecv_id = $ecv_id and ecv_meas_id = $ecv_meas_id";	
				 //echo "<br> $query <br>";

				 $result = $this->local_insert($query);
				 if ($result == NULL) {
				 	echo "<br> ERROR: Could not update values <br> $query <br>";
				 
				 }	
				//echo "<br> $query <br>";
			}
			 
			 
  
  }

  
  
  
  //===============================//


  //===================================//
 
 function simple_sanitize_file($fileN){
 
 			$fileN  = str_replace( " ", "_", $fileN  );
			$fileN  = str_replace( "\\", "", $fileN  );
			$fileN  = str_replace( "%20", "_", $fileN  );
			$fileN  = str_replace(';',"",$fileN);
			$fileN  = str_replace(',',"",$fileN);
			$fileN  = str_replace('/',"",$fileN);
			$fileN  = str_replace('<',"",$fileN);
			$fileN  = str_replace('>',"",$fileN);
			$fileN  = str_replace('{',"",$fileN);
			$fileN  = str_replace('}',"",$fileN);
			$fileN  = str_replace('(',"",$fileN);
			$fileN  = str_replace(')',"",$fileN);
			$fileN  = str_replace('"',"",$fileN);
			$fileN  = str_replace("'","",$fileN);
			$fileN  = str_replace("=","",$fileN);
	
	return $fileN;
 
 
 }
 
 // proess stuff
 

function get_process_by_lot($process_lot_id) {
 	$query = "select process_id from epi_process_lot where process_lot_id = $process_lot_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return 0;
 	}
 	return $result[0]['process_id'];
}

function get_process_name_by_lot($process_lot_id) {
 	$query = "select process_name from epi_process_lot where process_lot_id = $process_lot_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return 0;
 	}
 	return $result[0]['process_name'];
}

function remove_attachment_from_device_mask($attachment_id)
{
  $query = "DELETE FROM epi_device_mask_attachment ".
           "WHERE id = '$attachment_id'";

  $result = $this->local_insert($query);
  return $result;
}

function remove_attachment_from_study($attachment_id)
{
  $query = "DELETE FROM epi_study_attachment ".
           "WHERE id = '$attachment_id'";

  $result = $this->local_insert($query);
  return $result;
}

function remove_wafer_from_study($study_id, $wafer_id)
{
  $query = "DELETE FROM epi_study_wafer ".
           "WHERE study_id = '$study_id' AND wafer_id = '$wafer_id'";

 	$result = $this->local_insert($query);
  return $result;
}

function upload_attachment($destination_dir, $ref = -1)
{
    if ($_FILES['attachment']['error'] > 0) {
      echo "Problem: ";
      switch ($_FILES['attachment']['error']) {
        case 1: echo "File exceeded upload max file size. "; break;
        case 2: echo "File exceeded max file size. "; break;
        case 3: echo "File only partially uploaded. "; break;
        case 4: echo "No file uploaded. "; break;
      }
      exit();
    }

/*
    if ($_FILES['attachment']['type'] != 'text/plain') {
      echo "Problem: File type is not plain text. ";
      exit();
    }
*/

    $study = "";
    if ($ref > 0) {
      $study = "S$ref-";
    } 
    //$upload_file = "../" . $destination_dir ."S$ref-". $_FILES['attachment']['name']; 
    $upload_file = "../" . $destination_dir .$study. $_FILES['attachment']['name']; 
    if (is_uploaded_file($_FILES['attachment']['tmp_name'])) {
      if (!move_uploaded_file($_FILES['attachment']['tmp_name'], $upload_file)) {
        echo "Problem: Could not move file: $upload_file to destination directory. ";
        exit();
      }
    }
    else {
      echo "Problem: Possible file upload attack. ";
      echo $_FILES['attachment']['name'];
      exit();
    }

    //return "S$ref-".$_FILES['attachment']['name'];
    return $study.$_FILES['attachment']['name'];
}

function add_device_mask_attachment($name, $description, $author, $date, $url, $device_mask_id)
{
  $query = "INSERT INTO epi_device_mask_attachment (name, description, url, author, date, epi_device_mask_id) ".
           "VALUES ('$name', '$description', '$url', '$author', '$date', '$device_mask_id')";

  // echo "<p>query: $query</p>";
  $result = $this->local_insert($query);

  $errmsg = "";
  if ($result == NULL) {
     switch ($errno) {
       default: $errmsg = "FILE ALREADY ATTACHED!"; break;
     } 
     $errmsg = "<p style=\"padding: 2px 5px; border: 1px solid red; color:red\"><span> $errmsg</span></p>";
  }	

  return $errmsg;
}

function increment_device_mask_attachment_count($device_mask_id)
{
  $query = "UPDATE epi_device_mask SET `attachment_count` = `attachment_count` + 1 WHERE device_mask_id = '$device_mask_id'";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update device mask attachment count!"; break;
     } 
  }	
  return $errmsg;
}

function decrement_device_mask_attachment_count($device_mask_id)
{
  $query = "UPDATE epi_device_mask SET `attachment_count` = `attachment_count` - 1 WHERE device_mask_id = '$device_mask_id' AND attachment_count > 0";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update device mask attachment count!"; break;
     } 
  }	
  return $errmsg;
}

function add_study_attachment($name, $description, $author, $date, $url, $study_id)
{
  $query = "INSERT INTO epi_study_attachment (name, description, url, author, date, epi_study_id) ".
           "VALUES ('$name', '$description', '$url', '$author', '$date', '$study_id')";

  $result = $this->local_insert($query);

  $errmsg = "";
  if ($result == NULL) {
     switch ($errno) {
       default: $errmsg = "File already attached!"; break;
     } 
     $errmsg = "<p style=\"padding: 2px 5px; border: 1px solid red; color:red\"><span> $errmsg</span></p>";
  }	

  return $errmsg;
}

function increment_study_wafer_count($study_id, $value)
{
  $query = "UPDATE epi_study SET `wafer_count` = `wafer_count` + $value WHERE id = '$study_id'";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update study wafer count!"; break;
     } 
  }	
  return $errmsg;
}

function decrement_study_wafer_count($study_id, $value)
{
  $query = "UPDATE epi_study SET `wafer_count` = `wafer_count` - $value WHERE id = '$study_id' AND wafer_count > 0;";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update study wafer count!"; break;
     } 
  }	
  return $errmsg;
}

function increment_study_attachment_count($study_id)
{
  $query = "UPDATE epi_study SET `attachment_count` = `attachment_count` + 1 WHERE id = '$study_id'";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update study attachment count!"; break;
     } 
  }	
  return $errmsg;
}

function decrement_study_attachment_count($study_id)
{
  $query = "UPDATE epi_study SET `attachment_count` = `attachment_count` - 1 WHERE id = '$study_id' AND attachment_count > 0";
  $result = $this->local_insert($query);
  if ($result == NULL) {
     $errmsg = "<b style=\"color:red;\">ERROR</b>: ";
     switch ($errno) {
       default: $errmsg .= "Can't update study attachment count!"; break;
     } 
  }	
  return $errmsg;
}

function add_wafer_to_study($study_id, $wafer_name)
{
  $query = "SELECT wafer_id FROM epi_wafer WHERE wafer_name like '$wafer_name' LIMIT 1";
  $result = $this->local_query($query);
  if ($result == NULL ) {
    return 0;
  } 
  $wafer_id = $result[0]['wafer_id'];
 
  $query = "INSERT INTO epi_study_wafer (study_id, wafer_id) ".
           "VALUES ('$study_id', '$wafer_id')";

  $result = $this->local_insert($query);

  $errmsg = "";
  if ($result == NULL) {
     switch ($errno) {
       default: $errmsg = "Wafer already attached!"; break;
     } 
     $errmsg = "<p style=\"padding: 2px 5px; border: 1px solid red; color:red\"><span> $errmsg</span></p>";
  }	
  return $errmsg;
}

function add_wafers_to_study($study_id, $wafers)
{
  $query = "INSERT INTO epi_study_wafer (study_id, wafer_id) ";
  $query .= "VALUES ";
  foreach ($wafers as $i=>$wid) {
    $query .= "('$study_id', '$wid')";
    if ($i < (count($wafers)-1))
      $query .= ", ";
  } 

  $result = $this->local_insert($query);

  $errmsg = "";
  if ($result == NULL) {
     switch ($errno) {
       default: $errmsg = "Wafer already attached!"; break;
     } 
     $errmsg = "<p style=\"padding: 2px 5px; border: 1px solid red; color:red\"><span> $errmsg</span></p>";
  }	
  return $errmsg;
}

function get_device_mask_name($device_mask_id)
{
 	$query = "SELECT device_mask AS 'Device Mask' FROM epi_device_mask WHERE device_mask_id = $device_mask_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return '';
 	}
 	return $result[0]['Device Mask'];
}
 
function get_study_name($study_id)
{
 	$query = "SELECT name AS 'Study Name' FROM epi_study WHERE id = $study_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return '';
 	}
 	return $result[0]['Study Name'];
}
 
function get_process_lot_number_by_id($process_lot_id)
{
 	$query = "SELECT process_lot_number ".
					 "FROM epi_process_lot ".
					 "WHERE process_lot_id = $process_lot_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return '';
 	}
 	return $result[0]['process_lot_number'];
}
 
function get_process_name_by_id($process_id) {
 	$query = "select process_name from epi_process where process_id = $process_id";
 	$result = $this->local_query($query);
 	if ($result == NULL ) {
 		return '';
 	}
 	return $result[0]['process_name'];
}

function get_study_wafers_lot($study_id)
{
	$query = "SELECT wafer_id FROM epi_study_wafer WHERE study_id = $study_id";
	
	$result = $this->local_query($query);
	if ($result == NULL) {
		return array();
	}
	return $result;
}

function get_process_lot_wafers($process_lot_id)
{
	$query = "SELECT wafer_id FROM epi_process_lot_wafer WHERE process_lot_id = '$process_lot_id'";
	
	$result = $this->local_query($query);
	if ($result == NULL) {
		return array();
	}
	return $result;
}

function get_process_lot_wafers_names($process_lot_id)
{
	$query = "SELECT wafer_id, wafer_name ".
           "FROM epi_process_lot_wafer ".
           "WHERE process_lot_id = '$process_lot_id'";

	$result = $this->local_query($query);
	if ($result == NULL) {
		return array();
	}
	return $result;
}


function get_lot_wafers($process_lot_id)
{
	$query = "SELECT w.wafer_id as wafer_id, w.wafer_name as wafer_name, w.device_mask_id as device_mask_id, m.device_mask as device_mask ".
           "FROM epi_process_lot_wafer p, epi_wafer w, epi_device_mask m ".
           "WHERE p.wafer_id = w.wafer_id AND m.device_mask_id = w.device_mask_id AND p.process_lot_id = '$process_lot_id'";

	$result = $this->local_query($query);
	if ($result == NULL) {
		return array();
	}
	return $result;
}

function set_process_arc_design_name($arc_design_id,$arc_design_name)
{
	$query = "update epi_process_arc_design set arc_design_name = '$arc_design_name' where arc_design_id = '$arc_design_id'";
	$result = $this->local_insert($query);
	return $result;
}

function set_process_arc_design_description($arc_design_id,$arc_design_description)
{
	$query = "update epi_process_arc_design set arc_design_description = '$arc_design_description' where arc_design_id = $arc_design_id ";
	$result = $this->local_insert($query);
	return $result;
}

function set_process_lot_process_name($process_id,$process_name) {
	$query = "update epi_process_lot set process_name = '$process_name' where process_id = $process_id ";
	//echo "<br>$query";
	$result = $this->local_insert($query);
	
	$query = "update epi_process_lot_wafer set process_name = '$process_name' where process_lot_id in (select process_lot_id from epi_process_lot where process_id = $process_id)  ";
	//echo "<br> $query";
	$result = $this->local_insert($query);
	
	return $result;
}

function set_process_lot_number($process_lot_id,$process_lot_number) {

	$query = "update epi_process_lot_wafer set process_lot_number = '$process_lot_number' where process_lot_id = $process_lot_id ";

	$result = $this->local_insert($query);
	return $result;

}

function get_study_wafers_count($study_id)
{
	$query = "SELECT count(*) AS 'count' ".
           "FROM epi_study_wafer ".
           "WHERE study_id = '$study_id' ";

	$result = $this->local_query($query);
	if ($result == NULL ) {
		return -1;
	}
	return $result[0]['count'];
}

function get_device_mask_attachment_count($device_mask_id)
{
	$query = "SELECT count(*) AS 'count' ".
           "FROM epi_device_mask_attachment ".
           "WHERE epi_device_mask_id = '$device_mask_id' ";

	$result = $this->local_query($query);
	if ($result == NULL ) {
		return -1;
	}
	return $result[0]['count'];
}

function get_device_mask_attachments($device_mask_id)
{
	$query = "SELECT * ".
           "FROM epi_device_mask_attachment ".
           "WHERE epi_device_mask_id = '$device_mask_id' ".
           "ORDER BY id ASC";
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return array();
	}
	return $result;
}

function get_study_attachment_count($study_id)
{
	$query = "SELECT count(*) AS 'count' ".
           "FROM epi_study_attachment ".
           "WHERE epi_study_id = '$study_id' ";

	$result = $this->local_query($query);
	if ($result == NULL ) {
		return -1;
	}
	return $result[0]['count'];
}

function get_study_attachments($study_id)
{
	$query = "SELECT * ".
           "FROM epi_study_attachment ".
           "WHERE epi_study_id = '$study_id' ".
           "ORDER BY id ASC";
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return array();
	}
	return $result;
}

function get_study_wafers($study_id)
{
	$query = "SELECT s.wafer_id, w.wafer_name ".
           "FROM epi_study_wafer s INNER JOIN epi_wafer w ON s.wafer_id = w.wafer_id ".
           "WHERE s.study_id = '$study_id' ".
           "ORDER BY w.wafer_name ASC";
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return array();
	}
	return $result;
}

function get_studies()
{
	$query = "SELECT id, name FROM epi_study ORDER BY id DESC";
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return array();
	}
	return $result;
}

function get_wafer_studies($wafer_id)
{
	$query = "SELECT s.id AS study_id, s.name AS study_name ".
		 "FROM epi_study_wafer sw, epi_study s ".
		 "WHERE sw.wafer_id = $wafer_id AND sw.study_id = s.id ".
		 "ORDER BY s.id DESC";
	$result = $this->local_query($query);
	return $result;
}

function get_study_selection()
{
	$studies = $this->get_studies();

	if(count($studies) == 0) {
		return 'No study found. Please add these first';
	}
	$html = '<select name="sid" id="sid">';
	foreach ($studies as $s) {
		$html.='<option value="'.$s['id'].'">'.$s['name'].'</option>';		
	}
	$html.='</select>';
	return $html;
}


function get_process_lots()
{
	$query = "SELECT process_id, process_name, process_lot_number, process_lot_id ".
           "FROM epi_process_lot ".
           "ORDER BY process_lot_id DESC";
	$result = $this->local_query($query);
	if ($result == NULL ) {
		return array();
	}
	return $result;
}

function get_process_lot_selection()
{
	$process_lots = $this->get_process_lots();

	if(count($process_lots) == 0) {
		return 'No process lots found. Please add these first';
	}
	$html = '<select name="process_lot_id" id="process_lot_id">';
	foreach ($process_lots as $p) {
		$html.='<option value="'.$p['process_lot_id'].'">'.$p['process_name'].' lot# '.$p['process_lot_number'].'</option>';		
	}
	$html.='</select>';
	return $html;
}


//========== FSM Functions ========//



function fsm_get_header(&$handle,$data) {
	
	$header = fgetcsv($handle,100000,chr(9));
	$tmp = array();
	foreach($header as $h) {
		$tmp[] = str_replace(' ','_',trim(str_replace('/','_',str_replace('#','',str_replace('(','',str_replace(')','',str_replace('*','',str_replace('[','_',str_replace(']','_',strtolower(trim($h)))))))))));
	
	}
	return $tmp;
}

function check_fsm($file,$data) {
	$query = "select * from epi_fsm where 
				wafer_id = ".$data['sample_id']." and
				wafer_name = '".$data['sample']."' and
				user      = '".$data['user']."'";

	//echo "<br> $query";

	$result = $this->local_query($query);
	if($result != NULL ) {
		return $result[0]['fsm_id'];
	}
	return 0;
}


function new_fsm($file, $data)
{
	// echo '<pre>';
	$query  = "INSERT INTO epi_fsm";
	$query .= " SET";
	$query .= " wafer_id = ".$data['sample_id'].",";
	$query .= " wafer_name = '".$data['sample']."',";
	$query .= " annealed = '".$data['annealed']."',";
	$query .= " user = '".$data['user']."',";
	$query .= " txt_file = '".$file."'";
			
	// echo "<br> $query";
	// echo '</pre>';
	$result = $this->local_insert($query);
	if($result == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	$fsm_id = $this->last_insert();
	
	if($fsm_id == NULL ) {
		return array('error'=>2,'point'=>$query);
	}
	
	return array('error'=>0,'data_id'=>$fsm_id);
	

}


function new_fsm_meas($fsm_id,$header,$string) {
	
	
	$tmp = explode('.',$string[0]);
	
	$q[] = " fsm_id = $fsm_id";
	$q[] = " wafer_name = '".trim($tmp[0])."'";
	$q[] = " scan_number = '".trim($tmp[1])."'";
	$q[] = " scan_angle  = '".trim($tmp[2])."'";
	
	$query = "select fsm_meas_id from epi_fsm_meas where ";
	
	$query .= implode(' and ',$q);
	
	$result = $this->local_query($query);
	
	$fsm_meas_id = 0;
	
	if ($result != NULL && $result[0]['fsm_meas_id'] != NULL ) {
		$fsm_meas_id = $result[0]['fsm_meas_id'];
	}

	if ($fsm_meas_id ) {
		$query = "update epi_fsm_meas set";
		$where = " where fsm_meas_id = $fsm_meas_id ";
	} else { 

		$query = "insert into epi_fsm_meas set ";
		$where = " ";
	}
	
	for($i=1;$i<count($header);$i++){
		if ($header[$i] != NULL && $header[$i] != 'wafer_id' ){
			$q[] = $header[$i]." = '".trim(addslashes($string[$i]))."'";
		}
	}

	$query.= implode(',',$q);
	$query.= $where;
	
	//echo "<br>".$query;

	if ($fsm_meas_id == 0 ) {
	
		$result = $this->local_insert($query);
		if($result == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	
		$fsm_meas_id = $this->last_insert();
	
		if($fsm_meas_id == NULL ) {
			return array('error'=>2,'point'=>$query);
		}
	}
	
	return array('error'=>0,'data_id'=>$fsm_meas_id);

}

function fsm_update_data($fsm_id) {
	// first check how many scans there are:
	
	$query = "update epi_fsm set date_time= (select date_time from epi_fsm_meas where fsm_id = $fsm_id order by fsm_meas_id desc limit 1) where fsm_id =$fsm_id";

	$result = $this->local_insert($query);
	//$query = "select scan_number, min(height_um_) as height_min, max(height_um_) as height_max, avg(abs(height_um_)) as height_abs_avg, stddev_pop(abs(height_um_)) as height_abs_std ,avg(height_um_) as height_avg, stddev_pop(height_um_)  as height_std from epi_fsm_meas where fsm_id = $fsm_id group by scan_number";
	
	$query = "select scan_number, 
						min(height_um_) as height_min, 
						max(height_um_) as height_max, 
						avg(height_um_) as height_avg, 
						stddev_pop(height_um_)  as height_std 
					from epi_fsm_meas 
						where fsm_id = $fsm_id 
					group by scan_number";
	//echo "<br> $query";

	$result = $this->local_query($query);
	if ($result != NULL ) {
		foreach ($result as $row) {
			$scan_number = array_shift($row);
			$scan_numbers[] = $scan_number;
			if ($scan_number > 0 && $scan_number < 3) {
				$query2 = "update epi_fsm set ";
					foreach($row as $k=>$v) {
						$q[] = 'scan'.$scan_number.'_'.$k.'='.sprintf('%.3f',$v);
					}
				$query2.= implode(',',$q)." where fsm_id= $fsm_id";
				//echo "<br> $query2 <br>";
				$result2 = $this->local_insert($query2);
			}
		}
	
	}
	$query = "update epi_fsm set scan_number = ".count($scan_numbers)." where fsm_id = $fsm_id";
	
	//echo "<br> $query";
	$result = $this->local_insert($query);
	
	if(count($scan_numbers) == 2) {

		
		$query = "select s2.height_um_-s1.height_um_ as height_diff,s2.stress-s1.stress as stress_diff  from epi_fsm_meas s1, epi_fsm_meas s2 where s1.scan_number=1 and s2.scan_number =2 and s1.scan_angle=s2.scan_angle and s1.fsm_id = $fsm_id and s2.fsm_id = $fsm_id";
		
		$result = $this->local_query($query);
		/*
		if ($result == NULL) {
			break;
		}
		*/
		$diff = array();
		
		foreach ($result as $r){
			$height_diff[]=$r['height_diff'];;
			$stress_diff[]=$r['stress_diff'];;
		}

		$diff = $height_diff;
		$diff_min = min($diff);
		$diff_max = max($diff);
		$diff_avg = $this->average($diff);
		$diff_std = $this->deviation($diff);
		
		$query = "update epi_fsm set 
					height_difference_min = $diff_min,
					height_difference_max = $diff_max,
					height_difference_avg = $diff_avg,
					height_difference_std = $diff_std
				where fsm_id= $fsm_id";
		
		$result = $this->local_insert($query);
		
		$diff = $stress_diff;
		$diff_min = min($diff);
		$diff_max = max($diff);
		$diff_avg = $this->average($diff);
		$diff_std = $this->deviation($diff);
		
		$query = "update epi_fsm set 
					stress_difference_min = $diff_min,
					stress_difference_max = $diff_max,
					stress_difference_avg = $diff_avg,
					stress_difference_std = $diff_std
				where fsm_id= $fsm_id";
		//echo "<br> $query";
		$result = $this->local_insert($query);
	
	}
  }


  function parse_fsm($file,$data) {
	/* 
		should not need this as this is handled by the main 
		uploading script - duplicate file names do not upload 
	*/
	
	if (!$fsm_id) {
		$result = $this->new_fsm($file, $data);
	
	
		if ($result['error'] > 0 ) {
			return $result;
		}
		
		$fsm_id = $result['data_id'];
		
		if (!$fsm_id) {
			return $result;
		}
		
	}
	
	$handle = $this->open_file($file,'r');

	ini_set('auto_detect_line_endings',TRUE);
	
	if (!$handle){
		return array('error'=>3,'point'=>'File Read Error. Could not open '.$file);
	}

	$header = $this->fsm_get_header($handle,$data);

	while(!feof($handle)) {
		$string = fgetcsv($handle,100000,chr(9));
		if (strlen(trim(implode(' ',$string))) > 0 ) {
			$result = $this->new_fsm_meas($fsm_id,$header,$string);
		}
		//print_r($string);
		
		if ($result['error'] > 0 ) {
			return $result;
		}
	
	}
	
	$this->fsm_update_data($fsm_id);
	
	return  array('error'=>0,'data_id'=>$fsm_id);
	

  }

  function average($array){
    $sum   = array_sum($array);
    $count = count($array);
    return $sum/$count;
  }

  function deviation ($array){
 
    $avg = $this->average($array);
   
    
    foreach ($array as $value) {
        $variance[] = pow($value-$avg, 2);
    }
    $deviation = sqrt($this->average($variance));
    return $deviation;
  }


  /* admin functions */
  function get_all_tables() {
	$query = "show tables from epi";
	//echo $query;
	$result = $this->local_query($query);
	$ret = array();
	foreach ($result as $res ) {
		$ret[] = $res['Tables_in_epi'];
	
	}

	//print_r($ret);

	return $ret;
  }

  function get_admin_tables() {
	$query = "show tables from epi LIKE '%_admin_%'";
	//echo $query;
	$result = $this->local_query($query);
	$ret = array();
	foreach ($result as $res ) {
		$ret[] = $res['Tables_in_epi'];
	
	}

	//print_r($ret);

	return $ret;
  }


  function get_non_admin_tables() {
	$all = $this->get_all_tables();
	$admin = $this->get_admin_tables();
	return array_diff($all,$admin);

  }

  /* admin functions */

}

// end of utilities class . 
?>
