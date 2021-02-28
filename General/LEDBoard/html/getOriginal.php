<?php

function createJSON()
{	$path = 'images/original/*';
    $first = 1;
    echo "[\n";
	echo "\"\"";
    foreach(glob($path) as $file)
    {	echo ",\n";	
		$parts = explode("/",$file);
		echo "\"" . $file . "\"";
	}
	echo "\n]\n";
}

createJSON();
	
?>