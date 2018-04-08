<?php
    include('config/database-config.php');

?>

<!doctype html>
<html lang="en">
  <head>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

    <!-- Bootstrap CSS -->
    <link rel="stylesheet" href="css/bootstrap.min.css">
    <link rel="stylesheet" href="css/signin.css">
    <link rel="stylesheet" href="css/history.css">

    <title>RFID</title>
  </head>
  <body>
    <header>
        <nav class="navbar navbar-expand navbar-dark flex-column flex-md-row bd-navbar bg-primary">
            <img calss="navbar-brand mr-0 mr-md-2" src="assets/rmuti-logo.png" width="36" height="36" class="d-inline-block align-top" />
            <span class="navbar-text text-white pl-2">
                การประยุกต์ใช้ RFID สำหรับการผ่านเข้า - ออก กรณีศึกษา มหาวิทยาลัยเทคโนโลยีราชมงคลอีสาน วิทยาเขตขอนแก่น
            </span>
            <ul class="navbar-nav flex-row ml-md-auto ">
                <button class="btn btn-outline-light my-2 my-sm-0" data-toggle="modal" data-target="#popUpWindow">Setting</button>
            </ul>
        </nav>
    </header>   
    <div class="container">
        <!-- Content here -->
        <h2 class="bd-title pt-5" id="content">ตารางเข้า - ออกรถ</h2>
        <table class="table">
            <thead>
                <tr>
                    <th>เวลา</th>
                    <th>สถานะ</th>
                    <th>รถ</th>
                    <th>ทะเบียน</th>
                    <th>ผู้ขับ</th>
                </tr>
            </thead>
            <tbody>
                <?php
                    //$sql = "SELECT * FROM history ORDER BY time DESC";
                    $sql = "SELECT history.time ,history.status, 
                                 car.band, car.model, car.license_plate, car.image as carimage,
                                 driver.first_name, driver.last_name ,driver.image as driverimage
                          FROM history 
                          INNER JOIN car ON history.car = car.id 
                          INNER JOIN driver ON history.driver = driver.id
                          ORDER BY history.time DESC";
                    
                    $result = $conn->query($sql);
                    if($result->num_rows > 0){
                        while($row = $result->fetch_assoc()){
                            ?>
                            <tr>
                                <td><?php echo date('d/m/Y H:i:s', $row['time']); ?></td>
                                <td><?php echo ($row['status'] == 1? "เข้า" : "ออก"); ?></td>
                                <td>
                                    <div class="image">
                                        <img class="object-fit_cover" src = <?php echo(($row['carimage']== '')? "assets/car.png" : ("assets/car/".$row['carimage'])) ?> id = "car_image"width="100" height="100"/>
                                    </div>
                                    <br/>
                                    <?php echo  ($row['band'] ." " .$row['model']); ?>
                                </td>
                                <td><?php echo $row['license_plate']; ?></td>
                                <td>
                                    <div class="image">
                                        <img class="object-fit_cover" src = <?php echo(($row['driverimage']== '')? "assets/driver.png" : ("assets/driver/".$row['carimage'])) ?> id = "car_image"width="100" height="100"/>
                                    </div>
                                    <br/>
                                    <?php echo $row['first_name']." " .$row['last_name']; ?>
                                </td>
                            </tr>
                            <?php
                        }
                    } else {
                        echo "<tr>";
                        echo "<td>ไม่มีข้อมุล</td>";
                        echo "</tr>";
                    }
                ?>
            </tbody>
        </table>
    </div>

     <div class="modal fade" id="popUpWindow">
        <div class="modal-dialog">
            <div class="modal-content">
                <form class="form-signin text-center" method="post" action="config/login.php">
                    <h1 class="h3 mb-3 font-weight-normal">Login to setting</h1>
                    <input type="text" class="form-control" id="username" name="username" placeholder="Username">
                    <input type="password" class="form-control" id ="password" name="password" placeholder="Password">
                    <!-- <button class="btn btn-lg btn-primary btn-block" type="submit">Login</button> -->
                    <input class="btn btn-lg btn-primary btn-block" type="submit" name="login" class="loginmodal-submit" value="Login" onclick="onLoginClick()">
                </form>
            </div>
        </div>
    </div> 
          

    <!-- Optional JavaScript -->
    <!-- jQuery first, then Popper.js, then Bootstrap JS -->
    <script src="js/jquery-3.2.1.slim.min.js"></script>
    <script src="js/popper.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
  </body>
</html>