<?php
    include('database-config.php');
    
    $id = $_GET['driverId'];
    $fistName = $_GET['fistName'];
    $lastName = $_GET['lastName'];
    $telContract = $_GET['telContract'];
    $position = $_GET['position'];
    
    $sql = "UPDATE driver SET first_name ='" .$fistName ."', last_name = '" .$lastName ."', tel_contract = '" .$telContract ."', position = '" .$position   ."' WHERE id = '" .$id ."';";
    $result = $conn->query($sql);
    
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>