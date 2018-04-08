<?php
    include('database-config.php'); 
    $sql = "SELECT  * FROM driver ORDER BY driver.id DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        echo ($row['id']);
        return;
    }
    echo("D00001");
?>