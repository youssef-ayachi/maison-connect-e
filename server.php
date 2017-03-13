if ($select != "") {
$file = fopen('arduino.txt', 'w+');
$buffer = fgets($file);
fwrite($file, $select);
fclose($file);
}
$myfile = fopen("arduino.txt", "r") or die("Unable to open file!");
if (filesize("arduino.txt")>0 ){
$terme = fread($myfile,filesize("arduino.txt"));
}
fclose($myfile);
$link =='';
echo "Envoyé";
?>
