<?php

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-send-email-notification/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Receiver Email Address (where to send email notification)
$email_address = "YourEmailAddress@example.com";

// Keep this API Key value to be compatible with the ESP code provided in the project page. If you change this value, the ESP sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";

$api_key = $value1 = $value2 = $value3 = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) {
        $value1 = test_input($_POST["value1"]);
        $value2 = test_input($_POST["value2"]);
        $value3 = test_input($_POST["value3"]);
        
        // Email message
        $email_msg = "Temperature: " . $value1 . "ºC\nHumidity: " . $value2 . "%\nPressure: " . $value3 . "hPa";
        
        // Use wordwrap() if lines are longer than 70 characters
        $email_msg = wordwrap($email_msg, 70);
        
        // Uncomment the next if statement to set a threshold 
        // ($value1 = temperature, $value2 = humidity, $value3 = pressure)
        /*if($value1 < 24.0){
            echo "Temperature below threshold, don't send email";
            exit;
        }*/
        
        // send email with mail(receiver email address, email subject, email message)
        mail($email_address, "[NEW] ESP BME280 Readings", $email_msg);
        
        echo "Email sent";
    }
    else {
        echo "Wrong API Key provided.";
    }

}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
