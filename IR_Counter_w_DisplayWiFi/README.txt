If you have followed the directions in the MPU9250_to_MySQLdatabase folder and successfully setup a LAMP
stack and database, simply place the these .php files in the /var/www/html/ folder of the Raspberry Pi.
Then create a new database named countdb and a new table named SensorCount. Use this SQL code to create
the table.

CREATE TABLE SensorData (
    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    sensor VARCHAR(30) NOT NULL,
    location VARCHAR(30) NOT NULL,
    count VARCHAR(10),
    reading_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
)


The data can be viewed at "raspberry-pi-ip-address/countdb.php" for example "192.168.10.201/countdb.php".

The logic needs to be converted from 5v on the IR sensor to 3.3v for the ESP32 you can use a level shifter to do that.
Source: Method #2 https://randomnerdtutorials.com/how-to-level-shift-5v-to-3-3v/