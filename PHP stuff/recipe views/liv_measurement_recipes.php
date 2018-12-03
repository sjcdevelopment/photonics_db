<?php
	require('config/db.php');
	require('config/config.php');

	//get id

	$query = 'SELECT * from laser_liv_measurement_recipe';

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
			<th>Start Current (mA)</th>
			<th>End Current (mA)</th>
			<th>Step Size</th>
			<th>Duty Cycle</th>
			<th>Pulse Width</th>
			<th>Comments</th>
		</tr>
	<?php foreach($recipes as $recipe) :?>
		<tr class = "table-secondary">
			<th><?php echo $recipe['liv_measurement_recipe_id']; ?></th>
			<th><?php echo $recipe['start_current_ma']; ?></th>
			<th><?php echo $recipe['end_current_ma']; ?></th>
			<th><?php echo $recipe['step_size']; ?></th>
			<th><?php echo $recipe['duty_cycle']; ?></th>
			<th><?php echo $recipe['pulse_width_us']; ?></th>
			<th><?php echo $recipe['comments']; ?></th>
		</tr>
	<?php endforeach; ?>
	</table>
	</div>
<?php include('inc/footer.php') ?>