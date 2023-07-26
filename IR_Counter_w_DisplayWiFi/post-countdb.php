<?php

/*
  Rui Santos가 작성
  프로젝트 상세 정보는 https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/ 에서 확인 가능
  
  본 소프트웨어와 관련된 문서의 복사본을 얻는 사람에게는 무료로 권한이 부여된다.
  
  위의 저작권 고지 및 이 허가 고지는 소프트웨어의 모든 복사본 또는 주요 부분에 포함되어야 한다.
*/

// 서버 이름
$servername = "localhost";

// 여기에 데이터베이스 이름을 입력하세요
$dbname = "rapberry";
// 여기에 데이터베이스 사용자 이름을 입력하세요
$username = "tinywave";
// 여기에 데이터베이스 사용자 비밀번호를 입력하세요
$password = "tinywave!";

// 이 API Key 값을 프로젝트 페이지에서 제공되는 ESP32 코드와 호환되게 유지하세요. 
// 이 값을 변경하는 경우, ESP32 스케치도 일치해야 합니다
$api_key_value = "tPmAT5Ab3j7F9";

$api_key = $sensor = $location = $count = "";

// HTTP POST 방식으로 데이터가 전송되었는지 확인
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // API 키 값 검증
    $api_key = test_input($_POST["api_key"]);
    if ($api_key == $api_key_value) {
        // 입력값 검증 후 변수 할당
        $sensor = test_input($_POST["sensor"]);
        $location = test_input($_POST["location"]);
        $count = test_input($_POST["count"]);

        // 데이터베이스 연결 생성
        $conn = new mysqli($servername, $username, $password, $dbname);
        // 연결 확인
        if ($conn->connect_error) {
            die("연결 실패: " . $conn->connect_error);
        }

        // 쿼리 생성
        $sql = "INSERT INTO SensorCount (sensor, location, count)
        VALUES ('" . $sensor . "', '" . $location . "', '" . $count . "')";

        // 쿼리 실행 결과 확인
        if ($conn->query($sql) === TRUE) {
            echo "새로운 레코드가 성공적으로 생성되었습니다";
        } else {
            echo "오류: " . $sql . "<br>" . $conn->error;
        }

        // 데이터베이스 연결 종료
        $conn->close();
    } else {
        echo "제공된 API 키가 잘못되었습니다.";
    }

} else {
    echo "HTTP POST로 데이터가 전송되지 않았습니다. 수정해주세요.";
}

// 입력 데이터를 처리하는 함수
function test_input($data)
{
    $data = trim($data); // 공백 제거
    $data = stripslashes($data); // 백슬래시 제거
    $data = htmlspecialchars($data); // 특수 문자를 HTML 엔티티로 변환
    return $data;
}