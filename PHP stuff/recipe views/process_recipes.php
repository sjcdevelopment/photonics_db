<?php
	require('config/db.php');
	require('config/config.php');

	//get id

	$query = 'SELECT * from post_process_recipe';

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
			<th>Device ID</th>
			<th>Recipe Type</th>
			<th>Recipe Link</th>
		</tr>
	<?php foreach($recipes as $recipe) :?>
		<tr class = "table-secondary">
			<th><?php echo $recipe['post_process_recipe_id']; ?></th>
			<th><?php echo $recipe['device_id']; ?></th>
			<th><?php echo $recipe['recipe_type']; ?></th>
			<th><?php echo $recipe['recipe_link']; ?></th>
		</tr>
	<?php endforeach; ?>
	</table>
	</div>
<?php include('inc/footer.php') ?>