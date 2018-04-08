$( document ).ready(function() {
    var openWithDriverTap = document.getElementById("openWithDriverTap").value;
    //alert( "openWithDriverTap: "+ openWithDriverTap);
    if(openWithDriverTap == "true/"){
        $('#myTab a[href="#driver"]').tab('show')
        document.getElementById("openWithDriverTap").value = "false";
    }
});
$(document).on("click", ".editCar", function () {
    var carId = $(this).data('id');
    //alert(carId);
    document.getElementById("editCarModalLabel").innerHTML = ("แก้ไขข้อมูลรถ id: "+carId);
    $.ajax({
        url: ("../rfid/config/get-car-data.php?carId="+carId),
        method: "GET"
    }).then(function (response) {
        console.log(response);

        var json = JSON.parse(response);
        document.getElementById("carIdModel").value = carId;
        document.getElementById("band").value = json["band"];
        document.getElementById("model").value = json["model"];
        document.getElementById("licensePlate").value = json["license_plate"];
    });
});

$(document).on("click", ".editDriver", function () {
    var driverId = $(this).data('id');
    //alert(driverId);
    document.getElementById("editDriverModalLabel").innerHTML = ("แก้ไขข้อมูลผู้ขับ id: " +driverId);
    $.ajax({
        url: ("../rfid/config/get-driver-data.php?driverId="+driverId),
        method: "GET"
    }).then(function (response) {
        //console.log(response);
        var json = JSON.parse(response);
        document.getElementById("driverIdModel").value = driverId;
        document.getElementById("firstName").value = json["first_name"];
        document.getElementById("lastName").value = json["last_name"];
        document.getElementById("telContract").value = json["tel_contract"];
        document.getElementById("position").value = json["position"];
    });
});

$(document).on("click", ".btnSaveCar", function () {
    //alert(("DATA: "+document.getElementById("carIdModel").value));
    var carId = document.getElementById("carIdModel").value;
    var band = document.getElementById("band").value;
    var model = document.getElementById("model").value;
    var licensePlate = document.getElementById("licensePlate").value;

    $.ajax({
        url: ("../rfid/config/update-car-data.php?carId="+carId + "&band=" + band + "&model=" + model +"&licensePlate=" +licensePlate),
        method: "GET"
    }).then(function (response) {
        console.log(response);
        $('#editCarModal').modal('hide');
        if(response){
            //alert("True");
            //window.location.reload();
            window.location.href = '../rfid/setting.php';
            return;
        }
    });

});

$(document).on("click", ".btnSaveDriver", function () {
    var driverId = document.getElementById("driverIdModel").value;
    var fistName = document.getElementById("firstName").value;
    var lastName = document.getElementById("lastName").value;
    var telContract = document.getElementById("telContract").value;
    var position = document.getElementById("position").value;

    //alert("driverId: "+driverId);

    //&telContract=0897654321&position=driver
    $.ajax({
        url: ("../rfid/config/update-driver-data.php?driverId="+driverId + "&fistName=" + fistName + "&lastName=" + lastName +"&telContract=" +telContract +"&position=" +position),
        method: "GET"
    }).then(function (response) {
        console.log(response);
        $('#editDriverModal').modal('hide');
        if(response){
            //alert("True");
            window.location.href = '../rfid/setting.php?openWithDriverTap=true';
            return;
        }
        alert("False");
    
    });

});

$(document).on("click", ".deleteDriver", function () {
    var driverId = $(this).data('id');
    document.getElementById("deleteDriverModalLabel").innerHTML = ("คุณต้องการลบผู้ขับ id: " +driverId +" หรือไม่?");
    document.getElementById("deleteDriverIdModel").value = driverId;
});

$(document).on("click", ".btnDeleteDriver", function () {
    //alert("Delete: "+ document.getElementById("deleteDriverIdModel").value);
    var driverId = document.getElementById("deleteDriverIdModel").value;
    $.ajax({
        url: ("../rfid/config/delete-driver.php?driverId="+driverId),
        method: "GET"
    }).then(function (response) {
        //console.log(response);
        $('#deleteDriverModal').modal('hide');
        if(response){
            window.location.href = '../rfid/setting.php?openWithDriverTap=true';
            return;
        }
        alert("Delete Driver Error!");
    });
});

$(document).on("click", ".deleteCar", function () {
    var carId = $(this).data('id');
    document.getElementById("deleteCarModalLabel").innerHTML = ("คุณต้องการลบรถ id: " +carId +" หรือไม่?");
    document.getElementById("deleteCarIdModel").value = carId;
});

$(document).on("click", ".btnDeleteCar", function () {
    var carId = document.getElementById("deleteCarIdModel").value;
    $.ajax({
        url: ("../rfid/config/delete-car.php?carId="+carId),
        method: "GET"
    }).then(function (response) {
        //console.log(response);
        $('#deleteCarModal').modal('hide');
        if(response){
            //window.location.reload();
            window.location.href = '../rfid/setting.php';
            return;
        }
        alert("Delete Car Error!");
    });
});

$(document).on("click", ".btnAddCar", function () {
    var band = document.getElementById("addBand").value;
    var model = document.getElementById("addModel").value;
    var licensePlate = document.getElementById("addLicensePlate").value;

    //Clear data
    document.getElementById("addBand").value = "";
    document.getElementById("addModel").value = "";
    document.getElementById("addLicensePlate").value = "";

    if(band == "" || model =="" || licensePlate == ""){
        $('#addCarModal').modal('hide');
        alert("กรอกข้อมูลให้ครบด้วยนาจา");
        return;
    }

    $.ajax({
        url: ("../rfid/config/add-car.php?band=" +band +"&model=" +model +"&licensePlate=" +licensePlate),
        method: "GET"
    }).then(function (response) {
        //console.log(response);
        $('#addCarModal').modal('hide');
        if(response){
            window.location.href = '../rfid/setting.php';
            return;
        }
        alert("Add Car Error!");
    });

});  

$(document).on("click", ".btnAddDriver", function () {
    var firstname = document.getElementById("addFirstname").value;
    var lastname = document.getElementById("addLastname").value;
    var telContract = document.getElementById("addTelContract").value;
    var position = document.getElementById("addPosition").value;

    if(firstname == "" || lastname =="" || telContract == "" || position==""){
        //$('#addDriverModal').modal('hide');
        alert("กรอกข้อมูลให้ครบด้วยนาจา");
        return;
    }

    //Clear data
    document.getElementById("addFirstname").value = "";
    document.getElementById("addLastname").value = "";
    document.getElementById("addTelContract").value = "";
    document.getElementById("addPosition").value = "";
    $.ajax({
        url: ("../rfid/config/add-driver.php?firstname=" +firstname +"&lastname=" +lastname +"&telContract=" +telContract +"&position=" + position),
        method: "GET"
    }).then(function (response) {
        //console.log(response);
        $('#addDriverModal').modal('hide');
        if(response){
            window.location.href = '../rfid/setting.php?openWithDriverTap=true';
            return;
        }
        alert("Add Driver Error!");
    });

});  