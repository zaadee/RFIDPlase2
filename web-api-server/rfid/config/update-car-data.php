<?php
    include('database-config.php');
    
    $id = $_GET['carId'];
    $band = $_GET['band'];
    $model = $_GET['model'];
    $licensePlate = $_GET['licensePlate'];
    
    $sql = "UPDATE car SET band ='" .$band ."', model = '" .$model ."', license_plate = '" .$licensePlate ."' WHERE id = '" .$id ."';";
    $result = $conn->query($sql);
    
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>