<?php

$imgName = $_GET['fileName'];
$imgDest = $_GET['imgDest'];
$newWidth = $_GET['width'];
$newHeight = $_GET['height'];
$saveAs = $pathParts[0] . '/' . $pathParts[3] . $newWidth . 'x' . $newHeight . '.' . $imgParts[1];
list($width,$height) = getimagesize($imgName);
$output = imagecreatetruecolor($newWidth,$newHeight);
$source = imagecreatefromjpeg($imgName);
imagecopyresized($output, $source, 0,0,0,0, $newWidth,$newHeight,$width,$height);
imagejpeg($output,$imgDest);
echo $saveAs;

?>