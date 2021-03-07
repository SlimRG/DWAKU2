<?php

set_time_limit(1800);

$davDir = '/mnt/sda1/www/';
$mainDir = 'DWAKU2_appdata/';
$langDir = 'DWAKU2_lang/';
$site = 'https://slimpage.ru/';

function create_progress() {
  // First create our basic CSS that will control
  // the look of this bar:
  echo "
<style>
#text {
  position: absolute;
  top: 100px;
  left: 50%;
  margin: 0px 0px 0px -150px;
  font-size: 18px;
  text-align: center;
  width: 300px;
}
  #barbox_a {
  position: absolute;
  top: 130px;
  left: 50%;
  margin: 0px 0px 0px -160px;
  width: 304px;
  height: 24px;
  background-color: black;
}
.per {
  position: absolute;
  top: 130px;
  font-size: 18px;
  left: 50%;
  margin: 1px 0px 0px 150px;
  background-color: #FFFFFF;
}

.bar {
  position: absolute;
  top: 132px;
  left: 50%;
  margin: 0px 0px 0px -158px;
  width: 0px;
  height: 20px;
  background-color: #0099FF;
}

.blank {
  background-color: white;
  width: 300px;
}
</style>
";

  // Now output the basic, initial, XHTML that
  // will be overwritten later:
  echo "
<div id=\"progress\" style=\"width:500px;border:1px solid #ccc;\"></div>
<div id=\"information\" style=\"width\"></div>
";

  // Ensure that this gets to the screen
  // immediately:
  flush();
}
 
function update_progress($percent) {
  
  $percent = $percent;
  $percent = round($percent, 2);
  
  echo '<script language="javascript">
  document.getElementById("progress").innerHTML="<div style=\"width:'.$percent.'%;background-color:#ddd;\">&nbsp;</div>";
  document.getElementById("information").innerHTML="'.$percent.' % processed.";
  </script>';
  
  // Now, again, force this to be
  // immediately displayed:
  flush();
}

function remotefileSize($url)
{
    //return byte
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_NOBODY, 1);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER,false);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 0);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
    curl_setopt($ch, CURLOPT_MAXREDIRS, 3);
    curl_exec($ch);
    $filesize = curl_getinfo($ch, CURLINFO_CONTENT_LENGTH_DOWNLOAD);
    curl_close($ch);
    if ($filesize) return $filesize;
}

function progress($resource,$download_size, $downloaded, $upload_size, $uploaded)
{
    if($download_size > 0)
         update_progress($downloaded / $download_size  * 100);
}

function get_product_version($file_name)
{
   $key = "P\x00r\x00o\x00d\x00u\x00c\x00t\x00V\x00e\x00r\x00s\x00i\x00o\x00n\x00\x00\x00";
   $fptr = fopen($file_name, "rb");
   $data = "";
   while (!feof($fptr))
   {
      $data .= fread($fptr, 65536);
      if (strpos($data, $key)!==FALSE)
         break;
      $data = substr($data, strlen($data)-strlen($key));
   }
   fclose($fptr);
   if (strpos($data, $key)===FALSE)
      return "0.0.0.0";
   $pos = strpos($data, $key)+strlen($key);
   $version = "";
   for ($i=$pos; $data[$i]!="\x00"; $i+=2)
      $version .= $data[$i];
  if ($version == "") $version = "0.0.0.0";
   return $version;
}


function formatSizeUnits($bytes) {
	if ($bytes >= 1073741824)
	{
		$bytes = number_format($bytes / 1073741824, 2) . ' GB';
	}
	elseif ($bytes >= 1048576)
	{
		$bytes = number_format($bytes / 1048576, 2) . ' MB';
	}
	elseif ($bytes >= 1024)
	{
		$bytes = number_format($bytes / 1024, 2) . ' KB';
	}
	elseif ($bytes > 1)
	{
		$bytes = $bytes . ' bytes';
	}
	elseif ($bytes == 1)
	{
		$bytes = $bytes . ' byte';
	}
	else
	{
		$bytes = '0 bytes';
	}

	return $bytes;
}

create_progress();

$json = json_decode(file_get_contents($davDir . $mainDir ."list.json"), true);

foreach ($json as &$tmp99) {
	$newSize = remotefileSize($tmp99['Link']);
	echo "-------------------------------------------<br>";
	echo "Title: " . $tmp99['Title'] . "<br>";
	$newSize = formatSizeUnits($newSize);
	echo "Size: " . $newSize . "<br>";
	if ($newSize != '0 bytes'){
		if ($newSize != $tmp99['Size']) {
			// Update file
			$fileName = $davDir . $mainDir .str_replace(' ', '_', $tmp99['Title']) . '.exe';
			$fp = fopen($fileName, 'w+');
			$ch = curl_init($tmp99['Link']);
			curl_setopt($ch, CURLOPT_FILE, $fp);
			curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
			curl_setopt($ch, CURLOPT_PROGRESSFUNCTION, 'progress');
			curl_setopt($ch, CURLOPT_NOPROGRESS, false);
			curl_exec($ch);
			$statusCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);
			curl_close($ch);
			fclose($fp);
			
			if($statusCode == 200){
				echo "Download: Complete!" . "<br>";
				// Get file size
				$tmp99['Size'] = $newSize;
			
				// Get file version
				flush();
				$prver = get_product_version($fileName);
				flush();
				echo "Version: " . $prver . "<br>";
				$tmp99['Version'] = $prver;
			
				// Get link
				$tmp99['AlterLink'] = $site . $mainDir . str_replace(' ', '_', $tmp99['Title']) . '.exe';
				echo "AlterLink: " . $tmp99['AlterLink'] . "<br>";
			
				// Get hash
				flush();
				$tmp99['ServerHash'] = hash_file("sha256", $fileName);
				flush();
				echo "Hash: " . $tmp99['ServerHash'] . "<br>";
				
			} else{
				echo "Download: ERROR(" . $statusCode . ")<br>";
			}		
					
		} else {
			echo "Download: Already updated!" . "<br>";
			echo "Version: " . $tmp99['Version'] . "<br>";
			echo "AlterLink: " . $tmp99['AlterLink'] . "<br>";
			echo "Hash: " . $tmp99['ServerHash'] . "<br>";
		}		
	}
	flush();
}
echo '<script language="javascript">document.getElementById("information").innerHTML="Process completed"</script>';

$json = json_encode($json, JSON_UNESCAPED_UNICODE);

echo "-------------------------------------------<br>";
if (file_put_contents($davDir . $mainDir . "list.json", $json))
    echo "JSON file created successfully...<br>";
else 
    echo "Oops! Error creating json file...<br>";

echo "-------------------------------------------<br>";
echo "TRANSLATIONS<br>";
echo "-------------------------------------------<br>";

$LangHashes = array();
foreach(glob($davDir . $langDir . '/*') as $file) {	
	if (is_file($file)) {
		if (pathinfo($file, PATHINFO_FILENAME) != 'list') {
					$tmpHash = hash_file("sha256", $file);
					$LangHashes[pathinfo($file, PATHINFO_FILENAME)] = $tmpHash;	
					echo (pathinfo($file, PATHINFO_BASENAME)) . ": " . $tmpHash . "<br>";
		}
	}
} 
$json = json_encode($LangHashes, JSON_UNESCAPED_UNICODE);
echo "-------------------------------------------<br>";
if (file_put_contents($davDir . $langDir . "list.json", $json))
    echo "JSON file created successfully...<br>";
else 
    echo "Oops! Error creating json file...<br>";

echo "-------------------------------------------<br>";

?>