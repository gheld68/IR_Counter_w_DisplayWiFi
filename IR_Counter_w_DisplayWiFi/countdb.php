<!DOCTYPE html>
<html>

<body>
  <?php
  // 서버 정보 설정
  $servername = "localhost";
  $dbname = "SensorData";
  $username = "tinywave";
  $password = "tinywave!";

  // 데이터베이스에 연결
  $conn = new mysqli($servername, $username, $password, $dbname);

  // 연결 상태 확인
  if ($conn->connect_error) {
    die("연결 실패: " . $conn->connect_error);
  }

  // SQL 쿼리문: SensorCount 테이블에서 모든 데이터를 ID 내림차순으로 선택
  $sql = "SELECT id, sensor, location, Count, reading_time FROM SensorCount ORDER BY id DESC";

  // HTML 테이블 헤더 출력
  echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <td>ID</td> 
        <td>센서</td> 
        <td>위치</td> 
        <td>카운트</td> 
        <td>타임스탬프</td> 
      </tr>';

  // SQL 쿼리 실행 후 결과 처리
  if ($result = $conn->query($sql)) {
    // 각 행에 대해
    while ($row = $result->fetch_assoc()) {
      $row_id = $row["id"]; // ID
      $row_sensor = $row["sensor"]; // 센서
      $row_location = $row["location"]; // 위치
      $row_count = $row["Count"]; // 카운트
      $row_reading_time = $row["reading_time"]; // 읽은 시간
  
      // 각 행을 HTML 테이블로 출력
      echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_sensor . '</td> 
                <td>' . $row_location . '</td> 
                <td>' . $row_count . '</td> 
                <td>' . $row_reading_time . '</td> 
              </tr>';
    }
    // 결과셋 메모리 해제
    $result->free();
  }

  // 데이터베이스 연결 종료
  $conn->close();

  ?>
  </table>
</body>

</html>