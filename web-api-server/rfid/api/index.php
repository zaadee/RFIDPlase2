<?php
 

if (isset($_GET['action'])) {
    $action = $_GET['action'];
    if (($action == "post_in_out") && ($_SERVER['REQUEST_METHOD'] == 'POST')) {
        postInOutHistory();
    } else if (($action == "is_new_data")) {
        isNewData();
    } else {
        echoBadRequest("Action is invalid");
    }
} else {
    echoBadRequest("No action parameter");
}

function isNewData(){
    include('../config/database-config.php');
    $sql = "SELECT * FROM is_new;";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        if($row['is_new_data'] == true){
            setNewData(false);
            //echoResult(true, 200, "OK");
            echo(true);
            return;
        }
        echo(false);
        //echoResult(false, 200, "OK");
        return;
    }
    echoBadRequest("BadRequest");
}

function setNewData($is_new ){
    include('../config/database-config.php');
    $sql = "UPDATE is_new SET is_new_data ='" .$is_new ."' WHERE 1 ;";
    $result = $conn->query($sql);
    
    if(!$result){
        echo false;
        return;
    }
    echo true;
}
function postInOutHistory()
{
    include('../config/database-config.php');

    if (isset($_POST['car']) && isset($_POST['driver'])) {
            //get paramiter from post.
        $car = $_POST['car'];
        $driver = $_POST['driver'];
        $date = new DateTime();
        $time = $date->getTimestamp();

        if ((strlen($car) != 6) || (strlen($driver) != 6) || ($car[0] != "C") || ($driver[0] != "D")) {
            echoBadRequest("Data is invalid");
            return;
        }

        $sql = "SELECT id FROM car WHERE id = '".$car ."';";
        $result = $conn->query($sql);
        if ($result->num_rows == 0) {
            echoBadRequest("No car Id");
            return;
        }

        $sql = "SELECT id FROM driver WHERE id = '".$driver ."';";
        $result = $conn->query($sql);
        if ($result->num_rows == 0) {
            echoBadRequest("No driver Id");
            return;
        }

        //Default status is 1: (1 car alive in university, 2 car out university)
        $status = 1;
        //Default result code 200 is car come in university, 201 car out University.
        $resultCode = 200; 
           
        //Query status of car 
        $sql = "SELECT * FROM `history` WHERE (car = '" . $car . "') ORDER BY time DESC LIMIT 1;";
        $result = $conn->query($sql);

        if ($result->num_rows > 0) {
            $row = $result->fetch_assoc();
            $status = $row['status'];
        }

        if ($status == 1) {
            $status = 2; //set car out 
            $resultCode = 201;
        } else {
            $status = 1; //Set car in 
            $resultCode = 200;
        }
            
            //Insert history 
        $sql = "INSERT INTO `history`(`time`, `car`, `driver`, `status`) VALUES 
            ('" . $time . "','" . $car . "','" . $driver . "'," . $status . ");";

        if ($conn->query($sql) === true) {
            setNewData(true);
            echoResult("Success", $resultCode, "OK");
        } else {
            echoResult("Error", 500, "Internal Server Error");
        }
    } else {
        echoBadRequest("Data is invalid");
    }
}

function echoBadRequest($error_message)
{
    echoResult($error_message, 400, "BAD_REQUEST");
}
function echoResult($message, $results, $status)
{
    $error = array('message' => $message, 'results' => $results, 'status' => $status);
    echo json_encode($error, JSON_UNESCAPED_UNICODE);
}
?>