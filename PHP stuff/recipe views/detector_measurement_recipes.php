<?php
	require('config/db.php');
	require('config/config.php');

	//get id
	if( $_POST['only_common']){
		$query = 'SELECT * from detector_measurement_recipe WHERE common_recipe=1';
	}
	else{
		$query = 'SELECT * from detector_measurement_recipe';
	}
	

	//Get Result

	$result = mysqli_query($conn, $query);

	//Fetch Data
	$recipes = mysqli_fetch_all($result, MYSQLI_ASSOC);
	#var_dump($posts);
	//Free Result
	mysqli_free_result($result);

	//Close Connection
	mysqli_close($conn);

?>
<?php include('inc/header.php') ?>
<body>
	<div class ='containter'>
	<form action = "<?php $_PHP_SELF ?>" method="POST">
		Only Common Recipes: <input type = "checkbox" name = "only_common"/>
		<input type = "submit"/>
	</form>
	<div class ='containter'>
	<table style = "width:100%" class= "table table-hover">
		<tr class = "table-primary">
			<th>id</th>
			<th>Start Voltage (V)</th>
			<th>End Voltage (V))</th>
			<th>Data Points</th>
			<th>I Compliance</th>
			<th>Sweep Delay</th>
			<th>Sweep Direction</th>
			<th>Comments</th>
		</tr>
	<?php foreach($recipes as $recipe) :?>
		<tr class = "table-secondary">
			<th><?php echo $recipe['detector_measurement_recipe_id']; ?></th>
			<th><?php echo $recipe['start_voltage_v']; ?></th>
			<th><?php echo $recipe['end_voltage_v']; ?></th>
			<th><?php echo $recipe['number_data_points']; ?></th>
			<th><?php echo $recipe['I_compliance_ma']; ?></th>
			<th><?php echo $recipe['sweep_delay_s']; ?></th>
			<th><?php echo $recipe['direction']; ?></th>
			<th><?php echo $recipe['comments']; ?></th>
		</tr>
	<?php endforeach; ?>
	</table>
	</div>
<?php include('inc/footer.php') ?>