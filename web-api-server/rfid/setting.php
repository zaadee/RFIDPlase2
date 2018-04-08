<?php
    // session_start();
    // if (empty($_SESSION["admin"]) || (($_SESSION["time"] + 600) < time())) {//Session has life time 30 minuite.
    //     session_unset();     // unset $_SESSION variable for the run-time 
    //     session_destroy();   // destroy session data in storage
    //     header("location:index.php");
    //     exit();
    // } 

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
    <link rel="stylesheet" href="css/nav-color.css">
    <link rel="stylesheet" href="css/setting.css">
    <!-- <link rel="stylesheet" href="css/image-picker.css"> -->
    <title>Setting</title>
  </head>
  <body>
    <input type="hidden" id="openWithDriverTap" value=<?php echo ((isset($_GET['openWithDriverTap']))? "true" : "false");?>/>
    <header>
        <nav class="navbar navbar-expand navbar-dark flex-column flex-md-row bd-navbar bg-primary">
            <span class="navbar-text text-white pl-2 pr-4">
                การตั้งค่า
            </span>
            <ul class="nav nav-tabs nav-menu" id="myTab" role="tablist">
                <li class="nav-item">
                    <a class="nav-link active" id="car-tab" data-toggle="tab" href="#car" role="tab" aria-controls="car" aria-selected="true">รถ</a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" id="driver-tab" data-toggle="tab" href="#driver" role="tab" aria-controls="driver" aria-selected="false">คนขับ</a>
                </li>
            </ul>
            <ul class="navbar-nav flex-row ml-md-auto ">
                <div>
                    <button class="btn btn-outline-light my-2 my-sm-0 addCar" data-toggle="modal" data-target="#addCarModal">เพิ่มรถ</button>
                    <button class="btn btn-outline-light my-2 my-sm-0" data-toggle="modal" data-target="#addDriverModal">เพิ่มคนขับ</button>
                </div>
            </ul>
        </nav>
    </header>

    <div class="container">
        <!-- Content here -->
        <div class="tab-content" id="myTabContent">
            <div class="tab-pane fade show active" id="car" role="tabpanel" aria-labelledby="home-tab">
                <div class="container pt-5" >
                    <?php
                        $sql = "SELECT * FROM car";
                        $result = $conn->query($sql);
                        if($result->num_rows > 0){
                            while($row = $result->fetch_assoc()){?>
                                 <div class="card">
                                    <div class="container">
                                        <div class="row">
                                            <div class="col-xs-12 col-sm-6 col-md-6">
                                                <div class="well well-sm">
                                                    <div class="row">
                                                        <div class="col-sm-6 col-md-4 pt-3 pl-3">
                                                            <?php 
                                                                if($row['image'] == ''){
                                                                    echo ('<img class="img-fluid rounded" src="assets/car.png" id="car_image" class="img-responsive" width="100" height="100" />');
                                                                }else{
                                                                    echo('<img class="img-fluid rounded" src="assets/car/' .$row['image']  .'" id="car_image" class="img-responsive" width="100" height="100" />');
                                                                }
                                                            ?>
                                                        </div>
                                                        <div class="col-sm-6 col-md-8 pt-3 pb-3">
                                                            <h4><?php echo "ยี่ห้อ: ".$row['band']; ?></h4>
                                                            <h5><?php echo "รุ่น: ".$row['model']; ?></h5>
                                                            <h5><?php echo "เลขทะเบียน: ".$row['license_plate']; ?></h5>
                                                            <h5><?php echo "id: ".$row['id']; ?></h5> 
                                                            <div class="btn-group">
                                                                <button type="button" class="btn btn-warning editCar" data-toggle="modal" data-target="#editCarModal" data-id=<?php echo $row["id"];?>>Edit</button>
                                                                <button type="button" class="btn btn-danger deleteCar" data-toggle="modal" data-target="#deleteCarModal" data-id=<?php echo $row["id"];?>>Delete</button>
                                                            </div>
                                                        </div> 
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            <?php   
                            }
                        }else{
                            echo "<tr>";
                            echo "<td>ไม่มีข้อมุลรถ</td>";
                            echo "</tr>";  
                        }
                    ?> 
                </div>
            </div>
            <div class="tab-pane fade" id="driver" role="tabpanel" aria-labelledby="profile-tab">
                <div class="container pt-5" >
                    <?php
                        $sql = "SELECT * FROM driver";
                        $result = $conn->query($sql);
                        if($result->num_rows > 0){
                            while($row = $result->fetch_assoc()){?>
                                <div class="card">
                                    <div class="container">
                                        <div class="row">
                                            <div class="col-xs-12 col-sm-6 col-md-6">
                                                <div class="well well-sm">
                                                    <div class="row">
                                                        <div class="col-sm-6 col-md-4 p-3">
                                                            <?php 
                                                                if($row['image'] == ''){
                                                                    echo ('<img class="img-fluid rounded" src="assets/driver.png" id="car_image" class="img-responsive" width="100" height="100" />');
                                                                }else{
                                                                    echo('<img class="img-fluid rounded" src="assets/driver/' .$row['image']  .'" id="car_image" class="img-responsive" width="100" height="100" />');
                                                                }
                                                            ?>
                                                        </div>
                                                        <div class="col-sm-6 col-md-8 pt-3 pb-3">
                                                            <h4><?php echo "ชื่อ: ".$row['first_name'] ." " .$row['last_name']; ?></h4>
                                                            <h5><?php echo "ตำแหน่ง: ".$row['position']; ?></h5>
                                                            <h5><?php echo "โทรศัพท์: ".$row['tel_contract']; ?></h5>
                                                            <h5><?php echo "id: ".$row['id']; ?></h5> 
                                                            <div class="btn-group">
                                                                <button type="button" class="btn btn-warning editDriver" data-toggle="modal" data-target="#editDriverModal" data-id=<?php echo $row["id"];?>>Edit</button>
                                                                <button type="button" class="btn btn-danger deleteDriver" data-toggle="modal" data-target="#deleteDriverModal" data-id=<?php echo $row["id"];?>>Delete</button>
                                                            </div>
                                                        </div> 
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            <?php }
                        }else{
                            echo "<tr>";
                            echo "<td>ไม่มีข้อมุลรถ</td>";
                            echo "</tr>";  
                        }
                    ?> 
            </div>
        </div>
    </div>

    <!-- Modal -->
    <div class="modal fade" id="editCarModal" tabindex="-1" role="dialog" aria-labelledby="editCarModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <input type="hidden" id="carIdModel" />
                <div class="modal-header">
                    <h5 class="modal-title" id="editCarModalLabel"  name="editCarTitle">แก้ไขข้อมูลรถ</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="form-group">
		                <label for="band">Band</label>
	                    <input type="text" class="form-control" id="band" name="band" value="Band" />
		            </div>
		            <div class="form-group">
		                <label for="model">Model</label>
	                    <input type="text" class="form-control" id="model" name="model" value="Model" />
		            </div>
		            <div class="form-group">
		                <label for="licensePlate">License plate</label>
		                <input type="text" class="form-control" id="licensePlate" name="licensePlate" value="License plate" />
                    </div>
                    <!-- <div class="form-group"> 
                        <label >Image</label>
                        <div class="form-group col-sm-2"> 
                            <div class="img-picker btn-primary" width="100" height="100"></div>
                        </div> 
                    </div> -->
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-primary btnSaveCar">Save changes</button>
                </div>
            </div>
        </div>
    </div>
    <div class="modal fade" id="editDriverModal" tabindex="-1" role="dialog" aria-labelledby="editDriverModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <input type="hidden" id="driverIdModel" />
                <div class="modal-header">
                    <h5 class="modal-title" id="editDriverModalLabel"  name="editDriverTitle">แก้ไขข้อมูลผู้ขับ</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="form-group">
		                <label for="firstName">Fistname</label>
	                    <input type="text" class="form-control" id="firstName" name="firstName" value="Firstname" />
		            </div>
		            <div class="form-group">
		                <label for="lastName">Lastname</label>
	                    <input type="text" class="form-control" id="lastName" name="lastName" value="Lastname" />
		            </div>
		            <div class="form-group">
		                <label for="telContract">Tel Contract</label>
		                <input type="text" class="form-control" id="telContract" name="telContract" value="Tel Contract" />
                    </div>
                    <div class="form-group">
		                <label for="position">Position</label>
		                <input type="text" class="form-control" id="position" name="position" value="Position" />
                    </div>
                    <!-- <div class="form-group"> 
                        <label >Image</label>
                        <div class="form-group col-sm-2"> 
                            <div class="img-picker btn-primary" width="100" height="100"></div>
                        </div> 
                    </div> -->
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-primary btnSaveDriver">Save changes</button>
                </div>
            </div>
        </div>
    </div>

    <div class="modal fade" id="deleteCarModal" tabindex="-1" role="dialog" aria-labelledby="deleteCarModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <input type="hidden" id="deleteCarIdModel" />
                <div class="modal-header">
                    <h5 class="modal-title" id="deleteCarModalLabel"  name="deleteCarTitle">คุณต้องการลบผรถ หรือไม่?</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-danger btnDeleteCar">Delete</button>
                </div>
            </div>
        </div>
    </div>

    <div class="modal fade" id="deleteDriverModal" tabindex="-1" role="dialog" aria-labelledby="deleteDriverModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <input type="hidden" id="deleteDriverIdModel" />
                <div class="modal-header">
                    <h5 class="modal-title" id="deleteDriverModalLabel"  name="deleteDriverTitle">คุณต้องการลบผู้ขับ หรือไม่?</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-danger btnDeleteDriver">Delete</button>
                </div>
            </div>
        </div>
    </div>

    <div class="modal fade" id="addCarModal" tabindex="-1" role="dialog" aria-labelledby="addCarModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="addCarModalLabel"  name="addCarTitle">เพิ่มข้อมูลรถ</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="form-group">
		                <label for="band">Band</label>
	                    <input type="text" class="form-control" id="addBand" name="band" placeholder="Band" />
		            </div>
		            <div class="form-group">
		                <label for="model">Model</label>
	                    <input type="text" class="form-control" id="addModel" name="model" placeholder="Model" />
		            </div>
		            <div class="form-group">
		                <label for="licensePlate">License plate</label>
		                <input type="text" class="form-control" id="addLicensePlate" name="licensePlate" placeholder="License plate" />
                    </div>
                    <!-- <div class="form-group"> 
                        <label >Image</label>
                        <div class="form-group col-sm-2"> 
                            <div class="img-picker btn-primary" width="100" height="100"></div>
                        </div> 
                    </div> -->
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-primary btnAddCar">Add data</button>
                </div>
            </div>
        </div>
    </div>

    <div class="modal fade" id="addDriverModal" tabindex="-1" role="dialog" aria-labelledby="addDriverModalLabel" aria-hidden="true">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h5 class="modal-title" id="addDriverModalLabel"  name="addDriverTitle">เพิ่มข้อมูลคนขับ</h5>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                    <div class="form-group">
		                <label for="band">Firstname</label>
	                    <input type="text" class="form-control" id="addFirstname" name="firstname" placeholder="Firstname" />
		            </div>
		            <div class="form-group">
		                <label for="model">Lastname</label>
	                    <input type="text" class="form-control" id="addLastname" name="lastname" placeholder="Lastname" />
		            </div>
		            <div class="form-group">
		                <label for="licensePlate">Tel Contract</label>
		                <input type="text" class="form-control" id="addTelContract" name="telContract" placeholder="Tel Contract" />
                    </div>
                    <div class="form-group">
		                <label for="licensePlate">Position</label>
		                <input type="text" class="form-control" id="addPosition" name="position" placeholder="Position" />
                    </div>
                    <!-- <div class="form-group"> 
                        <label >Image</label>
                        <div class="form-group col-sm-2"> 
                            <div class="img-picker btn-primary" width="100" height="100"></div>
                        </div> 
                    </div> -->
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
                    <button type="button" class="btn btn-primary btnAddDriver">Add data</button>
                </div>
            </div>
        </div>
    </div>
          
    <!-- Optional JavaScript -->
    <!-- jQuery first, then Popper.js, then Bootstrap JS -->
    <script src="https://code.jquery.com/jquery-3.2.1.min.js"></script>
    <script src="js/popper.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
    <script src="js/setting.js"></script>
    <!-- <script src="js/image-picker.js"></script> -->
  </body>
</html>