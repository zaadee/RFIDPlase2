<?php
    include('database-config.php');
    $firstname;
    $lastname;
    $telContract;
    $position;

    if (!isset($_GET['firstname']) && !isset($_GET['lastname']) && !isset($_GET['telContract']) && !isset($_GET['position'])) {
        echo false; 
        return;
    }

    $firstname = $_GET['firstname'];
    $lastname = $_GET['lastname']; 
    $telContract = $_GET['telContract']; 
    $position = $_GET['position']; 


    $driverIdLast = "D00001"; //Default driver id.
    $sql = "SELECT  * FROM driver ORDER BY driver.id DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $driverIdLast = $row['id'];
    }

    $id = substr($driverIdLast,1,5);
    $id+=1;
    $len = strlen($id);
    if($len > 5){
        echo false;
        return;
    }
    $driverId = "D";
    while((6 - strlen($driverId)) > $len){
        $driverId = $driverId."0";
    }
    $driverId = $driverId.$id ;
    
    $firstname = $_GET['firstname'];
    $lastname = $_GET['lastname']; 
    $telContract = $_GET['telContract']; 
    $position = $_GET['position']; 

    echo ("New Driver id: ".$driverId);
    $sql = "INSERT INTO driver (`id`, `first_name`, `last_name`, `tel_contract`, `position`) VALUES ('".$driverId ."', '" .$firstname ."', '" .$lastname ."', '" .$telContract  ."', '" .$position ."');";
    $result = $conn->query($sql);
    if(!$result){
        echo false;
        return;
    }
    echo true;
?>