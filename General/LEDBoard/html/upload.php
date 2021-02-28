<?php
$uploadPath = 'images/';
$targetPath = $uploadPath . basename( $_FILES['image']['name']);
if(@move_uploaded_file($_FILES['image']['tmp_name'], $targetPath)){
	echo $targetPath;
}
else {
	echo "error";
}
?>