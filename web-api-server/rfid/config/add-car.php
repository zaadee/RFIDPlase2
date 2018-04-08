<?php
    include('database-config.php');
    $band;
    $model;
    $licensePlate;

    if (!isset($_GET['band']) && !isset($_GET['model']) && !isset($_GET['licensePlate'])) {
        echo false; 
        return;
    }

    $band = $_GET['band'];
    $model = $_GET['model']; 
    $licensePlate = $_GET['licensePlate']; 
    
    $carIdLast = "C00001"; //Default car id.
    $sql = "SELECT  * FROM car ORDER BY car.id DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $carIdLast = $row['id'];
    }

    //echo ("Last car id: ".$carIdLast ."\n");
    $id = substr($carIdLast,1,5);
    $id+=1;
    $len = strlen($id);
    if($len > 5){
        echo false;
        return;
    }
    $carId = "C";
    while((6 - strlen($carId)) > $len){
        $carId = $carId."0";
    }
    $carId = $carId.$id ;
    
    //echo ("New car id: ".$carId);
    $sql = "INSERT INTO car (`id`, `band`, `model`, `license_plate`) VALUES ('".$carId ."', '" .$band ."', '" .$model ."', '" .$licensePlate ."')";
    $result = $conn->query($sql);
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>