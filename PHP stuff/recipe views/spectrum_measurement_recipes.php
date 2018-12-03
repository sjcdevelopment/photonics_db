<?php
	require('config/db.php');
	require('config/config.php');

	//get id

	$query = 'SELECT * from laser_spectrum_measurement_recipe';

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
	<table style = "width:100%" class= "table table-hover">
		<tr class = "table-primary">
			<th>id</th>
			<th>Start Wavelength (nm)</th>
			<th>End Wavelength (nm)</th>
			<th>Step Size</th>
			<th>Resolution Bandwidth</th>
			<th>Sensitivity</th>
			<th>Comments</th>
		</tr>
	<?php foreach($recipes as $recipe) :?>
		<tr class = "table-secondary">
			<th><?php echo $recipe['spectrum_measurement_recipe_id']; ?></th>
			<th><?php echo $recipe['start_wavelength_nm']; ?></th>
			<th><?php echo $recipe['end_wavelength_nm']; ?></th>
			<th><?php echo $recipe['step_size_nm']; ?></th>
			<th><?php echo $recipe['resolution_bandwidth_nm']; ?></th>
			<th><?php echo $recipe['sensitivity_dbm']; ?></th>
			<th><?php echo $recipe['comments']; ?></th>
		</tr>
	<?php endforeach; ?>
	</table>
	</div>
<?php include('inc/footer.php') ?>