<?php

$dir = '/mnt/sda1/www/DWAKU2';
$davDir = '/mnt/sda1/www/DWAKU2_appdata';
$upDir = '/mnt/sda1/www/DWAKU2_up';
$langDir = '/mnt/sda1/www/DWAKU2_lang';

function file_force_download($file) {
  if (file_exists($file)) {
    // сбрасываем буфер вывода PHP, чтобы избежать переполнения памяти выделенной под скрипт
    // если этого не сделать файл будет читаться в память полностью!
    if (ob_get_level()) {
      ob_end_clean();
    }
    // заставляем браузер показать окно сохранения файла
    header('Content-Description: File Transfer');
    header('Content-Type: application/octet-stream');
    header('Content-Disposition: attachment; filename=' . basename($file));
    header('Content-Transfer-Encoding: binary');
    header('Expires: 0');
    header('Cache-Control: must-revalidate');
    header('Pragma: public');
    header('Content-Length: ' . filesize($file));
    // читаем файл и отправляем его пользователю
    readfile($file);
  }
}

// GET запросы
if (!empty($_GET)) {

	// Проверка доступности сервера
	if(isset($_GET['ping'])) {	
		echo 'pong';
		exit;
	} else

	// Получить версию для обновления
	if(isset($_GET['get_latest_version'])) {
		$files = array();
		foreach(glob($dir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'exe')
				$files[] = basename(pathinfo($file, PATHINFO_FILENAME));	
			}
		} 
		echo array_pop($files);
		exit;
	}  else

	// Загрузка новой версии
	if(isset($_GET['download_latest'])) {
		$files = array();
		foreach(glob($dir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'exe')
					$files[] = $file;					
			}
		} 
		file_force_download(array_pop($files));
		exit;
	} else	
		
	// Загрузка апдейтера
	if(isset($_GET['download_updater'])) {
		$files = array();
		foreach(glob($upDir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'exe')
					$files[] = $file;					
			}
		} 
		file_force_download(array_pop($files));
		exit;
	} else	
		
	// Получение хэша файла обновления
	if(isset($_GET['check_updater'])) {
		$files = array();
		foreach(glob($upDir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'exe')
					$files[] = $file;					
			}
		} 
		echo hash_file("sha256", array_pop($files));
		exit;
	} else	

	// Получение хэша файла обновления
	if(isset($_GET['check_latest'])) {
		$files = array();
		foreach(glob($dir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'exe')
					$files[] = $file;					
			}
		} 
		echo hash_file("sha256", array_pop($files));
		exit;
	} else	
		
	// Получение языкового файла
	if(isset($_GET['get_lang'])) {
		$file = $langDir . '/' . $_GET['get_lang'] . '.json';
		if (is_file($file)) {
			file_force_download($file);					
		} else echo  'Not found';
		exit;
	} else	
		
	// Получение списка переводов
	if(isset($_GET['get_langList'])) {
		$files = array();
		foreach(glob($langDir . '/*') as $file) {	
			if (is_file($file)) {
				if (pathinfo($file, PATHINFO_EXTENSION) == 'json')
					if (pathinfo($file, PATHINFO_FILENAME) != 'list')
						echo pathinfo($file, PATHINFO_FILENAME) . PHP_EOL;					
			}
		}
		
		exit;
	} else
		
	// Получение хэша файла перевода
	if(isset($_GET['get_langHash'])) {
		$json = json_decode(file_get_contents($langDir ."/list.json"), true);
		echo $json[$_GET['get_langHash']];
		exit;
	} else

	// Получение списка json программ
	if(isset($_GET['get_list'])) {
		$jsonFile = $davDir . '/list.json';
		if (is_file($jsonFile)) {
			echo file_get_contents($jsonFile);
		}
		exit;
	} 	
}


	

?>